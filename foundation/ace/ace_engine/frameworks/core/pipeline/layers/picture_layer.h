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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_PICTURE_LAYER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_PICTURE_LAYER_H

#include "flutter/lib/ui/painting/picture.h"

#include "base/geometry/offset.h"
#include "core/pipeline/layers/layer.h"
#include "core/pipeline/layers/scene_builder.h"

namespace OHOS::Ace::Flutter {

class PictureLayer : public Layer {
    DECLARE_ACE_TYPE(PictureLayer, Layer)
public:
    PictureLayer() = default;
    ~PictureLayer() override = default;
    void SetPicture(fml::RefPtr<flutter::Picture> picture);
    void AddToScene(SceneBuilder& builder, double x, double y) override;

    void Dump() override;

    fml::RefPtr<flutter::Picture> GetPicture() const
    {
        return picture_;
    }

    const Offset& GetOffset() const
    {
        return offset_;
    }

    void SetNeedAddToScene(bool need)
    {
        needAddToScene_ = need;
    }

private:
    bool needAddToScene_ = true;
    Offset offset_ = Offset(-1.0, -1.0);
    fml::RefPtr<flutter::Picture> picture_;
    bool hints_ = false;
};

} // namespace OHOS::Ace::Flutter

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_PICTURE_LAYER_H
