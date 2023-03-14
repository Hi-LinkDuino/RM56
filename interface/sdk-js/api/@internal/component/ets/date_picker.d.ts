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
 * Defines the struct of DatePickerResult.
 * @since 8
 */
declare interface DatePickerResult {
  /**
   * Application year
   * @since 8
   */
  year?: number;

  /**
   * Application month
   * @since 8
   */
  month?: number;

  /**
   * Application day
   * @since 8
   */
  day?: number;
}

/**
 * Defines the options of DatePicker.
 * @since 8
 */
declare interface DatePickerOptions {
  /**
   * Specifies the start date of the date selector.
   * @since 8
   */
  start?: Date;
  /**
   * Specifies the end date for the date selector.
   * @since 8
   */
  end?: Date;

  /**
   * Specifies the date selector check date or time selector check time.
   * @since 8
   */
  selected?: Date;
}

/**
 * Defines the DatePicker Component.
 * @since 8
 */
interface DatePickerInterface {
  /**
   * Defines the DatePicker constructor.
   * @since 8
   */
  (options?: DatePickerOptions): DatePickerAttribute;
}

/**
 * Defines the DatePicker attribute functions.
 * @since 8
 */
declare class DatePickerAttribute extends CommonMethod<DatePickerAttribute> {
  /**
   * Date selector: true: displays the lunar calendar. false: The lunar calendar is not displayed.
   * @since 8
   */
  lunar(value: boolean): DatePickerAttribute;

  /**
   * This event is triggered when a DatePicker date or time is selected.
   * @since 8
   */
  onChange(callback: (value: DatePickerResult) => void): DatePickerAttribute;
}

/**
 * Defines the DatePickerDialogOptions for Data Picker Dialog.
 * @since 8
 */
declare interface DatePickerDialogOptions extends DatePickerOptions {
  /**
   * Date selector: true: displays the lunar calendar. false: The lunar calendar is not displayed.
   * @since 8
   */
  lunar?: boolean;

  /**
   * Called when the OK button in the dialog is clicked.
   * @since 8
   */
  onAccept?: (value: DatePickerResult) => void;
  /**
   * Called when the Cancel button in the dialog is clicked.
   * @since 8
   */
  onCancel?: () => void;
  /**
   * This event is triggered when a DatePicker date or time is selected in dialog.
   * @since 8
   */
  onChange?: (value: DatePickerResult) => void;
}

/**
 * Defines DatePickerDialog which uses show method to show DatePicker dialog.
 * @since 8
 */
declare class DatePickerDialog {
  /**
   * Invoking method display.
   * @since 8
   */
  static show(options?: DatePickerDialogOptions);
}

declare const DatePicker: DatePickerInterface;
declare const DatePickerInstance: DatePickerAttribute;
