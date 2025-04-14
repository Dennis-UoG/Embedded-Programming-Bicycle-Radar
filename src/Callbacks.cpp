#include "bicycle_radar/Callbacks.h"

#include <sstream>
#include <bicycle_radar/camera_driver.h>


CollisionCallbacks::CollisionCallbacks(CameraSensor *camera_driver): camera_driver(camera_driver) {}

void CollisionCallbacks::onEvent(int eventId, std::string eventData) {
    IMUSensor::StructImu imu_data = IMUSensor::deserialize(eventData);
    this->camera_driver->TakePhoto();
}

RadarCallbacks::RadarCallbacks(LedDriver *led_driver): led_driver(led_driver) {}

void RadarCallbacks::onEvent(int eventId, std::string eventData) {
    ToFSensor::StructLidar lidar_data = ToFSensor::deserialize(eventData);
    this->led_driver->AdjustColourFrequency(lidar_data.Dist);
}

void TestCallbacks::onEvent(int eventId, std::string eventData) {
    std::cout << "TestCallbacks::onEvent " << eventId << " " << eventData << std::endl;
}
