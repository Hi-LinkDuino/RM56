/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "base_info_config.h"

#define HDF_LOG_TAG BaseInfoConfig

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
void BaseInfoConfig::SetBase(std::vector<BaseItem> &bastList)
{
    bastList_ = bastList;
}

std::vector<BaseItem> BaseInfoConfig::GetBaseItem()
{
    return bastList_;
}
} // V1_0
} // Thermal
} // HDI
} // OHOS