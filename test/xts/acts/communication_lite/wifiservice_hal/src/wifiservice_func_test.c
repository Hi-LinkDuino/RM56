/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "hctest.h"
#include "ohos_types.h"
#include "wifi_device.h"
#include "wifi_hotspot.h"
#include "cmsis_os2.h"
#include <unistd.h>

#define DEF_TIMEOUT 15
#define ONE_SECOND 1
#define LEVEL_ERROR (-1)
#define LEVEL_ONE 1
#define LEVEL_TWO 2
#define LEVEL_THREE 3
#define LEVEL_FOUR 4
#define DEF_TASK_STACK 2000
#define DEF_TASK_PRIORITY 20
#define TEST_SSID_COUNT 9

static int g_apEnableSuccess = 0;
static int g_staScanSuccess = 0;
WifiEvent g_wifiEventHandler = {0};

/**
 * callback task for wifi scan
 */
static void WifiScanStateTask(void)
{
    WifiScanInfo* info = malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    if (info == NULL) {
        printf("WifiScanStateTask:malloc fail.\n");
        return;
    }
    unsigned int checkSize = WIFI_SCAN_HOTSPOT_LIMIT;
    WifiErrorCode error = GetScanInfoList(info, &checkSize);
    if (error != WIFI_SUCCESS) {
        printf("WifiScanStateTask:get info fail, error is %d.\n", error);
    } else {
        printf("WifiScanStateTask:get scan size is %u.\n", checkSize);
        g_staScanSuccess = 1;
    }
    free(info);
    info = NULL;
}

/**
 * callback task for connection
 */
static void WifiConnectionStateTask(void)
{
    WifiLinkedInfo linkInfo = {0};
    WifiErrorCode error = GetLinkedInfo(&linkInfo);
    if (error != WIFI_SUCCESS) {
        printf("WifiConnectionChanged:get link info fail, error is %d.\n", error);
        return;
    }
    if (linkInfo.connState != WIFI_CONNECTED) {
        printf("WifiConnectionChanged:connect fail!\n");
        return;
    }
    printf("WifiConnectionChanged:connect success.\n");
}

/**
 * callback function for hotspot
 */
static void HotspotStateTask(void)
{
    StationInfo info[WIFI_MAX_STA_NUM] = {0};
    unsigned int size = WIFI_MAX_STA_NUM;
    WifiErrorCode error = GetStationList(info, &size);
    if (error != WIFI_SUCCESS) {
        printf("HotspotStaJoin:get list fail, error is %d.\n", error);
        return;
    }
    printf("HotspotStaJoin:list size is %u.\n", size);
    g_apEnableSuccess++;
}

/**
 * callback function for wifi scan
 */
static void OnWifiScanStateChangedHandler(int state, int size)
{
    if (state != WIFI_STATE_AVALIABLE) {
        printf("ScanStateChanged:state is unavailable.\n");
    } else {
        printf("ScanStateChanged:state[%d], size[%d].\n", state, size);
        osThreadAttr_t attr;
        attr.name = "WifiScanStateTask";
        attr.attr_bits = 0U;
        attr.cb_mem = NULL;
        attr.cb_size = 0U;
        attr.stack_mem = NULL;
        attr.stack_size = DEF_TASK_STACK;
        attr.priority = DEF_TASK_PRIORITY;
        if (osThreadNew((osThreadFunc_t)WifiScanStateTask, NULL, &attr) == NULL) {
            printf("ScanStateChanged:create task fail!\n");
        }
    }
}

/**
 * callback function for wifi connection
 */
static void OnWifiConnectionChangedHandler(int state, WifiLinkedInfo* info)
{
    if (info == NULL) {
        printf("WifiConnectionChanged:info is null, stat is %d.\n", state);
    } else {
        osThreadAttr_t attr;
        attr.name = "WifiConnectionStateTask";
        attr.attr_bits = 0U;
        attr.cb_mem = NULL;
        attr.cb_size = 0U;
        attr.stack_mem = NULL;
        attr.stack_size = DEF_TASK_STACK;
        attr.priority = DEF_TASK_PRIORITY;
        if (osThreadNew((osThreadFunc_t)WifiConnectionStateTask, NULL, &attr) == NULL) {
            printf("WifiConnectionStateTask:create task fail!\n");
        }
    }
}

/**
 * callback function for STA join AP
 */
static void OnHotspotStaJoinHandler(StationInfo* info)
{
    if (info == NULL) {
        printf("HotspotStaJoin:info is null.\n");
    }  else {
        osThreadAttr_t attr;
        attr.name = "HotspotStateTask";
        attr.attr_bits = 0U;
        attr.cb_mem = NULL;
        attr.cb_size = 0U;
        attr.stack_mem = NULL;
        attr.stack_size = DEF_TASK_STACK;
        attr.priority = DEF_TASK_PRIORITY;
        if (osThreadNew((osThreadFunc_t)HotspotStateTask, NULL, &attr) == NULL) {
            printf("HotspotStaJoin:create task fail!\n");
        }
    }
}

/**
 * callback function for STA leave AP
 */
static void OnHotspotStaLeaveHandler(StationInfo* info)
{
    if (info == NULL) {
        printf("HotspotStaLeave:info is null.\n");
    } else {
        g_apEnableSuccess--;
    }
}

/**
 * callback function for AP
 */
static void OnHotspotStateChangedHandler(int state)
{
    printf("HotspotStateChanged:state is %d.\n", state);
}

/**
 * common wait scan result
 */
static void WaitScanResult(void)
{
    int scanTimeout = DEF_TIMEOUT;
    while (scanTimeout > 0) {
        sleep(ONE_SECOND);
        scanTimeout--;
        if (g_staScanSuccess == 1) {
            printf("WaitScanResult:wait success[%d]s\n", (DEF_TIMEOUT - scanTimeout));
            break;
        }
    }
    if (scanTimeout <= 0) {
        printf("WaitScanResult:timeout!\n");
    }
}

/**
 * @tc.desc      : register a test suite, this test suite is used to test basic functions
 * @param        : subsystem name is communication
 * @param        : module name is wifiaware
 * @param        : test suit name is WifiAwareReliTestSuite
 */
LITE_TEST_SUIT(communication, wifiservice, WifiServiceFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL WifiServiceFuncTestSuiteSetUp(void)
{
    WifiErrorCode error;
    // check wifi stat
    int ret = IsWifiActive();
    if (ret == WIFI_STATE_AVALIABLE) {
        printf("[Setup]wifi is active, disable now...\n");
        error = DisableWifi();
        if (error == WIFI_SUCCESS) {
            printf("[Setup]disable wifi success\n");
        } else {
            TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
            printf("[Setup]disable wifi fail, please disable wifi, then run test cases!\n");
            return FALSE;
        }
    }

    // check AP stat
    ret = IsHotspotActive();
    if (ret == WIFI_HOTSPOT_ACTIVE) {
        printf("[Setup]AP is active, disable now...\n");
        error = DisableHotspot();
        if (error == WIFI_SUCCESS) {
            printf("[Setup]disable AP success\n");
        } else {
            TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
            printf("[Setup]disable AP fail, please disable ap, then run test cases!\n");
            return FALSE;
        }
    }

    // check device config
    WifiDeviceConfig config[WIFI_MAX_CONFIG_SIZE] = {0};
    unsigned int size = WIFI_MAX_CONFIG_SIZE;
    error = GetDeviceConfigs(config, &size);
    if (error != ERROR_WIFI_NOT_AVAILABLE) {
        printf("[Setup]there is device config, clear now...\n");
        int count = 0;
        for (int i = 0; i < WIFI_MAX_CONFIG_SIZE; i++) {
            if (&config[i] != NULL) {
                RemoveDevice(config[i].netId);
                count++;
            }
        }
        printf("[Setup]clear count [%d]\n", count);
    }

    // register wifi event
    g_wifiEventHandler.OnWifiScanStateChanged = OnWifiScanStateChangedHandler;
    g_wifiEventHandler.OnWifiConnectionChanged = OnWifiConnectionChangedHandler;
    g_wifiEventHandler.OnHotspotStaJoin = OnHotspotStaJoinHandler;
    g_wifiEventHandler.OnHotspotStaLeave = OnHotspotStaLeaveHandler;
    g_wifiEventHandler.OnHotspotStateChanged = OnHotspotStateChangedHandler;
    error = RegisterWifiEvent(&g_wifiEventHandler);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    if (error != WIFI_SUCCESS) {
        printf("[Setup]register wifi event fail!\n");
        return FALSE;
    }
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL WifiServiceFuncTestSuiteTearDown(void)
{
    WifiErrorCode error = UnRegisterWifiEvent(&g_wifiEventHandler);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    printf("+-------------------------------------------+\n");
    if (error != WIFI_SUCCESS) {
        return FALSE;
    }
    return TRUE;
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0100
 * @tc.name      : Test enable and disable wifi interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testEnableDisableWifi, Function | MediumTest | Level2)
{
    int stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);

    WifiErrorCode error = EnableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_AVALIABLE, stat);

    error = EnableWifi();
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_BUSY, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_AVALIABLE, stat);

    error = DisableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);

    error = DisableWifi();
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_NOT_STARTED, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0200
 * @tc.name      : Test scan and get scan info interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testScan, Function | MediumTest | Level2)
{
    unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
    WifiErrorCode error = EnableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    int stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_AVALIABLE, stat);

    WifiScanInfo* info = malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    TEST_ASSERT_NOT_NULL(info);
    error = GetScanInfoList(info, &size);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(0, size);

    g_staScanSuccess = 0;
    error = Scan();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    size = WIFI_SCAN_HOTSPOT_LIMIT;
    error = GetScanInfoList(info, &size);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    TEST_ASSERT_NOT_EQUAL(WIFI_SCAN_HOTSPOT_LIMIT, size);
    free(info);

    error = DisableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0300
 * @tc.name      : Test connect and disconnect interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testConnectDisConnect, Function | MediumTest | Level2)
{
    int netId = 0;
    WifiDeviceConfig config = {0};
    const char* ssid = "xts_execute";
    int ret = strncpy_s(config.ssid, WIFI_MAX_SSID_LEN, ssid, strlen(ssid));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);

    config.securityType = WIFI_SEC_TYPE_OPEN;
    WifiErrorCode error = AddDeviceConfig(&config, &netId);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);

    error = ConnectTo(netId);
    TEST_ASSERT_NOT_EQUAL(WIFI_SUCCESS, error);
    unsigned char mac[WIFI_MAC_LEN];
    error = GetDeviceMacAddress((unsigned char *)mac);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    error = Disconnect();
    TEST_ASSERT_NOT_EQUAL(WIFI_SUCCESS, error);
    error = RemoveDevice(netId);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0400
 * @tc.name      : Test handle device config interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testHandleDeviceConfig, Function | MediumTest | Level2)
{
    int netId = 0;
    const char* ssid0 = "TestWifi01";
    const char* ssid[TEST_SSID_COUNT] = {"TestWifi02", "TestWifi03", "TestWifi04", "TestWifi05", "TestWifi06",
    		"TestWifi07", "TestWifi08", "TestWifi09", "TestWifi10"};
    const char* ssid10 = "TestWifi11";
    const char* info = "12345678";
    unsigned char bssid[WIFI_MAC_LEN] = {0xac, 0x75, 0x1d, 0xd8, 0x55, 0xc1};
    WifiDeviceConfig config = {0};
    config.freq = 20;
    config.securityType = WIFI_SEC_TYPE_SAE;
    config.wapiPskType = WIFI_PSK_TYPE_ASCII;
    int ret = strncpy_s(config.ssid, WIFI_MAX_SSID_LEN, ssid0, strlen(ssid0));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    ret = strncpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, info, strlen(info));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    ret = memcpy_s(config.bssid, WIFI_MAC_LEN, bssid, WIFI_MAC_LEN);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    WifiErrorCode error = AddDeviceConfig(&config, &netId);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);

    for (int i = 0; i < WIFI_MAX_CONFIG_SIZE - 1; i++) {
        config.securityType = WIFI_SEC_TYPE_PSK;
        config.wapiPskType = WIFI_PSK_TYPE_HEX;
        ret = strncpy_s(config.ssid, WIFI_MAX_SSID_LEN, ssid[i], sizeof(ssid[i]));
        TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
        ret = strncpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, info, strlen(info));
        TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
        error = AddDeviceConfig(&config, &netId);
        TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
        if (error != WIFI_SUCCESS) {
            printf("Add fail[%d].\n", i);
            break;
        }
    }

    ret = strncpy_s(config.ssid, WIFI_MAX_SSID_LEN, ssid10, strlen(ssid10));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    ret = strncpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, info, strlen(info));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    config.securityType = WIFI_SEC_TYPE_PSK;
    error = AddDeviceConfig(&config, &netId);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_BUSY, error);

    WifiDeviceConfig allConfig[WIFI_MAX_CONFIG_SIZE] = {0};
    unsigned int size = WIFI_MAX_CONFIG_SIZE;
    error = GetDeviceConfigs(allConfig, &size);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(WIFI_MAX_CONFIG_SIZE, size);

    for (int i = 0; i < WIFI_MAX_CONFIG_SIZE; i++) {
        error = RemoveDevice(allConfig[i].netId);
        TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    }

    error = GetDeviceConfigs(allConfig, &size);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_NOT_AVAILABLE, error);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0500
 * @tc.name      : Test handle AP config interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testHandleHotspotConfig, Function | MediumTest | Level2)
{
    const char* ssid = "XtsTestAp";
    const char* info = "12345678";
    HotspotConfig config = {0};
    int ret = strncpy_s(config.ssid, WIFI_MAX_SSID_LEN, ssid, strlen(ssid));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    ret = strncpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, info, strlen(info));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    config.securityType = WIFI_SEC_TYPE_PSK;
    WifiErrorCode error = SetHotspotConfig(&config);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);

    HotspotConfig getConfig = {0};
    error = GetHotspotConfig(&getConfig);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(config.securityType, WIFI_SEC_TYPE_PSK);

    int band = 11;
    int bandOrig = 11;
    error = SetBand(band);
    TEST_ASSERT_NOT_EQUAL(WIFI_SUCCESS, error);
    error = GetBand(&band);
    TEST_ASSERT_NOT_EQUAL(WIFI_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(bandOrig, band);

    error = SetBand(HOTSPOT_BAND_TYPE_2G);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    error = GetBand(&band);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(HOTSPOT_BAND_TYPE_2G, band);

    HotspotConfig getConfigAgain = {0};
    error = GetHotspotConfig(&getConfigAgain);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    TEST_ASSERT_EQUAL_INT(HOTSPOT_BAND_TYPE_2G, getConfigAgain.band);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0600
 * @tc.name      : Test enable and disable AP interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testEnableDisableHotSpot, Function | MediumTest | Level2)
{
    const char* ssid = "XtsTestAp";
    const char* info = "12345678";
    HotspotConfig config = {0};
    int ret = strncpy_s(config.ssid, WIFI_MAX_SSID_LEN, ssid, strlen(ssid));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    ret = strncpy_s(config.preSharedKey, WIFI_MAX_KEY_LEN, info, strlen(info));
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, ret);
    config.securityType = WIFI_SEC_TYPE_PSK;
    WifiErrorCode error = SetHotspotConfig(&config);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);

    int stat = IsHotspotActive();
    TEST_ASSERT_EQUAL_INT(WIFI_HOTSPOT_NOT_ACTIVE, stat);
    error = EnableHotspot();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsHotspotActive();
    TEST_ASSERT_EQUAL_INT(WIFI_HOTSPOT_ACTIVE, stat);
    error = EnableHotspot();
    TEST_ASSERT_NOT_EQUAL(WIFI_SUCCESS, error);
    stat = IsHotspotActive();
    TEST_ASSERT_EQUAL_INT(WIFI_HOTSPOT_ACTIVE, stat);

    int timeout = 3;
    g_apEnableSuccess = 0;
    while (timeout > 0) {
        sleep(ONE_SECOND);
        timeout--;
        if (g_apEnableSuccess >= 1) {
            printf("Wait %d seconds.\n", (DEF_TIMEOUT - timeout));
            break;
        }
    }

    error = DisableHotspot();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsHotspotActive();
    TEST_ASSERT_EQUAL_INT(WIFI_HOTSPOT_NOT_ACTIVE, stat);
    error = DisableHotspot();
    TEST_ASSERT_NOT_EQUAL(WIFI_SUCCESS, error);
    stat = IsHotspotActive();
    TEST_ASSERT_EQUAL_INT(WIFI_HOTSPOT_NOT_ACTIVE, stat);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0700
 * @tc.name      : Test get signal Level interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testGetSignalLevel, Function | MediumTest | Level2)
{
    int level;
    int rssiNoLevel = -90;
    int rssiOf2gLevel1 = -88;
    int rssiOf2gLevel2 = -82;
    int rssiOf2gLevel3 = -75;
    int rssiOf5gLevel1 = -85;
    int rssiOf5gLevel2 = -79;
    int rssiOf5gLevel3 = -72;
    int rssiBothLevel4 = -65;

    level = GetSignalLevel(rssiNoLevel, HOTSPOT_BAND_TYPE_2G);
    TEST_ASSERT_EQUAL_INT(LEVEL_ERROR, level);
    level = GetSignalLevel(rssiOf2gLevel1, HOTSPOT_BAND_TYPE_2G);
    TEST_ASSERT_EQUAL_INT(LEVEL_ONE, level);
    level = GetSignalLevel(rssiOf2gLevel2, HOTSPOT_BAND_TYPE_2G);
    TEST_ASSERT_EQUAL_INT(LEVEL_TWO, level);
    level = GetSignalLevel(rssiOf2gLevel3, HOTSPOT_BAND_TYPE_2G);
    TEST_ASSERT_EQUAL_INT(LEVEL_THREE, level);
    level = GetSignalLevel(rssiBothLevel4, HOTSPOT_BAND_TYPE_2G);
    TEST_ASSERT_EQUAL_INT(LEVEL_FOUR, level);

    level = GetSignalLevel(rssiNoLevel, HOTSPOT_BAND_TYPE_5G);
    TEST_ASSERT_EQUAL_INT(-1, level);
    level = GetSignalLevel(rssiOf5gLevel1, HOTSPOT_BAND_TYPE_5G);
    TEST_ASSERT_EQUAL_INT(LEVEL_ONE, level);
    level = GetSignalLevel(rssiOf5gLevel2, HOTSPOT_BAND_TYPE_5G);
    TEST_ASSERT_EQUAL_INT(LEVEL_TWO, level);
    level = GetSignalLevel(rssiOf5gLevel3, HOTSPOT_BAND_TYPE_5G);
    TEST_ASSERT_EQUAL_INT(LEVEL_THREE, level);
    level = GetSignalLevel(rssiBothLevel4, HOTSPOT_BAND_TYPE_5G);
    TEST_ASSERT_EQUAL_INT(LEVEL_FOUR, level);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0800
 * @tc.name      : test adavance scan interface
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testAdvanceScanType, Function | MediumTest | Level2)
{
    WifiErrorCode error = EnableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    int stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_AVALIABLE, stat);

    int freq = 2460;
    WifiScanParams scanParams = {0};
    char bssid[WIFI_MAC_LEN] = {0xac, 0x75, 0x1d, 0xd8, 0x55, 0xc1};
    (void) strcpy_s(scanParams.ssid, sizeof(scanParams.ssid), "wifi_service_xts");
    scanParams.ssidLen = strlen(scanParams.ssid);
    scanParams.freqs = freq;
    (void) memcpy_s(scanParams.bssid, sizeof(scanParams.bssid), bssid, sizeof(bssid));

    scanParams.scanType = WIFI_SSID_SCAN;
    g_staScanSuccess = 0;
    error = AdvanceScan(&scanParams);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    scanParams.scanType = WIFI_FREQ_SCAN;
    g_staScanSuccess = 0;
    error = AdvanceScan(&scanParams);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    scanParams.scanType = WIFI_BSSID_SCAN;
    g_staScanSuccess = 0;
    error = AdvanceScan(&scanParams);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    scanParams.scanType = WIFI_BAND_SCAN;
    g_staScanSuccess = 0;
    error = AdvanceScan(&scanParams);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    error = DisableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_0900
 * @tc.name      : test adavance scan interface with invalid parameter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testAdvanceScanInvalidParam01, Function | MediumTest | Level2)
{
    WifiErrorCode error = EnableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    int stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_AVALIABLE, stat);

    g_staScanSuccess = 0;
    error = AdvanceScan(NULL);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_UNKNOWN, error);
    TEST_ASSERT_EQUAL_INT(0, g_staScanSuccess);

    WifiScanParams scanParams = {0};
    error = AdvanceScan(&scanParams);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_UNKNOWN, error);
    TEST_ASSERT_EQUAL_INT(0, g_staScanSuccess);

    error = DisableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_1000
 * @tc.name      : test adavance scan interface with different invalid scantype
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testAdvanceScanInvalidParam02, Function | MediumTest | Level2)
{
    WifiErrorCode error = EnableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    int stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_AVALIABLE, stat);

    WifiScanParams* scanParams = malloc(sizeof(WifiScanParams));
    TEST_ASSERT_NOT_NULL(scanParams);
    (void) memset_s(scanParams, sizeof(WifiScanParams), 0, sizeof(WifiScanParams));

    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_UNKNOWN, error);

    scanParams->scanType = WIFI_BSSID_SCAN;
    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_UNKNOWN, error);

    scanParams->scanType = WIFI_SSID_SCAN;
    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_UNKNOWN, error);

    (void) memset_s(scanParams, sizeof(WifiScanParams), 0, sizeof(WifiScanParams));
    (void) strcpy_s(scanParams->ssid, sizeof(scanParams->ssid), "wifi_service_xts");
    scanParams->scanType = WIFI_SSID_SCAN;
    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_UNKNOWN, error);

    scanParams->scanType = WIFI_FREQ_SCAN;
    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(ERROR_WIFI_UNKNOWN, error);

    error = DisableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);

    free(scanParams);
}

/**
 * @tc.number    : SUB_COMMUNICATION_WIFISERVICE_SDK_1100
 * @tc.name      : test adavance scan interface with different invalid scantype
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(WifiServiceFuncTestSuite, testAdvanceScanInvalidParam03, Function | MediumTest | Level2)
{
    WifiErrorCode error = EnableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    int stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_AVALIABLE, stat);

    WifiScanParams* scanParams = malloc(sizeof(WifiScanParams));
    TEST_ASSERT_NOT_NULL(scanParams);
    (void) memset_s(scanParams, sizeof(WifiScanParams), 0, sizeof(WifiScanParams));

    scanParams->scanType = WIFI_BAND_SCAN;
    g_staScanSuccess = 0;
    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    int errorType = -1; //Unnormal Type Val -> Default Type Val
    scanParams->scanType = errorType;
    g_staScanSuccess = 0;
    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    char bssid[WIFI_MAC_LEN] = {0xac, 0x75, 0x1d, 0xd8, 0x55, 0xc1};
    (void) memcpy_s(scanParams->bssid, sizeof(scanParams->bssid), bssid, sizeof(bssid));
    scanParams->scanType = WIFI_BSSID_SCAN;
    g_staScanSuccess = 0;
    error = AdvanceScan(scanParams);
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    WaitScanResult();
    TEST_ASSERT_EQUAL_INT(1, g_staScanSuccess);

    error = DisableWifi();
    TEST_ASSERT_EQUAL_INT(WIFI_SUCCESS, error);
    stat = IsWifiActive();
    TEST_ASSERT_EQUAL_INT(WIFI_STATE_NOT_AVALIABLE, stat);

    free(scanParams);
}

RUN_TEST_SUITE(WifiServiceFuncTestSuite);
