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

#ifndef USB_INFO_H
#define USB_INFO_H

#include <string>
#include <vector>

namespace OHOS {
namespace USB {
struct USBDeviceInfo {
    int32_t status;
    int32_t busNum;
    int32_t devNum;
};

class UsbInfo {
public:
    void setDevInfoStatus(int32_t status)
    {
        devInfo.status = status;
    }
    void setDevInfoBusNum(int32_t busNum)
    {
        devInfo.busNum = busNum;
    }
    void setDevInfoDevNum(int32_t devNum)
    {
        devInfo.devNum = devNum;
    }

    int32_t getDevInfoStatus() const
    {
        return devInfo.status;
    }
    int32_t getDevInfoBusNum() const
    {
        return devInfo.busNum;
    }
    int32_t getDevInfoDevNum() const
    {
        return devInfo.devNum;
    }

private:
    USBDeviceInfo devInfo;
};
} // namespace USB
} // namespace OHOS

#endif // USBMGR_USB_INFO_H
