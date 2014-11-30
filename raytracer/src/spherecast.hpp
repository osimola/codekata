#ifndef SPHERECAST_H
#define SPHERECAST_H

#include "clplat.hpp"
#include "linalg.hpp"
#include <cstdint>
#include <utility>
#include <memory>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Scene;
class Ray;

typedef Vec3f(*SpherePositionFunc)(int k, int time_ms);

class CLPlat;

class SphereCast {
public:
    SphereCast(SpherePositionFunc spherePos);
    virtual ~SphereCast();
    const static int windowWidth = 640;
    const static int windowHeight = 480;

    void render(uint32_t* buffer, uint32_t time);

    void addSphere();
    void removeSphere();

    void cameraY(int32_t timeDelta);
    void cameraW(int32_t timeDelta);

private:
    Scene *scene;
    std::unique_ptr<CLPlat> plat;
    const SpherePositionFunc spherePos;
    Vec3f renderRay(const Ray& ray);
};

class Ray {
public:
    Ray(const Vec3f& origin, const Vec3f& direction):
        origin(origin), direction(direction) {}
    Vec3f origin;
    Vec3f direction;
};

class Camera {
public:
    static Camera lookingAt(const Vec3f& location, const Vec3f& target, float fov);
    Camera(const Ray& center, const Vec3f& up, const Vec3f& right);
    cl_float4 buf[4];
};

class SphereBase {
protected:
    SphereBase(const Vec3f& center, float r):
        center(center), r(r), r2(r * r) {}

public:
    Vec3f center;
    const float r;
    const float r2;

    std::pair<float, float> intersectDistance(const Ray& ray) const;
};

class Sphere: public SphereBase {
public:
    Sphere(const Vec3f& center, float r, const Vec3f& diffuse, float gloss):
        SphereBase(center, r), diffuse(diffuse), gloss(gloss) {}

    Vec3f diffuse;
    float gloss;
};

class Light: public SphereBase {
public:
    Light(const Vec3f& center, float r, float intensity):
        SphereBase(center, r),
        intensity(intensity) {}

    float intensity;
};

#endif
