/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef EVENTLOGGER_LOG_CATCHER_UTILS_H
#define EVENTLOGGER_LOG_CATCHER_UTILS_H
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
namespace OHOS {
namespace HiviewDFX {
namespace LogCatcherUtils {
int DumpStacktrace(int fd, int pid);
bool ReadCPUInfo(int fd, int pid);
bool ReadMemoryInfo(int fd, int pid);
}
} // namespace HiviewDFX
} // namespace OHOS
#endif // EVENTLOGGER_STACK_UTILS_H