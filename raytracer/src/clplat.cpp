#include "clplat.hpp"

#include <string>
#include <cmath>
#include <iostream>
#include <iomanip>

#if defined(_MSC_VER)
#include <cstdio>
#include <memory>
#else
// Linked resource on Unixes
extern char _binary____src_renderer_cl_start;
extern char _binary____src_renderer_cl_end;
char* src_renderer = &_binary____src_renderer_cl_start;
size_t src_renderer_size = (&_binary____src_renderer_cl_end - &_binary____src_renderer_cl_start);
#endif

void CLPlat::printInfo(std::ostream &out) {
    std::string name;
    platform->getInfo(CL_PLATFORM_NAME, &name);
    std::string vendor;
    platform->getInfo(CL_PLATFORM_VENDOR, &vendor);
    out << "Platform: " << name << " by " << vendor << std::endl;

    device->getInfo(CL_DEVICE_NAME, &name);
    out << "Device: " << name << std::endl;

    size_t groupSize;
    device->getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &groupSize);
    out << "Max work group size: " << groupSize << std::endl;
    std::vector<size_t> workItemSizes;
    device->getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &workItemSizes);
    out << "Max work item sizes";
    for (size_t size: workItemSizes)
        out << " " << size;
    out << std::endl;

    std::vector<cl::ImageFormat> formats;
    context->getSupportedImageFormats(CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D, &formats);
    std::ios state(NULL);
    state.copyfmt(out);
    out << std::hex;
    for (cl::ImageFormat format: formats)
        out << "channel order: " << format.image_channel_order
            << " type: " << format.image_channel_data_type << std::endl;
    out.copyfmt(state);
}

CLPlat::CLPlat() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    platform.reset(new cl::Platform(platforms[0]));

    std::vector<cl::Device> devices;
    platform->getDevices(CL_DEVICE_TYPE_ALL, &devices);
    device.reset(new cl::Device(devices[0]));
    std::vector<cl::Device> singleDevice;
    singleDevice.push_back(devices[0]);

    context.reset(new cl::Context(singleDevice));
    queue.reset(new cl::CommandQueue(*context, *device, 0));

    initProgram(*device);

#if defined(DEBUG)
    printInfo(std::cout);
#endif

    // Actually kind of useless, since local memory use decides the
    // actual group size
    device->getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxGroupSize);
}

void CLPlat::initProgram(const cl::Device& device) {
    cl::Program::Sources sources;
#if defined(_MSC_VER)
    FILE* f = NULL;
    fopen_s(&f, "renderer.cl", "rb");
    if (f == NULL)
        throw "Could not open kernel file";
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::unique_ptr<char[]> source(new char[size]);
    fread(source.get(), size, 1, f);
    fclose(f);
    sources.push_back(std::pair<const char*, ::size_t>(source.get(), size));
#else
    sources.push_back(std::pair<const char*, ::size_t>(src_renderer, src_renderer_size));
#endif
    cl::Program program(*context, sources);

    try {
        std::vector<cl::Device> devices;
        devices.push_back(device);
        program.build(devices, "-cl-nv-verbose -cl-fast-relaxed-math");

#ifdef DEBUG
        std::string log;
        program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &log);
        std::cout << log << std::endl;
#endif

    } catch (const cl::Error& e) {
        if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
            std::string log;
            program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &log);
            std::cerr << log << std::endl;
        }
        throw;
    }
    renderKernel.reset(new cl::Kernel(program, "render"));
    postProcessKernel.reset(new cl::Kernel(program, "postprocess"));

    initBlur();
}

void CLPlat::initBlur() {
    const cl_float blurSigma = 5;
    // Compensate for the ludicrously bright HDR lights by a low
    // constant factor on gaussian. As a bonus, we don't need any
    // brightpass filtering.
    const cl_float blurA = 1 / std::sqrt(2 * M_PI * blurSigma * blurSigma) * 0.4;
    const cl_float blurB = -1 / (2 * blurSigma * blurSigma);

    cl_float buffer[blurWindowD * blurWindowD];
    cl_float* window[blurWindowD];
    for (int i = 0; i < blurWindowD; i++) {
        window[i] = buffer + blurWindowD * i;
    }

    for (int i = 0; i <= blurWindowR; i++) {
        cl_float k = blurA * std::exp(blurB * (i * i));
        buffer[blurWindowR - i] = k;
        buffer[blurWindowR + i] = k;
    }

    size_t bufferSize = blurWindowD * sizeof(cl_float);
    postProcessData.reset(new cl::Buffer(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, bufferSize, buffer));
}
