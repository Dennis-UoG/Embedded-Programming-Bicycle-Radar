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
#include <mutex>

static std::string SAVE_FOLDER_PATH = "/home/group16/Desktop/Embedded-Programming-Bicycle-Radar/frame";

class CameraSensor {
public:
    struct CameraCallback : Libcam2OpenCV::Callback {
        CameraSensor* parent = nullptr;
        virtual void hasFrame(const cv::Mat &frame, const libcamera::ControlList &metadata) {
            if (parent && parent->takephoto) {
                std::string filename = SAVE_FOLDER_PATH + "/frame_" + get_current_timestamp() + ".png";
                cv::imwrite(filename, frame);
                parent->takephoto = false;
            }
        }
    };
    CameraSensor();
    ~CameraSensor();
    bool takephoto = false;
    bool running = true;
    Libcam2OpenCV* camera;
    std::mutex mtx;
    std::string cameraName(libcamera::Camera *camera);
    void TakePhoto();
    static std::string get_current_timestamp();
    static void request_callback(libcamera::Request *request);
    void clearFolder(const std::string& folderPath);
    int Run();
};

#endif // CAMERA_DRIVER_HPP