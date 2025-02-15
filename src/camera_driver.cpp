//
// Created by Jiancheng Zhang on 14/02/2025.
//

#include <libcamera/libcamera.h>
#include <iostream>

int main() {
    libcamera::CameraManager cameraManager;

    if (cameraManager.start()) {
        std::cerr << "Failed to start camera manager" << std::endl;
        return -1;
    }

    auto cameras = cameraManager.cameras();
    if (cameras.empty()) {
        std::cerr << "No cameras found!" << std::endl;
        return -1;
    }

    auto camera = cameras.front();

    if (camera->acquire()) {
        std::cerr << "Failed to acquire camera" << std::endl;
        return -1;
    }

    std::cout << "Camera information: " << camera->properties().toString() << std::endl;

    camera->release();

    return 0;
}