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

#include <map>
#include <string>
#include <thread>

#include "bluetooth_log.h"
#include "napi_bluetooth_ble.h"
#include "napi_bluetooth_gatt_client.h"
#include "napi_bluetooth_gatt_server.h"
#include "napi_bluetooth_host.h"
#include "napi_bluetooth_hfp_ag.h"
#include "napi_bluetooth_hfp_hf.h"
#include "napi_bluetooth_profile.h"
#include "napi_bluetooth_spp_server.h"
#include "napi_bluetooth_a2dp_snk.h"
#include "napi_bluetooth_a2dp_src.h"
#include "napi_bluetooth_pbap_pce.h"
#include "napi_bluetooth_pbap_pse.h"
#include "napi_bluetooth_avrcp_ct.h"
#include "napi_bluetooth_avrcp_tg.h"
#include "napi_bluetooth_hid_host.h"
#include "napi_bluetooth_pan.h"
#include "napi_bluetooth_opp.h"

namespace OHOS {
namespace Bluetooth {
EXTERN_C_START
/*
 * Module initialization function
 */
static napi_value Init(napi_env env, napi_value exports)
{
    HILOGI("-----Init start------");
    napi_property_descriptor desc[] = {};

    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    NapiGattServer::DefineGattServerJSClass(env);
    NapiGattClient::DefineGattClientJSClass(env);
    DefineBLEJSObject(env, exports);
    DefineSppFunctions(env, exports);
    NapiProfile::DefineProfileFunctions(env, exports);
    NapiHandsFreeAudioGateway::DefineHandsFreeAudioGatewayJSClass(env);
    NapiHandsFreeUnit::DefineHandsFreeUnitJSClass(env);
    BluetoothHostInit(env, exports);
    NapiA2dpSink::DefineA2dpSinkJSClass(env);
    NapiA2dpSource::DefineA2dpSourceJSClass(env);
    NapiPbapClient::DefinePbapClientJSClass(env);
    NapiPbapServer::DefinePbapServerJSClass(env);
    NapiAvrcpController::DefineAvrcpControllerJSClass(env);
    NapiAvrcpTarget::DefineAvrcpTargetJSClass(env);
    NapiBluetoothHidHost::DefineHidHostJSClass(env);
    NapiBluetoothPan::DefinePanJSClass(env);
    NapiBluetoothOpp::DefineOppJSClass(env);
    DefineSystemBLEInterface(env, exports);

    HILOGI("-----Init end------");
    return exports;
}
EXTERN_C_END
/*
 * Module define
 */
static napi_module bluetoothModule = {.nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = Init,
    .nm_modname = "bluetooth",
    .nm_priv = ((void *)0),
    .reserved = {0}};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&bluetoothModule);
}
}  // namespace Bluetooth
}  // namespace OHOS
