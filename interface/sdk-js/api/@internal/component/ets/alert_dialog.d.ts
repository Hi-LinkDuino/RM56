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
 * The alignment of dialog,
 * @since 7
 */
declare enum DialogAlignment {
  /**
   * Vertical top alignment.
   * @since 7
   */
  Top,

  /**
   * Align vertically to the center.
   * @since 7
   */
  Center,

  /**
   * Vertical bottom alignment.
   * @since 7
   */
  Bottom,

  /**
   * Default alignment.
   * @since 7
   */
  Default,

  /**
   * Align the upper left corner.
   * @since 8
   */
  TopStart,

  /**
   * Align the upper right corner.
   * @since 8
   */
  TopEnd,

  /**
   * Left center alignment.
   * @since 8
   */
  CenterStart,

  /**
   * Right center alignment.
   * @since 8
   */
  CenterEnd,

  /**
   * Align the lower left corner.
   * @since 8
   */
  BottomStart,

  /**
   * Align the lower right corner.
   * @since 8
   */
  BottomEnd,
}

/**
 * Base param used for AlertDialog.show method.
 * @since 7
 */
declare interface AlertDialogParam {
  /**
   * Title Properties
   * @since 7
   */
  title?: ResourceStr;

  /**
   * message Properties
   * @since 7
   */
  message: ResourceStr;

  /**
   * Allows users to click the mask layer to exit.
   * @since 7
   */
  autoCancel?: boolean;

  /**
   * Execute Cancel Function.
   * @since 7
   */
  cancel?: () => void;

  /**
   * Alignment in the vertical direction.
   * @since 7
   */
  alignment?: DialogAlignment;

  /**
   * Offset of the pop-up window relative to the alignment position.
   * @since 7
   */
  offset?: Offset;

  /**
   * Grid count of dialog.
   * @since 7
   */
  gridCount?: number;
}

/**
 * Defines the AlertDialog with confirm button.
 * @since 7
 */
declare interface AlertDialogParamWithConfirm extends AlertDialogParam {
  /**
   * Invoke the commit function.
   * @since 7
   */
  confirm?: {
    /**
     * Text content of the confirmation button.
     * @since 7
     */
    value: ResourceStr;

    /**
     * Text color of the confirmation button.
     * @since 7
     */
    fontColor?: ResourceColor;

    /**
     * Background color of the confirmation button.
     * @since 7
     */
    backgroundColor?: ResourceColor;

    /**
     * Method executed by the callback.
     * @since 7
     */
    action: () => void;
  };
}

/**
 * Defines the dialog param with buttons.
 * @since 7
 */
declare interface AlertDialogParamWithButtons extends AlertDialogParam {
  /**
   * First button.
   * @since 7
   */
  primaryButton: {
    /**
     * Text content of the confirmation button.
     * @since 7
     */
    value: ResourceStr;

    /**
     * Text color of the confirmation button.
     * @since 7
     */
    fontColor?: ResourceColor;

    /**
     * Background color of the confirmation button.
     * @since 7
     */
    backgroundColor?: ResourceColor;

    /**
     * Method executed by the callback.
     * @since 7
     */
    action: () => void;
  };

  /**
   * Second button.
   * @since 7
   */
  secondaryButton: {
    /**
     * Text content of the confirmation button.
     * @since 7
     */
    value: ResourceStr;

    /**
     * Text color of the confirmation button.
     * @since 7
     */
    fontColor?: ResourceColor;

    /**
     * Background color of the confirmation button.
     * @since 7
     */
    backgroundColor?: ResourceColor;

    /**
     * Method executed by the callback.
     * @since 7
     */
    action: () => void;
  };
}

/**
 * Defines AlertDialog which uses show method to show alert dialog.
 * @since 7
 */
declare class AlertDialog {
  /**
   * Invoking method display.
   * @since 7
   */
  static show(value: AlertDialogParamWithConfirm | AlertDialogParamWithButtons);
}
