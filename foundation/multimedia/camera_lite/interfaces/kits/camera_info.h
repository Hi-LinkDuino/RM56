/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
 * @addtogroup MultiMedia_CameraInfo
 * @{
 *
 * @brief Defines the camera info set for applications to achieve and access the info.
 *
 * @since 2.0
 * @version 2.0
 */

/**
 * @file camera_info.h
 *
 * @brief Declares functions of the <b>CameraInfo</b> class.
 *
 *
 * @since 2.0
 * @version 2.0
 */

#ifndef OHOS_CAMERA_INFO_H
#define OHOS_CAMERA_INFO_H

#include "media_log.h"
#include "meta_data.h"

namespace OHOS {
namespace Media {
typedef enum {
    /* Wide angle camera */
    WIDE_ANGLE,
    /* Fisheye camera */
    FISH_EYE,
    /* Depth camera */
    TRUE_DEPTH,
    /* Other camera */
    OTHER_TYPE
} CameraType;

typedef enum {
    /* front */
    CAMERA_FACING_FRONT,
    /* back */
    CAMERA_FACING_BACK,
    /* other */
    CAMERA_FACING_OTHERS
} FacingType;

class CameraInfo {
public:
    /**
     * @brief A constructor used to create a <b>CameraInfo</b> instance.
     *
     */
    CameraInfo() = default;

    /**
     * @brief A destructor used to delete the <b>CameraInfo</b> instance.
     *
     */
    virtual ~CameraInfo() = default;

    /**
     * @brief Obtains the camera type.
     *
     * @return Returns the caemra type, whick can be {@link CameraType}.
     */
    virtual int32_t GetCameraType() const
    {
        return -1;
    }

    /**
     * @brief Obtains the camera type.
     *
     * @return Returns the caemra type, whick can be {@link FacingType}.
     */
    virtual int32_t GetCameraFacingType() const
    {
        return -1;
    }
};
} // namespace Media
} // namespace OHOS
#endif // OHOS_CAMERA_INFO_H
