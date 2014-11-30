#ifndef LINALG_HPP
#define LINALG_HPP

#ifndef USE_SSE
#include "linalg_plain.hpp"
#else
#include "linalg_sse.hpp"
#endif

#endif
