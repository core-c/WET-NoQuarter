#ifdef HW_BAN
/**
 *
 * Xor Crypt
 *
 * (c) 2011 J. M. Bogaard
 *
 * Feel free to use this code, give me a drink when you meet me, but don't remove this message.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "base64.h"
#include "crc32.h"

#include "xorcrypt.h"

#define XOR_MULTIRAND_COUNT 3

#define XOR_CHECKSUM_BYTES  4

/*void dumpBuffer(char* buffer, int bufferSize) {
	int i = 0;
	unsigned char c;

	while (i < bufferSize) {
		c = buffer[i];

		printf("%.2hX ", c);

		if (((i+1) % 50) == 0)
			printf("\n");

		i++;
	}

    if (((i+1) % 50) != 0)
        printf("\n");
}*/

void xor128_init(struct xor128_state* state, uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
	state->x = x;
	state->y = y;
	state->z = z;
	state->w = w;
}

//this is a xorshift PRNG created by George Marsaglia
//http://en.wikipedia.org/wiki/Xorshift
uint32_t xor128_rand(struct xor128_state* state) {
	uint32_t t;

	t = state->x ^ (state->x << 11);
	state->x = state->y; state->y = state->z; state->z = state->w;
	return state->w = state->w ^ (state->w >> 19) ^ (t ^ (t >> 8));
}

uint32_t xor128_multirand(struct xor128_state* state, uint32_t c) {
	unsigned int i = 0;
	uint32_t r = 0;

	while (i < c) {
		r = xor128_rand(state);

		i++;
	}

	return r;
}

void xor_crypt_init(struct xor128_state* state, uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
	xor128_init(state, x, y, z, w);

	x = xor128_multirand(state, XOR_MULTIRAND_COUNT);
	y = xor128_multirand(state, XOR_MULTIRAND_COUNT);
	z = xor128_multirand(state, XOR_MULTIRAND_COUNT);
	w = xor128_multirand(state, XOR_MULTIRAND_COUNT);

	xor128_init(state, x, y, z, w);
}

int xor_encrypt(struct xor128_state* state, const unsigned char* data, int dataLength, unsigned char* buffer, int bufferLength) {
	int   p          = 0;
	int   bufferP    = 0;

	char* xorData    = malloc(dataLength + 5);
	int   xorKey     = 0;
	char* xorKeyData = 0;
	int   xorKeyP    = 4;

	unsigned int checksum = 0;

	//XOR the data with a random key. This key is always the same when the same init values are used
	while (p < dataLength) {
		if (xorKeyP == 4) {
			xorKey = xor128_multirand(state, XOR_MULTIRAND_COUNT);
			xorKeyData = (char*)&xorKey;

			xorKeyP = 0;
		}

		xorData[p] = data[p] ^ xorKeyData[xorKeyP];

		p++;
		xorKeyP++;
	}

	//Calculate checksum from the ORIGINAL data
	checksum = calculateCRC32((char*) data, dataLength);

	xorData[p++] = (checksum >>  0) & 0xFF;
	xorData[p++] = (checksum >>  8) & 0xFF;
	xorData[p++] = (checksum >> 16) & 0xFF;
	xorData[p++] = (checksum >> 24) & 0xFF;

	//null append
	xorData[p++] = 0;

	//is the buffer big enough to pack the data?
	if (p > bufferLength) {
		free(xorData);

		return XOR_ERROR;
	}

	//Pack the data in a base64 container so its human readable
	bufferP = 0;
	p       = (dataLength + XOR_CHECKSUM_BYTES);

	while (p > 0) {
		if ((bufferP + 4) > bufferLength) {
			free(xorData);

			return XOR_ERROR;
		}

		base64_encode((unsigned char *) xorData + ((dataLength + XOR_CHECKSUM_BYTES) - p), buffer + bufferP, 4);

		p       -= 3;
		bufferP += 4;
	}

	if (p < 0) { //there is 1 or 2 characters left to encode and append a \0
		p = abs(p);

		if ((bufferP + 5) > bufferLength) { //4 bytes of the next block + \0
			free(xorData);

			return XOR_ERROR;
		}

		base64_encode((unsigned char *) xorData + ((dataLength + XOR_CHECKSUM_BYTES) - p), buffer + bufferP, p);

		buffer[bufferP + 4] = 0;
	}
	else { //nothing left, append a \0
		if ((bufferP + 1) > bufferLength) {
			free(xorData);

			return XOR_ERROR;
		}

		buffer[bufferP] = 0;
	}

	free(xorData);

	return XOR_OK;
}

int xor_decrypt(struct xor128_state* state, const unsigned char* data, int dataLength, unsigned char* buffer, int bufferLength) {
	int   p                = 0;
	int   bufferP          = 0;

	int   xorKey           = 0;
	char* xorKeyData       = 0;
	int   xorKeyP          = 4;

	unsigned int originalChecksum = 0;
	unsigned int dataChecksum     = 0;

	//unpack the base64 package
	if (dataLength > 0 && (dataLength % 4) != 0) { // the data must be a multiply of 4
		return XOR_ERROR;
	}

	while (p < dataLength) {
		if (bufferP + 3 > bufferLength) {
			return XOR_ERROR;
		}

		base64_decode((unsigned char*) data + p, buffer + bufferP);

		p       += 4;
		bufferP += 3;
	}

	//the last 2 bytes could be a null byte, however for some reason a addictional 3 bytes is added
	if (buffer[bufferP - 1] == 0xFF) {
		bufferP -= 1;
	}

	if (buffer[bufferP - 4] == 0) {
		if (buffer[bufferP - 5] == 0) {
			bufferP -= 5;
		}
		else {
			bufferP -= 4;
		}
	}

	if (bufferP + 1 > bufferLength) {
		return XOR_ERROR;
	}

	buffer[bufferP] = 0;

	//exstract checksum; last 4 bytes in the buffer are the checksum
	if (bufferP < 4) {//the resulting data must be at LEAST 4 bytes.. (the checksum)
		return XOR_ERROR;
	}

	originalChecksum  = buffer[bufferP - 4];
    originalChecksum += buffer[bufferP - 3] << 8;
    originalChecksum += buffer[bufferP - 2] << 16;
    originalChecksum += buffer[bufferP - 1] << 24;

	//unXOR the data with a random key. This key is always the same when the same init values are used
	p = 0;
	while (p < (bufferP - XOR_CHECKSUM_BYTES)) {
		if (xorKeyP == 4) {
			xorKey = xor128_multirand(state, XOR_MULTIRAND_COUNT);
			xorKeyData = (char*)&xorKey;

			xorKeyP = 0;
		}

		buffer[p] ^= xorKeyData[xorKeyP];

		p++;
		xorKeyP++;
	}

	buffer[p] = 0;

	//calculate data checksum; last 4 bytes in the buffer are the checksum
	dataChecksum = calculateCRC32((char*) buffer, bufferP - XOR_CHECKSUM_BYTES);

	if (originalChecksum != dataChecksum) {
		return XOR_ERROR;
	}

	return XOR_OK;
}

/*
int main() {
	const unsigned char data[] = "6FEAC8D5498F14A2F145CFE48E8818AB";

	unsigned char* encrypted_buffer = malloc(1024);
	unsigned char* decrypted_buffer = malloc(1024);

	struct xor128_state encryptState;
	struct xor128_state decryptState;

	int i = 0;
	for (i = 0; i < 100000; i++) {
		xor_crypt_init(&encryptState, 12345678, 56789012, 90123456, 34567890);
		xor_crypt_init(&decryptState, 12345678, 56789012, 90123456, 34567890);

		if (xor_encrypt(&encryptState, data, strlen(data), encrypted_buffer, 1024)) {
			printf("Encrypt error!\n");
			exit(EXIT_FAILURE);
		}

		if (xor_decrypt(&decryptState, encrypted_buffer, strlen(encrypted_buffer), decrypted_buffer, 1024)) {
			printf("Decrypt error!\n");
			exit(EXIT_FAILURE);
		}
	}

	printf("Data:      %s\n", data);
	printf("Encrypted: %s\n", encrypted_buffer);
	printf("Decrypted: %s\n", decrypted_buffer);

	free(encrypted_buffer);
	free(decrypted_buffer);
}
*/
#endif // HW_BAN

