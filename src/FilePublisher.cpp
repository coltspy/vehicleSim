#include "FilePublisher.h"
#include <iostream>

using json = nlohmann::json;

// Constructor implementation
FilePublisher::FilePublisher(const std::string& outputFilePath)
        : outputFilePath_(outputFilePath) {

    // Open file for writing
    outputFile_.open(outputFilePath, std::ios::out);

    if (!outputFile_.is_open()) {
        std::cerr << "Error opening output file: " << outputFilePath << std::endl;
        return;
    }

    // Write opening bracket for JSON array
    outputFile_ << "[" << std::endl;

    std::cout << "File publisher initialized successfully." << std::endl;
}

// Destructor
FilePublisher::~FilePublisher() {
    if (outputFile_.is_open()) {
        // Write closing bracket for JSON array
        outputFile_ << "]" << std::endl;
        outputFile_.close();
    }
}

// Publish vehicle update
bool FilePublisher::publishVehicleUpdate(const Vehicle& vehicle) {
    if (!outputFile_.is_open()) {
        std::cerr << "Output file not opened." << std::endl;
        return false;
    }

    // Convert vehicle to JSON string
    std::string payload = vehicleToJson(vehicle);

    // Check if this is not the first entry (we need a comma)
    if (outputFile_.tellp() > 2) {
        outputFile_ << "," << std::endl;
    }

    // Write to file
    outputFile_ << "  " << payload;
    outputFile_.flush();

    return true;
}

// Convert vehicle to JSON string
std::string FilePublisher::vehicleToJson(const Vehicle& vehicle) {
    json j;
    j["id"] = vehicle.getId();
    j["timestamp"] = std::time(nullptr);
    j["position"] = {
            {"lat", vehicle.getPosition().lat},
            {"lon", vehicle.getPosition().lon}
    };
    j["heading"] = vehicle.getHeading();
    j["speed"] = vehicle.getSpeed();
    return j.dump();
}