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
#include <gpiod.h>
#include "bicycle_radar/led_driver.h"


/*#include "bicycle_radar/CallbackInterface.h"
#include "bicycle_radar/Callbacks.h"
#include "bicycle_radar/imu_driver.h"
#include "bicycle_radar/tof_driver.h"
#include "bicycle_radar/led_driver.h"*/
//#include "bicycle_radar/camera_driver.h"

/*bool camera_running = true;
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
}*/

int main() 
{
    std::string imu_sensor_port = "/dev/ttyUSB0";
    std::string tof_sensor_port = "/dev/ttyUSB1";
    int gpio_chip_number = 0;
    std::string led_parameter_path = "parameters/led_freq_dist.yaml";
    std::vector<int> *gpio_pins = new std::vector<int> {20, 21, 26};

    LedDriver *led_driver = new LedDriver(gpio_chip_number, led_parameter_path, gpio_pins);

    led_driver->Init();
    /*RadarCallbacks radar_callbacks(led_driver);
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

    stop(workers, imu_sensor_driver, tof_sensor_driver, led_driver);*/
    /*int pin = 20;
    struct gpiod_chip *chip = gpiod_chip_open_by_number(0);
    if (!chip) {
        perror("Failed to open GPIO chip");
        return 1;
    }
    struct gpiod_line *line = gpiod_chip_get_line(chip, pin);
    if (!line) {
        perror("Failed to get GPIO line");
        gpiod_chip_close(chip);
        return 1;
    }
    if (gpiod_line_request_output(line, "write-thread", 0) < 0) {
        perror("Failed to request output");
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_line_set_value(line, 1);
    printf("GPIO %d set to: %d\n", pin, 1);
    sleep(1);
    gpiod_line_release(line);
    gpiod_chip_close(chip);*/
    return 0;
}