#include "bicycle_radar/camera_driver.h"


//std::string CameraSensor::cameraName(libcamera::Camera *camera)
//{
//    const libcamera::ControlList &props = camera->properties();
//    std::string name;
//
//    const auto &location = props.get(libcamera::properties::Location);
//    if (location) {
//        switch (*location) {
//            case libcamera::properties::CameraLocationFront:
//                name = "Internal front camera";
//            break;
//            case libcamera::properties::CameraLocationBack:
//                name = "Internal back camera";
//            break;
//            case libcamera::properties::CameraLocationExternal:
//                name = "External camera";
//            const auto &model = props.get(libcamera::properties::Model);
//            if (model)
//                name = " '" + *model + "'";
//            break;
//        }
//    }
//
//    name += " (" + camera->id() + ")";
//
//    return name;
//}

void CameraSensor::TakePhoto()
{
  	this->mtx.lock();
    std::string timestamp = get_current_timestamp();
    CameraSensor::CameraCallback camera_callback;
   	this->camera->registerCallback(&camera_callback);
//    Libcam2OpenCVSettings settings;
//    settings.framerate = 1;
    this->camera->start();
    this->camera->stop();
    this->mtx.unlock();
}

//void CameraSensor::request_callback(libcamera::Request *request) {
//    if (request->status() == libcamera::Request::RequestCancelled)
//    {
//        std::cout << "canceled" << std::endl;
//        return;
//    }
//
//    const std::map<const libcamera::Stream *, libcamera::FrameBuffer *> &buffers = request->buffers();
//    for (auto bufferPair : buffers) {
//        libcamera::FrameBuffer *buffer = bufferPair.second;
//        const libcamera::FrameMetadata &metadata = buffer->metadata();
//        std::cout << " seq: " <<  metadata.sequence << " bytesused: ";
//
//        unsigned int nplane = 0;
//        for (const libcamera::FrameMetadata::Plane &plane : metadata.planes())
//        {
//            std::cout << plane.bytesused << std::endl;
//        }
//
//        saveFrame(metadata, buffer, bufferPair.first);
//    }
//}

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

int CameraSensor::Run() {
    std::cout << "Camera Manager started." << std::endl;
//    clearFolder(SAVE_FOLDER_PATH);
	this->camera = new Libcam2OpenCV();
    std::this_thread::sleep_for(std::chrono::hours(8));

    return 0;
}