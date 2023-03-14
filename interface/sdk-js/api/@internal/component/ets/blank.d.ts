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
 * Create Blank.
 * @since 7
 */
interface BlankInterface {
  /**
   * The minimum size of the blank fill assembly on the container spindle.
   * @since 7
   */
  (min?: number | string): BlankAttribute;
}

/**
 * Inheritance CommonMethod Set Styles
 * @since 7
 */
declare class BlankAttribute extends CommonMethod<BlankAttribute> {
  /**
   * color: set color.
   * @since 7
   */
  color(value: ResourceColor): BlankAttribute;
}

declare const Blank: BlankInterface;
declare const BlankInstance: BlankAttribute;
