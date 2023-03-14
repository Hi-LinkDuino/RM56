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

#include "frameworks/core/components/svg/parse/svg_style.h"

namespace OHOS::Ace {

RefPtr<SvgNode> SvgStyle::Create()
{
    return AceType::MakeRefPtr<SvgStyle>();
}

void SvgStyle::ParseCssStyle(const std::string& styleValue, const PushAttr& callback)
{
    if (callback == nullptr) {
        LOGE("ParseCssStyle failed, callback is null");
        return;
    }
    std::string newStyle = styleValue;
    StringUtils::DeleteAllMark(newStyle, '\n');
    StringUtils::DeleteAllMark(newStyle, '\r');
    StringUtils::DeleteAllMark(newStyle, '\t');
    StringUtils::DeleteAllMark(newStyle, ' ');
    auto styles = SplitString(newStyle.substr(1), "}.");
    for (auto& style : styles) {
        auto nameEnd = style.find_first_of('{');
        if (nameEnd == std::string::npos) {
            continue;
        }
        auto names = style.substr(0, nameEnd);
        if (names.empty()) {
            continue;
        }
        auto splitNames = SplitString(names, ",.");
        auto attrs = style.substr(nameEnd + 1);
        for (auto& splitName : splitNames) {
            ParseCssAttrs(splitName, attrs, callback);
        }
    }
}

void SvgStyle::ParseCssAttrs(const std::string& styleName, const std::string& attrs, const PushAttr& callback)
{
    const auto& attrVector = SplitString(attrs, ";");
    for (const auto& attribute : attrVector) {
        auto arrPair = SplitString(attribute, ":");
        if (arrPair.size() != 2) {
            continue;
        }
        callback(styleName, std::make_pair(arrPair[0], arrPair[1]));
    }
}

std::vector<std::string> SvgStyle::SplitString(const std::string& source, const std::string& mark)
{
    std::string::size_type pos1;
    std::string::size_type pos2;
    std::vector<std::string> res;
    pos2 = source.find(mark);
    pos1 = 0;
    while (std::string::npos != pos2) {
        res.push_back(source.substr(pos1, pos2 - pos1));
        pos1 = pos2 + mark.size();
        pos2 = source.find(mark, pos1);
    }
    if (pos1 != source.length()) {
        res.push_back(source.substr(pos1));
    }
    return res;
}

} // namespace OHOS::Ace
