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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_RESOURCE_TEXTURE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_RESOURCE_TEXTURE_H

#include "core/common/platform_res_register.h"
#include "core/components/xcomponent/resource/xcomponent_resource.h"

namespace OHOS::Ace {
class ACE_EXPORT NativeTexture : public XComponentResource {
    DECLARE_ACE_TYPE(NativeTexture, XComponentResource);
public:
    using RefreshListener = std::function<void()>;

    NativeTexture(const WeakPtr<PipelineContext>& context, ErrorCallback&& onError)
        : XComponentResource("texture", context, std::move(onError)) {}
    ~NativeTexture() override;

    void Create(const std::function<void(int64_t)>& onCreate, const std::string& idStr);
    void CreateTexture(const std::function<void(int64_t)>& onCreate, const std::string& idStr);
    void OnSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight,
                const std::function<void(std::string&)>& callback);
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_XCOMPONENT_RESOURCE_TEXTURE_H
