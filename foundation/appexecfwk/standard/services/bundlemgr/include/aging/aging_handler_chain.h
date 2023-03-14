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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_HANDLER_CHAIN_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_HANDLER_CHAIN_H

#include <map>
#include <memory>

#include "aging_handler.h"
#include "aging_request.h"

namespace OHOS {
namespace AppExecFwk {
class AgingHandlerChain {
public:
    AgingHandlerChain();
    ~AgingHandlerChain();
    void AddHandler(const std::shared_ptr<AgingHandler> &handler);
    bool Process(AgingRequest &request) const;

private:
    std::vector<std::shared_ptr<AgingHandler>> handlers;
};
}  //  namespace AppExecFwk
}  //  namespace OHOS
#endif  //  FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_AGING_HANDLER_CHAIN_H