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
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 8
 * @import prompt from '@ohos.prompt';
 */
declare namespace prompt {

  /**
   * @since 8
   */
  interface ShowToastOptions {

    /**
     * Text to display.
     * @since 8
     */
    message: string;

    /**
     * Duration of toast dialog box. The default value is 1500.
     * The recommended value ranges from 1500 ms to 10000ms.
     * NOTE: A value less than 1500 is automatically changed to 1500. The maximum value is 10000 ms.
     * @since 8
     */
    duration?: number;

    /**
     * The distance between toast dialog box and the bottom of screen.
     * @since 8
     */
    bottom?: string | number;
  }

  /**
   * @since 8
   */
  interface Button {

    /**
     * @since 8
     */
    text: string;

    /**
     * @since 8
     */
    color: string;
  }

  /**
   * @since 8
   */
  interface ShowDialogSuccessResponse {

    /**
    * @since 8
    */
    index: number;
  }

  /**
   * @since 8
   */
  interface ShowDialogOptions {

    /**
     * Title of the text to display.
     * @since 8
     */
    title?: string;

    /**
     * Text body.
     * @since 8
     */
    message?: string;

    /**
     * Array of buttons in the dialog box.
     * The array structure is {text:'button', color: '#666666'}.
     * One to three buttons are supported. The first button is of the positiveButton type, the second is of the negativeButton type, and the third is of the neutralButton type.
     * @since 8
     */
    buttons?: [Button, Button?, Button?];
  }

  /**
   * @since 8
   */
  interface ActionMenuSuccessResponse {

    /**
     * @since 8
     */
    index: number;
  }

  /**
   * @since 8
   */
  interface ActionMenuOptions {

    /**
     * Title of the text to display.
     * @since 8
     */
    title?: string;

    /**
     * Array of buttons in the dialog box.
     * The array structure is {text:'button', color: '#666666'}.
     * One to six buttons are supported.
     * @since 8
     */
    buttons: [Button, Button?, Button?, Button?, Button?, Button?];
  }

  /**
   * Displays the notification text.
   * @param options Options.
   * @since 8
   */
  function showToast(options: ShowToastOptions):void;

  /**
   * Displays the dialog box.
   * @param options Options.
   * @since 8
   */
  function showDialog(options: ShowDialogOptions, callback: AsyncCallback<ShowDialogSuccessResponse>):void;
  function showDialog(options: ShowDialogOptions): Promise<ShowDialogSuccessResponse>;

  /**
   * Displays the menu.
   * @param options Options.
   * @since 8
   */
  function showActionMenu(options: ActionMenuOptions, callback: AsyncCallback<ActionMenuSuccessResponse>):void;
  function showActionMenu(options: ActionMenuOptions): Promise<ActionMenuSuccessResponse>;
}

export default prompt;
