/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

import AbilityStageContext from "./application/AbilityStageContext";
import Want from './@ohos.application.Want';
import { Configuration } from './@ohos.application.Configuration';

/**
 * The class of an ability stage.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 * @StageModelOnly
 */
export default class AbilityStage {
    /**
     * Indicates configuration information about context.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @StageModelOnly
     */
    context: AbilityStageContext;

    /**
     * Called back when an ability stage is started for initialization.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @return -
     * @StageModelOnly
     */
    onCreate(): void;

    /**
     * Called back when start specified ability.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param want Indicates the want info of startd ability.
     * @return The user returns an ability string ID. If the ability of this ID has been started before,
     *         do not create a new instance and pull it back to the top of the stack.
     *         Otherwise, create a new instance and start it.
     * @StageModelOnly
     */
    onAcceptWant(want: Want): string;

    /**
     * Called when the system configuration is updated.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param config Indicates the updated configuration.
     * @return -
     * @StageModelOnly
     */
    onConfigurationUpdated(config: Configuration): void;
}
