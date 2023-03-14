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

#define HST_LOG_TAG "PluginCoreBase"

#include "base.h"
#include "foundation/log.h"
#include "foundation/osal/thread/scoped_lock.h"
#include "plugin/interface/plugin_base.h"
#include "plugin_core_utils.h"

using namespace OHOS::Media::Plugin;

Base::Base(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<PluginBase> plugin)
    : pkgVersion_(pkgVer), apiVersion_(apiVer), plugin_(std::move(plugin)) {}

Status Base::Init()
{
    MEDIA_LOG_D(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    RETURN_WRONG_STATE_IF_CON_TRUE(pluginState_ == State::DESTROYED, plugin_, pluginState_.load());
    if (pluginState_ != State::CREATED) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " has been already inited", plugin_->GetName().c_str());
        return Status::OK;
    }
    auto ret = plugin_->Init();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK) {
        pluginState_ = State::INITIALIZED;
    }
    MEDIA_LOG_D(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status Base::Deinit()
{
    MEDIA_LOG_D(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    if (pluginState_ == State::DESTROYED) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " already deinited, no need to destroy any more",
                    plugin_->GetName().c_str());
        return Status::OK;
    }
    auto ret = plugin_->Deinit();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    pluginState_ = State::DESTROYED;
    MEDIA_LOG_D(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status Base::Prepare()
{
    MEDIA_LOG_D(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    RETURN_WRONG_STATE_IF_CON_TRUE(pluginState_ != State::PREPARED && pluginState_ != State::INITIALIZED,
        plugin_, pluginState_.load());
    if (pluginState_ == State::PREPARED) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " already prepared, ignore this prepare", plugin_->GetName().c_str());
        return Status::OK;
    }
    auto ret = plugin_->Prepare();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK || ret == Status::ERROR_DELAY_READY) {
        pluginState_ = State::PREPARED;
    }
    MEDIA_LOG_D(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status Base::Reset()
{
    MEDIA_LOG_D(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    RETURN_WRONG_STATE_IF_CON_TRUE(pluginState_ == State::RUNNING || pluginState_ == State::PAUSED ||
        pluginState_ == State::DESTROYED, plugin_, pluginState_.load());
    if (pluginState_ == State::CREATED || pluginState_ == State::INITIALIZED) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " no need to reset in state " PUBLIC_LOG_S,
                    plugin_->GetName().c_str(), GetStateString(pluginState_.load()));
        return Status::OK;
    }
    auto ret = plugin_->Reset();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    pluginState_ = State::INITIALIZED;
    MEDIA_LOG_D(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status Base::Start()
{
    MEDIA_LOG_D(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    RETURN_WRONG_STATE_IF_CON_TRUE(pluginState_ != State::PREPARED && pluginState_ != State::RUNNING, plugin_,
        pluginState_.load());
    if (pluginState_ == State::RUNNING) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " already in running state, ignore start", plugin_->GetName().c_str());
        return Status::OK;
    }
    auto ret = plugin_->Start();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK) {
        pluginState_ = State::RUNNING;
    }
    MEDIA_LOG_D(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status Base::Stop()
{
    MEDIA_LOG_D(PUBLIC_LOG_S " Enter.", __FUNCTION__);
    OSAL::ScopedLock lock(stateChangeMutex_);
    if (pluginState_ != State::RUNNING && pluginState_ != State::PAUSED) {
        MEDIA_LOG_I("plugin " PUBLIC_LOG_S " not running or paused, no need to stop", plugin_->GetName().c_str());
        return Status::OK;
    }
    auto ret = plugin_->Stop();
    LOG_WARN_IF_NOT_OK(plugin_, ret);
    if (ret == Status::OK) {
        pluginState_ = State::PREPARED;
    }
    MEDIA_LOG_D(PUBLIC_LOG_S " Exit.", __FUNCTION__);
    return ret;
}

Status Base::GetParameter(Tag tag, ValueType& value)
{
    return plugin_->GetParameter(tag, value);
}

Status Base::SetParameter(Tag tag, const ValueType& value)
{
    return plugin_->SetParameter(tag, value);
}

Status Base::GetState(State &state)
{
    state = pluginState_;
    return Status::OK;
}

std::shared_ptr<Allocator> Base::GetAllocator()
{
    return plugin_->GetAllocator();
}

Status Base::SetCallback(CallbackWrap* cb)
{
    pluginCallback_.SetCallbackWrap(cb);
    return plugin_->SetCallback(&pluginCallback_);
}

std::string Base::GetName()
{
    return plugin_->GetName();
}