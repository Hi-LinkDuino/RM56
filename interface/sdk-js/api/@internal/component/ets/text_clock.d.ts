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
 * Provides a way to control the textclock status.
 * @since 8
 */
declare class TextClockController {
  /**
   * constructor.
   * @since 8
   */
  constructor();
  /**
   * Provides a start event for textclock.
   * @since 8
   */
  start();
  /**
   * Provides a stop event for textclock.
   * @since 8
   */
  stop();
}

/**
 * TextClock component, which provides the text clock capability.
 * @since 8
 */
interface TextClockInterface {
  /**
   * Construct the text clock component.
   * Specifies the current time zone.
   * The valid value is an integer ranging from - 14 to 12,
   * Where a negative value indicates the eastern time zone, for example, -8.
   * @since 8
   */
  (options?: { timeZoneOffset?: number; controller?: TextClockController }): TextClockAttribute;
}

declare class TextClockAttribute extends CommonMethod<TextClockAttribute> {
  /**
   * set display time format,such as "yyyy/mm/dd","yyyy-mm-dd".
   * support time format：yyyy,mm,mmm(English month abbreviation),mmmm(Full name of the month in English),
   * dd,ddd(English Week abbreviation),dddd(Full name of the week in English),
   * HH/hh(24-hour clock/12-hour clock),MM/mm(minute),SS/ss(second).
   * @since 8
   */
  format(value: string): TextClockAttribute;
  /**
   * Provides a date change callback.
   * The callback parameter is Unix Time Stamp,
   * The number of milliseconds that have elapsed since January 1, 1970 (UTC).
   * The minimum callback interval for this event is seconds.
   * You can listen to this callback,
   * Use the format attribute method to customize data display in the callback.
   * @param event Listening date event callback.
   * @since 8
   */
  onDateChange(event: (value: number) => void): TextClockAttribute;
  /**
   * Called when the value of TextClock fontColor is set
   * @since 8
   */
  fontColor(value: ResourceColor): TextClockAttribute;
  /**
   * Called when the value of TextClock fontSize is set
   * @since 8
   */
  fontSize(value: Length): TextClockAttribute;
  /**
   * Called when the value of TextClock fontStyle is set
   * @since 8
   */
  fontStyle(value: FontStyle): TextClockAttribute;
  /**
   * Called when the value of TextClock fontWeight is set
   * @since 8
   */
  fontWeight(value: number | FontWeight | string): TextClockAttribute;
  /**
   * Called when the value of TextClock fontFamily is set
   * @since 8
   */
  fontFamily(value: ResourceStr): TextClockAttribute;
}

declare const TextClock: TextClockInterface;
declare const TextClockInstance: TextClockAttribute;
