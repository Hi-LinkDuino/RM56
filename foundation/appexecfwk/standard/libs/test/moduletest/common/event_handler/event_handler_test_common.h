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

#ifndef FOUNDATION_APPEXECFWK_LIBS_TEST_MODULETEST_COMMON_EVENT_HANDLER_EVENT_HANDLER_TEST_COMMON_H
#define FOUNDATION_APPEXECFWK_LIBS_TEST_MODULETEST_COMMON_EVENT_HANDLER_EVENT_HANDLER_TEST_COMMON_H

#include <atomic>

#include "nocopyable.h"

#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace AppExecFwk {
const uint32_t STOP_EVENT_ID = 0;
const uint32_t RUN_EVENT_ID = 10;

/**
 * Function: Create random number for uint32_t type.
 */
inline uint32_t Random()
{
    return static_cast<uint32_t>(std::rand());
}

enum class SmartPointerType {
    SHARED_PTR = 0,
    WEAK_PTR,
    LVALUE_REFERENCE_UNIQUE_PTR,
    RVALUE_REFERENCE_UNIQUE_PTR,
};

class CommonUtils {
public:
    CommonUtils() = delete;
    ~CommonUtils() = delete;
    DISALLOW_COPY_AND_MOVE(CommonUtils);

    /**
     * Function: Get the processed result of event.
     * @return Returns true if event has been processed.
     */
    static inline bool EventRunGet()
    {
        return eventRun_.load();
    }

    /**
     * Function: Set the processed result of event.
     * @param run Set false or true for processed result.
     */
    static inline void EventRunSet(bool run)
    {
        eventRun_.store(run);
    }

    /**
     * Function: Get the called result of task.
     * @return Returns true if task has been called.
     */
    static inline bool TaskCalledGet()
    {
        return taskCalled_.load();
    }

    /**
     * Function: Set the called result of task.
     * @param run Set false or true for called result.
     */
    static inline void TaskCalledSet(bool called)
    {
        taskCalled_.store(called);
    }

    /**
     * Function: Get the processed times of event.
     * @return Returns processed times.
     */
    static inline uint32_t EventRunCount()
    {
        return eventRunTimes_;
    }

    /**
     * Function: Increment of event processed times.
     */
    static inline void EventRunCountIncrement()
    {
        eventRunTimes_++;
    }

    /**
     * Function: Reset the count of event processed to 0.
     */
    static inline void EventRunCountReset()
    {
        eventRunTimes_ = 0;
    }

private:
    static std::atomic<bool> eventRun_;
    static std::atomic<bool> taskCalled_;
    static std::atomic<uint32_t> eventRunTimes_;
};

class MyEventHandler : public EventHandler {
public:
    explicit MyEventHandler(const std::shared_ptr<EventRunner> &runner);
    ~MyEventHandler();

    /**
     * Function: Process the event. Override the method of base class.
     * @param event The event need to be processed.
     */
    void ProcessEvent(const InnerEvent::Pointer &event) override;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // #ifndef FOUNDATION_APPEXECFWK_LIBS_TEST_MODULETEST_COMMON_EVENT_HANDLER_EVENT_HANDLER_TEST_COMMON_H