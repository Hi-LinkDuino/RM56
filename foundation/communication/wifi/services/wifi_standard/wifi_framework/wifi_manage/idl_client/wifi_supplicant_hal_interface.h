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

#ifndef OHOS_WIFI_SUPPLICANT_HAL_INTERFACE_H
#define OHOS_WIFI_SUPPLICANT_HAL_INTERFACE_H

#include <string>
#include "supplicant_event_callback.h"
#include "wifi_base_hal_interface.h"
#include "wifi_error_no.h"
#include "i_wifi_struct.h"

namespace OHOS {
namespace Wifi {
class WifiSupplicantHalInterface : public WifiBaseHalInterface {
public:
    /**
     * @Description Get the Instance object.
     *
     * @return WifiSupplicantHalInterface&
     */
    static WifiSupplicantHalInterface &GetInstance(void);

    /**
     * @Description Open Supplicant.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StartSupplicant(void) const;

    /**
     * @Description Close Supplicant.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo StopSupplicant(void) const;

    /**
     * @Description Connect Supplicant.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo ConnectSupplicant(void) const;

    /**
     * @Description Disconnect Supplicant.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo DisconnectSupplicant(void) const;

    /**
     * @Description Request to Supplicant.
     *
     * @param request
     * @return WifiErrorNo
     */
    WifiErrorNo RequestToSupplicant(const std::string &request) const;

    /**
     * @Description Registers the supplementary event callback function.
     *
     * @param callback
     * @return WifiErrorNo
     */
    WifiErrorNo RegisterSupplicantEventCallback(SupplicantEventCallback &callback);

    /**
     * @Description Deregisters the supplementary event callback function.
     *
     * @return WifiErrorNo
     */
    WifiErrorNo UnRegisterSupplicantEventCallback(void);

    /**
     * @Description Turn on/off power save mode for the interface.
     *
     * @param enable
     * @return WifiErrorNo
     */
    WifiErrorNo SetPowerSave(bool enable) const;

    /**
     * @Description Setting the country code.
     *
     * @param countryCode
     * @return WifiErrorNo
     */
    WifiErrorNo WpaSetCountryCode(const std::string &countryCode) const;

    /**
     * @Description Obtains the country code.
     *
     * @param countryCode
     * @return WifiErrorNo
     */
    WifiErrorNo WpaGetCountryCode(std::string &countryCode) const;

    /**
     * @Description Get register callback objects
     *
     * @return const SupplicantEventCallback& - register callback objects
     */
    const SupplicantEventCallback &GetCallbackInst(void) const;

private:
    SupplicantEventCallback mCallback;
};
}  // namespace Wifi
}  // namespace OHOS

#endif