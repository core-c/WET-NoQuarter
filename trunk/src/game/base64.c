#ifdef HW_BAN
/*********************************************************************\

MODULE NAME:    b64.c

AUTHOR:         Bob Trower 08/04/01

PROJECT:        Crypt Data Packaging

COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001

NOTE:           This source code may be used as you wish, subject to
                the MIT license.  See the LICENCE section below.

LICENCE:        Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.

                Permission is hereby granted, free of charge, to any person
                obtaining a copy of this software and associated
                documentation files (the "Software"), to deal in the
                Software without restriction, including without limitation
                the rights to use, copy, modify, merge, publish, distribute,
                sublicense, and/or sell copies of the Software, and to
                permit persons to whom the Software is furnished to do so,
                subject to the following conditions:

                The above copyright notice and this permission notice shall
                be included in all copies or substantial portions of the
                Software.

                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
                KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
                WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
                PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
                OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
                OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
                OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
                SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

VERSION HISTORY:
                Bob Trower 08/04/01 -- Create Version 0.00.00B

\******************************************************************* */

/**
 * Modified & cleaned by J. M. Bogaard
 * The algoritm is not changed, just restructered in 2 functions.
 */

/*
** base64_encode
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void base64_encode( const unsigned char in[3], unsigned char out[4], int len ) {
	// Translation Table as described in RFC1113
	static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/*
** base64_decode
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
void base64_decode( const unsigned char in[4], unsigned char out[3] ) {
	// Translation Table to decode (created by author)
	static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

	int           i = 0;
	unsigned char v = 0;
	unsigned char in_decoded[4];

	for (i = 0; i < 4; i++) {
		v = in[i];
		v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
		if(v) {
			v = (unsigned char) ((v == '$') ? 0 : v - 61);
			in_decoded[i] = (unsigned char) (v - 1);
		}
	}


    out[ 0 ] = (unsigned char ) (in_decoded[0] << 2 | in_decoded[1] >> 4);
    out[ 1 ] = (unsigned char ) (in_decoded[1] << 4 | in_decoded[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in_decoded[2] << 6) & 0xc0) | in_decoded[3]);
}
#endif
