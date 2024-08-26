#ifndef SISO2_CONVOLUTIONAL_CODEC_H
#define SISO2_CONVOLUTIONAL_CODEC_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>

#ifdef __ARM_NEON
#include <sse2neon.h>
#else
#include <xmmintrin.h>
#endif

namespace SISO2
{

struct viterbi_state
{
    unsigned long path;	// Decoded path to this state
    long metric;		// Cumulative metric to this state

    // vector implementation
    __m128i pp;		// Decoded path to this state
    __m128i mm;		// Cumulative metric to this state
};

union branchtab27
{
    unsigned char c[32];
    __m128i v[2];
};

class ConvolutionalCodec
{
    int mettab[2][256];

    static const unsigned char d_Partab[];
    branchtab27 Branchtab27_sse2[2];

    void gen_mettab();

    void viterbi_chunks_init(struct viterbi_state* state) const;
    void viterbi_butterfly2(const unsigned char *symbols, struct viterbi_state *state0, struct viterbi_state *state1) const;
    unsigned char viterbi_get_output(struct viterbi_state *state, unsigned char *outbuf) const;

    void decode_tb(const uint8_t* in_ptr, uint8_t* out_ptr, const int input_bit_num_tb, const int output_byte_num) const;

    void viterbi_chunks_init_sse2(__m128i *mm0, __m128i *pp0, unsigned char* mmresult, unsigned char ppresult[][64]);
    void viterbi_butterfly2_sse2(const unsigned char *symbols, __m128i m0[], __m128i m1[], __m128i p0[], __m128i p1[]);
    unsigned char viterbi_get_output_sse2(__m128i *mm0, __m128i *pp0, int& store_pos, unsigned char* mmresult, unsigned char ppresult[][64], unsigned char *outbuf);

public:
    ConvolutionalCodec() {
        gen_mettab();
    }

    std::vector<uint8_t> encode(const std::vector<uint8_t>& in) const;
    std::vector<uint8_t> decode(const std::vector<uint8_t>& in) const;
    std::vector<uint8_t> decode(const std::vector<float>& in) const;
    std::vector<uint8_t> decode_hard_SSE2(const std::vector<uint8_t>& in);

    size_t decoded_size(size_t encoded_size) const { return encoded_size/2; }
};

}

#endif

