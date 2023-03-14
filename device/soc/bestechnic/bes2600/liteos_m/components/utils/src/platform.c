/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "platform.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_trng.h"
#include "stdlib.h"

#define BUFSIZE 256

void print_string(const char *fmt, ...)
{
    if (!fmt)
        return;
    char buf[BUFSIZE] = {0};
    int len;
    va_list ap;
    va_start(ap, fmt);
    len = vsnprintf(buf, BUFSIZE, fmt, ap);
    va_end(ap);
    if (len > 0) {
        hal_trace_output((const unsigned char *)buf, len);
#ifdef LOG_FLUSH
        hal_trace_flush_buffer();
#endif
    }
}

void print_array(const char *name, const void *array, uint16_t len)
{
    if (!array || !len)
        return;
    char buf[BUFSIZE] = {0};
    const uint8_t *ptr = (const uint8_t *)array;
    uint16_t i, j = 0;
    if (name) {
        int n = P_MIN(BUFSIZE, strlen(name));
        strncpy(buf, name, BUFSIZE);
        j += n;
    }
    for (i = 0; i < len && j < BUFSIZE; ++i) {
        int n = snprintf(&buf[j], BUFSIZE - j, "%02X ", ptr[i]);
        j += n;
    }
    if (j <= BUFSIZE - 1) {
        buf[j] = '\n';
        j++;
    }
    hal_trace_output((const unsigned char *)buf, j);
#ifdef LOG_FLUSH
    hal_trace_flush_buffer();
#endif
}

uint32_t GetSysTime(void)
{
    return hal_fast_sys_timer_get();
}

uint32_t SysTimeDiff(uint32_t start)
{
    return FAST_TICKS_TO_US(hal_fast_sys_timer_get() - start) / 1000;
}

void DelayMs(uint32_t ms)
{
#if 1
    osDelay(ms); // os 1000 ticks/s here
#else
    hal_sys_timer_delay(MS_TO_TICKS(ms));
#endif
}

void DelayUs(uint32_t us)
{
    if (us >= 1000) {
        DelayMs(us / 1000);
        us = us % 1000;
    }
    uint32_t flags = int_lock();
    uint32_t now = hal_fast_sys_timer_get();
    while (hal_fast_sys_timer_get() - now < US_TO_FAST_TICKS(us));
    int_unlock(flags);
}

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    int ret;
    TRACE(0,"%s %d start",__func__,__LINE__);

    // ret = hal_get_trngdata(output, len);
    // if (ret) {
        uint32_t i;
        for (i = 0; i < len; i++) {
            output[i] = rand() & 0xFF;
        }
    // }
    *olen = len;
    TRACE(0,"%s %d done",__func__,__LINE__);
    return 0;
}

// implementation for js_app_host.h: LP_TaskBegin
void LP_TaskBegin() {
}

// implementation for js_app_host.h: LP_TaskEnd
void LP_TaskEnd() {
}
