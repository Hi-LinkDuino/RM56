/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

import {AsyncCallback} from './basic.d.ts';

/**
 * Provides a mechanism to prevent the system from hibernating so that the applications can run in the background or
 * when the screen is off.
 *
 * <p>{@link createRunningLock} can be called to obtain a {@link RunningLock}.
 * <p>{@link lock} can be called to set the lock duration, during which the system will not hibernate. After the
 * lock duration times out, the lock is automatically released and the system hibernates if no other {@link
  * RunningLock} is set.
 *
 * @syscap SystemCapability.PowerManager.PowerManager.Core
 * @since 7
 */
declare namespace runningLock {
    class RunningLock {
        /**
         * Prevents the system from hibernating and sets the lock duration.
         * This method requires the ohos.permission.RUNNING_LOCK permission.
         *
         * @param timeout Indicates the lock duration (ms). After the lock duration times out, the lock is automatically
         * released and the system hibernates if no other {@link RunningLock} is set.
         * @permission ohos.permission.RUNNING_LOCK
         * @since 7
         */
        lock(timeout: number): void;

        /**
         * Checks whether a lock is held or in use.
         *
         * @return Returns true if the lock is held or in use; returns false if the lock has been released.
         * @since 7
         */
        isUsed(): boolean;

        /**
         * Release the {@link RunningLock} that prevents the system from hibernating.
         * This method requires the ohos.permission.RUNNING_LOCK permission.
         *
         * @since 7
         * @permission ohos.permission.RUNNING_LOCK
         */
        unlock(): void;
    }

    /**
     * Enumerates the {@link RunningLock} types.
     *
     * <p>Two {@link RunningLock} types are available: {@link BACKGROUND}, and {@link PROXIMITY_SCREEN_CONTROL}.
     * {@link BACKGROUND} ensures that applications can run in the background.
     * {@link PROXIMITY_SCREEN_CONTROL} determines whether to turn on or off the screen based on the proximity sensor.
     *
     * @since 7
     */
    export enum RunningLockType {
        /**
         * Indicates the lock that prevents the system from hibernating.
         */
        BACKGROUND = 1,
        /**
         * Indicates the lock that determines whether to turn on or off the screen based on the proximity sensor.
         * For example, during a call, if the proximity sensor detects that the device is moving close to
         * the user's ear, the screen turns off; if the proximity sensor detects that the device is moving away
         * from the user's ear, the screen turns on.
         */
        PROXIMITY_SCREEN_CONTROL
    }

    /**
     * Checks whether the specified {@link RunningLockType} is supported.
     *
     * @param type Indicates the specified {@link RunningLockType}.
     * @return Returns true if the specified {@link RunningLockType} is supported;
     * returns false otherwise.
     * @since 7
     */
    function isRunningLockTypeSupported(type: RunningLockType, callback: AsyncCallback<boolean>): void;
    function isRunningLockTypeSupported(type: RunningLockType): Promise<boolean>;
    /**
     * Creates a {@link RunningLock} object.
     *
     * <p>This method requires the ohos.permission.RUNNING_LOCK permission.
     *
     * <p>The {@link RunningLock} object can be used to perform a lock operation to prevent the system from hibernating.
     *
     * @param name Indicates the {@link RunningLock} name. A recommended name consists of the package or class name and
     * a suffix.
     * @param type Indicates the {@link RunningLockType}.
     * @return Returns the {@link RunningLock} object.
     * @permission ohos.permission.RUNNING_LOCK
     * @since 7
     */
    function createRunningLock(name: string, type: RunningLockType, callback: AsyncCallback<RunningLock>): void;
    function createRunningLock(name: string, type: RunningLockType): Promise<RunningLock>;
}
export default runningLock;
