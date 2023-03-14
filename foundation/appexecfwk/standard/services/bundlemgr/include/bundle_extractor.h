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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_EXTRACTOR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_EXTRACTOR_H

#include <vector>

#include "base_extractor.h"

namespace OHOS {
namespace AppExecFwk {
class BundleExtractor : public BaseExtractor {
public:
    explicit BundleExtractor(const std::string &source);
    virtual ~BundleExtractor() override;
    /**
     * @brief Extract the config.json of a hap to dest stream.
     * @param dest Indicates the obtained std::ostream object.
     * @return Returns true if the Profile is successfully extracted; returns false otherwise.
     */
    virtual bool ExtractProfile(std::ostream &dest) const override;
    /**
     * @brief Extract the pack.info of a hap to dest stream.
     * @param dest Indicates the obtained std::ostream object.
     * @return Returns true if the file is successfully extracted; returns false otherwise.
     */
    virtual bool ExtractPackFile(std::ostream &dest) const override;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_EXTRACTOR_H
