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

import {Callback} from './basic';

/**
 * @syscap SystemCapability.ArkUI.ArkUI.Full
 * @since 8
 * @import router from '@ohos.router';
 */
declare namespace router {

  /**
   * @since 8
   */
  interface RouterOptions {

    /**
     * URI of the destination page, which supports the following formats:
     * 1. Absolute path of the page, which is provided by the pages list in the config.json file.
     *    Example:
     *      pages/index/index
     *      pages/detail/detail
     * 2. Particular path. If the URI is a slash (/), the home page is displayed.
     * @syscap SystemCapability.ArkUI.ArkUI.Lite
     * @since 8
     */
    url: string;

    /**
     * Data that needs to be passed to the destination page during navigation.
     * After the destination page is displayed, the parameter can be directly used for the page.
     * For example, this.data1 (data1 is the key value of the params used for page navigation.)
     * @syscap SystemCapability.ArkUI.ArkUI.Lite
     * @since 8
     */
    params?: Object;
  }

  /**
   * @since 8
   */
  interface RouterState {

    /**
     * Index of the current page in the stack.
     * NOTE: The index starts from 1 from the bottom to the top of the stack.
     * @since 8
     */
    index: number;

    /**
     * Name of the current page, that is, the file name.
     * @since 8
     */
    name: string;

    /**
     * Path of the current page.
     * @since 8
     */
    path: string;
  }

  /**
   * @since 8
   */
  interface EnableAlertOptions {

    /**
     * dialog context.
     * @since 8
     */
    message: string;
  }

  /**
   * Navigates to a specified page in the application based on the page URL and parameters.
   * @param options Options.
   * @since 8
   */
  function push(options: RouterOptions):void;

  /**
   * Replaces the current page with another one in the application. The current page is destroyed after replacement.
   * @param options Options.
   * @since 8
   */
  function replace(options: RouterOptions):void;

  /**
   * Returns to the previous page or a specified page.
   * @param options Options.
   * @since 8
   */
  function back(options?: RouterOptions ):void;

  /**
   * Clears all historical pages and retains only the current page at the top of the stack.
   * @since 8
   */
  function clear():void;

  /**
   * Obtains the number of pages in the current stack.
   * @returns Number of pages in the stack. The maximum value is 32.
   * @since 8
   */
  function getLength():string;

  /**
   * Obtains information about the current page state.
   * @returns Page state.
   * @since 8
   */
  function getState():RouterState;

  /**
   * Pop up dialog to ask whether to back
   * @param options Options.
   * @since 8
   */
  function enableAlertBeforeBackPage(options: EnableAlertOptions):void;

  /**
   * cancel enableAlertBeforeBackPage
   * @since 8
   */
  function disableAlertBeforeBackPage():void;

  /**
   * Obtains information about the current page params.
   * @returns Page params.
   * @since 8
   */
  function getParams(): Object;
}

export default router;
