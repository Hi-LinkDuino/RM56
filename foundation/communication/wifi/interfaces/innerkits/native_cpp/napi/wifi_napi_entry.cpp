/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "wifi_napi_device.h"
#include "wifi_napi_hotspot.h"
#include "wifi_napi_p2p.h"
#include "wifi_napi_event.h"
#include "wifi_logger.h"

namespace OHOS {
namespace Wifi {
#ifndef ENABLE_NAPI_COMPATIBLE
/*
 * Module initialization function
 */
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("enableWifi", EnableWifi),
        DECLARE_NAPI_FUNCTION("disableWifi", DisableWifi),
        DECLARE_NAPI_FUNCTION("isWifiActive", IsWifiActive),
        DECLARE_NAPI_FUNCTION("scan", Scan),
        DECLARE_NAPI_FUNCTION("getScanInfos", GetScanInfos),
        DECLARE_NAPI_FUNCTION("addDeviceConfig", AddDeviceConfig),
        DECLARE_NAPI_FUNCTION("addUntrustedConfig", AddUntrustedConfig),
        DECLARE_NAPI_FUNCTION("removeUntrustedConfig", RemoveUntrustedConfig),
        DECLARE_NAPI_FUNCTION("connectToNetwork", ConnectToNetwork),
        DECLARE_NAPI_FUNCTION("connectToDevice", ConnectToDevice),
        DECLARE_NAPI_FUNCTION("isConnected", IsConnected),
        DECLARE_NAPI_FUNCTION("disconnect", Disconnect),
        DECLARE_NAPI_FUNCTION("getSignalLevel", GetSignalLevel),
        DECLARE_NAPI_FUNCTION("reconnect", ReConnect),
        DECLARE_NAPI_FUNCTION("reassociate", ReAssociate),
        DECLARE_NAPI_FUNCTION("getIpInfo", GetIpInfo),
        DECLARE_NAPI_FUNCTION("getLinkedInfo", GetLinkedInfo),
        DECLARE_NAPI_FUNCTION("removeDevice", RemoveDevice),
        DECLARE_NAPI_FUNCTION("removeAllNetwork", RemoveAllNetwork),
        DECLARE_NAPI_FUNCTION("disableNetwork", DisableNetwork),
        DECLARE_NAPI_FUNCTION("getCountryCode", GetCountryCode),
        DECLARE_NAPI_FUNCTION("getDeviceConfigs", GetDeviceConfigs),
        DECLARE_NAPI_FUNCTION("updateNetwork", UpdateNetwork),
        DECLARE_NAPI_FUNCTION("getSupportedFeatures", GetSupportedFeatures),
        DECLARE_NAPI_FUNCTION("isFeatureSupported", IsFeatureSupported),
        DECLARE_NAPI_FUNCTION("getDeviceMacAddress", GetDeviceMacAddress),
        DECLARE_NAPI_FUNCTION("isHotspotActive", IsHotspotActive),
        DECLARE_NAPI_FUNCTION("isHotspotDualBandSupported", IsHotspotDualBandSupported),
        DECLARE_NAPI_FUNCTION("enableHotspot", EnableHotspot),
        DECLARE_NAPI_FUNCTION("disableHotspot", DisableHotspot),
        DECLARE_NAPI_FUNCTION("setHotspotConfig", SetHotspotConfig),
        DECLARE_NAPI_FUNCTION("getHotspotConfig", GetHotspotConfig),
        DECLARE_NAPI_FUNCTION("getStations", GetStations),
        DECLARE_NAPI_FUNCTION("getP2pLinkedInfo", GetP2pLinkedInfo),
        DECLARE_NAPI_FUNCTION("getCurrentGroup", GetCurrentGroup),
        DECLARE_NAPI_FUNCTION("getP2pPeerDevices", GetP2pDevices),
        DECLARE_NAPI_FUNCTION("createGroup", CreateGroup),
        DECLARE_NAPI_FUNCTION("removeGroup", RemoveGroup),
        DECLARE_NAPI_FUNCTION("p2pConnect", P2pConnect),
        DECLARE_NAPI_FUNCTION("p2pCancelConnect", P2pCancelConnect),
        DECLARE_NAPI_FUNCTION("startDiscoverDevices", StartDiscoverDevices),
        DECLARE_NAPI_FUNCTION("stopDiscoverDevices", StopDiscoverDevices),
        DECLARE_NAPI_FUNCTION("deletePersistentGroup", DeletePersistentGroup),
        DECLARE_NAPI_FUNCTION("setDeviceName", SetDeviceName),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    return exports;
}

static napi_module wifiJsModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = Init,
    .nm_modname = "wifi",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

#else

/*
 * Module initialization function
 */
static napi_value InitForCompatible(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("enableWifi", EnableWifi),
        DECLARE_NAPI_FUNCTION("disableWifi", DisableWifi),
        DECLARE_NAPI_FUNCTION("isWifiActive", IsWifiActive),
        DECLARE_NAPI_FUNCTION("scan", Scan),
        DECLARE_NAPI_FUNCTION("getScanInfos", GetScanInfos),
        DECLARE_NAPI_FUNCTION("addDeviceConfig", AddDeviceConfig),
        DECLARE_NAPI_FUNCTION("connectToNetwork", ConnectToNetwork),
        DECLARE_NAPI_FUNCTION("connectToDevice", ConnectToDevice),
        DECLARE_NAPI_FUNCTION("disconnect", Disconnect),
        DECLARE_NAPI_FUNCTION("getSignalLevel", GetSignalLevel),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    return exports;
}

/* @Deprecated - Changeme module name from "wifi_native_js" to "wifi",
 * "wifi_native_js" will be discarded. Modify @11/2021
 */
static napi_module wifiJsModuleForCompatible = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = InitForCompatible,
    .nm_modname = "wifi_native_js",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};
#endif

extern "C" __attribute__((constructor)) void RegisterModule(void) {
#ifndef ENABLE_NAPI_COMPATIBLE
    napi_module_register(&wifiJsModule);
#else
    napi_module_register(&wifiJsModuleForCompatible);
#endif
}
}  // namespace Wifi
}  // namespace OHOS
