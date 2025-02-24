#ifndef VEHICLE_SIM_ROUTE_H
#define VEHICLE_SIM_ROUTE_H

#include <vector>
#include <cmath>
#include <utility>

// Represents a 2D position with latitude and longitude
struct GeoPoint {
    double lat;  // Latitude
    double lon;  // Longitude

    // Distance calculation between two GeoPoints (simple Euclidean for now)
    double distanceTo(const GeoPoint& other) const {
        // Note: A proper implementation would use Haversine formula
        // This is simplified for initial implementation
        return std::sqrt(std::pow(lat - other.lat, 2) + std::pow(lon - other.lon, 2));
    }
};

// Represents a route as a series of waypoints
class Route {
public:
    // Constructor with initial waypoints
    Route(const std::vector<GeoPoint>& waypoints) : waypoints_(waypoints), currentWaypointIndex_(0) {}

    // Default constructor
    Route() : currentWaypointIndex_(0) {}

    // Add a waypoint to the route
    void addWaypoint(const GeoPoint& waypoint) {
        waypoints_.push_back(waypoint);
    }

    // Get current waypoint
    GeoPoint getCurrentWaypoint() const {
        if (waypoints_.empty()) {
            return {0.0, 0.0}; // Default point if no waypoints
        }
        return waypoints_[currentWaypointIndex_];
    }

    // Get next waypoint
    GeoPoint getNextWaypoint() const {
        if (waypoints_.empty() || currentWaypointIndex_ >= waypoints_.size() - 1) {
            return getCurrentWaypoint(); // Return current if no next exists
        }
        return waypoints_[currentWaypointIndex_ + 1];
    }

    // Advance to next waypoint
    bool advanceToNextWaypoint() {
        if (currentWaypointIndex_ < waypoints_.size() - 1) {
            currentWaypointIndex_++;
            return true;
        }
        return false; // Route completed
    }

    // Check if route is completed
    bool isCompleted() const {
        return !waypoints_.empty() && currentWaypointIndex_ >= waypoints_.size() - 1;
    }

    // Get closest point on the route to a given position
    std::pair<GeoPoint, double> getClosestPointOnRoute(const GeoPoint& position) const;

    // Get total route distance
    double getTotalDistance() const;

    // Get all waypoints
    const std::vector<GeoPoint>& getWaypoints() const {
        return waypoints_;
    }

private:
    std::vector<GeoPoint> waypoints_;
    size_t currentWaypointIndex_;
};

#endif // VEHICLE_SIM_ROUTE_H