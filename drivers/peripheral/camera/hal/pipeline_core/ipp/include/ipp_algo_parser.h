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

#ifndef HOS_IPP_ALGO_PARSER_H
#define HOS_IPP_ALGO_PARSER_H

#include "algo_plugin.h"
#include "device_resource_if.h"
#include <list>
#include <map>
#include <string>

namespace OHOS::Camera {
using AlgoPluginList = std::list<std::shared_ptr<AlgoPlugin>>;
class IppAlgoParser {
public:
    IppAlgoParser(const std::string& pathName);
    ~IppAlgoParser();
    IppAlgoParser(const IppAlgoParser& other) = delete;
    IppAlgoParser(IppAlgoParser&& other) = delete;
    IppAlgoParser& operator=(const IppAlgoParser& other) = delete;
    IppAlgoParser& operator=(IppAlgoParser&& other) = delete;

public:
    RetCode Init();
    AlgoPluginList ConstructPluginByHcsData();

private:
    std::shared_ptr<AlgoPlugin> ConstructPlugin(const DeviceResourceNode* node);

private:
    std::string pathName_ = "";
    const DeviceResourceIface* devResInstance_ = nullptr;
    const DeviceResourceNode* rootNode_ = nullptr;
    static std::map<std::string, int> algoModeMap_;
};
} // namespace OHOS::Camera
#endif
