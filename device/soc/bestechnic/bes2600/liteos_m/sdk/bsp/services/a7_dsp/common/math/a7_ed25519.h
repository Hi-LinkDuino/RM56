#ifndef __A7_ED25519_H__
#define __A7_ED25519_H__

#ifdef __cplusplus
extern "C" {
#endif

// For A7
int handle_ed25519(pkg_t *pkg);

// For M33
int mcu_math_ed25519(uint8_t* param1, int len1, uint8_t* param2, int len2,
                  uint8_t* param3, int len3, uint8_t* out, int out_buf_len, int* outlen);

#ifdef __cplusplus
}
#endif


#endif /* __A7_ED25519_H__ */
