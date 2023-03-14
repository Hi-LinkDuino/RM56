/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

import {AsyncCallback, Callback} from "./basic";

/**
 * Provides http related APIs.
 *
 * @since 6
 * @syscap SystemCapability.Communication.NetStack
 */
declare namespace http {
  /**
   * Creates an HTTP request task.
   */
  function createHttp(): HttpRequest;

  export interface HttpRequestOptions {
    /**
     * Request method.
     */
    method?: RequestMethod; // default is GET
    /**
     * Additional data of the request.
     * extraData can be a string or an Object (API 6) or an ArrayBuffer(API 8).
     */
    extraData?: string | Object | ArrayBuffer;
    /**
     * HTTP request header.
     */
    header?: Object; // default is 'content-type': 'application/json'
    /**
     * Read timeout period. The default value is 60,000, in ms.
     */
    readTimeout?: number; // default is 60s
    /**
     * Connection timeout interval. The default value is 60,000, in ms.
     */
    connectTimeout?: number; // default is 60s.
  }

  export interface HttpRequest {
    /**
     * Initiates an HTTP request to a given URL.
     *
     * @param url URL for initiating an HTTP request.
     * @param options Optional parameters {@link HttpRequestOptions}.
     * @param callback Returns {@link HttpResponse}.
     * @permission ohos.permission.INTERNET
     */
    request(url: string, callback: AsyncCallback<HttpResponse>): void;
    request(url: string, options: HttpRequestOptions, callback: AsyncCallback<HttpResponse>): void;
    request(url: string, options?: HttpRequestOptions): Promise<HttpResponse>;

    /**
     * Destroys an HTTP request.
     */
    destroy(): void;

    /**
     * Registers an observer for HTTP Response Header events.
     *
     * @deprecated use on_headersReceive instead since 8.
     */
    on(type: "headerReceive", callback: AsyncCallback<Object>): void;

    /**
     * Unregisters the observer for HTTP Response Header events.
     *
     * @deprecated use off_headersReceive instead since 8.
     */
    off(type: "headerReceive", callback?: AsyncCallback<Object>): void;

    /**
     * Registers an observer for HTTP Response Header events.
     *
     * @since 8
     */
    on(type: "headersReceive", callback: Callback<Object>): void;

    /**
     * Unregisters the observer for HTTP Response Header events.
     *
     * @since 8
     */
    off(type: "headersReceive", callback?: Callback<Object>): void;

    /**
     * Registers a one-time observer for HTTP Response Header events.
     *
     * @since 8
     */
    once(type: "headersReceive", callback: Callback<Object>): void;
  }

  export enum RequestMethod {
    OPTIONS = "OPTIONS",
    GET = "GET",
    HEAD = "HEAD",
    POST = "POST",
    PUT = "PUT",
    DELETE = "DELETE",
    TRACE = "TRACE",
    CONNECT = "CONNECT"
  }

  export enum ResponseCode {
    OK = 200,
    CREATED,
    ACCEPTED,
    NOT_AUTHORITATIVE,
    NO_CONTENT,
    RESET,
    PARTIAL,
    MULT_CHOICE = 300,
    MOVED_PERM,
    MOVED_TEMP,
    SEE_OTHER,
    NOT_MODIFIED,
    USE_PROXY,
    BAD_REQUEST = 400,
    UNAUTHORIZED,
    PAYMENT_REQUIRED,
    FORBIDDEN,
    NOT_FOUND,
    BAD_METHOD,
    NOT_ACCEPTABLE,
    PROXY_AUTH,
    CLIENT_TIMEOUT,
    CONFLICT,
    GONE,
    LENGTH_REQUIRED,
    PRECON_FAILED,
    ENTITY_TOO_LARGE,
    REQ_TOO_LONG,
    UNSUPPORTED_TYPE,
    INTERNAL_ERROR = 500,
    NOT_IMPLEMENTED,
    BAD_GATEWAY,
    UNAVAILABLE,
    GATEWAY_TIMEOUT,
    VERSION
  }

  export interface HttpResponse {
    /**
     * result can be a string (API 6) or an ArrayBuffer(API 8). Object is deprecated from API 8.
     */
    result: string | Object | ArrayBuffer;
    /**
     * Server status code.
     */
    responseCode: ResponseCode | number;
    /**
     * All headers in the response from the server.
     */
    header: Object;
    /**
     * @since 8
     */
    cookies: string;
  }
}

export default http;