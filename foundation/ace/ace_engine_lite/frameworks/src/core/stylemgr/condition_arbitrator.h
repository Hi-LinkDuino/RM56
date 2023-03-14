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

#ifndef OHOS_ACELITE_STYLEMGR_CONDITION_ARBITRATOR
#define OHOS_ACELITE_STYLEMGR_CONDITION_ARBITRATOR

#include "memory_heap.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
enum ConditionName : uint8_t {
    WIDTH,
    MIN_WIDTH,
    MAX_WIDTH,
    HEIGHT,
    MIN_HEIGHT,
    MAX_HEIGHT,
    ASPECT_RATIO,
    MIN_ASPECT_RATIO,
    MAX_ASPECT_RATIO,
    ROUND_SCREEN,
    DEVICE_TYPE,
    UNKOWN,
    MAX_COUNT = UNKOWN
};

class ConditionArbitrator final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(ConditionArbitrator);
    ConditionArbitrator() = default;
    virtual ~ConditionArbitrator() = default;

    /**
     * @brief determine whether the media query conditions are met based on
     * the current device environment
     * @return the result whether the condition is met
     */
    bool Decide(const char *conditions) const;

private:
    /**
     * @brief judge the single condition is success or not
     */
    bool JudgeCondition(const char *condition) const;
    bool JudgeConditionAction(ConditionName conditionId, const char *trimedTargetValue) const;
    bool JudgeConditionByStrValue(ConditionName conditionId, const char *trimedTargetValue) const;
    bool JudgeConditionByNumberValue(ConditionName conditionId, const char *targetValue) const;
    bool CompareIntDimension(ConditionName conditionId, const char *targetValue) const;
    bool CompareFloatDimension(ConditionName conditionId, const char *targetValue) const;
    bool CompareAspectRatio(ConditionName conditionId, float targetRatioValue) const;
    ConditionName GetConditionName(const char *conditionName) const;
    bool IsFloatValueEqual(float left, float right, float precision) const;
    static constexpr float CONDITION_FLOAT_VALUE_EPRECISION = 1E-5;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_STYLEMGR_CONDITION_ARBITRATOR
