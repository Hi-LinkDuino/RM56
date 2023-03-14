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

#ifndef OHOS_DM_AUTH_MESSAGE_PROCESSOR_H
#define OHOS_DM_AUTH_MESSAGE_PROCESSOR_H

#include <memory>
#include <vector>

#include "crypto_adapter.h"
#include "dm_auth_manager.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace DistributedHardware {
class DmAuthManager;
struct DmAuthRequestContext;
struct DmAuthResponseContext;
class ICryptoAdapter;
class AuthMessageProcessor {
public:
    explicit AuthMessageProcessor(std::shared_ptr<DmAuthManager> authMgr);
    ~AuthMessageProcessor();
    std::vector<std::string> CreateAuthRequestMessage();
    std::string CreateSimpleMessage(int32_t msgType);
    int32_t ParseMessage(const std::string &message);
    void SetRequestContext(std::shared_ptr<DmAuthRequestContext> authRequestContext);
    void SetResponseContext(std::shared_ptr<DmAuthResponseContext> authResponseContext);
    std::shared_ptr<DmAuthResponseContext> GetResponseContext();
    std::shared_ptr<DmAuthRequestContext> GetRequestContext();

private:
    std::string CreateRequestAuthMessage(nlohmann::json &json);
    void CreateNegotiateMessage(nlohmann::json &json);
    void CreateSyncGroupMessage(nlohmann::json &json);
    void CreateResponseAuthMessage(nlohmann::json &json);
    void ParseAuthResponseMessage(nlohmann::json &json);
    int32_t ParseAuthRequestMessage();
    void ParseNegotiateMessage(const nlohmann::json &json);
    void CreateResponseFinishMessage(nlohmann::json &json);
    void ParseResponseFinishMessage(nlohmann::json &json);

private:
    std::weak_ptr<DmAuthManager> authMgr_;
    std::shared_ptr<ICryptoAdapter> cryptoAdapter_;
    std::shared_ptr<DmAuthRequestContext> authRequestContext_;
    std::shared_ptr<DmAuthResponseContext> authResponseContext_;
    std::vector<nlohmann::json> authSplitJsonList_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_AUTH_MESSAGE_PROCESSOR_H
