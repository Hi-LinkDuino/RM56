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

#ifndef OHOS_SLITE_ABILITY_H
#define OHOS_SLITE_ABILITY_H

#include "lite_context.h"
#include "want.h"

namespace OHOS {
/**
 * @brief Declares ability-related functions, including ability lifecycle callbacks and functions for connecting to or
 *        disconnecting from Particle Abilities.
 *
 * As the fundamental unit of OpenHarmony applications, abilities are classified into Feature Abilities and Particle
 * Abilities. Feature Abilities support the Page template, and Particle Abilities support the Service template.
 * An ability using the Page template is called Page ability for short and that using the Service template
 * is called Service ability.
 *
 * @since 1.0
 * @version 1.0
 */
class SliteAbility : public LiteContext {
public:
    SliteAbility() = default;
    virtual ~SliteAbility() = default;

    /**
     * @brief Called when this ability is started. You must override this function if you want to perform some
     *        initialization operations during ability startup.
     *
     * This function can be called only once in the entire lifecycle of an ability.
     * @param want Indicates the {@link Want} structure containing startup information about the ability.
     */
    virtual void OnStart(const Want &want);

    /**
     * @brief Called when this ability enters the <b>STATE_INACTIVE</b> state.
     *
     * <b>STATE_INACTIVE</b> is an instantaneous state. The ability in this state may be visible but does not have
     * focus. You can override this function to implement your own processing logic.
     */
    virtual void OnInactive();

    /**
     * @brief Called when this ability enters the <b>STATE_ACTIVE</b> state.
     *
     * The ability in the <b>STATE_ACTIVE</b> state is visible and has focus.
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing activation information about the ability.
     */
    virtual void OnActive(const Want &want);

    /**
     * @brief Called when this ability enters the <b>STATE_BACKGROUND</b> state.
     *
     *
     * The ability in the <b>STATE_BACKGROUND</b> state is invisible.
     * You can override this function to implement your own processing logic.
     */
    virtual void OnBackground();

    /**
     * @brief Called when this ability enters the <b>STATE_STOP</b> state.
     *
     * The ability in the <b>STATE_STOP</b> is being destroyed.
     * You can override this function to implement your own processing logic.
     */
    virtual void OnStop();

    int GetState() const;
private:
    int abilityState_ = 0;
};
} // namespace OHOS
#endif // OHOS_SLITE_ABILITY_H
