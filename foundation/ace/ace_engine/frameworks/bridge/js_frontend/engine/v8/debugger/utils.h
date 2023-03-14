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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_UTILS_H

#include <iostream>

#include "third_party/v8/include/v8-inspector.h"
#include "third_party/v8/include/v8.h"

#include "base/log/log.h"

namespace V8Debugger {

static inline v8_inspector::StringView ConvertToStringView(const std::string& str)
{
    auto* stringView = reinterpret_cast<const uint8_t*>(str.c_str());
    return { stringView, str.length() };
}

} // namespace V8Debugger

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_UTILS_H
