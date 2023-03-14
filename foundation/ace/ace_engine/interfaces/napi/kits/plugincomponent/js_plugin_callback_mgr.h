/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_NAPI_ACE_PLUGIN_CALLBACK_MGR_H
#define OHOS_NAPI_ACE_PLUGIN_CALLBACK_MGR_H

#include <map>
#include <mutex>
#include <string>

#include "js_plugin_callback.h"
#include "js_plugin_data.h"

namespace OHOS::Ace::Napi {
using namespace OHOS::Ace;

class JSPluginCallbackMgr final {
public:
    JSPluginCallbackMgr();
    ~JSPluginCallbackMgr();

    static JSPluginCallbackMgr& Instance(void);
    bool RegisterOnEvent(napi_env env, CallBackType eventType, const AAFwk::Want& want, ACECallbackInfo& cbInfo);
    bool RegisterRequestEvent(napi_env env, const AAFwk::Want& want, ACECallbackInfo& cbInfo,
        const std::shared_ptr<AceJSPluginRequestParam>& param);
    void UnRegisterEvent(size_t key);
    void UnregisterCallBack(napi_env env, const AAFwk::Want& want);
private:
    std::mutex mutex_;
    std::map<size_t, std::shared_ptr<JSPluginCallback>> eventList_;
    ACE_DISALLOW_COPY_AND_MOVE(JSPluginCallbackMgr);
};
}  // namespace OHOS::Ace::Napi
#endif  // OHOS_NAPI_ACE_PLUGIN_CALLBACK_MGR_H
