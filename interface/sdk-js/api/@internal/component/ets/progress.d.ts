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
 * Defines the option of Progress.
 * @since 7
 */
 declare interface ProgressOptions {
  /**
   * Sets the value of Progress.
   * @since 7
   */
  value: number;

  /**
   * Sets the total of Progress.
   * @since 7
   */
  total?: number;

  /**
   * Sets the style of Progress.
   * @since 7
   * @deprecated since 8
   */
  style?: ProgressStyle

  /**
   * Sets the type of Progress.
   * @since 8
   */
  type?: ProgressType
}

/**
 * Type of progress bar
 * @since 8
 */
declare enum ProgressType {
  /**
   * Linear progress bar style.
   * @since 8
   */
  Linear,

  /**
   * Ring progress bar.
   * @since 8
   */
  Ring,

  /**
   * Eclipse progress bar.
   * @since 8
   */
  Eclipse,

  /**
   * ScaleRing progress bar.
   * @since 8
   */
  ScaleRing,

  /**
   * Capsule progress bar.
   * @since 8
   */
  Capsule,
}

/**
 * Defines style options for progress component.
 * @since 8
 */
declare interface ProgressStyleOptions {
  /**
   * Defines the strokeWidth property.
   * @since 8
   */
  strokeWidth?: Length;

  /**
   * Defines the scaleCoun property.
   * @since 8
   */
  scaleCount?: number;

  /**
   * Defines the scaleWidth property.
   * @since 8
   */
  scaleWidth?: Length;
}

/**
 * Type of progress bar
 * @since 7
 */
declare enum ProgressStyle {
  /**
   * Linear progress bar style.
   * @since 7
   */
  Linear,

  /**
   * Ring progress bar.
   * @since 8
   */
  Ring,

  /**
   * Eclipse progress bar.
   * @since 7
   */
  Eclipse,

  /**
   * ScaleRing progress bar.
   * @since 8
   */
  ScaleRing,

  /**
   * Capsule progress bar.
   * @since 8
   */
  Capsule,
}

/**
 * Provides the progress bar interface.
 * @since 7
 */
interface ProgressInterface {
  /**
   * Called when the progress bar is set.
   * @since 7
   */
  (options: ProgressOptions): ProgressAttribute;
}

/**
 * Defines the progress attibute functions.
 * @since 7
 */
declare class ProgressAttribute extends CommonMethod<ProgressAttribute> {
  /**
   * Called when the current progress value is set.
   * @since 7
   */
  value(value: number): ProgressAttribute;

  /**
   * Called when the progress bar foreground is set.
   * @since 7
   */
  color(value: ResourceColor): ProgressAttribute;

  /**
   * Called when the style of progress bar is set.
   * @since 8
   */
  style(value: ProgressStyleOptions): ProgressAttribute;
}

declare const Progress: ProgressInterface;
declare const ProgressInstance: ProgressAttribute;
