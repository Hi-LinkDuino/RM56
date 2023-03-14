/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import AppStateData from "./AppStateData";
import AbilityStateData from "./AbilityStateData";
import ProcessData from "./ProcessData";

/**
 * The application state observer.
 *
 * @since 8
 * @syscap SystemCapability.Ability.AbilityRuntime.Core
 * @systemapi hide for inner use.
 * @permission N/A
 */
export default class ApplicationStateObserver {
    /**
     * Will be called when foreground or background application changed.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param appStateData State changed Application info.
     * @systemapi hide for inner use.
     * @return -
     */
    onForegroundApplicationChanged(appStateData: AppStateData): void;

    /**
     * Will be called when ability state changed.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param abilityStateData State changed ability info.
     * @systemapi hide for inner use.
     * @return -
     */
    onAbilityStateChanged(abilityStateData: AbilityStateData): void;

    /**
     * Will be called when process created.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param processData Process info.
     * @systemapi hide for inner use.
     * @return -
     */
    onProcessCreated(processData: ProcessData): void;

    /**
     * Will be called when process died.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Core
     * @param processData Process info.
     * @systemapi hide for inner use.
     * @return -
     */
    onProcessDied(processData: ProcessData): void;
}