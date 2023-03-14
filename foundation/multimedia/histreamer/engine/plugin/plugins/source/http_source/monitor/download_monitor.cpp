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
#define HST_LOG_TAG "DownloadMonitor"

#include "download_monitor.h"
#include "foundation/cpp_ext/algorithm_ext.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
namespace {
    constexpr int RETRY_TIMES_TO_REPORT_ERROR = 10;
}
DownloadMonitor::DownloadMonitor(std::shared_ptr<MediaDownloader> downloader) noexcept
    : downloader_(std::move(downloader))
{
    auto statusCallback = [this] (DownloadStatus&& status, std::shared_ptr<Downloader>& downloader,
        std::shared_ptr<DownloadRequest>& request) {
        OnDownloadStatus(std::forward<decltype(downloader)>(downloader), std::forward<decltype(request)>(request));
    };
    downloader_->SetStatusCallback(statusCallback);
    task_ = std::make_shared<OSAL::Task>(std::string("HttpMonitor"));
    task_->RegisterHandler([this] { HttpMonitorLoop(); });
    task_->Start();
}

void DownloadMonitor::HttpMonitorLoop()
{
    if (isPlaying_) {
        time_t nowTime;
        time(&nowTime);
        if ((lastReadTime_ != 0) && (nowTime - lastReadTime_ >= 60)) {  // 60
            MEDIA_LOG_D("HttpMonitorLoop : too long without reading data, paused");
            Pause();
        }
    }
    {
        OSAL::ScopedLock lock(taskMutex_);
        if (!retryTasks_.empty()) {
            RetryRequest task = retryTasks_.front();
            task.function();
            retryTasks_.pop_front();
        }
    }
    OSAL::SleepFor(50); // 50
}

bool DownloadMonitor::Open(const std::string& url)
{
    isPlaying_ = true;
    retryTasks_.clear();
    return downloader_->Open(url);
}

void DownloadMonitor::Pause()
{
    downloader_->Pause();
    isPlaying_ = false;
}

void DownloadMonitor::Resume()
{
    downloader_->Resume();
    isPlaying_ = true;
}

void DownloadMonitor::Close()
{
    retryTasks_.clear();
    downloader_->Close();
    task_->Stop();
    isPlaying_ = false;
}

bool DownloadMonitor::Read(unsigned char *buff, unsigned int wantReadLength,
                           unsigned int &realReadLength, bool &isEos)
{
    bool ret = downloader_->Read(buff, wantReadLength, realReadLength, isEos);
    time(&lastReadTime_);
    if (!isPlaying_) {
        Resume();
    }
    return ret;
}

bool DownloadMonitor::Seek(int offset)
{
    isPlaying_ = true;
    retryTasks_.clear();
    return downloader_->Seek(offset);
}

size_t DownloadMonitor::GetContentLength() const
{
    return downloader_->GetContentLength();
}

double DownloadMonitor::GetDuration() const
{
    return downloader_->GetDuration();
}

Seekable DownloadMonitor::GetSeekable() const
{
    return downloader_->GetSeekable();
}

void DownloadMonitor::SetCallback(Callback* cb)
{
    callback_ = cb;
    downloader_->SetCallback(cb);
}

void DownloadMonitor::SetStatusCallback(StatusCallbackFunc cb)
{
}

bool DownloadMonitor::NeedRetry(const std::shared_ptr<DownloadRequest>& request)
{
    auto clientError = request->GetClientError();
    auto serverError = request->GetServerError();
    auto retryTimes = request->GetRetryTimes();
    if ((clientError != NetworkClientErrorCode::ERROR_OK && clientError != NetworkClientErrorCode::ERROR_NOT_RETRY)
        || serverError != 0) {
        MEDIA_LOG_I("NeedRetry: clientError = " PUBLIC_LOG_D32 ", serverError = " PUBLIC_LOG_D32
            ", retryTimes = " PUBLIC_LOG_D32, clientError, serverError, retryTimes);
        if (retryTimes > RETRY_TIMES_TO_REPORT_ERROR) { // Report error to upper layer
            if (clientError != NetworkClientErrorCode::ERROR_OK) {
                MEDIA_LOG_I("Send http client error, code " PUBLIC_LOG_D32, static_cast<int32_t>(clientError));
                callback_->OnEvent({PluginEventType::CLIENT_ERROR, {clientError}, "http"});
            }
            if (serverError != 0) {
                MEDIA_LOG_I("Send http server error, code " PUBLIC_LOG_D32, serverError);
                callback_->OnEvent({PluginEventType::SERVER_ERROR, {serverError}, "http"});
            }
        }
        return true;
    }
    return false;
}

void DownloadMonitor::OnDownloadStatus(std::shared_ptr<Downloader>& downloader,
                                       std::shared_ptr<DownloadRequest>& request)
{
    FALSE_RETURN_MSG(downloader != nullptr, "downloader is null, url is " PUBLIC_LOG_S, request->GetUrl().c_str());
    if (NeedRetry(request)) {
        OSAL::ScopedLock lock(taskMutex_);
        bool exists = CppExt::AnyOf(retryTasks_.begin(), retryTasks_.end(), [&](const RetryRequest& item) {
            return item.request->IsSame(request);
        });
        if (!exists) {
            RetryRequest retryRequest {request, [downloader, request] { downloader->Retry(request); }};
            retryTasks_.emplace_back(std::move(retryRequest));
        }
    }
}
}
}
}
}