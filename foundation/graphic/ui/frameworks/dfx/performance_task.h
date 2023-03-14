/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_LITE_PERFORMANCE_TASK_H
#define GRAPHIC_LITE_PERFORMANCE_TASK_H

#include "graphic_config.h"
#if ENABLE_DEBUG && ENABLE_DEBUG_PERFORMANCE_TRACE
#include "common/task.h"
#include "graphic_performance.h"

namespace OHOS {
class PerformanceTask : public Task {
public:
    static PerformanceTask* GetInstance();

    void Init() override
    {
        Performance::GetInstance()->RegisterSIGUSR1();
        Task::Init();
    }

    void Callback() override
    {
        Performance::GetInstance()->PrintResult();
    }
private:
    PerformanceTask() {}
    virtual ~PerformanceTask() {}
    PerformanceTask(const PerformanceTask&) = delete;
    PerformanceTask& operator=(const PerformanceTask&) = delete;
    PerformanceTask(PerformanceTask&&) = delete;
    PerformanceTask& operator=(PerformanceTask&&) = delete;
};
}
#define DEBUG_PERFORMANCE_TASK_INIT() OHOS::PerformanceTask::GetInstance()->Init()
#else
#define DEBUG_PERFORMANCE_TASK_INIT()
#endif
#endif