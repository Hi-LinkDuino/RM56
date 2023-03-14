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

#ifndef GRAPHIC_LITE_TASK_THREAD_H
#define GRAPHIC_LITE_TASK_THREAD_H

#include <QThread>
#include <windows.h>
#include "common/task_manager.h"

namespace OHOS {
class TaskThread : public QThread {
public:
    TaskThread() {}
    virtual ~TaskThread() {}
    void run() override;
    void Quit();

private:
    volatile bool taskQuitQry = false;
};
} // namespace OHOS

#endif // GRAPHIC_LITE_TASK_THREAD_H
