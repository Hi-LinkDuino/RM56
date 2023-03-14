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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_SCENE_BUILDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_SCENE_BUILDER_H

#include "experimental/svg/model/SkSVGDOM.h"
#include "flutter/lib/ui/painting/image_filter.h"
#include "flutter/lib/ui/painting/path.h"
#include "flutter/lib/ui/painting/picture.h"
#include "flutter/lib/ui/painting/rrect.h"
#include "flutter/lib/ui/painting/shader.h"

#include "base/geometry/matrix4.h"
#include "base/memory/ace_type.h"

namespace OHOS::Ace::Flutter {

class SceneBuilder : public AceType {
    DECLARE_ACE_TYPE(SceneBuilder, AceType)
public:
    SceneBuilder() = default;
    ~SceneBuilder() override = default;

    virtual void AddPicture(double dx, double dy, fml::RefPtr<flutter::Picture> picture, int hints) = 0;
    virtual void AddTexture(
        double dx, double dy, double width, double height, int64_t textureId, bool freeze, uint8_t opacity) = 0;
    virtual void PushTransform(const Matrix4& matrix4) = 0;
    virtual void PushClipRRect(const flutter::RRect& rrect, int32_t clipBehavior) = 0;
    virtual void PushClipPath(const flutter::CanvasPath* path, int32_t clipBehavior) = 0;
    virtual void PushOpacity(int32_t alpha, double dx = 0, double dy = 0) = 0;
    virtual void PushBackdropFilter(fml::RefPtr<flutter::ImageFilter> imageFilter) = 0;
    virtual void PushShaderMask(fml::RefPtr<flutter::Shader> shader, double maskRectLeft, double maskRectRight,
        double maskRectTop, double maskRectBottom, int32_t blendMode) = 0;
    virtual void PushGradientColorMask(const SkPaint& maskPaint) = 0;
    virtual void PushSvgMask(const sk_sp<SkSVGDOM>& svgDom, double x, double y, double scaleX, double scaleY) = 0;
    virtual void PushPathMask(const SkPaint& maskPaint, const SkPath& maskPath) = 0;
    virtual void PushFilter(const SkPaint& filterPaint) = 0;
    virtual void Pop() = 0;
};

} // namespace OHOS::Ace::Flutter

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_SCENE_BUILDER_H
