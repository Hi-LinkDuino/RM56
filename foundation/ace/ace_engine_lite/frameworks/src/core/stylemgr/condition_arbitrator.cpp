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

#include "condition_arbitrator.h"

#include "ace_log.h"
#include "js_fwk_common.h"
#include "securec.h"
#include "stdlib.h"
#include "string_util.h"
#include "strings.h"
#include "system_info.h"

namespace OHOS {
namespace ACELite {
/**
 * @brief Absolute value of x.
 */
#define ABS_VALUE(x) ((x) > 0 ? (x) : (-(x)))

/**
 * @brief parse the whole condition string into single condition items, and check if it matchs the current device
 *        environment, and gives the final result of the entire media query expression.
 * @param conditions the input media query condition string
 * @return the result representing if the media query matchs the current environment, true for positive result
 *
 * NOTE: only supports the pattern such as "screen and (device-type: liteWearable) and (width: 454)"
 */
bool ConditionArbitrator::Decide(const char *conditions) const
{
    if (conditions == nullptr || strlen(conditions) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the input condition str is invalid");
        return false;
    }

    const char *lastCondition = conditions;
    const char *remainCondition = lastCondition;
    char *singleCondition = nullptr;
    const uint8_t addedLen = 3;
    bool result = true;
    while (result && remainCondition) {
        lastCondition = remainCondition;
        remainCondition = strstr(remainCondition, "and");
        singleCondition = StringUtil::Slice(
            lastCondition, 0, strlen(lastCondition) - ((remainCondition == nullptr) ? 0 : strlen(remainCondition)));
        if (remainCondition != nullptr) {
            remainCondition = remainCondition + addedLen;
        }
        char *condition = StringUtil::Trim(singleCondition);
        if (condition == nullptr) {
            continue;
        }
        result = JudgeCondition(condition);
        ace_free(singleCondition);
        singleCondition = nullptr;
    }
    ACE_FREE(singleCondition);
    return result;
}

bool ConditionArbitrator::JudgeCondition(const char *condition) const
{
    if (condition == nullptr || strlen(condition) == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the condition is invalid");
        return false;
    }

    // treat screen as one single media query feature, and it always is true for any devices
    if (strcmp(condition, "screen") == 0) {
        return true;
    }

    uint8_t conditionLen = strlen(condition);
    // the condition must start with '(', end with ')'
    if (condition[0] != '(' || (conditionLen <= 1) || (condition[conditionLen - 1] != ')')) {
        HILOG_ERROR(HILOG_MODULE_ACE, "error format, condition is not properly packed with ( )");
        return false;
    }
    const uint8_t minTargetLen = 2;
    char *targetCondition = StringUtil::Slice(condition, 1, conditionLen - minTargetLen + 1);
    if (targetCondition == nullptr) {
        return false;
    }
    // devide the target condition into two parts : conditionName and value  through ":"
    char *targetValue = nullptr;
    char *conditionNameStr = strtok_s(targetCondition, ":", &targetValue);
    // judge the condition is success or not
    ConditionName conditionId = GetConditionName(StringUtil::Trim(conditionNameStr));
    char *trimedTargetValue = StringUtil::Trim(targetValue);
    if (conditionId == ConditionName::UNKOWN || trimedTargetValue == nullptr) {
        ace_free(targetCondition);
        targetCondition = nullptr;
        return false;
    }
    bool result = JudgeConditionAction(conditionId, trimedTargetValue);
    ace_free(targetCondition);
    targetCondition = nullptr;
    return result;
}

bool ConditionArbitrator::JudgeConditionAction(ConditionName conditionId, const char *trimedTargetValue) const
{
    if (trimedTargetValue == nullptr || strlen(trimedTargetValue) == 0) {
        return false;
    }
    switch (conditionId) {
        case ConditionName::DEVICE_TYPE: // fall through
        case ConditionName::ROUND_SCREEN:
            return JudgeConditionByStrValue(conditionId, trimedTargetValue);
        case ConditionName::WIDTH: // fall through
        case ConditionName::MIN_WIDTH: // fall through
        case ConditionName::MAX_WIDTH: // fall through
        case ConditionName::HEIGHT: // fall through
        case ConditionName::MIN_HEIGHT: // fall through
        case ConditionName::MAX_HEIGHT: // fall through
        case ConditionName::ASPECT_RATIO: // fall through
        case ConditionName::MIN_ASPECT_RATIO: // fall through
        case ConditionName::MAX_ASPECT_RATIO: {
            return JudgeConditionByNumberValue(conditionId, trimedTargetValue);
        }
        default: {
            HILOG_ERROR(HILOG_MODULE_ACE, "not supported condition feature %{public}d", conditionId);
            return false;
        }
    }
}

bool ConditionArbitrator::JudgeConditionByStrValue(ConditionName conditionId, const char *trimedTargetValue) const
{
    bool result = false;
    switch (conditionId) {
        case ConditionName::DEVICE_TYPE: {
            result = (strcmp(trimedTargetValue, SystemInfo::GetInstance().GetDeviceType()) == 0);
            break;
        }
        case ConditionName::ROUND_SCREEN: {
            if (!strcmp(trimedTargetValue, "TRUE") || !strcmp(trimedTargetValue, "true") ||
                !strcmp(trimedTargetValue, "1")) {
                result = (SystemInfo::GetInstance().IsRoundScreen() == true);
            } else if (!strcmp(trimedTargetValue, "FALSE") || !strcmp(trimedTargetValue, "false") ||
                       !(strcmp(trimedTargetValue, "0"))) {
                result = (SystemInfo::GetInstance().IsRoundScreen() == false);
            } else {
                result = false;
            }
            break;
        }
        default: {
            return false;
        }
    }
    return result;
}

bool ConditionArbitrator::JudgeConditionByNumberValue(ConditionName conditionId, const char *targetValue) const
{
    // must be started with number character
    if (!(targetValue[0] >= 48 && targetValue[0] <= 57)) {
        return false;
    }
    switch (conditionId) {
        case ConditionName::WIDTH: // fall through
        case ConditionName::MIN_WIDTH: // fall through
        case ConditionName::MAX_WIDTH: // fall through
        case ConditionName::HEIGHT: // fall through
        case ConditionName::MIN_HEIGHT: // fall through
        case ConditionName::MAX_HEIGHT: {
            return CompareIntDimension(conditionId, targetValue);
        }
        case ConditionName::ASPECT_RATIO: // fall through
        case ConditionName::MIN_ASPECT_RATIO: // fall through
        case ConditionName::MAX_ASPECT_RATIO: {
            return CompareFloatDimension(conditionId, targetValue);
        }
        default: {
            return false;
        }
    }
}

bool ConditionArbitrator::CompareIntDimension(ConditionName conditionId, const char *targetValue) const
{
    int dimensionValue = atoi(targetValue);
    if (dimensionValue <= 0 || dimensionValue >= UINT16_MAX) {
        return false;
    }
    switch (conditionId) {
        case ConditionName::WIDTH: {
            return SystemInfo::GetInstance().GetScreenWidth() == dimensionValue;
        }
        case ConditionName::MIN_WIDTH: {
            // the device screen width must be larger than the requirement
            return SystemInfo::GetInstance().GetScreenWidth() >= dimensionValue;
        }
        case ConditionName::MAX_WIDTH: {
            // the device screen width must be smaller than the requirement
            return SystemInfo::GetInstance().GetScreenWidth() <= dimensionValue;
        }
        case ConditionName::HEIGHT: {
            return SystemInfo::GetInstance().GetScreenHeight() == dimensionValue;
        }
        case ConditionName::MIN_HEIGHT: {
            // the device screen height must be larger than the requirement
            return SystemInfo::GetInstance().GetScreenHeight() >= dimensionValue;
        }
        case ConditionName::MAX_HEIGHT: {
            // the device screen height must be smaller than the requirement
            return SystemInfo::GetInstance().GetScreenHeight() <= dimensionValue;
        }
        default:
            return false;
    }
}

bool ConditionArbitrator::IsFloatValueEqual(float left, float right, float precision) const
{
    return (ABS_VALUE(left - right) < precision);
}

bool ConditionArbitrator::CompareFloatDimension(ConditionName conditionId, const char *targetValue) const
{
    float floatValue = atof(targetValue);
    switch (conditionId) {
        case ConditionName::ASPECT_RATIO: // fall through
        case ConditionName::MIN_ASPECT_RATIO: // fall through
        case ConditionName::MAX_ASPECT_RATIO: {
            return CompareAspectRatio(conditionId, floatValue);
        }
        default: {
            return false;
        }
    }
}

bool ConditionArbitrator::CompareAspectRatio(ConditionName conditionId, float targetRatioValue) const
{
    float currentAspectRatio = SystemInfo::GetInstance().GetAspectRatio();
    bool isEqual = IsFloatValueEqual(targetRatioValue, currentAspectRatio, CONDITION_FLOAT_VALUE_EPRECISION);
    if (isEqual) {
        // the equal case matchs for all ASPECT_RATIO media feature types
        return true;
    }
    switch (conditionId) {
        case ConditionName::ASPECT_RATIO: {
            return isEqual;
        }
        case ConditionName::MIN_ASPECT_RATIO: {
            return currentAspectRatio > targetRatioValue;
        }
        case ConditionName::MAX_ASPECT_RATIO: {
            return currentAspectRatio < targetRatioValue;
        }
        default: {
            return false;
        }
    }
}

ConditionName ConditionArbitrator::GetConditionName(const char *conditionName) const
{
    if (conditionName == nullptr || strlen(conditionName) == 0) {
        return ConditionName::UNKOWN;
    }
    static const struct {
        const char *nameStr;
        ConditionName name;
    } conditionNamePair[ConditionName::MAX_COUNT] = {
        {"width", ConditionName::WIDTH},
        {"height", ConditionName::HEIGHT},
        {"min-width", ConditionName::MIN_WIDTH},
        {"max-width", ConditionName::MAX_WIDTH},
        {"min-height", ConditionName::MIN_HEIGHT},
        {"max-height", ConditionName::MAX_HEIGHT},
        {"aspect-ratio", ConditionName::ASPECT_RATIO},
        {"min-aspect-ratio", ConditionName::MIN_ASPECT_RATIO},
        {"max-aspect-ratio", ConditionName::MAX_ASPECT_RATIO},
        {"device-type", ConditionName::DEVICE_TYPE},
        {"round-screen", ConditionName::ROUND_SCREEN}
    };
    ConditionName targetName = ConditionName::UNKOWN;
    uint8_t index = 0;
    for (; index < ConditionName::UNKOWN; index++) {
        if (strcmp(conditionName, conditionNamePair[index].nameStr) == 0) {
            targetName = conditionNamePair[index].name;
            break;
        }
    }
    if (index == ConditionName::UNKOWN) {
        HILOG_ERROR(HILOG_MODULE_ACE, "the condition name is not supported [%{public}s]", conditionName);
    }
    return targetName;
}
} // namespace ACELite
} // namespace OHOS
