#ifdef HW_BAN
#ifndef __BASE64_H__
#define __BASE64_H__

void base64_encode( const unsigned char in[3], unsigned char out[4], int len );
void base64_decode( const unsigned char in[4], unsigned char out[3] );

#endif
#endif // HW_BAN
