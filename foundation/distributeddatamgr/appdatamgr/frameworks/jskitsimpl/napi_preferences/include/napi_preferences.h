/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef PREFERENCES_JSKIT_NAPI_PREFERENCE_H
#define PREFERENCES_JSKIT_NAPI_PREFERENCE_H

#include <assert.h>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "preferences.h"
#include "preferences_helper.h"

namespace OHOS {
namespace PreferencesJsKit {
class PreferencesProxy {
public:
    static void Init(napi_env env, napi_value exports);
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_status NewInstance(napi_env env, napi_value arg, napi_value *instance);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

private:
    explicit PreferencesProxy(std::shared_ptr<OHOS::NativePreferences::Preferences> &value);
    ~PreferencesProxy();

    static napi_value GetValue(napi_env env, napi_callback_info info);
    static napi_value SetValue(napi_env env, napi_callback_info info);
    static napi_value HasKey(napi_env env, napi_callback_info info);
    static napi_value Delete(napi_env env, napi_callback_info info);
    static napi_value Flush(napi_env env, napi_callback_info info);
    static napi_value Clear(napi_env env, napi_callback_info info);
    static napi_value RegisterObserver(napi_env env, napi_callback_info info);
    static napi_value UnRegisterObserver(napi_env env, napi_callback_info info);

    std::shared_ptr<OHOS::NativePreferences::Preferences> value_;
    napi_env env_;
    napi_ref wrapper_;
    std::shared_ptr<OHOS::NativePreferences::PreferencesObserver> observer_;
};

class PreferencesObserverImpl : public OHOS::NativePreferences::PreferencesObserver {
public:
    PreferencesObserverImpl(napi_env env, napi_value callback);
    virtual ~PreferencesObserverImpl();
    void OnChange(OHOS::NativePreferences::Preferences &preferences, const std::string &key) override;

private:
    napi_env env_;
    napi_ref observerRef;
};
} // namespace PreferencesJsKit
} // namespace OHOS

#endif // PREFERENCES_JSKIT_NAPI_PREFERENCE_H
