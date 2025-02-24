#ifndef VEHICLE_SIM_FILE_PUBLISHER_H
#define VEHICLE_SIM_FILE_PUBLISHER_H

#include <string>
#include <fstream>
#include "Vehicle.h"
#include <nlohmann/json.hpp>

// Class for publishing vehicle updates to a file
class FilePublisher {
public:
    // Constructor with output file path
    explicit FilePublisher(const std::string& outputFilePath);

    // Destructor
    ~FilePublisher();

    // Publish vehicle update
    bool publishVehicleUpdate(const Vehicle& vehicle);

private:
    std::string outputFilePath_;
    std::ofstream outputFile_;

    // Convert vehicle to JSON string
    std::string vehicleToJson(const Vehicle& vehicle);
};

#endif // VEHICLE_SIM_FILE_PUBLISHER_H