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
#include "wifi_supplicant_hal_interface.h"
#include <mutex>
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "WifiSupplicantHalInterface"

namespace OHOS {
namespace Wifi {
WifiSupplicantHalInterface &WifiSupplicantHalInterface::GetInstance(void)
{
    static WifiSupplicantHalInterface inst;
    static int initFlag = 0;
    static std::mutex initMutex;
    if (initFlag == 0) {
        std::unique_lock<std::mutex> lock(initMutex);
        if (initFlag == 0) {
            if (inst.InitIdlClient()) {
                initFlag = 1;
            }
        }
    }
    return inst;
}

WifiErrorNo WifiSupplicantHalInterface::StartSupplicant(void) const
{
    return mIdlClient->ReqStartSupplicant();
}

WifiErrorNo WifiSupplicantHalInterface::StopSupplicant(void) const
{
    return mIdlClient->ReqStopSupplicant();
}

WifiErrorNo WifiSupplicantHalInterface::ConnectSupplicant(void) const
{
    return mIdlClient->ReqConnectSupplicant();
}

WifiErrorNo WifiSupplicantHalInterface::DisconnectSupplicant(void) const
{
    return mIdlClient->ReqDisconnectSupplicant();
}

WifiErrorNo WifiSupplicantHalInterface::RequestToSupplicant(const std::string &request) const
{
    return mIdlClient->ReqRequestToSupplicant(request);
}

WifiErrorNo WifiSupplicantHalInterface::RegisterSupplicantEventCallback(SupplicantEventCallback &callback)
{
    WifiErrorNo err = mIdlClient->ReqRegisterSupplicantEventCallback(callback);
    if (err == WIFI_IDL_OPT_OK) {
        mCallback = callback;
    }
    return err;
}

WifiErrorNo WifiSupplicantHalInterface::UnRegisterSupplicantEventCallback(void)
{
    WifiErrorNo err = mIdlClient->ReqUnRegisterSupplicantEventCallback();
    mCallback.onScanNotify = nullptr;
    return err;
}

WifiErrorNo WifiSupplicantHalInterface::SetPowerSave(bool enable) const
{
    return mIdlClient->ReqSetPowerSave(enable);
}

WifiErrorNo WifiSupplicantHalInterface::WpaSetCountryCode(const std::string &countryCode) const
{
    return mIdlClient->ReqWpaSetCountryCode(countryCode);
}

WifiErrorNo WifiSupplicantHalInterface::WpaGetCountryCode(std::string &countryCode) const
{
    return mIdlClient->ReqWpaGetCountryCode(countryCode);
}

const SupplicantEventCallback &WifiSupplicantHalInterface::GetCallbackInst(void) const
{
    return mCallback;
}
}  // namespace Wifi
}  // namespace OHOS