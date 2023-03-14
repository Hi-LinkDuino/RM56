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
 * Defines the options of TextPicker.
 * @since 8
 */
declare interface TextPickerOptions {
  /**
   * Specifies the range of the text selector.
   */
  range: string[] | Resource;
  /**
   * Value of the current selection.
   */
  value?: string;
  /**
   * Current selected subscript.
   */
  selected?: number;
}

/**
 * @since 8
 */
interface TextPickerInterface {
  /**
   * Defines the TextPicker constructor.
   * @since 8
   */
  (options?: TextPickerOptions): TextPickerAttribute;
}

/**
 * Style the text selector.
 * @since 8
 */
declare class TextPickerAttribute extends CommonMethod<TextPickerAttribute> {
  /**
   * Called when the default height of the selected element is set.
   * @since 8
   */
  defaultPickerItemHeight(value: number | string): TextPickerAttribute;
  /**
   * Called when the pop-up value is returned.
   * @since 8
   */
  onAccept(callback: (value: string, index: number) => void): TextPickerAttribute;
  /**
   * Called when the Cancel button in the pop-up window is clicked.
   * @since 8
   */
  onCancel(callback: () => void): TextPickerAttribute;
  /**
   * Called when the OK button in the pop-up window is clicked.
   * @since 8
   */
  onChange(callback: (value: string, index: number) => void): TextPickerAttribute;
}

/**
 * Defines the struct of TextPickerResult.
 * @since 8
 */
declare interface TextPickerResult {
  /**
   * The currently selected value.
   * @since 8
   */
  value: string;
  /**
   * The subscript of the current selection.
   * @since 8
   */
  index: number;
}

/**
 * Defines the TextPickerDialogOptions for Text Picker Dialog.
 * @since 8
 */
declare interface TextPickerDialogOptions extends TextPickerOptions {
  /**
   * Called when the default height of the selected element is set.
   * @since 8
   */
  defaultPickerItemHeight?: number | string;
  /**
   * Called when the OK button in the dialog is clicked.
   * @since 8
   */
  onAccept?: (value: TextPickerResult) => void;
  /**
   * Called when the Cancel button in the dialog is clicked.
   * @since 8
   */
  onCancel?: () => void;
  /**
   * This event is triggered when a TextPicker text is selected in dialog.
   * @since 8
   */
  onChange?: (value: TextPickerResult) => void;
}

/**
 * Defines TextPickerDialog which uses show method to show TextPicker dialog.
 * @since 8
 */
declare class TextPickerDialog {
  /**
   * Invoking method display.
   * @since 8
   */
  static show(options?: TextPickerDialogOptions);
}

declare const TextPicker: TextPickerInterface;
declare const TextPickerInstance: TextPickerAttribute;
