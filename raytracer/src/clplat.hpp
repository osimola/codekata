#ifndef CLPLAT_HPP
#define CLPLAT_HPP

// Using OpenCL 1.1 because nVidia
#include <CL/cl.h>
#undef CL_VERSION_1_2
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#define __CL_ENABLE_EXCEPTIONS

// cl.hpp line 3878 has deprecation macro in the wrong place. So, let's hack around it.
#undef CL_EXT_PREFIX__VERSION_1_1_DEPRECATED
#define CL_EXT_PREFIX__VERSION_1_1_DEPRECATED
#undef CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
#define CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
#include <CL/cl.hpp>

#include <memory>

class CLPlat {
private:
    std::unique_ptr<cl::Platform> platform;
    std::unique_ptr<cl::Device> device;

    void initProgram(const cl::Device& device);
    void initBlur();

public:
    std::unique_ptr<cl::Context> context;

    std::unique_ptr<cl::CommandQueue> queue;
    std::unique_ptr<cl::Kernel> renderKernel;
    std::unique_ptr<cl::Kernel> postProcessKernel;
    std::unique_ptr<cl::Buffer> postProcessData;
    size_t maxGroupSize;

    // 16 x 16, in order to stay under register and memory limits
    static const int renderGroupSize = 16;

    // 32 x 16 for less redundant memory copying
    static const int postGroupX = 32;
    static const int postGroupY = 16;

    static const cl_int blurWindowR = 11;
    static const cl_int blurWindowD = 2 * blurWindowR + 1;
    static const int postProcessBuffer1Size = blurWindowD * sizeof(cl_float);
    static const int postProcessBuffer2Size =
        (2 * blurWindowR + postGroupX) * (2 * blurWindowR + postGroupY) *
        sizeof(cl_float4);
    static const int postProcessBuffer3Size =
        (postGroupX + 2 * blurWindowR) * postGroupY * sizeof(cl_float4);

    CLPlat();
    void printInfo(std::ostream &out);
};

#endif
