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

#include "core/components/xcomponent/resource/native_texture.h"

#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {
const char TEXTURE_METHOD_REFRESH[] = "markTextureFrameAvailable";
const char TEXTURE_ID[] = "textureId";
const char SET_TEXTURE_SIZE[] = "setTextureSize";
const char TEXTURE_HEIGHT[] = "textureHeight";
const char TEXTURE_WIDTH[] = "textureWidth";
const char TEXTURE_SET_DEFAULT_SIZE[] = "setDefaultSize";
const char TEXTURE_REGISTER_SURFACE[] = "registerSurface";
const char TEXTURE_SET_DEFAULT_SIZE_TRUE[] = "true";

NativeTexture::~NativeTexture()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to create texture due to context is null");
        return;
    }

    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        resRegister->UnregisterEvent(MakeEventHash(TEXTURE_METHOD_REFRESH));
    } else {
        auto weak = AceType::WeakClaim(AceType::RawPtr(resRegister));
        platformTaskExecutor.PostTask([eventHash = MakeEventHash(TEXTURE_METHOD_REFRESH), weak] {
            auto resRegister = weak.Upgrade();
            if (resRegister == nullptr) {
                LOGE("resRegister is nullptr");
                return;
            }
            resRegister->UnregisterEvent(eventHash);
        });
    }
}

void NativeTexture::Create(const std::function<void(int64_t)>& onCreate, const std::string& idStr)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to create texture due to context is null");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

    platformTaskExecutor.PostTask([weak = WeakClaim(this), onCreate, idStr] {
        auto texture = weak.Upgrade();
        if (texture) {
            texture->CreateTexture(onCreate, idStr);
        }
    });
}

void NativeTexture::CreateTexture(const std::function<void(int64_t)>& onCreate, const std::string& idStr)
{
    auto context = context_.Upgrade();
    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        LOGE("NativeTexture::CreateTexture resRegister = null");
        return;
    }
    std::stringstream paramStream;
    paramStream << XCOMPONENT_PARAM_INIT << XCOMPONENT_PARAM_EQUALS << TEXTURE_REGISTER_SURFACE <<
    XCOMPONENT_PARAM_AND  << "id" << XCOMPONENT_PARAM_EQUALS << idStr;
    std::string param = paramStream.str();

    id_ = resRegister->CreateResource(type_, param);
    if (id_ == -1) {
        LOGE("NativeTexture::CreateTexture id_ invalid");
        return;
    }
    hash_ = MakeResourceHash();

    if (onCreate) {
        onCreate(id_);
    }
}

void NativeTexture::OnSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight,
                           const std::function<void(std::string&)>& callback)
{
    std::stringstream paramStream;
    paramStream << TEXTURE_ID << XCOMPONENT_PARAM_EQUALS << textureId << XCOMPONENT_PARAM_AND
        << TEXTURE_WIDTH << XCOMPONENT_PARAM_EQUALS << textureWidth << XCOMPONENT_PARAM_AND
        << TEXTURE_HEIGHT << XCOMPONENT_PARAM_EQUALS << textureHeight << XCOMPONENT_PARAM_AND
        << TEXTURE_SET_DEFAULT_SIZE << XCOMPONENT_PARAM_EQUALS << TEXTURE_SET_DEFAULT_SIZE_TRUE;
    std::string param = paramStream.str();
    CallResRegisterMethod(MakeMethodHash(SET_TEXTURE_SIZE), param, callback);
}
} // namespace OHOS::Ace