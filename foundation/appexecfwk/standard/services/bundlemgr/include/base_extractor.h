/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_EXTRACTOR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_EXTRACTOR_H

#include <string>

#include "zip_file.h"

namespace OHOS {
namespace AppExecFwk {
class BaseExtractor {
public:
    explicit BaseExtractor(const std::string &source);
    virtual ~BaseExtractor();
    /**
     * @brief Open compressed file.
     * @return Returns true if the file is successfully opened; returns false otherwise.
     */
    virtual bool Init();
    /**
     * @brief Extract the Profile to dest stream.
     * @param dest Indicates the obtained std::ostream object..
     * @return Returns true if the profile extracted successfully; returns false otherwise.
     */
    virtual bool ExtractProfile(std::ostream &dest) const = 0;

    virtual bool ExtractPackFile(std::ostream &dest) const = 0;
    /**
     * @brief Extract to dest stream by file name.
     * @param fileName Indicates the file name.
     * @param dest Indicates the obtained std::ostream object.
     * @return Returns true if the file extracted successfully; returns false otherwise.
     */
    bool ExtractByName(const std::string &fileName, std::ostream &dest) const;
    /**
     * @brief Extract to dest path on filesystem.
     * @param fileName Indicates the file name.
     * @param targetPath Indicates the target Path.
     * @return Returns true if the file extracted to filesystem successfully; returns false otherwise.
     */
    bool ExtractFile(const std::string &fileName, const std::string &targetPath) const;
    /**
     * @brief Get all file names in a hap file.
     * @param fileName Indicates the obtained file names in hap.
     * @return Returns true if the file names obtained successfully; returns false otherwise.
     */
    bool GetZipFileNames(std::vector<std::string> &fileNames);
    /**
     * @brief Has entry by name.
     * @param entryName Indicates the entry name.
     * @return Returns true if the ZipEntry is successfully finded; returns false otherwise.
     */
    bool HasEntry(const std::string &fileName) const;
    bool IsDirExist(const std::string &dir) const;
    bool IsStageBasedModel(std::string abilityName);
    bool IsNewVersion() const;

protected:
    const std::string sourceFile_;
    ZipFile zipFile_;
    bool initial_ = false;
private:
    bool isNewVersion_ = true;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BASE_EXTRACTOR_H
