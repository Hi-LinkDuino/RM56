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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_CONTEXT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_CONTEXT_H

#include <unordered_map>

#include "base/memory/ace_type.h"
#include "frameworks/core/components/svg/parse/svg_node.h"
#include "frameworks/core/components/svg/render_svg_base.h"

namespace OHOS::Ace {

using AttrMap = std::unordered_map<std::string, std::string>;
using ClassStyleMap = std::unordered_map<std::string, AttrMap>;

class SvgNode;

class SvgContext : public AceType {
    DECLARE_ACE_TYPE(SvgContext, AceType);

public:
    SvgContext() = default;
    ~SvgContext()
    {
        idMapper_.clear();
    }

    void Push(const std::string& value, const RefPtr<SvgNode>& svgNode)
    {
        idMapper_.emplace(value, svgNode);
    }

    RefPtr<SvgNode> GetSvgNodeById(const std::string& id) const
    {
        auto item = idMapper_.find(id);
        if (item != idMapper_.end()) {
            return item->second;
        }
        return nullptr;
    }

    void PushStyle(const std::string& styleName, const std::pair<std::string, std::string>& attrPair)
    {
        const auto& arrMapIter = styleMap_.find(styleName);
        if (arrMapIter == styleMap_.end()) {
            AttrMap attrMap;
            attrMap.emplace(attrPair);
            styleMap_.emplace(std::make_pair(styleName, attrMap));
        } else {
            if (arrMapIter->second.find(attrPair.first) != arrMapIter->second.end()) {
                arrMapIter->second.erase(attrPair.first);
            }
            arrMapIter->second.emplace(attrPair);
        }
    }

    const AttrMap& GetAttrMap(const std::string& key) const
    {
        auto styleClassIter = styleMap_.find(key);
        if (styleClassIter != styleMap_.end()) {
            return styleClassIter->second;
        } else {
            static AttrMap emptyMap;
            return emptyMap;
        }
    }

    void SetSvgRoot(const RefPtr<RenderSvgBase>& renderNode)
    {
        root_ = renderNode;
    }

    const RefPtr<RenderSvgBase>& GetSvgRoot() const
    {
        return root_;
    }

private:
    std::unordered_map<std::string, RefPtr<SvgNode>> idMapper_;
    ClassStyleMap styleMap_;
    RefPtr<RenderSvgBase> root_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_PARSE_SVG_CONTEXT_H