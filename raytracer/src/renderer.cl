struct CLScene {
    float4 camera[4];
    float4 ambient;
    int2 viewportSize;
    int sphereCount;
    int lightCount;
};

struct CLSphere {
    float4 center;
    float4 diffuse;
    float r;
    float r2;
    float gloss;
};

struct CLLight {
    float4 center;
    float r;
    float r2;
    float intensity;
};

float2 intersectDistsToSphere(const float4 rayOrigin, const float4 rayDirection,
                              const float4 center, const float r2);
float sigmoid(float x);

__kernel void render(__global const struct CLScene* scene,
                     __global const struct CLSphere* spheres,
                     __global const struct CLLight* lights,
                     __global float4* output)
{
    const int pixelX = get_global_id(0);
    const int pixelY = get_global_id(1);
    const int2 vpSize = scene->viewportSize;

    if (pixelX >= vpSize.x || pixelY >= vpSize.y)
        return;

    const float2 clipCoords = (float2)
        ((float) pixelX / (float) scene->viewportSize.x,
         (float) -pixelY / (float) scene->viewportSize.y)
        + (float2)(-0.5, 0.5);

    float4 rayOrigin = scene->camera[0];
    float4 rayDirection = normalize(scene->camera[1] +
                                    clipCoords.x * scene->camera[2] +
                                    clipCoords.y * scene->camera[3]);

    float4 color = (float4) (0, 0, 0, 0);
    float factor = 1;

    while (factor > 0.001) {
        // Find sphere to show
        float hitDist = FLT_MAX;
        int hitIdx = -1;
        for (size_t i = 0; i < scene->sphereCount; i++) {
            float2 dists =
                intersectDistsToSphere(rayOrigin, rayDirection,
                                       spheres[i].center, spheres[i].r2);
            if (dists.x < hitDist && dists.x > 0) {
                hitIdx = i;
                hitDist = dists.x;
            }
        }

        for (size_t i = 0; i < scene->lightCount; i++) {
            const __global struct CLLight* light = lights + i;
            // Now, a spherical light source!
            float2 directDists =
                intersectDistsToSphere(rayOrigin, rayDirection,
                                       light->center, light->r2);
            if (directDists.x > 0 && directDists.x < hitDist &&
                directDists.y < (light->r2)) {
                    // .y is second power of distance to light center perpendicular to ray direction
                float intensity = light->intensity / light->r2;
                intensity *= factor;
                color += (float4) (intensity, intensity, intensity, 0);
            }
        }

        if (hitIdx < 0) {
            factor = 0;
            break;
        }

        const __global struct CLSphere* hitSphere = spheres + hitIdx;
        color += hitSphere->diffuse * scene->ambient * factor;
        float4 intersectPoint = rayOrigin + rayDirection * hitDist;
        float4 intersectSurfNormal = fast_normalize(intersectPoint - hitSphere->center);
        // Tweak the intersection a bit so that sphere will
        // self-shadow even with rounding errors.
        intersectPoint += intersectSurfNormal * FLT_EPSILON * 128;

        for (size_t i = 0; i < scene->lightCount; i++) {
            const __global struct CLLight* light = lights + i;

            // Direct illumination: is the intersected surface lit by the
            // light? Extremely diffuse lighting: surface absorbs from and
            // emits to every direction at exactly same intensity.

            float4 lightRel = light->center - intersectPoint;
            float4 lightDirection = fast_normalize(lightRel);

            float lightDist = dot(lightDirection, lightRel);
            float lightRRad = asin(light->r / lightDist);

            float visibility = 1;

            for (size_t s = 0; s < scene->sphereCount; s++) {
                __global const struct CLSphere* sphere = spheres + s;
                float4 sphereRelCenter = sphere->center - intersectPoint;
                float4 sphereDirection = fast_normalize(sphereRelCenter);
                float sphereDist = dot(sphereRelCenter, sphereDirection);
                float cosAngle = dot(lightDirection, sphereDirection);
                float angle = acos(cosAngle);
                float sphereRRad = asin(sphere->r / sphereDist);

                // Totally cheating here. I *did* calculate the exact
                // intersection areas, but approximating with
                // sigmoid-ish of overlapping lines looks surprisingly
                // good. And of course a sphere that has already been
                // shadowed casts another shadow :)

                if (angle < sphereRRad + lightRRad) {
                    if (sphereRRad < lightRRad) {
                        if (angle < lightRRad - sphereRRad) {
                            visibility *= sigmoid(1 - sphereRRad / lightRRad);
                        } else {
                            visibility *= sigmoid((lightRRad + angle - sphereRRad) / (2 * lightRRad));
                        }
                    } else {
                        if (angle > sphereRRad - lightRRad) {
                            visibility *= sigmoid((lightRRad + angle - sphereRRad) / (2 * lightRRad));
                        } else {
                            visibility = 0;
                        }
                    }
                }
            }

            color += light->intensity * hitSphere->diffuse *
                visibility * factor / (lightDist * lightDist);
        }

        factor *= hitSphere->gloss;
        rayOrigin = intersectPoint;
        rayDirection = intersectSurfNormal * (dot(intersectSurfNormal, rayDirection) * -2) + rayDirection;
    }

    uint outIdx = pixelY * vpSize.x + pixelX;
    output[outIdx] = color;
}

float2 intersectDistsToSphere(const float4 rayOrigin, const float4 rayDirection,
                              const float4 center, const float r2) {
    float4 relCenter = center - rayOrigin;
    float distAlongRay = dot(relCenter, rayDirection);
    if (distAlongRay <= 0)
        return -1;

    float4 p = rayDirection * distAlongRay - relCenter;
    float c2 = dot(p, p);
    if (c2 > r2)
        return (float2) (-1, -1);
    else
        return (float2) (distAlongRay - sqrt(r2 - c2), c2);
}

float sigmoid(float x) {
    return 0.5 * (1 + sin(M_PI * (x-0.5)));
}

void toLocal(__local float* target, const __global float* source, int count) {
    const int threadCount = get_local_size(0) * get_local_size(1);
    for (int idx = get_local_size(0) * get_local_id(1) + get_local_id(0); idx < count; idx += threadCount) {
        if (idx < count)
            target[idx] = source[idx];
    }
}

void toLocalRect(__local float4* target, const __global float4* source, int2 padding, int2 vpSize) {
    // Making the wild assumption that padding < group size

    int rowLen = 2 * padding.x + get_local_size(0);

    int lid0 = get_local_id(0);
    int lid1 = get_local_id(1);
    int gid0 = get_global_id(1);
    int gid1 = get_global_id(1);

    const int windowSize = rowLen * (2 * padding.y + get_local_size(1));
    const int threadCount = get_local_size(0) * get_local_size(1);
    for (int idx = get_local_size(0) * lid1 + lid0; idx < windowSize; idx += threadCount) {
        if (idx < windowSize)
            target[idx] = (float4)(0, 0, 0, 0);
    }

    int srcLeftXOffset = max(0, (int)get_global_id(0) - padding.y);
    int srcRightXOffset = min(vpSize.x - 1, (int) get_global_id(0) + (int) get_local_size(0));

    int srcTopYOffset = vpSize.x * max(0, (int)get_global_id(1) - padding.y);
    int srcCenterYOffset = vpSize.x * get_global_id(1);
    int srcBottomYOffset = vpSize.x * min(vpSize.y - 1, (int)get_global_id(1) + (int) get_local_size(1));

    int tgtTopYOffset = rowLen * lid1;
    int tgtCenterYOffset = rowLen * (lid1 + padding.y);
    int tgtBottomYOffset = rowLen * (lid1 + padding.y + get_local_size(1));

    int tgtCenterXOffset = lid0 + padding.x;
    int tgtRightXOffset = padding.x + get_local_size(0) + lid0;

    if (lid1 < padding.y) {
        // Top
        target[tgtTopYOffset + tgtCenterXOffset] = source[srcTopYOffset + get_global_id(0)];
        // Bottom
        target[tgtBottomYOffset + tgtCenterXOffset] = source[srcBottomYOffset + get_global_id(0)];

        if (lid0 < padding.x) {
            // Top Left
            target[tgtTopYOffset + lid0] = source[srcTopYOffset + srcLeftXOffset];
            // Top Right
            target[tgtTopYOffset + tgtRightXOffset] = source[srcTopYOffset + srcRightXOffset];

            // Bottom Left
            target[tgtBottomYOffset + lid0] = source[srcBottomYOffset + srcLeftXOffset];
            // Bottom Right
            target[tgtBottomYOffset + tgtRightXOffset] = source[srcBottomYOffset + srcRightXOffset];
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    // Center
    target[tgtCenterYOffset + tgtCenterXOffset] = source[srcCenterYOffset + get_global_id(0)];

    if (lid0 < padding.x) {
        // Left
        target[tgtCenterYOffset + lid0] = source[srcCenterYOffset + srcLeftXOffset];
        // Right
        target[tgtCenterYOffset + tgtRightXOffset] = source[srcCenterYOffset + srcRightXOffset];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

uint toPixel(float4 rgb) {
    uchar4 chars = convert_uchar4_sat(rgb * 255);
    uint pixel = (chars.x << 16) | (chars.y << 8) | chars.z;
    return pixel;
}

__kernel void postprocess(int2 vpSize,
                          int blurWindowR,
                          __global float* blurTableGlobal,
                          __local float* blurTable,
                          __local float4* window1,
                          __local float4* window2,
                          __global float4* rendered,
                          __write_only __global int* output)
{
    const int pixelX = get_global_id(0);
    const int pixelY = get_global_id(1);

    const int blurWindowD = 2 * blurWindowR + 1;
    const int blurBufferSize = blurWindowD * blurWindowD;

    toLocal(blurTable, blurTableGlobal, blurBufferSize);
    toLocalRect(window1, rendered, (int2)(blurWindowR, blurWindowR), vpSize);

    if (pixelX >= vpSize.x || pixelY >= vpSize.y)
        return;

    const int windowRowLen = get_local_size(0) + 2 * blurWindowR;

    // Vertical convolution first because the window is wider than its height
    for (int x = get_local_id(0); x < windowRowLen; x += get_local_size(0)) {
        const int idx = get_local_id(1) * windowRowLen + x;
        window2[idx] = 0;
        const int last = idx + blurWindowD * windowRowLen;
        int blurIdx = 0;
        for (int win1Idx = idx; win1Idx < last; win1Idx += windowRowLen)
            window2[idx] += window1[win1Idx] * blurTable[blurIdx++];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    const int pixelIdx = pixelY * vpSize.x + pixelX;
    float4 pixel = rendered[pixelIdx];
    // float4 pixel = (float4)(0, 0, 0, 0);

    int windowIdx = get_local_id(1) * windowRowLen + get_local_id(0);
    for (int dx = 0; dx < blurWindowD; dx++) {
        pixel += window2[windowIdx++] * blurTable[dx];
    }

    output[pixelIdx] = toPixel(pixel);
}
