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

#ifndef OHOS_GT_BUNDLE_INSTALLER_H
#define OHOS_GT_BUNDLE_INSTALLER_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#include "bundle_common.h"
#include "bundle_info.h"
#include "bundle_info_utils.h"
#include "bundle_manager.h"
#include "bundle_manager_inner.h"
#include "stdint.h"

namespace OHOS {
class GtBundleInstaller {
public:
    GtBundleInstaller() = default;
    ~GtBundleInstaller() = default;

    uint8_t Install(const char *path, InstallerCallback installerCallback);
    uint8_t Uninstall(const char *bundleName);
private:
    uint8_t PreCheckBundle(const char *path, int32_t &fp, SignatureInfo &signatureInfo, uint32_t &fileSize,
        uint8_t bundleStyle);
    uint8_t ProcessBundleInstall(const char *path, const char *randStr, InstallRecord &installRecord,
        uint8_t bundleStyle, InstallerCallback installerCallback);
    uint8_t HandleFileAndBackUpRecord(const InstallRecord &record, const char *tmpPath, const char *randStr,
        const char *dataPath, bool isUpdate);
    uint8_t UpdateBundleInfo(uint8_t bundleStyle, uint32_t labelId, uint32_t iconId, BundleInfo *bundleInfo,
        bool isUpdate);
    void RecordThirdSystemBundle(const char *bundleName, const char *path);
    uint8_t StorePermissions(const char *bundleName, PermissionTrans *permissions, int32_t permNum, bool isUpdate);
    uint8_t CheckVersionAndSignature(const char *bundleName, const char *appId, BundleInfo *bundleInfo);
    bool RenameJsonFile(const char *fileName, const char *randStr);
    bool CheckIsThirdSystemBundle(const char *bundleName);
    bool BackUpInstallRecord(const InstallRecord &record, const char *jsonPath);
    void InitThirdSystemBundleRecord(const char *bundleName, const char *path);
    uint8_t VerifySignature(const char *path, SignatureInfo &signatureInfo, uint32_t &fileSize, uint8_t bundleStyle);
    bool MatchBundleName(const char *bundleName, const char *matchedBundleName);
    bool MatchPermissions(char ** restrictedPermissions, uint32_t restricNum, PermissionTrans *permissionsTrans,
        int32_t permTransNum);
    uint8_t CheckProvisionInfoIsValid(const SignatureInfo &signatureInfo, const Permissions &permissions,
        const char *bundleName);
    char *ReshapeAppId(const char *bundleName, char *appId);
    uint8_t SwitchErrorCode(int32_t errorCode);
    uint8_t AddBundleResList(const char *bundleName, uint32_t labelId, uint32_t iconId);
    uint8_t MoveRawFileToDataPath(const BundleInfo *bundleInfo);
    uint8_t TransformJsToBc(const char *codePath, InstallRecord &record);
};

#define FREE_PRO_RESOURCE(fp, permissions, bundleInfo) \
    do {                                               \
        if ((fp) >= 0) {                                 \
            close(fp);                                 \
        }                                              \
        BundleInfoUtils::FreeBundleInfo(bundleInfo);   \
        UI_Free((permissions).permissionTrans);          \
    } while (0)

#define FREE_SIGNATUREINFO(signatureInfo)                                                           \
    do {                                                                                            \
        AdapterFree((signatureInfo).appId);                                                           \
        AdapterFree(signatureInfo.bundleName);                                                      \
        BundleUtil::FreeStrArrayMemory(signatureInfo.restricPermission, signatureInfo.restricNum);  \
    } while (0)

#define CHECK_PRO_RESULT(errcode, fp, permissions, bundleInfo, signatureInfo)  \
    do {                                                                       \
        if ((errcode) != ERR_OK) {                                               \
            FREE_PRO_RESOURCE(fp, permissions, bundleInfo);                    \
            FREE_SIGNATUREINFO(signatureInfo);                                 \
            return errcode;                                                    \
        }                                                                      \
    } while (0)

#define CLEAR_INSTALL_ENV(bundleInfo)                                        \
    do {                                                                     \
        if ((bundleInfo) != nullptr) {                                         \
            BundleUtil::RemoveDir(bundleInfo->codePath);                     \
            BundleUtil::RemoveDir(bundleInfo->dataPath);                     \
            BundleInfoUtils::FreeBundleInfo(bundleInfo);                     \
        }                                                                    \
    } while (0)

#define CHECK_PRO_PART_ROLLBACK(errcode, path, permissions, bundleInfo, signatureInfo)   \
    do {                                                                                 \
        if ((errcode) != ERR_OK) {                                                         \
            BundleUtil::RemoveDir(path);                                                 \
            AdapterFree(path);                                                           \
            BundleInfoUtils::FreeBundleInfo(bundleInfo);                                 \
            FREE_SIGNATUREINFO(signatureInfo);                                           \
            UI_Free((permissions).permissionTrans);                                        \
            return errcode;                                                              \
        }                                                                                \
    } while (0)

#define CHECK_PRO_ROLLBACK(errcode, permissions, bundleInfo, signatureInfo, randStr)  \
    do {                                                                              \
        if ((errcode) != ERR_OK && (bundleInfo) != nullptr) {                             \
            FREE_SIGNATUREINFO(signatureInfo);                                        \
            UI_Free((permissions).permissionTrans);                                     \
            GtManagerService::GetInstance().RemoveBundleInfo(bundleInfo->bundleName); \
            BundleUtil::DeleteJsonFile(bundleInfo->bundleName, randStr);              \
            CLEAR_INSTALL_ENV(bundleInfo);                                            \
            return errcode;                                                           \
        }                                                                             \
    } while (0)
} // namespace OHOS
#endif // OHOS_GT_BUNDLE_INSTALLER_H