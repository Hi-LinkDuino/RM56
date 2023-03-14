/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @since 3
 */
export interface LocaleResponse {
  /**
   * Current language of the application. Example: zh.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  language: string;

  /**
   * Country or region. Example: CN.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  countryOrRegion: string;

  /**
   * Text layout direction. Available values are as follows:
   * ltr: The text direction is from left to right.
   * rtl: The text direction is from right to left.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  dir: "ltr" | "rtl";
}

/**
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @since 3
 */
export default class Configuration {
  /**
   * Obtains the current locale of the application, which is the same as the system locale.
   * @syscap SystemCapability.ArkUI.ArkUI.Lite
   * @since 3
   */
  static getLocale(): LocaleResponse;
}
