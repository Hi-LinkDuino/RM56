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

#ifndef OHOS_LITE_CONTEXT_H
#define OHOS_LITE_CONTEXT_H

#include "ability_manager.h"
#include "want.h"

namespace OHOS {
class LiteContext {
public:
    LiteContext() = default;
    ~LiteContext() = default;

    /**
     * @brief Starts an {@link Ability} based on the specified {@link Want} information.
     *
     * @param want Indicates the pointer to the {@link Want} structure containing information about the ability to
     *             start.
     * @return Returns <b>0</b> if the operation is successful; returns a non-zero value otherwise.
     */
    int StartAbility(const Want &want);

    /**
     * @brief Destroys this {@link Ability}.
     *
     * This function can be called only by this ability.
     *
     * @return Returns <b>0</b> if the operation is successful; returns <b>-1</b> otherwise.
     */
    int TerminateAbility();
private:
    friend class SliteAbility;
    uint64_t token_ { 0 };
};
} //  OHOS

#endif //  OHOS_LITE_CONTEXT_H
