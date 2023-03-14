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

#include "hdi_composer.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
HdiComposer::HdiComposer(std::unique_ptr<HdiComposition> pre, std::unique_ptr<HdiComposition> post)
{
    mPreComp = std::move(pre);
    mPostComp = std::move(post);
}

int32_t HdiComposer::Prepare(std::vector<HdiLayer *> &layers, HdiLayer &clientLayer)
{
    int ret = mPreComp->SetLayers(layers, clientLayer);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("pre composition prepare failed"));
    ret = mPostComp->SetLayers(layers, clientLayer);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("post composition prepare failed"));
    return DISPLAY_SUCCESS;
}

int32_t HdiComposer::Commit(bool modeSet)
{
    int ret = mPreComp->Apply(modeSet);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("pre composition apply failed"));
    ret = mPostComp->Apply(modeSet);
    DISPLAY_CHK_RETURN((ret != DISPLAY_SUCCESS), DISPLAY_FAILURE, DISPLAY_LOGE("post composition apply failed"));
    return DISPLAY_SUCCESS;
}
} // OHOS
} // HDI
} // DISPLAY
