# Raytracer

Because everyone has to have one, but this one is mine. Written with
OpenCL and SDL, and supports only one geometric primitive: the sphere.

Extras include postprocessing (because there is not enough bloom in
the world as it is) and soft shadows using some very approximate
math. The end result looks something like this:

<img src="http://ojs.kapsi.fi/spherecast2.png">

The camera can be moved around with arrow keys, and extra spheres can
be added and removed with + and - keys.


### Compiling and running:

Install Ninja, SDL2 development package, an OpenCL SDK (developed with
NVidia one) and Meson (http://mesonbuild.com/, also available through
PyPI). Create a separate build directory under main directory. Run the
following commands in the build directory:

```
CXXFLAGS=-O3 meson.py ..
ninja
```

Preprocessor macros `USE_SSE` and `DEBUG` are supported. One uses SSE
for vector math and other prints debug information, including OpenCL
compiler messages.

The compiled binary, `spherecast` will appear under `src/`

### Windows

There are a number of references to `_MSC_VER` in the code base. Well,
this code also works with Visual Studio 2012. The Visual Studio
project is not included because that would involve a copy of SDL, but
you can create one. Just add the correct dependencies to SDL and
OpenCL SDKs, copy `renderer.cl` to output directory in post build
action and remember to force rebuild when you change it.
