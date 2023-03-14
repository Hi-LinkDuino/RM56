/*
 * Copyright (c) 2021-2022 GOODIX.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "app_uart.h"
#include "uart/uart_core.h"
#include "uart_if.h"
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "los_sem.h"
#include "osal_mem.h"

#define HDF_LOG_TAG             uart_gr5xx

#define DEFAULT_BAUDRATE        115200
#define DEFAULT_DATABITS        UART_ATTR_DATABIT_8
#define DEFAULT_STOPBITS        UART_ATTR_STOPBIT_1
#define DEFAULT_PARITY          UART_ATTR_PARITY_NONE
#define CONFIG_MAX_BAUDRATE     921600
#define TX_BUF_SIZE             0X100
#define UART_STATE_NOT_OPENED   0
#define UART_STATE_OPENING      1
#define UART_STATE_USEABLE      2
#define UART_STATE_SUSPENED     3
#define UART_FLG_DMA_RX         (1 << 0)
#define UART_FLG_DMA_TX         (1 << 1)
#define UART_FLG_RD_BLOCK       (1 << 2)
#define UART_TRANS_TIMEOUT      1000

typedef int32_t (*app_uart_cfg_handler_t)(struct UartDriverData *udd);
struct UartDriverData {
    uint32_t id;
    uint32_t baudrate;
    int32_t count;
    struct UartAttribute attr;
    app_uart_params_t params;
    app_uart_evt_handler_t eventCallback;
    app_uart_cfg_handler_t config;
    app_uart_tx_buf_t txBuffer;
    int32_t state;
    uint32_t flags;
};

static uint32_t g_uartRxSem[APP_UART_ID_MAX];
static uint32_t g_uartTxMutex[APP_UART_ID_MAX];
static uint32_t g_uartRxMutex[APP_UART_ID_MAX];
static uint32_t g_rxNum[APP_UART_ID_MAX];

static void Uart0Callback(app_uart_evt_t *event);
static void Uart1Callback(app_uart_evt_t *event);

static const app_uart_evt_handler_t *g_evtHandler[APP_UART_ID_MAX] = {
    Uart0Callback,
    Uart1Callback
};

static void Uart0Callback(app_uart_evt_t *event)
{
    if (event->type == APP_UART_EVT_RX_DATA) {
        g_rxNum[APP_UART_ID_0] = event->data.size;
        LOS_SemPost(g_uartRxSem[APP_UART_ID_0]);
    } else if (event->type == APP_UART_EVT_ERROR) {
        LOS_SemPost(g_uartRxSem[APP_UART_ID_0]);
    }
}

static void Uart1Callback(app_uart_evt_t *event)
{
    if (event->type == APP_UART_EVT_RX_DATA) {
        g_rxNum[APP_UART_ID_1] = event->data.size;
        LOS_SemPost(g_uartRxSem[APP_UART_ID_1]);
    } else if (event->type == APP_UART_EVT_ERROR) {
        LOS_SemPost(g_uartRxSem[APP_UART_ID_1]);
    }
}

static uint32_t GetUartDataBits(uint32_t attrDataBits)
{
    uint32_t dataBits;

    switch (attrDataBits) {
        case UART_ATTR_DATABIT_5:
            dataBits = UART_DATABITS_5;
            break;
        case UART_ATTR_DATABIT_6:
            dataBits = UART_DATABITS_6;
            break;
        case UART_ATTR_DATABIT_7:
            dataBits = UART_DATABITS_7;
            break;
        case UART_ATTR_DATABIT_8:
        default:
            dataBits = UART_DATABITS_8;
            break;
    }

    return dataBits;
}

static uint32_t GetUartStopBits(uint32_t attrStopBits)
{
    uint32_t stopBits;

    switch (attrStopBits) {
        case UART_ATTR_STOPBIT_1:
            stopBits = UART_STOPBITS_1;
            break;
        case UART_ATTR_STOPBIT_1P5:
            stopBits = UART_STOPBITS_1_5;
            break;
        case UART_ATTR_STOPBIT_2:
            stopBits = UART_STOPBITS_2;
            break;
        default:
            stopBits = UART_STOPBITS_1;
            break;
    }

    return stopBits;
}

static uint32_t GetUartParity(uint32_t attrParity)
{
    uint32_t parity;

    switch (attrParity) {
        case UART_ATTR_PARITY_NONE:
            parity = UART_PARITY_NONE;
            break;
        case UART_ATTR_PARITY_ODD:
            parity = UART_PARITY_ODD;
            break;
        case UART_ATTR_PARITY_EVEN:
            parity = UART_PARITY_EVEN;
            break;
        default:
            parity = UART_PARITY_NONE;
            break;
    }

    return parity;
}

static int32_t Gr5xxUartConfig(struct UartDriverData *udd)
{
    uint32_t ret;
    app_uart_params_t *params = NULL;

    if (udd == NULL) {
        return HDF_FAILURE;
    }

    params = &udd->params;
    params->id = udd->id;
    params->init.baud_rate = udd->baudrate;

    params->init.data_bits = GetUartDataBits(udd->attr.dataBits);
    params->init.stop_bits = GetUartStopBits(udd->attr.stopBits);
    params->init.parity    = GetUartParity(udd->attr.parity);

    ret = app_uart_init(params, udd->eventCallback, &udd->txBuffer);
    if (ret != APP_DRV_SUCCESS) {
        HDF_LOGE("%s , app uart init failed\r\n", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UartHostDevRead(struct UartHost *host, uint8_t *data, uint32_t size)
{
    int32_t ret;
    uint32_t uwRet = 0;
    struct UartDriverData *udd = NULL;

    if ((host == NULL) || (host->priv == NULL) || (data == NULL)) {
        HDF_LOGE("%s: invalid parameter", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->id);
        return HDF_FAILURE;
    }

    LOS_MuxPend(g_uartRxMutex[udd->id], LOS_WAIT_FOREVER);

    g_rxNum[udd->id] = 0;
    LOS_SemPend(g_uartRxSem[udd->id], 0);
    ret = app_uart_receive_async(udd->id, data, size);
    if (ret != APP_DRV_SUCCESS) {
        HDF_LOGE("%s: uart_%d receive %d data failed", __func__, udd->id, size);
        LOS_MuxPost(g_uartRxMutex[udd->id]);
        return HDF_FAILURE;
    }

    uwRet = LOS_SemPend(g_uartRxSem[udd->id], LOS_WAIT_FOREVER);
    if (uwRet != LOS_OK)  {
        HDF_LOGE("%s: uart_%d rx sem pend failed", __func__, udd->id);
        LOS_MuxPost(g_uartRxMutex[udd->id]);
        return HDF_FAILURE;
    }

    LOS_MuxPost(g_uartRxMutex[udd->id]);

    return g_rxNum[udd->id];
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
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->id);
        return HDF_FAILURE;
    }

    LOS_MuxPend(g_uartTxMutex[udd->id], LOS_WAIT_FOREVER);
    ret = app_uart_transmit_sync(udd->id, data, size, UART_TRANS_TIMEOUT);
    if (ret != APP_DRV_SUCCESS) {
        LOS_MuxPost(g_uartTxMutex[udd->id]);
        HDF_LOGE("%s: uart_%d send %d data failed", __func__, udd->id, size);
        return HDF_FAILURE;
    }
    LOS_MuxPost(g_uartTxMutex[udd->id]);

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
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->id);
        return HDF_FAILURE;
    }
    *baudRate = udd->baudrate;

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
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->id);
        return HDF_FAILURE;
    }
    if ((baudRate > 0) && (baudRate <= CONFIG_MAX_BAUDRATE)) {
        udd->baudrate = baudRate;
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
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->id);
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
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->id);
        return HDF_FAILURE;
    }
    if (mode == UART_MODE_RD_BLOCK) {
        udd->flags |= UART_FLG_RD_BLOCK;
    } else if (mode == UART_MODE_RD_NONBLOCK) {
        HDF_LOGE("%s: uart_%d only support block mode", __func__, udd->id);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t UartDevSemInit(uint32_t id)
{
    uint32_t uwRet = 0;

    uwRet = LOS_BinarySemCreate(0, &g_uartRxSem[id]);
    if (uwRet != LOS_OK) {
        return HDF_FAILURE;
    }

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
    if (g_uartRxSem[id] != 0) {
        LOS_SemDelete(g_uartRxSem[id]);
    }

    if (g_uartTxMutex[id] != 0) {
        LOS_SemDelete(g_uartTxMutex[id]);
    }

    if (g_uartRxMutex[id] != 0) {
        LOS_SemDelete(g_uartRxMutex[id]);
    }

    g_uartRxSem[id]   = 0;
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
    if (udd->id >= APP_UART_ID_MAX) {
        HDF_LOGE("%s: uart id is greater than the maximum", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (udd->state == UART_STATE_NOT_OPENED) {
        udd->state = UART_STATE_OPENING;

        ptxBuf = (uint8_t *)OsalMemCalloc(TX_BUF_SIZE);
        if (ptxBuf == NULL) {
            HDF_LOGE("%s: alloc tx buffer failed", __func__);
            return HDF_ERR_MALLOC_FAIL;
        }

        ret = UartDevSemInit(udd->id);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: uart semaphor init failed", __func__);
            UartDevSemDeinit(udd->id);
            return HDF_FAILURE;
        }

        udd->txBuffer.tx_buf = ptxBuf;
        udd->txBuffer.tx_buf_size = TX_BUF_SIZE;
        udd->eventCallback = g_evtHandler[udd->id];
        udd->config = Gr5xxUartConfig;

        if (udd->config(udd) != HDF_SUCCESS) {
            UartDevSemDeinit(udd->id);
            (void)OsalMemFree(udd->txBuffer.tx_buf);
            udd->txBuffer.tx_buf = NULL;
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

    UartDevSemDeinit(udd->id);
    if (udd->txBuffer.tx_buf != NULL) {
        (void)OsalMemFree(udd->txBuffer.tx_buf);
        udd->txBuffer.tx_buf = NULL;
    }

    udd->state = UART_STATE_NOT_OPENED;
    return HDF_SUCCESS;
}

static int32_t UartHostDevPollEvent(struct UartHost *host, void *filep, void *table)
{
    struct UartDriverData *udd = NULL;

    if (host == NULL || host->priv == NULL) {
        HDF_LOGE("%s: host is NULL", __func__);
        return HDF_FAILURE;
    }
    udd = (struct UartDriverData *)host->priv;
    if (udd->state != UART_STATE_USEABLE) {
        HDF_LOGE("%s: uart_%d not useable", __func__, udd->id);
        return HDF_FAILURE;
    }

    return 0;
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
    .pollEvent = UartHostDevPollEvent,
};

static int32_t UartGetPinConfigFromHcs(struct UartDriverData *udd, const struct DeviceResourceNode *node)
{
    uint32_t resourceData;
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }

    if (iface->GetUint32(node, "pin_tx_type", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_tx_type fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.tx.type = resourceData;

    if (iface->GetUint32(node, "pin_tx_pin", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_tx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.tx.pin = (1 << resourceData);

    if (iface->GetUint32(node, "pin_tx_mux", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_tx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.tx.mux = resourceData;

    if (iface->GetUint32(node, "pin_tx_pull", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_tx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.tx.pull = resourceData;

    if (iface->GetUint32(node, "pin_rx_type", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_rx_type fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.rx.type = resourceData;

    if (iface->GetUint32(node, "pin_rx_pin", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_rx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.rx.pin = (1 << resourceData);

    if (iface->GetUint32(node, "pin_rx_mux", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_rx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.rx.mux = resourceData;

    if (iface->GetUint32(node, "pin_rx_pull", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read pin_rx_pin fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.pin_cfg.rx.pull = resourceData;

    return HDF_SUCCESS;
}

static int32_t UartGetDefaultConfigFromHcs(struct UartDriverData *udd, const struct DeviceResourceNode *node)
{
    uint32_t resourceData;
    struct DeviceResourceIface *iface = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);

    if (iface == NULL || iface->GetUint32 == NULL) {
        HDF_LOGE("%s: face is invalid", __func__);
        return HDF_FAILURE;
    }
    if (iface->GetUint32(node, "id", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read id fail", __func__);
        return HDF_FAILURE;
    }
    udd->id = resourceData;

    if (iface->GetUint32(node, "baudrate", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read baudrate fail", __func__);
        return HDF_FAILURE;
    }
    udd->baudrate = resourceData;

    if (iface->GetUint32(node, "use_mode_type", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read use_mode_type fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.use_mode.type = resourceData;

    if (iface->GetUint32(node, "use_mode_tx_dma_ch", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read use_mode_tx_dma_ch fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.use_mode.tx_dma_channel = resourceData;

    if (iface->GetUint32(node, "use_mode_rx_dma_ch", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read use_mode_rx_dma_ch fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.use_mode.rx_dma_channel = resourceData;

    if (iface->GetUint32(node, "rx_timeout_mode", &resourceData, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read rx_timeout_mode fail", __func__);
        return HDF_FAILURE;
    }
    udd->params.init.rx_timeout_mode = resourceData;

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

    ret = UartGetDefaultConfigFromHcs(udd, device->property);
    if (ret != HDF_SUCCESS || udd->id >= APP_UART_ID_MAX) {
        (void)OsalMemFree(udd);
        return HDF_FAILURE;
    }

    ret = UartGetPinConfigFromHcs(udd, device->property);
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(udd);
        return HDF_FAILURE;
    }

    udd->state = UART_STATE_NOT_OPENED;
    udd->config = NULL;
    udd->eventCallback = NULL;
    udd->count = 0;

    udd->params.id = udd->id;
    udd->params.init.baud_rate = udd->baudrate;
    udd->attr.dataBits = DEFAULT_DATABITS;
    udd->attr.stopBits = DEFAULT_STOPBITS;
    udd->attr.parity = DEFAULT_PARITY;

    host->priv = udd;
    host->num = udd->id;

    return HDF_SUCCESS;
}

static void Gr55xxDetach(struct UartHost *host)
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
        Gr55xxDetach(host);
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
