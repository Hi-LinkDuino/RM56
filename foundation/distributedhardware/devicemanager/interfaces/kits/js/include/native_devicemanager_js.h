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

#ifndef OHOS_DM_NATIVE_DEVICEMANAGER_JS_H
#define OHOS_DM_NATIVE_DEVICEMANAGER_JS_H

#include <memory>
#include <string>

#include "device_manager_callback.h"
#include "dm_app_image_info.h"
#include "dm_device_info.h"
#include "dm_native_event.h"
#include "dm_subscribe_info.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nlohmann/json.hpp"
#define DM_NAPI_BUF_LENGTH (256)

struct AsyncCallbackInfo {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;

    char bundleName[DM_NAPI_BUF_LENGTH] = {0};
    size_t bundleNameLen = 0;

    napi_ref callback = nullptr;
    int32_t status = -1;
};

struct DeviceInfoAsyncCallbackInfo {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;

    std::string bundleName;
    size_t bundleNameLen = 0;
    OHOS::DistributedHardware::DmDeviceInfo deviceInfo;
    std::string extra;
    // OHOS::DistributedHardware::DmFilterOptions filter;
    napi_ref callback = nullptr;
    napi_value thisVar = nullptr;
    napi_deferred deferred = nullptr;
    int32_t status = -1;
};

struct DeviceInfoListAsyncCallbackInfo {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;

    std::string bundleName;
    size_t bundleNameLen = 0;
    std::vector<OHOS::DistributedHardware::DmDeviceInfo> devList;
    std::string extra;
    // OHOS::DistributedHardware::DmFilterOptions filter;
    napi_ref callback = nullptr;
    napi_value thisVar = nullptr;
    napi_deferred deferred = nullptr;
    int32_t status = -1;
};

struct AuthAsyncCallbackInfo {
    napi_env env = nullptr;

    char bundleName[DM_NAPI_BUF_LENGTH] = {0};
    size_t bundleNameLen = 0;

    napi_ref callback = nullptr;
    int32_t authType = -1;
};

struct DmNapiStateJsCallback {
    std::string bundleName_;
    uint16_t subscribeId_;
    int32_t reason_;
    OHOS::DistributedHardware::DmDeviceInfo deviceInfo_;

    DmNapiStateJsCallback(std::string bundleName, uint16_t subscribeId, int32_t reason,
        OHOS::DistributedHardware::DmDeviceInfo deviceInfo)
        : bundleName_(bundleName), subscribeId_(subscribeId), reason_(reason), deviceInfo_(deviceInfo) {}
};

struct DmNapiAuthJsCallback {
    std::string bundleName_;
    std::string deviceId_;
    std::string token_;
    int32_t status_;
    int32_t reason_;

    DmNapiAuthJsCallback(std::string bundleName, std::string deviceId, std::string token, int32_t status,
        int32_t reason)
        : bundleName_(bundleName), deviceId_(deviceId), token_(token), status_(status), reason_(reason) {}
};

struct DmNapiVerifyJsCallback {
    std::string bundleName_;
    std::string deviceId_;
    int32_t resultCode_;
    int32_t flag_;

    DmNapiVerifyJsCallback(std::string bundleName, std::string deviceId, int32_t resultCode, int32_t flag)
        : bundleName_(bundleName), deviceId_(deviceId), resultCode_(resultCode), flag_(flag) {}
};

enum DmNapiDevStateChangeAction { ONLINE = 0, READY = 1, OFFLINE = 2, CHANGE = 3 };

class DmNapiInitCallback : public OHOS::DistributedHardware::DmInitCallback {
public:
    explicit DmNapiInitCallback(napi_env env, std::string &bundleName) : env_(env), bundleName_(bundleName)
    {
    }
    virtual ~DmNapiInitCallback()
    {
    }
    void OnRemoteDied() override;

private:
    napi_env env_;
    std::string bundleName_;
    std::unique_ptr<DmNapiStateJsCallback> jsCallback_;
};

class DmNapiDeviceStateCallback : public OHOS::DistributedHardware::DeviceStateCallback {
public:
    explicit DmNapiDeviceStateCallback(napi_env env, std::string &bundleName) : env_(env), bundleName_(bundleName)
    {
    }
    virtual ~DmNapiDeviceStateCallback() {};
    void OnDeviceOnline(const OHOS::DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceReady(const OHOS::DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceOffline(const OHOS::DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDeviceChanged(const OHOS::DistributedHardware::DmDeviceInfo &deviceInfo) override;

private:
    napi_env env_;
    std::string bundleName_;
    std::unique_ptr<DmNapiStateJsCallback> jsCallback_;
};

class DmNapiDiscoveryCallback : public OHOS::DistributedHardware::DiscoveryCallback {
public:
    explicit DmNapiDiscoveryCallback(napi_env env, std::string &bundleName)
        : env_(env), refCount_(0), bundleName_(bundleName)
    {
    }
    virtual ~DmNapiDiscoveryCallback() {};
    void OnDeviceFound(uint16_t subscribeId, const OHOS::DistributedHardware::DmDeviceInfo &deviceInfo) override;
    void OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason) override;
    void OnDiscoverySuccess(uint16_t subscribeId) override;
    void IncreaseRefCount();
    void DecreaseRefCount();
    int32_t GetRefCount();

private:
    napi_env env_;
    std::atomic<int32_t> refCount_;
    std::string bundleName_;
    std::unique_ptr<DmNapiStateJsCallback> jsCallback_;
};

class DmNapiDeviceManagerFaCallback : public OHOS::DistributedHardware::DeviceManagerFaCallback {
public:
    explicit DmNapiDeviceManagerFaCallback(napi_env env, std::string &bundleName) : env_(env), bundleName_(bundleName)
    {
    }
    virtual ~DmNapiDeviceManagerFaCallback() {};
    void OnCall(const std::string &paramJson) override;

private:
    napi_env env_;
    std::string bundleName_;
    std::unique_ptr<DmNapiAuthJsCallback> jsCallback_;
};

class DmNapiAuthenticateCallback : public OHOS::DistributedHardware::AuthenticateCallback {
public:
    explicit DmNapiAuthenticateCallback(napi_env env, std::string &bundleName) : env_(env), bundleName_(bundleName)
    {
    }
    virtual ~DmNapiAuthenticateCallback() {};
    void OnAuthResult(const std::string &deviceId, const std::string &token, int32_t status, int32_t reason) override;

private:
    napi_env env_;
    std::string bundleName_;
    std::unique_ptr<DmNapiAuthJsCallback> jsCallback_;
};

class DmNapiVerifyAuthCallback : public OHOS::DistributedHardware::VerifyAuthCallback {
public:
    explicit DmNapiVerifyAuthCallback(napi_env env, std::string &bundleName) : env_(env), bundleName_(bundleName)
    {
    }
    virtual ~DmNapiVerifyAuthCallback() {};
    void OnVerifyAuthResult(const std::string &deviceId, int32_t resultCode, int32_t flag) override;

private:
    napi_env env_;
    std::string bundleName_;
    std::unique_ptr<DmNapiVerifyJsCallback> jsCallback_;
};

class DeviceManagerNapi : public DmNativeEvent {
public:
    explicit DeviceManagerNapi(napi_env env, napi_value thisVar);
    virtual ~DeviceManagerNapi();
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static napi_value EnumTypeConstructor(napi_env env, napi_callback_info info);
    static napi_value InitDeviceTypeEnum(napi_env env, napi_value exports);
    static napi_value InitDeviceStateChangeActionEnum(napi_env env, napi_value exports);
    static napi_value InitDiscoverModeEnum(napi_env env, napi_value exports);
    static napi_value InitExchangeMediumEnum(napi_env env, napi_value exports);
    static napi_value InitExchangeFreqEnum(napi_env env, napi_value exports);
    static napi_value InitSubscribeCapEnum(napi_env env, napi_value exports);
    static napi_value CreateDeviceManager(napi_env env, napi_callback_info info);
    static napi_value ReleaseDeviceManager(napi_env env, napi_callback_info info);
    static napi_value SetUserOperationSync(napi_env env, napi_callback_info info);
    static napi_value GetTrustedDeviceListSync(napi_env env, napi_callback_info info);
    static napi_value GetTrustedDeviceList(napi_env env, napi_callback_info info);
    static napi_value GetLocalDeviceInfoSync(napi_env env, napi_callback_info info);
    static napi_value GetLocalDeviceInfo(napi_env env, napi_callback_info info);
    static napi_value UnAuthenticateDevice(napi_env env, napi_callback_info info);
    static napi_value StartDeviceDiscoverSync(napi_env env, napi_callback_info info);
    static napi_value StopDeviceDiscoverSync(napi_env env, napi_callback_info info);
    static napi_value AuthenticateDevice(napi_env env, napi_callback_info info);
    static napi_value VerifyAuthInfo(napi_env env, napi_callback_info info);
    static napi_value JsOn(napi_env env, napi_callback_info info);
    static napi_value JsOff(napi_env env, napi_callback_info info);
    static napi_value GetAuthenticationParamSync(napi_env env, napi_callback_info info);
    static void HandleCreateDmCallBack(const napi_env &env, AsyncCallbackInfo *asCallbackInfo);
    static DeviceManagerNapi *GetDeviceManagerNapi(std::string &buldleName);
    static void CreateDmCallback(napi_env env, std::string &bundleName, std::string &eventType);
    static void CreateDmCallback(napi_env env, std::string &bundleName, std::string &eventType, std::string &extra);
    static void ReleaseDmCallback(std::string &bundleName, std::string &eventType);
    static void DeviceInfoToJsArray(const napi_env &env,
                                    const std::vector<OHOS::DistributedHardware::DmDeviceInfo> &vecDevInfo,
                                    const int32_t idx, napi_value &arrayResult);
    static void DmAuthParamToJsAuthParam(const napi_env &env, const OHOS::DistributedHardware::DmAuthParam &authParam,
                                         napi_value &paramResult);
    static void SetValueInt32(const napi_env &env, const std::string &fieldStr, const int32_t intValue,
                              napi_value &result);
    static void SetValueUtf8String(const napi_env &env, const std::string &fieldStr, const std::string &str,
                                   napi_value &result);
    static void JsObjectToString(const napi_env &env, const napi_value &object, const std::string &fieldStr, char *dest,
                                 const int32_t destLen);
    static void JsObjectToBool(const napi_env &env, const napi_value &object, const std::string &fieldStr,
                               bool &fieldRef);
    static void JsObjectToInt(const napi_env &env, const napi_value &object, const std::string &fieldStr,
                              int &fieldRef);
    static std::string JsObjectToString(const napi_env &env, const napi_value &object);
    static int32_t JsToDmSubscribeInfo(const napi_env &env, const napi_value &object,
                                       OHOS::DistributedHardware::DmSubscribeInfo &info);
    static void JsToDmDeviceInfo(const napi_env &env, const napi_value &object,
                                 OHOS::DistributedHardware::DmDeviceInfo &info);
    static void JsToDmExtra(const napi_env &env, const napi_value &object, std::string &extra, int32_t &authType);
    static void JsToDmAuthInfo(const napi_env &env, const napi_value &object, std::string &extra);
    static void JsToDmBuffer(const napi_env &env, const napi_value &object, const std::string &fieldStr,
                             uint8_t **bufferPtr, int32_t &bufferLen);
    static void JsToJsonObject(const napi_env &env, const napi_value &object, const std::string &fieldStr,
                               nlohmann::json &jsonObj);
    static void JsToDmTokenInfo(const napi_env &env, const napi_value &object, const std::string &fieldStr,
                                nlohmann::json &jsonObj);
    static void JsToDmAuthExtra(const napi_env &env, const napi_value &param, nlohmann::json &jsonObj);
    static void DmDeviceInfotoJsDeviceInfo(const napi_env &env,
                                           const OHOS::DistributedHardware::DmDeviceInfo &vecDevInfo,
                                           napi_value &result);
    void OnDeviceStateChange(DmNapiDevStateChangeAction action,
                             const OHOS::DistributedHardware::DmDeviceInfo &deviceInfo);
    void OnDeviceFound(uint16_t subscribeId, const OHOS::DistributedHardware::DmDeviceInfo &deviceInfo);
    void OnDiscoveryFailed(uint16_t subscribeId, int32_t failedReason);
    void OnAuthResult(const std::string &deviceId, const std::string &token, int32_t status, int32_t reason);
    void OnVerifyResult(const std::string &deviceId, int32_t resultCode, int32_t flag);
    void OnDmfaCall(const std::string &paramJson);

private:
    static napi_value JsOffFrench(napi_env env, int32_t num, napi_value thisVar, napi_value argv[]);
    static napi_value JsOnFrench(napi_env env, int32_t num, napi_value thisVar, napi_value argv[]);
    static void CallAsyncWorkSync(napi_env env, DeviceInfoAsyncCallbackInfo *deviceInfoAsyncCallbackInfo);
    static void CallAsyncWork(napi_env env, DeviceInfoAsyncCallbackInfo *deviceInfoAsyncCallbackInfo);
    static void CallAsyncWorkSync(napi_env env, DeviceInfoListAsyncCallbackInfo *deviceInfoListAsyncCallbackInfo);
    static void CallAsyncWork(napi_env env, DeviceInfoListAsyncCallbackInfo *deviceInfoListAsyncCallbackInfo);
    static void CallGetTrustedDeviceListStatusSync(napi_env env, napi_status &status,
                                                   DeviceInfoListAsyncCallbackInfo *deviceInfoListAsyncCallbackInfo);
    static void CallGetTrustedDeviceListStatus(napi_env env, napi_status &status,
                                               DeviceInfoListAsyncCallbackInfo *deviceInfoListAsyncCallbackInfo);
    static napi_value CallDeviceList(napi_env env, napi_callback_info info,
                                     DeviceInfoListAsyncCallbackInfo *deviceInfoListAsyncCallbackInfo);
    static void CallGetLocalDeviceInfoSync(napi_env env, napi_status &status,
                                           DeviceInfoAsyncCallbackInfo *deviceInfoAsyncCallbackInfo);
    static void CallGetLocalDeviceInfo(napi_env env, napi_status &status,
                                       DeviceInfoAsyncCallbackInfo *deviceInfoAsyncCallbackInfo);

private:
    napi_env env_;
    napi_ref wrapper_;
    static thread_local napi_ref sConstructor_;
    std::string bundleName_;
    static AuthAsyncCallbackInfo authAsyncCallbackInfo_;
    static AuthAsyncCallbackInfo verifyAsyncCallbackInfo_;
};

#endif // OHOS_DM_NATIVE_DEVICEMANAGER_JS_H
