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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_H

#include <list>
#include "core/components/camera/camera_component.h"
#include "core/components/common/layout/constants.h"

#include "recorder.h"
#include "window_manager.h"
#include "input/camera_manager.h"
namespace OHOS::Ace {

using TakePhotoListener = std::function<void(const std::map<std::string, std::string>&)>;
using ErrorListener = std::function<void(const std::string&, const std::string&)>;
using RecordListener = std::function<void(const std::map<std::string, std::string>&)>;

constexpr int32_t TEMPORARY_WINDOW_SIZE = 480;

enum State : int32_t {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_BUTT
};

enum ReadyMode : int32_t {
    NONE,
    PHOTO,
    VIDEO
};

class CaptureListener;

class CameraCallback {
public:
    ACE_DISALLOW_COPY_AND_MOVE(CameraCallback);
    using PrepareEventListener = std::function<void()>;

    CameraCallback()
    {}

    ~CameraCallback()
    {
        photoListener_ = nullptr;
        if (captureConsumerSurface_ != nullptr) {
            captureConsumerSurface_->UnregisterConsumerListener();
        }
        Stop(true);
    }

    void SetPipelineContext(const WeakPtr<PipelineContext>& context)
    {
        context_ = context;
    }

    void SetRenderNode(const WeakPtr<RenderNode> &renderNode)
    {
        renderNode_ = renderNode;
    }
    void Stop(bool isClosePreView);
    void Release();
    void Capture(Size photoSize);
    void StartPreview();
    void StartRecord();
    void PrepareCameraInput(sptr<OHOS::CameraStandard::CaptureInput> InCamInput_);
    sptr<Surface> createSubWindowSurface();
    int32_t PreparePhoto(sptr<OHOS::CameraStandard::CameraManager> camManagerObj);
    int32_t PrepareVideo(sptr<OHOS::CameraStandard::CameraManager> camManagerObj);
    int32_t PrepareCamera(bool bIsRecorder);
    int32_t SaveData(char *buffer, int32_t size, std::string& path);
    void OnTakePhoto(bool isSucces, std::string info);
    void AddTakePhotoListener(TakePhotoListener&& listener);
    void AddErrorListener(ErrorListener&& listener);
    void AddRecordListener(RecordListener&& listener);
    void SetCatchFilePath(std::string cacheFilePath);
    void OnCameraSizeChange(double width, double height);
    void OnCameraOffsetChange(double x, double y);
    void MarkWhole();
    void OnFirstBufferAvailable();
    void SetLayoutOffset(double x, double y);
    void SetLayoutSize(double width, double height)
    {
        layoutSize_.SetWidth(width);
        layoutSize_.SetHeight(height);
    }
    void AddPrepareEventListener(PrepareEventListener&& listener)
    {
        prepareEventListener_ = std::move(listener);
    }
    friend class CaptureListener;

    bool hasMarkWhole_ = false;

protected:
    void CloseRecorder();

private:
    int PrepareRecorder();
    std::shared_ptr<Media::Recorder> CreateRecorder();
    void onError();
    void onRecord(bool isSucces, std::string info);
    void MakeDir(const std::string& path);

    std::shared_ptr<Media::Recorder> recorder_ = nullptr;
    int32_t videoSourceId_ = -1;
    int32_t PreviousReadyMode_ = ReadyMode::NONE;
    sptr<Surface> previewSurface_;
    Size layoutSize_;
    Offset layoutOffset_;

    ErrorListener onErrorListener_;
    RecordListener onRecordListener_;
    int32_t recordFileId_ = -1;
    std::string recordPath_;

    ::OHOS::sptr<::OHOS::Subwindow> subwindow_ = nullptr;
    WeakPtr<RenderNode> renderNode_;
    WeakPtr<PipelineContext> context_;
    sptr<Surface> captureSurface_;
    Size windowSize_ = Size(TEMPORARY_WINDOW_SIZE, TEMPORARY_WINDOW_SIZE);
    Offset windowOffset_ = Offset(0, 0);
    bool sizeInitSucceeded_ = false;
    bool offsetInitSucceeded_ = false;

    bool isReady_ = false;
    bool hasCallPreView_ = false;
    bool hasCallPhoto_ = false;
    State previewState_ = State::STATE_IDLE;
    State recordState_ = State::STATE_IDLE;
    sptr<OHOS::CameraStandard::CaptureSession> capSession_;
    sptr<OHOS::CameraStandard::CaptureInput> camInput_;
    sptr<IBufferConsumerListener> photoListener_;
    sptr<OHOS::CameraStandard::CaptureOutput> photoOutput_;
    sptr<OHOS::CameraStandard::CaptureOutput> previewOutput_;
    sptr<OHOS::CameraStandard::CaptureOutput> videoOutput_;
    std::string cacheFilePath_;
    TakePhotoListener takePhotoListener_;
    sptr<Surface> captureConsumerSurface_;
    PrepareEventListener prepareEventListener_;
};

class CaptureListener : public IBufferConsumerListener {
public:
    explicit CaptureListener(CameraCallback *cameraCallback) : cameraCallback_(cameraCallback) {}
    ~CaptureListener()
    {
        cameraCallback_->photoListener_ = nullptr;
        cameraCallback_ = nullptr;
    }
    void OnBufferAvailable() override;

private:
    CameraCallback *cameraCallback_;
};

class Camera : public virtual AceType {
    DECLARE_ACE_TYPE(Camera, AceType);

public:
    using PrepareEventListener = std::function<void()>;

    explicit Camera(const WeakPtr<PipelineContext> &context)
        : context_(context) {}
    ~Camera() override = default;

    void Release();
    void Create(const std::function<void()>& onCreate);
    void SetRenderNode(const WeakPtr<RenderNode> &renderNode);

    void Stop(bool isClosePreView);
    void TakePhoto(Size photoSize);
    void StartPreview();
    void StartRecord();

    void OnCameraSizeChange(double width, double height);
    void OnCameraOffsetChange(double x, double y);
    void AddPrepareEventListener(PrepareEventListener&& listener)
    {
        cameraCallback_.AddPrepareEventListener(std::move(listener));
    }

    void AddTakePhotoListener(TakePhotoListener&& listener);
    void AddErrorListener(ErrorListener&& listener);
    void AddRecordListener(RecordListener&& listener);

private:
    void CreateCamera();
    WeakPtr<PipelineContext> context_;
    WeakPtr<RenderNode> renderNode_;

    TakePhotoListener takePhotoListener_;
    ErrorListener onErrorListener_;
    RecordListener onRecordListener_;

    CameraCallback cameraCallback_;
    std::unique_ptr<OHOS::Window> window_;
    sptr<OHOS::CameraStandard::CaptureInput> camInput_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CAMERA_CAMERA_H
