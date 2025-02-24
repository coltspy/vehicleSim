#ifndef VEHICLE_SIM_SIMULATION_H
#define VEHICLE_SIM_SIMULATION_H

#include "Vehicle.h"
#include <vector>
#include <memory>
#include <chrono>
#include <functional>

// Callback type for vehicle updates
using VehicleUpdateCallback = std::function<void(const Vehicle&)>;

// Main simulation class
class Simulation {
public:
    // Constructor with simulation time step
    Simulation(double timeStep = 0.1) 
        : timeStep_(timeStep), 
          running_(false),
          simulationTime_(0.0) {}
    
    // Add a vehicle to the simulation
    void addVehicle(std::shared_ptr<Vehicle> vehicle) {
        vehicles_.push_back(vehicle);
    }
    
    // Register callback for vehicle updates
    void registerVehicleUpdateCallback(VehicleUpdateCallback callback) {
        vehicleUpdateCallbacks_.push_back(callback);
    }
    
    // Start simulation
    void start() {
        running_ = true;
    }
    
    // Stop simulation
    void stop() {
        running_ = false;
    }
    
    // Reset simulation
    void reset() {
        simulationTime_ = 0.0;
    }
    
    // Update simulation by one time step
    void update() {
        if (!running_) return;
        
        // Update all vehicles
        for (auto& vehicle : vehicles_) {
            vehicle->update(timeStep_);
            
            // Notify callbacks
            for (const auto& callback : vehicleUpdateCallbacks_) {
                callback(*vehicle);
            }
        }
        
        // Update simulation time
        simulationTime_ += timeStep_;
    }
    
    // Run simulation for specified duration
    void runFor(double duration) {
        double endTime = simulationTime_ + duration;
        while (running_ && simulationTime_ < endTime) {
            update();
        }
    }
    
    // Getters
    double getTimeStep() const { return timeStep_; }
    double getSimulationTime() const { return simulationTime_; }
    bool isRunning() const { return running_; }
    const std::vector<std::shared_ptr<Vehicle>>& getVehicles() const { return vehicles_; }
    
    // Setters
    void setTimeStep(double timeStep) { timeStep_ = timeStep; }
    
private:
    double timeStep_;      // Time step in seconds
    bool running_;         // Simulation running state
    double simulationTime_;// Current simulation time
    std::vector<std::shared_ptr<Vehicle>> vehicles_;
    std::vector<VehicleUpdateCallback> vehicleUpdateCallbacks_;
};

#endif // VEHICLE_SIM_SIMULATION_H