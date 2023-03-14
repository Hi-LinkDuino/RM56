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

#ifndef FOUNDATION_ACE_ADAPTER_PREVIEW_INSPECTOR_INSPECTOR_CLIENT_H
#define FOUNDATION_ACE_ADAPTER_PREVIEW_INSPECTOR_INSPECTOR_CLIENT_H

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace OHOS::Ace::Framework {

using AssembleJSONTreeCallback = std::function<bool(std::string &jsonTreeStr)>;
using AssembleDefaultJSONTreeCallback = std::function<bool(std::string &jsonTreeStr)>;
using OperateComponentCallback = std::function<bool(const std::string &attrsJson)>;
using FastPreviewErrorCallback = std::function<void(const std::string &jsonStr)>;

class InspectorClient {
public:
    InspectorClient &operator = (const InspectorClient &) = delete;
    InspectorClient(const InspectorClient &) = delete;
    ~InspectorClient() = default;

    static InspectorClient &GetInstance();
    void RegisterJSONTreeCallback(AssembleJSONTreeCallback &&callback);
    void RegisterDefaultJSONTreeCallback(AssembleDefaultJSONTreeCallback &&callback);
    void RegisterOperateComponentCallback(OperateComponentCallback &&callback);
    void RegisterFastPreviewErrorCallback(FastPreviewErrorCallback&& callback);
    bool AssembleJSONTreeStr(std::string &jsonTreeStr);
    bool AssembleDefaultJSONTreeStr(std::string &jsonTreeStr);
    bool OperateComponent(const std::string &attrsJson);
    void CallFastPreviewErrorCallback(const std::string& jsonStr)
    {
        if (onError_) {
            onError_(jsonStr);
        }
    }

private:
    InspectorClient() = default;
    AssembleJSONTreeCallback assembleJSONTreeCallback_;
    AssembleDefaultJSONTreeCallback assembleDefaultJSONTreeCallback_;
    OperateComponentCallback operateComponentCallback_;
    FastPreviewErrorCallback onError_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_ADAPTER_PREVIEW_INSPECTOR_INSPECTOR_CLIENT_H
