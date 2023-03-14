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

#ifndef LOG_DISPLAY_H
#define LOG_DISPLAY_H

#include "hilog_common.h"
#include "hilogtool_msg.h"
#include "log_controller.h"

namespace OHOS {
namespace HiviewDFX {
using namespace std;
int32_t ControlCmdResult(const char* message);
std::string ParseErrorCode(ErrorCode errorCode);
void HilogShowLog(uint32_t showFormat, HilogDataMessage* contentOut,
    const HilogArgs* context, vector<string>& tailBuffer);
HilogShowFormat HilogFormat(const char* formatArg);
} // namespace HiviewDFX
} // namespace OHOS
#endif
