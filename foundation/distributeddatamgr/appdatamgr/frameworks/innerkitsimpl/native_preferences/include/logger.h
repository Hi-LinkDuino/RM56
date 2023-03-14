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

#ifndef NATIVE_PREFERENCES_LOGGER_H
#define NATIVE_PREFERENCES_LOGGER_H

#include "hilog/log.h"

namespace OHOS {
namespace NativePreferences {
static const OHOS::HiviewDFX::HiLogLabel DFS_LABEL = { LOG_CORE, 0xD001650, "NativePreferences" };

#define LOG_DEBUG(...) ((void)OHOS::HiviewDFX::HiLog::Debug(DFS_LABEL, __VA_ARGS__))
#define LOG_INFO(...) ((void)OHOS::HiviewDFX::HiLog::Info(DFS_LABEL, __VA_ARGS__))
#define LOG_WARN(...) ((void)OHOS::HiviewDFX::HiLog::Warn(DFS_LABEL, __VA_ARGS__))
#define LOG_ERROR(...) ((void)OHOS::HiviewDFX::HiLog::Error(DFS_LABEL, __VA_ARGS__))
#define LOG_FATAL(...) ((void)OHOS::HiviewDFX::HiLog::Fatal(DFS_LABEL, __VA_ARGS__))
} // namespace NativePreferences
} // namespace OHOS

#endif // NATIVE_PREFERENCES_LOGGER_H