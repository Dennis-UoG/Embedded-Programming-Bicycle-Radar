#include "bicycle_radar/Callbacks.h"

#include <sstream>
#include <bicycle_radar/camera_driver.h>


/// @file Callbacks.cpp
/// @brief Defines callback handlers to manage events triggered by sensors or drivers.
/// @details This file implements the logic for event-based communication between
/// hardware components and system functionalities, such as camera actions or LED status changes.

/// @class CollisionCallbacks
/// @brief Handles collision-related events from the IMU sensor.
/// @details Translates collision events into appropriate actions, like triggering the camera.
/// @param camera_driver Pointer to CameraSensor used for photo capturing on collision.
CollisionCallbacks::CollisionCallbacks(CameraSensor *camera_driver): camera_driver(camera_driver) {}

/// @fn void CollisionCallbacks::onEvent(int eventId, std::string eventData)
/// @brief Processes collision events.
/// @param eventId ID of the triggered event (1 = collision).
/// @param eventData Serialized IMU data associated with the event.
void CollisionCallbacks::onEvent(int eventId, std::string eventData) {
    IMUSensor::StructImu imu_data = IMUSensor::deserialize(eventData);
    this->camera_driver->TakePhoto();
}

/// @class RadarCallbacks
/// @brief Manages events from the radar/ToF sensor.
/// @details Determines LED behavior based on the proximity of detected objects.
/// @param led_driver Pointer to LedDriver for LED adjustments.
RadarCallbacks::RadarCallbacks(LedDriver *led_driver): led_driver(led_driver) {}

/// @fn void RadarCallbacks::onEvent(int eventId, std::string eventData)
/// @brief Processes proximity events.
/// @param eventId ID of the triggered event (2 = radar detection).
/// @param eventData Serialized ToF (LIDAR) data associated with the event.
void RadarCallbacks::onEvent(int eventId, std::string eventData) {
    ToFSensor::StructLidar lidar_data = ToFSensor::deserialize(eventData);
    this->led_driver->AdjustColourFrequency(lidar_data.Dist);
}

/// @class TestCallbacks
/// @brief A simple callback class used for testing event handling.
/// @details This class prints event details to the standard output for debugging purposes.
///
/// @fn void TestCallbacks::onEvent(int eventId, std::string eventData)
/// @brief Prints event information.
/// @param eventId The ID of the triggered test event.
/// @param eventData Additional information about the event.
void TestCallbacks::onEvent(int eventId, std::string eventData) {
    std::cout << "TestCallbacks::onEvent " << eventId << " " << eventData << std::endl;
}
