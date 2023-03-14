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

import continuationManager from "../@ohos.continuation.continuationManager";

/**
 * Indicates the description of additional parameters for continuation.
 *
 * @since 8
 * @syscap SystemCapability.Ability.DistributedAbilityManager
 */
export interface ContinuationExtraParams {
    /**
     * Indicates the type of devices to be matched.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    deviceType?: Array<string>;

    /**
     * Indicates the bundle name of the target application where the ability will be hopped.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    targetBundle?: string;

    /**
     * Indicates the description used for device filtering.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    description?: string;

    /**
     * Parameters used for filtering devices.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    filter?: any;

    /**
     * Mode of continuation.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    continuationMode?: continuationManager.ContinuationMode;

    /**
     * Authentication extra infos.
     *
     * @since 8
     * @syscap SystemCapability.Ability.DistributedAbilityManager
     */
    authInfo?: { [key: string]: any }
}