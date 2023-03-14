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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_CHECKABLE_PAINTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_CHECKABLE_PAINTER_H

#include "flutter/lib/ui/painting/canvas.h"
#include "flutter/lib/ui/painting/paint.h"

#include "core/pipeline/base/render_node.h"
#include "core/pipeline/layers/opacity_layer.h"

namespace OHOS::Ace {

class FlutterCheckablePainter {
public:
    FlutterCheckablePainter() = default;
    ~FlutterCheckablePainter() = default;

    void SetStrokeWidth(double strokeWidth, flutter::Paint& paint) const;
    RenderLayer ConfigureOpacityLayer(bool disabled);

protected:
    RefPtr<Flutter::OpacityLayer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PAINTER_FLUTTER_CHECKABLE_PAINTER_H
