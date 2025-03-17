#ifndef CAMERA_DRIVER_HPP
#define CAMERA_DRIVER_HPP

#include <libcamera/libcamera.h>
#include <libcamera/camera.h>
#include <libcamera-apps/image/image.hpp>
#include <opencv2/opencv.hpp>


#include <iostream>
#include <thread>
#include <chrono>

int CameraMainThread();

#endif // CAMERA_DRIVER_HPP