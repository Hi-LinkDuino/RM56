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
 * Defines the badge position property.
 * @since 7
 */
declare enum BadgePosition {
  /**
   * The dot is displayed vertically centered on the right.
   * @since 7
   */
  RightTop,

  /**
   * Dots are displayed in the upper right corner.
   * @since 7
   */
  Right,

  /**
   * The dot is displayed in the left vertical center.
   * @since 7
   */
  Left,
}

/**
 * BadgeStyle object
 * @since 7
 */
declare interface BadgeStyle {
  /**
   * Text Color
   * @since 7
   */
  color?: ResourceColor;

  /**
   * Text size.
   * @since 7
   */
  fontSize?: number | string;

  /**
   * Size of a badge.
   * @since 7
   */
  badgeSize?: number | string;

  /**
   * Color of the badge.
   * @since 7
   */
  badgeColor?: ResourceColor;
}

/**
 * Defines the base param of badge.
 * @since 7
 */
declare interface BadgeParam {
  /**
   * Set the display position of the prompt point.
   * @since 7
   */
  position?: BadgePosition;

  /**
   * Defines the style of the Badge component, including the text color, size, dot color, and size.
   * @since 7
   */
  style: BadgeStyle;
}

/**
 * Defines the badge param with count and maxCount.
 * @since 7
 */
declare interface BadgeParamWithNumber extends BadgeParam {
  /**
   * Set the number of reminder messages.
   * @since 7
   */
  count: number;

  /**
   * Maximum number of messages. If the number of messages exceeds the maximum, only maxCount+ is displayed.
   * @since 7
   */
  maxCount?: number;
}

/**
 * Defines the badge param with string value.
 * @since 7
 */
declare interface BadgeParamWithString extends BadgeParam {
  /**
   * Text string of the prompt content.
   * @since 7
   */
  value: string;
}

/**
 * Defines Badge Componrnt.
 * @since 7
 */
interface BadgeInterface {
  /**
   * position: Set the display position of the prompt point.
   * maxCount: Maximum number of messages. If the number of messages exceeds the maximum, only maxCount+ is displayed.
   * count: Set the number of reminder messages.
   * style: You can set the style of the Badge component, including the text color, size, dot color, and size.
   * @since 7
   */
  (value: BadgeParamWithNumber): BadgeAttribute;

  /**
   * value: Text string of the prompt content.
   * position: Set the display position of the prompt point.
   * maxCount: Maximum number of messages. If the number of messages exceeds the maximum, only maxCount+ is displayed.
   * style: You can set the style of the Badge component, including the text color, size, dot color, and size.
   * @since 7
   */
  (value: BadgeParamWithString): BadgeAttribute;
}

/**
 * Defines Badge Componrnt attribute.
 * @since 7
 */
declare class BadgeAttribute extends CommonMethod<BadgeAttribute> {}

declare const Badge: BadgeInterface;
declare const BadgeInstance: BadgeAttribute;
