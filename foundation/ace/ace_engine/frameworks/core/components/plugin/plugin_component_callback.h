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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_COMPONENT_CALLBACK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_COMPONENT_CALLBACK_H

#include "core/components/plugin/plugin_component_template.h"
#include "ohos/aafwk/content/want.h"

namespace OHOS::Ace {
enum class CallBackType {
    UndefineType = -1,
    PushEvent = 0,
    RequestEvent,
    RequestCallBack,
};

class PluginComponentCallBack {
public:
    PluginComponentCallBack() = default;
    virtual ~PluginComponentCallBack() = default;

    virtual void OnPushEvent(const AAFwk::Want& want, const PluginComponentTemplate& pluginTemplate,
        const std::string& data, const std::string& extraData) = 0;
    virtual void OnRequestEvent(const AAFwk::Want& want, const std::string& name, const std::string& data) = 0;
    virtual void OnRequestCallBack(const PluginComponentTemplate& pluginTemplate,
        const std::string& data, const std::string& extraData) = 0;

    ACE_DISALLOW_COPY_AND_MOVE(PluginComponentCallBack);
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_COMPONENT_CALLBACK_H
