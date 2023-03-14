#ifndef MBEDTLS_AES_ALT_H
#define MBEDTLS_AES_ALT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cpluscplus
extern "c" {
#endif

typedef enum {
    AES_KEYLEN_128 = 0,
    AES_KEYLEN_192,
    AES_KEYLEN_256,
} aes_key_len_t;

typedef struct mbedtls_aes_context
{
    uint8_t key[32];
    aes_key_len_t  keylen;    
} mbedtls_aes_context;

#ifdef __cpluscplus
}
#endif

#endif
