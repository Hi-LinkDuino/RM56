//此文件用于定义类型结构体，声明各功能需要调用的头文件
// v_1_9
#ifndef _A7_MATH_FUNCTION_H_
#define _A7_MATH_FUNCTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tlv.h"

//这是功能类型
typedef enum {
    A7_MATH_TYPE_EXPTMOD,
    A7_MATH_TYPE_ADD,
    A7_MATH_TYPE_MODEXPMUL,
    A7_MATH_TYPE_CURVE25519,
    A7_MATH_TYPE_ED25519,
    A7_MATH_TYPE_SRPSERVER_START,
    A7_MATH_TYPE_SRPSERVER_VERIFY,
    A7_MATH_TYPE_SRPSERVER_DELETE,
    A7_MATH_TYPE_SRPSERVER_CREATE,
    A7_MATH_TYPE_NUM
} A7_MATH_TYPE_E;

//这是图的调用类型
typedef struct {
    const int id;
    int (*func)(pkg_t *pkg);
} math_func_item_t;

//这是图
extern math_func_item_t math_map[];

//这是定义在a7_math.c中，各功能需要调用的
extern void a7_math_send_to_mcu(void *data, int len);

//这是定义在mcu_math.c中，各功能需要调用的
extern int wait_result_done;
extern int g_receiver_valid;
extern uint8_t g_receiver_buf[1024];
extern void mcu_math_send(void *user_data, int user_data_len);
extern int wait_recv_result_done();
extern int wait_recv_result_done_timeout(int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif