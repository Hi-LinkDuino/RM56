/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "connected_tag_service.h"
#include <hdf_log.h>

namespace OHOS {
namespace HDI {
namespace NFC {
namespace V1_0 {
int32_t ConnectedTagService::Init()
{
    HDF_LOGI("%{public}s, no vendor impl", __func__);
    return HDF_SUCCESS;
}

int32_t ConnectedTagService::Uninit()
{
    HDF_LOGI("%{public}s, no vendor impl", __func__);
    return HDF_SUCCESS;
}
std::string ConnectedTagService::ReadNdefTag()
{
    HDF_LOGI("%{public}s, no vendor impl", __func__);
    return "no-vendor-impl-for-read";
}

int32_t ConnectedTagService::WriteNdefTag(std::string ndefData)
{
    HDF_LOGI("%{public}s, no vendor impl, ndefData = %{public}s", __func__, ndefData.c_str());
    return HDF_SUCCESS;
}
}  // namespace V1_0
}  // namespace NFC
}  // namespace HDI
}  // namespace OHOS
