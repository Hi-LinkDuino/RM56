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


/**
 * Defines the options of CustomDialogController.
 * @since 7
 */
declare interface CustomDialogControllerOptions {
  /**
   * Custom builder function.
   * @since 7
   */
  builder: any;

  /**
   * Defines the cancel function.
   * @since 7
   */
  cancel?: () => void;

  /**
   * Defines if use auto cancel when click on the outside of the dialog.
   * @since 7
   */
  autoCancel?: boolean;

  /**
   * Defines the dialog alignment of the screen.
   * @since 7
   */
  alignment?: DialogAlignment;

  /**
   * Defines the dialog offset.
   * @since 7
   */
  offset?: Offset;

  /**
   * Defines if use costom style.
   * @since 7
   */
  customStyle?: boolean;

  /**
   * Grid count of dialog.
   * @since 8
   */
  gridCount?: number;
}

/**
 * Use the CustomDialogController class to display the custom pop-up window.
 * @since 7
 */
declare class CustomDialogController {
  /**
   * The constructor transfers parameter settings.
   * @since 7
   */
  constructor(value: CustomDialogControllerOptions);

  /**
   * Display the content of the customized pop-up window. If the content has been displayed, it does not take effect.
   * @since 7
   */
  open();

  /**
   * Closes the custom pop-up window. If the window is closed, the window does not take effect.
   * @since 7
   */
  close();
}
