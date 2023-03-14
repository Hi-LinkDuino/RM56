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
#include "devmgr_hdi.h"
#include "osal_time.h"

static int32_t g_testItemCnt = 0;
static int32_t g_testItemFailCnt = 0;

#define HDF_USABLE_DEVICE_CNT 100
#define HDF_UNUSABLE_DEVICE_CNT 10
#define HDF_QUERY_TIME_RANGE 10
#define HDF_QUERY_DEVICE_ONE 0xffff
#define HDF_QUERY_DEVICE_MORE 1000
#define HDF_MICRO_UNIT 1000000
#define TEST_ITEM_CNT 3
#define USABLE 0
#define UNUSABLE 1
#define FREE 2

static int g_index[TEST_ITEM_CNT];
static uint64_t g_totalUsec[TEST_ITEM_CNT];

#define GET_TIME_BEGIN(time) OsalGetTime(time)
#define GET_TIME_END(index, time1, time2, diff) do { \
    OsalGetTime(time2); \
    OsalDiffTime(time1, time2, diff); \
    CalcAverageTime(index, diff); \
} while (0)

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

#define HDF_LOG_TAG hdf_query_device_test

void PrintTestResult()
{
    HDF_LOGE("[HDF_TEST] %{public}s test items: %{public}d", __func__, g_testItemCnt);
    HDF_LOGE("[HDF_TEST] %{public}s test PASS: %{public}d", __func__, g_testItemCnt - g_testItemFailCnt);
    HDF_LOGE("[HDF_TEST] %{public}s test FAIL: %{public}d", __func__, g_testItemFailCnt);
    printf("[HDF_TEST] %s test items: %d\r\n", __func__, g_testItemCnt);
    printf("[HDF_TEST] %s test PASS: %d\r\n", __func__, g_testItemCnt - g_testItemFailCnt);
    printf("[HDF_TEST] %s test FAIL: %d\r\n", __func__, g_testItemFailCnt);
}

static void CalcAverageTime(int index, const OsalTimespec *diff)
{
    g_totalUsec[index] += diff->sec * HDF_MICRO_UNIT + diff->usec;
    g_index[index]++;
}

static void CheckAverageTime()
{
    HDF_LOGD("[HDF_TEST] %{public}s line:%{public}d test query usable %{public}d time use time:%{public}lu us",
        __func__, __LINE__, g_index[USABLE], g_totalUsec[USABLE]);
    printf("[HDF_TEST] %s line:%d test query usable %d time use time: %lu us\r\n",
        __func__, __LINE__, g_index[USABLE], g_totalUsec[USABLE]);

    HDF_LOGD("[HDF_TEST] %{public}s line:%{public}d test query unusable %{public}d time use time:%{public}lu us",
        __func__, __LINE__, g_index[UNUSABLE], g_totalUsec[UNUSABLE]);
    printf("[HDF_TEST] %s line:%d test query nusable %d time use time: %lu us\r\n",
        __func__, __LINE__, g_index[UNUSABLE], g_totalUsec[UNUSABLE]);

    HDF_LOGD("[HDF_TEST] %{public}s line:%{public}d test query free %{public}d time use time:%{public}lu us",
        __func__, __LINE__, g_index[FREE], g_totalUsec[FREE]);
    printf("[HDF_TEST] %s line:%d test query free %d time use time: %lu us\r\n",
        __func__, __LINE__, g_index[FREE], g_totalUsec[FREE]);

    UT_TEST_CHECK_RET((g_totalUsec[USABLE] / HDF_KILO_UNIT / g_index[USABLE]) <= HDF_QUERY_TIME_RANGE, true);
    UT_TEST_CHECK_RET((g_totalUsec[UNUSABLE] / HDF_KILO_UNIT / g_index[UNUSABLE]) <= HDF_QUERY_TIME_RANGE, true);
    UT_TEST_CHECK_RET((g_totalUsec[FREE] / HDF_KILO_UNIT / g_index[FREE]) <= HDF_QUERY_TIME_RANGE, true);
}


void HdfFreeQueryDeviceList(struct DeviceInfoList *list)
{
    struct HDIDeviceManager *iDevmgr = HDIDeviceManagerGet();
    if (iDevmgr == NULL) {
        return;
    }

    iDevmgr->FreeQueryDeviceList(iDevmgr, list);

    HDIDeviceManagerRelease(iDevmgr);
}

int32_t HdfQueryUsableDeviceInfo(struct DeviceInfoList *list)
{
    struct HDIDeviceManager *iDevmgr = HDIDeviceManagerGet();
    if (iDevmgr == NULL) {
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    int32_t ret = iDevmgr->QueryUsableDeviceInfo(iDevmgr, list);

    HDIDeviceManagerRelease(iDevmgr);
    return ret;
}

int32_t HdfQueryUnusableDeviceInfo(struct DeviceInfoList *list)
{
    struct HDIDeviceManager *iDevmgr = HDIDeviceManagerGet();
    if (iDevmgr == NULL) {
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }

    int32_t ret = iDevmgr->QueryUnusableDeviceInfo(iDevmgr, list);

    HDIDeviceManagerRelease(iDevmgr);
    return ret;
}

/*
 * HDF_QUERY_DEVICE_BaseFunction_AR_001 HDF_QUERY_DEVICE_BaseFunction_AR_002
 * HDF_QUERY_DEVICE_BaseFunction_AR_005 HDF_QUERY_DEVICE_BaseFunction_AR_006
 */
static void QueryDeviceUsableTest(int index)
{
    int32_t ret;
    OsalTimespec time1 = { 0, 0 };
    OsalTimespec time2 = { 0, 0 };
    OsalTimespec diff = { 0, 0 };
    bool flag = (index == HDF_QUERY_DEVICE_ONE) ? true : false;

    struct DeviceInfoList list;
    struct DeviceInfoNode *devNode = NULL;
    GET_TIME_BEGIN(&time1);

    ret = HdfQueryUsableDeviceInfo(&list);
    GET_TIME_END(USABLE, &time1, &time2, &diff);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, flag);
    if (ret == HDF_SUCCESS) {
        UT_TEST_CHECK_RET(list.deviceCnt == HDF_USABLE_DEVICE_CNT, flag);
        HDF_LOGD("[HDF_TEST] %{public}s usable:%{public}d line:%{public}d", __func__, list.deviceCnt, __LINE__);
        if (flag) {
            printf("[HDF_TEST] %s usable:%d line:%d\r\n", __func__, list.deviceCnt, __LINE__);
        }
        DLIST_FOR_EACH_ENTRY(devNode, &list.list, struct DeviceInfoNode, node) {
            HDF_LOGD("usable: %{public}s %{public}d", devNode->svcName, devNode->deviceType);
        }
    } else {
        HDF_LOGE("[HDF_TEST] %{public}s line:%{public}d", __func__, __LINE__);
    }

    GET_TIME_BEGIN(&time1);
    HdfFreeQueryDeviceList(&list);
    GET_TIME_END(FREE, &time1, &time2, &diff);
}

/* HDF_QUERY_DEVICE_BaseFunction_AR_003 HDF_QUERY_DEVICE_BaseFunction_AR_004 HDF_QUERY_DEVICE_BaseFunction_AR_007 */
static void QueryDeviceUnusableTest(int index)
{
    int32_t ret;
    OsalTimespec time1 = {0, 0};
    OsalTimespec time2 = {0, 0};
    OsalTimespec diff = {0, 0};
    bool flag = (index == HDF_QUERY_DEVICE_ONE) ? true : false;

    struct DeviceInfoList list;
    struct DeviceInfoNode *devNode = NULL;
    GET_TIME_BEGIN(&time1);
    ret = HdfQueryUnusableDeviceInfo(&list);
    GET_TIME_END(UNUSABLE, &time1, &time2, &diff);
    UT_TEST_CHECK_RET(ret == HDF_SUCCESS, flag);
    if (ret == HDF_SUCCESS) {
        UT_TEST_CHECK_RET(list.deviceCnt == HDF_UNUSABLE_DEVICE_CNT, flag);
        HDF_LOGD("[HDF_TEST] %{public}s unusable:%{public}d line:%{public}d", __func__, list.deviceCnt, __LINE__);
        if (flag) {
            printf("[HDF_TEST] %s unusable:%d line:%d\r\n", __func__, list.deviceCnt, __LINE__);
        }
        DLIST_FOR_EACH_ENTRY(devNode, &list.list, struct DeviceInfoNode, node) {
            HDF_LOGD("[HDF_TEST] usable: %{public}s %{public}d", devNode->svcName, devNode->deviceType);
        }
    } else {
        HDF_LOGE("%{public}s %{public}d", __func__, __LINE__);
    }

    HdfFreeQueryDeviceList(&list);
    GET_TIME_END(FREE, &time1, &time2, &diff);
}

/* HDF_QUERY_DEVICE_BaseFunction_AR_008 */
int main(int argc, char **argv)
{
    (void)argv;
    if (argc == 1) {
        QueryDeviceUsableTest(HDF_QUERY_DEVICE_ONE);
        QueryDeviceUnusableTest(HDF_QUERY_DEVICE_ONE);
    } else {
        int testCnt = HDF_QUERY_DEVICE_MORE;
        for (int i = 0; i < testCnt; i++) {
            QueryDeviceUsableTest(i);
            QueryDeviceUnusableTest(i);
        }
    }
    CheckAverageTime();
    PrintTestResult();
    return 0;
}

