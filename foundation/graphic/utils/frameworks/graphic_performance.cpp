/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "graphic_performance.h"
#if ENABLE_DEBUG && ENABLE_DEBUG_PERFORMANCE_TRACE
namespace OHOS {
Performance* Performance::GetInstance()
{
    static Performance instance;
    return &instance;
}
} // namespace OHOS
#endif // ENABLE_DEBUG && ENABLE_DEBUG_PERFORMANCE_TRACE
