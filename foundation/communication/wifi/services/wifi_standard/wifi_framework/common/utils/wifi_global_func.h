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

#ifndef OHOS_WIFI_GLOBAL_FUNC_H
#define OHOS_WIFI_GLOBAL_FUNC_H

#include <vector>
#include <random>
#include <string>
#include "wifi_errcode.h"
#include "wifi_ap_msg.h"
#include "wifi_scan_msg.h"
#include "wifi_settings.h"

namespace OHOS {
namespace Wifi {
constexpr int MAC_STRING_SIZE = 17;
constexpr int MIN_SSID_LEN = 1;
constexpr int MAX_SSID_LEN = 32;
constexpr int MIN_PSK_LEN = 8;
constexpr int MAX_PSK_LEN = 63;
constexpr int HEX_TYPE_LEN = 3; /* 3 hex type: 0 a A */
constexpr int MAX_AP_CONN = 32;

/**
 * @Description Check valid ssid config
 *
 * @param cfg - HotspotConfig
 * @return ErrCode - WIFI_OPT_SUCCESS or others
 */
ErrCode CfgCheckSsid(const HotspotConfig &cfg);

/**
 * @Description Check valid psk config
 *
 * @param cfg - HotspotConfig
 * @return ErrCode - WIFI_OPT_SUCCESS or others
 */
ErrCode CfgCheckPsk(const HotspotConfig &cfg);

/**
 * @Description Check valid band config
 *
 * @param cfg - HotspotConfig
 * @param bandsFromCenter - vector of BandType
 * @return ErrCode - WIFI_OPT_SUCCESS or others
 */
ErrCode CfgCheckBand(const HotspotConfig &cfg, std::vector<BandType> &bandsFromCenter);

/**
 * @Description Check valid channel config
 *
 * @param cfg - HotspotConfig
 * @param channInfoFromCenter - ChannelsTable object
 * @return ErrCode - WIFI_OPT_SUCCESS or others
 */
ErrCode CfgCheckChannel(const HotspotConfig &cfg, ChannelsTable &channInfoFromCenter);

/**
 * @Description Check valid hotspot config
 *
 * @param cfg - HotspotConfig
 * @param cfgFromCenter - Get HotspotConfig from config center
 * @param bandsFromCenter - vector of BandType
 * @param channInfoFromCenter - ChannelsTable object
 * @return ErrCode - WIFI_OPT_SUCCESS or others
 */
ErrCode IsValidHotspotConfig(const HotspotConfig &cfg, const HotspotConfig &cfgFromCenter,
    std::vector<BandType> &bandsFromCenter, ChannelsTable &channInfoFromCenter);

/**
 * @Description Get a random string
 *
 * @param len - Random string length
 * @return std::string - Random String
 */
std::string GetRandomStr(int len);

/**
 * @Description If allowed scan always according the scan control policy
 *
 * @param info - ScanControlInfo object
 * @return true - allowed
 * @return false - not allowed
 */
bool IsAllowScanAnyTime(const ScanControlInfo &info);

/**
 * @Description Internal transition from OperateResState struct to ConnState
 *
 * @param resState - OperateResState state
 * @param isReport - true : need report; flase : not report
 * @return ConnState - convert output connection state
 */
ConnState ConvertConnStateInternal(OperateResState resState, bool &isReport);

/**
 * @Description Check whether the MAC address is valid
 *
 * @param macStr - input the mac address
 * @return int - 0 Valid; -1 Invalid
 */
int CheckMacIsValid(const std::string &macStr);

/**
 * @Description Split string to vector accord split
 *
 * @param str - input string
 * @param split - split string
 * @param vec - return string vector
 */
void SplitString(const std::string &str, const std::string &split, std::vector<std::string> &vec);

/**
 * @Description Converts a numeric vector to a character array.
 *
 * @param vec - Input numeric vector.[in]
 * @param pChar - Character array.[out]
 * @param len - Length of character array.[out]
 * @param memSize - Character array's memory size.[in]
 * @return int - 0 Valid; -1 Invalid
 */
template <typename T>
int Vec2Char(const std::vector<T> &vec, T *pChar, int& len, int memSize)
{
    if (pChar == nullptr) {
        len = 0;
        return -1;
    }

    const int vecSize = static_cast<int>(vec.size());
    if (vecSize > memSize) {
        pChar = nullptr;
        len = 0;
        return -1;
    }

    for (int i = 0; i < vecSize; i++) {
        pChar[i] = vec[i];
    }
    len = vecSize;
    return 0;
}

/**
 * @Description Converts a character array to a numeric vector.
 *
 * @param pChar - Character array.[in]
 * @param len - Length of character array.[in]
 * @param vec - Input numeric vector.[out]
 * @return int - 0 Valid; -1 Invalid
 */
template <typename T>
int Char2Vec(const T *pChar, int len, std::vector<T> &vec)
{
    vec.clear();
    if (pChar == nullptr || len < 0) {
        return -1;
    }

    for (int i = 0; i < len; i++) {
        vec.push_back(pChar[i]);
    }

    return 0;
}

/**
 * @Description Converts a char/unsigned char/byte/int8 vector to a hexadecimal character array. A numeric
 * value is converted to two characters. e.g. 0x3F -> '3' 'F'
 *
 * @param vec - Input numeric vector.
 * @param pHexChar - Character array.
 * @param memSize - Character array's memory size.
 * @return int - 0 Valid; -1 Invalid
 */
template<typename T>
int Val2HexChar(const std::vector<T> &vec, char *pHexChar, unsigned memSize)
{
    unsigned size = vec.size();
    unsigned doubleSize = (size << 1);
    if (doubleSize >= memSize) {
        return -1;
    }
    const std::string hexStr = "0123456789ABCDEF";
    const unsigned highBit = 4;
    int pos = 0;
    for (unsigned i = 0; i < size; ++i) {
        unsigned char tmp = vec[i];
        pHexChar[pos] = hexStr[(tmp >> highBit) & 0x0F];
        ++pos;
        pHexChar[pos] = hexStr[tmp & 0x0F];
        ++pos;
    }
    pHexChar[pos] = '\0';
    return 0;
}

/**
 * @Description  Output vecChar to stream.
 * @param prefix  - prefix string[in]
 * @param vecChar - vector char[in]
 * @param suffix  - suffix string[in]
 */
std::string Vec2Stream(const std::string &prefix, const std::vector<char> &vecChar, const std::string &sufffix = "");

/**
 * @Description Convert a hex type string to vector.
 *
 * @param str - input hex string, eg: 010203...
 * @param vec - output vector result, eg: [1,2,3,...]
 * @return int - convert result, 0 success, -1 failed
 */
int HexStringToVec(const std::string &str, std::vector<char> &vec);

/**
 * @Description Check is a valid 5G frequency.
 *
 * @param freq - Frequency input
 * @return true - valid
 * @return false - invalid
 */
bool IsValid5GHz(int freq);

/**
 * @Description Check is a valid 2.4G frequency.
 *
 * @param freq - Frequency input
 * @return true - valid
 * @return false - invalid
 */
bool IsValid24GHz(int freq);

/**
 * @Description Obtain and report available channel information.
 *
 * @param apConfig - configuration input
 * @param validChanTable - Valid channel tables.
 */
void CheckBandChannel(HotspotConfig &apConfig, const std::map<BandType, std::vector<int32_t>> &validChanTable);

/**
 * @Description Convert the frequency in the container into a channel.
 *
 * @param freqVector - frequency vector input
 * @param chanVector - Channel vector output
 */
void TransformFrequencyIntoChannel(const std::vector<int> &freqVector, std::vector<int> &chanVector);
}  // namespace Wifi
}  // namespace OHOS
#endif