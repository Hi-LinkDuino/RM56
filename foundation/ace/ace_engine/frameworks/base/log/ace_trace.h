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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_TRACE_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_TRACE_H

#include <cstdarg>
#include <cstdio>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

#define ACE_SCOPED_TRACE(fmt, ...) AceScopedTrace aceScopedTrace(fmt, ##__VA_ARGS__)
#ifdef ACE_DEBUG
#define ACE_DEBUG_SCOPED_TRACE(fmt, ...) AceScopedTrace aceScopedTrace(fmt, ##__VA_ARGS__)
#else
#define ACE_DEBUG_SCOPED_TRACE(fmt, ...)
#endif

#define ACE_FUNCTION_TRACE() ACE_SCOPED_TRACE(__func__)

namespace OHOS::Ace {

bool ACE_EXPORT AceTraceEnabled();
void ACE_EXPORT AceTraceBegin(const char* name);
bool ACE_EXPORT AceTraceBeginWithArgs(const char* format, ...) __attribute__((__format__(printf, 1, 2)));
bool ACE_EXPORT AceTraceBeginWithArgv(const char* format, va_list args);
void ACE_EXPORT AceTraceEnd();

class ACE_EXPORT AceScopedTrace final {
public:
    explicit AceScopedTrace(const char* format, ...) __attribute__((__format__(printf, 2, 3)));
    ~AceScopedTrace();

    ACE_DISALLOW_COPY_AND_MOVE(AceScopedTrace);

private:
    bool traceEnabled_ { false };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_TRACE_H
