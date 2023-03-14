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

import ConfigurationConstant from "./@ohos.application.ConfigurationConstant";

/**
 * configuration item.
 *
 * @name Configuration
 * @since 8
 * @syscap SystemCapability.Ability.AbilityBase
 * @permission N/A
 */
export interface Configuration {
    /**
     * Indicates the current language of the application.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityBase
     */
    language?: string;

    /**
     * Indicates the current colorMode of the application.
     *
     * @since 8
     * @syscap SystemCapability.Ability.AbilityBase
     */
    colorMode?: ConfigurationConstant.ColorMode;

    /**
     * Indicates the screen direction of the current device.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityBase
     */
     direction?: ConfigurationConstant.Direction;

    /**
     * Indicates the screen density of the current device.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityBase
     */
    screenDensity?: ConfigurationConstant.ScreenDensity;

    /**
     * Indicates the displayId of the current device.
     *
     * @since 9
     * @syscap SystemCapability.Ability.AbilityBase
     */
    displayId?: number;
}
