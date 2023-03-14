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
#ifndef HIVIEW_FAULT_LOGGER_CONSTANTS_H
#define HIVIEW_FAULT_LOGGER_CONSTANTS_H
#include <cstdint>
#include <list>
#include <map>
#include <string>

namespace OHOS {
namespace HiviewDFX {
namespace FaultLogger {
// faultlogger consts
constexpr char APPTYPE[] = "APPTYPE";
constexpr char OHOS_LOG_PATH[] = "OHOSLOGPATH";
constexpr char DEFAULT_FAULTLOG_FOLDER[] = "/data/log/faultlog/faultlogger/";
constexpr char DEFAULT_FAULTLOG_TEMP_FOLDER[] = "/data/log/faultlog/temp/";
constexpr int32_t MAX_FAULT_INFO_IN_MEM = 100; // 100 : log count
constexpr int32_t MAX_FAULT_LOG_PER_HAP = 10;
constexpr int32_t MAX_FAULTLOG_STORAGE_SIZE = 10485760; // 10485760 : 10M
}
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEW_FAULT_LOGGGER_CONSTANTS_H
