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
* Provides interfaces to generate system logs.
*
* @since 7
* @syscap SystemCapability.HiviewDFX.HiLog
*/

declare namespace hilog {
  /**
   * Outputs debug-level logs.
   *
   * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF
   * @param tag Identifies the log tag.
   * @param format Indicates the log format string.
   * @param args Indicates the log parameters.
   * @since 7
   * @syscap SystemCapability.HiviewDFX.HiLog
   */
  function debug(domain: number, tag: string, format: string, ...args: any[]) : void;
  /**
   * Outputs info-level logs.
   *
   * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF
   * @param tag Identifies the log tag.
   * @param format Indicates the log format string.
   * @param args Indicates the log parameters.
   * @since 7
   * @syscap SystemCapability.HiviewDFX.HiLog
   */
  function info(domain: number, tag: string, format: string, ...args: any[]) : void;
  /**
   * Outputs warning-level logs.
   *
   * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF
   * @param tag Identifies the log tag.
   * @param format Indicates the log format string.
   * @param args Indicates the log parameters.
   * @since 7
   * @syscap SystemCapability.HiviewDFX.HiLog
   */
  function warn(domain: number, tag: string, format: string, ...args: any[]) : void;
  /**
   * Outputs error-level logs.
   *
   * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF
   * @param tag Identifies the log tag.
   * @param format Indicates the log format string.
   * @param args Indicates the log parameters.
   * @since 7
   * @syscap SystemCapability.HiviewDFX.HiLog
   */
  function error(domain: number, tag: string, format: string, ...args: any[]) : void;
  /**
   * Outputs fatal-level logs.
   *
   * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF
   * @param tag Identifies the log tag.
   * @param format Indicates the log format string.
   * @param args Indicates the log parameters.
   * @since 7
   * @syscap SystemCapability.HiviewDFX.HiLog
   */
  function fatal(domain: number, tag: string, format: string, ...args: any[]) : void;
  /**
   * Checks whether logs of the specified tag, and level can be printed.
   *
   * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF
   * @param tag Identifies the log tag.
   * @param level log level
   * @since 7
   * @syscap SystemCapability.HiviewDFX.HiLog
   */
  function isLoggable(domain: number, tag: string, level: LogLevel) : boolean;
  /**
   * Log level define
   *
   * @since 7
   * @syscap SystemCapability.HiviewDFX.HiLog
   */
  enum LogLevel {
      DEBUG = 3,
      INFO = 4,
      WARN = 5,
      ERROR = 6,
      FATAL = 7,
  }
}

export default hilog;