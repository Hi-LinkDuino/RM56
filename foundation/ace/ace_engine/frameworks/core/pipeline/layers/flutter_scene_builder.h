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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_FLUTTER_SCENE_BUILDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_FLUTTER_SCENE_BUILDER_H

#include "flutter/lib/ui/compositing/scene.h"
#include "flutter/lib/ui/compositing/scene_builder.h"

#include "core/pipeline/layers/scene_builder.h"

namespace OHOS::Ace::Flutter {

class FlutterSceneBuilder : public SceneBuilder {
    DECLARE_ACE_TYPE(FlutterSceneBuilder, SceneBuilder)
public:
    FlutterSceneBuilder();
    ~FlutterSceneBuilder() override = default;

    void AddPicture(double dx, double dy, fml::RefPtr<flutter::Picture> picture, int hints) override;
    void AddTexture(
        double dx, double dy, double width, double height, int64_t textureId, bool freeze, uint8_t opacity) override;
    void PushTransform(const Matrix4& matrix4) override;
    void PushClipRRect(const flutter::RRect& rrect, int32_t clipBehavior) override;
    void PushClipPath(const flutter::CanvasPath* path, int32_t clipBehavior) override;
    void PushOpacity(int32_t alpha, double dx = 0, double dy = 0) override;
    void PushBackdropFilter(fml::RefPtr<flutter::ImageFilter> imageFilter) override;
    void PushShaderMask(fml::RefPtr<flutter::Shader> shader, double maskRectLeft, double maskRectRight,
        double maskRectTop, double maskRectBottom, int32_t blendMode) override;
    void PushGradientColorMask(const SkPaint& maskPaint) override;
    void PushSvgMask(const sk_sp<SkSVGDOM>& svgDom, double x, double y, double scaleX, double scaleY) override;
    void PushPathMask(const SkPaint& maskPaint, const SkPath& maskPath) override;
    void PushFilter(const SkPaint& filterPaint) override;
    void Pop() override;

    fml::RefPtr<flutter::Scene> Build();

private:
    fml::RefPtr<flutter::SceneBuilder> sceneBuilder_;
};

} // namespace OHOS::Ace::Flutter

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_FLUTTER_SCENE_BUILDER_H
