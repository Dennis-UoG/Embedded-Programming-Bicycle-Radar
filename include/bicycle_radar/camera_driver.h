#ifndef CAMERA_DRIVER_H
#define CAMERA_DRIVER_H

#include <libcamera/libcamera.h>

#include <libcamera/camera.h>
#include <libcamera-apps/image/image.hpp>
#include "libcam2opencv.h"
#include <libcamera/libcamera.h>

// #include <libyuv.h>


#include <iostream>
#include <sys/mman.h>
#include <png.h>
#include <filesystem>
#include <thread>
#include <chrono>

class CameraSensor {
public:
    struct CameraCallback : Libcam2OpenCV::Callback {
        //cv::Window* window = nullptr;
        virtual void hasFrame(const cv::Mat &frame, const libcamera::ControlList &metadata) {
            for (const auto &ctrl : metadata) {
                const libcamera::ControlId *id = libcamera::controls::controls.at(ctrl.first);
                const libcamera::ControlValue &value = ctrl.second;

                std::cout << "\t" << id->name() << " = " << value.toString()
                      << std::endl;
            }
            std::cout << std::endl;
        }
    };
    CameraSensor camera_sensor(std::string save_path);
    bool running = true;
    std::string SAVE_FOLDER_PATH;
    std::string cameraName(libcamera::Camera *camera);
    void TakePhoto();
    std::string get_current_timestamp();
    static void request_callback(libcamera::Request *request);
    void clearFolder(const std::string& folderPath);
    int Run();
};

#endif // CAMERA_DRIVER_HPP