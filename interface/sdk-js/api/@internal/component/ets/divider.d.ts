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
 * Provides a divider component to separate different content blocks/content elements.
 * @since 7
 */
interface DividerInterface {
  /**
   * Return Divider.
   * @since 7
   */
  (): DividerAttribute;
}

/**
 * Defines the Divider attribute functions.
 * @since 7
 */
declare class DividerAttribute extends CommonMethod<DividerAttribute> {
  /**
   * Indicates whether to use a horizontal splitter or a vertical splitter.
   * The options are as follows: false: horizontal splitter; true: vertical splitter.
   * @since 7
   */
  vertical(value: boolean): DividerAttribute;

  /**
   * Sets the color of the divider line.
   * @since 7
   */
  color(value: ResourceColor): DividerAttribute;

  /**
   * Sets the width of the dividing line.
   * @since 7
   */
  strokeWidth(value: number | string): DividerAttribute;

  /**
   * Sets the end style of the split line. The default value is Butt.
   * @since 7
   */
  lineCap(value: LineCapStyle): DividerAttribute;
}

declare const Divider: DividerInterface;
declare const DividerInstance: DividerAttribute;
