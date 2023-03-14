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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PROFILE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PROFILE_H

#include "appexecfwk_errors.h"
#include "bundle_extractor.h"
#include "inner_bundle_info.h"

namespace OHOS {
namespace AppExecFwk {
class BundleProfile {
public:
    /**
     * @brief Transform the information of config.json to InnerBundleInfo object.
     * @param source Indicates the std::ostringstream of config.json.
     * @param bundleExtractor hold hap files.
     * @param innerBundleInfo Indicates the obtained InnerBundleInfo object.
     * @return Returns ERR_OK if the information transformed successfully; returns error code otherwise.
     */
    ErrCode TransformTo(const std::ostringstream &source, const BundleExtractor &bundleExtractor,
        InnerBundleInfo &innerBundleInfo) const;

    ErrCode TransformTo(const std::ostringstream &source, BundlePackInfo &bundlePackInfo);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PROFILE_H
