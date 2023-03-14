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

#include "inspect_node.h"

namespace OHOS::Ace::Framework {
InspectNode::InspectNode(NodeId nodeId, const std::string& nodeName)
    : nodeId_(nodeId), tag_(nodeName)
{}

InspectNode::~InspectNode()
{
    std::unordered_map<std::string, std::string>().swap(attrs_);
    attrs_.clear();
    std::unordered_map<std::string, std::string>().swap(styles_);
    styles_.clear();
}

void InspectNode::PackAttrAndStyle()
{}

void InspectNode::InitCommonStyles()
{
    styles_.insert(std::make_pair("padding-left", "0"));
    styles_.insert(std::make_pair("padding-top", "0"));
    styles_.insert(std::make_pair("padding-right", "0"));
    styles_.insert(std::make_pair("padding-bottom", "0"));
    styles_.insert(std::make_pair("padding-start", "0"));
    styles_.insert(std::make_pair("padding-end", "0"));
    styles_.insert(std::make_pair("margin-left", "0"));
    styles_.insert(std::make_pair("margin-top", "0"));
    styles_.insert(std::make_pair("margin-right", "0"));
    styles_.insert(std::make_pair("margin-bottom", "0"));
    styles_.insert(std::make_pair("margin-start", "0"));
    styles_.insert(std::make_pair("margin-end", "0"));
    styles_.insert(std::make_pair("border", "0"));
    styles_.insert(std::make_pair("border-left-style", "solid"));
    styles_.insert(std::make_pair("border-top-style", "solid"));
    styles_.insert(std::make_pair("border-right-style", "solid"));
    styles_.insert(std::make_pair("border-bottom-style", "solid"));
    styles_.insert(std::make_pair("border-left-width", "0"));
    styles_.insert(std::make_pair("border-top-width", "0"));
    styles_.insert(std::make_pair("border-right-width", "0"));
    styles_.insert(std::make_pair("border-bottom-width", "0"));
    styles_.insert(std::make_pair("border-left-color", "black"));
    styles_.insert(std::make_pair("border-top-color", "black"));
    styles_.insert(std::make_pair("border-right-color", "black"));
    styles_.insert(std::make_pair("border-bottom-color", "black"));
    styles_.insert(std::make_pair("background-size", "auto"));
    styles_.insert(std::make_pair("background-repeat", "repeat"));
    styles_.insert(std::make_pair("background-position", "0px 0px"));
    styles_.insert(std::make_pair("opacity", "1"));
    styles_.insert(std::make_pair("display", "flex"));
    styles_.insert(std::make_pair("visibility", "visible"));
    styles_.insert(std::make_pair("flex-grow", "0"));
    styles_.insert(std::make_pair("flex-shrink", "1"));
    styles_.insert(std::make_pair("position", "relative"));
}

void InspectNode::SetAllAttr(std::unique_ptr<JsonValue>& jsonNode, const char* attrsTag)
{
    auto attrJsonNode = JsonUtil::Create(true);
    for (auto iter = attrs_.begin(); iter != attrs_.end(); iter++) {
        attrJsonNode->Put(iter->first.c_str(), iter->second.c_str());
    }
    jsonNode->Put(attrsTag, attrJsonNode);
}

void InspectNode::SetAllStyle(std::unique_ptr<JsonValue>& jsonNode, const char* stylesTag)
{
    auto styleJsonNode = JsonUtil::Create(true);
    for (auto iter = styles_.begin(); iter != styles_.end(); iter++) {
        styleJsonNode->Put(iter->first.c_str(), iter->second.c_str());
    }
    jsonNode->Put(stylesTag, styleJsonNode);
}
} // namespace OHOS::Ace::Framework
