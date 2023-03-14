/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#define HST_LOG_TAG "ClientFactory"
#include "client_factory.h"
#include "foundation/log.h"
#include "http_curl_client.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
ClientFactory::ClientFactory(RxHeader headCallback, RxBody bodyCallback, void *userParam)
    : rxHeader_(headCallback), rxBody_(bodyCallback), userParam_(userParam)
{
}

ClientFactory::~ClientFactory()
{
    if (httpClient_ != nullptr) {
        httpClient_->Deinit();
        httpClient_ = nullptr;
    }
}

std::shared_ptr<NetworkClient> ClientFactory::CreateClient(const std::string& url)
{
    FALSE_RETURN_V(!url.empty(), nullptr);
    if (url.compare(0, 4, "http") == 0) { // 0 : position, 4: count
        if (httpClient_ == nullptr) {
            httpClient_ = std::make_shared<HttpCurlClient>(rxHeader_, rxBody_, userParam_);
            httpClient_->Init();
        }
        return httpClient_;
    }
    return nullptr;
}
}
}
}
}