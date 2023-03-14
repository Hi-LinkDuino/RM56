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

import {AsyncCallback} from './basic';

/**
 * inputmethodengine
 *
 * @since 8
 * @syscap SystemCapability.MiscServices.InputMethodFramework
 */
declare namespace inputMethodEngine {
    const ENTER_KEY_TYPE_UNSPECIFIED: number;
    const ENTER_KEY_TYPE_GO: number;
    const ENTER_KEY_TYPE_SEARCH: number;
    const ENTER_KEY_TYPE_SEND: number;
    const ENTER_KEY_TYPE_NEXT: number;
    const ENTER_KEY_TYPE_DONE: number;
    const ENTER_KEY_TYPE_PREVIOUS: number;

    const PATTERN_NULL: number;
    const PATTERN_TEXT: number;
    const PATTERN_NUMBER: number;
    const PATTERN_PHONE: number;
    const PATTERN_DATETIME: number;
    const PATTERN_EMAIL: number;
    const PATTERN_URI: number;
    const PATTERN_PASSWORD: number;

    const FLAG_SELECTING: number;
    const FLAG_SINGLE_LINE: number;

    const DISPLAY_MODE_PART: number;
    const DISPLAY_MODE_FULL: number;

    const OPTION_ASCII: number;
    const OPTION_NONE: number;
    const OPTION_AUTO_CAP_CHARACTERS: number;
    const OPTION_AUTO_CAP_SENTENCES: number;
    const OPTION_AUTO_WORDS: number;
    const OPTION_MULTI_LINE: number;
    const OPTION_NO_FULLSCREEN: number;

    function getInputMethodEngine(): InputMethodEngine;

    function createKeyboardDelegate(): KeyboardDelegate;

    interface KeyboardController {
        hideKeyboard(callback: AsyncCallback<void>): void;

        hideKeyboard(): Promise<void>;
    }

    interface InputMethodEngine {
        on(type: 'inputStart', callback: (kbController: KeyboardController, textInputClient: TextInputClient) => void): void;

        off(type: 'inputStart', callback?: (kbController: KeyboardController, textInputClient: TextInputClient) => void): void;

        on(type: 'keyboardShow'|'keyboardHide', callback: () => void): void;

        off(type: 'keyboardShow'|'keyboardHide', callback?: () => void): void;
    }

    interface TextInputClient {
        sendKeyFunction(action: number, callback: AsyncCallback<boolean>): void;

        sendKeyFunction(action: number): Promise<boolean>;

        deleteForward(length: number, callback: AsyncCallback<boolean>): void;

        deleteForward(length: number): Promise<boolean>;

        deleteBackward(length: number, callback: AsyncCallback<boolean>): void;

        deleteBackward(length: number): Promise<boolean>;

        insertText(text: string, callback: AsyncCallback<boolean>): void;

        insertText(text: string): Promise<boolean>;

        getForward(length: number, callback: AsyncCallback<string>): void;

        getForward(length: number): Promise<string>;

        getBackward(length: number, callback: AsyncCallback<string>): void;

        getBackward(length: number): Promise<string>;

        getEditorAttribute(callback: AsyncCallback<EditorAttribute>): void;

        getEditorAttribute(): Promise<EditorAttribute>;
    }

    interface KeyboardDelegate {
        on(type: 'keyDown'|'keyUp', callback: (event: KeyEvent) => boolean): void;

        off(type: 'keyDown'|'keyUp', callback?: (event: KeyEvent) => boolean): void;

        on(type: 'cursorContextChange', callback: (x: number, y: number, height: number) => void): void;

        off(type: 'cursorContextChange', callback?: (x: number, y: number, height: number) => void): void;

        on(type: 'selectionChange', callback: (oldBegin: number, oldEnd: number, newBegin: number, newEnd: number) => void): void;

        off(type: 'selectionChange', callback?: (oldBegin: number, oldEnd: number, newBegin: number, newEnd: number) => void): void;

        on(type: 'textChange', callback: (text: string) => void): void;

        off(type: 'textChange', callback?: (text: string) => void): void;
    }

    interface EditorAttribute {
        readonly inputPattern: number;
        readonly enterKeyType: number;
    }

    interface KeyEvent {
        readonly keyCode: number;
        readonly keyAction: number;
    }
}

export default inputMethodEngine;
