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

#include "core/pipeline/layers/texture_layer.h"

#include "base/log/dump_log.h"

namespace OHOS::Ace::Flutter {

void TextureLayer::AddToScene(SceneBuilder& builder, double x, double y)
{
    builder.AddTexture(x_ + x, y_ + y, width_, height_, id_, freeze_, opacity_);
}

void TextureLayer::Update(int64_t textureId, bool freeze, double width, double height, uint8_t opacity)
{
    width_ = width;
    height_ = height;
    id_ = textureId;
    freeze_ = freeze;
    opacity_ = opacity;
}

void TextureLayer::Dump()
{
    OffsetLayer::Dump();
    if (DumpLog::GetInstance().GetDumpFile()) {
        DumpLog::GetInstance().AddDesc("Size:(", width_, ",", height_, ")");
        DumpLog::GetInstance().AddDesc("TextureId:", id_);
    }
}

} // namespace OHOS::Ace::Flutter
