/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "ability_callback_utils.h"

namespace OHOS {
std::string ObtainErrorMessage(uint8_t errorCode)
{
    switch (errorCode) {
        case PARAM_NULL_ERROR:
            return "PARAM_NULL_ERROR";
        case MEMORY_MALLOC_ERROR:
            return "MEMORY_MALLOC_ERROR";
        case DUMP_ABILITIES_ERROR:
            return "DUMP_ABILITIES_ERROR";
        case IPC_REQUEST_ERROR:
            return "IPC_REQUEST_ERROR";
        case SERIALIZE_ERROR:
            return "SERIALIZE_ERROR";
        case CREATE_APPTASK_ERROR:
            return "CREATE_APPTASK_ERROR";
        case SCHEDULER_LIFECYCLE_ERROR:
            return "SCHEDULER_LIFECYCLE_ERROR";
        case PARAM_CHECK_ERROR:
            return "PARAM_CHECK_ERROR";
        case COMMAND_ERROR:
            return "COMMAND_ERROR";
        default: {
            return "";
        }
    }
}
} // namespace