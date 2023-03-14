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
 * Defines the basic callback.
 * @since 6
 */
export interface Callback<T> {
  /**
   * Defines the callback info.
   * @since 6
   */
  (data: T): void;
}

/**
 * Defines the basic error callback.
 * @since 6
 */
export interface ErrorCallback<T extends Error = BusinessError> {
  /**
   * Defines the basic error callback.
   * @since 6
   */
  (err: T): void;
}

/**
 * Defines the basic async callback.
 * @since 6
 */
export interface AsyncCallback<T> {
  /**
   * Defines the callback data.
   * @since 6
   */
  (err: BusinessError, data: T): void;
}

/**
 * Defines the error interface.
 * @since 6
 */
export interface BusinessError extends Error {
  /**
   * Defines the basic error code.
   * @since 6
   */
  code: number;
}
