/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "stdbool.h"
#include "hal_intersys.h"
#include "hal_iomux.h"
#include "hal_uart.h"
#include "hal_trace.h"
#include "string.h"
#include "hal_chipid.h"
#include "heap_api.h"
#include "bt_drv.h"
#include "bt_drv_2003_internal.h"
#include "bt_drv_interface.h"
#include "bt_drv_reg_op.h"
#ifdef __EBQ_TEST__
#define BT_UART_BRIDGE_BUF_LEN (1030)
#else
#define BT_UART_BRIDGE_BUF_LEN (1024)
#endif

static const struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_4,
    .baud = 115200,
    .dma_rx = true,
    .dma_tx = true,
    .dma_rx_stop_on_err = false,
};

#if (DEBUG_PORT == 1)
enum HAL_UART_ID_T br_uart = HAL_UART_ID_0;
#else
enum HAL_UART_ID_T br_uart = HAL_UART_ID_1;
#endif
enum HAL_INTERSYS_ID_T br_intersys = HAL_INTERSYS_ID_0;

static volatile bool uart_rx_done = false;
static volatile bool uart_tx_done = false;
static volatile bool bt_rx_done = false;
static volatile bool bt_tx_done = false;

static unsigned char *uart_rx_data_p;
static volatile unsigned int uart_rx_len = 0;

static unsigned char *bt_rx_data_p;
static volatile unsigned int bt_rx_len = 0;

/* hci send lock thread id */
static osSemaphoreId hci_rx_wait_sem = NULL;
osSemaphoreDef(hci_rx_wait);

static void uart_rx(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    if (dma_error) {
        BT_DRV_TRACE(1,"uart_rx dma error: xfer_size=%d", xfer_size);
        uart_rx_len = 0;
    } else if (status.BE || status.FE || status.OE || status.PE) {
        BT_DRV_TRACE(2,"uart_rx uart error: xfer_size=%d, status=0x%08x", xfer_size, status.reg);
        uart_rx_len = 0;
    } else {
        uart_rx_len = xfer_size;
    }
    uart_rx_done = true;
}

static void uart_tx(uint32_t xfer_size, int dma_error)
{
    if (dma_error) {
        BT_DRV_TRACE(1,"uart_tx dma error: xfer_size=%d", xfer_size);
    }
    uart_tx_done = true;
}

#ifdef VCO_TEST_TOOL
static unsigned int bt_rx(const unsigned char *data, unsigned int len);

void vco_test_simu_bt_rx(void)
{
    const  unsigned char hci_bt_vco_test_event[] = {0x04, 0x0e, 0x04, 0x05, 0xaa,0xfc,0x00};
    bt_rx(hci_bt_vco_test_event, sizeof(hci_bt_vco_test_event));
}
#endif

le_test_end_handler_func le_test_cmd_complete_callback = NULL;
void btdrv_regist_le_teset_end_callback(le_test_end_handler_func cb)
{
    if(cb != NULL)
    {
        le_test_cmd_complete_callback = cb;
    }
}

bt_nonsig_test_end_handler_func bt_nonsignalint_test_complete_callback = NULL;
void btdrv_regist_bt_nonsignalint_test_end_callback(bt_nonsig_test_end_handler_func cb)
{
    if(cb != NULL)
    {
        bt_nonsignalint_test_complete_callback = cb;
    }
}

static void bt_bridge_mode_rx_callback(const unsigned char *data, unsigned int len)
{
    if(data[0] == 0x4 && data[1] == 0xe && data[4] == 0x3 && data[5] == 0xc)
    {
        memset((uint32_t *)BT_EM_ADDR_BASE,0,BT_EM_SIZE/4);
        *(uint32_t *)0xd02201e4 = 0;
        //fix nosignal rx gain initial value to 4 to fixed power
        BTDIGITAL_REG_SET_FIELD(0xd0211208, 0xf, 0, 0x4);
    }

    if(le_test_cmd_complete_callback != NULL)
    {
        if(data[0] == 0x4 && data[1] == 0xe && data[4] == 0x1f && data[5] == 0x20)
        {
            le_test_cmd_complete_callback(data[6], data[7]|data[8]<<8);
        }
    }

    if(bt_nonsignalint_test_complete_callback != NULL)
    {
        if(data[0] == 0x4 && data[1] == 0xe && data[4] == 0x87 && data[5] == 0xFC)
        {
            bt_nonsignalint_test_complete_callback(data[6], data[7]|data[8]<<8,
                data[9]|data[10]<<8, data[11]|data[12]<<8);
        }
    }
}

static unsigned int bt_rx(const unsigned char *data, unsigned int len)
{
    unsigned int processed = len;

    hal_intersys_stop_recv(br_intersys);
    if (bt_rx_len + processed > BT_UART_BRIDGE_BUF_LEN) {
        BT_DRV_TRACE(3,"bt_rx data too long: bt_rx_len=%d, len=%d, limit=%d", bt_rx_len, processed, sizeof(bt_rx_data_p));
        processed = BT_UART_BRIDGE_BUF_LEN-bt_rx_len;
    }
    if (processed > 0) {
        memcpy(bt_rx_data_p + bt_rx_len, data, processed);

        if (bt_rx_data_p[0] == 0x04 && bt_rx_data_p[1] == 0xFF)
        {
            BT_DRV_TRACE(0,"[RX]:");
            DUMP8("%02x ",bt_rx_data_p + bt_rx_len,processed);
        }

        bt_bridge_mode_rx_callback(data, len);
        bt_rx_len += processed;
        bt_rx_done = true;

        if(hci_rx_wait_sem)
            osSemaphoreRelease(hci_rx_wait_sem);

    }
    return len;
}

static void bt_tx(const unsigned char *data, unsigned int len)
{
    bt_tx_done = true;
}

uint8_t le_rx_test_v2_longrange_flag = 0;
void btdrv_uart_bridge_loop(void)
{
    int ret;
    union HAL_UART_IRQ_T mask;

    syspool_init();

    syspool_get_buff(&uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN);
    syspool_get_buff(&bt_rx_data_p, BT_UART_BRIDGE_BUF_LEN);

    if (br_uart == HAL_UART_ID_0) {
        hal_iomux_set_uart0();
        hal_iomux_set_uart1();
        hal_trace_switch(HAL_TRACE_TRANSPORT_UART1);
    } else {
        hal_iomux_set_uart1();
        hal_trace_switch(HAL_TRACE_TRANSPORT_UART0);
        //hal_iomux_set_uart0();
    }

    ret = hal_uart_open(br_uart, &uart_cfg);
    if (ret) {
        BT_DRV_TRACE(0,"Failed to open uart");
        return;
    }

    hal_uart_irq_set_dma_handler(br_uart, uart_rx, uart_tx);
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    mask.RT = 1;
    hal_uart_dma_recv_mask(br_uart, uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN, NULL, NULL,&mask);

    ret = hal_intersys_open(br_intersys, HAL_INTERSYS_MSG_HCI, bt_rx, bt_tx, false);
    if (ret) {
        BT_DRV_TRACE(0,"Failed to open intersys");
        return;
    }
    hal_intersys_start_recv(br_intersys);
    BT_DRV_TRACE(0,"Enter bridge mode");

    while (1) {
        if (uart_rx_done) {
            uart_rx_done = false;
            if (uart_rx_len > 0) {
                BT_DRV_TRACE(0,"[RX]:");
                DUMP8("%02x ",uart_rx_data_p,uart_rx_len);

                if(uart_rx_data_p[0] == 0x01 && uart_rx_data_p[1] == 0x33 && uart_rx_data_p[2] == 0x20 && ((uart_rx_data_p[5] == 0x03)||(uart_rx_data_p[5] == 0x04))){
                    le_rx_test_v2_longrange_flag = 1;
                    //[Enable AHI1 NON_LR]
                    TRACE(0, "Long range");
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 0, 0x0c02);
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 16, 0x0200);
                }else{
                    //[Disable AHI1 LR]
                    TRACE(0, "Non Long range");
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 0, 0x0c03);
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 16, 0x0200);
                }

#ifdef VCO_TEST_TOOL
                if(btdrv_vco_test_bridge_intsys_callback(uart_rx_data_p))
                {
                    uart_rx_len =0;
                    continue ;
                }
#endif
                hal_intersys_send(br_intersys, HAL_INTERSYS_MSG_HCI, uart_rx_data_p, uart_rx_len);
            } else {
                bt_tx_done = true;
            }
        }
        if (uart_tx_done) {
            uart_tx_done = false;
            bt_rx_len = 0;
            hal_intersys_start_recv(br_intersys);
        }
        if (bt_rx_done) {
            bt_rx_done = false;
            if (bt_rx_len > 0) {
                hal_uart_dma_send(br_uart, bt_rx_data_p, bt_rx_len, NULL, NULL);
            } else {
                uart_tx_done = true;
            }
        }
        if (bt_tx_done) {
            bt_tx_done = false;
            hal_uart_dma_recv_mask(br_uart, uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN, NULL, NULL,&mask);
        }
#ifdef VCO_TEST_TOOL
        if(btdrv_get_vco_test_process_flag())
        {
            btdrv_vco_test_process(btdrv_get_vco_test_process_flag());
            vco_test_simu_bt_rx();
        }
#endif
    }
}

extern void btdrv_ins_patch_test_init(void);
void btdrv_hci_bridge_start(void)
{
    int ret;

    btdrv_testmode_start();

    syspool_init();
    syspool_get_buff(&uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN);
    syspool_get_buff(&bt_rx_data_p, BT_UART_BRIDGE_BUF_LEN);
    btdrv_ins_patch_test_init();
    if(!hci_rx_wait_sem)
        hci_rx_wait_sem = osSemaphoreCreate(osSemaphore(hci_rx_wait), 0);

    hal_intersys_close(HAL_INTERSYS_ID_0,HAL_INTERSYS_MSG_HCI);
    ret = hal_intersys_open(br_intersys, HAL_INTERSYS_MSG_HCI, bt_rx, bt_tx, false);
    if (ret)
    {
        TRACE(0,"Failed to open intersys ret %d", ret);
        return;
    }
    TRACE(0,"BT MP PATCH download patch already 1:, standby");
}


void btdrv_hci_bridge_loop(uint8_t *hci_tx_p,uint8_t hci_tx_len,uint8_t *hci_rx_p,uint8_t *hci_rx_len)
{
    uint8_t SendFlag = 1;
    int32_t status;
    TRACE(3,"hci_tx_p %p hci_rx_p %p hci_rx_len %p",hci_tx_p,hci_rx_p,hci_rx_len);
    hal_intersys_start_recv(br_intersys);
    while(1)
    {
        if(hci_tx_len >0)
        {
            if(SendFlag)
            {
                SendFlag = 0;
                DUMP8("%02x ",hci_tx_p,hci_tx_len);
                if(hci_tx_p[0] == 0x01 && hci_tx_p[1] == 0x33 && hci_tx_p[2] == 0x20 && ((hci_tx_p[5] == 0x03)||(hci_tx_p[5] == 0x04))){
                    //[Enable AHI1 NON_LR]
                    TRACE(0, "Long range");
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 0, 0x0c02);
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 16, 0x0200);
                }else{
                    //[Disable AHI1 LR]
                    TRACE(0, "Non Long range");
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 0, 0x0c03);
                    BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 16, 0x0200);
                }
                hal_intersys_send(br_intersys, HAL_INTERSYS_MSG_HCI, hci_tx_p, hci_tx_len);
            }
        }
        else
        {
            bt_tx_done = true;
        }
        status = osSemaphoreWait(hci_rx_wait_sem, 1000);
        if(status <= 0)
        {
            bt_drv_reg_op_crash_dump();
            ASSERT(0, "%s osEventTimeout", __func__)
        }
        if (bt_rx_done)
        {
            bt_rx_done = false;
            if (bt_rx_len > 0)
            {
                if (bt_rx_data_p[0] == 0x04 && bt_rx_data_p[1] == 0xFF)
                {
                    hal_intersys_start_recv(br_intersys);
                    bt_rx_len = 0;
                    continue;
                }
                else
                {
                    memcpy(hci_rx_p,bt_rx_data_p,bt_rx_len);
                    *hci_rx_len = bt_rx_len;
                }
            }
            bt_rx_len = 0;
            break;
        }
    }
}

void btdrv_hci_receive_loop(uint8_t *hci_rx_p,uint8_t *hci_rx_len)
{
    int32_t status;
    hal_intersys_start_recv(br_intersys);
    while(1)
    {
        status = osSemaphoreWait(hci_rx_wait_sem, 1000);
        if(status <= 0)
        {
            bt_drv_reg_op_crash_dump();
            ASSERT(0, "%s osEventTimeout", __func__)
        }
        if (bt_rx_done)
        {
            bt_rx_done = false;
            if (bt_rx_len > 0)
            {
                if (bt_rx_data_p[0] == 0x04 && bt_rx_data_p[1] == 0xFF)
                {
                    hal_intersys_start_recv(br_intersys);
                    bt_rx_len = 0;
                    continue;
                }
                else
                {
                    memcpy(hci_rx_p,bt_rx_data_p,bt_rx_len);
                    *hci_rx_len = bt_rx_len;
                }
            }
            bt_rx_len = 0;
            break;
        }
    }
}

void ble_nosignal_start(void)
{
    int ret;
    union HAL_UART_IRQ_T mask;
    TRACE(0,"ble_nosignal_start");
    hal_iomux_set_uart2();
    ret = hal_uart_open(HAL_UART_ID_2, &uart_cfg);
    if (ret)
    {
        TRACE(0,"Failed to open uart");
        return;
    }

    hal_uart_irq_set_dma_handler(HAL_UART_ID_2, uart_rx, uart_tx);
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    mask.RT = 1;
    hal_uart_dma_recv_mask(HAL_UART_ID_2, uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN, NULL, NULL,&mask);
    hal_intersys_start_recv(br_intersys);
}

void ble_nosignal_uart_bridge_loop(void)
{
    union HAL_UART_IRQ_T mask;
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    mask.RT = 1;

    if (uart_rx_done)
    {
        TRACE(0,"receive uart2 data");
        DUMP8("%02x ",uart_rx_data_p,uart_rx_len);
        uart_rx_done = false;

        if(uart_rx_data_p[0] == 0x01 && uart_rx_data_p[1] == 0x33 && uart_rx_data_p[2] == 0x20 && ((uart_rx_data_p[5] == 0x03)||(uart_rx_data_p[5] == 0x04))){
            //[Enable AHI1 NON_LR]
            TRACE(0, "Long range");
            BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 0, 0x0c02);
            BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 16, 0x0200);
        }else{
            //[Disable AHI1 LR]
            TRACE(0, "Non Long range");
            BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 0, 0x0c03);
            BTDIGITAL_REG_SET_FIELD(0xD035000C, 0xffff, 16, 0x0200);
        }

        if (uart_rx_len > 0)
        {
            hal_intersys_send(br_intersys, HAL_INTERSYS_MSG_HCI, uart_rx_data_p, uart_rx_len);
        }
        else
        {
            bt_tx_done = true;
        }
    }
    if (uart_tx_done)
    {
         uart_tx_done = false;
        bt_rx_len = 0;
        hal_intersys_start_recv(br_intersys);
    }
    if (bt_rx_done)
    {
        TRACE(0,"receive HCI data");
        DUMP8("%02x ",bt_rx_data_p,bt_rx_len);
        bt_rx_done = false;
        if (bt_rx_len > 0)
        {
            hal_uart_dma_send(HAL_UART_ID_2, bt_rx_data_p, bt_rx_len, NULL, NULL);
        }
        else
        {
            uart_tx_done = true;
        }
    }
    if (bt_tx_done)
    {
         bt_tx_done = false;
        hal_uart_dma_recv_mask(HAL_UART_ID_2, uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN, NULL, NULL,&mask);
    }
}

void ble_nosignal_close(void)
{
    hal_uart_close(HAL_UART_ID_2);
}

