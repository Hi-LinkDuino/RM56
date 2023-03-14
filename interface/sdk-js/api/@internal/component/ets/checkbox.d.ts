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
 * Defines the options of Checkbox.
 * @since 8
 */
declare interface CheckboxOptions {
  /**
   * Current name of Checkbox.
   * @since 8
   */
  name?: string;

  /**
   * Sets the group of Checkbox.
   * @since 8
   */
  group?: string;
}

/**
 * Provides an interface for the Checkbox component.
 * @since 8
 */
interface CheckboxInterface {
  /**
   * Construct the Checkbox component.
   * Called when the Checkbox component is used.
   * @since 8
   */
  (options?: CheckboxOptions): CheckboxAttribute;
}

/**
 * Defines the attribute functions of Checkbox.
 * @since 8
 */
declare class CheckboxAttribute extends CommonMethod<CheckboxAttribute> {
  /**
   * setting whether checkbox is selected.
   * @since 8
   */
  select(value: boolean): CheckboxAttribute;

  /**
   * setting the display color of checkbox.
   * @since 8
   */
  selectedColor(value: ResourceColor): CheckboxAttribute;

  /**
   * Called when the selection status changes.
   * @since 8
   */
  onChange(callback: (value: boolean) => void): CheckboxAttribute;
}

declare const Checkbox: CheckboxInterface;
declare const CheckboxInstance: CheckboxAttribute;
