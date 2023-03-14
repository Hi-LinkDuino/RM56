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

import { AbilityDelegator } from './application/abilityDelegator'
import { AbilityDelegatorArgs } from './application/abilityDelegatorArgs'

/**
 * A global register used to store the AbilityDelegator and AbilityDelegatorArgs objects registered
 * during application startup.
 *
 * @since 8
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @import import AbilityDelegatorRegistry from '@ohos.application.abilityDelegatorRegistry'
 * @permission N/A
 */
declare namespace abilityDelegatorRegistry {
    /**
     * Get the AbilityDelegator object of the application.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @return the AbilityDelegator object initialized when the application is started.
     */
    function getAbilityDelegator(): AbilityDelegator;

    /**
     * Get unit test parameters stored in the AbilityDelegatorArgs object.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @return the previously registered AbilityDelegatorArgs object.
     */
    function getArguments(): AbilityDelegatorArgs;

    /**
     * Describes all lifecycle states of an ability.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     */
    export enum AbilityLifecycleState {
        UNINITIALIZED,
        CREATE,
        FOREGROUND,
        BACKGROUND,
        DESTROY,
    }
}

export default abilityDelegatorRegistry;