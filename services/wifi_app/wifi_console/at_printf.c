#ifdef DEBUG_SPP_LOG

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "at_printf.h"
#include "at_spp.h"
#include "hal_trace.h"


#define PRINTF_BUFFER_SIZE 1024
#define END_SHIFT_TWO 2
#define END_SHIFT_ONE 1

unsigned int SppPrintf(const char *fmt, ...)
{
    va_list ap;
    char buf[PRINTF_BUFFER_SIZE] = {0};
    int ret;

    va_start(ap,fmt);
    ret = vsnprintf(buf,  PRINTF_BUFFER_SIZE - END_SHIFT_ONE, fmt, ap);
    if(ret < 0) {
        return HI_ERR_FAILURE;
    }

    if(strlen(buf) + END_SHIFT_TWO < PRINTF_BUFFER_SIZE) {
        buf[ret++] = '\r';
        buf[ret++] = '\n';
    }
    else {
        return HI_ERR_FAILURE;
    }
    va_end(ap);

    if(ret > 0) {
        // hal_trace_disable_pause(true);
        // hal_trace_output((const unsigned char *)buf, ret);
        // hal_trace_flush_buffer();
        // hal_trace_disable_pause(false);
        ret = SppSendData(buf, ret);
        if(ret != HI_ERR_SUCCESS) {
            return HI_ERR_FAILURE;
        }
    }

    return HI_ERR_SUCCESS;
}

unsigned int SppResponseInfo(const char *fmt, ...)
{
    va_list ap;
    char buf[PRINTF_BUFFER_SIZE] = {0};
    int ret;

    va_start(ap, fmt);
    ret = vsnprintf(buf,  PRINTF_BUFFER_SIZE - END_SHIFT_ONE, fmt, ap);
    if(ret < 0) {
        return HI_ERR_FAILURE;
    }

    if(strlen(buf)  + END_SHIFT_TWO < PRINTF_BUFFER_SIZE) {
        buf[ret++] = '\r';
        buf[ret++] = '\n';
    }
    else {
        return HI_ERR_FAILURE;
    }
    va_end(ap);

    if(ret > 0) {
        //hal_trace_output((const unsigned char *)buf, ret);
        ret = SppSendData(buf, ret);
        if(ret != HI_ERR_SUCCESS) {
            return HI_ERR_FAILURE;
        }
    }
    return HI_ERR_SUCCESS;
}

unsigned int SppAtShowCmd(const char *fmt, ...)
{
    va_list ap;
    char buf[PRINTF_BUFFER_SIZE] = {0};
    int ret;

    va_start(ap, fmt);
    ret = vsnprintf(buf, PRINTF_BUFFER_SIZE - END_SHIFT_ONE, fmt, ap);
    if(ret < 0) {
        return HI_ERR_FAILURE;
    }

    if(strlen(buf) + END_SHIFT_TWO < PRINTF_BUFFER_SIZE) {
        buf[ret + END_SHIFT_ONE] = '\0';
    }
    else {
        return HI_ERR_FAILURE;
    }
    va_end(ap);

    if(ret > 0) {
        //hal_trace_output((const unsigned char *)buf, ret);
        ret = SppSendData(buf, ret);
        if (ret != HI_ERR_SUCCESS) {
            return HI_ERR_FAILURE;
        }
    }
    return HI_ERR_SUCCESS;
}


#endif