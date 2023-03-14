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
#include "plugin_core_utils.h"

namespace OHOS {
namespace Media {
namespace Plugin {
const char* GetStateString(State state)
{
    using namespace OHOS::Media::Plugin;
    switch (state) {
        case State::CREATED:
            return "Created";
        case State::INITIALIZED:
            return "Initialized";
        case State::DESTROYED:
            return "Destroyed";
        case State::PREPARED:
            return "Prepared";
        case State::RUNNING:
            return "Running";
        case State::PAUSED:
            return "Paused";
        default:
            return "Invalid";
    }
}
} // Plugin
} // Media
} // OHOS