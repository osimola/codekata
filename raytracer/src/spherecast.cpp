#include "spherecast.hpp"
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cfloat>
#include <utility>
#include <cstdlib>

static const float cameraStepY(0.0015f);
static const float cameraStepW((float)M_PI / 2800);
static const float presetFov(1);
static const Vec3f presetAmbient(0.05f, 0.05f, 0.05f);

struct CameraPos {
    float alpha;
    float r;
    float y;

    CameraPos(float alpha, float r, float y):
        alpha(alpha), r(r), y(y) {}

    CameraPos(const CameraPos& other):
        alpha(other.alpha), r(other.r), y(other.y) {}
};

static Camera cameraForPos(const CameraPos& cameraPos);

struct CLScene;
struct CLSphere;
struct CLLight;

class Scene {
public:
    Camera camera;
    CameraPos cameraPos;
    std::vector<Sphere> spheres;
    std::vector<Light> lights;
    Vec3f ambient;

    Scene(const CameraPos& cameraPos, const Vec3f& ambient,
          const std::vector<Sphere>& spheres,
          const std::vector<Light>& lights):
        camera(cameraForPos(cameraPos)),
        cameraPos(cameraPos),
        spheres(spheres),
        lights(lights),
        ambient(ambient) {
    }

    CLScene* toCLScene();
    std::vector<CLSphere> toCLSpheres();
    std::vector<CLLight> toCLLights();
};

struct CLScene {
    cl_float4 camera[4];
    cl_float4 ambient;
    cl_int2 viewportSize;
    cl_int sphereCount;
    cl_int lightCount;
};

struct CLSphere {
    cl_float4 center;
    cl_float4 diffuse;
    cl_float r;
    cl_float r2;
    cl_float gloss;
    cl_float padding;
};

struct CLLight {
    cl_float4 center;
    cl_float r;
    cl_float r2;
    cl_float intensity;
    cl_float padding;
};

CLScene* Scene::toCLScene() {
    CLScene* s = new CLScene();
    for (size_t i = 0; i < 4; i++)
        s->camera[i] = camera.buf[i];
    s->ambient = ambient.toCL();
    s->sphereCount = (cl_int)spheres.size();
    s->lightCount = (cl_int)lights.size();
    s->viewportSize.s[0] = SphereCast::windowWidth;
    s->viewportSize.s[1] = SphereCast::windowHeight;
    return s;
}

std::vector<CLSphere> Scene::toCLSpheres() {
    std::vector<CLSphere> v(spheres.size());
    for (size_t i = 0; i < spheres.size(); i++) {
        v[i].center = spheres[i].center.toCL();
        v[i].diffuse = spheres[i].diffuse.toCL();
        v[i].r = spheres[i].r;
        v[i].r2 = spheres[i].r2;
        v[i].gloss = spheres[i].gloss;
    }
    return v;
}

std::vector<CLLight> Scene::toCLLights() {
    std::vector<CLLight> v(lights.size());
    for (size_t i = 0; i < lights.size(); i++) {
        v[i].center =  lights[i].center.toCL();
        v[i].r = lights[i].r;
        v[i].r2 = lights[i].r2;
        v[i].intensity = lights[i].intensity;
    }
    return v;
}


SphereCast::SphereCast(SpherePositionFunc spherePos) : spherePos(spherePos) {
    CameraPos cameraPos(1.f, 8.f, -1.f);

    std::srand(0); // Same sequence every time

    std::vector<Sphere> spheres;

    spheres.push_back(Sphere(Vec3f(0.f, 0.f, 0.f), 0.7f, Vec3f(0.f, 0.8f, 0.f), 0.6f));
    spheres.push_back(Sphere(Vec3f(0.f, -500.f, 0.f), 498.5f, Vec3f(0.8f, 0.f, 0.f), 0.1f));
    spheres.push_back(Sphere(Vec3f(1.f, -0.4f, 0.f), 0.2f, Vec3f(0.f, 0.f, 0.8f), 0.5f));

    std::vector<Light> lights;
    lights.push_back(Light(Vec3f(30, 30, 0), 3, 30 * 25));
    lights.push_back(Light(Vec3f(20, 8, 20), 8, 30 * 15));
    scene = new Scene(cameraPos, presetAmbient, spheres, lights);

    plat.reset(new CLPlat());
}

SphereCast::~SphereCast() {
    delete scene;
}

static float randFloat(float max) {
    float r = (float) std::rand() / RAND_MAX;
    return r * max;
}

void SphereCast::addSphere() {
	Sphere sphere(Vec3f(0.f, 0.f, 0.f), 0.2f,
                  Vec3f(randFloat(0.2f), randFloat(0.2f), randFloat(0.2f)), randFloat(1.0f));
    scene->spheres.push_back(sphere);
}

void SphereCast::removeSphere() {
    // The 2 fixed spheres
    if (scene->spheres.size() > 2) {
        scene->spheres.pop_back();
    }
}

static Camera cameraForPos(const CameraPos &cameraPos) {
    return Camera::lookingAt(Vec3f(cos(cameraPos.alpha) * cameraPos.r,
                                   cameraPos.y,
                                   sin(cameraPos.alpha) * cameraPos.r),
                             Vec3f(0, 0, 0), presetFov);
}

void SphereCast::cameraY(int32_t timeDelta) {
    scene->cameraPos.y += cameraStepY * (float)timeDelta;
    scene->camera = cameraForPos(scene->cameraPos);
}

void SphereCast::cameraW(int32_t timeDelta) {
    scene->cameraPos.alpha += cameraStepW * (float) timeDelta;
    scene->camera = cameraForPos(scene->cameraPos);
}

static int globalSizeFor(int realSize, int groupSize) {
    return (((realSize - 1) / groupSize) + 1) * groupSize;
}

void SphereCast::render(uint32_t* buffer, uint32_t time) {
    for (size_t i = 2; i < scene->spheres.size(); i++) {
        scene->spheres[i].center = spherePos((cl_int)i, time);
    }

    // For some reason or other, allocating buffers separately for
    // each frame doesn't seem to have much performance impact. Async
    // copying of the buffers on the other hand seems to have some.

    // Mysteriously, on my machine READ_WRITE seems to be faster than
    // READ_ONLY. And copying scene to local memory before rendering
    // seems to slow down the rendering.

    std::unique_ptr<CLScene> clScene(scene->toCLScene());
    cl::Buffer bScene(*plat->context, CL_MEM_READ_WRITE, sizeof(CLScene));
    plat->queue->enqueueWriteBuffer(bScene, false, 0, sizeof(CLScene), clScene.get());

    std::vector<CLSphere> clSpheres = scene->toCLSpheres();
    cl::Buffer bSpheres(*plat->context, CL_MEM_READ_WRITE, sizeof(CLSphere[1]) * scene->spheres.size());
    plat->queue->enqueueWriteBuffer(bSpheres, false, 0, sizeof(CLSphere[1]) * scene->spheres.size(), clSpheres.data());

    std::vector<CLLight> clLights = scene->toCLLights();
    cl::Buffer bLights(*plat->context, CL_MEM_READ_WRITE, sizeof(CLLight[1]) * scene->lights.size());
    plat->queue->enqueueWriteBuffer(bLights, false, 0, sizeof(CLLight[1]) * scene->lights.size(), clLights.data());

    const size_t pixelCount = clScene->viewportSize.s[0] * clScene->viewportSize.s[1];
    cl::Image2D rendered(*plat->context, CL_MEM_READ_WRITE, cl::ImageFormat(CL_RGBA, CL_HALF_FLOAT),
                       clScene->viewportSize.s[0], clScene->viewportSize.s[1]);
    cl::Buffer output(*plat->context, CL_MEM_WRITE_ONLY, pixelCount * sizeof(cl_int));

    plat->renderKernel->setArg(0, bScene);
    plat->renderKernel->setArg(1, bSpheres);
    plat->renderKernel->setArg(2, bLights);
    plat->renderKernel->setArg(3, rendered);

    plat->queue->enqueueBarrier();
    plat->queue->enqueueNDRangeKernel(*plat->renderKernel, cl::NullRange,
                                      cl::NDRange(globalSizeFor(SphereCast::windowWidth, CLPlat::renderGroupSize),
                                                  globalSizeFor(SphereCast::windowHeight, CLPlat::renderGroupSize)),
                                      cl::NDRange(CLPlat::renderGroupSize, CLPlat::renderGroupSize));

    plat->postProcessKernel->setArg(0, clScene->viewportSize);
    plat->postProcessKernel->setArg(1, CLPlat::blurWindowR);
    plat->postProcessKernel->setArg(2, *plat->postProcessData);
    plat->postProcessKernel->setArg(3, cl::__local(CLPlat::postProcessBuffer1Size));
    plat->postProcessKernel->setArg(4, cl::__local(CLPlat::postProcessBuffer2Size));
    plat->postProcessKernel->setArg(5, cl::__local(CLPlat::postProcessBuffer3Size));
    plat->postProcessKernel->setArg(6, rendered);
    plat->postProcessKernel->setArg(7, output);
    plat->queue->enqueueBarrier();
    plat->queue->enqueueNDRangeKernel(*plat->postProcessKernel, cl::NullRange,
                                      cl::NDRange(globalSizeFor(SphereCast::windowWidth,
                                                                CLPlat::postGroupX),
                                                  globalSizeFor(SphereCast::windowHeight,
                                                                CLPlat::postGroupY)),
                                      cl::NDRange(CLPlat::postGroupX, CLPlat::postGroupY));


    plat->queue->enqueueReadBuffer(output, true, 0, pixelCount * sizeof(cl_int), buffer);
}


Camera Camera::lookingAt(const Vec3f& location, const Vec3f& target, float fov) {
    auto direction = (target - location).normalized_accurate();

    // fov is radians from left to right window edge
    // Note: Assumes that width < height
    float w = std::tan(fov / 2);
    float aspect = (float) SphereCast::windowWidth / SphereCast::windowHeight;

    // Note: Do not point the camera straight up or down
    auto right = direction.cross(Vec3f(0, 1, 0)).normalized_accurate();
    auto up = right.cross(direction);

    return Camera(Ray(location, direction), up * (w / aspect), right * w);
}

Camera::Camera(const Ray& center, const Vec3f& up, const Vec3f& right) {
    buf[0] = center.origin.toCL();
    buf[1] = center.direction.toCL();
    buf[2] = right.toCL();
    buf[3] = up.toCL();
}
