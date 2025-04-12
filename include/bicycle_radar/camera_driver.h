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

static std::string SAVE_FOLDER_PATH = "./frame";

class CameraSensor {
public:
    struct CameraCallback : Libcam2OpenCV::Callback {
        //cv::Window* window = nullptr;
        virtual void hasFrame(const cv::Mat &frame, const libcamera::ControlList &) {
            cv::imwrite(SAVE_FOLDER_PATH, frame);
        }
    };
<<<<<<< HEAD
    CameraSensor(std::string save_path);
=======
    CameraSensor();
>>>>>>> 2b4cd82780259a7a2e1266a35cfaacff6ca1bb48
    bool running = true;
    Libcam2OpenCV camera;
    std::string cameraName(libcamera::Camera *camera);
    void TakePhoto();
    std::string get_current_timestamp();
    static void request_callback(libcamera::Request *request);
    void clearFolder(const std::string& folderPath);
    int Run();
};

#endif // CAMERA_DRIVER_HPP