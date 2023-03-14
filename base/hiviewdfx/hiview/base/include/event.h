/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef HIVIEW_BASE_EVENT_H
#define HIVIEW_BASE_EVENT_H
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string>
#include <sys/types.h>
#include "defines.h"
#include "public_defines.h"

namespace OHOS {
namespace HiviewDFX {
class DllExport Event : public std::enable_shared_from_this<Event> {
public:
    explicit Event(const std::string &sender)
        : messageType_(MessageType::NONE),
          processType_(ManageType::UNORDERED),
          what_(0),
          eventId_(0),
          happenTime_(0),
          targetDispatchTime_(0),
          createTime_(0),
          sender_(sender),
          domain_(""),
          eventName_(""),
          isPipeline_(false),
          hasFinish_(false),
          hasPending_(false),
          traceId_(""),
          spanId_(""),
          parentSpanId_(""),
          traceFlag_(""),
          normalExtraInfo_(""),
          jsonExtraInfo_("")
    {
        ResetTimestamp();
    };

    explicit Event(const std::string &sender, const std::string &name)
        : messageType_(MessageType::NONE),
          processType_(ManageType::UNORDERED),
          what_(0),
          eventId_(0),
          happenTime_(0),
          targetDispatchTime_(0),
          createTime_(0),
          sender_(sender),
          domain_(""),
          eventName_(name),
          isPipeline_(false),
          hasFinish_(false),
          hasPending_(false),
          traceId_(""),
          spanId_(""),
          parentSpanId_(""),
          traceFlag_(""),
          normalExtraInfo_(""),
          jsonExtraInfo_("")
    {
        ResetTimestamp();
    };

    virtual ~Event() {};

    virtual bool OnContinue()
    {
        return true;
    };

    virtual bool OnFinish()
    {
        hasFinish_ = true;
        return true;
    };

    virtual void OnRepack() {};

    virtual void OnPending() {};

    virtual uint32_t GetPendingProcessorSize()
    {
        return 0;
    }

    // call from audit module
    // if you want to override this function
    // you should call parent function first and append to your result
    virtual std::string GetEventInfo()
    {
        return std::to_string(eventId_);
    };

    // use for broadcasting events
    enum MessageType {
        NONE = 0,
        PLUGIN_MAINTENANCE, // Reserved
        FAULT_EVENT,
        STATISTICS_EVENT,
        RAW_EVENT,
        SYS_EVENT,
        UE_EVENT,
        EXTERNAL_EVENT,
        EXTERNAL_REMOTE_EVENT,
        CROSS_PLATFORM,
        PRIVATE_MESSAGE_TYPE // Expand macro from public_defines.h
    };

    enum ManageType {
        ORDERED,
        UNORDERED,
        PROCESS_TYPE_NUM,
    };

    enum EventId {
        PLUGIN_LOADED,
    };

    MessageType messageType_;
    ManageType processType_;
    uint16_t what_;
    uint32_t eventId_;
    uint64_t happenTime_;
    uint64_t targetDispatchTime_;
    uint64_t createTime_;
    std::string sender_;
    std::string domain_;
    std::string eventName_;
    bool isPipeline_;
    bool hasFinish_;
    bool hasPending_;
    // dft fault listener params
    std::string traceId_;
    std::string spanId_;
    std::string parentSpanId_;
    std::string traceFlag_;
    std::string normalExtraInfo_;
    std::string jsonExtraInfo_;
    void SetValue(const std::string &name, const std::string &value);
    void SetValue(const std::string &name, int32_t value);
    void SetKeyValuePairs(std::map<std::string, std::string> keyValuePairs);
    const std::string GetValue(const std::string &name) const;
    int32_t GetIntValue(const std::string &name) const;
    std::map<std::string, std::string> GetKeyValuePairs() const;
    void ResetTimestamp();
    void ResetPendingStatus()
    {
        hasPending_ = false;
    };

    bool IsPipelineEvent() const
    {
        return isPipeline_;
    };

    bool HasFinish() const
    {
        return hasFinish_;
    };

    bool HasPending() const
    {
        return hasPending_;
    };

    std::string GetEventName() const
    {
        return eventName_;
    }

    void SetEventName(const std::string &name)
    {
        eventName_ = name;
    }

    template <typename Derived>
    static std::shared_ptr<Derived> DownCastTo(std::shared_ptr<Event> event)
    {
        return std::static_pointer_cast<Derived>(event);
    };

    // Ensure the copy constructor of Base and Derived classes are carefully arranged
    template <typename Base, typename Derived>
    static std::shared_ptr<Derived> Repack(std::shared_ptr<Event>& event, bool replace = true)
    {
        auto base = std::static_pointer_cast<Base>(event);
        if (replace) {
            auto derived = new Derived(*(base.get()));
            event.reset(derived);
            return std::static_pointer_cast<Derived>(event);
        }

        auto newEvent = std::make_shared<Derived>(*(base.get()));
        newEvent->OnRepack();
        return newEvent;
    };

protected:
    std::map<std::string, std::string> bundle_;
};
class DllExport EventHandler {
public:
    virtual ~EventHandler(){};
    virtual bool OnEvent(std::shared_ptr<Event>& event) = 0;
    virtual bool OnEventProxy(std::shared_ptr<Event> event)
    {
        return OnEvent(event);
    };

    virtual bool CanProcessEvent(std::shared_ptr<Event> event __UNUSED)
    {
        return true;
    };

    virtual bool CanProcessMoreEvents()
    {
        return true;
    };
    
    virtual std::string GetHandlerInfo()
    {
        return "";
    };
};
class EventListener {
public:
    struct EventIdRange {
        uint32_t begin;
        uint32_t end;
        EventIdRange(uint32_t id)
        {
            begin = id;
            end = id;
        };

        EventIdRange(uint32_t begin, uint32_t end)
        {
            this->begin = begin;
            this->end = end;
        };

        bool operator<(const EventIdRange &range) const
        {
            return (end < range.begin);
        };

        bool operator==(const EventIdRange &range) const
        {
            return ((begin == range.begin) && (end == range.end));
        };
    };

    EventListener() {};
    virtual ~EventListener(){};

    virtual bool OnOrderedEvent(const Event &msg)
    {
        return false;
    }
    virtual void OnUnorderedEvent(const Event &msg) = 0;
    virtual std::string GetListenerName() = 0;

    // Make sure that you insert non-overlayed range
    void AddListenerInfo(uint32_t type, const EventListener::EventIdRange &range = EventListener::EventIdRange(0));
    void AddListenerInfo(uint32_t type, const std::set<EventListener::EventIdRange> &listenerInfo);
    bool GetListenerInfo(uint32_t type, std::set<EventListener::EventIdRange> &listenerInfo);

    void AddListenerInfo(uint32_t type, const std::string& eventName);
    void AddListenerInfo(uint32_t type, const std::set<std::string> &eventNames);
    bool GetListenerInfo(uint32_t type, std::set<std::string> &eventNames);
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_EVENT_H
