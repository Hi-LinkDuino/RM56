/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

 /**
 * Global Key Event Injection
 *
 * @since 8
 * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
 * @import import inputEventClient from '@ohos.multimodalInput.inputEventClient';
 * @systemapi hide for inner use
 */

declare namespace inputEventClient {
    /**
     * Defines event of key that user want to inject.
     *
     * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
     * @systemapi hide for inner use
     * @param isPressed The status of key.
     * @param keyCode The keyCode value of key.
     * @param keyDownDuration Key hold duration.
     * @param isIntercepted Whether the key is blocked.
     */
    interface KeyEvent {
        isPressed: boolean,
        keyCode: number;
        keyDownDuration: number;
        isIntercepted: boolean;
    }

    /**
     * Inject system keys.
     *
     * @since 8
     * @syscap SystemCapability.MultimodalInput.Input.InputSimulator
     * @systemapi hide for inner use
     * @param KeyEvent the key event to be injected.
     */
    function injectEvent({KeyEvent: KeyEvent}): void;
}

export default inputEventClient;
