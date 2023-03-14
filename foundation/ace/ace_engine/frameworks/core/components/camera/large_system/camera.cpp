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

#include "core/components/camera/large_system/camera.h"

#include <sstream>
#include "base/log/log.h"

namespace OHOS::Ace {
namespace {

const char CAMERA_PARAM_TEXTURE[] = "texture";
const char CAMERA_OEPN_CAMERA[] = "openCamera";
const char CAMERA_TAKE_PHOTO[] = "takePhoto";
const char CAMERA_ON_TAKE_PHOTO[] = "onTakePhoto";
const char DEVICE_POSITION[] = "devicePosition";
const char CACHE_FILE_PATH[] = "cacheFilePath";
const char CAMERA_FLASH_MODE[] = "flashMode";
const char CAMERA_SET_FLASH_MODE[] = "setFlashMode";
const char CAMERA_PERMISSION[] = "cameraPermission";
const char CAMERA_BUFFER_WIDTH[] = "bufferWidth";
const char CAMERA_BUFFER_HEIGHT[] = "bufferHeight";
const char ERROR_MSG[] = "errormsg";
const char ERROR_CODE[] = "errorcode";
const char CAMERA_GRANT_RESULT[] = "grantResult";
const char ORIENTATION[] = "orientation";
const char CAMERA_PREVIEW_WIDTH[] = "preViewSizeWidth";
const char CAMERA_PREVIEW_HEIGHT[] = "preViewSizeHeight";
const char CAMERA_SET_PREVIEW_SIZE[] = "setPreViewSize";
const char CAMERA_QUALITY[] = "quality";
const char CAMERA_ON_PREPARE[] = "onPrepare";
const char CAMERA_CHANGE_COMPONENT_ID_FUNCTION[] = "changeCameraCompenentId";
const char CAMERA_CHANGE_COMPONENT_ID[] = "compenentId";
const char CAMERA_CHANGE_TYPE[] = "changeType";

}

void Camera::Release(const std::function<void(bool)>& onRelease)
{
    UnRegisterEvent();
    Resource::Release(onRelease);
}

void Camera::Create(const std::function<void(int64_t)>& onCreate)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context to create Camera");
        return;
    }

    auto platformTaskExecutor =
        SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

    platformTaskExecutor.PostTask([weak = WeakClaim(this), onCreate] {
        auto camera = weak.Upgrade();
        if (camera) {
            camera->CreateCamera(onCreate);
        }
    });
}

void Camera::CreateCamera(const std::function<void(int64_t)>& onCreate)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    if (!resRegister) {
        LOGE("fail to get platform resRegister");
        return;
    }

    std::stringstream paramStream;
    paramStream << CAMERA_PARAM_TEXTURE << PARAM_EQUALS << textureId_ << PARAM_AND << CAMERA_BUFFER_WIDTH
                << PARAM_EQUALS << bufferSize_.Width() << PARAM_AND << CAMERA_BUFFER_HEIGHT << PARAM_EQUALS
                << bufferSize_.Height();
    std::string param = paramStream.str();
    id_ = resRegister->CreateResource(type_, param);
    if (id_ == INVALID_ID) {
        LOGE("Camera: create camera fail");
        return;
    }
    hash_ = MakeResourceHash();
    if (onCreate) {
        onCreate(id_);
    }
    resRegister->RegisterEvent(
        MakeEventHash(CAMERA_ON_TAKE_PHOTO), [weak = WeakClaim(this)](const std::string& param) {
        auto camera = weak.Upgrade();
        if (camera) {
            camera->OnTakePhoto(param);
        }
    });
    resRegister->RegisterEvent(
        MakeEventHash(CAMERA_PERMISSION), [weak = WeakClaim(this)](const std::string& param) {
        auto camera = weak.Upgrade();
        if (camera) {
            camera->OnError(param);
        }
    });
    resRegister->RegisterEvent(
        MakeEventHash(CAMERA_ON_PREPARE), [weak = WeakClaim(this)](const std::string& param) {
        auto camera = weak.Upgrade();
        if (camera) {
            camera->OnPrepare(param);
        }
    });
    OpenCamera();
}

void Camera::UnRegisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }
    resRegister->UnregisterEvent(MakeEventHash(CAMERA_ON_TAKE_PHOTO));
    resRegister->UnregisterEvent(MakeEventHash(CAMERA_PERMISSION));
    resRegister->UnregisterEvent(MakeEventHash(CAMERA_ON_PREPARE));
}

void Camera::AddTakePhotoListener(TakePhotoListener&& listener)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    auto platformTaskExecutor =
        SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    platformTaskExecutor.PostTask([weak = WeakClaim(this), listener = std::move(listener)]() mutable {
        auto camera = weak.Upgrade();
        if (camera) {
            camera->OnAddTakePhotoListener(std::move(listener));
        }
    });
}

void Camera::OnAddTakePhotoListener(TakePhotoListener&& listener)
{
    takePhotoListener_ = std::move(listener);
}

void Camera::AddErrorListener(ErrorListener&& listener)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    auto platformTaskExecutor =
        SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    platformTaskExecutor.PostTask([weak = WeakClaim(this), listener = std::move(listener)]() mutable {
        auto camera = weak.Upgrade();
        if (camera) {
            camera->OnAddErrorListener(std::move(listener));
        }
    });
}

void Camera::OnAddErrorListener(ErrorListener&& listener)
{
    onErrorListener_ = std::move(listener);
}

void Camera::TakePhoto(int32_t quality)
{
    std::stringstream paramStream;
    paramStream << CAMERA_QUALITY << PARAM_EQUALS << quality;
    std::string param = paramStream.str();
    CallResRegisterMethod(MakeMethodHash(CAMERA_TAKE_PHOTO), param, nullptr);
}

void Camera::SetFlashMode(FlashType flash)
{
    std::stringstream paramStream;
    paramStream << CAMERA_FLASH_MODE << PARAM_EQUALS << static_cast<int32_t>(flash);
    std::string param = paramStream.str();
    CallResRegisterMethod(MakeMethodHash(CAMERA_SET_FLASH_MODE), param, nullptr);
}

void Camera::OnTakePhoto(const std::string& param)
{
    std::map<std::string, std::string> result = Resource::ParseMapFromString(param);
    if (takePhotoListener_) {
        takePhotoListener_(result);
    }
    result.clear();
}

void Camera::OnError(const std::string& param)
{
    if (!onErrorListener_) {
        return;
    }

    auto result = Resource::ParseMapFromString(param);
    auto iter = result.find(CAMERA_GRANT_RESULT);
    if (iter != result.end() && iter->second == "-1") {
        onErrorListener_(result[ERROR_CODE], result[ERROR_MSG]);
    }
}

void Camera::OpenCamera()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    std::stringstream paramStream;
    paramStream << DEVICE_POSITION << PARAM_EQUALS << static_cast<int32_t>(devicePosition_) << PARAM_AND
                << CACHE_FILE_PATH << PARAM_EQUALS << context->GetPhotoCachePath() << PARAM_AND << ORIENTATION
                << PARAM_EQUALS << (isPortrait_ ? "1" : "0");
    std::string param = paramStream.str();
    CallResRegisterMethod(MakeMethodHash(CAMERA_OEPN_CAMERA), param, nullptr);
}

void Camera::AddPreViewSizeChang(SizeChangeListener&& listener)
{
    onSizeChangeListener_ = std::move(listener);
}

void Camera::OnPreViewSizeChang(const std::string& param)
{
    int32_t preViewWidth = GetIntParam(param, CAMERA_PREVIEW_WIDTH);
    int32_t preViewHeight = GetIntParam(param, CAMERA_PREVIEW_HEIGHT);
    if (onSizeChangeListener_) {
        onSizeChangeListener_(preViewWidth, preViewHeight);
    }
}

void Camera::AddPrepareEventListener(PrepareEventListener&& listener)
{
    prepareEventListener_ = std::move(listener);
}

void Camera::OnPrepare(const std::string& param)
{
    if (prepareEventListener_) {
        prepareEventListener_();
    }
}

void Camera::SetPreViewSize(int32_t width, int32_t height)
{
    std::stringstream paramStream;
    paramStream << CAMERA_PREVIEW_WIDTH << PARAM_EQUALS << width << PARAM_AND
                << CAMERA_PREVIEW_HEIGHT << PARAM_EQUALS << height;
    std::string param = paramStream.str();
    CallResRegisterMethod(
        MakeMethodHash(CAMERA_SET_PREVIEW_SIZE), param, [weak = WeakClaim(this)](std::string& result) {
            auto camera = weak.Upgrade();
            if (camera) {
                camera->OnPreViewSizeChang(result);
            }
        });
}

void Camera::ChangeCameraCompenentId(bool changType, std::string id)
{
    LOGI("ChangeCameraCompenentId: changType %{private}d.", changType);
    std::stringstream paramStream;
    paramStream << CAMERA_CHANGE_COMPONENT_ID << PARAM_EQUALS << id << PARAM_AND << CAMERA_CHANGE_TYPE
                << PARAM_EQUALS << (changType ? "1" : "0");
    std::string param = paramStream.str();
    CallResRegisterMethod(MakeMethodHash(CAMERA_CHANGE_COMPONENT_ID_FUNCTION), param, nullptr);
}

} // namespace OHOS::Ace
