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

#include "wifi_napi_event.h"
#include <uv.h>
#include "wifi_napi_utils.h"
#include "wifi_device.h"
#include "wifi_scan.h"
#include "wifi_logger.h"

namespace OHOS {
namespace Wifi {
DEFINE_WIFILOG_LABEL("WifiNAPIEvent");

const std::string EVENT_STA_POWER_STATE_CHANGE = "wifiStateChange";
const std::string EVENT_STA_CONN_STATE_CHANGE = "wifiConnectionChange";
const std::string EVENT_STA_SCAN_STATE_CHANGE = "wifiScanStateChange";
const std::string EVENT_STA_RSSI_STATE_CHANGE = "wifiRssiChange";
const std::string EVENT_HOTSPOT_STATE_CHANGE = "hotspotStateChange";
const std::string EVENT_HOTSPOT_STA_JOIN = "hotspotStaJoin";
const std::string EVENT_HOTSPOT_STA_LEAVE = "hotspotStaLeave";
const std::string EVENT_P2P_STATE_CHANGE = "p2pStateChange";
const std::string EVENT_P2P_CONN_STATE_CHANGE = "p2pConnectionChange";
const std::string EVENT_P2P_DEVICE_STATE_CHANGE = "p2pDeviceChange";
const std::string EVENT_P2P_PERSISTENT_GROUP_CHANGE = "p2pPersistentGroupChange";
const std::string EVENT_P2P_PEER_DEVICE_CHANGE = "p2pPeerDeviceChange";
const std::string EVENT_P2P_DISCOVERY_CHANGE = "p2pDiscoveryChange";
const std::string EVENT_STREAM_CHANGE = "streamChange";

static std::set<std::string> g_supportEventList = {
    EVENT_STA_POWER_STATE_CHANGE,
    EVENT_STA_CONN_STATE_CHANGE,
    EVENT_STA_SCAN_STATE_CHANGE,
    EVENT_STA_RSSI_STATE_CHANGE,
    EVENT_HOTSPOT_STATE_CHANGE,
    EVENT_HOTSPOT_STA_JOIN,
    EVENT_HOTSPOT_STA_LEAVE,
    EVENT_P2P_STATE_CHANGE,
    EVENT_P2P_CONN_STATE_CHANGE,
    EVENT_P2P_DEVICE_STATE_CHANGE,
    EVENT_P2P_PERSISTENT_GROUP_CHANGE,
    EVENT_P2P_PEER_DEVICE_CHANGE,
    EVENT_P2P_DISCOVERY_CHANGE,
};

void NapiEvent::EventNotify(AsyncEventData *asyncEvent)
{
    WIFI_LOGI("Enter wifi event notify");
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(asyncEvent->env, &loop);

    uv_work_t* work = new uv_work_t;
    if (work == nullptr) {
        WIFI_LOGE("uv_work_t work is null.");
        delete asyncEvent;
        asyncEvent = nullptr;
        return;
    }

    WIFI_LOGI("Get the event loop, napi_env: %{private}p", asyncEvent->env);
    work->data = asyncEvent;
    uv_queue_work(
        loop,
        work,
        [](uv_work_t* work) {},
        [](uv_work_t* work, int status) {
            AsyncEventData *asyncData = static_cast<AsyncEventData*>(work->data);
            WIFI_LOGI("Napi event uv_queue_work, env: %{private}p, status: %{public}d", asyncData->env, status);
            napi_value handler = nullptr;
            napi_handle_scope scope = nullptr;
            napi_value jsEvent = nullptr;
            napi_open_handle_scope(asyncData->env, &scope);
            if (scope == nullptr) {
                WIFI_LOGE("scope is nullptr");
                napi_close_handle_scope(asyncData->env, scope);
                goto EXIT;
            }
            napi_value undefine;
            napi_get_undefined(asyncData->env, &undefine);
            napi_get_reference_value(asyncData->env, asyncData->callbackRef, &handler);
            jsEvent = asyncData->packResult();
            WIFI_LOGI("Push event to js, env: %{private}p, ref : %{private}p", asyncData->env, &asyncData->callbackRef);
            if (napi_call_function(asyncData->env, nullptr, handler, 1, &jsEvent, &undefine) != napi_ok) {
                WIFI_LOGE("Report event to Js failed");
            }
            napi_close_handle_scope(asyncData->env, scope);

        EXIT:
            delete asyncData;
            asyncData = nullptr;
            delete work;
            work = nullptr;
        }
    );
}

bool NapiEvent::CheckIsRegister(const std::string& type)
{
    return g_eventRegisterInfo.find(type) != g_eventRegisterInfo.end();
}

napi_value NapiEvent::CreateResult(const napi_env& env, int value) {
    napi_value result;
    napi_create_int32(env, value, &result);
    return result;
}

napi_value NapiEvent::CreateResult(const napi_env& env, const StationInfo& info)
{
    napi_value result;
    napi_create_object(env, &result);
    SetValueUtf8String(env, "name", info.deviceName.c_str(), result);
    SetValueUtf8String(env, "macAddress", info.bssid.c_str(), result);
    SetValueUtf8String(env, "ipAddress", info.ipAddr.c_str(), result);
    return result;
}

napi_value NapiEvent::CreateResult(const napi_env& env, const WifiP2pDevice& device)
{
    napi_value result;
    napi_create_object(env, &result);
    SetValueUtf8String(env, "deviceName", device.GetDeviceName().c_str(), result);
    SetValueUtf8String(env, "deviceAddress", device.GetDeviceAddress().c_str(), result);
    SetValueUtf8String(env, "primaryDeviceType", device.GetPrimaryDeviceType().c_str(), result);
    SetValueInt32(env, "devStatus", static_cast<int>(device.GetP2pDeviceStatus()), result);
    SetValueInt32(env, "groupCapability", device.GetGroupCapabilitys(), result);
    return result;
}

napi_value NapiEvent::CreateResult(const napi_env& env, const std::vector<WifiP2pDevice>& devices)
{
    uint32_t idx = 0;
    napi_value arrayResult;
    napi_create_array_with_length(env, devices.size(), &arrayResult);
    for (auto& each : devices) {
        if (napi_set_element(env, arrayResult, idx++, CreateResult(env, each)) != napi_ok) {
            WIFI_LOGE("Array result set element error, idx: %{public}u", idx - 1);
        }
    }
    return arrayResult;
}

napi_value NapiEvent::CreateResult(const napi_env& env, const WifiP2pLinkedInfo& info)
{
    napi_value result;
    napi_create_object(env, &result);
    SetValueInt32(env, "connectState", static_cast<int>(info.GetConnectState()), result);
    SetValueBool(env, "isGroupOwner", info.IsGroupOwner(), result);
    SetValueUtf8String(env, "groupOwnerAddr", info.GetGroupOwnerAddress().c_str(), result);
    return result;
}

napi_value NapiEvent::NapiEvent::CreateResult(const napi_env& env, napi_value placehoders)
{
    return placehoders == nullptr ? UndefinedNapiValue(env) : placehoders;
}

class WifiNapiDeviceEventCallback : public IWifiDeviceCallBack, public NapiEvent {
public:
    WifiNapiDeviceEventCallback() {
    }

    virtual ~WifiNapiDeviceEventCallback() {
    }

public:
    void OnWifiStateChanged(int state) override {
        WIFI_LOGI("sta received state changed event: %{public}d", state);
        if (m_wifiStateConvertMap.find(state) == m_wifiStateConvertMap.end()) {
            return;
        }
        CheckAndNotify(EVENT_STA_POWER_STATE_CHANGE, m_wifiStateConvertMap[state]);
    }

    void OnWifiConnectionChanged(int state, const WifiLinkedInfo &info) override {
        WIFI_LOGI("sta received connection changed event: %{public}d", state);
        if (m_connectStateConvertMap.find(state) == m_connectStateConvertMap.end()) {
            return;
        }
        CheckAndNotify(EVENT_STA_CONN_STATE_CHANGE, m_connectStateConvertMap[state]);
    }

    void OnWifiRssiChanged(int rssi) override {
        WIFI_LOGI("sta received rssi changed event: %{public}d", rssi);
        CheckAndNotify(EVENT_STA_RSSI_STATE_CHANGE, rssi);
    }

    void OnWifiWpsStateChanged(int state, const std::string &pinCode) override {
    }

    void OnStreamChanged(int direction) override {
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override {
        return nullptr;
    }

private:
    enum class JsLayerWifiState {
        DISABLED = 0,
        ENABLED = 1,
        ENABLING = 2,
        DISABLING = 3
    };

    enum class JsLayerConnectStatus {
        DISCONNECTED = 0,
        CONNECTED = 1,
    };

    std::map<int, int> m_wifiStateConvertMap = {
        { static_cast<int>(WifiState::DISABLING), static_cast<int>(JsLayerWifiState::DISABLING) },
        { static_cast<int>(WifiState::DISABLED), static_cast<int>(JsLayerWifiState::DISABLED) },
        { static_cast<int>(WifiState::ENABLING), static_cast<int>(JsLayerWifiState::ENABLING) },
        { static_cast<int>(WifiState::ENABLED), static_cast<int>(JsLayerWifiState::ENABLED) },
    };

    std::map<int, int> m_connectStateConvertMap = {
        { static_cast<int>(ConnState::CONNECTED), static_cast<int>(JsLayerConnectStatus::CONNECTED) },
        { static_cast<int>(ConnState::DISCONNECTED), static_cast<int>(JsLayerConnectStatus::DISCONNECTED) },
    };
};

class WifiNapiScanEventCallback : public IWifiScanCallback, public NapiEvent {
public:
    WifiNapiScanEventCallback() {
    }

    virtual ~WifiNapiScanEventCallback() {
    }

public:
    void OnWifiScanStateChanged(int state) override {
        WIFI_LOGI("scan received state changed event: %{public}d", state);
        CheckAndNotify(EVENT_STA_SCAN_STATE_CHANGE, state);
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override {
        return nullptr;
    }
};

class WifiNapiHotspotEventCallback : public IWifiHotspotCallback, public NapiEvent {
public:
    WifiNapiHotspotEventCallback() {
    }

    virtual ~WifiNapiHotspotEventCallback() {
    }

public:
    void OnHotspotStateChanged(int state) override {
        WIFI_LOGI("Hotspot received state changed event: %{public}d", state);
        if (m_apStateConvertMap.find(state) == m_apStateConvertMap.end()) {
            return;
        }

        CheckAndNotify(EVENT_HOTSPOT_STATE_CHANGE, m_apStateConvertMap[state]);
    }

    void OnHotspotStaJoin(const StationInfo &info) override {
        WIFI_LOGI("Hotspot received sta join event");
        CheckAndNotify(EVENT_HOTSPOT_STA_JOIN, info);
    }

    void OnHotspotStaLeave(const StationInfo &info) override {
        WIFI_LOGI("Hotspot received sta leave event");
        CheckAndNotify(EVENT_HOTSPOT_STA_LEAVE, info);
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override {
        return nullptr;
    }

private:
    enum class JsLayerApState {
        DISABLED = 0,
        ENABLED = 1,
        ENABLING = 2,
        DISABLING = 3
    };

    std::map<int, int> m_apStateConvertMap = {
        { static_cast<int>(ApState::AP_STATE_STARTING), static_cast<int>(JsLayerApState::ENABLING) },
        { static_cast<int>(ApState::AP_STATE_STARTED), static_cast<int>(JsLayerApState::ENABLED) },
        { static_cast<int>(ApState::AP_STATE_CLOSING), static_cast<int>(JsLayerApState::DISABLING) },
        { static_cast<int>(ApState::AP_STATE_CLOSED), static_cast<int>(JsLayerApState::DISABLED) },
    };
};

class WifiNapiP2pEventCallback : public IWifiP2pCallback, public NapiEvent {
public:
    WifiNapiP2pEventCallback() {
    }

    virtual ~WifiNapiP2pEventCallback() {
    }

public:
    void OnP2pStateChanged(int state) override {
        WIFI_LOGI("received p2p state changed event: %{public}d", state);
        CheckAndNotify(EVENT_P2P_STATE_CHANGE, state);
    }

    void OnP2pPersistentGroupsChanged(void) override {
        WIFI_LOGI("received persistent group changed event");
        CheckAndNotify(EVENT_P2P_PERSISTENT_GROUP_CHANGE, nullptr);
    }

    void OnP2pThisDeviceChanged(const WifiP2pDevice& device) override {
        WIFI_LOGI("received this device changed event");
        CheckAndNotify(EVENT_P2P_DEVICE_STATE_CHANGE, device);
    }

    void OnP2pPeersChanged(const std::vector<WifiP2pDevice>& devices) override {
        WIFI_LOGI("received peers changed event: %{public}d", (int)devices.size());
        CheckAndNotify(EVENT_P2P_PEER_DEVICE_CHANGE, devices);
    }

    void OnP2pServicesChanged(const std::vector<WifiP2pServiceInfo>& srvInfo) override {
    }

    void OnP2pConnectionChanged(const WifiP2pLinkedInfo& info) override {
        WIFI_LOGI("received connection changed event");
        CheckAndNotify(EVENT_P2P_CONN_STATE_CHANGE, info);
    }

    void OnP2pDiscoveryChanged(bool isChange) override {
        WIFI_LOGI("received discovery state changed event");
        CheckAndNotify(EVENT_P2P_DISCOVERY_CHANGE, (int)isChange);
    }

    void OnP2pActionResult(P2pActionCallback action, ErrCode code) override {
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override {
        return nullptr;
    }
};

napi_value On(napi_env env, napi_callback_info cbinfo) {
    TRACE_FUNC_CALL;
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    NAPI_ASSERT(env, argc >= requireArgc, "requires 2 parameter");

    napi_valuetype eventName = napi_undefined;
    napi_typeof(env, argv[0], &eventName);
    NAPI_ASSERT(env, eventName == napi_string, "type mismatch for parameter 1");

    napi_valuetype handler = napi_undefined;
    napi_typeof(env, argv[1], &handler);
    NAPI_ASSERT(env, handler == napi_function, "type mismatch for parameter 2");

    char type[64] = {0};
    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[0], type, sizeof(type), &typeLen);
    EventRegister::GetInstance().Register(env, type, argv[1]);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value Off(napi_env env, napi_callback_info cbinfo) {
    TRACE_FUNC_CALL;
    size_t requireArgc = 1;
    size_t requireArgcWithCb = 2;
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    NAPI_ASSERT(env, argc >= requireArgc, "requires at least 1 parameter");

    napi_valuetype eventName = napi_undefined;
    napi_typeof(env, argv[0], &eventName);
    NAPI_ASSERT(env, eventName == napi_string, "type mismatch for parameter 1");

    if (argc >= requireArgcWithCb) {
        napi_valuetype handler = napi_undefined;
        napi_typeof(env, argv[1], &handler);
        NAPI_ASSERT(env, handler == napi_function, "type mismatch for parameter 2");
    }

    char type[64] = {0};
    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[0], type, sizeof(type), &typeLen);
    EventRegister::GetInstance().Unregister(env, type, argc >= requireArgcWithCb ? argv[1] : nullptr);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

sptr<WifiNapiDeviceEventCallback> wifiDeviceCallback =
    sptr<WifiNapiDeviceEventCallback>(new (std::nothrow) WifiNapiDeviceEventCallback());

sptr<WifiNapiScanEventCallback> wifiScanCallback =
    sptr<WifiNapiScanEventCallback>(new (std::nothrow) WifiNapiScanEventCallback());

sptr<WifiNapiHotspotEventCallback> wifiHotspotCallback =
    sptr<WifiNapiHotspotEventCallback>(new (std::nothrow) WifiNapiHotspotEventCallback());

sptr<WifiNapiP2pEventCallback> wifiP2pCallback =
    sptr<WifiNapiP2pEventCallback>(new (std::nothrow) WifiNapiP2pEventCallback());

bool EventRegister::isEventRegistered = false;

ErrCode EventRegister::RegisterWifiEvents()
{
    std::unique_ptr<WifiDevice> wifiStaPtr = WifiDevice::GetInstance(WIFI_DEVICE_ABILITY_ID);
    if (wifiStaPtr == nullptr) {
        WIFI_LOGE("Register sta event get instance failed!");
        return WIFI_OPT_FAILED;
    }
    ErrCode ret = wifiStaPtr->RegisterCallBack(wifiDeviceCallback);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Register sta event failed!");
        return ret;
    }

    std::unique_ptr<WifiScan> wifiScanPtr = WifiScan::GetInstance(WIFI_SCAN_ABILITY_ID);
    if (wifiScanPtr == nullptr) {
        WIFI_LOGE("Register scan event get instance failed!");
        return WIFI_OPT_FAILED;
    }
    ret = wifiScanPtr->RegisterCallBack(wifiScanCallback);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Register scan event failed!");
        return ret;
    }

    std::unique_ptr<WifiHotspot> wifiHotspotPtr = WifiHotspot::GetInstance(WIFI_HOTSPOT_ABILITY_ID);
    if (wifiHotspotPtr == nullptr) {
        WIFI_LOGE("Register hotspot event get instance failed!");
        return WIFI_OPT_FAILED;
    }
    ret = wifiHotspotPtr->RegisterCallBack(wifiHotspotCallback);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Register hotspot event failed!");
        return ret;
    }

    std::unique_ptr<WifiP2p> wifiP2pPtr = WifiP2p::GetInstance(WIFI_P2P_ABILITY_ID);
    if (wifiP2pPtr == nullptr) {
        WIFI_LOGE("Register p2p event get instance failed!");
        return WIFI_OPT_FAILED;
    }
    ret = wifiP2pPtr->RegisterCallBack(wifiP2pCallback);
    if (ret != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Register p2p event failed!");
        return ret;
    }
    return ret;
}

EventRegister& EventRegister::GetInstance()
{
    static EventRegister inst;
    return inst;
}

bool EventRegister::IsEventSupport(const std::string& type)
{
    return g_supportEventList.find(type) != g_supportEventList.end();
}

void EventRegister::Register(const napi_env& env, const std::string& type, napi_value handler)
{
    WIFI_LOGI("Register event: %{public}s, env: %{private}p", type.c_str(), env);
    if (!IsEventSupport(type)) {
        WIFI_LOGE("Register type error or not support!");
        return;
    }
    std::unique_lock<std::shared_mutex> guard(g_regInfoMutex);
    if (!isEventRegistered) {
        if (RegisterWifiEvents() != WIFI_OPT_SUCCESS) {
            return;
        }
        isEventRegistered = true;
    }
    napi_ref handlerRef = nullptr;
    napi_create_reference(env, handler, 1, &handlerRef);
    RegObj regObj(env, handlerRef);
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        g_eventRegisterInfo[type] = std::vector<RegObj>{regObj};
    } else {
        iter->second.emplace_back(regObj);
    }
}

void EventRegister::DeleteRegisterObj(const napi_env& env, std::vector<RegObj>& vecRegObjs, napi_value& handler)
{
    auto iter = vecRegObjs.begin();
    for (; iter != vecRegObjs.end();) {
        if (env == iter->m_regEnv) {
            napi_value handlerTemp = nullptr;
            napi_get_reference_value(iter->m_regEnv, iter->m_regHanderRef, &handlerTemp);
            bool isEqual = false;
            napi_strict_equals(iter->m_regEnv, handlerTemp, handler, &isEqual);
            if (isEqual) {
                napi_delete_reference(iter->m_regEnv, iter->m_regHanderRef);
                WIFI_LOGI("Delete register object ref.");
                iter = vecRegObjs.erase(iter);
            } else {
                ++iter;
            }
        } else {
            WIFI_LOGI("Unregister event, env is not equal %{private}p, : %{private}p", env, iter->m_regEnv);
            ++iter;
        }
    }
}

void EventRegister::DeleteAllRegisterObj(const napi_env& env, std::vector<RegObj>& vecRegObjs)
{
    auto iter = vecRegObjs.begin();
    for (; iter != vecRegObjs.end();) {
        if (env == iter->m_regEnv) {
            napi_delete_reference(iter->m_regEnv, iter->m_regHanderRef);
            iter = vecRegObjs.erase(iter);
        } else {
            WIFI_LOGI("Unregister all event, env is not equal %{private}p, : %{private}p", env, iter->m_regEnv);
            ++iter;
        }
    }
}

void EventRegister::Unregister(const napi_env& env, const std::string& type, napi_value handler)
{
    WIFI_LOGI("Unregister event: %{public}s, env: %{private}p", type.c_str(), env);
    if (!IsEventSupport(type)) {
        WIFI_LOGE("Unregister type error or not support!");
        return;
    }
    std::unique_lock<std::shared_mutex> guard(g_regInfoMutex);
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        WIFI_LOGE("Unregister type not registered!");
        return;
    }
    if (handler != nullptr) {
        DeleteRegisterObj(env, iter->second, handler);
    } else {
        WIFI_LOGW("Unregister all relevant subscribe for: %{public}s", type.c_str());
        DeleteAllRegisterObj(env, iter->second);
    }
    if (iter->second.empty()) {
        g_eventRegisterInfo.erase(iter);
    }
}
}  // namespace Wifi
}  // namespace OHOS
