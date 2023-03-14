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

/**
 * The process data.
 *
 * @since 8
 * @syscap SystemCapability.Ability.AbilityRuntime.Mission
 * @systemapi hide for inner use.
 * @permission N/A
 */
export default class ProcessData {
    /**
     * The bundle name.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Mission
     * @systemapi hide for inner use.
     */
    bundleName: string;

    /**
     * The pid.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Mission
     * @systemapi hide for inner use.
     */
    pid: number;

    /**
     * The uid.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityRuntime.Mission
     * @systemapi hide for inner use.
     */
    uid: number;
}