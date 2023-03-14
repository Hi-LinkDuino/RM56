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

#include "ipp_algo_parser.h"
#include "hcs_dm_parser.h"
#include <cstdlib>

#define ENUM_TO_STRING(e) #e

namespace OHOS::Camera {
std::map<std::string, int> IppAlgoParser::algoModeMap_ = {
    {ENUM_TO_STRING(IPP_ALGO_MODE_NORMAL), IPP_ALGO_MODE_NORMAL},
    {ENUM_TO_STRING(IPP_ALGO_MODE_BEAUTY), IPP_ALGO_MODE_BEAUTY},
    {ENUM_TO_STRING(IPP_ALGO_MODE_HDR), IPP_ALGO_MODE_HDR},
};

IppAlgoParser::IppAlgoParser(const std::string& pathName)
{
    pathName_ = pathName;
}

IppAlgoParser::~IppAlgoParser()
{
    ReleaseHcsTree();
    devResInstance_ = nullptr;
    rootNode_ = nullptr;
}

RetCode IppAlgoParser::Init()
{
    ReleaseHcsTree();
    devResInstance_ = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (devResInstance_ == nullptr) {
        CAMERA_LOGE("get hcs interface failed.");
        return RC_ERROR;
    }

    CAMERA_LOGI("pathname = %{public}s", pathName_.c_str());
    SetHcsBlobPath(pathName_.c_str());
    rootNode_ = devResInstance_->GetRootNode();
    if (rootNode_ == nullptr) {
        CAMERA_LOGE("GetRootNode failed");
        return RC_ERROR;
    }
    if (rootNode_->name != nullptr) {
        CAMERA_LOGI("rootNode = %{public}s", rootNode_->name);
    }

    return RC_OK;
}

AlgoPluginList IppAlgoParser::ConstructPluginByHcsData()
{
    const DeviceResourceNode* ippAlgoConfig = devResInstance_->GetChildNode(rootNode_, "ipp_algo_config");
    if (ippAlgoConfig == nullptr) {
        return {};
    }
    if (ippAlgoConfig->name != nullptr) {
        CAMERA_LOGI("config name = %{public}s", ippAlgoConfig->name);
    }

    AlgoPluginList pluginList = {};
    const struct DeviceResourceNode* childNodeTmp = nullptr;
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(ippAlgoConfig, childNodeTmp)
    {
        if (childNodeTmp == nullptr || childNodeTmp->name == nullptr) {
            CAMERA_LOGD("param is null");
            return {};
        }
        std::string nodeName = std::string(childNodeTmp->name);
        CAMERA_LOGI("subnode name = %{public}s", nodeName.c_str());
        std::shared_ptr<AlgoPlugin> plugin = ConstructPlugin(childNodeTmp);
        if (plugin != nullptr) {
            pluginList.emplace_back(plugin);
        }
    }

    return pluginList;
}

std::shared_ptr<AlgoPlugin> IppAlgoParser::ConstructPlugin(const DeviceResourceNode* node)
{
    const char* name = nullptr;
    int32_t ret = devResInstance_->GetString(node, "name", &name, nullptr);
    if (ret != 0) {
        CAMERA_LOGE("get algo name failed");
        return nullptr;
    }
    const char* description = nullptr;
    ret = devResInstance_->GetString(node, "description", &description, nullptr);
    if (ret != 0) {
        CAMERA_LOGE("get algo description failed");
        return nullptr;
    }
    const char* path = nullptr;
    ret = devResInstance_->GetString(node, "path", &path, nullptr);
    if (ret != 0) {
        CAMERA_LOGE("get algo path failed");
        return nullptr;
    }

    const char* mode = nullptr;
    ret = devResInstance_->GetString(node, "mode", &mode, nullptr);
    if (ret != 0) {
        CAMERA_LOGE("get algo mode failed");
        return nullptr;
    }
    auto it = algoModeMap_.find(std::string(mode));
    if (it == algoModeMap_.end()) {
        CAMERA_LOGE("invalid mode %{public}s", mode);
        return nullptr;
    }

    std::string nodeName = std::string(path);
    if (path[0] != '/') {
        nodeName = nodeName.insert(0, HDF_LIBRARY_DIR "/");
    }
    std::shared_ptr<AlgoPlugin> plugin = std::make_shared<AlgoPlugin>(name, description, algoModeMap_[mode], nodeName);
    if (plugin == nullptr) {
        CAMERA_LOGE("create plugin failed");
        return nullptr;
    }

    return plugin;
}
} // namespace OHOS::Camera
