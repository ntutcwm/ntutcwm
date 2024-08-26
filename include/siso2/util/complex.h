#ifndef SISO2_COMPLEX_H
#define SISO2_COMPLEX_H

#include <complex>
#include <string>
#include <stdint.h>

#ifndef __HLS__

#include <fmt/format.h>

template<typename T> struct fmt::formatter<std::complex<T>>: fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const std::complex<T>& c, FormatContext& ctx) -> decltype(ctx.out()) {
        format_to(ctx.out(), "({}, {})", c.real(), c.imag());
        return ctx.out();
    }
};

#endif

template<typename T, typename U>
std::complex<T> complex_cast(std::complex<U> c)
{
    return std::complex<T>(c.real(), c.imag());
}

#endif

