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

#include "core/pipeline/layers/picture_layer.h"

#include "base/log/dump_log.h"
#include "base/log/log.h"

namespace OHOS::Ace::Flutter {

void PictureLayer::AddToScene(SceneBuilder& builder, double x, double y)
{
    offset_.SetX(x);
    offset_.SetY(y);
    if (needAddToScene_) {
        builder.AddPicture(x, y, picture_, hints_);
    }
}

void PictureLayer::SetPicture(fml::RefPtr<flutter::Picture> picture)
{
    if (!picture) {
        LOGE("PictureLayer::SetPicture, origin picture null");
        return;
    }
    if (!picture->picture()) {
        LOGE("PictureLayer::SetPicture, picture null");
        return;
    }
    needAddToScene_ = true;
    picture_ = picture;
}

void PictureLayer::Dump()
{
    if (DumpLog::GetInstance().GetDumpFile()) {
        DumpLog::GetInstance().AddDesc(
            "UniqueID:", (picture_ && picture_->picture()) ? picture_->picture()->uniqueID() : 0);
    }
}

} // namespace OHOS::Ace::Flutter
