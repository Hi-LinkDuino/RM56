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

#ifndef HILOG_WRAPPER_H
#define HILOG_WRAPPER_H

#define CONFIG_HILOG
#ifdef CONFIG_HILOG

#include "hilog/log.h"

#ifdef HILOG_FATAL
#undef HILOG_FATAL
#endif

#ifdef HILOG_ERROR
#undef HILOG_ERROR
#endif

#ifdef HILOG_WARN
#undef HILOG_WARN
#endif

#ifdef HILOG_INFO
#undef HILOG_INFO
#endif

#ifdef HILOG_DEBUG
#undef HILOG_DEBUG
#endif

namespace OHOS {
namespace Global {
namespace Resource {
extern LogLevel g_logLevel;
} // namespace Resource
} // namespace Global
} // namespace OHOS

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0, "ResourceManager"};

#define HILOG_FATAL(...) (void)OHOS::HiviewDFX::HiLog::Fatal(LOG_LABEL, __VA_ARGS__)
#define HILOG_ERROR(...) (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, __VA_ARGS__)
#define HILOG_WARN(...) {                                            \
if (OHOS::Global::Resource::g_logLevel <= LOG_WARN) {                \
    (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, __VA_ARGS__);      \
}                                                                    \
}

#define HILOG_INFO(...) {                                            \
if (OHOS::Global::Resource::g_logLevel <= LOG_INFO) {                \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, __VA_ARGS__);      \
}                                                                    \
}

#define HILOG_DEBUG(...) {                                           \
if (OHOS::Global::Resource::g_logLevel <= LOG_DEBUG) {               \
    (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, __VA_ARGS__);     \
}                                                                    \
}

#else

#define HILOG_FATAL(...)
#define HILOG_ERROR(...)
#define HILOG_WARN(...)
#define HILOG_INFO(...)
#define HILOG_DEBUG(...)
#endif  // CONFIG_HILOG

#endif  // HILOG_WRAPPER_H