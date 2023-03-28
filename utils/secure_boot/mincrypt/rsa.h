/* rsa.h
**
** Copyright 2008, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of Google Inc. nor the names of its contributors may
**       be used to endorse or promote products derived from this software
**       without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY Google Inc. ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
** EVENT SHALL Google Inc. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
** OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
** OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
** ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SYSTEM_CORE_INCLUDE_MINCRYPT_RSA_H_
#define SYSTEM_CORE_INCLUDE_MINCRYPT_RSA_H_

#include <stdint.h>
#ifndef _WIN32
#include "plat_addr_map.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_EXPONENT_65537
//#define KEY_EXPONENT_3

#if SECURE_BOOT_VER >= 4
#define RSANUMBYTES 384           /* 3072 bit key length */
#else
#define RSANUMBYTES 256           /* 2048 bit key length */
#endif
#define RSANUMWORDS (RSANUMBYTES / sizeof(uint32_t))

typedef struct RSAPublicKey {
    uint32_t n0inv;           /* -1 / n[0] mod 2^32 */
    uint32_t n[RSANUMWORDS];  /* modulus as little endian array */
    uint32_t rr[RSANUMWORDS]; /* R^2 as little endian array */
} RSAPublicKey;

int RSA_verify_hash(int eng_en,
                    const RSAPublicKey *key,
                    const uint8_t *signature,
                    const uint32_t len,
                    const uint8_t *hash,
                    const uint32_t hash_len);

int RSA_verify(int eng_en,
               const RSAPublicKey *key,
               const uint8_t *signature,
               const uint32_t len,
               const uint8_t *msg,
               const uint32_t msg_len);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_CORE_INCLUDE_MINCRYPT_RSA_H_
