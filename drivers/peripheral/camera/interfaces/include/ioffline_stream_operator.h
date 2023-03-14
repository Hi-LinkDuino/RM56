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
 * @file ioffline_stream_operator.h
 *
 * @brief Declares APIs for offline stream operations.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_OFFLINE_STREAM_OPERATOR_CLIENT_INF_H
#define HDI_OFFLINE_STREAM_OPERATOR_CLIENT_INF_H

#include <list>
#include <map>
#include <vector>
#include "types.h"
#include "icamera_interface.h"

namespace OHOS::Camera {
class IOfflineStreamOperator : public ICameraInterface {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Camera.V1_0.OfflineStreamOperator");

    virtual ~IOfflineStreamOperator() {}

    /**
     * @brief Cancels a capture request.
     *
     * @param captureId Indicates the ID of the capture request to cancel.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode CancelCapture(int captureId) = 0;

    /**
     * @brief Releases offline streams.
     *
     * @param streamIds Indicates the IDs of the offline streams to release.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode ReleaseStreams(const std::vector<int> &streamIds) = 0;

    /**
     * @brief Releases all offline streams.
     *
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful; returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode Release() = 0;
};
}
#endif // HDI_OFFLINE_STREAM_OPERATOR_CLIENT_INF_H