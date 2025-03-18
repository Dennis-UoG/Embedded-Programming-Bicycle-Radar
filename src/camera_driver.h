#ifndef CAMERA_DRIVER_HPP
#define CAMERA_DRIVER_HPP

#include <libcamera/libcamera.h>
#include <libcamera/camera.h>
#include <libcamera-apps/image/image.hpp>


#include <iostream>
#include <png.h>
#include <filesystem>
#include <thread>
#include <chrono>

int CameraMainThread();

#endif // CAMERA_DRIVER_HPP