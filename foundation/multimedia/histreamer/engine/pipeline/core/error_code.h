/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_FOUNDATION_ERROR_CODE_H
#define HISTREAMER_FOUNDATION_ERROR_CODE_H

#include <cstdint>

#ifdef WIN32
// Fix compile error: expected identifier before numeric constant
// also can add in plugin_loader.cpp before include log.h
#undef ERROR_INVALID_OPERATION
#undef ERROR_INVALID_STATE
#endif

namespace OHOS {
namespace Media {
enum struct ErrorCode : int32_t {
    END_OF_STREAM = 1,
    SUCCESS = 0,
    ERROR_UNKNOWN = INT32_MIN + 0,
    ERROR_UNIMPLEMENTED = ERROR_UNKNOWN + 1,
    ERROR_AGAIN = ERROR_UNKNOWN + 2,
    ERROR_INVALID_PARAMETER_VALUE = ERROR_UNKNOWN + 3,
    ERROR_INVALID_PARAMETER_TYPE = ERROR_UNKNOWN + 4,
    ERROR_INVALID_OPERATION = ERROR_UNKNOWN + 5,
    ERROR_UNSUPPORTED_FORMAT = ERROR_UNKNOWN + 6,
    ERROR_NOT_EXISTED = ERROR_UNKNOWN + 7,
    ERROR_TIMED_OUT = ERROR_UNKNOWN + 8,
    ERROR_NO_MEMORY = ERROR_UNKNOWN + 9,
    ERROR_INVALID_STATE = ERROR_UNKNOWN + 10,
    ERROR_PERMISSION_DENIED = ERROR_UNKNOWN + 11,
};

const char* GetErrorName(ErrorCode code);

#ifndef FAIL_RETURN
#define FAIL_RETURN(exec)                                                                                              \
    do {                                                                                                               \
        ErrorCode returnValue = (exec);                                                                                \
        if (returnValue != ErrorCode::SUCCESS) {                                                                       \
            MEDIA_LOG_E("FAIL_RETURN on ErrorCode(" PUBLIC_LOG_S ").", GetErrorName(returnValue));                     \
            return returnValue;                                                                                        \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FAIL_RETURN_MSG_IMPL
#define FAIL_RETURN_MSG_IMPL(loglevel, exec, fmt, args...)                                                             \
    do {                                                                                                               \
        ErrorCode returnValue = (exec);                                                                                \
        if (returnValue != ErrorCode::SUCCESS) {                                                                       \
            loglevel(fmt, ##args);                                                                                     \
            return returnValue;                                                                                        \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FAIL_RETURN_MSG
#define FAIL_RETURN_MSG(exec, fmt, args...) FAIL_RETURN_MSG_IMPL(MEDIA_LOG_E, exec, fmt, ##args)
#endif

#ifndef FAIL_RETURN_MSG_W
#define FAIL_RETURN_MSG_W(exec, fmt, args...) FAIL_RETURN_MSG_IMPL(MEDIA_LOG_W, exec, fmt, ##args)
#endif

#ifndef FAIL_LOG
#define FAIL_LOG(exec)                                                                                                 \
    do {                                                                                                               \
        ErrorCode returnValue = (exec);                                                                                \
        if (returnValue != ErrorCode::SUCCESS) {                                                                       \
            MEDIA_LOG_E("FAIL_LOG on ErrorCode(" PUBLIC_LOG_S ").", GetErrorName(returnValue));                        \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FAIL_LOG_MSG_IMPL
#define FAIL_LOG_MSG_IMPL(loglevel, exec, fmt, args...)                                                                \
    do {                                                                                                               \
        ErrorCode returnValue = (exec);                                                                                \
        if (returnValue != ErrorCode::SUCCESS) {                                                                       \
            loglevel(fmt, ##args);                                                                                     \
        }                                                                                                              \
    } while (0)
#endif

#ifndef FAIL_LOG_MSG
#define FAIL_LOG_MSG(exec, fmt, args...) FAIL_LOG_MSG_IMPL(MEDIA_LOG_E, exec, fmt, ##args)
#endif

#ifndef FAIL_LOG_MSG_W
#define FAIL_LOG_MSG_W(exec, fmt, args...) FAIL_LOG_MSG_IMPL(MEDIA_LOG_W, exec, fmt, ##args)
#endif
}  // namespace Media
}  // namespace OHOS
#endif  // HISTREAMER_FOUNDATION_ERROR_CODE_H
