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

#ifndef INTERFACES_CMD_COMMON_H
#define INTERFACES_CMD_COMMON_H

namespace OHOS::Camera {
/**
 * enum of cameraDevice callback
 */
enum {
    CMD_CAMERA_DEVICE_CALLBACK_ON_ERROR = 0,
    CMD_CAMERA_DEVICE_CALLBACK_ON_RESULT,
};

/**
 * enum of cameraHost callback
 */
enum {
    CMD_CAMERA_HOST_CALLBACK_ON_STATUS = 0,
    CMD_CAMERA_HOST_CALLBACK_ON_FLASHLIGHT_STATUS,
    CMD_CAMERA_HOST_CALLBACK_ON_CAMERA_EVENT,
};

/**
 * enum of streamOperator callback
 */
enum {
    CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_STARTED = 0,
    CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ENDED,
    CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ERROR,
    CMD_STREAM_OPERATOR_CALLBACK_ON_FRAME_SHUTTER,
};

/**
 * enum of cameraDevice
 */
enum {
    CMD_CAMERA_DEVICE_GET_STREAM_OPERATOR = 0,
    CMD_CAMERA_DEVICE_UPDATE_SETTINGS,
    CMD_CAMERA_DEVICE_SET_RESULT_MODE,
    CMD_CAMERA_DEVICE_GET_ENABLED_RESULTS,
    CMD_CAMERA_DEVICE_ENABLE_RESULT,
    CMD_CAMERA_DEVICE_DISABLE_RESULT,
    CMD_CAMERA_DEVICE_CLOSE,
};

/**
 * enum of cameraHost
 */
enum {
    CMD_CAMERA_HOST_SET_CALLBACK = 0,
    CMD_CAMERA_HOST_GET_CAMERAID,
    CMD_CAMERA_HOST_GET_CAMERA_ABILITY,
    CMD_CAMERA_HOST_OPEN_CAMERA,
    CMD_CAMERA_HOST_SET_FLASH_LIGHT,
};

/**
 * enum of offlineStreamOperator
 */
enum {
    CMD_OFFLINE_STREAM_OPERATOR_CANCEL_CAPTURE = 0,
    CMD_OFFLINE_STREAM_OPERATOR_RELEASE_STREAMS,
    CMD_OFFLINE_STREAM_OPERATOR_RELEASE,
};

/**
 * enum of streamOperator
 */
enum {
    CMD_STREAM_OPERATOR_IS_STREAMS_SUPPORTED = 0,
    CMD_STREAM_OPERATOR_CREATE_STREAMS,
    CMD_STREAM_OPERATOR_RELEASE_STREAMS,
    CMD_STREAM_OPERATOR_COMMIT_STREAMS,
    CMD_STREAM_OPERATOR_GET_STREAM_ATTRIBUTES,
    CMD_STREAM_OPERATOR_ATTACH_BUFFER_QUEUE,
    CMD_STREAM_OPERATOR_DETACH_BUFFER_QUEUE,
    CMD_STREAM_OPERATOR_CAPTURE,
    CMD_STREAM_OPERATOR_CANCEL_CAPTURE,
    CMD_STREAM_OPERATOR_CHANGE_TO_OFFLINE_STREAM,
};
} // namespace OHOS::Camera
#endif