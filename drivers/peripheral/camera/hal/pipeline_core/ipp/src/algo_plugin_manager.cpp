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

#include "hdf_base.h"
#include "algo_plugin_manager.h"
#ifdef CAMERA_BUILT_ON_OHOS_LITE
#define IPP_ALGO_PATH HDF_ETC_DIR"/camera/ipp_algo_config.hcb"
#else
#define IPP_ALGO_PATH HDF_CONFIG_DIR"/ipp_algo_config.hcb"
#endif

namespace OHOS::Camera {
AlgoPluginManager::AlgoPluginManager() {}
AlgoPluginManager::~AlgoPluginManager()
{
    UnloadPlugin();
    algoPluginList_.clear();
}

RetCode AlgoPluginManager::LoadPlugin()
{
    parser_ = std::make_unique<IppAlgoParser>(IPP_ALGO_PATH);
    RetCode ret = parser_->Init();
    if (ret != RC_OK) {
        CAMERA_LOGE("can't load algo plugin %{public}s", IPP_ALGO_PATH);
        return RC_ERROR;
    }
    algoPluginList_ = parser_->ConstructPluginByHcsData();
    for (auto it : algoPluginList_) {
        if (it->LoadLib() != RC_OK) {
            CAMERA_LOGE("load algorithm lib: %{public}s failed", it->GetName().c_str());
            continue;
        }
    }
    return RC_OK;
}

std::shared_ptr<AlgoPlugin> AlgoPluginManager::GetAlgoPlugin(int32_t mode)
{
    std::shared_ptr<AlgoPlugin> plugin = nullptr;
    for (auto& it : algoPluginList_) {
        if (it->GetMode() == mode) {
            plugin = it;
            break;
        }
    }

    return plugin;
}

void AlgoPluginManager::UnloadPlugin()
{
    for (auto it : algoPluginList_) {
        it->UnloadLib();
    }
    return;
}
} // namespace OHOS::Camera
