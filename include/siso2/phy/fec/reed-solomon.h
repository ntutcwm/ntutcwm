#ifndef SISO2_REED_SOLOMON_H
#define SISO2_REED_SOLOMON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <utility>
#include <optional>

namespace SISO2
{

class ReedSolomon
{
    const size_t d_n        = 255;
    const size_t d_k        = 239;
    const size_t d_p        = 2;
    const size_t d_m        = 8;
    const size_t d_gfpoly   = 285;
    const size_t d_t        = 8;
    //maximum number of elements in the GF(p^m)
    const size_t d_q = (1<<d_m); /*powl(d_p, d_m)*/

    std::vector<uint8_t> d_gf_exp;
    std::vector<uint8_t> d_gf_log;

    int gf_add(int a, int b)     const { return a^b; }
    int gf_exp(int a)            const { return d_gf_exp[a % d_n]; }
    int gf_mul(int a, int b)     const { return (a==0 || b==0) ? 0 : gf_exp(d_gf_log[a] + d_gf_log[b]); }
    int gf_div(int a, int b)     const { return (a==0 || b==0) ? 0 : gf_exp(d_n + d_gf_log[a] - d_gf_log[b]); }
    int gf_log(int a)            const { return d_gf_log[a % d_n]; }
    int gf_pow(int a, int power) const { return (a==0) ? 0 : gf_exp(d_n + d_gf_log[a] + power); }

    std::vector<uint8_t> d_l;
    std::vector<uint8_t> d_g;

    int gf_lpow(int power) { return d_l[power % d_n]; }

    void rs_encode(uint8_t* data_in, uint8_t* parity) const;
    int rs_decode(uint8_t* data, uint8_t* eras, const int no_eras) const;

public:
    ReedSolomon();
    std::vector<uint8_t> encode(const std::vector<uint8_t>& in) const;
    std::optional<std::vector<uint8_t>> decode(const std::vector<uint8_t>& in) const;

    size_t encoded_size(size_t decoded_size) const { return decoded_size/d_k*d_n + (decoded_size%d_k) + ((decoded_size%d_k)?(d_n-d_k):0); }
    size_t decoded_size(size_t encoded_size) const { return encoded_size/d_n*d_k + (encoded_size%d_n) - ((encoded_size%d_n)?(d_n-d_k):0); }
};

}

#endif