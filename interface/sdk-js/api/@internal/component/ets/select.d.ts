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
 * The declare of slectOption.
 * @since 8
 */
declare interface SelectOption {
  /**
   * option string.
   * @since 8
   */
  value: ResourceStr;

  /**
   * option icon.
   * @since 8
   */
  icon?: ResourceStr;
}

/**
 * Provides the select interface.
 * @since 8
 */
interface SelectInterface {
  /**
   * Called when the select is set.
   * @since 8
   */
  (options: Array<SelectOption>): SelectAttribute;
}

/**
 * The commonMethod of select.
 * @since 8
 */
declare class SelectAttribute extends CommonMethod<SelectAttribute> {
  /**
   * Sets the serial number of the select item, starting from 0.
   * @since 8
   */
  selected(value: number): SelectAttribute;

  /**
   * Sets the text display of the select button itself.
   * @since 8
   */
  value(value: string): SelectAttribute;

  /**
   * Sets the text properties of the select button itself.
   * @since 8
   */
  font(value: Font): SelectAttribute;

  /**
   * Sets the text color of the select button itself.
   * @since 8
   */
  fontColor(value: ResourceColor): SelectAttribute;

  /**
   * Sets the background color of the selected items in the select.
   * @since 8
   */
  selectedOptionBgColor(value: ResourceColor): SelectAttribute;

  /**
   * Sets the text style of the selected items in the select.
   * @since 8
   */
  selectedOptionFont(value: Font): SelectAttribute;

  /**
   * Sets the text color of the selected item in the select.
   * @since 8
   */
  selectedOptionFontColor(value: ResourceColor): SelectAttribute;

  /**
   * Sets the background color of the select item.
   * @since 8
   */
  optionBgColor(value: ResourceColor): SelectAttribute;

  /**
   * Sets the text style for select items.
   * @since 8
   */
  optionFont(value: Font): SelectAttribute;

  /**
   * Sets the text color for select items.
   * @since 8
   */
  optionFontColor(value: ResourceColor): SelectAttribute;

  /**
   * Callback for selecting an item from the select.
   * @since 8
   */
  onSelect(callback: (index: number, value?: string) => void): SelectAttribute;
}

declare const Select: SelectInterface;
declare const SelectInstance: SelectAttribute;
