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
#define HST_LOG_TAG "Downloader"

#include "downloader.h"
#include <algorithm>

#include "foundation/log.h"
#include "http_curl_client.h"
#include "osal/utils/util.h"
#include "securec.h"
#include "steady_clock.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
namespace {
constexpr int PER_REQUEST_SIZE = 48 * 1024;
constexpr unsigned int SLEEP_TIME = 5;    // Sleep 5ms
constexpr size_t RETRY_TIMES = 200;  // Retry 200 times
constexpr size_t REQUEST_QUEUE_SIZE = 50;
}

DownloadRequest::DownloadRequest(const std::string& url, DataSaveFunc saveData, StatusCallbackFunc statusCallback,
                                 bool requestWholeFile)
    : url_(url), saveData_(std::move(saveData)), statusCallback_(std::move(statusCallback)),
    requestWholeFile_(requestWholeFile)
{
    (void)memset_s(&headerInfo_, sizeof(HeaderInfo), 0x00, sizeof(HeaderInfo));
    headerInfo_.fileContentLen = 0;
    headerInfo_.contentLen = 0;
}

size_t DownloadRequest::GetFileContentLength() const
{
    WaitHeaderUpdated();
    return headerInfo_.GetFileContentLength();
}

void DownloadRequest::SaveHeader(const HeaderInfo* header)
{
    headerInfo_.Update(header);
    isHeaderUpdated = true;
}

bool DownloadRequest::IsChunked() const
{
    WaitHeaderUpdated();
    return headerInfo_.isChunked;
};

bool DownloadRequest::IsEos() const
{
    return isEos_;
}

int DownloadRequest::GetRetryTimes()
{
    return retryTimes_;
}

NetworkClientErrorCode DownloadRequest::GetClientError()
{
    return clientError_;
}

NetworkServerErrorCode DownloadRequest::GetServerError()
{
    return serverError_;
}

void DownloadRequest::WaitHeaderUpdated() const
{
    size_t times = 0;
    while (!isHeaderUpdated && times < RETRY_TIMES) { // Wait Header(fileContentLen etc.) updated
        OSAL::SleepFor(SLEEP_TIME);
        times++;
    }
    MEDIA_LOG_D("isHeaderUpdated " PUBLIC_LOG_D32 ", times " PUBLIC_LOG_ZU, isHeaderUpdated, times);
}

Downloader::Downloader(std::string name) noexcept : name_(std::move(name))
{
    shouldStartNextRequest = true;

    client_ = std::make_shared<HttpCurlClient>(&RxHeaderData, &RxBodyData, this);
    client_->Init();
    requestQue_ = std::make_shared<BlockingQueue<std::shared_ptr<DownloadRequest>>>(name_ + "RequestQue",
                                                                                    REQUEST_QUEUE_SIZE);

    task_ = std::make_shared<OSAL::Task>(std::string(name_ + "Downloader"));
    task_->RegisterHandler([this] { HttpDownloadLoop(); });
}

bool Downloader::Download(const std::shared_ptr<DownloadRequest>& request, int32_t waitMs)
{
    MEDIA_LOG_I("In");
    if (waitMs == -1) { // wait until push success
        requestQue_->Push(request);
        return true;
    }
    return requestQue_->Push(request, static_cast<int>(waitMs));
}

void Downloader::Start()
{
    MEDIA_LOG_I("Begin");
    task_->Start();
    MEDIA_LOG_I("End");
}

void Downloader::Pause()
{
    MEDIA_LOG_I("Begin");
    task_->Pause();
    client_->Close();
    MEDIA_LOG_I("End");
}

void Downloader::Resume()
{
    MEDIA_LOG_I("Begin");
    client_->Open(currentRequest_->url_);
    currentRequest_->isEos_ = false;
    Start();
    MEDIA_LOG_I("End");
}

void Downloader::Stop()
{
    MEDIA_LOG_I("Begin");
    requestQue_->SetActive(false);
    task_->Stop();
    MEDIA_LOG_I("End");
}

bool Downloader::Seek(int64_t offset)
{
    MEDIA_LOG_I("Begin");
    if (offset >= 0 && offset < currentRequest_->GetFileContentLength()) {
        currentRequest_->startPos_ = offset;
    }
    int64_t temp = currentRequest_->GetFileContentLength() - currentRequest_->startPos_;
    currentRequest_->requestSize_ = static_cast<int>(std::min(temp, static_cast<int64_t>(PER_REQUEST_SIZE)));
    currentRequest_->isEos_ = false;
    shouldStartNextRequest = false; // Reuse last request when seek
    return true;
}

// Pause download thread before use currentRequest_
bool Downloader::Retry(const std::shared_ptr<DownloadRequest>& request)
{
    MEDIA_LOG_I(PUBLIC_LOG_S " Retry Begin, url : " PUBLIC_LOG_S, name_.c_str(), request->url_.c_str());
    FALSE_RETURN_V(client_ != nullptr, false);
    Pause();
    if (currentRequest_ != nullptr && currentRequest_->IsSame(request) && !shouldStartNextRequest) {
        currentRequest_->retryTimes_++;
        MEDIA_LOG_D("Do retry.");
    }
    Resume();
    MEDIA_LOG_I("Downloader Retry End");
    return true;
}

bool Downloader::BeginDownload()
{
    MEDIA_LOG_I("Begin");
    std::string url = currentRequest_->url_;
    FALSE_RETURN_V(!url.empty(), false);

    client_->Open(url);

    currentRequest_->requestSize_ = 1;
    currentRequest_->startPos_ = 0;
    currentRequest_->isEos_ = false;
    currentRequest_->retryTimes_ = 0;

    task_->Start();
    MEDIA_LOG_I("End");
    return true;
}

void Downloader::HttpDownloadLoop()
{
    if (shouldStartNextRequest) {
        currentRequest_ = requestQue_->Pop(); // 1000);
        if (!currentRequest_) {
            return;
        }
        BeginDownload();
        shouldStartNextRequest = false;
    }
    FALSE_RETURN_W(currentRequest_ != nullptr);
    NetworkClientErrorCode clientCode = NetworkClientErrorCode::ERROR_OK;
    NetworkServerErrorCode serverCode = 0;
    long startPos = currentRequest_->startPos_;
    if (currentRequest_->requestWholeFile_) {
        startPos = -1;
    }
    Status ret = client_->RequestData(startPos, currentRequest_->requestSize_,
                                      serverCode, clientCode);
    currentRequest_->clientError_ = clientCode;
    currentRequest_->serverError_ = serverCode;
    if (ret == Status::OK) {
        if (currentRequest_->retryTimes_ > 0) {
            currentRequest_->retryTimes_ = 0;
        }
        int64_t remaining = currentRequest_->headerInfo_.fileContentLen - currentRequest_->startPos_;
        if (currentRequest_->headerInfo_.fileContentLen > 0 && remaining <= 0) { // 检查是否播放结束
            MEDIA_LOG_I("http transfer reach end, startPos_ " PUBLIC_LOG_D64 " url: " PUBLIC_LOG_S,
                        currentRequest_->startPos_, currentRequest_->url_.c_str());
            currentRequest_->isEos_ = true;
            if (requestQue_->Empty()) {
                task_->PauseAsync();
            }
            shouldStartNextRequest = true;
        } else if (remaining < PER_REQUEST_SIZE) {
            currentRequest_->requestSize_ = remaining;
        } else {
            currentRequest_->requestSize_ = PER_REQUEST_SIZE;
        }
    } else {
        MEDIA_LOG_E("Client request data failed. ret = " PUBLIC_LOG_D32, static_cast<int32_t>(ret));
        std::shared_ptr<Downloader> unused;
        currentRequest_->statusCallback_(DownloadStatus::PARTTAL_DOWNLOAD, unused, currentRequest_);
        task_->PauseAsync();
    }
}

size_t Downloader::RxBodyData(void* buffer, size_t size, size_t nitems, void* userParam)
{
    auto mediaDownloader = static_cast<Downloader *>(userParam);
    HeaderInfo* header = &(mediaDownloader->currentRequest_->headerInfo_);
    size_t dataLen = size * nitems;

    if (header->fileContentLen == 0) {
        if (header->contentLen > 0) {
            MEDIA_LOG_W("Unsupported range, use content length as content file length");
            header->fileContentLen = header->contentLen;
        } else {
            MEDIA_LOG_E("fileContentLen and contentLen are both zero.");
            return 0;
        }
    }
    if (!mediaDownloader->currentRequest_->isDownloading_) {
        mediaDownloader->currentRequest_->isDownloading_ = true;
    }
    mediaDownloader->currentRequest_->saveData_(static_cast<uint8_t*>(buffer), dataLen,
                                                mediaDownloader->currentRequest_->startPos_);
    mediaDownloader->currentRequest_->isDownloading_ = false;
    MEDIA_LOG_I("RxBodyData: dataLen " PUBLIC_LOG_ZU ", startPos_ " PUBLIC_LOG_D64, dataLen,
                mediaDownloader->currentRequest_->startPos_);
    mediaDownloader->currentRequest_->startPos_ = mediaDownloader->currentRequest_->startPos_ + dataLen;

    return dataLen;
}

namespace {
char* StringTrim(char* str)
{
    if (str == nullptr) {
        return nullptr;
    }
    char* p = str;
    char* p1 = p + strlen(str) - 1;

    while (*p && isspace(static_cast<int>(*p))) {
        p++;
    }
    while (p1 > p && isspace(static_cast<int>(*p1))) {
        *p1-- = 0;
    }
    return p;
}
}

size_t Downloader::RxHeaderData(void* buffer, size_t size, size_t nitems, void* userParam)
{
    auto mediaDownloader = reinterpret_cast<Downloader *>(userParam);
    HeaderInfo* info = &(mediaDownloader->currentRequest_->headerInfo_);
    char* next = nullptr;
    char* key = strtok_s(reinterpret_cast<char*>(buffer), ":", &next);
    FALSE_RETURN_V(key != nullptr, size * nitems);
    if (!strncmp(key, "Content-Type", strlen("Content-Type"))) {
        char* token = strtok_s(nullptr, ":", &next);
        FALSE_RETURN_V(token != nullptr, size * nitems);
        char* type = StringTrim(token);
        (void)memcpy_s(info->contentType, sizeof(info->contentType), type, sizeof(info->contentType));
    }

    if (!strncmp(key, "Content-Length", strlen("Content-Length")) ||
        !strncmp(key, "content-length", strlen("content-length"))) {
        char* token = strtok_s(nullptr, ":", &next);
        FALSE_RETURN_V(token != nullptr, size * nitems);
        char* contLen = StringTrim(token);
        info->contentLen = atol(contLen);
    }

    if (!strncmp(key, "Transfer-Encoding", strlen("Transfer-Encoding")) ||
        !strncmp(key, "transfer-encoding", strlen("transfer-encoding"))) {
        char* token = strtok_s(nullptr, ":", &next);
        FALSE_RETURN_V(token != nullptr, size * nitems);
        char* transEncode = StringTrim(token);
        if (!strncmp(transEncode, "chunked", strlen("chunked"))) {
            info->isChunked = true;
        }
    }

    if (!strncmp(key, "Content-Range", strlen("Content-Range")) ||
        !strncmp(key, "content-range", strlen("content-range"))) {
        char* token = strtok_s(nullptr, ":", &next);
        FALSE_RETURN_V(token != nullptr, size * nitems);
        char* strRange = StringTrim(token);
        size_t start, end, fileLen;
        FALSE_LOG_MSG(sscanf_s(strRange, "bytes %ld-%ld/%ld", &start, &end, &fileLen) != -1,
            "sscanf get range failed");
        if (info->fileContentLen > 0 && info->fileContentLen != fileLen) {
            MEDIA_LOG_E("FileContentLen doesn't equal to fileLen");
        }
        if (info->fileContentLen == 0) {
            info->fileContentLen = fileLen;
        }
    }
    mediaDownloader->currentRequest_->SaveHeader(info);
    return size * nitems;
}
}
}
}
}