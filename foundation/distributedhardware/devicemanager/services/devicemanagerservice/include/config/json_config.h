/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_OHOS_DM_JSON_CONFIG_H
#define OHOS_OHOS_DM_JSON_CONFIG_H

#include <string>

namespace OHOS {
namespace DistributedHardware {
const std::string adapterJsonConfigString =
    R"({
    "devicemanager_adapter_components": [
        {
            "name": "crypto_adapter",
            "type": "CPYPTO",
            "version": "1.0",
            "funcName": "CreateCryptoAdapterObject",
            "soName": "libdevicemanager_crypto_adapter.z.so",
            "soPath": "/system/lib/"
        },
        {
            "name": "device_profile",
            "type": "PROFILE",
            "version": "1.0",
            "funcName": "CreateDeviceProfileObject",
            "soName": "libdevicemanagerext_profile.z.so",
            "soPath": "/system/lib/"
        },
        {
            "name": "device_decision",
            "type": "DECISION",
            "version": "1.0",
            "funcName": "CreateDeviceDecisionObject",
            "soName": "libdevicemanagerext_decision.z.so",
            "soPath": "/system/lib/"
        }
    ]
})";

const std::string authJsonConfigString =
    R"({
    "devicemanager_auth_components": [
        {
            "name": "pin_auth",
            "type": "AUTHENTICATE",
            "version": "1.0",
            "authType": 1,
            "funcName": "CreatePinAuthObject",
            "soName": "libdevicemanagerext_pin_auth.z.so",
            "soPath": "/system/lib/"
        },
        {
            "name": "QRcode_auth",
            "type": "AUTHENTICATE",
            "version": "1.0",
            "authType": 2,
            "funcName": "CreateQRcodeAuthObject",
            "soName": "libdevicemanager_qrcodeauth.z.so",
            "soPath": "/system/lib/"
        },
        {
            "name": "nfc_auth",
            "type": "AUTHENTICATE",
            "version": "1.0",
            "authType": 3,
            "funcName": "CreateNfcAuthObject",
            "soName": "libdevicemanager_nfcauth.z.so",
            "soPath": "/system/lib/"
        },
        {
            "name": "no_interaction_auth",
            "type": "AUTHENTICATE",
            "version": "1.0",
            "authType": 4,
            "funcName": "CreatePinAuthObject",
            "soName": "libdevicemanagerext_no_interaction_auth.z.so",
            "soPath": "/system/lib/"
        }
    ]
})";
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_OHOS_DM_JSON_CONFIG_H
