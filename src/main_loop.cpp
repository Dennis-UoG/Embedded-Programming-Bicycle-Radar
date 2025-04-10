#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sstream>
#include <typeinfo>
#include <thread>
#include <vector>
#include <csignal>

#include "bicycle_radar/CallbackInterface.h"
#include "bicycle_radar/Callbacks.h"
#include "bicycle_radar/imu_driver.h"
#include "bicycle_radar/tof_driver.h"
#include "bicycle_radar/led_driver.h"
#include "bicycle_radar/camera_driver.h"

bool camera_running = true;
bool test_running = true;

void RunIMUSensor(IMUSensor& imu_sensor) {
    std::cout << "Starting IMU sensor..." << std::endl;
    imu_sensor.Init();
    std::cout << "IMU sensor is running" << std::endl;
    imu_sensor.Run();
}

void RunToFSensor(ToFSensor& tof_sensor) {
    std::cout << "Starting ToF sensor..." << std::endl;
    tof_sensor.Init();
    std::cout << "ToF sensor is running" << std::endl;
    tof_sensor.Run();
}

void RunLED(LedDriver& led_driver) {
    std::cout << "Starting LED driver..." << std::endl;
    led_driver.Init();
    std::cout << "LED driver is running" << std::endl;
    led_driver.FlashLED();
}

void RunCamera() {
    std::cout << "Starting camera driver..." << std::endl;
}

std::vector<std::thread> start(IMUSensor& imu_sensor_driver, ToFSensor& tof_sensor_driver, LedDriver& led_driver) {
    std::vector<std::thread> workers;
    std::thread imu_thread(RunIMUSensor, imu_sensor_driver);
    workers.push_back(imu_thread);
    imu_thread.detach();
    std::thread tof_thread(RunToFSensor, tof_sensor_driver);
    workers.push_back(tof_thread);
    tof_thread.detach();
    std::thread led_thread(RunLED, led_driver);
    workers.push_back(led_thread);
    led_thread.detach();
    return workers;
}

void stop(std::vector<std::thread> workers, IMUSensor& imu_sensor_driver, ToFSensor& tof_sensor_driver, LedDriver& led_driver) {
    imu_sensor_driver.running = false;
    tof_sensor_driver.running = false;
    led_driver.running = false;
    for (auto& worker : workers) {
        worker.join();
    }
}

int main() 
{
    std::string imu_sensor_port = "/dev/ttyUSB0";
    std::string tof_sensor_port = "/dev/ttyUSB1";
    std::string gpio_chip_name = "/dev/gpiochip0";
    std::string led_parameter_path = "parameters/led_freq_dist.yaml";
    std::vector<int> gpio_pins = {20, 21, 26};

    LedDriver led_driver(gpio_chip_name, led_parameter_path, gpio_pins);

    RadarCallbacks radar_callbacks(led_driver);
    CollisionCallbacks collision_callbacks();

    EventTrigger* IMUeventTrigger;
    EventTrigger* RadareventTrigger;
    // IMUeventTrigger->addCallback(collision_callbacks);
    RadareventTrigger->addCallback(&radar_callbacks);

    IMUSensor imu_sensor_driver(imu_sensor_port, IMUeventTrigger);
    ToFSensor tof_sensor_driver(tof_sensor_port, RadareventTrigger);

    std::vector<std::thread> workers(start(imu_sensor_driver, tof_sensor_driver, led_driver));

    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(8));
    }

    stop(workers, imu_sensor_driver, tof_sensor_driver, led_driver);
    return 0;
}