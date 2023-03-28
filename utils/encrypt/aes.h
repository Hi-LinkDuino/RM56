#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>
#include <stdbool.h>

// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES128 encryption in CBC-mode of operation and handles 0-padding.
// ECB enables the basic ECB 16-byte block algorithm. Both can be enabled simultaneously.

// The #ifndef-guard allows it to be configured before #include'ing or at compile time.
#ifndef CBC
#define CBC 1
#endif

#ifndef ECB
#define ECB 1
#endif

#ifdef __cplusplus
extern "C" {
#endif


#if defined(ECB) && ECB

void AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
void AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);

#endif // #if defined(ECB) && ECB


#if defined(CBC) && CBC

void AES128_CBC_encrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
void AES128_CBC_decrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);

bool AES128_CBC_Encrypt(
    uint8_t *plaintext, 
    size_t plaintext_len,
    uint8_t *cyphertext, 
    size_t* cyphertext_len, 
    uint8_t iv[16], 
    uint8_t key[16], 
    size_t padding_len);

#endif // #if defined(CBC) && CBC

void AES128_CTR_encrypt_buffer(uint8_t* input, uint32_t length, const uint8_t* key, uint8_t* iv, uint8_t* output);

#ifdef __cplusplus
}
#endif


#endif //_AES_H_

