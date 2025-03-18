//
// Created by Jiancheng Zhang on 14/02/2025.
//

#include "camera_driver.h"

#define SAVE_FOLDER_PATH std::string("./frame")

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

static void saveFrame(const libcamera::FrameMetadata &metadata, const libcamera::FrameBuffer *buffer,const libcamera::Stream *stream)
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y%m%d_%H%M%S");
    std::string timestamp = ss.str();

    std::string filename = SAVE_FOLDER_PATH  + "/frame_" + timestamp + ".png";

    int fd = buffer->planes()[0].fd.get();
    size_t size = metadata.planes().size();

    uint8_t *data = static_cast<uint8_t *>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
    if (data == MAP_FAILED) {
        std::cerr << "Failed to mmap frame data" << std::endl;
        return;
    }

    int width = stream->configuration().size.width;
    int height = stream->configuration().size.height;

    FILE *file = fopen(filename.c_str(), "wb");
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        munmap(data, size);
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "Failed to create PNG write structure" << std::endl;
        munmap(data, size);
        fclose(file);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Failed to create PNG info structure" << std::endl;
        munmap(data, size);
        png_destroy_write_struct(&png, nullptr);
        fclose(file);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << "Error during PNG creation" << std::endl;
        munmap(data, size);
        png_destroy_write_struct(&png, &info);
        fclose(file);
        return;
    }

    png_init_io(png, file);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);


    png_write_info(png, info);
    for (int y = 0; y < height; y++) {
        png_write_row(png, data + y * width * 3);
    }
    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    fclose(file);
    munmap(data, size);

    std::cout << "Frame saved to " << filename << std::endl;
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

        saveFrame(metadata, buffer, bufferPair.first);
    }
}

void clearFolder(const std::string& folderPath) {
    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directory(folderPath);
    } else {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            std::filesystem::remove(entry.path());
        }
    }
}

int CameraMainThread() {
    std::cout << "Camera Manager started." << std::endl;

    clearFolder(SAVE_FOLDER_PATH);
    

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
    while(running){
        for (std::unique_ptr<libcamera::Request> &request : requests){
            camera->queueRequest(request.get());
            request->reuse(libcamera::Request::ReuseBuffers);
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
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