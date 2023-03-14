/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "aging/aging_handler_chain.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
AgingHandlerChain::AgingHandlerChain()
{
}

AgingHandlerChain::~AgingHandlerChain()
{
    handlers.clear();
    APP_LOGD("AgingHandlerChain is destroyed");
}

void AgingHandlerChain::AddHandler(const ::std::shared_ptr<AgingHandler> &handler)
{
    if (handler == nullptr) {
        APP_LOGE("agingHandler: invalid handler.");
        return;
    }
    handlers.emplace_back(handler);
    APP_LOGD("agingHandler: %{public}s is added into handlers", handler->GetName().c_str());
}

bool AgingHandlerChain::Process(AgingRequest &request) const
{
    for (auto handler : handlers) {
        bool passed = handler->Process(request);
        APP_LOGD("agingHandler: %{public}s process passed: %{public}d", handler->GetName().c_str(), passed);
        if (!passed) {
            break;
        }
    }
    APP_LOGD("agingHandler: aging handler chain process done.");
    return request.IsReachEndAgingThreshold();
}
}  //  namespace AppExecFwk
}  //  namespace OHOS