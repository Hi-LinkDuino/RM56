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

#include "core/components/video/resource/texture.h"

#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {

const char TEXTURE_ERRORCODE_CREATEFAIL[] = "error_video_000001";
const char TEXTURE_ERRORMSG_CREATEFAIL[] = "Unable to initialize video player.";

const char TEXTURE_METHOD_REFRESH[] = "markTextureFrameAvailable";

const char SET_TEXTURE_SIZE[] = "setTextureSize";
const char TEXTURE_HEIGHT[] = "textureHeight";
const char TEXTURE_ID[] = "textureId";
const char TEXTURE_WIDTH[] = "textureWidth";

Texture::~Texture()
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

void Texture::Create(const std::function<void(int64_t)>& onCreate)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to create texture due to context is null");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

    platformTaskExecutor.PostTask([weak = WeakClaim(this), onCreate] {
        auto texture = weak.Upgrade();
        if (texture) {
            texture->CreateTexture(onCreate);
        }
    });
}

void Texture::CreateTexture(const std::function<void(int64_t)>& onCreate)
{
    auto context = context_.Upgrade();
    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }
    id_ = resRegister->CreateResource(type_, PARAM_NONE);
    if (id_ == INVALID_ID) {
        if (onError_) {
            onError_(TEXTURE_ERRORCODE_CREATEFAIL, TEXTURE_ERRORMSG_CREATEFAIL);
        }
        return;
    }
    hash_ = MakeResourceHash();
    resRegister->RegisterEvent(
        MakeEventHash(TEXTURE_METHOD_REFRESH), [weak = WeakClaim(this)](const std::string& param) {
            auto texture = weak.Upgrade();
            if (texture) {
                texture->OnRefresh(param);
            }
        });

    if (onCreate) {
        onCreate(id_);
    }
}

void Texture::OnRefresh(const std::string& param)
{
    if (onRefreshListener_) {
        onRefreshListener_();
    }
}

void Texture::OnSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight)
{
    std::stringstream paramStream;
    paramStream << TEXTURE_ID << PARAM_EQUALS << textureId << PARAM_AND << TEXTURE_WIDTH << PARAM_EQUALS << textureWidth
                << PARAM_AND << TEXTURE_HEIGHT << PARAM_EQUALS << textureHeight;
    std::string param = paramStream.str();
    CallResRegisterMethod(MakeMethodHash(SET_TEXTURE_SIZE), param);
}

} // namespace OHOS::Ace