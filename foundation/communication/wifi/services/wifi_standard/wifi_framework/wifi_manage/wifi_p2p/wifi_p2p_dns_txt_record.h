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

#ifndef OHOS_DNS_TXT_RECORD_H
#define OHOS_DNS_TXT_RECORD_H

#include <string>
#include <vector>
#include "wifi_log.h"

namespace OHOS {
namespace Wifi {
class WifiP2PDnsTxtRecord {
public:
    /**
     * @Description Construct a new P2pMonitor object.
     */
    WifiP2PDnsTxtRecord();
    /**
     * @Description Destroy the P2pMonitor object.
     */
    virtual ~WifiP2PDnsTxtRecord();
    /**
     * @Description Parses key-value pair and sets it to data.
     *
     * @param key - the key of key-value pair
     * @param val - the value of key-value pair
     * @return true - success
     * @return false - failed
     */
    bool SetRecord(const std::string &key, const std::string &val);
    /**
     * @Description Remove the corresponding data based on the key.
     *
     * @param key - the key of key-value pair
     * @return int - If the operation is successful, the index is returned. If the operation fails, -1 is returned.
     */
    int RemoveData(const std::string &key);
    /**
     * @Description Count the number of key-value pairs.
     *
     * @return int - the number of key-value pairs.
     */
    int KeyNum() const;
    /**
     * @Description Get the data object
     *
     * @return const std::vector<unsigned char>&
     */
    const std::vector<unsigned char> &GetData() const;

private:
    /**
     * @Description Add key-value pair data at the index.
     *
     * @param keyBytes - the key of key-value pair
     * @param valBytes - the value of key-value pair
     * @param index - inserted index position
     */
    void InsertData(const std::vector<unsigned char> &keyBytes, const std::vector<unsigned char> &valBytes, int index);

private:
    std::vector<unsigned char> data;
};
}  // namespace Wifi
}  // namespace OHOS
#endif