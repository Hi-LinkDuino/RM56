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
#include "algo_plugin.h"
#include <dlfcn.h>

namespace OHOS::Camera {
AlgoPlugin::AlgoPlugin(std::string name, std::string description, int mode, std::string path)
{
    name_ = name;
    desc_ = description;
    mode_ = mode;
    path_ = path;
}

AlgoPlugin::~AlgoPlugin()
{
    if (algoHandler_ != nullptr) {
        UnloadLib();
        delete algoHandler_;
        algoHandler_ = nullptr;
    }
}

RetCode AlgoPlugin::Init(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    (void)meta;
    if (algoHandler_->func.Init == nullptr) {
        CAMERA_LOGE("unsupport operation.");
        return RC_ERROR;
    }
    // parse metadata
    int ret = algoHandler_->func.Init(nullptr);
    if (ret == 0) {
        CAMERA_LOGI("algo init success, ret = %{public}d", ret);
        return RC_OK;
    }

    return RC_ERROR;
}

RetCode AlgoPlugin::Start()
{
    if (algoHandler_->func.Start == nullptr) {
        CAMERA_LOGE("unsupport operation.");
        return RC_ERROR;
    }
    int ret = algoHandler_->func.Start();
    if (ret != 0) {
        CAMERA_LOGE("start algo failed, ret = %{public}d", ret);
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode AlgoPlugin::Flush()
{
    if (algoHandler_->func.Flush == nullptr) {
        CAMERA_LOGE("unsupport operation.");
        return RC_ERROR;
    }
    int ret = algoHandler_->func.Flush();
    if (ret != 0) {
        CAMERA_LOGE("flush algo failed, ret = %{public}d", ret);
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode AlgoPlugin::Process(std::shared_ptr<IBuffer>& outBuffer,
                            std::vector<std::shared_ptr<IBuffer>>& inBuffers,
                            std::shared_ptr<CameraStandard::CameraMetadata>& meta)
{
    (void)meta;
    if (algoHandler_->func.Process == nullptr) {
        CAMERA_LOGE("unsupport operation.");
        return RC_ERROR;
    }

    IppAlgoBuffer* outAlgoBuffer = nullptr;
    if (outBuffer == nullptr) {
        outAlgoBuffer = nullptr;
    } else {
        outAlgoBuffer = new IppAlgoBuffer();
        if (outAlgoBuffer != nullptr) {
            outAlgoBuffer->addr = outBuffer->GetVirAddress();
            outAlgoBuffer->size = outBuffer->GetSize();
        }
    }

    IppAlgoBuffer** inAlgoBuffers = new IppAlgoBuffer* [inBuffers.size()];
    for (uint32_t i = 0; i < inBuffers.size(); i++) {
        if (inBuffers[i] == nullptr) {
            inAlgoBuffers[i] = nullptr;
        } else {
            inAlgoBuffers[i] = new IppAlgoBuffer();
            inAlgoBuffers[i]->addr = inBuffers[i]->GetVirAddress();
            inAlgoBuffers[i]->size = inBuffers[i]->GetSize();
            inAlgoBuffers[i]->width = inBuffers[i]->GetWidth();
            inAlgoBuffers[i]->height = inBuffers[i]->GetHeight();
            inAlgoBuffers[i]->stride = inBuffers[i]->GetStride();
            inAlgoBuffers[i]->id = i;
        }
    }

    int ret = algoHandler_->func.Process(inAlgoBuffers, inBuffers.size(), outAlgoBuffer, nullptr);
    if (outAlgoBuffer != nullptr) {
        delete outAlgoBuffer;
        outAlgoBuffer = nullptr;
    }

    for (int i = 0; i < inBuffers.size(); i++) {
        if (inAlgoBuffers[i] != nullptr) {
            delete inAlgoBuffers[i];
            inAlgoBuffers[i] = nullptr;
        }
    }
    delete[] inAlgoBuffers;
    inAlgoBuffers = nullptr;

    if (ret != 0) {
        CAMERA_LOGE("process algo failed, ret = %{public}d", ret);
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode AlgoPlugin::Stop()
{
    if (algoHandler_->func.Stop == nullptr) {
        CAMERA_LOGE("unsupport operation.");
        return RC_ERROR;
    }
    int ret = algoHandler_->func.Stop();
    if (ret != 0) {
        CAMERA_LOGE("stop algo failed, ret = %{public}d", ret);
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode AlgoPlugin::CheckLibPath(const char *path)
{
    char absPath[PATH_MAX] = {0};
    if (path == nullptr || (realpath(path, absPath) == nullptr)) {
        CAMERA_LOGE("path is nullptr.");
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode AlgoPlugin::LoadLib()
{
    algoHandler_ = new IppAlgoHandler();
    if (algoHandler_ == nullptr) {
        CAMERA_LOGE("create algo handler failed.");
        return RC_ERROR;
    }

    if (path_.size() == 0) {
        CAMERA_LOGE("path is null");
        return RC_ERROR;
    }

    uint32_t ret = CheckLibPath(path_.c_str());
    if (ret == RC_ERROR) {
        CAMERA_LOGE("lib path:%{public}s is error.", path_.c_str());
        return RC_ERROR;
    }

    algoHandler_->handle = ::dlopen(path_.c_str(), RTLD_NOW);
    if (algoHandler_->handle == nullptr) {
        CAMERA_LOGE("dlopen %{public}s failed, %{public}s", path_.c_str(), ::dlerror());
        return RC_ERROR;
    }

    algoHandler_->func.Init = reinterpret_cast<AlgoFuncInit>(::dlsym(algoHandler_->handle, "Init"));
    if (algoHandler_->func.Init == nullptr) {
        CAMERA_LOGE("can't get symbol of function Init, %{public}s", ::dlerror());
        return RC_ERROR;
    }

    algoHandler_->func.Start = reinterpret_cast<AlgoFuncStart>(::dlsym(algoHandler_->handle, "Start"));
    if (algoHandler_->func.Start == nullptr) {
        CAMERA_LOGE("can't get symbol of function Start, %{public}s", ::dlerror());
        return RC_ERROR;
    }

    algoHandler_->func.Flush = reinterpret_cast<AlgoFuncFlush>(::dlsym(algoHandler_->handle, "Flush"));
    if (algoHandler_->func.Flush == nullptr) {
        CAMERA_LOGE("can't get symbol of function Flush, %{public}s", ::dlerror());
        return RC_ERROR;
    }

    algoHandler_->func.Process = reinterpret_cast<AlgoFuncProcess>(::dlsym(algoHandler_->handle, "Process"));
    if (algoHandler_->func.Process == nullptr) {
        CAMERA_LOGE("can't get symbol of function Process, %{public}s", ::dlerror());
        return RC_ERROR;
    }

    algoHandler_->func.Stop = reinterpret_cast<AlgoFuncStop>(::dlsym(algoHandler_->handle, "Stop"));
    if (algoHandler_->func.Stop == nullptr) {
        CAMERA_LOGE("can't get symbol of function Stop, %{public}s", ::dlerror());
        return RC_ERROR;
    }

    return RC_OK;
}

RetCode AlgoPlugin::UnloadLib()
{
    if (algoHandler_->handle != nullptr) {
        CAMERA_LOGI("handle is already closed.");
        return RC_OK;
    }

    int ret = ::dlclose(algoHandler_->handle);
    if (ret) {
        CAMERA_LOGE("dlclose %{public}s failed", path_.c_str());
        return RC_ERROR;
    }

    return RC_OK;
}

int AlgoPlugin::GetMode() const
{
    return mode_;
}

std::string AlgoPlugin::GetName() const
{
    return name_;
}
} // namespace OHOS::Camera
