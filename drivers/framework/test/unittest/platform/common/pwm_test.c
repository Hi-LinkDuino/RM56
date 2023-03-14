/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "pwm_test.h"
#include "device_resource_if.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG           pwm_test
#define SEQ_OUTPUT_DELAY      100 /* Delay time of sequential output, unit: ms */
#define OUTPUT_WAVES_DELAY    1 /* Delay time of waves output, unit: second */
#define TEST_WAVES_NUMBER     10 /* The number of waves for test. */

static int32_t PwmTesterGetConfig(struct PwmTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    service = HdfIoServiceBind("PWM_TEST");
    if ((service == NULL) || (service->dispatcher == NULL) || (service->dispatcher->Dispatch == NULL)) {
        HDF_LOGE("%s: HdfIoServiceBind failed\n", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSbufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        HdfIoServiceRecycle(service);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: remote dispatch failed", __func__);
        HdfIoServiceRecycle(service);
        HdfSbufRecycle(reply);
        return ret;
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: read buf failed", __func__);
        HdfIoServiceRecycle(service);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: config size:%zu, read size:%u", __func__, sizeof(*config), len);
        HdfIoServiceRecycle(service);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: memcpy buf failed", __func__);
        HdfIoServiceRecycle(service);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    HdfIoServiceRecycle(service);
    HdfSbufRecycle(reply);
    return HDF_SUCCESS;
}

struct PwmTester *PwmTesterGet(void)
{
    int32_t ret;
    static struct PwmTester tester;

    OsalMSleep(SEQ_OUTPUT_DELAY);
    ret = PwmTesterGetConfig(&tester.config);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read config failed:%d", __func__, ret);
        return NULL;
    }

    tester.handle = PwmOpen(tester.config.num);
    if (tester.handle == NULL) {
        HDF_LOGE("%s: open pwm device:%u failed", __func__, tester.config.num);
        return NULL;
    }

    return &tester;
}

static void PwmTesterPut(struct PwmTester *tester)
{
    if (tester == NULL) {
        HDF_LOGE("%s: tester is NULL", __func__);
        return;
    }
    PwmClose(tester->handle);
    tester->handle = NULL;
    OsalMSleep(SEQ_OUTPUT_DELAY);
}

static int32_t PwmSetGetConfigTest(struct PwmTester *tester)
{
    int32_t ret;
    struct PwmConfig cfg = {0};
    uint32_t number;

    number = tester->config.cfg.number;
    tester->config.cfg.number = ((number > 0) ? 0 : TEST_WAVES_NUMBER);
    HDF_LOGI("%s: Set number %u.", __func__, tester->config.cfg.number);
    ret = PwmSetConfig(tester->handle, &(tester->config.cfg));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmSetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    OsalSleep(OUTPUT_WAVES_DELAY);
    tester->config.cfg.number = number;
    HDF_LOGI("%s: Set number %u.", __func__, tester->config.cfg.number);
    ret = PwmSetConfig(tester->handle, &(tester->config.cfg));
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmSetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    ret = PwmGetConfig(tester->handle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmGetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    if (memcmp(&cfg, &(tester->config.cfg), sizeof(cfg)) != 0) {
        HDF_LOGE("%s: [memcmp_s] failed.", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t PwmSetPeriodTest(struct PwmTester *tester)
{
    int32_t ret;
    struct PwmConfig cfg = {0};
    uint32_t period;

    period = tester->config.cfg.period + tester->originCfg.period;
    ret = PwmSetPeriod(tester->handle, period);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmSetPeriod] failed, ret %d.", __func__, ret);
        return ret;
    }

    ret = PwmGetConfig(tester->handle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmGetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    if (cfg.period != period) {
        HDF_LOGE("%s: failed: cfg.period:%u period:%u", __func__, cfg.period, period);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t PwmSetDutyTest(struct PwmTester *tester)
{
    int32_t ret;
    struct PwmConfig cfg = {0};
    uint32_t duty;

    duty = tester->config.cfg.duty+ tester->originCfg.duty;
    ret = PwmSetDuty(tester->handle, duty);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmSetDuty] failed, ret %d.", __func__, ret);
        return ret;
    }

    ret = PwmGetConfig(tester->handle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmGetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    if (cfg.duty != duty) {
        HDF_LOGE("%s: failed.", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t PwmSetPolarityTest(struct PwmTester *tester)
{
    int32_t ret;
    struct PwmConfig cfg = {0};

    tester->config.cfg.polarity = PWM_NORMAL_POLARITY;
    HDF_LOGI("%s: Test [PwmSetPolarity] polarity %u.", __func__, tester->config.cfg.polarity);
    ret = PwmSetPolarity(tester->handle, tester->config.cfg.polarity);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmSetPolarity] failed, ret %d.", __func__, ret);
        return ret;
    }

    tester->config.cfg.polarity = PWM_INVERTED_POLARITY;
    HDF_LOGI("%s: Test [PwmSetPolarity] polarity %u.", __func__, tester->config.cfg.polarity);
    ret = PwmSetPolarity(tester->handle, tester->config.cfg.polarity);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmSetPolarity] failed, ret %d.", __func__, ret);
        return ret;
    }

    ret = PwmGetConfig(tester->handle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmGetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    if (cfg.polarity != tester->config.cfg.polarity) {
        HDF_LOGE("%s: failed.", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t PwmEnableTest(struct PwmTester *tester)
{
    int32_t ret;
    struct PwmConfig cfg = {0};

    ret = PwmDisable(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmDisable] failed, ret %d.", __func__, ret);
        return ret;
    }

    HDF_LOGI("%s: Test [PwmEnable] enable.", __func__);
    ret = PwmEnable(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmEnable] failed, ret %d.", __func__, ret);
        return ret;
    }

    ret = PwmGetConfig(tester->handle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmGetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    if (cfg.status == PWM_DISABLE_STATUS) {
        HDF_LOGE("%s: failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t PwmDisableTest(struct PwmTester *tester)
{
    int32_t ret;
    struct PwmConfig cfg = {0};

    ret = PwmEnable(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmEnable] failed, ret %d.", __func__, ret);
        return ret;
    }

    HDF_LOGI("%s: Test [PwmDisable] disable.", __func__);
    ret = PwmDisable(tester->handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmDisable] failed, ret %d.", __func__, ret);
        return ret;
    }

    ret = PwmGetConfig(tester->handle, &cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: [PwmGetConfig] failed, ret %d.", __func__, ret);
        return ret;
    }

    if (cfg.status == PWM_ENABLE_STATUS) {
        HDF_LOGE("%s: failed.", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

#define TEST_PERIOD 2147483647
#define TEST_DUTY 2147483647
#define TEST_POLARITY 10
static int32_t PwmReliabilityTest(struct PwmTester *tester)
{
    struct PwmConfig cfg = {0};

    (void)PwmSetConfig(tester->handle, &(tester->config.cfg));
    (void)PwmSetConfig(tester->handle, NULL);
    (void)PwmGetConfig(tester->handle, &cfg);
    (void)PwmGetConfig(tester->handle, NULL);

    (void)PwmSetPeriod(tester->handle, 0);
    (void)PwmSetPeriod(tester->handle, TEST_PERIOD);

    (void)PwmSetDuty(tester->handle, 0);
    (void)PwmSetDuty(tester->handle, TEST_DUTY);

    (void)PwmSetPolarity(tester->handle, 0);
    (void)PwmSetPolarity(tester->handle, TEST_POLARITY);

    (void)PwmEnable(tester->handle);
    (void)PwmEnable(tester->handle);

    (void)PwmDisable(tester->handle);
    (void)PwmDisable(tester->handle);
    HDF_LOGI("%s: success.", __func__);
    return HDF_SUCCESS;
}

static int32_t PwmIfPerformanceTest(struct PwmTester *tester)
{
#ifdef __LITEOS__
    // liteos the accuracy of the obtained time is too large and inaccurate.
    if (tester == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
#endif

    struct PwmConfig cfg = {0};
    uint64_t startMs;
    uint64_t endMs;
    uint64_t useTime;    // ms

    startMs = OsalGetSysTimeMs();
    PwmGetConfig(tester->handle, &cfg);
    endMs = OsalGetSysTimeMs();

    useTime = endMs - startMs;
    HDF_LOGI("----->interface performance test:[start:%llu(ms) - end:%llu(ms) = %llu (ms)] < 1ms[%d]\r\n",
        startMs, endMs, useTime, useTime < 1 ? true : false);
    return HDF_SUCCESS;
}

struct PwmTestEntry {
    int cmd;
    int32_t (*func)(struct PwmTester *tester);
};

static struct PwmTestEntry g_entry[] = {
    { PWM_SET_PERIOD_TEST, PwmSetPeriodTest },
    { PWM_SET_DUTY_TEST, PwmSetDutyTest },
    { PWM_SET_POLARITY_TEST, PwmSetPolarityTest },
    { PWM_ENABLE_TEST, PwmEnableTest },
    { PWM_DISABLE_TEST, PwmDisableTest },
    { PWM_SET_GET_CONFIG_TEST, PwmSetGetConfigTest },
    { PWM_RELIABILITY_TEST, PwmReliabilityTest },
    { PWM_IF_PERFORMANCE_TEST, PwmIfPerformanceTest },
};

int32_t PwmTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret;
    struct PwmTester *tester = NULL;

    if (cmd > PWM_TEST_CMD_MAX) {
        HDF_LOGE("%s: invalid cmd:%d", __func__, cmd);
        return HDF_ERR_NOT_SUPPORT;
    }

    tester = PwmTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    // At first test case.
    if (cmd == PWM_SET_PERIOD_TEST) {
        ret = PwmGetConfig(tester->handle, &(tester->originCfg));
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: [PwmGetConfig] failed, ret %d.", __func__, ret);
            HDF_LOGI("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
            PwmTesterPut(tester);
            return ret;
        }
    }

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = g_entry[i].func(tester);
        break;
    }

    // At last test case.
    if (cmd == PWM_DISABLE_TEST) {
        PwmSetConfig(tester->handle, &(tester->originCfg));
    }

    HDF_LOGI("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    PwmTesterPut(tester);
    return ret;
}
