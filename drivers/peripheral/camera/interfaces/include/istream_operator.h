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
 * @file istream_operator.h
 *
 * @brief Declares APIs for stream operations.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_STREAM_OPERATOR_CLIENT_INF_H
#define HDI_STREAM_OPERATOR_CLIENT_INF_H

#include <list>
#include <map>
#include <vector>
#include "istream_operator_callback.h"
#include "ioffline_stream_operator.h"
#include "types.h"
#include "icamera_interface.h"

namespace OHOS::Camera {
class IBufferProducer;
class IStreamOperator : public ICameraInterface {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.Camera.V1_0.StreamOperator");

    virtual ~IStreamOperator() {}

    /**
     * @brief Checks whether a specific stream can be dynamically created.
     *
     * A stream is a sequence of data elements output from a bottom-layer device, processed by the current module,
     * and then transmitted to an upper-layer service or application.
     * The current module supports preview streams, video streams, photographing streams, and the like.
     * For details, see {@link StreamIntent}.
     *
     * This function is used to check whether a stream or streams can be dynamically created based on the
     * operation mode, configuration information, and existing streams in the current module.
     * If the streams can be created without stopping the existing streams or making the upper-layer service or
     * application unaware of the stopping of the existing streams,
     * <b>type</b> is set to <b>DYNAMIC_SUPPORTED</b> so that the upper-layer service or application
     * can directly add the new stream.
     * If the streams can be created only after the upper-layer service or application stops capturing all streams,
     * <b>type</b> is set to <b>RE_CONFIGURED_REQUIRED</b>.
     * If the streams are not supported, <b>type</b> is set to <b>NOT_SUPPORTED</b>.
     * This function must be called prior to {@link CreateStreams}.
     *
     * @param mode Indicates the operation mode of the streams. For details, see {@link OperationMode}.
     * @param modeSetting Indicates the stream configuration parameters, including the frame rate and 3A.
     * 3A stands for automatic focus (AF), automatic exposure (AE), and automatic white-balance (AWB).
     * @param info Indicates the stream configuration information. For details, see {@link StreamInfo}.
     * @param type Indicates the support type of the dynamically created stream.
     * The supported types are defined in {@link StreamSupportType}.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode IsStreamsSupported(
        OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
        const std::vector<std::shared_ptr<StreamInfo>> &info,
        StreamSupportType &type) = 0;

    /**
     * @brief Creates streams.
     *
     * Before calling this function, you must use {@link IsStreamsSupported} to check whether the hardware
     * abstraction layer (HAL) supports the streams to create.
     *
     * @param streamInfos Indicates the list of stream information, which is defined by {@link StreamInfo}.
     * The passed stream information may be changed. Therefore, you can run {@link GetStreamAttributes} to
     * obtain the latest stream attributes after the stream is created.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos) = 0;

    /**
     * @brief Releases streams.
     *
     * @param streamIds Indicates the IDs of the streams to release.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode ReleaseStreams(const std::vector<int> &streamIds) = 0;

    /**
     * @brief Configures a stream.
     *
     * This function must be called after {@link CreateStreams}.
     *
     * @param mode Indicates the operation mode of the stream. For details, see {@link OperationMode}.
     * @param modeSetting Indicates the stream configuration parameters, including the frame rate and zoom information.
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode CommitStreams(OperationMode mode,
        const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting) = 0;

    /**
     * @brief Obtains stream attributes.
     *
     * @param attributes Indicates the obtained stream attributes.
     * Stream information passed by the <b>streamInfos</b> parameter in {@link CreateStreams}
     * may be overridden. Therefore, the stream attributes obtained by using this function may be
     * different from the stream information passed in {@link CreateStreams}.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode GetStreamAttributes(
        std::vector<std::shared_ptr<StreamAttribute>> &attributes) = 0;

    /**
     * @brief Attaches a producer handle to a stream.
     *
     * If a producer handle has been specified during stream creation (by {@link CreateStreams}),
     * you do not need to call this function. If you want to attach a new procedure handle,
     * call {@link DetachBufferQueue} to detach the existing procedure handle and then
     * call this function to attach the new one.
     * You do not need to attach a procedure handle for IoT devices that do not support or require
     * image data caching and transferring of preview streams.
     * In this case, set <b>bufferQueue_</b> in the {@link StreamInfo} parameter of {@link CreateStreams}
     * to <b>null</b>,
     * and set <b>tunneledMode_</b> to <b>false</b>.
     *
     * @param streamId Indicates the ID of the stream to which the procedure handle is to be attached.
     * @param producer Indicates the producer handle to be attached.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer> &producer) = 0;

    /**
     * @brief Detaches the producer handle from a stream.
     *
     * @param streamId Indicates the ID of the stream from which the procedure handle is to be detached.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode DetachBufferQueue(int streamId) = 0;

    /**
     * @brief Captures an image.
     *
     * This function must be called after {@link CommitStreams}.
     * There are two image capture modes: continuous capture and single capture.
     * A continuous capture is performed inside the module after being triggered,
     * and the consumer can continuously receive captured image data after calling this function only once.
     * If this function is called again, the current capture is stopped, the capture request configuration is updated,
     * and a new capture is performed.
     * This mode is mainly used in preview, video recording, or continuous shooting scenarios.
     * After a single capture is triggered, only one frame of image data is captured.
     * This mode applies to the single shooting scenario.
     * When the capture is started, {@link OnCaptureStarted} is called to notify of the start.
     * To stop a continuous capture, call {@link CancelCapture}.
     * When the capture ends, {@link OnCaptureEnded} is called to notify the caller of the information
     * such as the number of captured frames.
     * <b>enableShutterCallback_</b> in {@link CaptureInfo} is used to enable the callback for each capture.
     * After the callback is enabled, {@link OnFrameShutter} is called upon each capture.
     * In the scenario where multiple streams are captured at the same time, this module ensures that
     * the captured data of multiple streams is reported simultaneously.
     *
     * @param captureId Indicates the ID of the capture request. You must ensure that the ID of the
     * capture request is unique when the camera is started.
     * @param info Indicates the capture request configuration information. For details, see {@link CaptureInfo}.
     * @param isStreaming Specifies whether to continuously capture images.
     * The value <b>true</b> means to continuously capture images, and <b>false</b> means the opposite.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode Capture(int captureId,
        const std::shared_ptr<CaptureInfo> &info,  bool isStreaming) = 0;

    /**
     * @brief Cancels a capture.
     *
     * {@link OnCaptureEnded} is called when a continuous capture is canceled.
     *
     * @param captureId Indicates the ID of the capture request to cancel.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode CancelCapture(int captureId) = 0;

    /**
     * @brief Converts a specific stream to an offline stream.
     *
     * Only photographing streams can be converted into offline streams.
     * Due to the limited processing capability, some devices may spend a long period of time on
     * algorithm processing during photographing, causing the capture requests to stack in the module.
     * Converting to an offline stream enables the bottom-layer device to close and
     * the offline stream to take over for subsequent processing.
     *
     * @param streamIds Indicates the IDs of the streams to be converted.
     * @param callback Indicates the callback for conversion to offline streams.
     * @param offlineOperator Indicates the offline stream after conversion.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful;
     * returns an error code defined in {@link CamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual CamRetCode ChangeToOfflineStream(const std::vector<int> &streamIds,
        OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IOfflineStreamOperator> &offlineOperator) = 0;
};
}
#endif // HDI_STREAM_OPERATOR_CLIENT_INF_H