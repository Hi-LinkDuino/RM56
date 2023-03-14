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

#ifndef CAMERA_TYPE_H
#define CAMERA_TYPE_H

#include <string>

namespace OHOS {
namespace Media {
typedef enum {
    CAMERA_SERVER_GET_CAMERA_ABILITY,
    CAMERA_SERVER_GET_CAMERA_INFO,
    CAMERA_SERVER_GET_CAMERAIDLIST,
    CAMERA_SERVER_CREATE_CAMERA,
    CAMERA_SERVER_CLOSE_CAMERA,
    CAEMRA_SERVER_SET_CAMERA_CONFIG,
    CAMERA_SERVER_TRIGGER_SINGLE_CAPTURE,
    CAMERA_SERVER_TRIGGER_LOOPING_CAPTURE,
    CAMERA_SERVER_STOP_LOOPING_CAPTURE,
    CAMERA_SERVER_SET_CAMERA_CALLBACK,
    CAMERA_SERVER_GET_CAMERA_MODE_NUM,
    CAMERA_SERVER_SET_CAMERA_MODE_NUM,
} CameraServerCall;

typedef enum {
    ON_CAMERA_STATUS_CHANGE,
    ON_CAMERA_CONFIGURED,
    ON_TRIGGER_SINGLE_CAPTURE_FINISHED,
    ON_TRIGGER_LOOPING_CAPTURE_FINISHED
} CameraServerback;

struct CallBackPara {
    int funcId;
    std::string cameraId;
    void* data;
    void* frameConfig;
    void* cameraConfig;
};
const char SERVICE_NAME[] = "CameraServer";
static const int32_t DEFAULT_IPC_SIZE = 300;
} // namespace Media
} // namespace OHOS
#endif
