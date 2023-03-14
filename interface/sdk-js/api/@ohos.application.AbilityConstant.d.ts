/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

/**
 * The definition of AbilityConstant.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 * @StageModelOnly
 */
declare namespace AbilityConstant {
    /**
     * Interface of launch param.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @StageModelOnly
     */
    export interface LaunchParam {
        /**
         * Indicates launch reason.
         *
         * @since 9
         * @syscap SystemCapability.Ability.AbilityRuntime.Core
         * @StageModelOnly
         */
        launchReason: LaunchReason;

        /**
         * Indicates last exit reason.
         *
         * @since 9
         * @syscap SystemCapability.Ability.AbilityRuntime.Core
         * @StageModelOnly
         */
        lastExitReason: LastExitReason;
    }

    /**
     * Type of launch reason.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @StageModelOnly
     */
    export enum LaunchReason {
        UNKNOWN = 0,
        START_ABILITY = 1,
        CALL = 2,
        CONTINUATION = 3,
    }

    /**
     * Type of last exit reason.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @StageModelOnly
     */
    export enum LastExitReason {
        UNKNOWN = 0,
        ABILITY_NOT_RESPONDING = 1,
        NORMAL = 2,
    }

    /**
     * Type of onContinue result.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @StageModelOnly
     */
     export enum OnContinueResult {
        AGREE = 0,
        REJECT = 1,
        MISMATCH = 2,
    }
}

export default AbilityConstant
