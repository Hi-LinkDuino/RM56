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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_ACE_EVENT_HANDLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_ACE_EVENT_HANDLER_H

#include <memory>

#include "base/memory/ace_type.h"
#include "core/common/container_scope.h"
#include "core/event/ace_events.h"
#include "core/event/key_event.h"
#include "core/event/rotation_event.h"

namespace OHOS::Ace {

class GestureEvent;

class EventMarker final {
public:
    using Function = std::function<void()>;
    using StrFunction = std::function<void(const std::string& info)>;
    using ArgFunction = std::function<void(BaseEventInfo* info)>;

    struct Data final {
        Data() : instanceId(ContainerScope::CurrentId()) {}
        explicit Data(const Data& other) = default;
        explicit Data(Data&& other) = default;
        explicit Data(Function&& preFunc) : preFunction(std::move(preFunc)), instanceId(ContainerScope::CurrentId()) {}
        explicit Data(StrFunction&& func) : uiStrFunction(std::move(func)), instanceId(ContainerScope::CurrentId()) {}
        Data(const std::string& eventId, const std::string& eventType, int32_t pageId, bool isFront)
            : eventId(eventId), eventType(eventType), pageId(pageId), isFront(isFront),
              instanceId(ContainerScope::CurrentId())
        {}
        Data(Function&& func, const std::string& eventType, int32_t pageId, bool isFront)
            : uiFunction(std::move(func)), eventId("-1"), eventType(eventType), pageId(pageId), isFront(isFront),
              isDeclarativeUi(true), instanceId(ContainerScope::CurrentId())
        {}
        Data(ArgFunction&& func, const std::string& eventType, int32_t pageId, bool isFront)
            : uiArgFunction(std::move(func)), eventId("-1"), eventType(eventType), pageId(pageId), isFront(isFront),
              isDeclarativeUi(true), instanceId(ContainerScope::CurrentId())
        {}
        Data(StrFunction&& func, const std::string& eventType, int32_t pageId, bool isFront)
            : uiStrFunction(std::move(func)), eventId("-1"), eventType(eventType), pageId(pageId), isFront(isFront),
              isDeclarativeUi(true), instanceId(ContainerScope::CurrentId())
        {}

        ~Data() = default;

        std::string GetEventParam() const
        {
            return std::string("\"").append(eventType).append("\",");
        }

        Function preFunction;
        Function uiFunction;
        ArgFunction uiArgFunction;
        StrFunction uiStrFunction;
        std::string eventId;
        std::string eventType;
        int32_t pageId = -1;
        bool isFront = true;
        bool isDeclarativeUi = false;
        bool isCatchMode = true;
        int32_t instanceId = -1;
    };

    EventMarker() = default;
    explicit EventMarker(const std::string& eventId, const std::string& eventType = std::string(), int32_t pageId = -1,
        bool isFront = true)
        : data_(std::make_unique<Data>(eventId, eventType, pageId, isFront))
    {}

    EventMarker(EventMarker&& other) = default;
    explicit EventMarker(
        Function&& func, const std::string& eventType = std::string(), int32_t pageId = -1, bool isFront = true)
        : data_(std::make_unique<Data>(std::move(func), eventType, pageId, isFront))
    {}

    explicit EventMarker(
        ArgFunction&& func, const std::string& eventType = std::string(), int32_t pageId = -1, bool isFront = true)
        : data_(std::make_unique<Data>(std::move(func), eventType, pageId, isFront))
    {}
    explicit EventMarker(
        StrFunction&& func, const std::string& eventType = std::string(), int32_t pageId = -1, bool isFront = true)
        : data_(std::make_unique<Data>(std::move(func), eventType, pageId, isFront))
    {}

    EventMarker(const EventMarker& other)
    {
        if (other.data_) {
            ContainerScope id(other.data_->instanceId);
            data_ = std::make_unique<Data>(*other.data_);
        }
    }
    ~EventMarker() = default;

    EventMarker& operator=(EventMarker&& other) = default;
    EventMarker& operator=(const EventMarker& other)
    {
        data_ = other.data_ ? std::make_unique<Data>(*other.data_) : nullptr;
        return *this;
    }

    void Reset()
    {
        data_.release();
    }

    bool operator!=(const std::string& markerEventId) const
    {
        return GetData().eventId != markerEventId;
    }

    bool operator==(const std::string& markerEventId) const
    {
        return GetData().eventId == markerEventId;
    }

    bool IsEmpty() const
    {
        return !data_ || data_->eventId.empty();
    }

    void SetPreFunction(Function&& preFunc)
    {
        if (data_) {
            data_->preFunction = std::move(preFunc);
        } else {
            data_ = std::make_unique<Data>(std::move(preFunc));
        }
    }

    void SetUiStrFunction(StrFunction&& uiStrFunction)
    {
        if (data_) {
            data_->uiStrFunction = std::move(uiStrFunction);
        } else {
            data_ = std::make_unique<Data>(std::move(uiStrFunction));
        }
    }

    StrFunction GetUiStrFunction() const
    {
        if (data_) {
            return data_->uiStrFunction;
        }
        return nullptr;
    }

    void SetCatchMode(bool isCatchMode)
    {
        if (data_) {
            data_->isCatchMode = isCatchMode;
        }
    }

    bool GetCatchMode() const
    {
        return !data_ || data_->isCatchMode;
    }

    void CallPreFunction() const
    {
        if (data_ && data_->preFunction) {
            data_->preFunction();
        }
    }

    bool HasPreFunction() const
    {
        return data_ && data_->preFunction;
    }

    void CallUiFunction() const
    {
        if (data_ && data_->uiFunction) {
            data_->uiFunction();
        }
    }

    void CallUiArgFunction(BaseEventInfo* info) const
    {
        if (data_ && data_->uiArgFunction) {
            data_->uiArgFunction(info);
        }
    }

    void CallUiStrFunction(const std::string& info) const
    {
        if (data_ && data_->uiStrFunction) {
            data_->uiStrFunction(info);
        }
    }

    bool HasUiStrFunction() const
    {
        return data_ && (data_->uiStrFunction);
    }

    const Data& GetData() const
    {
        if (data_) {
            return *data_;
        }
        static const Data emptyData;
        return emptyData;
    }

private:
    std::unique_ptr<Data> data_;
};

class AceEventHandler : public AceType {
    DECLARE_ACE_TYPE(AceEventHandler, AceType);

public:
    virtual void HandleAsyncEvent(const EventMarker& eventMarker) = 0;
    virtual void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) = 0;
    virtual void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) = 0;
    virtual void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& info) = 0;
    virtual void HandleAsyncEvent(const EventMarker& eventMarker, const GestureEvent& info) {}
    virtual void HandleAsyncEvent(const EventMarker& eventMarker, const RotationEvent& info) {}
    // For json dsl event which has json format param.
    virtual void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) = 0;
    virtual void HandleAsyncEvent(const EventMarker& eventMarker, const std::shared_ptr<BaseEventInfo>& info)
    {
        HandleAsyncEvent(eventMarker, *info);
    }

    virtual void HandleSyncEvent(const EventMarker& eventMarker, bool& result) = 0;
    virtual void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) = 0;
    virtual void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& info, bool& result) = 0;
    // For json dsl event which has json format param and json format result.
    virtual void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) = 0;
    virtual void HandleSyncEvent(const EventMarker& eventMarker, const std::shared_ptr<BaseEventInfo>& info)
    {
        bool result = true;
        HandleSyncEvent(eventMarker, *info, result);
    }
    virtual void HandleSyncEvent(
        const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_ACE_EVENT_HANDLER_H
