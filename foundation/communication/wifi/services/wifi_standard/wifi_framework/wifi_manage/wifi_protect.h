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

#ifndef OHOS_WIFIPROTECT_H
#define OHOS_WIFIPROTECT_H

#include <string>
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
class WifiProtect {
public:
    /**
     * @Description Construct a new Wifi Protect object
     *
     * @param protectType - protect type
     * @param protectMode - protect mode
     * @param name - protect name, which is a unique identifier
     */
    WifiProtect(const WifiProtectType &protectType, const WifiProtectMode &protectMode, const std::string &name);

    /**
     * @Description Construct a new Wifi Full Protect object
     *
     * @param tag - protect name, which is a unique identifier
     */
    explicit WifiProtect(const std::string &name);

    /**
     * @Description Construct a new Default Wifi Protect object
     *
     */
    WifiProtect();

    /**
     * @Description Destroy the Wifi Protect object
     *
     */
    ~WifiProtect();

    /**
     * @Description Set the Tag object
     *
     * @param tag - protect name
     */
    void SetName(const std::string &name);

    /**
     * @Description Get the Tag object
     *
     * @return std::string - Wifi protect Tag
     */
    std::string GetName() const;

    /**
     * @Description Set the Protect Type object
     *
     * @param protectType - protect type
     */
    void SetProtectType(const WifiProtectType &protectType);

    /**
     * @Description Get the Protect Type object
     *
     * @return WifiProtectType - protect type
     */
    WifiProtectType GetProtectType() const;

    /**
     * @Description Set the Protect Mode object
     *
     * @param protectMode - protect mode
     */
    void SetProtectMode(const WifiProtectMode &protectMode);

    /**
     * @Description Get the Protect Mode object
     *
     * @return WifiProtectMode - protect mode
     */
    WifiProtectMode GetProtectMode() const;

    /**
     * @Description Get the Acq Timestamp
     *
     * @return long - timestamp
     */
    long GetAcqTimestamp() const;

private:
    std::string mName;
    /* not used: int mUid; */
    WifiProtectType mType;
    WifiProtectMode mMode;
    long mAcqTimestamp;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
