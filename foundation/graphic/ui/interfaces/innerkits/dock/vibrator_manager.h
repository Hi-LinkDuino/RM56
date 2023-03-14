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

#ifndef GRAPHIC_LITE_VIBRATOR_MANAGER_H
#define GRAPHIC_LITE_VIBRATOR_MANAGER_H

#include "graphic_config.h"
#if ENABLE_VIBRATOR
#include "components/ui_view_group.h"
namespace OHOS {
/**
 * @brief Enumerates vibrator types.
 *
 * @since 5.0
 * @version 3.0
 */
enum class VibratorType { VIBRATOR_TYPE_ONE, VIBRATOR_TYPE_TWO, VIBRATOR_TYPE_THREE };

/**
 * @brief vibrator function.
 *
 * @param vibratorType vibrator type.
 */
typedef void (*VibratorFunc)(VibratorType vibratorType);

class VibratorManager {
public:
    /**
     * @brief Get the VibratorManager's singleton.
     *
     * @return VibratorManager's singleton.
     * @since 5.0
     * @version 3.0
     */
    static VibratorManager* GetInstance();

    void RegisterVibratorFunc(VibratorFunc vibratorFunc)
    {
        GRAPHIC_LOGI("VibratorManager::RegisterVibratorFunc");
        vibratorFunc_ = vibratorFunc;
    }

    VibratorFunc GetVibratorFunc()
    {
        return vibratorFunc_;
    }

private:
    VibratorManager() {}
    ~VibratorManager() {}

    VibratorFunc vibratorFunc_ = nullptr;
};
} // namespace OHOS
#endif
#endif // GRAPHIC_LITE_VIBRATOR_MANAGER_H
