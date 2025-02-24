#ifndef VEHICLE_SIM_VEHICLE_H
#define VEHICLE_SIM_VEHICLE_H

#include "Route.h"
#include <string>
#include <cmath>

// Vehicle class representing a simulated vehicle
class Vehicle {
public:
    // Constructor with ID, initial position and route
    Vehicle(const std::string& id, const GeoPoint& position, const Route& route)
            : id_(id),
              position_(position),
              heading_(0.0),
              speed_(0.0),
              maxSpeed_(25.0),      // m/s (~55 mph)
              acceleration_(2.0),   // m/s²
              deceleration_(4.0),   // m/s²
              route_(route),
              waypointThreshold_(0.0001) // Approx 10m in geo coords
    {}

    // Update vehicle position based on time delta
    void update(double deltaTime) {
        // Skip update if route is completed
        if (route_.isCompleted()) {
            speed_ = 0.0;
            return;
        }

        // Get target waypoint
        GeoPoint targetWaypoint = route_.getCurrentWaypoint();

        // Calculate heading to waypoint
        double targetHeading = calculateHeading(position_, targetWaypoint);

        // Gradually adjust current heading towards target
        adjustHeading(targetHeading, deltaTime);

        // Adjust speed based on proximity to waypoint and heading difference
        adjustSpeed(targetWaypoint, deltaTime);

        // Move vehicle
        moveVehicle(deltaTime);

        // Check if reached waypoint
        checkWaypointReached();
    }

    // Getters
    const std::string& getId() const { return id_; }
    const GeoPoint& getPosition() const { return position_; }
    double getHeading() const { return heading_; }
    double getSpeed() const { return speed_; }
    const Route& getRoute() const { return route_; }

    // Setters
    void setRoute(const Route& route) { route_ = route; }
    void setMaxSpeed(double maxSpeed) { maxSpeed_ = maxSpeed; }
    void setAcceleration(double acceleration) { acceleration_ = acceleration; }
    void setDeceleration(double deceleration) { deceleration_ = deceleration; }

private:
    std::string id_;
    GeoPoint position_;
    double heading_;     // In radians, 0 = north, increases clockwise
    double speed_;       // Current speed in m/s
    double maxSpeed_;    // Maximum speed in m/s
    double acceleration_;// Acceleration rate in m/s²
    double deceleration_;// Deceleration rate in m/s²
    Route route_;
    double waypointThreshold_; // Distance threshold to consider waypoint reached

    // Calculate heading from current position to target position
    double calculateHeading(const GeoPoint& from, const GeoPoint& to) const {
        double dx = to.lon - from.lon;
        double dy = to.lat - from.lat;
        return std::atan2(dx, dy); // 0 = north, increases clockwise
    }

    // Adjust current heading towards target heading
    void adjustHeading(double targetHeading, double deltaTime) {
        // Simple linear interpolation for now
        double headingDiff = targetHeading - heading_;

        // Normalize to [-π, π]
        while (headingDiff > M_PI) headingDiff -= 2 * M_PI;
        while (headingDiff < -M_PI) headingDiff += 2 * M_PI;

        // Adjust heading (could add rotation rate limit here)
        heading_ += headingDiff * 2.0 * deltaTime;

        // Normalize result
        while (heading_ > 2 * M_PI) heading_ -= 2 * M_PI;
        while (heading_ < 0) heading_ += 2 * M_PI;
    }

    // Adjust speed based on distance to waypoint and heading difference
    void adjustSpeed(const GeoPoint& targetWaypoint, double deltaTime) {
        // Distance to next waypoint
        double distance = position_.distanceTo(targetWaypoint);

        // Define target speed based on distance
        double targetSpeed = maxSpeed_;

        // Slow down when approaching waypoint
        if (distance < 3 * waypointThreshold_) {
            targetSpeed = maxSpeed_ * (distance / (3 * waypointThreshold_));
        }

        // Adjust speed based on heading alignment
        if (speed_ > 0) {
            // Accelerate or decelerate as needed
            if (speed_ < targetSpeed) {
                speed_ += acceleration_ * deltaTime;
                if (speed_ > targetSpeed) speed_ = targetSpeed;
            } else if (speed_ > targetSpeed) {
                speed_ -= deceleration_ * deltaTime;
                if (speed_ < targetSpeed) speed_ = targetSpeed;
            }
        } else {
            // Starting from stop
            speed_ = std::min(acceleration_ * deltaTime, targetSpeed);
        }

        // Ensure speed stays within limits
        speed_ = std::max(0.0, std::min(speed_, maxSpeed_));
    }

    // Move vehicle based on current speed and heading
    void moveVehicle(double deltaTime) {
        // Calculate movement deltas
        double distance = speed_ * deltaTime;
        double dx = distance * std::sin(heading_);
        double dy = distance * std::cos(heading_);

        // Update position
        position_.lon += dx;
        position_.lat += dy;
    }

    // Check if vehicle reached current waypoint and advance to next
    void checkWaypointReached() {
        GeoPoint currentWaypoint = route_.getCurrentWaypoint();
        if (position_.distanceTo(currentWaypoint) <= waypointThreshold_) {
            route_.advanceToNextWaypoint();
        }
    }
};

#endif // VEHICLE_SIM_VEHICLE_H