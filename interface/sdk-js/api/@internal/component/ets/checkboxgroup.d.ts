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
 * CheckboxGroup SelectStatus
 * @since 8
 */
declare enum SelectStatus {
  /**
   * All checkboxs is selected.
   * @since 8
   */
  All,
  /**
   * Part of the checkbox is selected.
   * @since 8
   */
  Part,
  /**
   * None of the checkbox is selected.
   * @since 8
   */
  None,
}

/**
 * Defines the options of CheckboxGroup.
 * @since 8
 */
declare interface CheckboxGroupOptions {
  /**
   * Setting the group of CheckboxGroup.
   * @since 8
   */
  group?: string;
}

/**
 * Defines the options of CheckboxGroupResult.
 * @since 8
 */
declare interface CheckboxGroupResult {
  /**
   * Checkbox name.
   * @since 8
   */
  name: Array<string>;
  /**
   * Set the group of status.
   * @since 8
   */
  status: SelectStatus;
}

/**
 * Provides an interface for the CheckboxGroup component.
 * @since 8
 */
interface CheckboxGroupInterface {
  /**
   * Called when the CheckboxGroup component is used.
   * @since 8
   */
  (options?: CheckboxGroupOptions): CheckboxGroupAttribute;
}

/**
 * Defines the attribute functions of CheckboxGroup.
 * @since 8
 */
declare class CheckboxGroupAttribute extends CommonMethod<CheckboxGroupAttribute> {
  /**
   * setting whether all checkbox is selected.
   * @since 8
   */
  selectAll(value: boolean): CheckboxGroupAttribute;

  /**
   * setting the display color of checkbox.
   * @since 8
   */
  selectedColor(value: ResourceColor): CheckboxGroupAttribute;

  /**
   * Called when the selection status changes.
   * @since 8
   */
  onChange(callback: (event: CheckboxGroupResult) => void): CheckboxGroupAttribute;
}

declare const CheckboxGroup: CheckboxGroupInterface;
declare const CheckboxGroupInstance: CheckboxGroupAttribute;
