#include "bicycle_radar/Callbacks.h"

#include <sstream>


std::string CollisionCallbacks::get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H:%M:%S");

    return oss.str();
}

void CollisionCallbacks::onEvent(int eventId, std::string eventData) {
    IMUSensor::StructImu imu_data = IMUSensor::deserialize(eventData);
    std::string timestamp = get_current_timestamp();

}

RadarCallbacks::RadarCallbacks(LedDriver& led_driver): led_driver(led_driver) {}

void RadarCallbacks::onEvent(int eventId, std::string eventData) {
    ToFSensor::StructLidar lidar_data = ToFSensor::deserialize(eventData);
    this->led_driver.AdjustColourFrequency(lidar_data.Dist);
}

void TestCallbacks::onEvent(int eventId, std::string eventData) {
    std::cout << "TestCallbacks::onEvent " << eventId << " " << eventData << std::endl;
}
