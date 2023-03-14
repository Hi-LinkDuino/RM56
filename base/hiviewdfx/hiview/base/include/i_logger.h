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

#ifndef HIVIEW_BASE_INTERFACE_LOGGER_H
#define HIVIEW_BASE_INTERFACE_LOGGER_H
#include <string>
#include <memory>
#include "defines.h"
namespace OHOS {
namespace HiviewDFX {
class DllExport ILogger {
public:
    ILogger() {};
    virtual ~ILogger() {};
public:
    virtual void Print(uint32_t level, uint32_t domain, const char* tag, const char* format, va_list args) {};
};
} // HiviewDFX
} // OHOS
#endif // HIVIEW_BASE_INTERFACE_LOGGER_H