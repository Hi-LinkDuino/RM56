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
 * @import import fetch from '@system.fetch';
 * @since 3
 * @syscap SystemCapability.Communication.NetStack
 */
export interface FetchResponse {
  /**
   * Server status code.
   * @since 3
   */
  code: number;

  /**
   * Data returned by the success function.
   * @since 3
   */
  data: string | object;

  /**
   * All headers in the response from the server.
   * @since 3
   */
  headers: Object;
}

/**
 * @import import fetch from '@system.fetch';
 * @since 3
 * @syscap SystemCapability.Communication.NetStack
 */
export default class Fetch {
  /**
   * Obtains data through the network.
   * @param options
   */
  static fetch(options: {
    /**
     * Resource URL.
     * @since 3
     */
    url: string;

    /**
     * Request parameter, which can be of the string type or a JSON object.
     * @since 3
     */
    data?: string | object;

    /**
     * Request header, which accommodates all attributes of the request.
     * @since 3
     */
    header?: Object;

    /**
     * Request methods available: OPTIONS, GET, HEAD, POST, PUT, DELETE and TRACE. The default value is GET.
     * @since 3
     */
    method?: string;

    /**
     * The return type can be text, or JSON. By default, the return type is determined based on Content-Type in the header returned by the server.
     * @since 3
     */
    responseType?: string;

    /**
     * Called when the network data is obtained successfully.
     * @since 3
     */
    success?: (data: FetchResponse) => void;

    /**
     * Called when the network data fails to be obtained.
     * @since 3
     */
    fail?: (data: any, code: number) => void;

    /**
     * Called when the execution is completed.
     * @since 3
     */
    complete?: () => void;
  }): void;
}
