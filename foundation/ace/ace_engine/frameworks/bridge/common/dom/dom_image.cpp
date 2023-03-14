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

#include "frameworks/bridge/common/dom/dom_image.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/common/properties/decoration.h"

namespace OHOS::Ace::Framework {
namespace {

const char SVG_THEME_FILL[] = "theme_fill";

ImageFit ConvertStrToFit(const std::string& fit)
{
    static const LinearMapNode<ImageFit> imageFitMap[] = {
        { "contain", ImageFit::CONTAIN },
        { "cover", ImageFit::COVER },
        { "fill", ImageFit::FILL },
        { "none", ImageFit::NONE },
        { "scale-down", ImageFit::SCALEDOWN },
    };
    ImageFit imageFit = ImageFit::COVER;
    auto iter = BinarySearchFindIndex(imageFitMap, ArraySize(imageFitMap), fit.c_str());
    if (iter != -1) {
        imageFit = imageFitMap[iter].value;
    }
    return imageFit;
}

} // namespace

ImageObjectPosition ImageObjectPosition(const std::string& value)
{
    return ParseImageObjectPosition(value.c_str());
}

void DOMImage::InitializeStyle()
{
    theme_ = GetTheme<ImageTheme>();
    if (!theme_) {
        LOGW("ImageTheme is null");
    }
}

DOMImage::DOMImage(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    imageChild_ = AceType::MakeRefPtr<ImageComponent>();
    imageChild_->SetFitMaxSize(true);
}

bool DOMImage::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_SRC) {
        imageChild_->SetSrc(ParseImageSrc(attr.second));
        return true;
    }
    if (attr.first == DOM_IMAGE_SYNC_LOAD) {
        imageChild_->SetSyncMode(StringToBool(attr.second));
        return true;
    }
    if (attr.first == DOM_IMAGE_ALT) {
        imageChild_->SetAlt(ParseImageSrc(attr.second));
        return true;
    }
    return false;
}

bool DOMImage::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, DOMImage&)> imageStylesOperators[] = {
        { DOM_IMAGE_FILL_COLOR,
            [](const std::string& val, DOMImage& image) {
                if (val == SVG_THEME_FILL && image.theme_) {
                    image.imageChild_->SetColor(image.theme_->GetFillColor());
                }
            } },
        { DOM_IMAGE_FIT_ORIGINAL_SIZE,
            [](const std::string& val, DOMImage& image) { image.imageChild_->SetFitMaxSize(!StringToBool(val)); } },
        { DOM_IMAGE_MATCH_TEXT_DIRECTION,
            [](const std::string& val, DOMImage& image) {
                image.imageChild_->SetMatchTextDirection(StringToBool(val));
            } },
        { DOM_IMAGE_FIT, [](const std::string& val,
                         DOMImage& image) { image.imageChild_->SetImageFit(ConvertStrToFit(val.c_str())); } },
        { DOM_IMAGE_POSITION, [](const std::string& val, DOMImage& image) {
            image.imageChild_->SetImageObjectPosition(ImageObjectPosition(val.c_str())); } },
    };
    auto operatorIter =
        BinarySearchFindIndex(imageStylesOperators, ArraySize(imageStylesOperators), style.first.c_str());
    if (operatorIter != -1) {
        imageStylesOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

bool DOMImage::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_COMPLETE) {
        loadSuccessEvent_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        imageChild_->SetLoadSuccessEvent(loadSuccessEvent_);
    } else if (event == DOM_ERROR) {
        loadFailEvent_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        imageChild_->SetLoadFailEvent(loadFailEvent_);
    } else {
        return false;
    }
    return true;
}

void DOMImage::PrepareSpecializedComponent()
{
    imageChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    // If there is a corresponding box decoration, specialize the box.
    if (boxComponent_) {
        auto backDecoration = boxComponent_->GetBackDecoration();
        if (backDecoration) {
            imageChild_->SetBorder(backDecoration->GetBorder());
        }
    }
    if (flexItemComponent_ && !imageChild_->GetFitMaxSize()) {
        flexItemComponent_->SetStretchFlag(false);
    }

    imageChild_->SetImageFill(GetImageFill());
}

} // namespace OHOS::Ace::Framework
