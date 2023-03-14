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
 * Provides a way to control the process.
 * @since 8
 */
declare class TextTimerController {
  /**
   * constructor.
   * @since 8
   */
  constructor();

  /**
   * Provides a start event for timer.
   * @since 8
   */
  start();

  /**
   * Provides a pause event for timer.
   * @since 8
   */
  pause();

  /**
   * Provides an event to reset timer.
   * @since 8
   */
  reset();
}

/**
 * Defines the options of TextTimer.
 * @since 8
 */
interface TextTimerOptions {
  /**
   * Sets whether to countdown.The default value is false.
   * @since 8
   */
  isCountDown?: boolean;
  /**
   * Specifies the timer range.
   * In the non-countDown scenario, a negative value indicates that the timer is not limited.
   * The unit is millisecond.
   * @since 8
   */
  count?: number;

  /**
   * Controller of Texttimer.
   * @since 8
   */
  controller?: TextTimerController;
}

/**
 * Provides an interface for texttimer containers.
 * @since 8
 */
interface TextTimerInterface {
  /**
   * Defines the TextTimer constructor.
   * @since 8
   */
  (options?: TextTimerOptions): TextTimerAttribute;
}

/**
 * Defines the TextTimer attribute functions.
 * @since 8
 */
declare class TextTimerAttribute extends CommonMethod<TextTimerAttribute> {
  /**
   * Set the display time format, for example, now is hh/mm/ss/ms and current: hh-mm-ss-ms.
   * The time format string can be hh, mm, ss, or ms.
   * @since 8
   */
  format(value: string): TextTimerAttribute;
  /**
   * Called when the font color is set.
   * @since 8
   */
  fontColor(value: ResourceColor): TextTimerAttribute;
  /**
   * Called when the font size is set.
   * @since 8
   */
  fontSize(value: Length): TextTimerAttribute;
  /**
   * Called when the fontStyle is set
   * @since 8
   */
  fontStyle(value: FontStyle): TextTimerAttribute;
  /**
   * Called when the fontWeight is set
   * @since 8
   */
  fontWeight(value: number | FontWeight | string): TextTimerAttribute;
  /**
   * Called when the fontFamily is set
   * @since 8
   */
  fontFamily(value: ResourceStr): TextTimerAttribute;
  /**
   * Called when the timer value is returned.
   * @since 8
   */
  onTimer(event: (utc: number, elapsedTime: number) => void): TextTimerAttribute;
}

declare const TextTimer: TextTimerInterface;
declare const TextTimerInstance: TextTimerAttribute;
