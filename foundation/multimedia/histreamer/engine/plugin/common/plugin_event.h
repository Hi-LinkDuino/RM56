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

#ifndef HISTREAMER_PLUGIN_EVENT_H
#define HISTREAMER_PLUGIN_EVENT_H

#include <cstdint> // NOLINT: using int32_t in this file
#include <string>
#include "any.h"

namespace OHOS {
namespace Media {
namespace Plugin {
enum struct PluginEventType : int32_t {
    CLIENT_ERROR,
    SERVER_ERROR,
    OTHER_ERROR,
    BELOW_LOW_WATERLINE,
    ABOVE_LOW_WATERLINE,
};

enum class NetworkClientErrorCode : int32_t {
    ERROR_OK,
    ERROR_TIME_OUT,
    ERROR_NOT_RETRY,
    ERROR_UNKNOWN,
};

using NetworkServerErrorCode = int;

struct PluginEvent {
    PluginEventType type;
    Any param;
    std::string description;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_TYPES_H
