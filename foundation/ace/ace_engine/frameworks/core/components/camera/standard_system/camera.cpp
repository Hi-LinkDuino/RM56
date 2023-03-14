/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/camera/standard_system/camera.h"

#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <securec.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "base/log/log.h"
#include "display_type.h"
#include "window_manager.h"

#include "core/image/image_cache.h"

#ifdef ENABLE_ROSEN_BACKEND
#include "core/components/video/rosen_render_texture.h"
#endif

namespace OHOS::Ace {
namespace {

const char IS_SUCESS[] = "isSucceed";
const char ERROR_CODE[] = "errorcode";
const char PHOTO_PATH[] = "uri";
const char NULL_STRING[] = "";
const char DEFAULT_CATCH_PATH[] = "/data/media/camera/";

const char SURFACE_STRIDE_ALIGNMENT[] = "surface_stride_Alignment";
const char SURFACE_WIDTH[] = "surface_width";
const char SURFACE_HEIGHT[] = "surface_height";
const char SURFACE_FORMAT[] = "surface_format";
const char REGION_POSITION_X[] = "region_position_x";
const char REGION_POSITION_Y[] = "region_position_y";
const char REGION_WIDTH[] = "region_width";
const char REGION_HEIGHT[] = "region_height";
constexpr int32_t DEFAULT_WIDTH = 640;
#ifdef PRODUCT_RK
constexpr int32_t DEFAULT_HEIGHT = 480;
#else
constexpr int32_t DEFAULT_HEIGHT = 360;
#endif
constexpr int32_t SURFACE_STRIDE_ALIGNMENT_VAL = 8;
constexpr int32_t PREVIEW_SURFACE_WIDTH = 640;
constexpr int32_t PREVIEW_SURFACE_HEIGHT = 480;
constexpr int32_t PHOTO_SURFACE_WIDTH = 1280;
constexpr int32_t PHOTO_SURFACE_HEIGHT = 960;
constexpr int32_t MAX_DURATION = 36000;
constexpr int32_t FRAME_RATE = 30;
constexpr int32_t RATE = 48000;
#ifndef PRODUCT_RK
constexpr int32_t AUDIO_CHANNEL_COUNT = 2;
constexpr int32_t AUDIO_SAMPLE_RATE = 48000;
constexpr int32_t AUDIO_ENCODING_BITRATE = 48000;
#endif
#ifndef ENABLE_ROSEN_BACKEND
constexpr int32_t QUEUE_SIZE = 10;
#endif
constexpr double FPS = 30;
const uid_t CHOWN_OWNER_ID = -1;
const gid_t CHOWN_GROUP_ID = 1023;
const mode_t MKDIR_RWX_USR_GRP_DIR = 0770;
const mode_t MKDIR_RWX_USR_GRP_FILE = 0660;

inline bool IsDirectory(const char *dirName)
{
    struct stat statInfo {};
    if (stat(dirName, &statInfo) == 0) {
        if (statInfo.st_mode & S_IFDIR) {
            return true;
        }
    }
    return false;
}

}

void CameraCallback::PrepareCameraInput(sptr<OHOS::CameraStandard::CaptureInput> InCamInput_)
{
    camInput_ = InCamInput_;
    if (prepareEventListener_) {
        prepareEventListener_();
    }

    int32_t ret = 0;
    ret = PrepareCamera(false);
    if (ret != ERR_OK) {
        LOGE("Prepare Preview Failed");
        return;
    }
}
std::shared_ptr<Media::Recorder> CameraCallback::CreateRecorder()
{
    LOGI("Camera CreateRecorder start.");
    int ret = 0;
    Media::VideoSourceType videoSource = Media::VIDEO_SOURCE_SURFACE_ES;
#ifndef PRODUCT_RK
    Media::AudioSourceType audioSource = Media::AUDIO_MIC;
    int32_t audioSourceId = 0;
#endif
    int32_t videoSourceId = 0;
    int32_t width = DEFAULT_WIDTH;
    int32_t height = DEFAULT_HEIGHT;
    Media::VideoCodecFormat encoder = Media::H264;

    std::shared_ptr<Media::Recorder> recorder = Media::RecorderFactory::CreateRecorder();
    if ((ret = recorder->SetVideoSource(videoSource, videoSourceId)) != ERR_OK) {
        LOGE("SetVideoSource failed. ret= %{private}d.", ret);
        return nullptr;
    }
#ifndef PRODUCT_RK
    if ((ret = recorder->SetAudioSource(audioSource, audioSourceId)) != ERR_OK) {
        LOGE("SetAudioSource failed. ret= %{private}d.", ret);
        return nullptr;
    }
#endif
    if ((ret = recorder->SetOutputFormat(Media::FORMAT_MPEG_4)) != ERR_OK) {
        LOGE("SetOutputFormat failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncoder(videoSourceId, encoder)) != ERR_OK) {
        LOGE("SetVideoEncoder failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetVideoSize(videoSourceId, width, height)) != ERR_OK) {
        LOGE("SetVideoSize failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetVideoFrameRate(videoSourceId, FRAME_RATE)) != ERR_OK) {
        LOGE("SetVideoFrameRate failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetVideoEncodingBitRate(videoSourceId, RATE)) != ERR_OK) {
        LOGE("SetVideoEncodingBitRate failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetCaptureRate(videoSourceId, FPS)) != ERR_OK) {
        LOGE("SetCaptureRate failed. ret= %{private}d.", ret);
        return nullptr;
    }
#ifndef PRODUCT_RK
    if ((ret = recorder->SetAudioEncoder(audioSourceId, Media::AAC_LC)) != ERR_OK) {
        LOGE("SetAudioEncoder failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetAudioSampleRate(audioSourceId, AUDIO_SAMPLE_RATE)) != ERR_OK) {
        LOGE("SetAudioSampleRate failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetAudioChannels(audioSourceId, AUDIO_CHANNEL_COUNT)) != ERR_OK) {
        LOGE("SetAudioChannels failed. ret= %{private}d.", ret);
        return nullptr;
    }
    if ((ret = recorder->SetAudioEncodingBitRate(audioSourceId, AUDIO_ENCODING_BITRATE)) != ERR_OK) {
        LOGE("SetAudioEncodingBitRate failed. ret= %{private}d.", ret);
        return nullptr;
    }
#endif
    if ((ret = recorder->SetMaxDuration(MAX_DURATION)) != ERR_OK) { // 36000s=10h
        LOGE("SetMaxDuration failed. ret= %{private}d.", ret);
        return nullptr;
    }
    videoSourceId_ = videoSourceId;
    LOGI("Camera CreateRecorder success.");
    return recorder;
}

int CameraCallback::PrepareRecorder()
{
    LOGI("Camera PrepareRecorder.");
    if (recorder_ == nullptr) {
        recorder_ = CreateRecorder();
    }
    if (recorder_ == nullptr) {
        LOGE("Camera: Recorder not available.");
        return -1;
    }
    return ERR_OK;
}

void CameraCallback::CloseRecorder()
{
    if (videoOutput_ != nullptr) {
        ((sptr<OHOS::CameraStandard::VideoOutput> &)(videoOutput_))->Stop();
        ((sptr<OHOS::CameraStandard::VideoOutput> &)(videoOutput_))->Release();
        videoOutput_ = nullptr;
    }
    if (recorder_ != nullptr) {
        recorder_->Stop(false);
        recorder_->Release();
        recorder_ = nullptr;
    }
}

sptr<Surface> CameraCallback::createSubWindowSurface()
{
#ifdef ENABLE_ROSEN_BACKEND
    // create surface for rosen backend.
    auto renderNode = renderNode_.Upgrade();
    if (!renderNode) {
        return nullptr;
    }
    auto rosenTexture = AceType::DynamicCast<RosenRenderTexture>(renderNode);
    if (!rosenTexture) {
        return nullptr;
    }
    previewSurface_ = rosenTexture->GetSurface();
#else
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Camera:fail to get context to create Camera");
        return nullptr;
    }

    if (subwindow_ == nullptr) {
        const auto &wmi = ::OHOS::WindowManager::GetInstance();
        if (wmi == nullptr) {
            LOGE("Camera:fail to get window manager to create Camera");
            return nullptr;
        }

        auto option = ::OHOS::SubwindowOption::Get();
        option->SetWidth(windowSize_.Width());
        option->SetHeight(windowSize_.Height());
        option->SetX(windowOffset_.GetX());
        option->SetY(windowOffset_.GetY());
#ifdef PRODUCT_RK
        option->SetWindowType(SUBWINDOW_TYPE_NORMAL);
#else
        option->SetWindowType(SUBWINDOW_TYPE_VIDEO);
#endif
        auto window = wmi->GetWindowByID(context->GetWindowId());
        if (window == nullptr) {
            LOGE("Camera:fail to get window to create Camera");
            return nullptr;
        }

        auto wret = ::OHOS::WindowManager::GetInstance()->CreateSubwindow(subwindow_, window, option);
        if (wret != WM_OK) {
            LOGE("Camera:create subwindow failed, because %{public}s", WMErrorStr(wret).c_str());
            return nullptr;
        }
        subwindow_->GetSurface()->SetQueueSize(QUEUE_SIZE);
        hasMarkWhole_ = false;
        subwindow_->OnBeforeFrameSubmit([this]() {
            OnFirstBufferAvailable();
        });
    }
    previewSurface_ = subwindow_->GetSurface();
#endif

    if (previewSurface_ == nullptr) {
        LOGE("create surface fail.");
        return nullptr;
    }

    previewSurface_->SetUserData(SURFACE_STRIDE_ALIGNMENT, std::to_string(SURFACE_STRIDE_ALIGNMENT_VAL));
#ifdef PRODUCT_RK
    previewSurface_->SetUserData(SURFACE_FORMAT, std::to_string(PIXEL_FMT_RGBA_8888));
    previewSurface_->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                                 std::to_string(OHOS_CAMERA_FORMAT_RGBA_8888));
#else
    previewSurface_->SetUserData(SURFACE_FORMAT, std::to_string(PIXEL_FMT_YCRCB_420_SP));
    previewSurface_->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                                 std::to_string(OHOS_CAMERA_FORMAT_YCRCB_420_SP));
#endif
    previewSurface_->SetUserData(SURFACE_WIDTH, std::to_string(PREVIEW_SURFACE_WIDTH));
    previewSurface_->SetUserData(SURFACE_HEIGHT, std::to_string(PREVIEW_SURFACE_HEIGHT));
    previewSurface_->SetUserData(REGION_WIDTH, std::to_string(windowSize_.Width()));
    previewSurface_->SetUserData(REGION_HEIGHT, std::to_string(windowSize_.Height()));
    previewSurface_->SetUserData(REGION_POSITION_X, std::to_string(windowOffset_.GetX()));
    previewSurface_->SetUserData(REGION_POSITION_Y, std::to_string(windowOffset_.GetY()));
    return previewSurface_;
}

void CameraCallback::OnFirstBufferAvailable()
{
    if (hasMarkWhole_) {
        return;
    }
    LOGI("CameraCallback:OnFirstBufferAvailable first MarkWhole.");
    MarkWhole();
    hasMarkWhole_ = true;
}

void CameraCallback::MarkWhole()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Camera:fail to get context to create Camera");
        return;
    }

    context->SetClipHole(layoutOffset_.GetX(), layoutOffset_.GetY(), layoutSize_.Width(), layoutSize_.Height());
    auto renderNode = renderNode_.Upgrade();
    if (renderNode) {
        renderNode->SetHasSubWindow(true);
    }
    RenderNode::MarkWholeRender(renderNode_, true);
    LOGI("CameraCallback:MarkWhole success.");
}

void CameraCallback::SetLayoutOffset(double x, double y)
{
    layoutOffset_.SetX(x);
    layoutOffset_.SetY(y);
    if (hasMarkWhole_ && subwindow_) {
        LOGI("CameraCallback:Hole change  %{public}lf  %{public}lf ", x, y);
        MarkWhole();
    }
}

int32_t CameraCallback::PreparePhoto(sptr<OHOS::CameraStandard::CameraManager> camManagerObj)
{
    int32_t intResult = 0;
    captureConsumerSurface_ = Surface::CreateSurfaceAsConsumer();
    if (captureConsumerSurface_ == nullptr) {
        LOGE("Camera CreateSurface failed.");
        return -1;
    }
    captureConsumerSurface_->SetDefaultWidthAndHeight(PHOTO_SURFACE_WIDTH, PHOTO_SURFACE_HEIGHT);
    captureConsumerSurface_->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                                         std::to_string(OHOS_CAMERA_FORMAT_JPEG));
    if (photoListener_ == nullptr) {
        photoListener_ = new CaptureListener(this);
    }
    captureConsumerSurface_->RegisterConsumerListener(photoListener_);

    photoOutput_ = camManagerObj->CreatePhotoOutput(captureConsumerSurface_);
    if (photoOutput_ == nullptr) {
        LOGE("Failed to create PhotoOutput");
        return -1;
    }
    intResult = capSession_->AddOutput(photoOutput_);
    if (intResult != 0) {
        LOGE("Failed to Add output to session");
        return intResult;
    }

    std::string cacheFilePath = ImageCache::GetImageCacheFilePath();
    if (cacheFilePath.empty()) {
        cacheFilePath = DEFAULT_CATCH_PATH;
    }
    cacheFilePath_ = cacheFilePath;
    MakeDir(cacheFilePath_);
    return 0;
}

void CameraCallback::MakeDir(const std::string& path)
{
    LOGI("Camera MakeDir: %{public}s.", path.c_str());
    if (IsDirectory(path.c_str())) {
        LOGE("Camera MakeDir: It's already a directory, %{public}s.", path.c_str());
        return;
    }

    if (mkdir(path.c_str(), MKDIR_RWX_USR_GRP_DIR) != -1) {
        chown(path.c_str(), CHOWN_OWNER_ID, CHOWN_GROUP_ID);
        if (chmod(path.c_str(), MKDIR_RWX_USR_GRP_DIR) == -1) {
            LOGE("chmod failed for the newly created directory");
        }
    }
    LOGI("Camera MakeDir: success %{public}s.", path.c_str());
}

int32_t CameraCallback::PrepareVideo(sptr<OHOS::CameraStandard::CameraManager> camManagerObj)
{
    int ret = PrepareRecorder();
    if (ret != ERR_OK) {
        LOGE("Camera PrepareRecorder failed.");
        return -1;
    }

    MakeDir(DEFAULT_CATCH_PATH);
    ret = recorder_->SetOutputPath(DEFAULT_CATCH_PATH);
    if (ret != ERR_OK) {
        LOGE("Camera SetOutputPath failed. ret= %{private}d", ret);
        return -1;
    }
    ret = recorder_->Prepare();
    if (ret != ERR_OK) {
        LOGE("Prepare failed. ret= %{private}d", ret);
        return -1;
    }
    sptr<Surface> recorderSurface = (recorder_->GetSurface(videoSourceId_));
#ifdef PRODUCT_RK
    recorderSurface->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                                 std::to_string(OHOS_CAMERA_FORMAT_RGBA_8888));
#else
    recorderSurface->SetUserData(CameraStandard::CameraManager::surfaceFormat,
                                 std::to_string(OHOS_CAMERA_FORMAT_YCRCB_420_SP));
#endif
    videoOutput_ = camManagerObj->CreateVideoOutput(recorderSurface);
    if (videoOutput_ == nullptr) {
        LOGE("Create Video Output Failed");
        return -1;
    }
    int intResult = 0;
    intResult = capSession_->AddOutput(videoOutput_);
    if (intResult != 0) {
        LOGE("Failed to add Video Output");
        return -1;
    }

    return 0;
}

int32_t CameraCallback::PrepareCamera(bool bIsRecorder)
{
    if (camInput_ == nullptr) {
        LOGE("Prepare Camera: camInput is not Init Succeeded and wait.");
        return -1;
    }

#ifndef ENABLE_ROSEN_BACKEND
    if (!sizeInitSucceeded_ || !offsetInitSucceeded_) {
        LOGE("Prepare Camera: size or offset is not Init Succeeded and wait, %{public}d, %{public}d.",
            sizeInitSucceeded_, offsetInitSucceeded_);
        return -1;
    }
#endif

    int32_t intResult = 0;
    sptr<OHOS::Surface> surface = nullptr;
    LOGI("Prepare Camera start.");

    sptr<OHOS::CameraStandard::CameraManager> camManagerObj = OHOS::CameraStandard::CameraManager::GetInstance();
    if (capSession_ != nullptr) {
        LOGI("Recreating the Session");
        Stop(true);
        hasCallPreView_ = true;
    }
    capSession_ = camManagerObj->CreateCaptureSession();
    if (capSession_ == nullptr) {
        LOGE("CameraCallback: Create was not Proper, capSession is null!");
        return -1;
    }
    capSession_->BeginConfig();
    intResult = capSession_->AddInput(camInput_);
    if (intResult != 0) {
        LOGE("AddInput Failed!");
        return -1;
    }
    surface = createSubWindowSurface();
    LOGI("Preview surface width: %{public}d, height: %{public}d", surface->GetDefaultWidth(),
        surface->GetDefaultHeight());
    if (surface == nullptr) {
        LOGE("Preview surface is null");
        return -1;
    }
#ifdef PRODUCT_RK
    previewOutput_ = camManagerObj->CreateCustomPreviewOutput(surface, PREVIEW_SURFACE_WIDTH,
                                                              PREVIEW_SURFACE_HEIGHT);
#else
    previewOutput_ = camManagerObj->CreateCustomPreviewOutput(surface, surface->GetDefaultHeight(),
                                                              surface->GetDefaultWidth());
#endif
    if (previewOutput_ == nullptr) {
        LOGE("Failed to create PreviewOutput");
        return -1;
    }
    intResult = capSession_->AddOutput(previewOutput_);
    if (intResult != 0) {
        LOGE("Failed to Add Preview Output");
        return -1;
    }
    if (!bIsRecorder) {
        intResult = PreparePhoto(camManagerObj);
        if (intResult != 0) {
            LOGE("Failed to Prepare Photo");
            return -1;
        }
        PreviousReadyMode_ = ReadyMode::PHOTO;
    } else {
        intResult = PrepareVideo(camManagerObj);
        if (intResult != 0) {
            LOGE("Failed to Prepare Video");
            return -1;
        }
        PreviousReadyMode_ = ReadyMode::VIDEO;
    }
    intResult = capSession_->CommitConfig();
    if (intResult != 0) {
        LOGE("Failed to Commit config");
        return -1;
    }
    isReady_ = true;
    if (hasCallPreView_) {
        LOGE("StartPreview is already called, so initiating preview once camera is ready");
        StartPreview();
    }

    LOGI("Prepare Camera end.");
    return 0;
}

void CameraCallback::StartPreview()
{
    LOGI("CameraCallback::StartPreview() is called!");
    int intResult = 0;
    if (!isReady_) {
        LOGE("Not ready for StartPreview.");
        hasCallPreView_ = true;
        return;
    }

    LOGI("Camera StartPreview.");

    if (previewState_ == State::STATE_RUNNING) {
        LOGE("Camera is already previewing.");
        return;
    }

    intResult = capSession_->Start();
    if (intResult != 0) {
        LOGE("Camera::Start Capture Session Failed");
        return;
    }
    previewState_ = State::STATE_RUNNING;
    hasCallPreView_ = false;
    LOGI("Camera start preview succeed.");
}

void CameraCallback::StartRecord()
{
    LOGI("Camera Record start.");
    int ret = 0;
    if (recordState_ == State::STATE_RUNNING) {
        LOGE("Camera is already recording.");
        return;
    }
    LOGI("Prepare Recorder Start");
    ret = PrepareCamera(true);
    if (ret != ERR_OK) {
        LOGE("Prepare Recording Failed");
        onRecord(false, NULL_STRING);
        return;
    }

    LOGI("Video Output Start");
    ((sptr<OHOS::CameraStandard::VideoOutput> &)(videoOutput_))->Start();
    LOGI("Recorder Start");
    ret = recorder_->Start();
    if (ret != ERR_OK) {
        LOGE("recorder start failed. ret= %{private}d", ret);
        onRecord(false, NULL_STRING);
        return;
    }

    recordState_ = State::STATE_RUNNING;
    LOGI("Camera start recording succeed.");
}

void CameraCallback::Capture(Size photoSize)
{
    LOGI("Camera capture start.");
    int32_t intResult = 0;
    if (PreviousReadyMode_ != PHOTO) {
        LOGI("Photo was not prepared, so need to prepare");
        intResult = PrepareCamera(false);
        if (intResult != 0) {
            LOGE("Prepare Camera Failed");
            return;
        }
    }

    if (!isReady_ || (photoOutput_ == nullptr)) {
        LOGE("Camera is not ready for Take Photo");
        return;
    }

    intResult = ((sptr<OHOS::CameraStandard::PhotoOutput> &)(photoOutput_))->Capture();
    if (intResult != 0) {
        LOGE("Capture Photo Failed");
        return;
    }
    hasCallPhoto_ = true;
    LOGI("Camera capture end.");
}

void CameraCallback::Release()
{
    LOGI("CameraCallback: Release start.");
    if (capSession_ != nullptr) {
        capSession_->Release();
        capSession_ = nullptr;
    }
    PreviousReadyMode_ = ReadyMode::NONE;
    if (subwindow_ != nullptr) {
        LOGI("CameraCallback: Destroy subWindow.");
        subwindow_ = nullptr;
        auto context = context_.Upgrade();
        context->SetClipHole(0, 0, 0, 0);
        auto renderNode = renderNode_.Upgrade();
        if (renderNode) {
            renderNode->SetHasSubWindow(false);
        }
    }
    LOGI("CameraCallback: Release end.");
}

void CameraCallback::Stop(bool isClosePreView)
{
    CloseRecorder();
    bool isSuccess = false;
    if (recordState_ == State::STATE_RUNNING) {
        isSuccess = true;
    }

    PreviousReadyMode_ = ReadyMode::NONE;
    recordState_ = State::STATE_IDLE;
    if (isClosePreView) {
        if (capSession_ != nullptr) {
            capSession_->Stop();
        }
        previewState_ = State::STATE_IDLE;
        isReady_ = false;
        Release();
    }

    if (isSuccess) {
        onRecord(true, DEFAULT_CATCH_PATH);
    }
}

void CameraCallback::AddTakePhotoListener(TakePhotoListener&& listener)
{
    takePhotoListener_ = std::move(listener);
}

void CameraCallback::AddErrorListener(ErrorListener&& listener)
{
    onErrorListener_ = std::move(listener);
}

void CameraCallback::AddRecordListener(RecordListener&& listener)
{
    onRecordListener_ = std::move(listener);
}

void CameraCallback::onError()
{
    if (onErrorListener_) {
        onErrorListener_(NULL_STRING, NULL_STRING);
    }
}

void CameraCallback::onRecord(bool isSucces, std::string info)
{
    LOGI("CameraCallback: onRecord callback.");
    if (!onRecordListener_) {
        return;
    }

    std::map<std::string, std::string> result;
    if (isSucces) {
        result[IS_SUCESS] = "1";
        result[PHOTO_PATH] = info;
    } else {
        result[IS_SUCESS] = "0";
        result[ERROR_CODE] = info;
    }
    onRecordListener_(result);
}

void CameraCallback::OnCameraSizeChange(double width, double height)
{
    if (sizeInitSucceeded_) {
        LOGE("Camera::CameraCallback: size has Init Succeeded.");
        return;
    }

    windowSize_.SetWidth(width);
    windowSize_.SetHeight(height);
    sizeInitSucceeded_ = true;
    LOGI("CameraCallback::OnCameraSizeChange success: %{public}lf  %{public}lf.", width, height);
    PrepareCamera(false);
}

void CameraCallback::OnCameraOffsetChange(double x, double y)
{
    std::vector<struct ::OHOS::WMDisplayInfo> displays;
    ::OHOS::WindowManager::GetInstance()->GetDisplays(displays);
    if (displays.size() <= 0) {
        LOGE("WindowManager::GetDisplays return no screen.");
        return;
    }

    auto maxWidth = displays[0].width;
    auto maxHeight = displays[0].height;
    bool isIllegalOffset = (int32_t)x + (int32_t)windowSize_.Width() > maxWidth
                    || (int32_t)y + (int32_t)windowSize_.Height() > maxHeight;
    if (isIllegalOffset) {
        LOGE("CameraCallback::OnCameraOffsetChange: valid offset(%{public}lf, %{public}lf).", x, y);
        return;
    }

    windowOffset_.SetX(x);
    windowOffset_.SetY(y);

    LOGI("CameraCallback::OnCameraOffsetChange success: %{public}lf  %{public}lf.", x, y);
    if (!offsetInitSucceeded_) {
        offsetInitSucceeded_ = true;
        PrepareCamera(false);
    }

    if (subwindow_) {
        LOGI("CameraCallback: subwindow move: %{public}d  %{public}d.",
            (int32_t)windowOffset_.GetX(), (int32_t)windowOffset_.GetY());
        WMError moveRet = subwindow_->Move((int32_t)windowOffset_.GetX(), (int32_t)windowOffset_.GetY());
        if (moveRet != WM_OK) {
            LOGE("CameraCallback: subwindow move failed.");
        }
    }
}

void CaptureListener::OnBufferAvailable()
{
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage;
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;

    cameraCallback_->captureConsumerSurface_->AcquireBuffer(buffer, flushFence, timestamp, damage);
    if (buffer != nullptr) {
        char *addr = static_cast<char *>(buffer->GetVirAddr());
        uint32_t size = buffer->GetSize();
        std::string path;
        cameraCallback_->SaveData(addr, size, path);
        cameraCallback_->OnTakePhoto(true, path);
        cameraCallback_->captureConsumerSurface_->ReleaseBuffer(buffer, -1);
        cameraCallback_->hasCallPhoto_ = false;
    } else {
        LOGE("AcquireBuffer failed!");
    }
}

int32_t CameraCallback::SaveData(char *buffer, int32_t size, std::string& path)
{
    struct timeval tv = {};
    gettimeofday(&tv, nullptr);
    struct tm *ltm = localtime(&tv.tv_sec);
    if (ltm != nullptr) {
        std::ostringstream ss("Capture_");
        ss << "Capture" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec << ".jpg";
        path = cacheFilePath_ + ss.str();
        std::ofstream pic(path.c_str(), std::ofstream::out | std::ofstream::trunc);
        pic.write(buffer, size);
        pic.close();
        chown(path.c_str(), CHOWN_OWNER_ID, CHOWN_GROUP_ID);
        if (chmod(path.c_str(), MKDIR_RWX_USR_GRP_FILE) == -1) {
            LOGE("chmod failed for the newly file %{public}s.", path.c_str());
        }
    }

    return 0;
}

void CameraCallback::OnTakePhoto(bool isSucces, std::string info)
{
    LOGI("Camera:SurfaceListener OnTakePhoto %{public}d  %{public}s--", isSucces, info.c_str());
    if (!takePhotoListener_) {
        return;
    }

    std::map<std::string, std::string> result;
    if (isSucces) {
        result[IS_SUCESS] = "1";
        result[PHOTO_PATH] = info;
    } else {
        result[IS_SUCESS] = "0";
        result[ERROR_CODE] = info;
    }
    takePhotoListener_(result);
    result.clear();
}

void Camera::Release()
{
    LOGI("Camera: Release!");
    cameraCallback_.Release();
}

void Camera::CreateCamera()
{
    LOGI("Create Camera start.");
    sptr<OHOS::CameraStandard::CameraManager> camManagerObj = OHOS::CameraStandard::CameraManager::GetInstance();

    if (camManagerObj == nullptr) {
        LOGE("Get Camera Manager Failed");
        return;
    }
    std::vector<sptr<OHOS::CameraStandard::CameraInfo>> cameraObjList;
    cameraObjList = camManagerObj->GetCameras();
    if (cameraObjList.empty()) {
        LOGE("Get Cameras Failed");
        return;
    }
    camInput_ = camManagerObj->CreateCameraInput(cameraObjList[0]);
    if (camInput_ == nullptr) {
        LOGE("Create Camera Input Failed");
        return;
    }
    cameraCallback_.PrepareCameraInput(camInput_);
    LOGI("Create Camera end.");
}

void Camera::Create(const std::function<void()>& onCreate)
{
    LOGI("Camera: Create start");
    cameraCallback_.SetPipelineContext(context_);
    CreateCamera();

    if (onCreate) {
        onCreate();
    }
    LOGI("Camera:Create end");
}

void Camera::SetRenderNode(const WeakPtr<RenderNode> &renderNode)
{
    LOGI("Camera: camera set RenderNode");
    renderNode_ = renderNode;
    cameraCallback_.SetRenderNode(renderNode);
}

void Camera::Stop(bool isClosePreView)
{
    LOGI("Camera:Stop");
    cameraCallback_.Stop(isClosePreView);
}

void Camera::TakePhoto(Size photoSize)
{
    LOGI("Camera:TakePhoto");
    cameraCallback_.Capture(photoSize);
}

void Camera::StartPreview()
{
    LOGI("Camera:StartPreview");
    cameraCallback_.StartPreview();
}

void Camera::StartRecord()
{
    LOGI("Camera:StartRecord");
    cameraCallback_.StartRecord();
}

void Camera::AddTakePhotoListener(TakePhotoListener&& listener)
{
    cameraCallback_.AddTakePhotoListener(std::move(listener));
}

void Camera::AddErrorListener(ErrorListener&& listener)
{
    cameraCallback_.AddErrorListener(std::move(listener));
}

void Camera::AddRecordListener(RecordListener&& listener)
{
    cameraCallback_.AddRecordListener(std::move(listener));
}

void Camera::OnCameraSizeChange(double width, double height)
{
#ifndef ENABLE_ROSEN_BACKEND
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Camera::OnCameraSizeChange: context is null.");
        return;
    }
    float viewScale = context->GetViewScale();

    LOGI("Camera::OnCameraSizeChange:viewScale %{public}f .", viewScale);
    cameraCallback_.SetLayoutSize(width, height);
    cameraCallback_.OnCameraSizeChange(width * viewScale, height * viewScale);
#endif
}

void Camera::OnCameraOffsetChange(double x, double y)
{
#ifndef ENABLE_ROSEN_BACKEND
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Camera::OnCameraOffsetChange: context is null.");
        return;
    }
    float viewScale = context->GetViewScale();

    LOGI("Camera::OnCameraOffsetChange:viewScale %{public}f .", viewScale);
    cameraCallback_.SetLayoutOffset(x, y);
    cameraCallback_.OnCameraOffsetChange(x * viewScale, y * viewScale);
#endif
}
} // namespace OHOS::Ace
