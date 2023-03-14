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

#define HST_LOG_TAG "PluginCoreAuSink"

#include "audio_sink.h"

#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "interface/audio_sink_plugin.h"
#include "plugin_core_utils.h"

using namespace OHOS::Media::Plugin;

AudioSink::AudioSink(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<AudioSinkPlugin> plugin)
    : Base(pkgVer, apiVer, plugin), audioSink(std::move(plugin)) {}

Status AudioSink::Pause()
{
    MEDIA_LOG_I(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    if (pluginState_ != State::RUNNING) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " pause in status " PUBLIC_LOG_S ", ignore pause",
                    plugin_->GetName().c_str(), GetStateString(pluginState_.load()));
        return Status::OK;
    }
    auto ret = audioSink->Pause();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK) {
        pluginState_ = State::PAUSED;
    }
    MEDIA_LOG_I(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status AudioSink::Resume()
{
    MEDIA_LOG_I(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    if (pluginState_ != State::PAUSED) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " resume in status " PUBLIC_LOG_S ", ignore pause",
                    plugin_->GetName().c_str(), GetStateString(pluginState_.load()));
        return Status::OK;
    }
    auto ret = audioSink->Resume();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK) {
        pluginState_ = State::RUNNING;
    }
    MEDIA_LOG_I(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status AudioSink::Flush()
{
    return audioSink->Flush();
}

Status AudioSink::Drain()
{
    return audioSink->Drain();
}

Status AudioSink::Write(const std::shared_ptr<Buffer>& input)
{
    return audioSink->Write(input);
}

Status AudioSink::SetVolume(float volume)
{
    return audioSink->SetVolume(volume);
}

Status AudioSink::GetLatency(uint64_t& hstTime)
{
    return audioSink->GetLatency(hstTime);
}