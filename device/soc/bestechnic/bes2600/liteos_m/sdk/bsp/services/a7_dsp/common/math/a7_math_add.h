#ifndef _A7_MATH_ADD_H_
#define _A7_MATH_ADD_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "tlv.h"
#include <stdint.h>
#include <stdio.h>

//这是输入/输出参数类型
typedef enum {
    A7_MATH_TYPE_ADD_TLV_1,
    A7_MATH_TYPE_ADD_TLV_2,
    A7_MATH_TYPE_ADD_RET
} A7_MATH_TYPE_ADD_TLV_E;

//这是将 mcu/a7 接收/发送 的 pkg/buf 转成 buf/pkg 的
int mcu_send_buf2pkg_add(uint8_t *arr1, int arrlen1, uint8_t *arr2, int arrlen2, uint8_t **out, int *outlen);
int mcu_recv_pkg2buf_add(uint8_t *recvbuf, uint8_t *out, int out_buf_len, int *outlen);
int a7_recv_pkg2buf_add(pkg_t *pkg, uint8_t **out1, int *out1_len, uint8_t **out2, int *out2_len);
int a7_send_buf2pkg_add(uint8_t *buf, int len, uint8_t **out, int *outlen);

//这是负责实际运算的
int handle_add(pkg_t *pkg);

//这是负责申请的
int mcu_math_add(uint8_t *arr1, int arrlen1, uint8_t *arr2, int arrlen2,
                 uint8_t *out, int out_buf_len, int *outlen,int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif