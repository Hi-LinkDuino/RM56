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

#ifndef LAUNCHER_STATUS_CALLBACK_H
#define LAUNCHER_STATUS_CALLBACK_H

#include <future>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "bundle_status_callback_interface.h"
#include "iremote_stub.h"
#include "nocopyable.h"

class BundleStatusCallback : public OHOS::IRemoteStub<OHOS::AppExecFwk::IBundleStatusCallback> {
public:
    BundleStatusCallback() = default;
    BundleStatusCallback(napi_env env, napi_ref addedCallback, napi_ref updatedCallback, napi_ref removeCallback);
    virtual ~BundleStatusCallback();
    virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
                                      const std::string &bundleName) override {};
    virtual void OnBundleAdded(const std::string &bundleName, const int userId) override;
    virtual void OnBundleUpdated(const std::string &bundleName, const int userId) override;
    virtual void OnBundleRemoved(const std::string &bundleName, const int userId) override;

private:
    napi_env env_;
    napi_ref addedCallback_;
    napi_ref updatedCallback_;
    napi_ref removeCallback_;
    DISALLOW_COPY_AND_MOVE(BundleStatusCallback);
};

struct AsyncCallbackInfo {
    napi_env env_;
    napi_ref callback_ = 0;
    std::string bundleName_;
    int32_t userId_;
};

struct DelRefCallbackInfo {
    napi_env env_;
    napi_ref addedCallback_;
    napi_ref updatedCallback_;
    napi_ref removeCallback_;
};

#endif  // LAUNCHER_STATUS_CALLBACK_H