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

#ifndef HDI_COMPOSER_H
#define HDI_COMPOSER_H
#include <vector>
#include <memory>
#include "hdi_layer.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
class HdiComposition {
public:
    HdiComposition() {}
    virtual int32_t Init()
    {
        return DISPLAY_SUCCESS;
    };
    virtual int32_t SetLayers(std::vector<HdiLayer *> &layers, HdiLayer &clientLayer)
    {
        return DISPLAY_SUCCESS;
    }
    virtual int32_t Apply(bool modeSet)
    {
        return DISPLAY_SUCCESS;
    }
    virtual ~HdiComposition() {}

protected:
    std::vector<HdiLayer *> mCompLayers;
};

class HdiComposer {
public:
    HdiComposer(std::unique_ptr<HdiComposition> pre, std::unique_ptr<HdiComposition> post);
    virtual ~HdiComposer() {}
    int32_t Prepare(std::vector<HdiLayer *> &layers, HdiLayer &clientLayer);
    int32_t Commit(bool modeSet);
    HdiComposition *GetPreCompostion()
    {
        return mPreComp.get();
    }
    HdiComposition *GetPostCompostion()
    {
        return mPostComp.get();
    }

private:
    std::unique_ptr<HdiComposition> mPreComp;
    std::unique_ptr<HdiComposition> mPostComp;
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // HDI_COMPOSER_H