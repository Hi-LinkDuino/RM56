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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RESOURCE_PLUGIN_CALLBACK_CLIENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RESOURCE_PLUGIN_CALLBACK_CLIENT_H

namespace OHOS::Ace {
class PluginCallbackClient : public std::enable_shared_from_this<PluginCallbackClient> {
public:
    PluginCallbackClient() = default;
    virtual ~PluginCallbackClient() = default;

    void OnDeathReceived()
    {
        auto delegate = delegate_.Upgrade();
        if (delegate) {
            delegate->OnDeathReceived();
        }
    }

    void SetPluginManagerDelegate(WeakPtr<PluginManagerDelegate> delegate)
    {
        delegate_ = delegate;
    }

private:
    WeakPtr<PluginManagerDelegate> delegate_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RESOURCE_PLUGIN_CALLBACK_CLIENT_H