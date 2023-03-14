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

#ifndef POWERMGR_SUSPEND_OPS_H
#define POWERMGR_SUSPEND_OPS_H

#include <ohos_types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct RunningLockOps {
    void (*Acquire)(const char *name);
    void (*Release)(const char *name);
};

struct AutoSuspendOps {
    void (*Enable)();
    void (*IncSuspendBlockCounter)();
    void (*DecSuspendBlockCounter)();
};

typedef void (*AutoSuspendWait)();
typedef BOOL (*AutoSuspendLoop)(AutoSuspendWait waitFunc);

struct RunningLockOps* RunningLockOpsInit();
struct AutoSuspendOps* AutoSuspendOpsInit();
AutoSuspendLoop AutoSuspendLoopInit();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_SUSPEND_OPS_H
