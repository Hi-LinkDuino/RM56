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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_TEXTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_TEXTURE_H

#include "core/common/platform_res_register.h"
#include "core/components/video/resource/resource.h"

namespace OHOS::Ace {

class ACE_EXPORT Texture : public Resource {
    DECLARE_ACE_TYPE(Texture, Resource);
public:
    using RefreshListener = std::function<void()>;

    Texture(const WeakPtr<PipelineContext>& context, ErrorCallback&& onError)
        : Resource("texture", context, std::move(onError)) {}
    ~Texture() override;

    void Create(const std::function<void(int64_t)>& onCreate);
    void CreateTexture(const std::function<void(int64_t)>& onCreate);
    void OnSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight);

    void SetRefreshListener(RefreshListener&& listener)
    {
        onRefreshListener_ = std::move(listener);
    }

private:
    void OnRefresh(const std::string& param);
    RefreshListener onRefreshListener_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_TEXTURE_H
