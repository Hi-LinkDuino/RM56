/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * Contains rawFile descriptor information.
 * @name Contains rawFile descriptor information
 * @since 8
 * @syscap SystemCapability.Global.ResourceManager
 *
 */
 export interface RawFileDescriptor {
  /**
   * rawfile descriptor
   *
   * @since 8
   */
  fd: number;

  /**
   * the offset from where the raw file starts in the HAP
   *
   * @since 8
   */
  offset: number;

  /**
   * the length of the raw file in the HAP
   *
   * @since 8
   */
  length: number;
}
