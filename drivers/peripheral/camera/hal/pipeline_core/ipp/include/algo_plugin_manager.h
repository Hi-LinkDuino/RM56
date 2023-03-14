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

#ifndef HOS_CAMERA_ALGO_PLUGIN_MANAGER_H
#define HOS_CAMERA_ALGO_PLUGIN_MANAGER_H

#include "algo_plugin.h"
#include "camera.h"
#include "memory.h"
#include <list>
#include "ipp_algo_parser.h"

namespace OHOS::Camera {
class AlgoPluginManager {
public:
    AlgoPluginManager();
    ~AlgoPluginManager();

    RetCode LoadPlugin();
    void UnloadPlugin();
    std::shared_ptr<AlgoPlugin> GetAlgoPlugin(int32_t mode);

private:
    AlgoPluginList algoPluginList_ = {};
    std::unique_ptr<IppAlgoParser> parser_ = nullptr;
};
} // namespace OHOS::Camera

#endif
