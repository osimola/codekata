#include <SDL.h>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "clplat.hpp"
#include "spherecast.hpp"

#if defined(_MSC_VER)
#define MIN min
#define MAX max
#else
#define MIN std::min
#define MAX std::max
#endif

Vec3f spherePos(int k, int time_ms) {
    float angle = (float) k + 1;
    float phase = ((float)time_ms) / 4000.f * 2.f * M_PI;

    float x = 0.4f * angle * std::cos(angle + phase);
    float y = - 0.4f + 0.1f * k;
    float z = 0.4f * angle * std::sin(angle + phase);
    return Vec3f(x, y, z);
}

void mainLoop(SDL_Window* window, SphereCast &renderer) {
    bool quit = false;

    const int smoothLen = 128;
    Uint32 frameTimes[smoothLen];
    Uint32 maxSmoothDuration = 0;
    Uint32 minSmoothDuration = 99999999;

    SDL_Surface* windowSurf = SDL_GetWindowSurface(window);
    SDL_Surface* drawSurf = SDL_CreateRGBSurface(0, SphereCast::windowWidth, SphereCast::windowHeight,
                                                 32, 0, 0, 0, 0);

    Uint32 startTime = frameTimes[0] = SDL_GetTicks();
    int frame = 1;

    bool camUp = false;
    bool camDown = false;
    bool camLeft = false;
    bool camRight = false;

    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_PLUS:
                    renderer.addSphere();
                    break;
                case SDLK_MINUS:
                    renderer.removeSphere();
                    break;
                case SDLK_UP:
                    camUp = true;
                    break;
                case SDLK_DOWN:
                    camDown = true;
                    break;
                case SDLK_LEFT:
                    camLeft = true;
                    break;
                case SDLK_RIGHT:
                    camRight = true;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch(e.key.keysym.sym) {
                case SDLK_UP:
                    camUp = false;
                    break;
                case SDLK_DOWN:
                    camDown = false;
                    break;
                case SDLK_LEFT:
                    camLeft = false;
                    break;
                case SDLK_RIGHT:
                    camRight = false;
                    break;
                }
                break;
            }
        }

        if (camLeft)
            renderer.cameraW(SDL_GetTicks() - frameTimes[(frame - 1) % smoothLen]);
        if (camRight)
            renderer.cameraW(-(int32_t)(SDL_GetTicks() - frameTimes[(frame - 1) % smoothLen]));
        if (camUp)
            renderer.cameraY(SDL_GetTicks() - frameTimes[(frame - 1) % smoothLen]);
        if (camDown)
            renderer.cameraY(-(int32_t)(SDL_GetTicks() - frameTimes[(frame - 1) % smoothLen]));

        renderer.render((uint32_t*) drawSurf->pixels, SDL_GetTicks() - startTime);
        SDL_BlitSurface(drawSurf, NULL, windowSurf, NULL);
        SDL_UpdateWindowSurface(window);

        frame++;
        frameTimes[frame % smoothLen] = SDL_GetTicks();
        if (frame >= smoothLen) {
            Uint32 timeDelta = frameTimes[frame % smoothLen] -
                frameTimes[(frame + 1) % smoothLen];
            minSmoothDuration = MIN(minSmoothDuration, timeDelta);
            maxSmoothDuration = MAX(maxSmoothDuration, timeDelta);
        }

    }

    std::cout << "Frames: " << frame << std::endl;
    std::cout << "Frames per second: average "
              << ((float) (frame * 1000) / (SDL_GetTicks() - startTime));
    if (frame > smoothLen) {
        std::cout << " min " << ((float)((smoothLen - 1) * 1000) / maxSmoothDuration)
                  << " max " << ((float)((smoothLen -1) * 1000) / minSmoothDuration);
    }
    std::cout << std::endl;

    SDL_FreeSurface(drawSurf);
}

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    atexit(SDL_Quit);

    SDL_Window* window = SDL_CreateWindow("SphereCast",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SphereCast::windowWidth, SphereCast::windowHeight, SDL_WINDOW_SHOWN);

    try {
        SphereCast renderer(spherePos);

        mainLoop(window, renderer);
    } catch (const cl::Error &e) {
        std::cerr << " err(): " << e.err() << std::endl;
        std::cerr << " what(): " << e.what() << std::endl;
    }

    SDL_DestroyWindow(window);
}
