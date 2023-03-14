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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_MASK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_MASK_H

#include <string>

#include "base/geometry/rect.h"
#include "base/memory/ace_type.h"
#include "core/components/common/properties/clip_path.h"
#include "core/components/common/properties/decoration.h"

namespace OHOS::Ace {

enum class MaskImageType {
    NONE = 0,
    SVG,
    COLOR,
    PATH,
};

class ACE_EXPORT Mask : public AceType {
    DECLARE_ACE_TYPE(Mask, AceType);

public:
    Mask() = default;
    ~Mask() = default;

    static RefPtr<Mask> Create();

    virtual void LoadMask(const WeakPtr<PipelineContext>& context, const RefPtr<RenderNode>& parent) = 0;

    void SetMaskImage(const std::string& maskImage)
    {
        maskImageType_ = MaskImageType::NONE;
        maskImage_ = maskImage;

        if (maskImage.size() > 4) {
            std::string::size_type start = maskImage.find("url(");
            if (start != std::string::npos) {
                start += std::strlen("url(");
                std::string::size_type end = maskImage.find_first_of(')', start);
                if (end == std::string::npos) {
                    return;
                }

                file_ = maskImage.substr(start, end - start);
                if (file_.size() > 4 && file_.substr(file_.size() - 4) == ".svg") {
                    maskImageType_ = MaskImageType::SVG;
                }
                return;
            } else if (maskImage.substr(maskImage.size() - 4) == ".svg") {
                maskImageType_ = MaskImageType::SVG;
                file_ = maskImage;
                return;
            }

            if (maskImage.find("Gradient") != std::string::npos) {
                maskImageType_ = MaskImageType::COLOR;
            }
        }
    }

    void SetMask(const RefPtr<MaskPath>& maskPath)
    {
        maskImageType_ = MaskImageType::PATH;
        maskPath_ = maskPath;
    }

    RefPtr<MaskPath> GetMaskPath() const
    {
        return maskPath_;
    }

    void SetMaskPosition(const BackgroundImagePosition& position)
    {
        position_ = position;
    }

    void SetMaskSize(const BackgroundImageSize& size)
    {
        size_ = size;
    }

    const BackgroundImagePosition& GetMaskPosition() const
    {
        return position_;
    }

    const BackgroundImageSize& GetMaskSize() const
    {
        return size_;
    }

    bool IsSvgImage() const
    {
        return maskImageType_ == MaskImageType::SVG;
    }

    bool IsLastSvgImage() const
    {
        return lastMaskImageType_ == MaskImageType::SVG;
    }

    bool IsColorGradient() const
    {
        return maskImageType_ == MaskImageType::COLOR;
    }

    bool IsLastColorGradient() const
    {
        return lastMaskImageType_ == MaskImageType::COLOR;
    }

    bool IsPath() const
    {
        return maskImageType_ == MaskImageType::PATH;
    }

    bool IsValid() const
    {
        return IsSvgImage() || IsColorGradient() || IsPath();
    }

protected:
    std::string maskImage_;
    std::string file_;
    std::string lastFile_;
    MaskImageType maskImageType_ = MaskImageType::NONE;
    MaskImageType lastMaskImageType_ = MaskImageType::NONE;
    BackgroundImagePosition position_;
    BackgroundImageSize size_;
    RefPtr<MaskPath> maskPath_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BOX_MASK_H
