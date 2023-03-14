#ifndef __A7_CURVE25519_H__
#define __A7_CURVE25519_H__

#ifdef __cplusplus
extern "C" {
#endif

// For A7
int handle_curve25519(pkg_t *pkg);

// For M33
int mcu_math_curve25519(uint8_t *arr1, int arrlen1, uint8_t *arr2, int arrlen2,
                        uint8_t *out, int out_buf_len, int *outlen);
// For MCU,with timeout
int mcu_math_curve25519_timeout(uint8_t *arr1, int arrlen1, uint8_t *arr2, int arrlen2,
                                uint8_t *out, int out_buf_len, int *outlen, int timeout_ms);

#ifdef __cplusplus
}
#endif


#endif /* __A7_CURVE25519_H__ */
