#ifndef _A7_EXPTMOD_H_
#define _A7_EXPTMOD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdio.h>

typedef enum {
    A7_MATH_TYPE_EXPTMOD_TLV_1,
    A7_MATH_TYPE_EXPTMOD_TLV_2,
    A7_MATH_TYPE_EXPTMOD_TLV_3,
    A7_MATH_TYPE_EXPTMOD_TLV_4,
} A7_MATH_TYPE_EXPTMOD_TLV_E;

//这是a7_math.c需要调用的函数
int handle_exptmod(pkg_t *pkg);

//这是原本mcu里的
int mcu_math_exptmod(uint8_t *arr1, int arrlen1, uint8_t *arr2, int arrlen2,
                     uint8_t *arr3, int arrlen3, uint8_t *out, int out_buf_len, int *outlen);
//增加了等待超时
int mcu_math_exptmod_timeout(uint8_t *arr1, int arrlen1, uint8_t *arr2, int arrlen2,
                             uint8_t *arr3, int arrlen3,
                             uint8_t *out, int out_buf_len, int *outlen, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // _A7_EXPTMOD_H_
