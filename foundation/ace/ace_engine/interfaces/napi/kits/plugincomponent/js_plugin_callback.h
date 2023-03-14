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

#ifndef OHOS_NAPI_ACE_PLUGIN_CALLBACK_H
#define OHOS_NAPI_ACE_PLUGIN_CALLBACK_H

#include <atomic>
#include <string>
#include <uv.h>
#include "core/components/plugin/plugin_component_callback.h"
#include "js_plugin_data.h"

namespace OHOS::Ace::Napi {
using namespace OHOS::Ace;

struct OnPluginUvWorkData {
    AAFwk::Want want;
    std::string sourceName;
    std::string abilityName;
    std::string data;
    std::string extraData;
    std::string name;
    void *that = nullptr;
};

class AceJSPluginRequestParam {
public:
    AceJSPluginRequestParam(const AAFwk::Want& want, const std::string name, const std::string data,
        const std::string jsonPath) : want_(want), name_(name), data_(data), jsonPath_(jsonPath)
    {}
    ~AceJSPluginRequestParam() = default;
    bool operator==(const AceJSPluginRequestParam& param) const;
    bool operator!=(const AceJSPluginRequestParam& param) const;

public:
    AAFwk::Want want_;
    std::string name_;
    std::string data_;
    std::string jsonPath_;

    ACE_DISALLOW_COPY_AND_MOVE(AceJSPluginRequestParam);
};

class JSPluginCallback : public PluginComponentCallBack {
public:
    JSPluginCallback(CallBackType eventType, ACECallbackInfo& cbInfo);
    virtual ~JSPluginCallback();
    void SetWant(const AAFwk::Want& want);
    AAFwk::Want& GetWant();
    void SetRequestParam(const std::shared_ptr<AceJSPluginRequestParam>& param);
    bool OnEventStrictEquals(CallBackType eventType, const AAFwk::Want& want, ACECallbackInfo& cbInfo);
    bool RequestStrictEquals(CallBackType eventType, const AAFwk::Want& want, ACECallbackInfo& cbInfo,
        const std::shared_ptr<AceJSPluginRequestParam>& param);

    size_t GetID(void);
    void OnPushEvent(const AAFwk::Want& want, const PluginComponentTemplate& pluginTemplate, const std::string& data,
        const std::string& extraData) override;
    void OnRequestEvent(const AAFwk::Want& want, const std::string& name, const std::string& data) override;
    void OnRequestCallBack(
        const PluginComponentTemplate& pluginTemplate, const std::string& data, const std::string& extraData) override;

private:
    void DestroyAllResource(void);
    void SendRequestEventResult(napi_value jsObject);
    napi_value MakeCallbackParamForRequest(const PluginComponentTemplate& pluginTemplate,
        const std::string& data, const std::string& extraData);
    napi_value MakePluginTemplateObject(const PluginComponentTemplate& pluginTemplate);
    void OnPushEventInner(const OnPluginUvWorkData* workData);
    void OnRequestEventInner(const OnPluginUvWorkData* workData);
    void OnRequestCallBackInner(const OnPluginUvWorkData* workData);

    AAFwk::Want want_;
    ACECallbackInfo cbInfo_;
    CallBackType eventType_;
    std::shared_ptr<AceJSPluginRequestParam> requestParam_ = nullptr;
    static std::atomic_size_t uuid_;
    OnPluginUvWorkData uvWorkData_;
};
}  // namespace OHOS::Ace::Napi
#endif  // OHOS_NAPI_ACE_PLUGIN_CALLBACK_H
