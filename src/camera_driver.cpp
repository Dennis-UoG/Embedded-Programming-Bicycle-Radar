#include "bicycle_radar/camera_driver.h"


void CameraSensor::TakePhoto()
{
    std::lock_guard<std::mutex> lock(this->mtx);
    std::string timestamp = get_current_timestamp();
    this->takephoto = true;
}

std::string CameraSensor::get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H:%M:%S");

    return oss.str();
}

void CameraSensor::clearFolder(const std::string& folderPath) {
    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directory(folderPath);
    } else {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            std::filesystem::remove(entry.path());
        }
    }
}

CameraSensor::CameraSensor() {
    this->camera = new Libcam2OpenCV();
    CameraCallback* camera_callback = new CameraCallback();
    camera_callback->parent = this;
    this->camera->registerCallback(camera_callback);
}

CameraSensor::~CameraSensor() {
    this->camera->stop();
}

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