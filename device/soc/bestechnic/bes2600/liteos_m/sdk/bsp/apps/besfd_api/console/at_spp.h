#ifdef DEBUG_SPP_LOG

#ifndef AT_SPP_H
#define AT_SPP_H

#ifdef __cplusplus
extern "C"{
#endif

#if defined (__3M_PACK__)
#define L2CAP_MTU 980
#else
#define L2CAP_MTU 672
#endif

enum {
    HI_ERR_SUCCESS = 0,
    HI_ERR_RECVING = 0X80000003,
    hi_ERR_EVENT_CREATE_SYS_FAIL = 0x80000242,
    HI_ERR_EVENT_SEND_FAIL = 0x80000243,
    HI_ERR_EVENT_WAIT_FAIL = 0x80000244,
    HI_ERR_EVENT_WAIT_TIME_OUT =0x8000024A,
    HI_ERR_AT_NAME_OR_FUNC_REPEAT_REGISTERED = 0x80003280,
    HI_ERR_AT_INVALID_PARAMETER = 0x80003281,
    HI_ERR_FAILURE = (unsigned int)(-1),
};

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define LOS_WAIT_FOREVER  0xFFFFFFFF

unsigned int AtSppInitApi(void);
unsigned int SppSendData(const char *sendData,unsigned short dataLen);
int SppThreadInit(void);

#ifdef __cplusplus
}
#endif

#endif

#endif