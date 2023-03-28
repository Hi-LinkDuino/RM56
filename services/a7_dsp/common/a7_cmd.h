#ifndef A7_CMD_H
#define A7_CMD_H

#ifdef __cplusplus
extern "C" {
#endif
#define A7_CMD_MAX_LEN  120
typedef enum {
    AUD_DUMP_SRC_BEFORE_ALG,
    AUD_DUMP_SRC_INTER_ALG,
    AUD_DUMP_SRC_AFTER_ALG,

    AUD_DUMP_SRC_NUM,
}AUD_DUMP_SRC_T;

typedef enum {
    A7_CMD_TYPE_DEBUG_MM,
    A7_CMD_TYPE_AUDIO_DUMP,
    A7_CMD_TYPE_UART2_TRACE,
    A7_CMD_TYPE_STATPRINT,
    A7_CMD_TYPE_PANIC,
    A7_CMD_TYPE_HEAP_INIT,
    A7_CMD_TYPE_HANDSHAKE,
    A7_CMD_TYPE_CUST_CTRL_PLANE,
    A7_CMD_TYPE_TASKBT,
    A7_CMD_TYPE_MMLK,
    A7_CMD_TYPE_BOOTNUM,
    A7_CMD_TYPE_TIME,
    A7_CMD_TYPE_ALG_DATA,
    A7_CMD_TYPE_ALG_LEN,
    A7_CMD_TYPE_WAKEUP_CONFIG,
    A7_CMD_TYPE_WAKEUP_LEN,
    A7_CMD_TYPE_ALGO_MODEL_INIT,
    A7_CMD_TYPE_NUM,
}A7_CMD_TYPE_T;

typedef struct {
    A7_CMD_TYPE_T type;
    unsigned int p1;
    unsigned int p2;
    char str[32];
}A7_CMD_T;

typedef void (* a7_cmd_callback_t)(char *data, int len);

void a7_cmd_init(void);
void a7_cmd_send(void *data, int len);
void mcu_cmd_send(A7_CMD_T *cmd);
void a7_cmd_send2(A7_CMD_T *cmd);
void a7_cmd_recv_init(void);
void set_a7_cmd_callback_handler (a7_cmd_callback_t callback);
void a7_debug_mm_print(void);

#ifdef __cplusplus
}
#endif

#endif
