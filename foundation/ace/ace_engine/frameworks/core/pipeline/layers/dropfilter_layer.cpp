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

#include "core/pipeline/layers/dropfilter_layer.h"

#include "base/log/log.h"

namespace OHOS::Ace::Flutter {

void DropFilterLayer::AddToScene(SceneBuilder& builder, double x, double y)
{
    if (!imageFilter_) {
        LOGE("can't create image filter");
        return;
    }
    builder.PushBackdropFilter(imageFilter_);
    imageFilter_->initBlur(sigmaX_, sigmaY_);
    AddChildToScene(builder, x + x_, y + y_);
    builder.Pop();
}

void DropFilterLayer::SetDropFilter(fml::RefPtr<flutter::ImageFilter> imageFilter, double sigmaX, double sigmaY)
{
    sigmaX_ = sigmaX;
    sigmaY_ = sigmaY;
    imageFilter_ = imageFilter;
}

} // namespace OHOS::Ace::Flutter