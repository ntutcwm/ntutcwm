#ifndef SISO2_FAST_ATAN2F_H
#define SISO2_FAST_ATAN2F_H

#ifdef __cplusplus
extern "C" {
#endif

float fast_atan2f(float y, float x);

#ifdef __cplusplus
#include <complex>
static inline float fast_atan2c(std::complex<float> z) { return fast_atan2f(z.imag(), z.real()); }
}
#endif

#endif