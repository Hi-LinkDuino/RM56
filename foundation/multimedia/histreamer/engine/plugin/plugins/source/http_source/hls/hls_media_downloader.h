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
 
#ifndef HISTREAMER_HLS_MEDIA_DOWNLOADER_H
#define HISTREAMER_HLS_MEDIA_DOWNLOADER_H

#include "playlist_downloader.h"
#include "ring_buffer.h"
#include "plugin/plugins/source/http_source/media_downloader.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
class HlsMediaDownloader : public MediaDownloader, public PlayListChangeCallback {
public:
    HlsMediaDownloader() noexcept;
    ~HlsMediaDownloader() override = default;
    bool Open(const std::string& url) override;
    void Close() override;
    void Pause() override;
    void Resume() override;
    bool Read(unsigned char* buff, unsigned int wantReadLength, unsigned int& realReadLength, bool& isEos) override;
    bool Seek(int offset) override;

    size_t GetContentLength() const override;
    double GetDuration() const override;
    Seekable GetSeekable() const override;
    void SetCallback(Callback* cb) override;
    void OnPlayListChanged(const std::vector<std::string>& playList) override;
    void SetStatusCallback(StatusCallbackFunc cb) override;

private:
    void SaveData(uint8_t* data, uint32_t len, int64_t offset);
    void FragmentDownloadLoop();

private:
    std::shared_ptr<RingBuffer> buffer_;
    std::shared_ptr<Downloader> downloader_;
    std::shared_ptr<DownloadRequest> downloadRequest_;

    Callback* callback_ {nullptr};
    DataSaveFunc dataSave_;
    StatusCallbackFunc statusCallback_;

    std::shared_ptr<PlayListDownloader> playListDownloader_;

    std::shared_ptr<OSAL::Task> downloadTask_;
    std::shared_ptr<BlockingQueue<std::string>> playList_;
    std::map<std::string, bool> fragmentDownloadStart;
};
}
}
}
}
#endif