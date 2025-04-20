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
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <filesystem>

#include "bicycle_radar/CallbackInterface.h"
#include "bicycle_radar/Callbacks.h"
#include "bicycle_radar/imu_driver.h"
#include "bicycle_radar/tof_driver.h"
#include "bicycle_radar/led_driver.h"
#include "bicycle_radar/camera_driver.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void RunIMUSensor(IMUSensor *imu_sensor) {
    std::cout << "Starting IMU sensor..." << std::endl;
    imu_sensor->Init();
    std::cout << "IMU sensor is running" << std::endl;
    imu_sensor->Run();
}

void RunToFSensor(ToFSensor *tof_sensor) {
    std::cout << "Starting ToF sensor..." << std::endl;
    tof_sensor->Init();
    std::cout << "ToF sensor is running" << std::endl;
    tof_sensor->Run();
}

void RunLED(LedDriver *led_driver) {
    std::cout << "Starting LED driver..." << std::endl;
    led_driver->Init();
    std::cout << "LED driver is running" << std::endl;
    led_driver->FlashLED();
}

void RunCamera(CameraSensor *camera_sensor) {
    std::cout << "Starting camera driver..." << std::endl;
    std::cout << "Camera driver is running" << std::endl;
    camera_sensor->Run();
}

std::vector<std::thread*>* start(IMUSensor *imu_sensor_driver, ToFSensor *tof_sensor_driver, LedDriver *led_driver, CameraSensor *camera_sensor) {
    std::vector<std::thread*> *workers = new std::vector<std::thread*>();
    std::thread imu_thread(RunIMUSensor, imu_sensor_driver);
    workers->push_back(&imu_thread);
    imu_thread.detach();
    std::thread tof_thread(RunToFSensor, tof_sensor_driver);
    workers->push_back(&tof_thread);
    tof_thread.detach();
    std::thread led_thread(RunLED, led_driver);
    workers->push_back(&led_thread);
    led_thread.detach();
    std::thread camera_thread(RunCamera, camera_sensor);
    workers->push_back(&camera_thread);
    camera_thread.detach();
    return workers;
}

void stop(std::vector<std::thread*> *workers, IMUSensor *imu_sensor_driver, ToFSensor *tof_sensor_driver, LedDriver *led_driver, CameraSensor *camera_sensor) {
    imu_sensor_driver->running = false;
    tof_sensor_driver->running = false;
    led_driver->running = false;
    camera_sensor->running = false;
    for (std::thread* worker : *workers) {
        worker->join();
    }
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

std::vector<uint8_t> readFileB(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return {};
    }
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        std::cerr << "Failed to read file: " << filePath << std::endl;
        return {};
    }

    file.close();
    return buffer;
}

std::string getMimeType(const std::string& filePath) {
    std::string extension = fs::path(filePath).extension().string();
    if (extension == ".html") return "text/html";
    if (extension == ".css") return "text/css";
    if (extension == ".js") return "application/javascript";
    if (extension == ".png") return "image/png";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    return "text/plain";
}





httplib::Server svr;

int main() 
{
    std::string led_parameter_path = "./parameters/led_freq_dist.json";

    std::ifstream ifs(led_parameter_path);
    json data = json::parse(ifs);
    //YAML::Node *data = YAML::LoadFile(led_parameter_path);
    svr.Get("/getpara", [&data](const httplib::Request &, httplib::Response &res) {
        res.set_content(data.dump(), "application/json");
    });
    
    svr.Post("/updatepara", [&data](const httplib::Request &req, httplib::Response &res) {
        try {
            json new_data = json::parse(req.body);
            data = new_data;
            std::ofstream ofs("parameters/led_freq_dist.json");
            ofs << data.dump();
            ofs.close();

            res.set_content("Parameters updated successfully!", "text/plain");
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("Invalid JSON data!", "text/plain");
        }
    });

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        std::string filePath = "./www/index.html";
        std::string content = readFile(filePath);

        if (!content.empty()) {
            res.set_content(content, "text/html");
        } else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
    });
    svr.Get("/picframelist", [](const httplib::Request& req, httplib::Response& res) {
        std::string folderPath = "./frame/";
 
        json result = json::array();
        if (!std::filesystem::exists(folderPath)) {
            std::cout << "Folder does not exist: " << folderPath << std::endl;
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                std::string mime_type = getMimeType(entry.path().string());
                if (mime_type.find("image/") != std::string::npos) {
                    json file_info = {
                        {"filename", filename},
                        {"size", entry.file_size()},
                        {"mime_type", mime_type}
                    };
                    result.push_back(file_info);
                }
            }
        }
        res.set_content(result.dump(), "application/json");
    });
    svr.Get(R"(/frame/.*)", [](const httplib::Request& req, httplib::Response& res) {
        try{
            std::string basePath = ".";
            std::cout<< basePath << std::endl;
            std::string requestedPath = req.path;
            std::cout<<"test:"<< requestedPath << std::endl;
            fs::path fullPath = fs::absolute(basePath + requestedPath);
            std::cout<<"test2:"<< fullPath.string() << std::endl;
    
            if (!fs::exists(fullPath) || !fs::is_regular_file(fullPath)) {
                res.status = 404;
                res.set_content("File not found", "text/plain");
                return;
            }
        
            std::vector<uint8_t> content = readFileB(fullPath.string());
            if (!content.empty()) {
                res.set_content(reinterpret_cast<char*>(content.data()), content.size(), getMimeType(fullPath.string()));
            } else {
                //res.status = 500;
                res.set_content("Failed to read file", "text/plain");
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error resolving path: " << e.what() << std::endl;
            //res.status = 500;
            res.set_content("Internal server error", "text/plain");
        }
    });
    svr.Get(R"(/(?!frame/).*)", [](const httplib::Request& req, httplib::Response& res) {
        std::string basePath = "./www";
        std::string requestedPath = req.path;

        fs::path fullPath = fs::canonical(basePath + requestedPath);

        if (!fs::exists(fullPath) || !fs::is_regular_file(fullPath)) {
            res.status = 404;
            res.set_content("File not found", "text/plain");
            return;
        }

        std::string content = readFile(fullPath.string());
        if (!content.empty()) {
            res.set_content(content, getMimeType(fullPath.string()));
        } else {
            res.status = 500;
            res.set_content("Failed to read file", "text/plain");
        }
    });

    std::string imu_sensor_port = "/dev/IMU";
    std::string tof_sensor_port = "/dev/ToF";

    int gpio_chip_number = 0;

    std::vector<int> *gpio_pins = new std::vector<int> {20, 21, 26};

    LedDriver *led_driver = new LedDriver(gpio_chip_number, &data, gpio_pins);
    CameraSensor *camera_driver = new CameraSensor();

    RadarCallbacks radar_callbacks(led_driver);
    CollisionCallbacks collision_callbacks(camera_driver);

    EventTrigger* IMUeventTrigger = new EventTrigger();
    EventTrigger* RadareventTrigger = new EventTrigger();
    IMUeventTrigger->addCallback(&collision_callbacks);
    RadareventTrigger->addCallback(&radar_callbacks);

    IMUSensor *imu_sensor_driver = new IMUSensor(imu_sensor_port, IMUeventTrigger);
    ToFSensor *tof_sensor_driver = new ToFSensor(tof_sensor_port, RadareventTrigger);

    std::vector<std::thread*> *workers(start(imu_sensor_driver, tof_sensor_driver, led_driver, camera_driver));
    svr.listen("0.0.0.0", 8080);
    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(8));
    }

    stop(workers, imu_sensor_driver, tof_sensor_driver, led_driver, camera_driver);
    return 0;
}