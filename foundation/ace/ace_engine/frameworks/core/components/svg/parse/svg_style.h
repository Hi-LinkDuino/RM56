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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_STYLE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_STYLE_H

#include "frameworks/core/components/svg/parse/svg_context.h"
#include "frameworks/core/components/svg/parse/svg_node.h"

namespace OHOS::Ace {

using PushAttr = std::function<void(const std::string&, const std::pair<std::string, std::string>&)>;

class SvgStyle : public SvgNode {
    DECLARE_ACE_TYPE(SvgStyle, SvgNode);

public:
    SvgStyle() = default;
    ~SvgStyle() override = default;

    static RefPtr<SvgNode> Create();
    static void ParseCssStyle(const std::string& styleValue, const PushAttr& callback);
    static void ParseCssAttrs(const std::string& styleName, const std::string& attrs, const PushAttr& callback);
    static std::vector<std::string> SplitString(const std::string& srcString, const std::string& splitString);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_G_H
