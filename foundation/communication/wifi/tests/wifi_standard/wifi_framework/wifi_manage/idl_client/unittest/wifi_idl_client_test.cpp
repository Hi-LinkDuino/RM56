/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <iostream>
#include "wifi_idl_client_test.h"
#include "securec.h"
#include "wifi_scan_param.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "WifiIdlClientTest"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiIdlClientTest, InitClientTest, TestSize.Level1)
{
    EXPECT_TRUE(mClient.InitClient() == 0);
}

HWTEST_F(WifiIdlClientTest, StartWifiTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    EXPECT_TRUE(mClient.StartWifi() == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiIdlClientTest, GetStaDeviceMacAddressTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::string mac;
    WifiErrorNo err = mClient.GetStaDeviceMacAddress(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK) << " Get device mac address is: " << mac;
}

HWTEST_F(WifiIdlClientTest, GetStaCapabilitiesTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    unsigned int capabilities = 0;
    WifiErrorNo err = mClient.GetStaCapabilities(capabilities);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiIdlClientTest, GetSupportFrequenciesTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::vector<int> freqs;
    WifiErrorNo err = mClient.GetSupportFrequencies(1, freqs);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
    for (auto iter = freqs.begin(); iter != freqs.end(); ++iter) {
        LOGD("Get frequency: %{public}d", *iter);
    }
    freqs.clear();
    err = mClient.GetSupportFrequencies(2, freqs);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
    for (auto iter = freqs.begin(); iter != freqs.end(); ++iter) {
        LOGD("Get frequency: %{public}d", *iter);
    }
    freqs.clear();
    err = mClient.GetSupportFrequencies(4, freqs);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
    for (auto iter = freqs.begin(); iter != freqs.end(); ++iter) {
        LOGD("Get frequency: %{public}d", *iter);
    }
}

HWTEST_F(WifiIdlClientTest, SetConnectMacAddrTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::string mac = "abcdefghijklmn";
    WifiErrorNo err = mClient.SetConnectMacAddr(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    err = mClient.SetConnectMacAddr(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiIdlClientTest, SetScanMacAddressTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::string mac = "abcdefghijklmn";
    WifiErrorNo err = mClient.SetScanMacAddress(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    err = mClient.SetScanMacAddress(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiIdlClientTest, DisconnectLastRoamingBssidTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::string mac = "abcdefghijklmn";
    WifiErrorNo err = mClient.DisconnectLastRoamingBssid(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    err = mClient.DisconnectLastRoamingBssid(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiIdlClientTest, ReqGetSupportFeatureTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    long feature = 0;
    WifiErrorNo err = mClient.ReqGetSupportFeature(feature);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiIdlClientTest, SetTxPowerTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    int power = 1;
    WifiErrorNo err = mClient.SetTxPower(power);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
}

static char **MockConVectorToCArrayString(const std::vector<std::string> &vec)
{
    int size = vec.size();
    if (size == 0) {
        return nullptr;
    }
    char **list = (char **)calloc(size, sizeof(char *));
    if (list == nullptr) {
        return nullptr;
    }
    int i = 0;
    for (; i < size; ++i) {
        int len = vec[i].length();
        list[i] = (char *)calloc(len + 1, sizeof(char));
        if (list[i] == nullptr) {
            break;
        }
        if (strncpy_s(list[i], len + 1, vec[i].c_str(), len) != EOK) {
            break;
        }
    }
    if (i < size) {
        for (int j = 0; j <= i; ++j) {
            free(list[j]);
        }
        free(list);
        return nullptr;
    } else {
        return list;
    }
}

HWTEST_F(WifiIdlClientTest, MockConVectorToCArrayStringTest, TestSize.Level1)
{
    std::vector<std::string> vec;
    char **list = MockConVectorToCArrayString(vec);
    ASSERT_TRUE(list == nullptr);
    vec.push_back("hello");
    vec.push_back("world");
    list = MockConVectorToCArrayString(vec);
    ASSERT_TRUE(list != nullptr);
    for (std::size_t i = 0; i < vec.size(); ++i) {
        ASSERT_TRUE(list[i] != nullptr);
        ASSERT_TRUE(strcmp(list[i], vec[i].c_str()) == 0);
    }
    for (std::size_t i = 0; i < vec.size(); ++i) {
        free(list[i]);
    }
    free(list);
}

static bool MockScanTest(const WifiScanParam &scanParam)
{
    ScanSettings settings;
    if (memset_s(&settings, sizeof(settings), 0, sizeof(settings)) != EOK) {
        return false;
    }
    bool bfail = false;
    do {
        if (scanParam.hiddenNetworkSsid.size() > 0) {
            settings.hiddenSsidSize = scanParam.hiddenNetworkSsid.size();
            settings.hiddenSsid = MockConVectorToCArrayString(scanParam.hiddenNetworkSsid);
            if (settings.hiddenSsid == nullptr) {
                bfail = true;
                break;
            }
        }
        if (scanParam.scanFreqs.size() > 0) {
            settings.freqSize = scanParam.scanFreqs.size();
            settings.freqs = (int *)calloc(settings.freqSize, sizeof(int));
            if (settings.freqs == nullptr) {
                bfail = true;
                break;
            }
            for (int i = 0; i < settings.freqSize; ++i) {
                settings.freqs[i] = scanParam.scanFreqs[i];
            }
        }
        if (scanParam.scanStyle > 0) {
            settings.scanStyle = scanParam.scanStyle;
        }
    } while (0);
    if (settings.freqs != nullptr) {
        free(settings.freqs);
    }
    if (settings.hiddenSsid != nullptr) {
        for (int i = 0; i < settings.hiddenSsidSize; ++i) {
            free(settings.hiddenSsid[i]);
        }
        free(settings.hiddenSsid);
    }
    return !bfail;
}

HWTEST_F(WifiIdlClientTest, ScanTest, TestSize.Level1)
{
    WifiScanParam param;
    param.hiddenNetworkSsid.push_back("abcd");
    param.hiddenNetworkSsid.push_back("efgh");
    ASSERT_TRUE(MockScanTest(param));
    param.scanFreqs.push_back(2412);
    param.scanFreqs.push_back(2417);
    param.scanStyle = 1;
    ASSERT_TRUE(MockScanTest(param));
}

HWTEST_F(WifiIdlClientTest, ReqGetNetworkListTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::vector<WifiWpaNetworkInfo> infos;
    WifiErrorNo err = mClient.ReqGetNetworkList(infos);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK);
}

static void FreePnoScanPstr(PnoScanSettings &settings)
{
    if (settings.freqs != nullptr) {
        free(settings.freqs);
    }
    if (settings.hiddenSsid != nullptr) {
        for (int i = 0; i < settings.hiddenSsidSize; ++i) {
            free(settings.hiddenSsid[i]);
        }
        free(settings.hiddenSsid);
    }
    if (settings.savedSsid != nullptr) {
        for (int i = 0; i < settings.savedSsidSize; ++i) {
            free(settings.savedSsid[i]);
        }
        free(settings.savedSsid);
    }
}

static bool MockPnoScanTest(const WifiPnoScanParam &scanParam)
{
    PnoScanSettings settings;
    if (memset_s(&settings, sizeof(settings), 0, sizeof(settings)) != EOK) {
        return false;
    }
    bool bfail = false;
    do {
        if (scanParam.scanInterval > 0) {
            settings.scanInterval = scanParam.scanInterval;
        }
        settings.minRssi2Dot4Ghz = scanParam.minRssi2Dot4Ghz;
        settings.minRssi5Ghz = scanParam.minRssi5Ghz;
        if (scanParam.hiddenSsid.size() > 0) {
            settings.hiddenSsidSize = scanParam.hiddenSsid.size();
            settings.hiddenSsid = MockConVectorToCArrayString(scanParam.hiddenSsid);
            if (settings.hiddenSsid == nullptr) {
                bfail = true;
                break;
            }
        }
        if (scanParam.savedSsid.size() > 0) {
            settings.savedSsidSize = scanParam.savedSsid.size();
            settings.savedSsid = MockConVectorToCArrayString(scanParam.savedSsid);
            if (settings.savedSsid == nullptr) {
                bfail = true;
                break;
            }
        }
        if (scanParam.scanFreqs.size() > 0) {
            settings.freqSize = scanParam.scanFreqs.size();
            settings.freqs = (int *)calloc(settings.freqSize, sizeof(int));
            if (settings.freqs == nullptr) {
                return WIFI_IDL_OPT_FAILED;
            }
            for (int i = 0; i < settings.freqSize; ++i) {
                settings.freqs[i] = scanParam.scanFreqs[i];
            }
        }
    } while (0);
    FreePnoScanPstr(settings);
    return !bfail;
}

HWTEST_F(WifiIdlClientTest, ReqStartPnoScanTest, TestSize.Level1)
{
    WifiPnoScanParam param;
    param.hiddenSsid.push_back("abcd");
    param.hiddenSsid.push_back("efgh");
    ASSERT_TRUE(MockPnoScanTest(param));
    param.scanFreqs.push_back(2412);
    param.scanFreqs.push_back(2417);
    ASSERT_TRUE(MockPnoScanTest(param));
    param.savedSsid.push_back("abcd");
    ASSERT_TRUE(MockPnoScanTest(param));
}

HWTEST_F(WifiIdlClientTest, StopWifiTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    EXPECT_TRUE(mClient.StopWifi() == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiIdlClientTest, AddBlockByMacTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::string mac = "abcdefghijklmn";
    WifiErrorNo err = mClient.AddBlockByMac(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    err = mClient.AddBlockByMac(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK || err == WIFI_IDL_OPT_HOSTAPD_NOT_INIT);
}

HWTEST_F(WifiIdlClientTest, DelBlockByMacTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::string mac = "abcdefghijklmn";
    WifiErrorNo err = mClient.DelBlockByMac(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    err = mClient.DelBlockByMac(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK || err == WIFI_IDL_OPT_HOSTAPD_NOT_INIT);
}

HWTEST_F(WifiIdlClientTest, RemoveStationTest, TestSize.Level1)
{
    ASSERT_TRUE(mClient.InitClient() == 0);
    std::string mac = "abcdefghijklmn";
    WifiErrorNo err = mClient.RemoveStation(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    err = mClient.RemoveStation(mac);
    EXPECT_TRUE(err == WIFI_IDL_OPT_OK || err == WIFI_IDL_OPT_HOSTAPD_NOT_INIT);
}
}  // namespace Wifi
}  // namespace OHOS