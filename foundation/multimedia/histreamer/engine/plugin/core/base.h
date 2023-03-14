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

#ifndef HISTREAMER_PLUGIN_CORE_BASE_H
#define HISTREAMER_PLUGIN_CORE_BASE_H

#include <atomic>
#include <memory>

#include "common/plugin_types.h"
#include "common/plugin_tags.h"
#include "common/plugin_buffer.h"
#include "foundation/osal/thread/mutex.h"
#include "interface/plugin_base.h"

namespace OHOS {
namespace Media {
namespace Plugin {
struct PluginBase;

struct CallbackWrap {
    virtual ~CallbackWrap() = default;

    virtual void OnEvent(const PluginEvent &event) = 0;
};

class CallbackImpl : public Plugin::Callback {
public:
    void OnEvent(const PluginEvent &event) override
    {
        callbackWrap_->OnEvent(event);
    }

    void SetCallbackWrap(CallbackWrap* callbackWrap)
    {
        callbackWrap_ = callbackWrap;
    }

private:
    CallbackWrap* callbackWrap_;
};

class Base {
public:
    Base(const Base &) = delete;

    Base operator=(const Base &) = delete;

    virtual ~Base() = default;

    virtual Status Init();

    virtual Status Deinit();

    virtual Status Prepare();

    virtual Status Reset();

    virtual Status Start();

    virtual Status Stop();

    virtual Status GetParameter(Tag tag, ValueType &value);

    virtual Status SetParameter(Tag tag, const ValueType &value);

    virtual Status GetState(State &state);

    virtual std::shared_ptr<Allocator> GetAllocator();

    virtual Status SetCallback(CallbackWrap* cb);

    std::string GetName();

protected:
    friend class PluginManager;

    Base(uint32_t pkgVer, uint32_t apiVer, std::shared_ptr<PluginBase> plugin);

protected:
    const uint32_t pkgVersion_;

    const uint32_t apiVersion_;

    std::shared_ptr<PluginBase> plugin_;

    OHOS::Media::OSAL::Mutex stateChangeMutex_ {};
    std::atomic<State> pluginState_ {State::CREATED};
private:
    CallbackImpl pluginCallback_;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_CORE_BASE_H
