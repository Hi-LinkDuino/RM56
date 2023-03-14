/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include "error_code.h"
#include <map>

namespace OHOS {
namespace Media {
static const std::map<ErrorCode, const char*> g_ErrorTypeMap = {
    {ErrorCode::END_OF_STREAM,  "END_OF_STREAM"},
    {ErrorCode::SUCCESS,    "SUCCESS"},
    {ErrorCode::ERROR_UNKNOWN,  "ERROR_UNKNOWN"},
    {ErrorCode::ERROR_UNIMPLEMENTED,    "ERROR_UNIMPLEMENTED"},
    {ErrorCode::ERROR_AGAIN,    "ERROR_AGAIN"},
    {ErrorCode::ERROR_INVALID_PARAMETER_VALUE,   "ERROR_INVALID_PARAMETER_VALUE"},
    {ErrorCode::ERROR_INVALID_PARAMETER_TYPE,     "ERROR_INVALID_PARAMETER_TYPE"},
    {ErrorCode::ERROR_INVALID_OPERATION,    "ERROR_INVALID_OPERATION"},
    {ErrorCode::ERROR_UNSUPPORTED_FORMAT,   "ERROR_UNSUPPORTED_FORMAT"},
    {ErrorCode::ERROR_NOT_EXISTED,  "ERROR_NOT_EXISTED"},
    {ErrorCode::ERROR_TIMED_OUT,    "ERROR_TIMED_OUT"},
    {ErrorCode::ERROR_NO_MEMORY,    "ERROR_NO_MEMORY"},
    {ErrorCode::ERROR_INVALID_STATE,    "ERROR_INVALID_STATE"},
    {ErrorCode::ERROR_PERMISSION_DENIED,    "ERROR_PERMISSION_DENIED"}
};

const char* GetErrorName(ErrorCode code)
{
    auto it = g_ErrorTypeMap.find(code);
    if (it!= g_ErrorTypeMap.end()) {
        return it->second;
    }
    return "Unknow error type";
}
}
}