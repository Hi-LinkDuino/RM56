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

#include "core/components/common/properties/alignment.h"

namespace OHOS::Ace {

const Alignment Alignment::TOP_LEFT = Alignment(-1.0, -1.0);
const Alignment Alignment::TOP_CENTER = Alignment(0.0, -1.0);
const Alignment Alignment::TOP_RIGHT = Alignment(1.0, -1.0);
const Alignment Alignment::CENTER_LEFT = Alignment(-1.0, 0.0);
const Alignment Alignment::CENTER = Alignment(0.0, 0.0);
const Alignment Alignment::CENTER_RIGHT = Alignment(1.0, 0.0);
const Alignment Alignment::BOTTOM_LEFT = Alignment(-1.0, 1.0);
const Alignment Alignment::BOTTOM_CENTER = Alignment(0.0, 1.0);
const Alignment Alignment::BOTTOM_RIGHT = Alignment(1.0, 1.0);

/*
 * This function is used to calculate the relative position of child rectangle in parent rectangle.
 * Note that parentSize should not be less than childSize,
 * which means neither width or height of parent rectangle should be less than child's.
 *
 * @param parentSize Size of parent in which you want to put a child.
 * @param childSize Size of child you want to put in parent.
 * @param alignment A variable by which to decide the position of a child in parent.
 * @return A relative position toward top-left point of parent.
 */
const Offset Alignment::GetAlignPosition(const Size& parentSize, const Size& childSize, const Alignment& alignment)
{
    Offset offset;
    if (GreatOrEqual(parentSize.Width(), childSize.Width())) {
        offset.SetX((1.0 + alignment.GetHorizontal()) * (parentSize.Width() - childSize.Width()) / 2.0);
    }
    if (GreatOrEqual(parentSize.Height(), childSize.Height())) {
        offset.SetY((1.0 + alignment.GetVertical()) * (parentSize.Height() - childSize.Height()) / 2.0);
    }
    return offset;
}

std::string Alignment::GetAlignmentStr(TextDirection direction) const
{
    std::string result = "";
    Alignment alignment = Alignment(horizontal_, vertical_);
    if (alignment == TOP_LEFT) {
        result = direction == TextDirection::RTL ? "Alignment.TopEnd" : "Alignment.TopStart";
    } else if (alignment == TOP_CENTER) {
        result = "Alignment.Top";
    } else if (alignment == TOP_RIGHT) {
        result = direction == TextDirection::RTL ? "Alignment.TopStart" : "Alignment.TopEnd";
    } else if (alignment == CENTER_LEFT) {
        result = direction == TextDirection::RTL ? "Alignment.End" : "Alignment.Start";
    } else if (alignment == CENTER) {
        result = "Alignment.Center";
    } else if (alignment == CENTER_RIGHT) {
        result = direction == TextDirection::RTL ? "Alignment.Start" : "Alignment.End";
    } else if (alignment == BOTTOM_LEFT) {
        result = direction == TextDirection::RTL ? "Alignment.BottomEnd" : "Alignment.BottomStart";
    } else if (alignment == BOTTOM_CENTER) {
        result = "Alignment.Bottom";
    } else if (alignment == BOTTOM_RIGHT) {
        result = direction == TextDirection::RTL ? "Alignment.BottomStart" : "Alignment.BottomEnd";
    } else {
        result = "Alignment.Center";
    }
    return result;
}

} // namespace OHOS::Ace
