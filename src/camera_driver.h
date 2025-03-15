#ifndef CAMERA_DRIVER_H
#define CAMERA_DRIVER_H

#include <libcamera/libcamera.h>
#include <libcamera/camera.h>
#include <libcamera-apps/image/image.hpp>


#include <iostream>
#include <sys/mman.h>
#include <png.h>
#include <filesystem>
#include <thread>
#include <chrono>

extern bool camera_running;
int CameraMainThread();

#endif // CAMERA_DRIVER_HPP