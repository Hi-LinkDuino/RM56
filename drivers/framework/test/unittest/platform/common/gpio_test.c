/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "gpio_test.h"
#include "gpio_if.h"
#include "hdf_base.h"
#include "hdf_io_service_if.h"
#include "hdf_log.h"
#include "osal_irq.h"
#include "osal_time.h"
#include "securec.h"

#define HDF_LOG_TAG gpio_test

#define GPIO_TEST_IRQ_TIMEOUT 1000
#define GPIO_TEST_IRQ_DELAY   200

static int32_t GpioTestGetConfig(struct GpioTestConfig *config)
{
    int32_t ret;
    struct HdfSBuf *reply = NULL;
    struct HdfIoService *service = NULL;
    const void *buf = NULL;
    uint32_t len;

    HDF_LOGD("%s: enter!", __func__);
    service = HdfIoServiceBind("GPIO_TEST");
    if (service == NULL) {
        HDF_LOGE("%s: failed to bind gpio test server", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    reply = HdfSbufObtain(sizeof(*config) + sizeof(uint64_t));
    if (reply == NULL) {
        HDF_LOGE("%s: failed to obtain reply", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = service->dispatcher->Dispatch(&service->object, 0, NULL, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: remote dispatch fail:%d", __func__, ret);
    }

    if (!HdfSbufReadBuffer(reply, &buf, &len)) {
        HDF_LOGE("%s: read buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (len != sizeof(*config)) {
        HDF_LOGE("%s: config size:%u, but read size:%u!", __func__, sizeof(*config), len);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    if (memcpy_s(config, sizeof(*config), buf, sizeof(*config)) != EOK) {
        HDF_LOGE("%s: memcpy buf failed", __func__);
        HdfSbufRecycle(reply);
        return HDF_ERR_IO;
    }

    HdfSbufRecycle(reply);
    HDF_LOGD("%s: exit", __func__);
    HdfIoServiceRecycle(service);
    return HDF_SUCCESS;
}

static struct GpioTester *GpioTesterGet(void)
{
    int32_t ret;
    static struct GpioTester tester;
    static struct GpioTester *pTester = NULL;

    if (pTester != NULL) {
        return pTester;
    }

    ret = GpioTestGetConfig(&tester.cfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get config failed:%d", __func__, ret);
        return NULL;
    }
    HDF_LOGI("%s: test cfg: gpio=%u, gpioIrq=%u, testUserApi=%u", __func__,
        tester.cfg.gpio, tester.cfg.gpioIrq, tester.cfg.testUserApi);

    pTester = &tester;
    return pTester;
}

static int32_t GpioTestSetUp(void)
{
    int32_t ret;
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = GpioGetDir(tester->cfg.gpio, &tester->oldDir);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get old dir fail! ret:%d", __func__, ret);
        return ret;
    }
    ret = GpioRead(tester->cfg.gpio, &tester->oldVal);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read old val fail! ret:%d", __func__, ret);
        return ret;
    }

    tester->fails = 0;
    tester->irqCnt = 0;
    tester->irqTimeout = GPIO_TEST_IRQ_TIMEOUT;
    return HDF_SUCCESS;
}

static int32_t GpioTestTearDown(void)
{
    int ret;
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = GpioSetDir(tester->cfg.gpio, tester->oldDir);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set old dir fail! ret:%d", __func__, ret);
        return ret;
    }
    if (tester->oldDir == GPIO_DIR_IN) {
        return HDF_SUCCESS;
    }
    ret = GpioWrite(tester->cfg.gpio, tester->oldVal);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write old val fail! ret:%d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t GpioTestSetGetDir(void)
{
    int32_t ret;
    uint16_t dirSet;
    uint16_t dirGet;
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    dirSet = GPIO_DIR_OUT;
    dirGet = GPIO_DIR_IN;

    ret = GpioSetDir(tester->cfg.gpio, dirSet);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set dir fail! ret:%d", __func__, ret);
        return ret;
    }
    ret = GpioGetDir(tester->cfg.gpio, &dirGet);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get dir fail! ret:%d", __func__, ret);
        return ret;
    }
    if (dirSet != dirGet) {
        HDF_LOGE("%s: set dir:%u, but get:%u", __func__, dirSet, dirGet);
        return HDF_FAILURE;
    }
    /* change the value and test one more time */
    dirSet = GPIO_DIR_IN;
    dirGet = GPIO_DIR_OUT;
    ret = GpioSetDir(tester->cfg.gpio, dirSet);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set dir fail! ret:%d", __func__, ret);
        return ret;
    }
    ret = GpioGetDir(tester->cfg.gpio, &dirGet);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get dir fail! ret:%d", __func__, ret);
        return ret;
    }
    if (dirSet != dirGet) {
        HDF_LOGE("%s: set dir:%hu, but get:%hu", __func__, dirSet, dirGet);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GpioTestWriteRead(void)
{
    int32_t ret;
    uint16_t valWrite;
    uint16_t valRead;
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = GpioSetDir(tester->cfg.gpio, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set dir fail! ret:%d", __func__, ret);
        return ret;
    }
    valWrite = GPIO_VAL_LOW;
    valRead = GPIO_VAL_HIGH;

    ret = GpioWrite(tester->cfg.gpio, valWrite);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write val:%hu fail! ret:%d", __func__, valWrite, ret);
        return ret;
    }
    ret = GpioRead(tester->cfg.gpio, &valRead);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail! ret:%d", __func__, ret);
        return ret;
    }
    if (valWrite != valRead) {
        HDF_LOGE("%s: write:%u, but get:%u", __func__, valWrite, valRead);
        return HDF_FAILURE;
    }
    /* change the value and test one more time */
    valWrite = GPIO_VAL_HIGH;
    valRead = GPIO_VAL_LOW;
    ret = GpioWrite(tester->cfg.gpio, valWrite);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: write val:%u fail! ret:%d", __func__, valWrite, ret);
        return ret;
    }
    ret = GpioRead(tester->cfg.gpio, &valRead);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read fail! ret:%d", __func__, ret);
        return ret;
    }
    if (valWrite != valRead) {
        HDF_LOGE("%s: write:%u, but get:%u", __func__, valWrite, valRead);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GpioTestIrqHandler(uint16_t gpio, void *data)
{
    struct GpioTester *tester = (struct GpioTester *)data;

    HDF_LOGE("%s: >>>>>>>>>>>>>>>>>>>>>enter gpio:%hu<<<<<<<<<<<<<<<<<<<<<<", __func__, gpio);
    if (tester != NULL) {
        tester->irqCnt++;
        return GpioDisableIrq(gpio);
    }
    return HDF_FAILURE;
}

static inline void GpioTestHelperInversePin(uint16_t gpio, uint16_t mode)
{
    uint16_t dir = 0;
    uint16_t valRead = 0;

    (void)GpioRead(gpio, &valRead);
    (void)GpioWrite(gpio, (valRead == GPIO_VAL_LOW) ? GPIO_VAL_HIGH : GPIO_VAL_LOW);
    (void)GpioRead(gpio, &valRead);
    (void)GpioGetDir(gpio, &dir);
    HDF_LOGE("%s, gpio:%u, val:%u, dir:%u, mode:%x", __func__, gpio, valRead, dir, mode);
}

static int32_t GpioTestIrqSharedFunc(struct GpioTester *tester, uint16_t mode, bool inverse)
{
    int32_t ret;
    uint32_t timeout;

    HDF_LOGE("%s: mark gona set irq ...", __func__);
    ret = GpioSetIrq(tester->cfg.gpioIrq, mode, GpioTestIrqHandler, (void *)tester);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d", __func__, ret);
        return ret;
    }
    HDF_LOGE("%s: mark gona enable irq ...", __func__);
    ret = GpioEnableIrq(tester->cfg.gpioIrq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d", __func__, ret);
        (void)GpioUnsetIrq(tester->cfg.gpioIrq, tester);
        return ret;
    }

    HDF_LOGE("%s: mark gona inverse irq ...", __func__);
    for (timeout = 0; tester->irqCnt <= 0 && timeout <= tester->irqTimeout;
        timeout += GPIO_TEST_IRQ_DELAY) {
        if (inverse) {
            // maybe can make an inverse ...
            GpioTestHelperInversePin(tester->cfg.gpioIrq, mode);
        }
        OsalMSleep(GPIO_TEST_IRQ_DELAY);
    }
    (void)GpioUnsetIrq(tester->cfg.gpioIrq, tester);

#if defined(_LINUX_USER_) || defined(__KERNEL__)
    if (inverse) {
        HDF_LOGI("%s: do not judge edge trigger!", __func__);
        return HDF_SUCCESS;
    }
#endif
    if (tester->irqCnt <= 0) {
        HDF_LOGE("%s: set mode:%x on %u failed", __func__, mode, tester->cfg.gpioIrq);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t GpioTestIrqLevel(void)
{
    return HDF_SUCCESS;
}

static int32_t GpioTestIrqEdge(void)
{
#if defined(_LINUX_USER_) || defined(__USER__)
    (void)GpioTestIrqSharedFunc;
    return HDF_SUCCESS;
#else
    uint16_t mode;
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    /* set dir to out for self trigger on liteos */
#if defined(_LINUX_USER_) || defined(__KERNEL__)
    (void)GpioSetDir(tester->cfg.gpioIrq, GPIO_DIR_IN);
#else
    (void)GpioSetDir(tester->cfg.gpioIrq, GPIO_DIR_OUT);
#endif
    mode = GPIO_IRQ_TRIGGER_FALLING | GPIO_IRQ_TRIGGER_RISING;
    return GpioTestIrqSharedFunc(tester, mode, true);
#endif
}

int32_t GpioTestIrqThread(void)
{
#if defined(_LINUX_USER_) || defined(__USER__)
    return HDF_SUCCESS;
#else
    uint16_t mode;
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    /* set dir to out for self trigger on liteos */
#if defined(_LINUX_USER_) || defined(__KERNEL__)
    (void)GpioSetDir(tester->cfg.gpioIrq, GPIO_DIR_IN);
#else
    (void)GpioSetDir(tester->cfg.gpioIrq, GPIO_DIR_OUT);
#endif
    mode = GPIO_IRQ_TRIGGER_FALLING | GPIO_IRQ_TRIGGER_RISING | GPIO_IRQ_USING_THREAD;
    return GpioTestIrqSharedFunc(tester, mode, true);
#endif
}

static int32_t GpioTestReliability(void)
{
    uint16_t val = 0;
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    (void)GpioWrite(-1, val);              /* invalid gpio number */
    (void)GpioWrite(tester->cfg.gpio, -1);     /* invalid gpio value */

    (void)GpioRead(-1, &val);              /* invalid gpio number */
    (void)GpioRead(tester->cfg.gpio, NULL);    /* invalid pointer */

    (void)GpioSetDir(-1, val);             /* invalid gpio number */
    (void)GpioSetDir(tester->cfg.gpio, -1);    /* invalid value */

    (void)GpioGetDir(-1, &val);            /* invalid gpio number */
    (void)GpioGetDir(tester->cfg.gpio, NULL);  /* invalid pointer */

    /* invalid gpio number */
    (void)GpioSetIrq(-1, OSAL_IRQF_TRIGGER_RISING, GpioTestIrqHandler, (void *)tester);
    /* invalid irq handler */
    (void)GpioSetIrq(tester->cfg.gpioIrq, OSAL_IRQF_TRIGGER_RISING, NULL, (void *)tester);

    (void)GpioUnsetIrq(-1, NULL);          /* invalid gpio number */

    (void)GpioEnableIrq(-1);               /* invalid gpio number */

    (void)GpioDisableIrq(-1);              /* invalid gpio number */

    return HDF_SUCCESS;
}

static int32_t GpioIfPerformanceTest(void)
{
#ifdef __LITEOS__
    return HDF_SUCCESS;
#endif
    uint16_t val;
    uint64_t startMs;
    uint64_t endMs;
    uint64_t useTime;    // ms
    struct GpioTester *tester = NULL;

    tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGE("%s: get tester failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    startMs = OsalGetSysTimeMs();
    GpioRead(tester->cfg.gpio, &val);
    endMs = OsalGetSysTimeMs();

    useTime = endMs - startMs;
    HDF_LOGI("----->interface performance test:[start:%lld(ms) - end:%lld(ms) = %lld (ms)] < 1ms[%d]\r\n",
        startMs, endMs, useTime, useTime < 1 ? true : false);
    return HDF_SUCCESS;
}

struct GpioTestEntry {
    int cmd;
    int32_t (*func)(void);
    const char *name;
};

static struct GpioTestEntry g_entry[] = {
    { GPIO_TEST_SET_GET_DIR, GpioTestSetGetDir, "GpioTestSetGetDir" },
    { GPIO_TEST_WRITE_READ, GpioTestWriteRead, "GpioTestWriteRead" },
    { GPIO_TEST_IRQ_LEVEL, GpioTestIrqLevel, "GpioTestIrqLevel" },
    { GPIO_TEST_IRQ_EDGE, GpioTestIrqEdge, "GpioTestIrqEdge" },
    { GPIO_TEST_IRQ_THREAD, GpioTestIrqThread, "GpioTestIrqThread" },
    { GPIO_TEST_RELIABILITY, GpioTestReliability, "GpioTestReliability" },
    { GPIO_TEST_PERFORMANCE, GpioIfPerformanceTest, "GpioIfPerformanceTest" },
};

int32_t GpioTestExecute(int cmd)
{
    uint32_t i;
    int32_t ret = HDF_ERR_NOT_SUPPORT;

#if defined(_LINUX_USER_) || defined(__USER__)
    struct GpioTester *tester = GpioTesterGet();
    if (tester == NULL) {
        HDF_LOGI("%s: tester is null", __func__);
        return HDF_SUCCESS;
    }
    if (tester->cfg.testUserApi == 0) {
        HDF_LOGI("%s: do not test user api", __func__);
        return HDF_SUCCESS;
    }
#endif

    for (i = 0; i < sizeof(g_entry) / sizeof(g_entry[0]); i++) {
        if (g_entry[i].cmd != cmd || g_entry[i].func == NULL) {
            continue;
        }
        ret = GpioTestSetUp();
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: setup failed", __func__);
            return ret;
        }

        ret = g_entry[i].func();

        (void)GpioTestTearDown();
        break;
    }

    if (ret == HDF_ERR_NOT_SUPPORT) {
        HDF_LOGE("%s: cmd:%d not supportted", __func__, cmd);
    }

    HDF_LOGI("[%s][======cmd:%d====ret:%d======]", __func__, cmd, ret);
    return ret;
}

void GpioTestExecuteAll(void)
{
    int32_t i;
    int32_t ret;
    int32_t fails = 0;

    for (i = 0; i < GPIO_TEST_MAX; i++) {
        ret = GpioTestExecute(i);
        fails += (ret != HDF_SUCCESS) ? 1 : 0;
    }

    HDF_LOGE("%s: **********PASS:%d  FAIL:%d************\n\n",
        __func__, GPIO_TEST_MAX - fails, fails);
}
