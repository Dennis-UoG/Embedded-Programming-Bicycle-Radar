//
// Created by Jiancheng Zhang on 14/02/2025.
//

#include <libcamera/libcamera.h>
#include <libcamera/camera.h>
#include <libcamera-apps/image/image.hpp>
#include <iostream>
#include <thread>
#include <chrono>

std::string cameraName(libcamera::Camera *camera)
{
    const libcamera::ControlList &props = camera->properties();
    std::string name;

    const auto &location = props.get(libcamera::properties::Location);
    if (location) {
        switch (*location) {
            case libcamera::properties::CameraLocationFront:
                name = "Internal front camera";
            break;
            case libcamera::properties::CameraLocationBack:
                name = "Internal back camera";
            break;
            case libcamera::properties::CameraLocationExternal:
                name = "External camera";
            const auto &model = props.get(libcamera::properties::Model);
            if (model)
                name = " '" + *model + "'";
            break;
        }
    }

    name += " (" + camera->id() + ")";

    return name;
}

static void request_callback(libcamera::Request *request)
{
    if (request->status() == libcamera::Request::RequestCancelled)
        return;

    const std::map<const libcamera::Stream *, libcamera::FrameBuffer *> &buffers = request->buffers();
    for (auto bufferPair : buffers) {
        libcamera::FrameBuffer *buffer = bufferPair.second;
        const libcamera::FrameMetadata &metadata = buffer->metadata();
        std::cout << " seq: " <<  metadata.sequence << " bytesused: ";


        unsigned int nplane = 0;
        for (const libcamera::FrameMetadata::Plane &plane : metadata.planes())
        {
            std::cout << plane.bytesused << std::endl;
        }

        png_save(metadata.planes(), bufferPair.first, );
    }
}

int main() {
    std::unique_ptr<libcamera::CameraManager> cm = std::make_unique<libcamera::CameraManager>();

    cm->start();

    for (auto const &camera : cm->cameras())
        std::cout << " - " << cameraName(camera.get()) << std::endl;

    if (cm->cameras().empty()) {
        std::cout << "No cameras were identified on the system."
              << std::endl;
        cm->stop();
        return EXIT_FAILURE;
    }
    std::string cameraId = cm->cameras()[0]->id();
    auto camera = cm->get(cameraId);
    camera->acquire();

    std::unique_ptr<libcamera::CameraConfiguration> config =
        camera->generateConfiguration( { libcamera::StreamRole::StillCapture } );

    libcamera::StreamConfiguration &streamConfig = config->at(0);
    std::cout << "Default viewfinder configuration is: "
          << streamConfig.toString() << std::endl;

    streamConfig.size.width = 1920;
    streamConfig.size.height = 1080;

    config->validate();
    std::cout << "Validated viewfinder configuration is: "
          << streamConfig.toString() << std::endl;
    camera->configure(config.get());

    libcamera::FrameBufferAllocator *allocator = new libcamera::FrameBufferAllocator(camera);

    for (libcamera::StreamConfiguration &cfg : *config) {
        int ret = allocator->allocate(cfg.stream());
        if (ret < 0) {
            std::cerr << "Can't allocate buffers" << std::endl;
            return EXIT_FAILURE;
        }

        size_t allocated = allocator->buffers(cfg.stream()).size();
        std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
    }

    libcamera::Stream *stream = streamConfig.stream();
    const std::vector<std::unique_ptr<libcamera::FrameBuffer>> &buffers = allocator->buffers(stream);
    std::vector<std::unique_ptr<libcamera::Request>> requests;
    std::cout << buffers.size() << std::endl;
    for (unsigned int i = 0; i < buffers.size(); ++i) {
        std::unique_ptr<libcamera::Request> request = camera->createRequest();
        if (!request)
        {
            std::cerr << "Can't create request" << std::endl;
            return EXIT_FAILURE;
        }

        const std::unique_ptr<libcamera::FrameBuffer> &buffer = buffers[i];
        int ret = request->addBuffer(stream, buffer.get());
        if (ret < 0)
        {
            std::cerr << "Can't set buffer for request"
                  << std::endl;
            return EXIT_FAILURE;
        }

        /*
         * Controls can be added to a request on a per frame basis.
         */
        // ControlList &controls = request->controls();
        // controls.set(controls::Brightness, 0.5);

        requests.push_back(std::move(request));
    }
    std::cout << "request size" << requests.size() << std::endl;
    camera->requestCompleted.connect(request_callback);

    camera->start();
    for (std::unique_ptr<libcamera::Request> &request : requests)
        camera->queueRequest(request.get());

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    camera->stop();

    allocator->free(stream);
    delete allocator;
    camera->release();
    camera.reset();
    cm->stop();
    return 0;
}