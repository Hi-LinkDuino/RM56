#ifndef _A7_MODEXPMUL_H_
#define _A7_MODEXPMUL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tlv.h"
#include <stdint.h>
#include <stdio.h>

//这是输入/输出参数类型
//四输入两输出
typedef enum {
    A7_MATH_TYPE_MODEXPMUL_TLV_1,
    A7_MATH_TYPE_MODEXPMUL_TLV_2,
    A7_MATH_TYPE_MODEXPMUL_TLV_3,
    A7_MATH_TYPE_MODEXPMUL_TLV_4,
    A7_MATH_TYPE_MODEXPMUL_RET_1,
    A7_MATH_TYPE_MODEXPMUL_RET_2,
} A7_MATH_TYPE_MODEXPMUL_TLV_E;

// 这个函数用于实际运算，需要调用定义在a7_math.c中的函数 a7_math_send_to_mcu
int handle_modexpmul(pkg_t *pkg);

// 这个函数需要调用 mcu_math.c 中的 mcu_math_send，在里面运行
int mcu_math_modexpmul(uint8_t *arr1, int arrlen1, uint8_t *arr2, int arrlen2,
                       uint8_t *arr3, int arrlen3, uint8_t *arr4, int arrlen4,
                       uint8_t *out1, int out_buf_len1, int *outlen1,
                       uint8_t *out2, int out_buf_len2, int *outlen2);

#ifdef __cplusplus
}
#endif

#endif