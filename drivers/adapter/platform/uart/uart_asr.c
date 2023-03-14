/*
 * Copyright (c) 2022 ASR Microelectronics (Shanghai) Co., Ltd. All rights reserved.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart/uart_core.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "los_sem.h"
#include "osal_mem.h"
#include "duet_pinmux.h"
#include "duet_uart.h"

#define ASR_UART_NUM  DUET_UART_NUM
#define asr_uart_config_t duet_uart_config_t
#define asr_uart_dev_t duet_uart_dev_t
#define asr_uart_struct_init duet_uart_struct_init
#define asr_uart_dma_config duet_uart_dma_config
#define asr_uart_init duet_uart_init
#define asr_uart_send duet_uart_send
#define asr_uart_finalize duet_uart_finalize
#define asr_uart_start duet_uart_start
#define asr_uart_stop duet_uart_stop
#define asr_uart_set_callback duet_uart_set_callback
#define asr_uart_calc_baud duet_uart_calc_baud
#define asr_uart_interrupt_config duet_uart_interrupt_config
#define asr_uart_clear_interrupt duet_uart_clear_interrupt
#define asr_uart_get_raw_interrupt_status duet_uart_get_raw_interrupt_status
#define asr_uart_get_interrupt_status duet_uart_get_interrupt_status
#define asr_uart_get_flag_status duet_uart_get_flag_status
#define asr_uart_callback_func duet_uart_callback_func
#define asr_pinmux_config duet_pinmux_config

#define HDF_LOG_TAG             uart_asr

#define DEFAULT_BAUDRATE        115200
#define DEFAULT_DATABITS        UART_ATTR_DATABIT_8
#define DEFAULT_STOPBITS        UART_ATTR_STOPBIT_1
#define DEFAULT_PARITY          UART_ATTR_PARITY_NONE
#define CONFIG_MAX_BAUDRATE     921600
#define UART_STATE_NOT_OPENED   0
#define UART_STATE_OPENING      1
#define UART_STATE_USEABLE      2
#define UART_STATE_SUSPENED     3
#define UART_FLG_DMA_RX         (1 << 0)
#define UART_FLG_DMA_TX         (1 << 1)
#define UART_FLG_RD_BLOCK       (1 << 2)
#define UART_TRANS_TIMEOUT      1000
#define UART_RX_BUF_LEN         512

typedef int32_t (*app_uart_cfg_handler_t)(struct UartDriverData *udd);
struct UartResource {
    uint32_t port;
    uint32_t pin_tx_pin;
    uint32_t pin_tx_mux;
    uint32_t pin_rx_pin;
    uint32_t pin_rx_mux;
    uint32_t tx_rx;
};

struct UartDriverData {
    asr_uart_dev_t params;
    struct UartAttribute attr;
    struct UartResource resource;
    app_uart_cfg_handler_t config;
    int32_t count;
    int32_t state;
    uint32_t flags;
};

static uint32_t g_uartTxMutex[ASR_UART_NUM];
static uint32_t g_uartRxMutex[ASR_UART_NUM];
static uint8_t *rx_buf[ASR_UART_NUM];
static uint16_t rx_head[ASR_UART_NUM];
static uint16_t rx_tail[ASR_UART_NUM];

static void Uart0Callback(char data);
static void Uart1Callback(char data);
static void Uart2Callback(char data);

static const asr_uart_callback_func g_evtHandler[ASR_UART_NUM] = {
    Uart0Callback,
    Uart1Callback,
    Uart2Callback
};

static void Uart0Callback(char data)
{
    uint8_t *dst = rx_buf[UART0_INDEX];
    if (dst) {
        dst[rx_head[UART0_INDEX]++] = (uint8_t)data;
        rx_head[UART0_INDEX] %= UART_RX_BUF_LEN;
    }
}

static void Uart1Callback(char data)
{
    uint8_t *dst = rx_buf[UART1_INDEX];
    if (dst) {
        dst[rx_head[UART1_INDEX]++] = (uint8_t)data;
        rx_head[UART1_INDEX] %= UART_RX_BUF_LEN;
    }
}
static void Uart2Callback(char data)
{
    uint8_t *dst = rx_buf[UART2_INDEX];
    if (dst) {
        dst[rx_head[UART2_INDEX]++] = (uint8_t)data;
        rx_head[UART2_INDEX] %= UART_RX_BUF_LEN;
    }
}

static uint32_t GetUartDataBits(uint32_t attrDataBits)
{
    uint32_t dataBits;

    switch (attrDataBits) {
        case UART_ATTR_DATABIT_5:
            dataBits = DATA_5BIT;
            break;
        case UART_ATTR_DATABIT_6:
            dataBits = DATA_6BIT;
            break;
        case UART_ATTR_DATABIT_7:
            dataBits = DATA_7BIT;
            break;
        case UART_ATTR_DATABIT_8:
            dataBits = DATA_8BIT;
            break;
        default:
            dataBits = DATA_8BIT;
            break;
    }

    return dataBits;
}

static uint32_t GetUartStopBits(uint32_t attrStopBits)
{
    uint32_t stopBits;

    switch (attrStopBits) {
        case UART_ATTR_STOPBIT_1:
            stopBits = STOP_1BIT;
            break;
        case UART_ATTR_STOPBIT_2:
            stopBits = STOP_2BITS;
            break;
        default:
            stopBits = STOP_1BIT;
            break;
    }

    return stopBits;
}

static uint32_t GetUartParity(uint32_t attrParity)
{
    uint32_t parity;

    switch (attrParity) {
        case UART_ATTR_PARITY_NONE:
            parity = PARITY_NO;
            break;
        case UART_ATTR_PARITY_ODD:
            parity = PARITY_ODD;
            break;
        case UART_ATTR_PARITY_EVEN:
            parity = PARITY_EVEN;
            break;
        default:
            parity = PARITY_NO;
            break;
    }

    return parity;
}

static uint32_t GetUartFlowControl(uint32_t rts, uint32_t cts)
{
    uint32_t flow_control;

    if (!rts && !cts) {
        flow_control = FLOW_CTRL_DISABLED;
    } else if (rts && cts) {
        flow_control = FLOW_CTRL_CTS_RTS;
    } else if (rts) {
        flow_control = FLOW_CTRL_RTS;
    } else {
        flow_control = FLOW_CTRL_CTS;
    }

    return flow_control;
}

static int32_t Asr582xUartConfig(struct UartDriverData *udd)
{
    uint32_t ret;
    asr_uart_dev_t *params = NULL;

    if (udd == NULL) {
        return HDF_FAILURE;
    }
    asr_pinmux_config(udd->resource.pin_tx_pin, udd->resource.pin_tx_mux);
    asr_pinmux_config(udd->resource.pin_rx_pin, udd->resource.pin_rx_mux);
    params = &udd->params;
    params->port = udd->resource.port;
    params->config.data_width = GetUartDataBits(udd->attr.dataBits);
    params->config.stop_bits = GetUartStopBits(udd->attr.stopBits);
    params->config.parity    = GetUartParity(udd->attr.parity);
    params->config.flow_control = GetUartFlowControl(udd->attr.rts, udd->attr.cts);
    params->config.mode = udd->resource.tx_rx;
    params->priv = (void *)g_evtHandler[udd->resource.port];

    ret = asr_uart_init(params);
    if (ret != 0) {
        HDF_LOGE("%s , app uart init failed\r\n", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    int32_t ret;
    uint32_t uwRet = 0;
    uint32_t recv_len = 0;
    struct UartDriverData *udd = NULL;
    uint8_t port = 0;
    uint8_t *src = NULL;

    if ((host == NULL) || (host->priv == NULL) || (data == NULL)) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    udd = (struct UartDriverData *)host->priv;
    port = udd->resource.port;
    src = rx_buf[port];
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, port);
        return HDF_FAILURE;
    }

    LOS_MuxPend(g_uartRxMutex[port], LOS_WAIT_FOREVER);
    if (udd->flags & UART_FLG_RD_BLOCK) {
        while (recv_len != size) {
            if (rx_head[port] != rx_tail[port]) {
                data[recv_len++] = src[rx_tail[port]++];
                rx_tail[port] %= UART_RX_BUF_LEN;
            }
        }
    } else {
        while ((recv_len != size) && (rx_head[port] != rx_tail[port])) {
                data[recv_len++] = src[rx_tail[port]++];
                rx_tail[port] %= UART_RX_BUF_LEN;
        }
    }
    LOS_MuxPost(g_uartRxMutex[port]);

    return recv_len;
}

static int32_t UartHostDevWrite(struct UartHost *host, uint8_t *data, uint32_t size)
{
    int32_t ret;
    struct UartDriverData *udd = NULL;

    if ((host == NULL) || (host->priv == NULL) || (data == NULL)) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->resource.port);
        return HDF_FAILURE;
    }

    LOS_MuxPend(g_uartTxMutex[udd->resource.port], LOS_WAIT_FOREVER);
    ret = asr_uart_send(&udd->params, data, size, UART_TRANS_TIMEOUT);
    if (ret != 0) {
        LOS_MuxPost(g_uartTxMutex[udd->resource.port]);
        HDF_LOGE("%s: uart_%d send %d data failed", __func__, udd->resource.port, size);
        return HDF_FAILURE;
    }
    LOS_MuxPost(g_uartTxMutex[udd->resource.port]);

    return HDF_SUCCESS;
}

static int32_t UartHostDevGetBaud(struct UartHost *host, uint32_t *baudRate)
{
    struct UartDriverData *udd = NULL;

    if (host == NULL || host->priv == NULL || baudRate == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->resource.port);
        return HDF_FAILURE;
    }
    *baudRate = udd->params.config.baud_rate;

    return HDF_SUCCESS;
}

static int32_t UartHostDevSetBaud(struct UartHost *host, uint32_t baudRate)
{
    struct UartDriverData *udd = NULL;

    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->resource.port);
        return HDF_FAILURE;
    }
    if ((baudRate > 0) && (baudRate <= CONFIG_MAX_BAUDRATE)) {
        udd->params.config.baud_rate = baudRate;
        if (udd->config == NULL) {
            HDF_LOGE("%s: not support", __func__);
            return HDF_ERR_NOT_SUPPORT;
        }
        if (udd->config(udd) != HDF_SUCCESS) {
            HDF_LOGE("%s: config baudrate %d failed", __func__, baudRate);
            return HDF_FAILURE;
        }
    } else {
        HDF_LOGE("%s: invalid baudrate, which is:%d", __func__, baudRate);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UartHostDevGetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    struct UartDriverData *udd = NULL;

    if (host == NULL || host->priv == NULL || attribute == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        return HDF_FAILURE;
    }

    *attribute = udd->attr;

    return HDF_SUCCESS;
}

static int32_t UartHostDevSetAttribute(struct UartHost *host, struct UartAttribute *attribute)
{
    struct UartDriverData *udd = NULL;

    if (host == NULL || host->priv == NULL || attribute == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->resource.port);
        return HDF_FAILURE;
    }

    udd->attr = *attribute;
    if (udd->config == NULL) {
        HDF_LOGE("%s: not support", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (udd->config(udd) != HDF_SUCCESS) {
        HDF_LOGE("%s: config failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UartHostDevSetTransMode(struct UartHost *host, enum UartTransMode mode)
{
    struct UartDriverData *udd = NULL;

    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->resource.port);
        return HDF_FAILURE;
    }
    if (UART_MODE_RD_BLOCK == mode) {
        udd->flags |= UART_FLG_RD_BLOCK;
    } else if (UART_MODE_RD_NONBLOCK == mode) {
        udd->flags &= (~UART_FLG_RD_BLOCK);
    } else {
        HDF_LOGE("%s: uart_%d not support mode:%d", __func__, udd->resource.port, mode);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UartDevSemInit(uint32_t id)
{
    uint32_t uwRet = 0;

    uwRet = LOS_MuxCreate(&g_uartTxMutex[id]);
    if (uwRet != LOS_OK) {
        return HDF_FAILURE;
    }

    uwRet = LOS_MuxCreate(&g_uartRxMutex[id]);
    if (uwRet != LOS_OK) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static void UartDevSemDeinit(uint32_t id)
{
    if (g_uartTxMutex[id] != 0) {
        LOS_MuxDelete(g_uartTxMutex[id]);
    }

    if (g_uartRxMutex[id] != 0) {
        LOS_MuxDelete(g_uartRxMutex[id]);
    }

    g_uartTxMutex[id] = 0;
    g_uartRxMutex[id] = 0;
}

static int32_t UartHostDevInit(struct UartHost *host)
{
    struct UartDriverData *udd = NULL;
    uint32_t ret = 0;
    uint8_t *ptxBuf = NULL;

    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    udd = (struct UartDriverData *)host->priv;
    if (udd->resource.port >= ASR_UART_NUM) {
        HDF_LOGE("%s: uart id is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (udd->state == UART_STATE_NOT_OPENED) {
        udd->state = UART_STATE_OPENING;

        ptxBuf = (uint8_t *)OsalMemCalloc(UART_RX_BUF_LEN);
        if (ptxBuf == NULL) {
            HDF_LOGE("%s: alloc tx buffer failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        ret = UartDevSemInit(udd->resource.port);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: uart semaphor init failed", __func__);
            UartDevSemDeinit(udd->resource.port);
            return HDF_FAILURE;
        }

        rx_buf[udd->resource.port] = ptxBuf;
        udd->config = Asr582xUartConfig;

        if (udd->config(udd) != HDF_SUCCESS) {
            UartDevSemDeinit(udd->resource.port);
            (void)OsalMemFree(rx_buf[udd->resource.port]);
            rx_buf[udd->resource.port] = NULL;
            return HDF_FAILURE;
        }
    }

    udd->state = UART_STATE_USEABLE;
    udd->count++;
    return HDF_SUCCESS;
}

static int32_t UartHostDevDeinit(struct UartHost *host)
{
    struct UartDriverData *udd = NULL;
    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    udd = (struct UartDriverData *)host->priv;
    if ((--udd->count) != 0) {
        return HDF_SUCCESS;
    }
    asr_uart_finalize(&udd->params);
    UartDevSemDeinit(udd->resource.port);
    if (rx_buf[udd->resource.port] != NULL) {
        (void)OsalMemFree(rx_buf[udd->resource.port]);
        rx_buf[udd->resource.port] = NULL;
    }

    udd->state = UART_STATE_NOT_OPENED;
    return HDF_SUCCESS;
}

struct UartHostMethod g_uartHostMethod = {
    .Init = UartHostDevInit,
    .Deinit = UartHostDevDeinit,
    .Read = UartHostDevRead,
    .Write = UartHostDevWrite,
    .SetBaud = UartHostDevSetBaud,
    .GetBaud = UartHostDevGetBaud,
    .SetAttribute = UartHostDevSetAttribute,
    .GetAttribute = UartHostDevGetAttribute,
    .SetTransMode = UartHostDevSetTransMode,
};

static int32_t UartGetPinConfigFromHcs(struct UartDriverData *udd, const struct DeviceResourceNode *node)
{
    uint32_t resourceData;
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }

    if (iface->GetUint32(node, "port", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read port fail", __func__);
        return HDF_FAILURE;
    }
    udd->resource.port =  resourceData;

    if (iface->GetUint32(node, "pin_tx_pin", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_tx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->resource.pin_tx_pin =  resourceData;

    if (iface->GetUint32(node, "pin_tx_mux", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_tx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->resource.pin_tx_mux = resourceData;

    if (iface->GetUint32(node, "pin_rx_pin", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_rx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->resource.pin_rx_pin = resourceData;

    if (iface->GetUint32(node, "pin_rx_mux", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_rx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->resource.pin_rx_mux = resourceData;

    if (iface->GetUint32(node, "tx_rx", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read tx_rx fail", __func__);
        return HDF_FAILURE;
    }
    udd->resource.tx_rx = resourceData;

    return HDF_SUCCESS;
}

static int32_t UartDevAttach(struct UartHost *host, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct UartDriverData *udd = NULL;

    if (device->property == NULL) {
        HDF_LOGE("%s: property is null", __func__);
        return HDF_FAILURE;
    }
    udd = (struct UartDriverData *)OsalMemCalloc(sizeof(*udd));
    if (udd == NULL) {
        HDF_LOGE("%s: OsalMemCalloc udd error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = UartGetPinConfigFromHcs(udd, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(udd);
        return HDF_FAILURE;
    }

    udd->state = UART_STATE_NOT_OPENED;
    udd->config = NULL;
    udd->count = 0;

    asr_uart_struct_init(&udd->params);
    udd->params.port = udd->resource.port;
    udd->params.config.baud_rate = DEFAULT_BAUDRATE;
    udd->attr.dataBits = DEFAULT_DATABITS;
    udd->attr.stopBits = DEFAULT_STOPBITS;
    udd->attr.parity = DEFAULT_PARITY;

    host->priv = udd;
    host->num = udd->resource.port;

    return HDF_SUCCESS;
}

static void UartDevDetach(struct UartHost *host)
{
    struct UartDriverData *udd = NULL;

    if (host->priv == NULL) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return;
    }
    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_NOT_OPENED) {
        HDF_LOGE("%s: uart driver data state invalid", __func__);
        return;
    }

    (void)OsalMemFree(udd);
    host->priv = NULL;
}

static int32_t HdfUartDeviceBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return (UartHostCreate(device) == NULL) ? HDF_FAILURE : HDF_SUCCESS;
}

int32_t HdfUartDeviceInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct UartHost *host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    host = UartHostFromDevice(device);
    if (host == NULL) {
        HDF_LOGE("%s: host is null", __func__);
        return HDF_FAILURE;
    }
    ret = UartDevAttach(host, device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error", __func__);
        return HDF_FAILURE;
    }
    host->method = &g_uartHostMethod;
    return ret;
}

void HdfUartDeviceRelease(struct HdfDeviceObject *device)
{
    struct UartHost *host = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is null", __func__);
        return;
    }
    host = UartHostFromDevice(device);
    if (host == NULL) {
        HDF_LOGE("%s: host is null", __func__);
        return;
    }
    if (host->priv != NULL) {
        UartDevDetach(host);
    }
    UartHostDestroy(host);
}

struct HdfDriverEntry g_hdfUartDevice = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_UART",
    .Bind = HdfUartDeviceBind,
    .Init = HdfUartDeviceInit,
    .Release = HdfUartDeviceRelease,
};

HDF_INIT(g_hdfUartDevice);
