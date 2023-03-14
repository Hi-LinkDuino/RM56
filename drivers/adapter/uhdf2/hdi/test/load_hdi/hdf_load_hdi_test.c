/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdf_log.h"
#include "hdf_load_hdi.h"
#include "osal_time.h"

static int32_t g_testItemCnt = 0;
static int32_t g_testItemFailCnt = 0;

#define UT_TEST_CHECK_RET(val, flag) do { \
    if (!(val)) { \
        HDF_LOGE("[HDF_TEST] %{public}s line:%{public}d HDF_TEST_FAIL", __func__, __LINE__); \
        printf("[HDF_TEST] %s line:%d HDF_TEST_FAIL\r\n", __func__, __LINE__); \
        g_testItemFailCnt++; \
    } else if ((flag)) { \
        HDF_LOGD("[HDF_TEST] %{public}s line:%{public}d HDF_TEST_PASS", __func__, __LINE__); \
        printf("[HDF_TEST] %s line:%d HDF_TEST_PASS\r\n", __func__, __LINE__); \
    } \
    g_testItemCnt++; \
} while (0)

#define SERRVICE_A_RESULT 100
#define SERRVICE_B_RESULT 101
#define HDI_LOAD_TIME_RANGE 10
#define HDI_LOAD_PRESSURE_TIME 1000
#define HDF_MICRO_UNIT 1000000

struct HdiObject *g_hdi[HDI_LOAD_PRESSURE_TIME];

#define HDF_LOG_TAG hdf_hdi_test

struct HdiSampleA {
    struct HdiBase base;
    int (*ServiceA)();
    int (*ServiceB)(int para);
};

void PrintTestResult()
{
    HDF_LOGE("[HDF_TEST] %{public}s test items: %{public}d", __func__, g_testItemCnt);
    HDF_LOGE("[HDF_TEST] %{public}s test PASS: %{public}d", __func__, g_testItemCnt - g_testItemFailCnt);
    HDF_LOGE("[HDF_TEST] %{public}s test FAIL: %{public}d", __func__, g_testItemFailCnt);
    printf("[HDF_TEST] %s test items: %d\r\n", __func__, g_testItemCnt);
    printf("[HDF_TEST] %s test PASS: %d\r\n", __func__, g_testItemCnt - g_testItemFailCnt);
    printf("[HDF_TEST] %s test FAIL: %d\r\n", __func__, g_testItemFailCnt);
}

/* HDF_LOAD_HDI_BaseFunction_AR_001/HDF_LOAD_HDI_BaseFunction_AR_002 */
static void HdiOpenTest()
{
    int32_t ret;
    OsalTimespec time1 = {0, 0};
    OsalTimespec time2 = {0, 0};
    OsalTimespec diff = {0, 0};
    struct HdiObject *hdi = NULL;
    struct HdiSampleA *sampleA = NULL;

    OsalGetTime(&time1);

    hdi = LoadHdi("hdf_test_load_hdi_driver.so", 1);
    OsalGetTime(&time2);
    UT_TEST_CHECK_RET(hdi != NULL, true);
    if (hdi == NULL) {
        return;
    }
    sampleA = (struct HdiSampleA *)hdi->hdiBase;
    UT_TEST_CHECK_RET(sampleA != NULL, true);
    ret = sampleA->ServiceA();
    HDF_LOGD("%{public}s %{public}d", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_A_RESULT, true);
    ret = sampleA->ServiceB(0);
    HDF_LOGD("%{public}s %{public}d", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_B_RESULT, true);
    CloseHdi(hdi);
    OsalDiffTime(&time1, &time2, &diff);
    HDF_LOGD("%{public}s load one times: use %{public}lu s %{public}lu us", __func__, diff.sec, diff.usec);
    printf("[HDF_TEST] %s load one times: use %lu s %lu us\r\n", __func__, diff.sec, diff.usec);
    UT_TEST_CHECK_RET((diff.usec / HDF_KILO_UNIT) <= HDI_LOAD_TIME_RANGE, true);
}

/* HDF_LOAD_HDI_BaseFunction_AR_003 */
static void HdiReCloseTest()
{
    int32_t ret;
    struct HdiObject *hdi = NULL;
    struct HdiSampleA *sampleA = NULL;

    hdi = LoadHdi("hdf_test_load_hdi_driver.so", 1);
    UT_TEST_CHECK_RET(hdi != NULL, true);
    if (hdi == NULL) {
        return;
    }
    sampleA = (struct HdiSampleA *)hdi->hdiBase;
    UT_TEST_CHECK_RET(sampleA != NULL, true);
    ret = sampleA->ServiceA();
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_A_RESULT, true);
    ret = sampleA->ServiceB(0);
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_B_RESULT, true);
    CloseHdi(hdi);
    CloseHdi(hdi);
}

/* HDF_LOAD_HDI_BaseFunction_AR_004 */
static void HdiOpenVersionErrorTest()
{
    int32_t ret;
    struct HdiObject *hdi = NULL;
    struct HdiSampleA *sampleA = NULL;

    hdi = LoadHdi("hdf_test_load_hdi_driver.so", 0);
    UT_TEST_CHECK_RET(hdi == NULL, true);
    if (hdi == NULL) {
        return;
    }
    sampleA = (struct HdiSampleA *)hdi->hdiBase;
    UT_TEST_CHECK_RET(sampleA == NULL, true);
    ret = sampleA->ServiceA();
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret != SERRVICE_A_RESULT, true);
    ret = sampleA->ServiceB(0);
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret != SERRVICE_B_RESULT, true);
    CloseHdi(hdi);
}

/* HDF_LOAD_HDI_BaseFunction_AR_005 */
static void HdiOpenNameErrorTest()
{
    int32_t ret;
    struct HdiObject *hdi = NULL;
    struct HdiSampleA *sampleA = NULL;

    hdi = LoadHdi("hdf_test_load_hdi_service_error.so", 1);
    UT_TEST_CHECK_RET(hdi == NULL, true);
    if (hdi == NULL) {
        return;
    }
    sampleA = (struct HdiSampleA *)hdi->hdiBase;
    UT_TEST_CHECK_RET(sampleA == NULL, true);
    ret = sampleA->ServiceA();
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret != SERRVICE_A_RESULT, true);
    ret = sampleA->ServiceB(0);
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret != SERRVICE_B_RESULT, true);
    CloseHdi(hdi);
}

/* HDF_LOAD_HDI_BaseFunction_AR_006 */
static void HdiOpenErrorTest()
{
    int32_t ret;
    struct HdiObject *hdi = NULL;
    struct HdiSampleA *sampleA = NULL;

    hdi = LoadHdi("hdf_test_load_hdi_service_error.so", 0);
    UT_TEST_CHECK_RET(hdi == NULL, true);
    if (hdi == NULL) {
        return;
    }
    sampleA = (struct HdiSampleA *)hdi->hdiBase;
    UT_TEST_CHECK_RET(sampleA == NULL, true);
    ret = sampleA->ServiceA();
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret != SERRVICE_A_RESULT, true);
    ret = sampleA->ServiceB(0);
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret != SERRVICE_B_RESULT, true);
    CloseHdi(hdi);
}

/* HDF_LOAD_HDI_BaseFunction_AR_007 HDF_LOAD_HDI_BaseFunction_AR_008 */
static void HdiOpenMoreTest()
{
    int32_t ret;
    struct HdiSampleA *sampleA = NULL;
    struct HdiObject *hdi = NULL;
    OsalTimespec time1 = {0, 0};
    OsalTimespec time2 = {0, 0};
    OsalTimespec diff = {0, 0};
    uint64_t totalUsec;

    OsalGetTime(&time1);
    for (int i = 0; i < HDI_LOAD_PRESSURE_TIME; i++) {
        g_hdi[i] = LoadHdi("hdf_test_load_hdi_driver.so", 1);
        UT_TEST_CHECK_RET(g_hdi[i] != NULL, false);
    }
    OsalGetTime(&time2);
    OsalDiffTime(&time1, &time2, &diff);
    totalUsec = diff.sec * HDF_MICRO_UNIT + diff.usec;
    HDF_LOGD("%{public}s load %{public}d times: use %{public}lu s %{public}lu us",
        __func__, HDI_LOAD_PRESSURE_TIME, diff.sec, diff.usec);
    printf("[HDF_TEST] %s load %d times: use %lu s %lu us\r\n", __func__, HDI_LOAD_PRESSURE_TIME, diff.sec, diff.usec);
    UT_TEST_CHECK_RET((totalUsec / HDF_KILO_UNIT / HDI_LOAD_PRESSURE_TIME) <= HDI_LOAD_TIME_RANGE, true);

    for (int i = 0; i < HDI_LOAD_PRESSURE_TIME; i++) {
        hdi = g_hdi[i];
        if (hdi == NULL) {
            continue;
        }
        sampleA = (struct HdiSampleA *)hdi->hdiBase;
        UT_TEST_CHECK_RET(sampleA != NULL, false);
        ret = sampleA->ServiceA();
        HDF_LOGD("%{public}s %{public}d ", __func__, ret);
        UT_TEST_CHECK_RET(ret == SERRVICE_A_RESULT, false);
        ret = sampleA->ServiceB(0);
        HDF_LOGD("%{public}s %{public}d ", __func__, ret);
        UT_TEST_CHECK_RET(ret == SERRVICE_B_RESULT, false);
    }
    OsalGetTime(&time1);
    for (int i = 0; i < HDI_LOAD_PRESSURE_TIME; i++) {
        hdi = g_hdi[i++];
        CloseHdi(hdi);
    }
    OsalGetTime(&time2);
    OsalDiffTime(&time1, &time2, &diff);
    totalUsec = diff.sec * HDF_MICRO_UNIT + diff.usec;
    HDF_LOGD("%{public}s close %{public}d times: use %{public}lu s %{public}lu us",
        __func__, HDI_LOAD_PRESSURE_TIME, diff.sec, diff.usec);
    printf("[HDF_TEST] %s close %d times: use %lu s %lu us\r\n", __func__, HDI_LOAD_PRESSURE_TIME, diff.sec, diff.usec);
    UT_TEST_CHECK_RET((totalUsec / HDF_KILO_UNIT / HDI_LOAD_PRESSURE_TIME) <= HDI_LOAD_TIME_RANGE, true);
}

/* HDF_LOAD_HDI_BaseFunction_AR_009 */
static void HdiOpenTwoSoTest()
{
    int32_t ret;
    struct HdiObject *hdiA = NULL;
    struct HdiObject *hdiB = NULL;
    struct HdiSampleA *sampleA = NULL;
    struct HdiSampleA *sampleB = NULL;

    hdiA = LoadHdi("hdf_test_load_hdi_driver.so", 1);
    UT_TEST_CHECK_RET(hdiA != NULL, true);
    if (hdiA == NULL) {
        return;
    }
    sampleA = (struct HdiSampleA *)hdiA->hdiBase;
    UT_TEST_CHECK_RET(sampleA != NULL, true);
    ret = sampleA->ServiceA();
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_A_RESULT, true);
    ret = sampleA->ServiceB(0);
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_B_RESULT, true);

    hdiB = LoadHdi("hdf_test_load_hdi_driver_b.so", 1);
    UT_TEST_CHECK_RET(hdiB != NULL, true);
    if (hdiB == NULL) {
        return;
    }
    sampleB = (struct HdiSampleA *)hdiB->hdiBase;
    UT_TEST_CHECK_RET(sampleB != NULL, true);
    ret = sampleB->ServiceA();
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_A_RESULT, true);
    ret = sampleB->ServiceB(0);
    HDF_LOGD("%{public}s %{public}d ", __func__, ret);
    UT_TEST_CHECK_RET(ret == SERRVICE_B_RESULT, true);

    CloseHdi(hdiA);
    CloseHdi(hdiB);
}

int main(void)
{
    HdiOpenTest();
    HdiReCloseTest();
    HdiOpenVersionErrorTest();
    HdiOpenNameErrorTest();
    HdiOpenErrorTest();
    HdiOpenMoreTest();
    HdiOpenTwoSoTest();
    PrintTestResult();
    return 0;
}

