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

#include "wifi_p2p_dns_txt_record.h"
#include "wifi_logger.h"
#include <algorithm>

DEFINE_WIFILOG_P2P_LABEL("WifiP2pDnsTxtRecord");

namespace OHOS {
namespace Wifi {
WifiP2PDnsTxtRecord::WifiP2PDnsTxtRecord() : data()
{}

WifiP2PDnsTxtRecord::~WifiP2PDnsTxtRecord()
{}

void WifiP2PDnsTxtRecord::InsertData(
    const std::vector<unsigned char> &keyBytes, const std::vector<unsigned char> &valBytes, int index)
{
    std::vector<unsigned char> oldData = data;
    unsigned long insertIndex = 0;

    const int mapLengthSize = 1;
    const int equalSeparatorSize = 1;
    for (int i = 0; i < index && insertIndex < data.size(); ++i) {
        insertIndex += (data[insertIndex] + mapLengthSize) & 0xff;
    }

    int addLength = keyBytes.size() + valBytes.size() + ((valBytes.size() == 0) ? 0 : mapLengthSize);
    int newDataLength = addLength + mapLengthSize + oldData.size();

    data.clear();

    data.insert(data.begin(), oldData.begin(), oldData.begin() + insertIndex);
    int secondHalfLength = oldData.size() - insertIndex;
    data.insert(data.begin() + newDataLength - secondHalfLength,
        oldData.begin() + insertIndex,
        oldData.begin() + insertIndex + secondHalfLength);
    data.insert(data.begin() + insertIndex, static_cast<unsigned char>(addLength));
    data.insert(data.begin() + insertIndex + mapLengthSize, keyBytes.begin(), keyBytes.end());

    if (!valBytes.empty()) {
        data.insert(data.begin() + insertIndex + mapLengthSize + keyBytes.size(), 0x3d);
        data.insert(data.begin() + insertIndex + keyBytes.size() + mapLengthSize + equalSeparatorSize,
            valBytes.begin(),
            valBytes.end());
    }
}

int WifiP2PDnsTxtRecord::KeyNum() const
{
    int count = 0;
    for (unsigned long nextKey = 0; nextKey < data.size(); ++count) {
        nextKey += (data[nextKey] + 1) & 0xff;
    }
    return count;
}

int WifiP2PDnsTxtRecord::RemoveData(const std::string &key)
{
    unsigned long aDataStart = 0;
    std::string dataStr("");
    for (unsigned long i = 0; i < data.size(); ++i) {
        dataStr += data[i];
    }
    std::transform(dataStr.begin(), dataStr.end(), dataStr.begin(), ::tolower);

    for (int index = 0; aDataStart < data.size(); ++index) {
        unsigned long aDataLength = data[aDataStart];
        if (key.length() <= aDataLength &&
            (key.length() == aDataLength || data[aDataStart + key.length() + 1] == '=')) {
            std::string tmp(dataStr, aDataStart + 1, key.length());
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
            if (dataStr == tmp) {
                std::vector<unsigned char> oldData = data;
                data.clear();
                data.insert(data.begin(), oldData.begin(), oldData.begin() + aDataStart);
                data.insert(
                    data.begin() + aDataStart, oldData.begin() + aDataStart + aDataLength + 1, oldData.end());
                return index;
            }
        }
        aDataStart += (aDataLength + 1) & 0xff;
    }
    return -1;
}

bool WifiP2PDnsTxtRecord::SetRecord(const std::string &key, const std::string &val)
{
    std::vector<unsigned char> keyBytes;
    std::vector<unsigned char> valBytes;

    for (unsigned long index = 0; index < val.length(); ++index) {
        valBytes.push_back(static_cast<unsigned char>(*(val.c_str() + index)));
    }
    for (unsigned long index = 0; index < key.length(); ++index) {
        if (key[index] == '=') {
            WIFI_LOGE("= is invaild in keys!");
            return false;
        }
        keyBytes.push_back(static_cast<unsigned char>(*(key.c_str() + index)));
    }

    const int maxRecordSize = 255;
    if (keyBytes.size() + valBytes.size() >= maxRecordSize) {
        WIFI_LOGE("The key and value cannot be greater than or equal to 255.");
        return false;
    }

    int addKeyPos = RemoveData(key);
    if (addKeyPos == -1) {
        addKeyPos = KeyNum();
    }

    InsertData(keyBytes, valBytes, addKeyPos);
    return true;
}

const std::vector<unsigned char> &WifiP2PDnsTxtRecord::GetData() const
{
    return data;
}
}  // namespace Wifi
}  // namespace OHOS