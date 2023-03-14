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

#ifndef HISTREAMER_DOWNLOAD_MONITOR_H
#define HISTREAMER_DOWNLOAD_MONITOR_H

#include <ctime>
#include <list>
#include <memory>
#include <string>
#include "blocking_queue.h"
#include "plugin/interface/plugin_base.h"
#include "plugin/plugins/source/http_source/download/downloader.h"
#include "plugin/plugins/source/http_source/media_downloader.h"
#include "ring_buffer.h"
#include "osal/thread/task.h"
#include "osal/thread/mutex.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
struct RetryRequest {
    std::shared_ptr<DownloadRequest> request;
    std::function<void()> function;
};

class DownloadMonitor : public MediaDownloader {
public:
    explicit DownloadMonitor(std::shared_ptr<MediaDownloader> downloader) noexcept;
    ~DownloadMonitor() override = default;
    bool Open(const std::string& url) override;
    void Close() override;
    bool Read(unsigned char* buff, unsigned int wantReadLength, unsigned int& realReadLength, bool& isEos) override;
    bool Seek(int offset) override;
    void Pause() override;
    void Resume() override;
    size_t GetContentLength() const override;
    double GetDuration() const override;
    Seekable GetSeekable() const override;
    void SetCallback(Callback *cb) override;
    void SetStatusCallback(StatusCallbackFunc cb) override;

private:
    void HttpMonitorLoop();
    void OnDownloadStatus(std::shared_ptr<Downloader>& downloader, std::shared_ptr<DownloadRequest>& request);
    bool NeedRetry(const std::shared_ptr<DownloadRequest>& request);

    std::shared_ptr<MediaDownloader> downloader_;
    std::list<RetryRequest> retryTasks_;
    std::atomic<bool> isPlaying_ {false};
    std::shared_ptr<OSAL::Task> task_;
    time_t lastReadTime_ {0};
    Callback* callback_ {nullptr};
    OSAL::Mutex taskMutex_ {};
};
}
}
}
}
#endif
