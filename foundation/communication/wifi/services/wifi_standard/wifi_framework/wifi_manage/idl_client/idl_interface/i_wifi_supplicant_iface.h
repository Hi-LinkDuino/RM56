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

#ifndef OHOS_IDL_IWIFI_SUPPLICANT_IFACE_H
#define OHOS_IDL_IWIFI_SUPPLICANT_IFACE_H

#include <stdint.h>
#include "wifi_error_no.h"
#include "i_wifi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Set the supplementary event callback function.
 *
 * @param callback
 */
void SetSupplicantEventCallback(ISupplicantEventCallback callback);

/**
 * @Description Obtains the supplementary event callback structure.
 *
 * @return ISupplicantEventCallback*
 */
ISupplicantEventCallback *GetSupplicantEventCallback(void);

/**
 * @Description Start Supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StartSupplicant(void);

/**
 * @Description Close Supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo StopSupplicant(void);

/**
 * @Description Connect supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo ConnectSupplicant(void);

/**
 * @Description DisConnect Supplicant.
 *
 * @return WifiErrorNo
 */
WifiErrorNo DisconnectSupplicant(void);

/**
 * @Description Request to Supplicant.
 *
 * @param buf
 * @param bufSize
 * @return WifiErrorNo
 */
WifiErrorNo RequestToSupplicant(unsigned char *buf, int32_t bufSize);

/**
 * @Description Registers the supplementary event callback function.
 *
 * @param callback
 * @return WifiErrorNo
 */
WifiErrorNo RegisterSupplicantEventCallback(ISupplicantEventCallback callback);

/**
 * @Description Connect.
 *
 * @param networkId
 * @return WifiErrorNo
 */
WifiErrorNo Connect(int networkId);

/**
 * @Description Reconnect.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Reconnect(void);

/**
 * @Description Reassociate.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Reassociate(void);

/**
 * @Description Disconnect.
 *
 * @return WifiErrorNo
 */
WifiErrorNo Disconnect(void);

/**
 * @Description Turn on/off power save mode for the interface.
 *
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo SetPowerSave(int enable);

/**
 * @Description Setting the country code.
 *
 * @param countryCode
 * @return WifiErrorNo
 */
WifiErrorNo WpaSetCountryCode(const char *countryCode);

/**
 * @Description Obtains the country code.
 *
 * @param countryCode
 * @param codeSize
 * @return WifiErrorNo
 */
WifiErrorNo WpaGetCountryCode(char *countryCode, int codeSize);
#ifdef __cplusplus
}
#endif
#endif