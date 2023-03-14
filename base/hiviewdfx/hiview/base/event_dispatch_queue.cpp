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
#include "event_dispatch_queue.h"

#include <algorithm>
#include <memory>

#include "file_util.h"
#include "logger.h"
#include "plugin.h"
#include "thread_util.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("HiView-EventDispatchQueue");
EventDispatchQueue::EventDispatchQueue(const std::string& name, Event::ManageType type, HiviewContext* context)
    : stop_(false), isRunning_(false), threadName_(name), type_(type), context_(context)
{}

EventDispatchQueue::~EventDispatchQueue()
{
    Stop();
}

void EventDispatchQueue::Run()
{
    const int threadNameLen = 15;
    Thread::SetThreadDescription(threadName_.substr(0, threadNameLen));
    isRunning_ = true;
    while (true) {
        std::shared_ptr<Event> event = nullptr;
        {
            std::unique_lock<std::mutex> lock(mutexLock_);
            while (pendingEvents_.empty()) {
                condition_.wait(lock);
                if (stop_) {
                    return;
                }
            }
            event = pendingEvents_.front();
            pendingEvents_.pop_front();
        }

        if (event == nullptr) {
            continue;
        }

        if (type_ == Event::ManageType::UNORDERED) {
            ProcessUnorderedEvent(*(event.get()));
        }

        if (stop_) {
            break;
        }
    }
}

void EventDispatchQueue::ProcessUnorderedEvent(const Event& event)
{
    auto eventName = event.domain_ + "_" + event.eventName_;
    auto listeners = context_->GetListenerInfo(event.messageType_, eventName, event.eventId_);
    for (auto& tmp : listeners) {
        auto listener = tmp.lock();
        if (listener == nullptr) {
            continue;
        }
        if (listener->isPlugin) {
            auto ptr = listener->plugin.lock();
            if (ptr != nullptr) {
                ptr->OnEventListeningCallback(event);
            }
        } else {
            auto ptr = listener->listener.lock();
            if (ptr != nullptr) {
                ptr->OnUnorderedEvent(event);
            }
        }
    }
}

void EventDispatchQueue::Stop()
{
    stop_ = true;
    condition_.notify_all();
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }
    isRunning_ = false;
}

void EventDispatchQueue::Start()
{
    std::unique_lock<std::mutex> lock(mutexLock_);
    if (!IsRunning()) {
        thread_ = std::make_unique<std::thread>(&EventDispatchQueue::Run, this);
    }
}

void EventDispatchQueue::Enqueue(std::shared_ptr<Event> event)
{
    HIVIEW_LOGD("EventDispatchQueue Enqueue");
    std::unique_lock<std::mutex> lock(mutexLock_);
    pendingEvents_.push_back(std::move(event));
    condition_.notify_one();
}

int EventDispatchQueue::GetWaitQueueSize() const
{
    return pendingEvents_.size();
}
} // namespace HiviewDFX
} // namespace OHOS