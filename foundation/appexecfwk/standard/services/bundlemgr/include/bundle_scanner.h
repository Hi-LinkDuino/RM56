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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_SCANNER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_SCANNER_H

#include <list>
#include <memory>
#include <string>

namespace OHOS {
namespace AppExecFwk {
class BundleMgrService;
class BundleScanner {
public:
    BundleScanner();
    ~BundleScanner();
    /**
     * @brief Scan the specific path.
     * @param dirPath Indicates the path to scan.
     * @return Returns entries if this function is successfully called; returns false otherwise.
     */
    const std::list<std::string> &Scan(const std::string &dirPath);

private:
    /**
     * @brief Scan the specific path and save .hap files to entries_.
     * @param dirPath Indicates the path to scan.
     * @return Returns true if the specific path is successfully scanned; returns false otherwise.
     */
    bool ScanImpl(const std::string &dirPath);

private:
    std::list<std::string> entries_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_SCANNER_H
