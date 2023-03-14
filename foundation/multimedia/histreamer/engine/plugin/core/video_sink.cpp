/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "PluginCoreViSink"

#include "video_sink.h"

#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "interface/video_sink_plugin.h"
#include "plugin_core_utils.h"

using namespace OHOS::Media::Plugin;

VideoSink::VideoSink(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<VideoSinkPlugin> plugin)
    : Base(pkgVer, apiVer, plugin), videoSink(std::move(plugin))
{
}

Status VideoSink::Pause()
{
    MEDIA_LOG_I(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    if (pluginState_ != State::RUNNING) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " pause in status " PUBLIC_LOG_S ", ignore pause",
                    plugin_->GetName().c_str(), GetStateString(pluginState_.load()));
        return Status::OK;
    }
    auto ret = videoSink->Pause();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK) {
        pluginState_ = State::PAUSED;
    }
    MEDIA_LOG_I(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status VideoSink::Resume()
{
    MEDIA_LOG_I(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    if (pluginState_ != State::PAUSED) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " resume in status " PUBLIC_LOG_S ", ignore pause",
                    plugin_->GetName().c_str(), GetStateString(pluginState_.load()));
        return Status::OK;
    }
    auto ret = videoSink->Resume();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK) {
        pluginState_ = State::RUNNING;
    }
    MEDIA_LOG_I(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status VideoSink::Flush()
{
    return videoSink->Flush();
}

Status VideoSink::Write(const std::shared_ptr<Buffer>& input)
{
    return videoSink->Write(input);
}

Status VideoSink::GetLatency(uint64_t& hstTime)
{
    return videoSink->GetLatency(hstTime);
}