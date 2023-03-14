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

#ifndef HISTREAMER_PIPELINE_CORE_CLOCK_PROVIDER_H
#define HISTREAMER_PIPELINE_CORE_CLOCK_PROVIDER_H

#include <cstdint>
#include "error_code.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class ClockProvider {
public:
    virtual ~ClockProvider() = default;
    virtual ErrorCode CheckPts(int64_t pts, int64_t& delta) = 0;
    virtual ErrorCode GetCurrentPosition(int64_t& position) = 0;
    virtual ErrorCode GetCurrentTimeNano(int64_t& nowNano) = 0;
};
} // Pipeline
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_CORE_CLOCK_PROVIDER_H
