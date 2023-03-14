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

#ifndef HISTREAMER_HTTP_CURL_CLIENT_H
#define HISTREAMER_HTTP_CURL_CLIENT_H

#include <string>
#include "network_client.h"
#include "curl/curl.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
class HttpCurlClient : public NetworkClient {
public:
    HttpCurlClient(RxHeader headCallback, RxBody bodyCallback, void* userParam);

    ~HttpCurlClient() override;

    Status Init() override;

    Status Open(const std::string& url) override;

    Status RequestData(long startPos, int len, NetworkServerErrorCode& serverCode,
                       NetworkClientErrorCode& clientCode) override;

    Status Close() override;

    Status Deinit() override;
    bool CheckUrl(const std::string& url);
private:
    void InitCurlEnvironment(const std::string& url, bool isChecked = false);
    std::string UrlParse(const std::string& url) const;
private:
    RxHeader rxHeader_;
    RxBody rxBody_;
    void *userParam_;
    CURL* easyHandle_ {nullptr};
};
}
}
}
}
#endif