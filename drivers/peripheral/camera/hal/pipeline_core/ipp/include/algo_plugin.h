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

#ifndef HOS_CAMERA_ALGO_PLUGIN_H
#define HOS_CAMERA_ALGO_PLUGIN_H

#include "camera.h"
#include "camera_metadata_info.h"
#include "ibuffer.h"
#include "ipp_algo.h"
#include <memory>
#include <string>
#include <vector>

namespace OHOS::Camera {
class AlgoPlugin {
public:
    AlgoPlugin();
    ~AlgoPlugin();
    AlgoPlugin(std::string name, std::string description, int mode, std::string path);

    int GetMode() const;
    RetCode LoadLib();
    RetCode UnloadLib();
    RetCode Init(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode Start();
    RetCode Flush();
    RetCode Process(std::shared_ptr<IBuffer>& outBuffer,
                    std::vector<std::shared_ptr<IBuffer>>& inBuffers,
                    std::shared_ptr<CameraStandard::CameraMetadata>& meta);
    RetCode Stop();
    std::string GetName() const;

private:
    RetCode CheckLibPath(const char *path);

public:
    struct IppAlgoHandler {
        void* handle;
        IppAlgoFunc func;
    };

private:
    std::string path_ = "";
    std::string desc_ = "";
    std::string name_ = "";
    int mode_ = -1;
    IppAlgoHandler* algoHandler_ = nullptr;
};
} // namespace OHOS::Camera
#endif
