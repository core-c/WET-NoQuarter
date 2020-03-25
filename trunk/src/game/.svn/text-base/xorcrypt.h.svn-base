#ifdef HW_BAN

#ifndef __XORCRYPT_H__
#define __XORCRYPT_H__

//uint32_t is not support on windows so we need a little trick
#ifdef WIN32
typedef unsigned __int32 uint32_t;
#else
#include <stdint.h>
#endif // WIN32

#define XOR_OK     0
#define XOR_ERROR -1

struct xor128_state {
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;
};

void xor128_init(struct xor128_state* state, uint32_t x, uint32_t y, uint32_t z, uint32_t w);
uint32_t xor128_rand(struct xor128_state* state);
uint32_t xor128_multirand(struct xor128_state* state, uint32_t c);
void xor_crypt_init(struct xor128_state* state, uint32_t x, uint32_t y, uint32_t z, uint32_t w);
int xor_encrypt(struct xor128_state* state, const unsigned char* data, int dataLength, unsigned char* buffer, int bufferLength);
int xor_decrypt(struct xor128_state* state, const unsigned char* data, int dataLength, unsigned char* buffer, int bufferLength);

#endif

#endif // HW_BAN
