/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { AsyncCallback , Callback} from './basic';
import { WantAgent } from "./@ohos.wantAgent";
import { Context } from './app/context';

/**
 * Manages background tasks.
 *
 * @since 7
 * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.TransientTask
 */
declare namespace backgroundTaskManager {
    /**
     * The info of delay suspend.
     *
     * @name DelaySuspendInfo
     * @since 7
     * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.TransientTask
     */
    interface DelaySuspendInfo {
        /**
         * The unique identifier of the delay request.
         */
        requestId: number;
        /**
         * The actual delay duration (ms).
         */
        actualDelayTime: number;
    }

    /**
     * Cancels delayed transition to the suspended state.
     *
     * @since 7
     * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.TransientTask
     * @param requestId Indicates the identifier of the delay request.
     */
    function cancelSuspendDelay(requestId: number): void;

    /**
     * Obtains the remaining time before an application enters the suspended state.
     *
     * @since 7
     * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.TransientTask
     * @param requestId Indicates the identifier of the delay request.
     * @return The remaining delay time
     */
    function getRemainingDelayTime(requestId: number, callback: AsyncCallback<number>): void;
    function getRemainingDelayTime(requestId: number): Promise<number>;

    /**
     * Requests delayed transition to the suspended state.
     *
     * @since 7
     * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.TransientTask
     * @param reason Indicates the reason for delayed transition to the suspended state.
     * @param callback The callback delay time expired.
     * @return Info of delay request
     */
    function requestSuspendDelay(reason: string, callback: Callback<void>): DelaySuspendInfo;

    /**
     * Service ability uses this method to request start running in background.
     * system will publish a notification related to the this service.
     *
     * @since 8
     * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
     * @permission ohos.permission.KEEP_BACKGROUND_RUNNING
     * @param context app running context.
     * @param bgMode Indicates which background mode to request.
     * @param wantAgent Indicates which ability to start when user click the notification bar.
     */
    function startBackgroundRunning(context: Context, bgMode: BackgroundMode, wantAgent: WantAgent, callback: AsyncCallback<void>): void;
    function startBackgroundRunning(context: Context, bgMode: BackgroundMode, wantAgent: WantAgent): Promise<void>;

    /**
     * Service ability uses this method to request stop running in background.
     *
     * @since 8
     * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
     * @param context app running context.
     */
    function stopBackgroundRunning(context: Context, callback: AsyncCallback<void>): void;
    function stopBackgroundRunning(context: Context): Promise<void>;

    /**
     * supported background mode.
     *
     * @since 8
     * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
     */
    export enum BackgroundMode {
        /**
         * data transfer mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         */
        DATA_TRANSFER = 1,

        /**
         * audio playback mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         */
        AUDIO_PLAYBACK = 2,

        /**
         * audio recording mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         */
        AUDIO_RECORDING = 3,

        /**
         * location mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         */
        LOCATION = 4,

        /**
         * bluetooth interaction mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         */
        BLUETOOTH_INTERACTION = 5,

        /**
         * multi-device connection mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         */
        MULTI_DEVICE_CONNECTION = 6,

        /**
         * wifi interaction mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         * @systemapi Hide this for inner system use.
         */
        WIFI_INTERACTION = 7,

        /**
         * Voice over Internet Phone mode
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         * @systemapi Hide this for inner system use.
         */
        VOIP = 8,

        /**
         * background continuous calculate mode, for example 3D render.
         * only supported in particular device
         *
         * @since 8
         * @syscap SystemCapability.ResourceSchedule.BackgroundTaskManager.ContinuousTask
         */
        TASK_KEEPING = 9,
    }
}

export default backgroundTaskManager;
