#include "KafkaPublisher.h"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Constructor implementation
KafkaPublisher::KafkaPublisher(const std::string& brokerAddress, const std::string& topicName)
        : brokerAddress_(brokerAddress), topicName_(topicName) {

    std::string errstr;

    // Create Kafka configuration
    conf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));

    // Set broker address
    if (conf_->set("bootstrap.servers", brokerAddress_, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << "Error setting Kafka broker address: " << errstr << std::endl;
        return;
    }

    // Create producer instance
    producer_.reset(RdKafka::Producer::create(conf_.get(), errstr));
    if (!producer_) {
        std::cerr << "Failed to create Kafka producer: " << errstr << std::endl;
        return;
    }

    // Create topic configuration
    RdKafka::Conf* tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    // Create topic handle
    topic_.reset(RdKafka::Topic::create(producer_.get(), topicName_, tconf, errstr));
    delete tconf; // Topic has taken ownership

    if (!topic_) {
        std::cerr << "Failed to create Kafka topic: " << errstr << std::endl;
        return;
    }

    std::cout << "Kafka publisher initialized successfully." << std::endl;
}

// Destructor
KafkaPublisher::~KafkaPublisher() {
    // Allow Kafka to flush any pending messages before destruction
    if (producer_) {
        producer_->flush(1000);
    }
}

// Publish vehicle update
bool KafkaPublisher::publishVehicleUpdate(const Vehicle& vehicle) {
    if (!producer_ || !topic_) {
        std::cerr << "Kafka producer not initialized." << std::endl;
        return false;
    }

    // Convert vehicle to JSON string
    std::string payload = vehicleToJson(vehicle);

    // Publish message
    RdKafka::ErrorCode err = producer_->produce(
            topic_.get(),
            RdKafka::Topic::PARTITION_UA, // Use builtin partitioner
            RdKafka::Producer::RK_MSG_COPY, // Copy payload
            const_cast<char*>(payload.c_str()),
            payload.size(),
            vehicle.getId().c_str(),  // Message key = vehicle ID
            vehicle.getId().size(),
            nullptr  // Message opaque
    );

    if (err != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to produce message: " << RdKafka::err2str(err) << std::endl;
        return false;
    }

    // Poll to trigger delivery report callbacks
    producer_->poll(0);
    return true;
}

// Convert vehicle to JSON string
std::string KafkaPublisher::vehicleToJson(const Vehicle& vehicle) {
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