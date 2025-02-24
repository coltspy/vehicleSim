#include "Simulation.h"
#include "Simulation.h"
#include "FilePublisher.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

#ifdef USE_KAFKA
#include "KafkaPublisher.h"
#endif

// For JSON serialization
using json = nlohmann::json;

// Convert vehicle to JSON
json vehicleToJson(const Vehicle& vehicle) {
    json j;
    j["id"] = vehicle.getId();
    j["position"] = {
            {"lat", vehicle.getPosition().lat},
            {"lon", vehicle.getPosition().lon}
    };
    j["heading"] = vehicle.getHeading();
    j["speed"] = vehicle.getSpeed();
    return j;
}

// Vehicle update callback to print updates
void printVehicleUpdate(const Vehicle& vehicle) {
    json j = vehicleToJson(vehicle);
    std::cout << j.dump(2) << std::endl;
}

int main(int argc, char* argv[]) {
    // Default configuration
    std::string outputFile = "vehicle_positions.json";
    bool useFile = true;

#ifdef USE_KAFKA
    std::string kafkaBroker = "localhost:9092";
    std::string kafkaTopic = "vehicle-positions";
    bool useKafka = true;
#endif

    // Check command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--output-file" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--no-file") {
            useFile = false;
        }
#ifdef USE_KAFKA
        else if (arg == "--no-kafka") {
            useKafka = false;
        } else if (arg == "--broker" && i + 1 < argc) {
            kafkaBroker = argv[++i];
        } else if (arg == "--topic" && i + 1 < argc) {
            kafkaTopic = argv[++i];
        }
#endif
    }

    // Initialize publishers
    std::unique_ptr<FilePublisher> filePublisher;
    if (useFile) {
        std::cout << "Initializing file publisher to " << outputFile << std::endl;
        try {
            filePublisher = std::make_unique<FilePublisher>(outputFile);
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize file publisher: " << e.what() << std::endl;
            useFile = false;
        }
    }

#ifdef USE_KAFKA
    std::unique_ptr<KafkaPublisher> kafkaPublisher;
    if (useKafka) {
        std::cout << "Initializing Kafka publisher..." << std::endl;
        try {
            kafkaPublisher = std::make_unique<KafkaPublisher>(kafkaBroker, kafkaTopic);
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize Kafka: " << e.what() << std::endl;
            useKafka = false;
        }
    }
#endif

    // Create routes
    Route route1;
    route1.addWaypoint({37.7749, -122.4194}); // San Francisco
    route1.addWaypoint({37.7749, -122.4104}); // Moving east
    route1.addWaypoint({37.7839, -122.4104}); // Moving north
    route1.addWaypoint({37.7839, -122.4014}); // Moving east again

    // Create vehicles
    auto vehicle1 = std::make_shared<Vehicle>("vehicle1", GeoPoint{37.7749, -122.4194}, route1);
    vehicle1->setMaxSpeed(15.0); // Slower speed for testing

    // Create simulation
    Simulation sim(0.1); // 100ms time step
    sim.addVehicle(vehicle1);

    // Register callback for console output
    sim.registerVehicleUpdateCallback(printVehicleUpdate);

    // Register callbacks for publishers
    if (useFile) {
        sim.registerVehicleUpdateCallback([&filePublisher](const Vehicle& vehicle) {
            filePublisher->publishVehicleUpdate(vehicle);
        });
    }

#ifdef USE_KAFKA
    if (useKafka) {
        sim.registerVehicleUpdateCallback([&kafkaPublisher](const Vehicle& vehicle) {
            kafkaPublisher->publishVehicleUpdate(vehicle);
        });
    }
#endif

    // Start simulation
    sim.start();

    // Run for a while, printing updates
    std::cout << "Starting simulation..." << std::endl;

    // Run for 20 seconds of simulation time
    double simulationDuration = 20.0;

    while (sim.getSimulationTime() < simulationDuration) {
        // Update simulation by one time step
        sim.update();

        // Sleep to avoid maxing out CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Simulation completed." << std::endl;
    return 0;
}