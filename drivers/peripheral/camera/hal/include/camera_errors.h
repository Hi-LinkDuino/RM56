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
 * @file camera_errors.h
 *
 * @brief Declares the <b>camera_errors</b> class to define errors that may occur during camera operations.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef CAMERA_ERRORS_H
#define CAMERA_ERRORS_H

#include <cstdint>


namespace OHOS::Camera {
constexpr int MODULE_CAMERA = 1;

using ErrCode = int32_t;

/**
 * @brief Generates a start error code with a unique identifier based on specified subsystem and module bit numbers.
 *
 * @param subsystem Indicates the subsystem bit number.
 * @param module Indicates the module bit number.
 * @return
 * @since 1.0
 * @version 1.0
 */

constexpr int32_t BASE_CAMERA_ERR_OFFSET = ErrCodeOffset(SUBSYS_CAMERA, MODULE_CAMERA);

/** Status error */
const int32_t  ERR_ILLEGAL_STATE = BASE_CAMERA_ERR_OFFSET;

/** Invalid parameter */
const int32_t  ERR_INVALID_PARAM = BASE_CAMERA_ERR_OFFSET + 1;

/** Null Ptr */
const int32_t  ERR_NULL_PTR = BASE_CAMERA_ERR_OFFSET + 2;

/** Alloc Fail */
const int32_t  ERR_MEM_ALLOC_FAIL = BASE_CAMERA_ERR_OFFSET + 3;

/** Invalid operation */
const int32_t  ERR_INVALID_OPERATION = BASE_CAMERA_ERR_OFFSET + 4;

/** Device Unexist */
const int32_t  ERR_DEVICE_UNEXIST = BASE_CAMERA_ERR_OFFSET + 5;

/** Device Busy */
const int32_t  ERR_DEVICE_BUSY = BASE_CAMERA_ERR_OFFSET + 6;

/**  Device Connect Fail */
const int32_t  ERR_DEVICE_CONNECT_FAIL = BASE_CAMERA_ERR_OFFSET + 7;

/**  Process Fail */
const int32_t  ERR_PROCESS_FAIL = BASE_CAMERA_ERR_OFFSET + 8;

/**  Wait TimeOut */
const int32_t  ERR_WAIT_TIMEOUT = BASE_CAMERA_ERR_OFFSET + 9;

/**  Config TimeOut */
const int32_t  ERR_CFG_TIMEOUT = BASE_CAMERA_ERR_OFFSET + 10;

/**  Invalid Pattern */
const int32_t  ERR_INVALID_PATTERN = BASE_CAMERA_ERR_OFFSET + 11;

/**  Invalid Camera Id */
const int32_t  ERR_INVALID_CAMERA_ID = BASE_CAMERA_ERR_OFFSET + 12;

/** Unknown error */
const int32_t  ERR_UNKNOWN = BASE_CAMERA_ERR_OFFSET + 200;
}  // namespace OHOS::Camera
#endif  // CAMERA_ERRORS_H
