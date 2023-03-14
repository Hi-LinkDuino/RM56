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
#define HST_LOG_TAG "HttpCurlClient"

#include "http_curl_client.h"
#include <algorithm>
#include <regex>
#include <vector>
#include "foundation/log.h"
#include "securec.h"

#ifndef CA_DIR
#define CA_DIR "/etc/ssl/certs/"
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
HttpCurlClient::HttpCurlClient(RxHeader headCallback, RxBody bodyCallback, void *userParam)
    : rxHeader_(headCallback), rxBody_(bodyCallback), userParam_(userParam)
{
    MEDIA_LOG_I("HttpCurlClient ctor");
}

HttpCurlClient::~HttpCurlClient()
{
    MEDIA_LOG_I("~HttpCurlClient dtor");
}

Status HttpCurlClient::Init()
{
    FALSE_LOG(curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK);
    return Status::OK;
}

Status HttpCurlClient::Open(const std::string& url)
{
    easyHandle_ = curl_easy_init();
    FALSE_RETURN_V(easyHandle_ != nullptr, Status::ERROR_NULL_POINTER);
    InitCurlEnvironment(url);
    return Status::OK;
}

Status HttpCurlClient::Close()
{
    MEDIA_LOG_I("Close client");
    curl_easy_setopt(easyHandle_, CURLOPT_TIMEOUT, 1);
    if (easyHandle_) {
        curl_easy_cleanup(easyHandle_);
        easyHandle_ = nullptr;
    }
    return Status::OK;
}

Status HttpCurlClient::Deinit()
{
    if (easyHandle_) {
        curl_easy_cleanup(easyHandle_);
        easyHandle_ = nullptr;
    }
    curl_global_cleanup();
    return Status::OK;
}

bool HttpCurlClient::CheckUrl(const std::string& url)
{
    easyHandle_ = curl_easy_init();
    FALSE_RETURN_V(easyHandle_ != nullptr, false);
    InitCurlEnvironment(url, true);
    int serverCode {0};
    int clientCode = curl_easy_perform(easyHandle_);
    if (clientCode == CURLE_OK) {
        curl_easy_getinfo(easyHandle_, CURLINFO_RESPONSE_CODE, &serverCode);
    }
    // 200 request success, 206 partial download
    FALSE_RETURN_V_MSG_E(serverCode == 200 || serverCode == 206, false, "Check url (" PUBLIC_LOG_S ") failed, "
        "client code(" PUBLIC_LOG_D32 "), server code(" PUBLIC_LOG_D32 ").", url.c_str(), clientCode, serverCode);
    return true;
}

void HttpCurlClient::InitCurlEnvironment(const std::string& url, bool isChecked)
{
    curl_easy_setopt(easyHandle_, CURLOPT_URL, UrlParse(url).c_str());
    curl_easy_setopt(easyHandle_, CURLOPT_CONNECTTIMEOUT, 5); // 5

    curl_easy_setopt(easyHandle_, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(easyHandle_, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(easyHandle_, CURLOPT_CAINFO, CA_DIR "cacert.pem");
    if (isChecked) {
        curl_easy_setopt(easyHandle_, CURLOPT_RANGE, "0-1");
        return;
    }
    curl_easy_setopt(easyHandle_, CURLOPT_HTTPGET, 1L);

    curl_easy_setopt(easyHandle_, CURLOPT_FORBID_REUSE, 0L);
    curl_easy_setopt(easyHandle_, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(easyHandle_, CURLOPT_VERBOSE, 1);

    curl_easy_setopt(easyHandle_, CURLOPT_WRITEFUNCTION, rxBody_);
    curl_easy_setopt(easyHandle_, CURLOPT_WRITEDATA, userParam_);
    curl_easy_setopt(easyHandle_, CURLOPT_HEADERFUNCTION, rxHeader_);
    curl_easy_setopt(easyHandle_, CURLOPT_HEADERDATA, userParam_);

    curl_easy_setopt(easyHandle_, CURLOPT_LOW_SPEED_LIMIT, 2); // 2
    curl_easy_setopt(easyHandle_, CURLOPT_LOW_SPEED_TIME, 5); // 连续5s下载速度低于2kb/s会触发timeout

    curl_easy_setopt(easyHandle_, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(easyHandle_, CURLOPT_TCP_KEEPINTVL, 5L); // 5 心跳
}

std::string HttpCurlClient::UrlParse(const std::string& url) const
{
    std::string s;
    std::regex_replace(std::back_inserter(s), url.begin(), url.end(), std::regex(" "), "%20");
    return s;
}

// RequestData run in HttpDownload thread,
// Open, Close, Deinit run in other thread.
// Should call Open before start HttpDownload thread.
// Should Pause HttpDownload thread then Close, Deinit.
Status HttpCurlClient::RequestData(long startPos, int len, NetworkServerErrorCode& serverCode,
                                   NetworkClientErrorCode& clientCode)
{
    FALSE_RETURN_V(easyHandle_ != nullptr, Status::ERROR_NULL_POINTER);
    if (startPos >= 0) {
        char requestRange[128] = {0};
        if (len > 0) {
            snprintf_s(requestRange, sizeof(requestRange), sizeof(requestRange) - 1, "%ld-%ld",
                       startPos, startPos + len -1);
        } else {
            snprintf_s(requestRange, sizeof(requestRange), sizeof(requestRange) - 1, "%ld-", startPos);
        }
        curl_easy_setopt(easyHandle_, CURLOPT_RANGE, requestRange);
    }
    curl_slist *headers {nullptr};
    headers = curl_slist_append(headers, "Connection: Keep-alive");
    headers = curl_slist_append(headers, "Keep-Alive: timeout=120");
    curl_easy_setopt(easyHandle_, CURLOPT_HTTPHEADER, headers);
    
    MEDIA_LOG_D("RequestData: startPos " PUBLIC_LOG_D32 ", len " PUBLIC_LOG_D32, static_cast<int>(startPos), len);
    CURLcode returnCode = curl_easy_perform(easyHandle_);
    if (headers != nullptr) {
        curl_slist_free_all(headers);
    }
    clientCode = NetworkClientErrorCode::ERROR_OK;
    serverCode = 0;
    if (returnCode != CURLE_OK) {
        MEDIA_LOG_E("Curl error " PUBLIC_LOG_D32, returnCode);
        if (returnCode == CURLE_COULDNT_CONNECT || returnCode == CURLE_OPERATION_TIMEDOUT) {
            clientCode = NetworkClientErrorCode::ERROR_TIME_OUT;
        } else {
            clientCode = NetworkClientErrorCode::ERROR_UNKNOWN;
        }
        return Status::ERROR_CLIENT;
    } else {
        int httpCode = 0;
        curl_easy_getinfo(easyHandle_, CURLINFO_RESPONSE_CODE, &httpCode);
        if (httpCode >= 400) { // 400
            MEDIA_LOG_E("Http error " PUBLIC_LOG_D32, httpCode);
            serverCode = httpCode;
            return Status::ERROR_SERVER;
        }
    }
    return Status::OK;
}
}
}
}
}