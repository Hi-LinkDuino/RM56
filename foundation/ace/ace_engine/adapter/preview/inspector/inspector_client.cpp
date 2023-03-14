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

#include "adapter/preview/inspector/inspector_client.h"

namespace OHOS::Ace::Framework {

InspectorClient& InspectorClient::GetInstance()
{
    static InspectorClient instance;
    return instance;
}

void InspectorClient::RegisterJSONTreeCallback(AssembleJSONTreeCallback&& callback)
{
    assembleJSONTreeCallback_ = callback;
}

void InspectorClient::RegisterDefaultJSONTreeCallback(AssembleDefaultJSONTreeCallback&& callback)
{
    assembleDefaultJSONTreeCallback_ = callback;
}

void InspectorClient::RegisterOperateComponentCallback(OperateComponentCallback&& callback)
{
    operateComponentCallback_ = callback;
}

void InspectorClient::RegisterFastPreviewErrorCallback(FastPreviewErrorCallback&& callback)
{
    onError_ = callback;
}

bool InspectorClient::AssembleJSONTreeStr(std::string& jsonTreeStr)
{
    if (assembleJSONTreeCallback_) {
        return assembleJSONTreeCallback_(jsonTreeStr);
    } else {
        return false;
    }
}

bool InspectorClient::AssembleDefaultJSONTreeStr(std::string &jsonTreeStr)
{
    if (assembleDefaultJSONTreeCallback_) {
        return assembleDefaultJSONTreeCallback_(jsonTreeStr);
    } else {
        return false;
    }
}

bool InspectorClient::OperateComponent(const std::string &attrsJson)
{
    if (operateComponentCallback_) {
        return operateComponentCallback_(attrsJson);
    } else {
        return false;
    }
}

} // namespace OHOS::Ace::Framework
