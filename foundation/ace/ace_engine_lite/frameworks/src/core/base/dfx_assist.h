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

#ifndef OHOS_ACELITE_DFX_ASSIST_H
#define OHOS_ACELITE_DFX_ASSIST_H

#include "acelite_config.h"
#include "js_app_context.h"
#include "memory_heap.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class DfxAssist final : public MemoryHeap {
public:
    DfxAssist() = default;

    ~DfxAssist() = default;

    /**
     * @fn DfxAssist::DumpErrorCode()
     *
     * @brief dump js error error code
     */
    void DumpErrorCode(const jerry_value_t errorValue);

    /**
     * @fn DfxAssist::DumpErrorMessage()
     *
     * @brief dump js stack error message
     */
    void DumpErrorMessage(const jerry_value_t errorValue);
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_DFX_ASSIST_H
