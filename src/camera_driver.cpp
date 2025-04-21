#include "bicycle_radar/camera_driver.h"

/// @file camera_driver.cpp
/// @brief Implements the camera driver for capturing and managing photos.
/// @details Provides functionalities to initialize cameras, take photos, manage storage,
/// and run the camera loop.

/// @fn void CameraSensor::TakePhoto()
/// @brief Captures a photo using the camera driver.
/// @details Locks the thread for concurrency control and applies a timestamp to the photo.
void CameraSensor::TakePhoto()
{
    std::lock_guard<std::mutex> lock(this->mtx);
    std::string timestamp = get_current_timestamp();
    this->takephoto = true;
}

/// @fn std::string CameraSensor::get_current_timestamp()
/// @brief Generates a timestamp of the current date and time.
/// @return A string formatted as "YYYY-MM-DD_HH:MM:SS".
std::string CameraSensor::get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H:%M:%S");

    return oss.str();
}

/// @fn void CameraSensor::clearFolder(const std::string& folderPath)
/// @brief Clears all contents from a specified folder.
/// @param folderPath The path of the folder to clear.
/// @details Deletes all files from the target folder and creates it if it does not exist.
void CameraSensor::clearFolder(const std::string& folderPath) {
    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directory(folderPath);
    } else {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            std::filesystem::remove(entry.path());
        }
    }
}

/// @fn CameraSensor::CameraSensor()
/// @brief Constructs a CameraSensor object.
/// @details Initializes the camera system and registers a callback for Libcam2OpenCV.
CameraSensor::CameraSensor() {
    this->camera = new Libcam2OpenCV();
    CameraCallback* camera_callback = new CameraCallback();
    camera_callback->parent = this;
    this->camera->registerCallback(camera_callback);
}

/// @fn CameraSensor::~CameraSensor()
/// @brief Destroys a CameraSensor object.
/// @details Stops the camera from capturing further photos and cleans up resources.
CameraSensor::~CameraSensor() {
    this->camera->stop();
}

/// @fn int CameraSensor::Run()
/// @brief Main execution loop for the camera.
/// @details Configures camera settings and continuously monitors for photo-taking requests.
/// @return 0 if the loop executes successfully.
int CameraSensor::Run() {
    std::cout << "Camera Manager started." << std::endl;
    //clearFolder(SAVE_FOLDER_PATH);
    Libcam2OpenCVSettings settings;
    settings.framerate = 10;
    this->camera->start();

    //this->TakePhoto();

    while (running) {
        std::this_thread::sleep_for(std::chrono::hours(1));
    }
    return 0;
}