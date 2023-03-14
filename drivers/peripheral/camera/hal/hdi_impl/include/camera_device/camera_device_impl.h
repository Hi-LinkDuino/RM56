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

#ifndef CAMERA_DEVICE_CAMERA_DEVICE_IMPL_H
#define CAMERA_DEVICE_CAMERA_DEVICE_IMPL_H

#include "camera_device.h"
#include "camera.h"
#include "camera_metadata_info.h"
#include "stream_operator.h"
#include <mutex>

namespace OHOS::Camera {
class IPipelineCore;
class CameraDeviceImpl : public CameraDevice, public std::enable_shared_from_this<CameraDeviceImpl> {
public:
    CameraDeviceImpl(const std::string &cameraId,
        const std::shared_ptr<IPipelineCore> &pipelineCore);
    CameraDeviceImpl() = default;
    virtual ~CameraDeviceImpl() = default;
    CameraDeviceImpl(const CameraDeviceImpl& other) = delete;
    CameraDeviceImpl(CameraDeviceImpl &&other) = delete;
    CameraDeviceImpl& operator=(const CameraDeviceImpl &other) = delete;
    CameraDeviceImpl& operator=(CameraDeviceImpl &&other) = delete;

public:
    virtual CamRetCode GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback> &callback,
        OHOS::sptr<IStreamOperator> &streamOperator) override;
    virtual CamRetCode UpdateSettings(const std::shared_ptr<CameraSetting> &settings) override;
    virtual CamRetCode SetResultMode(const ResultCallbackMode &mode) override;
    virtual CamRetCode GetEnabledResults(std::vector<MetaType> &results) override;
    virtual CamRetCode EnableResult(const std::vector<MetaType> &results) override;
    virtual CamRetCode DisableResult(const std::vector<MetaType> &results) override;
    virtual void Close() override;

    virtual std::shared_ptr<IPipelineCore> GetPipelineCore() const override;
    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraDeviceCallback> &callback) override;
    virtual ResultCallbackMode GetMetaResultMode() const override;
    /* RC_OK: metadata changed；RC_ERROR: metadata unchanged； */
    virtual RetCode GetMetadataResults(std::shared_ptr<CameraStandard::CameraMetadata> &metadata) override;
    virtual void ResultMetadata() override;
    virtual void GetCameraId(std::string &cameraId) const override;
    virtual bool IsOpened() const override;
    virtual void SetStatus(bool isOpened) override;
    void OnRequestTimeout();

protected:
    virtual void OnMetadataChanged(const std::shared_ptr<CameraStandard::CameraMetadata> &metadata) override;
    virtual void OnDevStatusErr() override;

private:
    RetCode GetEnabledFromCfg();
    bool CompareTagData(const camera_metadata_item_t &baseEntry,
        const camera_metadata_item_t &newEntry);
    RetCode UpdataMetadataResultsBase();
    uint64_t GetCurrentLocalTimeStamp();

private:
    bool isOpened_;
    std::string cameraId_;
    std::shared_ptr<IPipelineCore> pipelineCore_;
    OHOS::sptr<ICameraDeviceCallback> cameraDeciceCallback_;
    OHOS::sptr<IStreamOperatorCallback> spCameraDeciceCallback_;
    OHOS::sptr<StreamOperator> spStreamOperator_;
    ResultCallbackMode metaResultMode_;
    std::vector<MetaType> deviceMetaTypes_;
    std::mutex enabledRstMutex_;
    std::vector<MetaType> enabledResults_;
    std::shared_ptr<CameraStandard::CameraMetadata> metadataResultsBase_;
    std::mutex metaRstMutex_;
    std::shared_ptr<CameraStandard::CameraMetadata> metadataResults_;

    // to keep OHOS::sptr<IStreamOperator> alive
    OHOS::sptr<IStreamOperator> ismOperator_ = nullptr;
};
} // end namespace OHOS::Camera
#endif // CAMERA_DEVICE_CAMERA_DEVICE_IMPL_H
