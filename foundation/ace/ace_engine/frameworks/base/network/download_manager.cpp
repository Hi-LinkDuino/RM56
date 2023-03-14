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

#include "base/network/download_manager.h"

#include <memory>
#include <mutex>

#include "curl/curl.h"

#include "base/log/log.h"
#include "base/utils/singleton.h"
#include "base/utils/utils.h"

#define ACE_CURL_EASY_SET_OPTION(handle, opt, data) \
    do { \
        CURLcode result = curl_easy_setopt(handle, opt, data); \
        if (result != CURLE_OK) { \
            LOGE("Failed to set option: %{public}s, %{public}s", #opt, curl_easy_strerror(result)); \
            return false; \
        } \
    } while (0)

namespace OHOS::Ace {
namespace {

class DownloadManagerImpl final : public DownloadManager, public Singleton<DownloadManagerImpl> {
    DECLARE_SINGLETON(DownloadManagerImpl);
    ACE_DISALLOW_MOVE(DownloadManagerImpl);

public:
    bool Download(const std::string& url, std::vector<uint8_t>& dataOut) override
    {
        if (!Initialize()) {
            return false;
        }

        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> handle(curl_easy_init(), &curl_easy_cleanup);
        if (!handle) {
            LOGE("Failed to create download task");
            return false;
        }

        dataOut.clear();
        std::string errorStr;
        errorStr.reserve(CURL_ERROR_SIZE);

        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_URL, url.c_str());
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_WRITEFUNCTION, OnWritingMemory);
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_WRITEDATA, &dataOut);
        // Some servers don't like requests that are made without a user-agent field, so we provide one
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_USERAGENT, "libcurl-agent/1.0");
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_URL, url.c_str());
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_CAINFO, "/etc/ssl/certs/cacert.pem");
#endif
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_VERBOSE, 1L);
        ACE_CURL_EASY_SET_OPTION(handle.get(), CURLOPT_ERRORBUFFER, errorStr.data());

        CURLcode result = curl_easy_perform(handle.get());
        if (result != CURLE_OK) {
            LOGE("Failed to download, url: %{private}s, %{public}s", url.c_str(), curl_easy_strerror(result));
            if (!errorStr.empty()) {
                LOGE("Failed to download reason: %{public}s", errorStr.c_str());
            }
            dataOut.clear();
            return false;
        }
        dataOut.shrink_to_fit();
        return true;
    }

private:
    static size_t OnWritingMemory(void* data, size_t size, size_t memBytes, void* userData)
    {
        // size is always 1, for more details see https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
        auto& dataOut = *static_cast<std::vector<uint8_t>*>(userData);
        auto chunkData = static_cast<uint8_t*>(data);
        dataOut.insert(dataOut.end(), chunkData, chunkData + memBytes);
        return memBytes;
    }

    bool Initialize()
    {
        if (initialized_) {
            return true;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        if (initialized_) {
            return true;
        }
        if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
            LOGE("Failed to initialize 'curl'");
            return false;
        }
        initialized_ = true;
        return true;
    }

    std::mutex mutex_;
    bool initialized_ = false;
};

DownloadManagerImpl::DownloadManagerImpl() = default;

DownloadManagerImpl::~DownloadManagerImpl()
{
    curl_global_cleanup();
}

}

DownloadManager& DownloadManager::GetInstance()
{
    return Singleton<DownloadManagerImpl>::GetInstance();
}

} // namespace OHOS::Ace
