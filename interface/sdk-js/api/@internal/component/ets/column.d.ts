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
 * Defines the Column Component.
 * @since 7
 */
interface ColumnInterface {
  /**
   * Set the value.
   * useAlign:Use a custom alignment.
   * space: Vertical layout element spacing.
   * @since 7
   */
  (value?: { space?: string | number }): ColumnAttribute;
}

/**
 * Defines the Column component attribute functions.
 * @since 7
 */
declare class ColumnAttribute extends CommonMethod<ColumnAttribute> {
  /**
   * Sets the alignment format of the subassembly in the horizontal direction.
   * @since 7
   */
  alignItems(value: HorizontalAlign): ColumnAttribute;

  /**
   * Sets the alignment format of the subassembly in the vertical direction.
   * @since 8
   */
  justifyContent(value: FlexAlign): ColumnAttribute;
}

declare const Column: ColumnInterface;
declare const ColumnInstance: ColumnAttribute;
