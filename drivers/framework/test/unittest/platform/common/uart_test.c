/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "uart_test.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "uart_if.h"

#define HDF_LOG_TAG uart_test

static int32_t UartTestGetConfig(struct UartTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    struct UartTestConfig *cfg;
    const void *buf = NULL;
    uint32_t len;

    service = HdfIoServiceBind("UART_TEST");
    if (service == NULL) {
        HDF_LOGE("%s: failed to bind service", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSbufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: Failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Remote dispatch failed", __func__);
        HdfSbufRecycle(reply);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, (const void **)&cfg, &len)) {
        HDF_LOGE("%s: Read buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    if (len != sizeof(*cfg)) {
        HDF_LOGE("%s: cfg size:%zu, read size:%u", __func__, sizeof(*cfg), len);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    if (memcpy_s(config, sizeof(*config), cfg, sizeof(*cfg)) != EOK) {
        HDF_LOGE("%s: Memcpy buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    
    if (!HdfSbufReadBuffer(reply, (const void **)&buf, &len)) {
        HDF_LOGE("%s: Read buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != config->len) {
        HDF_LOGE("%s: cfg size:%zu, read size:%u", __func__, sizeof(*cfg), len);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }
    config->wbuf = NULL;
    config->wbuf = (uint8_t *)OsalMemCalloc(len);
    if (config->wbuf == NULL) {
        HDF_LOGE("%s: malloc wbuf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (memcpy_s(config->wbuf, config->len, buf, len) != EOK) {
        HDF_LOGE("%s: Memcpy wbuf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    HdfSbufRecycle(reply);
    HDF_LOGD("%s: Done", __func__);
    HdfIoServiceRecycle(service);
    return HDF_SUCCESS;
}

struct UartTester *UartTesterGet(void)
{
    int32_t ret;
    static struct UartTester tester;

    ret = UartTestGetConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }
    tester.handle = UartOpen(tester.config.port);
    if (tester.handle == NULL) {
        HDF_LOGE("%s: open uart port:%u fail!", __func__, tester.config.port);
        return NULL;
    }
    return &tester;
}

static void UartTesterPut(struct UartTester *tester)
{
    if (tester == NULL || tester->handle == NULL) {
        HDF_LOGE("%s: uart handle is null", __func__);
        return;
    }
    UartClose(tester->handle);
    tester->handle = NULL;
}

static int32_t UartWriteTest(struct UartTester *tester)
{
    int32_t ret;

    ret = UartWrite(tester->handle, tester->config.wbuf, tester->config.len);
    HDF_LOGE("%s: len is %d", __func__, tester->config.len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartReadTest(struct UartTester *tester)
{
    int32_t ret;

    ret = UartSetTransMode(tester->handle, UART_MODE_RD_NONBLOCK);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: transmode error.", __func__);
        return HDF_FAILURE;
    }
    ret = UartRead(tester->handle, tester->config.rbuf, tester->config.len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

#define BAUD_921600 921600
static int32_t UartSetBaudTest(struct UartTester *tester)
{
    int32_t ret;

    ret = UartSetBaud(tester->handle, BAUD_921600);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set baud failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartGetBaudTest(struct UartTester *tester)
{
    int32_t ret;
    uint32_t baud;

    ret = UartGetBaud(tester->handle, &baud);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get baud failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: baud %u success", __func__, baud);
    return HDF_SUCCESS;
}

static int32_t UartSetAttributeTest(struct UartTester *tester)
{
    struct UartAttribute attribute;
    int32_t ret;

    attribute.dataBits = UART_ATTR_DATABIT_7;
    attribute.parity = UART_ATTR_PARITY_NONE;
    attribute.stopBits = UART_ATTR_STOPBIT_1;
    attribute.rts = UART_ATTR_RTS_DIS;
    attribute.cts = UART_ATTR_CTS_DIS;
    attribute.fifoRxEn = UART_ATTR_RX_FIFO_EN;
    attribute.fifoTxEn = UART_ATTR_TX_FIFO_EN;
    ret = UartSetAttribute(tester->handle, &attribute);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set attribute failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartGetAttributeTest(struct UartTester *tester)
{
    struct UartAttribute attribute;
    int32_t ret;

    ret = UartGetAttribute(tester->handle, &attribute);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get attribute failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("dataBits %u", attribute.dataBits);
    HDF_LOGD("parity %u", attribute.parity);
    HDF_LOGD("stopBits %u", attribute.stopBits);
    HDF_LOGD("rts %u", attribute.rts);
    HDF_LOGD("cts %u", attribute.cts);
    HDF_LOGD("fifoRxEn %u", attribute.fifoRxEn);
    HDF_LOGD("fifoTxEn %u", attribute.fifoTxEn);
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartSetTransModeTest(struct UartTester *tester)
{
    int32_t ret;

    ret = UartSetTransMode(tester->handle, UART_MODE_RD_NONBLOCK);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set transmode failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartReliabilityTest(struct UartTester *tester)
{
    uint32_t baud;
    struct UartAttribute attribute = {0};

    (void)UartSetTransMode(tester->handle, UART_MODE_RD_NONBLOCK);
    (void)UartSetTransMode(tester->handle, -1);
    (void)UartWrite(tester->handle, tester->config.wbuf, tester->config.len);
    (void)UartWrite(tester->handle, NULL, -1);
    (void)UartRead(tester->handle, tester->config.rbuf, tester->config.len);
    (void)UartRead(tester->handle, NULL, -1);
    (void)UartSetBaud(tester->handle, BAUD_921600);
    (void)UartSetBaud(tester->handle, -1);
    (void)UartGetBaud(tester->handle, &baud);
    (void)UartGetBaud(tester->handle, NULL);
    (void)UartSetAttribute(tester->handle, &attribute);
    (void)UartSetAttribute(tester->handle, NULL);
    (void)UartGetAttribute(tester->handle, &attribute);
    (void)UartGetAttribute(tester->handle, NULL);
    HDF_LOGD("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t UartIfPerformanceTest(struct UartTester *tester)
{
#ifdef __LITEOS__
    if (tester == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
#endif
    uint32_t baudRate;
    uint64_t startMs;
    uint64_t endMs;
    uint64_t useTime;    // ms

    startMs = OsalGetSysTimeMs();
    UartGetBaud(tester->handle, &baudRate);
    endMs = OsalGetSysTimeMs();

    useTime = endMs - startMs;
    HDF_LOGI("----->interface performance test:[start:%llu(ms) - end:%llu(ms) = %llu (ms)] < 1ms[%d]\r\n",
        startMs, endMs, useTime, useTime < 1 ? true : false);
    return HDF_SUCCESS;
}

struct UartTestEntry {
    int cmd;
    int32_t (*func)(struct UartTester *tester);
    const char *name;
};

static struct UartTestEntry g_entry[] = {
    { UART_TEST_CMD_WRITE, UartWriteTest, "UartWriteTest" },
    { UART_TEST_CMD_READ, UartReadTest, "UartReadTest" },
    { UART_TEST_CMD_SET_BAUD, UartSetBaudTest, "UartSetBaudTest" },
    { UART_TEST_CMD_GET_BAUD, UartGetBaudTest, "UartGetBaudTest" },
    { UART_TEST_CMD_SET_ATTRIBUTE, UartSetAttributeTest, "UartSetAttributeTest" },
    { UART_TEST_CMD_GET_ATTRIBUTE, UartGetAttributeTest, "UartGetAttributeTest" },
    { UART_TEST_CMD_SET_TRANSMODE, UartSetTransModeTest, "UartSetTransModeTest" },
    { UART_TEST_CMD_RELIABILITY, UartReliabilityTest, "UartReliabilityTest" },
    { UART_TEST_CMD_PERFORMANCE, UartIfPerformanceTest, "UartIfPerformanceTest" },
};

int32_t UartTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;
    struct UartTester *tester = NULL;

    tester = UartTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: tester is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (cmd > UART_TEST_CMD_MAX) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        ret = HDF_ERR_NOT_SUPPORT;
        HDF_LOGE("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
        UartTesterPut(tester);
        return ret;
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = g_entry[i].func(tester);
        break;
    }

    HDF_LOGE("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    UartTesterPut(tester);
    return ret;
}
