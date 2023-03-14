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
#define HST_LOG_TAG "HlsPlayListDownloader"
#include <mutex>
#include "hls_playlist_downloader.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace HttpPlugin {
void HlsPlayListDownloader::PlayListUpdateLoop()
{
    OSAL::SleepFor(5000); // 5000 how often is playlist updated
    UpdateManifest();
}

// StateMachine thread: call plugin SetSource -> call Open
// StateMachine thread: call plugin GetSeekable -> call GetSeekable
// PlayListDownload thread: call ParseManifest
// First call Open, then start PlayListDownload thread, it seems no lock is required.
// [In future] StateMachine thread: call plugin GetDuration -> call GetDuration
void HlsPlayListDownloader::Open(std::string url)
{
    url_ = url;
    master_ = nullptr;
    DoOpen(url);
}

void HlsPlayListDownloader::UpdateManifest()
{
    if (currentVariant_ && currentVariant_->m3u8_ && !currentVariant_->m3u8_->uri_.empty()) {
        DoOpen(currentVariant_->m3u8_->uri_);
    } else {
        MEDIA_LOG_E("UpdateManifest currentVariant_ not ready.");
    }
}

void HlsPlayListDownloader::SetPlayListCallback(PlayListChangeCallback* callback)
{
    callback_ = callback;
}

double HlsPlayListDownloader::GetDuration() const
{
    if (!master_) {
        return 0;
    }
    return master_->bLive_ ? -1.0 : master_->duration_; // -1.0
}

Seekable HlsPlayListDownloader::GetSeekable() const
{
    if (!master_) {
        return Seekable::INVALID;
    }
    return master_->bLive_ ? Seekable::UNSEEKABLE : Seekable::SEEKABLE;
}

void HlsPlayListDownloader::ParseManifest()
{
    if (!master_) {
        master_ = std::make_shared<M3U8MasterPlaylist>(playList_, url_);
        currentVariant_ = master_->defaultVariant_;
        if (!master_->isSimple_) {
            UpdateManifest();
        }
        updateTask_->Start();
    } else {
        currentVariant_->m3u8_->Update(playList_);
        auto files = currentVariant_->m3u8_->files_;
        auto playList = std::vector<std::string>();
        playList.reserve(files.size());
        for (auto &file: files) {
            playList.push_back(file->uri_);
        }
        callback_->OnPlayListChanged(playList);
    }
}
}
}
}
}