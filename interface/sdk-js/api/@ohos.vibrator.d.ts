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
import { AsyncCallback } from './basic';

/**
 * This module provides the capability to control motor vibration.
 *
 * @since 8
 * @syscap SystemCapability.Sensors.MiscDevice
 * @import import vibrator from '@ohos.vibrator'
 * @permission ohos.permission.VIBRATE
 */
declare namespace vibrator {
    /**
     * The trigger motor vibrates for a specified length of time.
     * @param duration Indicate the duration of the motor vibration.
     * @syscap SystemCapability.Sensors.MiscDevice
     * @permission ohos.permission.VIBRATE
     * @since 8
     */
    function vibrate(duration: number, callback?: AsyncCallback<void>): void;
    function vibrate(duration: number): Promise<void>;

    /**
     * The trigger motor vibrates for the specified effect of the preset.
     * @param effectId Indicate the specified effect of the preset, {@code EffectId}.
     * @syscap SystemCapability.Sensors.MiscDevice
     * @permission ohos.permission.VIBRATE
     * @since 8
     */
    function vibrate(effectId: EffectId): Promise<void>;
    function vibrate(effectId: EffectId, callback?: AsyncCallback<void>): void;

    /**
     * Stop the motor from vibrating.
     * @param stopMode Indicate the stop mode in which the motor vibrates, {@code VibratorStopMode}.
     * @syscap SystemCapability.Sensors.MiscDevice
     * @permission ohos.permission.VIBRATE
     * @since 8
     */
    function stop(stopMode: VibratorStopMode): Promise<void>;
    function stop(stopMode: VibratorStopMode, callback?: AsyncCallback<void>): void;

    /**
     * Preset vibration effect string.
     * @syscap SystemCapability.Sensors.MiscDevice
     * @since 8
     */
    enum EffectId {
        /* Describes the vibration effect of the vibrator when a user adjusts the timer.*/
        EFFECT_CLOCK_TIMER = "haptic.clock.timer",
    }

    /**
     * Vibrator vibration stop mode.
     * @syscap SystemCapability.Sensors.MiscDevice
     * @since 8
     */
    enum VibratorStopMode {
        /* Indicates the mode of stopping a one-shot vibration effect.*/
        VIBRATOR_STOP_MODE_TIME = "time",
        /* Indicates the mode of stopping a preset vibration effect.*/
        VIBRATOR_STOP_MODE_PRESET = "preset",
    }
}

export default vibrator;