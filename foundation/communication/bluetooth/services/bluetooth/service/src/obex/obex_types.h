/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OBEX_TYPES_H
#define OBEX_TYPES_H

#include <cstdint>

namespace OHOS {
namespace bluetooth {
#define OBEX_LOG_DEBUG(fmt, ...) LOG_DEBUG("[OBEX]:" fmt, ##__VA_ARGS__)
#define OBEX_LOG_INFO(fmt, ...) LOG_INFO("[OBEX]:" fmt, ##__VA_ARGS__)
#define OBEX_LOG_ERROR(fmt, ...) LOG_ERROR("[OBEX]:" fmt, ##__VA_ARGS__)

const uint8_t OBEX_VERSION = 0x10;
const uint8_t OBEX_FINAL_BIT_MASK = 0x80;
// Min, Max and default transport MTU
const uint16_t OBEX_DEFAULT_MTU = 1024;
const uint16_t OBEX_MINIMUM_MTU = 255;
const uint16_t OBEX_MAXIMUM_MTU = 65535;
// The value of 0xffffffff indicates a timeout of infinity
const uint32_t OBEX_SESSION_MAX_TIMEOUT_SEC = 0xFFFFFFFF;
// backup a level before applying name (equivalent to ../ on many systems)
const uint8_t OBEX_SETPATH_BACKUP = 0x01;
// Don’t create folder if it does not exist, return an error instead.
const uint8_t OBEX_SETPATH_NOCREATE = 0x02;

enum class ObexOpeId : uint8_t {
    CONNECT = 0x80,
    DISCONNECT = 0x81,
    PUT = 0x02,
    PUT_FINAL = 0x82,
    GET = 0x03,
    GET_FINAL = 0x83,
    SETPATH = 0x85,
    ACTION = 0x86,
    SESSION = 0x87,
    ABORT = 0xFF
};
enum class ObexRspCode : uint8_t {
    CONTINUE = 0x90,                    // 100 Continue
    SUCCESS = 0xA0,                     // 200 OK, Success
    CREATED = 0xA1,                     // 201 Created
    ACCEPTED = 0xA2,                    // 202 Accepted
    NON_AUTH = 0xA3,                    // 203 Non-Authoritative Information
    NO_CONTENT = 0xA4,                  // 204 No Content
    RESET_CONTENT = 0xA5,               // 205 Reset Content
    PARTIAL_CONTENT = 0xA6,             // 206 Partial Content
    MULTIPLE_CHOICES = 0xB0,            // 300 Multiple Choices
    MOVED_PERMANENTLY = 0xB1,           // 301 Moved Permanently
    MOVED_TEMPORARILY = 0xB2,           // 302 Moved temporarily
    SEE_OTHER = 0xB3,                   // 303 See Other
    NOT_MODIFIED = 0xB4,                // 304 Not modified
    USE_PROXY = 0xB5,                   // 305 Use Proxy
    BAD_REQUEST = 0xC0,                 // 400 Bad Request - server couldn’t understand request
    UNAUTHORIZED = 0xC1,                // 401 Unauthorized
    PAYMENT_REQUIRED = 0xC2,            // 402 Payment required
    FORBIDDEN = 0xC3,                   // 403 Forbidden - operation is understood but refused
    NOT_FOUND = 0xC4,                   // 404 Not Found
    METHOD_NOT_ALLOWED = 0xC5,          // 405 Method not allowed
    NOT_ACCEPTABLE = 0xC6,              // 406 Not Acceptable
    PROXY_AUTH_REQUIRED = 0xC7,         // 407 Proxy Authentication required
    REQUEST_TIME_OUT = 0xC8,            // 408 Request Time Out
    CONFLICT = 0xC9,                    // 409 Conflict
    GONE = 0xCA,                        // 410 Gone
    LENGTH_REQUIRED = 0xCB,             // 411 Length Required
    PRECONDITION_FAILED = 0xCC,         // 412 Precondition failed
    REQUESTED_ENTITY_TOO_LARGE = 0xCD,  // 413 Requested entity too large
    REQUEST_URL_TOO_LARGE = 0xCE,       // 414 Request URL too large
    UNSUPPORTED_MEDIA_TYPE = 0xCF,      // 415 Unsupported media type
    INTERNAL_SERVER_ERROR = 0xD0,       // 500 Internal Server Error
    NOT_IMPLEMENTED = 0xD1,             // 501 Not Implemented
    BAD_GATEWAY = 0xD2,                 // 502 Bad Gateway
    SERVICE_UNAVAILABLE = 0xD3,         // 503 Service Unavailable
    GATEWAY_TIMEOUT = 0xD4,             // 504 Gateway Timeout
    HTTP_VERSION_NOT_SUPPORTED = 0xD5,  // 505 HTTP version not supported
    DATABASE_FULL = 0xE0,               // Database Full
    DATABASE_LOCKED = 0xE1              // Database Locked
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_TYPES_H
