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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_WEB_WEB_CLIENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_WEB_WEB_CLIENT_H

#include <functional>

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class WebClient {
public:
    using ReloadCallback = std::function<bool()>;
    using UpdateUrlCallback = std::function<void(const std::string& url)>;
    WebClient &operator = (const WebClient &) = delete;
    WebClient(const WebClient &) = delete;
    ~WebClient() = default;

    static WebClient& GetInstance()
    {
        static WebClient instance;
        return instance;
    }

    void RegisterReloadCallback(ReloadCallback &&callback)
    {
        reloadCallback_ = callback;
    }

    void RegisterUpdageUrlCallback(UpdateUrlCallback &&callback)
    {
        updateUrlCallback_ = callback;
    }

    void UpdateWebviewUrl(const std::string& url)
    {
        if (updateUrlCallback_) {
            return updateUrlCallback_(url);
        }
    }

    bool ReloadWebview()
    {
        if (reloadCallback_) {
            return reloadCallback_();
        } else {
            return false;
        }
    }

private:
    WebClient() = default;
    ReloadCallback reloadCallback_;
    UpdateUrlCallback updateUrlCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_WEB_WEB_CLIENT_H
