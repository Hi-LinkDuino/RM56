/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_BUNDLE_INSTALLER_H
#define OHOS_BUNDLE_INSTALLER_H

#include "bundle_common.h"
#include "bundle_info.h"
#include "bundle_info_utils.h"
#include "install_param.h"
#include "hap_sign_verify.h"
#include "stdint.h"

#include <string>
#include <vector>

namespace OHOS {
class BundleInstaller {
public:
    BundleInstaller(const std::string &codeDirPath, const std::string &dataDirPath);
    ~BundleInstaller();

    uint8_t Install(const char *path, const InstallParam &installParam);
    uint8_t Uninstall(const char *bundleName, const InstallParam &installParam);
    std::string GetCodeDirPath() const;
    std::string GetDataDirPath() const;
private:
    uint8_t ProcessBundleInstall(const std::string &path, const char *randStr, InstallRecord &installRecord,
        uint8_t hapType);
    uint8_t HandleFileAndBackUpRecord(const char *codePath, const char *randStr, InstallRecord &record,
        bool isUpdate, uint8_t hapType);
    uint8_t UpdateBundleInfo(const char *appId, const BundleRes &bundleRes, BundleInfo *bundleInfo, bool isUpdate,
        uint8_t hapType);
    uint8_t ReshapeAppId(const char *bundleName, std::string &appId);
    uint8_t CheckProvisionInfoIsValid(const SignatureInfo &signatureInfo, const Permissions &permissions,
        const char *bundleName);
    bool MatchPermissions(const std::vector<std::string> & restrictedPermissions,
        PermissionTrans *permissionsTrans, int32_t permTransNum);
    bool MatchBundleName(const char *bundleName, const char *matchedBundleName);
    uint8_t CheckInstallFileIsValid(const char *path);
    void RecordThirdSystemBundle(const char *bundleName, const char *path);
    uint8_t StorePermissions(const char *bundleName, PermissionTrans *permissions, int32_t permNum, bool isUpdate);
    uint8_t CheckVersionAndSignature(const char *bundleName, BundleInfo *bundleInfo);
    bool RenameJsonFile(const char *fileName, const char *randStr);
    bool CheckIsThirdSystemBundle(const char *bundleName);
    bool BackUpInstallRecord(const InstallRecord &record, const char *jsonPath);
    void InitThirdSystemBundleRecord(const char *bundleName, const char *path);
    bool BackUpUidAndGidInfo(const InstallRecord &record, const char *jsonPath);
    void ModifyInstallDirByHapType(const InstallParam &installParam, uint8_t hapType);
    uint8_t GetHapType(const char *path);
    void RestoreInstallEnv(const InstallParam &installParam);
    uint8_t CheckDeviceCapIsValid(BundleInfo *bundleInfo);
    bool CheckAbilityCapIsValid(AbilityInfo *abilityInfo, char sysCaps[][MAX_SYSCAP_NAME_LEN], int32_t sysNum);

    std::string codeDirPath_;
    std::string dataDirPath_;
};

#define CHECK_PRO_RESULT(errcode, bundleInfo, permissions, abilityRes)       \
    do {                                                                     \
        if ((errcode) != ERR_OK) {                                             \
            BundleInfoUtils::FreeBundleInfo(bundleInfo);                     \
            AdapterFree((permissions).permissionTrans);                        \
            AdapterFree(abilityRes);                                         \
            return errcode;                                                  \
        }                                                                    \
    } while (0)

#define CLEAR_INSTALL_ENV(bundleInfo)                                        \
    do {                                                                     \
        if ((bundleInfo) != nullptr) {                                         \
            BundleDaemonClient::GetInstance().RemoveInstallDirectory(        \
                (bundleInfo)->codePath, (bundleInfo)->dataPath, false);          \
            BundleInfoUtils::FreeBundleInfo(bundleInfo);                     \
        }                                                                    \
    } while (0)

#define CHECK_PRO_PART_ROLLBACK(errcode, path, permissions, bundleInfo, abilityRes)      \
    do {                                                                                 \
        if ((errcode) != ERR_OK && (bundleInfo) != nullptr) {                                \
            BundleDaemonClient::GetInstance().RemoveFile((path).c_str());                  \
            BundleInfoUtils::FreeBundleInfo(bundleInfo);                                 \
            AdapterFree((permissions).permissionTrans);                                    \
            AdapterFree(abilityRes);                                                     \
            return errcode;                                                              \
        }                                                                                \
    } while (0)

#define CHECK_PRO_ROLLBACK(errcode, permissions, bundleInfo, abilityRes, randStr)        \
    do {                                                                                 \
        if ((errcode) != ERR_OK && (bundleInfo) != nullptr) {                                \
            AdapterFree((permissions).permissionTrans);                                    \
            AdapterFree(abilityRes);                                                     \
            ManagerService::GetInstance().RemoveBundleInfo((bundleInfo)->bundleName);      \
            BundleUtil::DeleteJsonFile((bundleInfo)->bundleName, randStr);                 \
            CLEAR_INSTALL_ENV(bundleInfo);                                               \
            return errcode;                                                              \
        }                                                                                \
    } while (0)
} // namespace OHOS
#endif // OHOS_BUNDLE_INSTALLER_H
