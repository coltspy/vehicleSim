#ifndef VEHICLE_SIM_KAFKA_PUBLISHER_H
#define VEHICLE_SIM_KAFKA_PUBLISHER_H

#include <string>
#include <memory>
#include <librdkafka/rdkafkacpp.h>
#include "Vehicle.h"
#include <nlohmann/json.hpp>

// Class for publishing vehicle updates to Kafka
class KafkaPublisher {
public:
    // Constructor with Kafka broker address and topic name
    KafkaPublisher(const std::string& brokerAddress, const std::string& topicName);

    // Destructor
    ~KafkaPublisher();

    // Publish vehicle update
    bool publishVehicleUpdate(const Vehicle& vehicle);

private:
    std::string brokerAddress_;
    std::string topicName_;

    // Kafka producer configuration
    std::unique_ptr<RdKafka::Conf> conf_;

    // Kafka producer instance
    std::unique_ptr<RdKafka::Producer> producer_;

    // Kafka topic handle
    std::unique_ptr<RdKafka::Topic> topic_;

    // Convert vehicle to JSON string
    std::string vehicleToJson(const Vehicle& vehicle);
};

#endif // VEHICLE_SIM_KAFKA_PUBLISHER_H