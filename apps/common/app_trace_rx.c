/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "app_trace_rx.h"
#include "hal_trace.h"
#include "stdio.h"
#include "string.h"


#define APP_TRACE_RX_HEAD_SIZE   4
#define APP_TRACE_RX_NAME_SIZE   20
#if (defined(A2DP_SOURCE_TEST) || defined(HFP_AG_TEST))
#define APP_TRACE_RX_ROLE_NUM    30
#else
#define APP_TRACE_RX_ROLE_NUM    6
#endif

#define APP_TRACE_RX_BUF_SIZE    1024

#define APP_TRACE_RX_HEAD       '['
#define APP_TRACE_RX_END        ']'
#define APP_TRACE_RX_SEPARATOR  ','

typedef struct{
    char *name;
    uint32_t len;
    uint8_t  *buf;
}APP_TRACE_RX_CFG_T;

typedef struct{
    char name[APP_TRACE_RX_NAME_SIZE];
    APP_TRACE_RX_CALLBACK_T callback;
}APP_TRACE_RX_LIST_T;

typedef struct{
    uint32_t list_num;
    APP_TRACE_RX_LIST_T list[APP_TRACE_RX_ROLE_NUM];
    uint8_t rx_buf[APP_TRACE_RX_BUF_SIZE + 1];
} APP_TRACE_RX_T;

#if defined(_AUTO_TEST_)
extern unsigned int auto_test_prase(unsigned char *cmd, unsigned int len);
#endif

static APP_TRACE_RX_T app_trace_rx;

int app_trace_rx_dump_list(void)
{
    for(int i=0;i<APP_TRACE_RX_ROLE_NUM;i++)
    {
        TRACE(2,"%d: %s", i, app_trace_rx.list[i].name);
    }
    return 0;
}

int app_trace_rx_is_in_list(const char *name)
{
    for(int i=0;i<APP_TRACE_RX_ROLE_NUM;i++)
    {
        if(!strcmp(app_trace_rx.list[i].name, name))
        {
            return i;
        }
    }
    app_trace_rx_dump_list();
    // TRACE(1,"%s", app_trace_rx.list[0].name);
    // TRACE(1,"%s", name);
    // TRACE(1,"%d", strlen(app_trace_rx.list[0].name));
    // TRACE(1,"%d", strlen(name));
    // TRACE(1,"%d", strcmp(app_trace_rx.list[0].name, name));
    return -1;
}

int app_trace_rx_add_item_to_list(const char *name, APP_TRACE_RX_CALLBACK_T callback)
{
    for(int i=0;i<APP_TRACE_RX_ROLE_NUM;i++)
    {
        if(app_trace_rx.list[i].name[0] == 0)
        {
            memcpy(app_trace_rx.list[i].name, name, strlen(name));
            app_trace_rx.list[i].callback = callback;
            app_trace_rx.list_num++;
            return 0;
        }
    }

    return 1;
}

int app_trace_rx_del_item_to_list(int id)
{
    memset(app_trace_rx.list[id].name, 0, sizeof(app_trace_rx.list[id].name));
    app_trace_rx.list[id].callback = NULL;
    app_trace_rx.list_num--;

    return 0;
}

int app_trace_rx_register(const char *name, APP_TRACE_RX_CALLBACK_T callback)
{
    TRACE(2,"[%s] Add %s", __func__, name);
    if(app_trace_rx_is_in_list(name) == -1)
    {
        app_trace_rx_add_item_to_list(name, callback);
        return 0;
    }
    else
    {
        return 0;
    }
}

int app_trace_rx_deregister(const char *name)
{
    int id = 0;

    id = app_trace_rx_is_in_list(name);

    if(id != -1)
    {
        app_trace_rx_del_item_to_list(id);
        return 0;
    }
    else
    {
        return 1;
    }
}

// [test,12,102.99]
static int app_trace_rx_parse(int8_t *buf, unsigned int buf_len, APP_TRACE_RX_CFG_T *cfg)
{
    // TRACE(1,"[%s] Start...", __func__);
    int pos = 0;
    int len = 0;

    for(; pos<APP_TRACE_RX_HEAD_SIZE; pos++)
    {
        if(buf[pos] == APP_TRACE_RX_HEAD)
        {
            buf[pos] = 0;
            break;
        }
    }

    if(pos == APP_TRACE_RX_HEAD_SIZE)
    {
        return 3;
    }

    pos++;

    cfg->name = (char *)(buf+pos);
    for(; pos<APP_TRACE_RX_NAME_SIZE+APP_TRACE_RX_HEAD_SIZE; pos++)
    {
        if(buf[pos] == APP_TRACE_RX_SEPARATOR)
        {
            buf[pos] = 0;
            break;
        }
    }

    // TRACE(1,"Step1: %s", cfg->name);
    // TRACE(1,"%d", strlen(cfg->name));

    if(pos == APP_TRACE_RX_NAME_SIZE)
    {
        return 1;
    }

    pos++;

    len = 0;
    cfg->buf = (uint8_t*)(buf+pos);
    for(; pos<buf_len; pos++)
    {
        if(buf[pos] == APP_TRACE_RX_END)
        {
            buf[pos] = 0;
            break;
        }
        len++;
    }
    cfg->len = len;
    if(pos == buf_len)
    {
        return 2;
    }

    return 0;
}

#if defined(IBRT)
void app_ibrt_peripheral_automate_test(const char* ibrt_cmd, uint32_t cmd_len);
void app_ibrt_peripheral_perform_test(const char* ibrt_cmd, uint32_t cmd_len);
#else
void app_bt_cmd_perform_test(const char* ibrt_cmd, uint32_t cmd_len);
#endif

#if defined(CONFIG_BES_AUDIO_DUMP)
#include "hal_uart.h"
typedef struct {
    HAL_UART_IRQ_RXDMA_HANDLER_T rxdma;
    uint8_t *rx_buffer;
    uint32_t buff_len;
}NUTTX_RX_T;

NUTTX_RX_T nuttx_rx_process;
static unsigned int app_nuttx_callback(unsigned char *buf, uint32_t len)
{
    TRACE(2,"[%s] len = %d,buf:%s", __func__, len, buf);
    if (nuttx_rx_process.rxdma && nuttx_rx_process.rx_buffer)
    {
        union HAL_UART_IRQ_T status;
        status.reg = 0;
        if (len > nuttx_rx_process.buff_len-1)
            len = nuttx_rx_process.buff_len-1;
        memcpy(nuttx_rx_process.rx_buffer,buf,len);
        nuttx_rx_process.rx_buffer[len]='\n';
        nuttx_rx_process.rxdma(len+1, 0, status);
    }
    if (strncmp("assert",nuttx_rx_process.rx_buffer,6) ==0)
        ASSERT(0,"TEST");
    return 0;
}
void hal_trace_set_rx_hander(HAL_UART_IRQ_RXDMA_HANDLER_T rxdma,uint8_t *rx_buffer, uint32_t buff_len)
{
    nuttx_rx_process.rx_buffer = rx_buffer;
    nuttx_rx_process.buff_len = buff_len;
    nuttx_rx_process.rxdma = rxdma;
}
#endif

uint32_t app_trace_rx_process(uint8_t *buf, uint32_t len)
{
    APP_TRACE_RX_CFG_T cfg;
    int id = 0;
    int res = 0;

    if (buf == NULL) {
        return 1;
    }

    buf[len] = 0;

#if defined(IBRT)
    if (len > 10 &&
        ((strncmp((char*)buf, "auto_test:", 10) == 0) || (strncmp((char*)buf, "ibrt_test:", 10) == 0) || (strncmp((char*)buf, "best_test:", 10) == 0)))
    {
#ifdef BES_AUTOMATE_TEST
        app_ibrt_peripheral_automate_test((char*)(buf + 10), len - 10);
#else
        app_ibrt_peripheral_perform_test((char*)(buf + 10), len - 10);
#endif
        return 0;
    }
#else
    if (len > 10 && (strncmp((char*)buf, "best_test:", 10) == 0))
    {
        app_bt_cmd_perform_test((char*)(buf + 10), len - 10);
        return 0;
    }
#endif

    res = app_trace_rx_parse((int8_t*)buf, len, &cfg);

    if(res)
    {
        TRACE(1,"ERROR: app_trace_rx_parse %d", res);
        return 2;
    }
    else
    {
        // TRACE(1,"%s rx OK", cfg.name);
    }

    id = app_trace_rx_is_in_list(cfg.name);

    if(id == -1)
    {
        TRACE(1,"%s is invalid", cfg.name);
        return 3;
    }

    if(app_trace_rx.list[id].callback)
    {
        app_trace_rx.list[id].callback(cfg.buf, cfg.len);
    }
    else
    {
        TRACE(1,"%s has not callback", app_trace_rx.list[id].name);
        return 4;
    }

    return 0;
}

static unsigned int app_test_callback(unsigned char *buf, uint32_t len)
{
    TRACE(2,"[%s] len = %d", __func__, len);

    // Process string
    int num_int = 0;
    int num_float = 0.0;
    TRACE(2,"[%s] %s", __func__, buf);
    sscanf((char*)buf, "%d,%d", &num_int, &num_float);

    TRACE(3,"[%s] %d:%d", __func__, num_int, num_float);

    return 0;
}

void app_trace_rx_open(void)
{
    HAL_TRACE_RX_CALLBACK_T rx_proc;

#if defined(_AUTO_TEST_)
    rx_proc = auto_test_prase;
#else
    rx_proc = app_trace_rx_process;
#endif

    hal_trace_rx_open(app_trace_rx.rx_buf, APP_TRACE_RX_BUF_SIZE, rx_proc);

    app_trace_rx_register("test", app_test_callback);
#if defined(CONFIG_BES_AUDIO_DUMP)
    app_trace_rx_register("nuttx", app_nuttx_callback);
#endif
}

