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

import { HapModuleInfo } from "../bundle/hapModuleInfo";
import { Configuration } from '../@ohos.application.Configuration';
import Context from "./Context";

/**
 * The context of an abilityStage. It allows access to abilityStage-specific resources.
 *
 * @since 9
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @permission N/A
 * @StageModelOnly
 */
export default class AbilityStageContext extends Context {

    /**
     * Indicates configuration information about an module.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @StageModelOnly
     */
    currentHapModuleInfo: HapModuleInfo;

    /**
     * Indicates configuration information.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @StageModelOnly
     */
    config: Configuration;
}