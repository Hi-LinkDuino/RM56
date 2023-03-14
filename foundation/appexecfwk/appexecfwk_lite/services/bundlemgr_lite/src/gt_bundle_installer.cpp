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

#include "gt_bundle_installer.h"
#include "app_verify_pub.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
#include "appexecfwk_errors.h"
#include "bundle_util.h"
#include "bundlems_log.h"
#include "cstdio"
#include "dirent.h"
#include "fcntl.h"
#include "global.h"
#include "gt_bundle_extractor.h"
#include "gt_bundle_manager_service.h"
#include "gt_bundle_parser.h"
#include "sys/stat.h"
#include "unistd.h"
#include "utils.h"

namespace OHOS {
const char MATCHED_ALL_STR[] = ".*";
const uint8_t OPERATION_DOING = 200;
const uint8_t BMS_FIRST_FINISHED_PROCESS = 10;
const uint8_t BMS_SECOND_FINISHED_PROCESS = 50;
const uint8_t BMS_THIRD_FINISHED_PROCESS = 60;
const uint8_t BMS_FOURTH_FINISHED_PROCESS = 70;
const uint8_t BMS_FIFTH_FINISHED_PROCESS = 80;
const uint8_t BMS_SIXTH_FINISHED_PROCESS = 90;
const uint8_t RADN_NUM = 16;

uint8_t GtBundleInstaller::PreCheckBundle(const char *path, int32_t &fp, SignatureInfo &signatureInfo,
    uint32_t &fileSize, uint8_t bundleStyle)
{
    // check install file path and file whether is valid
    if (!BundleUtil::IsFile(path)) {
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_NOT_EXISTS;
    }

    uint32_t size = BundleUtil::GetFileSize(path);
    if (size == 0) {
        return ERR_APPEXECFWK_INSTALL_FAILED_BAD_FILE;
    }

    // get fp and bundleName from install file
    char *bundleName = nullptr;
    uint8_t errorCode = GtBundleExtractor::ExtractBundleParam(path, fp, &bundleName);
    if (errorCode != ERR_OK) {
        UI_Free(bundleName);
        return errorCode;
    }

    errorCode = VerifySignature(path, signatureInfo, fileSize, bundleStyle);
    if (errorCode != ERR_OK) {
        UI_Free(bundleName);
        return errorCode;
    }
    // reshape appId
    if ((signatureInfo.appId != nullptr) && strstr(signatureInfo.appId, MATCHED_ALL_STR) == nullptr) {
        if (!BundleUtil::StartWith(signatureInfo.appId, bundleName)) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "bundleName does not match the appId!");
            UI_Free(bundleName);
            return ERR_APPEXECFWK_INSTALL_FAILED_INVALID_PROVISIONINFO;
        }
    }
    if ((signatureInfo.appId != nullptr) && strstr(signatureInfo.appId, bundleName) == nullptr) {
        signatureInfo.appId = ReshapeAppId(bundleName, signatureInfo.appId);
        if (signatureInfo.appId == nullptr) {
            UI_Free(bundleName);
            return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
        }
    }

    // check number of current installed third bundles whether is to MAX_THIRD_BUNDLE_NUMBER
    if (bundleStyle == THIRD_APP_FLAG) {
        uint32_t numOfBundles = GtManagerService::GetInstance().GetNumOfThirdBundles();
        if (GtManagerService::GetInstance().QueryBundleInfo(bundleName) == nullptr &&
            numOfBundles >= MAX_THIRD_BUNDLE_NUMBER) {
            UI_Free(bundleName);
            return ERR_APPEXECFWK_INSTALL_FAILED_EXCEED_MAX_BUNDLE_NUMBER;
        }
    }
    UI_Free(bundleName);
    return ERR_OK;
}

uint8_t GtBundleInstaller::VerifySignature(const char *path, SignatureInfo &signatureInfo, uint32_t &fileSize,
    uint8_t bundleStyle)
{
#ifndef __LITEOS_M__
    VerifyResult verifyResult;
    // verify signature
    (void) APPVERI_SetDebugMode(true);
    int32_t ret = (bundleStyle == THIRD_APP_FLAG) ? APPVERI_AppVerify(path, &verifyResult) :
        APPVERI_AppVerify(path, &verifyResult);
    HILOG_INFO(HILOG_MODULE_AAFWK, "[BMS] APPVERI_AppVerify is %d", ret);
    uint8_t errorCode = SwitchErrorCode(ret);
    if (errorCode != ERR_OK) {
        return errorCode;
    }
    fileSize = APPVERI_GetUnsignedFileLength(path);
    signatureInfo.appId = Utils::Strdup(verifyResult.profile.appid);
    signatureInfo.bundleName = Utils::Strdup(verifyResult.profile.bundleInfo.bundleName);
    if (signatureInfo.appId == nullptr || signatureInfo.bundleName == nullptr) {
        APPVERI_FreeVerifyRst(&verifyResult);
        FREE_SIGNATUREINFO(signatureInfo);
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }
    signatureInfo.restricNum = verifyResult.profile.permission.restricNum;
    if (signatureInfo.restricNum == 0) {
        APPVERI_FreeVerifyRst(&verifyResult);
        return ERR_OK;
    }
    char **restricPermission = verifyResult.profile.permission.restricPermission;
    signatureInfo.restricPermission = reinterpret_cast<char **>(AdapterMalloc(sizeof(char *) *
        signatureInfo.restricNum));
    if (signatureInfo.restricPermission == nullptr) {
        APPVERI_FreeVerifyRst(&verifyResult);
        FREE_SIGNATUREINFO(signatureInfo);
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }
    for (int i = 0; i < signatureInfo.restricNum; i++) {
        *(signatureInfo.restricPermission + i) = Utils::Strdup(*(restricPermission + i));
        if (*(signatureInfo.restricPermission + i) == nullptr) {
            APPVERI_FreeVerifyRst(&verifyResult);
            FREE_SIGNATUREINFO(signatureInfo);
            return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
        }
    }
    APPVERI_FreeVerifyRst(&verifyResult);
#endif
    return ERR_OK;
}

uint8_t GtBundleInstaller::SwitchErrorCode(int32_t errorCode)
{
    if (errorCode >= V_ERR_GET_CERT_INFO && errorCode <= V_ERR_GET_CERT_TYPE) {
        return ERR_APPEXECFWK_INSTALL_FAILED_PARSE_APP_SIGNATURE_ERROR;
    } else if (errorCode >= V_ERR_GET_PROFILE_DATA && errorCode <= V_ERR_INVALID_DEVID) {
        return ERR_APPEXECFWK_INSTALL_FAILED_PARSE_PROFILE_SIGNATURE_ERROR;
    } else if (errorCode >= V_ERR_FILE_OPEN && errorCode <= V_ERR_FILE_LENGTH) {
        return ERR_APPEXECFWK_INSTALL_FAILED_OPERATE_SIGNED_FILE_ERROR;
    } else if ((errorCode >= V_ERR_MEMSET && errorCode <= V_ERR_MALLOC) || errorCode == V_ERR) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_SIGNATURE_ERROR;
    } else {
        return ERR_OK;
    }
}

uint8_t GtBundleInstaller::Install(const char *path, InstallerCallback installerCallback)
{
    if (path == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_PARAM_ERROR;
    }

    InstallRecord installRecord = {
        .bundleName = nullptr,
        .codePath = nullptr,
        .appId = nullptr,
#ifdef BC_TRANS_ENABLE
        .jsEngineVersion = nullptr,
        .transformResult = -1,
#endif
        .versionCode = -1
    };
    // process bundle install
    uint8_t bundleStyle = 0;
    if (BundleUtil::StartWith(path, SYSTEM_BUNDLE_PATH)) {
        bundleStyle = SYSTEM_APP_FLAG;
    } else if (BundleUtil::StartWith(path, THIRD_SYSTEM_BUNDLE_PATH)) {
        bundleStyle = THIRD_SYSTEM_APP_FLAG;
    } else {
        bundleStyle = THIRD_APP_FLAG;
    }
    char randStr[RADN_NUM] = { 0 };
    BundleUtil::CreateRandStr(randStr, RADN_NUM);
    if (strlen(randStr) == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] create random str fail!");
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    uint8_t errorCode = ProcessBundleInstall(path, randStr, installRecord, bundleStyle, installerCallback);
    if (errorCode != ERR_OK) {
        return errorCode;
    }
    (void) GtManagerService::GetInstance().ReportInstallCallback(OPERATION_DOING,
        0, BMS_SIXTH_FINISHED_PROCESS, installerCallback);

    // rename bundle.json
    if (!RenameJsonFile(installRecord.bundleName, randStr)) {
        BundleInfo *bundleInfo = GtManagerService::GetInstance().QueryBundleInfo(installRecord.bundleName);
        CLEAR_INSTALL_ENV(bundleInfo);
        return ERR_APPEXECFWK_INSTALL_FAILED_RENAME_FILE_ERROR;
    }
    // if third system bundle, it need to record bundleName in THIRD_SYSTEM_BUNDLE_JSON
    if (bundleStyle == THIRD_SYSTEM_APP_FLAG && !CheckIsThirdSystemBundle(installRecord.bundleName)) {
        RecordThirdSystemBundle(installRecord.bundleName, THIRD_SYSTEM_BUNDLE_JSON);
    }

    if (bundleStyle == THIRD_APP_FLAG) {
        GtManagerService::GetInstance().AddNumOfThirdBundles();
    }
    // record app info event when install app
    return ERR_OK;
}

uint8_t GtBundleInstaller::ProcessBundleInstall(const char *path, const char *randStr, InstallRecord &installRecord,
    uint8_t bundleStyle, InstallerCallback installerCallback)
{
    SignatureInfo signatureInfo;
    signatureInfo = {.bundleName = nullptr, .appId = nullptr, .restricPermission = nullptr, .restricNum = 0};
    int32_t fp = -1;
    uint32_t fileSize = 0;
    // check bundle whether is valid
    Permissions permissions = {.permNum = 0, .permissionTrans = nullptr};
    BundleRes bundleRes = {
        .bundleName = nullptr, .moduleDescriptionId = 0, .abilityRes = nullptr, .totalNumOfAbilityRes = 0
    };
    BundleInfo *bundleInfo = nullptr;
    (void) GtManagerService::GetInstance().ReportInstallCallback(OPERATION_DOING, 0,
        BMS_FIRST_FINISHED_PROCESS, installerCallback);
    uint8_t errorCode = PreCheckBundle(path, fp, signatureInfo, fileSize, bundleStyle);
    CHECK_PRO_RESULT(errorCode, fp, permissions, bundleInfo, signatureInfo);
    (void) GtManagerService::GetInstance().ReportInstallCallback(OPERATION_DOING,
        0, BMS_SECOND_FINISHED_PROCESS, installerCallback);
#ifdef __LITEOS_M__
    fp = open(path, O_RDONLY, S_IREAD);
    if (fp < 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] process bundle install fp open fail");
        errorCode = ERR_APPEXECFWK_INSTALL_FAILED_FILE_NOT_EXISTS;
    }
    CHECK_PRO_RESULT(errorCode, fp, permissions, bundleInfo, signatureInfo);
#endif
    // parse HarmoyProfile.json, get permissions and bundleInfo
    errorCode = GtBundleParser::ParseHapProfile(fp, fileSize, permissions, bundleRes, &bundleInfo);
    CHECK_PRO_RESULT(errorCode, fp, permissions, bundleInfo, signatureInfo);
    SetCurrentBundle(bundleInfo->bundleName);
    // terminate current runing app
    uint32_t labelId = (bundleRes.abilityRes != nullptr) ? bundleRes.abilityRes->labelId : 0;
    uint32_t iconId = (bundleRes.abilityRes != nullptr) ? bundleRes.abilityRes->iconId : 0;
    AdapterFree(bundleRes.abilityRes);
    // check signatureInfo
    errorCode = CheckProvisionInfoIsValid(signatureInfo, permissions, bundleInfo->bundleName);
    CHECK_PRO_RESULT(errorCode, fp, permissions, bundleInfo, signatureInfo);
    installRecord.codePath = bundleInfo->codePath;
    installRecord.bundleName = bundleInfo->bundleName;
    installRecord.appId = signatureInfo.appId;
    installRecord.versionCode = bundleInfo->versionCode;
    bundleInfo->appId = Utils::Strdup(signatureInfo.appId);
    // check version when in update status
    errorCode = CheckVersionAndSignature(installRecord.bundleName, installRecord.appId, bundleInfo);
    CHECK_PRO_RESULT(errorCode, fp, permissions, bundleInfo, signatureInfo);
    // restore bin data to file
    char *tmpCodePathComp[] = {installRecord.codePath, const_cast<char *>(randStr)};
    char *tmpCodePath = BundleUtil::Strscat(tmpCodePathComp, sizeof(tmpCodePathComp) / sizeof(char *));
    errorCode = (tmpCodePath == nullptr) ? ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR : ERR_OK;
    CHECK_PRO_RESULT(errorCode, fp, permissions, bundleInfo, signatureInfo);
    errorCode = GtBundleExtractor::ExtractHap(tmpCodePath, installRecord.bundleName, fp, fileSize, bundleStyle);
    close(fp);
    CHECK_PRO_PART_ROLLBACK(errorCode, tmpCodePath, permissions, bundleInfo, signatureInfo);
    (void) GtManagerService::GetInstance().ReportInstallCallback(OPERATION_DOING, 0,
        BMS_THIRD_FINISHED_PROCESS, installerCallback);
    // get js engine version
#ifdef BC_TRANS_ENABLE
    char *jsEngineVersion = get_jerry_version_no();
    errorCode = (jsEngineVersion != nullptr) ? ERR_OK : ERR_APPEXECFWK_INSTALL_FAILED_GET_JS_ENGINE_VERSION_ERROR;
    CHECK_PRO_PART_ROLLBACK(errorCode, tmpCodePath, permissions, bundleInfo, signatureInfo);
    installRecord.jsEngineVersion = jsEngineVersion;
    // try to transform js file to bc file
    errorCode = TransformJsToBc(tmpCodePath, installRecord);
    CHECK_PRO_PART_ROLLBACK(errorCode, tmpCodePath, permissions, bundleInfo, signatureInfo);
#endif
    (void) GtManagerService::GetInstance().ReportInstallCallback(OPERATION_DOING, 0,
        BMS_FOURTH_FINISHED_PROCESS, installerCallback);
    // rename install path and record install infomation
    bool isUpdate = GtManagerService::GetInstance().QueryBundleInfo(installRecord.bundleName) != nullptr;
    errorCode = HandleFileAndBackUpRecord(installRecord, tmpCodePath, randStr, bundleInfo->dataPath, isUpdate);
    AdapterFree(tmpCodePath);
    CHECK_PRO_ROLLBACK(errorCode, permissions, bundleInfo, signatureInfo, randStr);

    // move rawfile to data path when rawfile is exists
    errorCode = MoveRawFileToDataPath(bundleInfo);
    CHECK_PRO_ROLLBACK(errorCode, permissions, bundleInfo, signatureInfo, randStr);
    (void) GtManagerService::GetInstance().ReportInstallCallback(OPERATION_DOING, 0,
        BMS_FIFTH_FINISHED_PROCESS, installerCallback);
    // store permissions
    errorCode = StorePermissions(installRecord.bundleName, permissions.permissionTrans, permissions.permNum,
        isUpdate);
    CHECK_PRO_ROLLBACK(errorCode, permissions, bundleInfo, signatureInfo, randStr);
    // update bundle Info
    errorCode = UpdateBundleInfo(bundleStyle, labelId, iconId, bundleInfo, isUpdate);
    CHECK_PRO_ROLLBACK(errorCode, permissions, bundleInfo, signatureInfo, randStr);
    // free memory
    UI_Free(permissions.permissionTrans);
    FREE_SIGNATUREINFO(signatureInfo);
    return ERR_OK;
}

uint8_t GtBundleInstaller::MoveRawFileToDataPath(const BundleInfo *bundleInfo)
{
    if (bundleInfo == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    char *srcRawFilePathComp[] = {
        const_cast<char *>(bundleInfo->codePath), const_cast<char *>(ASSETS),
        const_cast<char *>(bundleInfo->moduleInfos[0].moduleName), const_cast<char *>(RESOURCES_RAW_FILE)
    };
    char *srcRawFilePath = BundleUtil::Strscat(srcRawFilePathComp, sizeof(srcRawFilePathComp) / sizeof(char *));
    if (srcRawFilePath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] srcRawFilePath is nullptr");
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    if (!BundleUtil::IsDir(srcRawFilePath)) {
        HILOG_INFO(HILOG_MODULE_AAFWK, "[BMS] rawfile is not exists!");
        AdapterFree(srcRawFilePath);
        return ERR_OK;
    }

    char *dstRawFilePathComp[] = {const_cast<char *>(bundleInfo->dataPath), const_cast<char *>(RAW_FILE)};
    char *dstRawFilePath = BundleUtil::Strscat(dstRawFilePathComp, sizeof(dstRawFilePathComp) / sizeof(char *));
    if (dstRawFilePath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] dstRawFilePath is nullptr");
        AdapterFree(srcRawFilePath);
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    if (!BundleUtil::RenameDir(srcRawFilePath, dstRawFilePath)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] frename rawfile fail!");
        AdapterFree(srcRawFilePath);
        AdapterFree(dstRawFilePath);
        return ERR_APPEXECFWK_INSTALL_FAILED_RENAME_DIR_ERROR;
    }
    AdapterFree(srcRawFilePath);
    AdapterFree(dstRawFilePath);
    return ERR_OK;
}

char *GtBundleInstaller::ReshapeAppId(const char *bundleName, char *appId)
{
    if (appId == nullptr || bundleName == nullptr) {
        return nullptr;
    }

    char *pos = appId;
    for (int32_t i = 0; appId[i] != '\0'; i++) {
        if (appId[i] == '_') {
            break;
        }
        pos++;
    }

    char *appIdComp[] = {const_cast<char *>(bundleName), pos};
    char *newAppid = BundleUtil::Strscat(appIdComp, sizeof(appIdComp) / sizeof(char *));
    AdapterFree(appId);
    return newAppid;
}

uint8_t GtBundleInstaller::CheckProvisionInfoIsValid(const SignatureInfo &signatureInfo, const Permissions &permissions,
    const char *bundleName)
{
    if (bundleName == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    if (!MatchBundleName(bundleName, signatureInfo.bundleName)) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INVALID_PROVISIONINFO;
    }
    return ERR_OK;
}


bool GtBundleInstaller::MatchBundleName(const char *bundleName, const char *matchedBundleName)
{
    if (bundleName == nullptr || matchedBundleName == nullptr) {
        return false;
    }

    if (strcmp(matchedBundleName, MATCHED_ALL_STR) == 0 || strcmp(bundleName, matchedBundleName) == 0) {
        return true;
    }

    char *pos = const_cast<char *>(matchedBundleName);
    for (uint32_t i = 0; matchedBundleName[i] != '\0' && bundleName[i] != '\0'; i++, pos++) {
        if (bundleName[i] != matchedBundleName[i]) {
            if (strcmp(pos, MATCHED_ALL_STR) == 0) {
                return true;
            }
            return false;
        } else if (strcmp(pos, MATCHED_ALL_STR) == 0) {
            return true;
        }
    }
    return false;
}

bool GtBundleInstaller::MatchPermissions(char ** restrictedPermissions, uint32_t restricNum,
    PermissionTrans *permissionsTrans, int32_t permTransNum)
{
    if (permTransNum == 0) {
        return true;
    }

    if (permissionsTrans == nullptr || restrictedPermissions == nullptr) {
        return false;
    }

    PermissionTrans *perm = permissionsTrans;
    for (int32_t i = 0; i < permTransNum; i++) {
        bool isMatched = false;
        for (int32_t j = 0; j < restricNum; j++) {
            if (((perm + i)->name != nullptr) && strcmp(*(restrictedPermissions + j), (perm + i)->name) == 0) {
                isMatched = true;
                break;
            }
        }
        if (!isMatched) {
            return false;
        }
    }
    return true;
}

uint8_t GtBundleInstaller::HandleFileAndBackUpRecord(const InstallRecord &record, const char *tmpPath,
    const char *randStr, const char *dataPath, bool isUpdate)
{
    if (tmpPath == nullptr || dataPath == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }
    // delete old bundleRes
    GtManagerService::GetInstance().RemoveBundleResList(record.bundleName);

    if (!BundleUtil::RenameDir(tmpPath, record.codePath)) {
        BundleUtil::RemoveDir(tmpPath);
        return ERR_APPEXECFWK_INSTALL_FAILED_RENAME_DIR_ERROR;
    }

    char *bundleTmpJsonPathComp[] = {
        const_cast<char *>(JSON_PATH), const_cast<char *>(record.bundleName), const_cast<char *>(randStr),
        const_cast<char *>(JSON_SUFFIX)
    };
    char *bundleTmpJsonPath = BundleUtil::Strscat(bundleTmpJsonPathComp,
        sizeof(bundleTmpJsonPathComp) / sizeof(char *));
    if (bundleTmpJsonPath == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_RECORD_INFO_ERROR;
    }

    if (!BackUpInstallRecord(record, bundleTmpJsonPath)) {
        unlink(bundleTmpJsonPath);
        AdapterFree(bundleTmpJsonPath);
        return ERR_APPEXECFWK_INSTALL_FAILED_RECORD_INFO_ERROR;
    }
    AdapterFree(bundleTmpJsonPath);

    if (!isUpdate) {
        (void) BundleUtil::RemoveDir(dataPath);
        if (!BundleUtil::MkDirs(dataPath)) {
            return ERR_APPEXECFWK_INSTALL_FAILED_CREATE_DATA_DIR_ERROR;
        }
    }
    return ERR_OK;
}

#ifdef BC_TRANS_ENABLE
uint8_t GtBundleInstaller::TransformJsToBc(const char *codePath, InstallRecord &record)
{
    if (codePath == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    char *jsPathComp[] = {const_cast<char *>(codePath), const_cast<char *>(ASSET_JS_PATH)};
    char *jsPath = BundleUtil::Strscat(jsPathComp, sizeof(jsPathComp) / sizeof(char *));
    if (jsPath == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }
    EXECRES result = walk_directory(jsPath);
    HILOG_INFO(HILOG_MODULE_AAFWK, "[BMS] transform js to bc when install, result is %d", result);
    if (result != EXCE_ACE_JERRY_EXEC_OK) {
        AdapterFree(jsPath);
        return ERR_APPEXECFWK_INSTALL_FAILED_TRANSFORM_BC_FILE_ERROR;
    }
    AdapterFree(jsPath);
    record.transformResult = 0;
    return ERR_OK;
}
#endif

uint8_t GtBundleInstaller::UpdateBundleInfo(uint8_t bundleStyle, uint32_t labelId, uint32_t iconId,
    BundleInfo *bundleInfo, bool isUpdate)
{
    if (bundleInfo == nullptr || bundleInfo->abilityInfo == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    if (!BundleUtil::IsDir(bundleInfo->abilityInfo->srcPath)) {
        return ERR_APPEXECFWK_INSTALL_FAILED_PARSE_JS_DIR_ERROR;
    }

    uint8_t errorCode = GtBundleParser::ConvertResInfoToBundleInfo(bundleInfo->codePath, labelId, iconId, bundleInfo);
    if (errorCode != ERR_OK) {
        return errorCode;
    }
    if (!isUpdate) {
        if (bundleStyle == SYSTEM_APP_FLAG) {
            bundleInfo->isSystemApp = true;
            GtManagerService::GetInstance().AddBundleInfo(bundleInfo);
        } else if (bundleStyle == THIRD_SYSTEM_APP_FLAG) {
            bundleInfo->isSystemApp = false;
            GtManagerService::GetInstance().AddBundleInfo(bundleInfo);
        } else {
            bundleInfo->isSystemApp = false;
            GtManagerService::GetInstance().AddBundleInfo(bundleInfo);
        }
    } else {
        BundleInfo *oldBundleInfo = GtManagerService::GetInstance().QueryBundleInfo(bundleInfo->bundleName);
        if (oldBundleInfo == nullptr) {
            return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
        }
        bundleInfo->isSystemApp = oldBundleInfo->isSystemApp;

        if (!GtManagerService::GetInstance().UpdateBundleInfo(bundleInfo)) {
            return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
        }
    }
    // update bundle res list
    return AddBundleResList(bundleInfo->bundleName, labelId, iconId);
}

uint8_t GtBundleInstaller::AddBundleResList(const char *bundleName, uint32_t labelId, uint32_t iconId)
{
    if (labelId == 0 && iconId == 0) {
        return ERR_OK;
    }

    if (bundleName == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    BundleRes *bundleRes = reinterpret_cast<BundleRes *>(AdapterMalloc(sizeof(BundleRes)));
    if (bundleRes == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    bundleRes->abilityRes = reinterpret_cast<AbilityRes *>(AdapterMalloc(sizeof(AbilityRes)));
    if (bundleRes->abilityRes == nullptr) {
        AdapterFree(bundleRes);
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }
    bundleRes->bundleName = const_cast<char *>(bundleName);
    bundleRes->abilityRes->labelId = labelId;
    bundleRes->abilityRes->iconId = iconId;

    GtManagerService::GetInstance().AddBundleResList(bundleRes);
    return ERR_OK;
}

uint8_t GtBundleInstaller::Uninstall(const char *bundleName)
{
    if (bundleName == nullptr) {
        return ERR_APPEXECFWK_UNINSTALL_FAILED_PARAM_ERROR;
    }

    BundleInfo *bundleInfo = GtManagerService::GetInstance().QueryBundleInfo(bundleName);
    if (bundleInfo == nullptr) {
        return ERR_APPEXECFWK_UNINSTALL_FAILED_BUNDLE_NOT_EXISTS;
    }

    if (bundleInfo->isSystemApp) {
        return ERR_APPEXECFWK_UNINSTALL_FAILED_BUNDLE_NOT_UNINSTALLABLE;
    }

    char bundleJsonPath[PATH_LENGTH] = { 0 };
    if (sprintf_s(bundleJsonPath, PATH_LENGTH, "%s%s%s", JSON_PATH, bundleName, JSON_SUFFIX) < 0) {
        return ERR_APPEXECFWK_UNINSTALL_FAILED_INTERNAL_ERROR;
    }
#ifndef __LITEOS_M__
    if (DeletePermissions(const_cast<char *>(bundleName)) < 0) {
        return ERR_APPEXECFWK_UNINSTALL_FAILED_DELETE_PERMISSIONS_ERROR;
    }
#endif
    bool res = CheckIsThirdSystemBundle(bundleName);
    if (!(BundleUtil::RemoveDir(bundleInfo->codePath) && BundleUtil::RemoveDir(bundleInfo->dataPath))) {
        GtManagerService::GetInstance().RemoveBundleInfo(bundleName);
        GtManagerService::GetInstance().RemoveBundleResList(bundleName);
        if (!res) {
            GtManagerService::GetInstance().ReduceNumOfThirdBundles();
        }
        return ERR_APPEXECFWK_UNINSTALL_FAILED_DELETE_DIRS_ERROR;
    }

    GtManagerService::GetInstance().RemoveBundleInfo(bundleName);
    GtManagerService::GetInstance().RemoveBundleResList(bundleName);
    if (!res) {
        GtManagerService::GetInstance().ReduceNumOfThirdBundles();
    }
    if (unlink(bundleJsonPath) < 0) {
        return ERR_APPEXECFWK_UNINSTALL_FAILED_DELETE_RECORD_INFO_ERROR;
    }

    if (res) {
        RecordThirdSystemBundle(bundleName, UNINSTALL_THIRD_SYSTEM_BUNDLE_JSON);
    }

    return ERR_OK;
}

bool GtBundleInstaller::CheckIsThirdSystemBundle(const char *bundleName)
{
    if (bundleName == nullptr) {
        return false;
    }
    cJSON *root = BundleUtil::GetJsonStream(THIRD_SYSTEM_BUNDLE_JSON);
    if (root == nullptr) {
        return false;
    }

    cJSON *son = cJSON_GetObjectItem(root, JSON_MAIN_KEY);
    if (!cJSON_IsArray(son)) {
        (void) unlink(THIRD_SYSTEM_BUNDLE_JSON);
        cJSON_Delete(root);
        return false;
    }

    cJSON *item = nullptr;
    cJSON_ArrayForEach(item, son) {
        if (!cJSON_IsString(item)) {
            cJSON_Delete(root);
            return false;
        }
        if ((item->valuestring != nullptr) && strcmp(bundleName, item->valuestring) == 0) {
            cJSON_Delete(root);
            return true;
        }
    }
    cJSON_Delete(root);
    return false;
}

void GtBundleInstaller::RecordThirdSystemBundle(const char *bundleName, const char *path)
{
    if (bundleName == nullptr || path == nullptr) {
        return;
    }
    if (BundleUtil::IsFile(path)) {
        cJSON *root = BundleUtil::GetJsonStream(path);
        if (root == nullptr) {
            return;
        }

        cJSON *son = cJSON_GetObjectItem(root, JSON_MAIN_KEY);
        if (!cJSON_IsArray(son)) {
            unlink(path);
            cJSON_Delete(root);
            InitThirdSystemBundleRecord(bundleName, path);
            return;
        }

        cJSON *str = cJSON_CreateString(bundleName);
        if (str == nullptr) {
            cJSON_Delete(root);
            return;
        }
        cJSON_AddItemToArray(son, str);

        (void) BundleUtil::StoreJsonContentToFile(path, root);
        cJSON_Delete(root);
    } else {
        InitThirdSystemBundleRecord(bundleName, path);
    }
}

void GtBundleInstaller::InitThirdSystemBundleRecord(const char *bundleName, const char *path)
{
    if (bundleName == nullptr || path == nullptr) {
        return;
    }
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }

    cJSON *son = cJSON_CreateArray();
    if (son == nullptr) {
        cJSON_Delete(root);
        return;
    }

    if (!cJSON_AddItemToObject(root, JSON_MAIN_KEY, son)) {
        cJSON_Delete(son);
        cJSON_Delete(root);
        return;
    }

    cJSON *str = cJSON_CreateString(bundleName);
    if (str == nullptr) {
        cJSON_Delete(root);
        return;
    }

    if (!cJSON_AddItemToArray(son, str)) {
        cJSON_Delete(str);
        cJSON_Delete(root);
        return;
    }

    BundleUtil::StoreJsonContentToFile(path, root);
    cJSON_Delete(root);
}

bool GtBundleInstaller::BackUpInstallRecord(const InstallRecord &record, const char *jsonPath)
{
    if (jsonPath == nullptr) {
        return false;
    }

    cJSON *object = BundleUtil::ConvertInstallRecordToJson(record);
    if (object == nullptr) {
        return false;
    }

    if (!BundleUtil::StoreJsonContentToFile(jsonPath, object)) {
        cJSON_Delete(object);
        return false;
    }
    cJSON_Delete(object);
    return true;
}

uint8_t GtBundleInstaller::StorePermissions(const char *bundleName, PermissionTrans *permissions, int32_t permNum,
    bool isUpdate)
{
    if (permNum == 0) {
#ifndef __LITEOS_M__
        if (isUpdate) {
            int32_t ret = DeletePermissions(bundleName);
            HILOG_INFO(HILOG_MODULE_AAFWK, "[BMS] delete permissions, result is %d", ret);
        }
#endif
        return ERR_OK;
    }

    if (bundleName == nullptr || permissions == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }

    if (!BundleUtil::IsDir(PERMISSIONS_PATH)) {
        BundleUtil::MkDirs(PERMISSIONS_PATH);
    }
#ifndef __LITEOS_M__
    if (SaveOrUpdatePermissions(const_cast<char *>(bundleName), permissions, permNum,
        static_cast<IsUpdate>(isUpdate)) != 0) {
            return ERR_APPEXECFWK_INSTALL_FAILED_STORE_PERMISSIONS_ERROR;
    }
#endif
    return ERR_OK;
}

uint8_t GtBundleInstaller::CheckVersionAndSignature(const char *bundleName, const char *appId, BundleInfo *bundleInfo)
{
    if (bundleName == nullptr || appId == nullptr || bundleInfo == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
    }
    BundleInfo *oldBundleInfo = GtManagerService::GetInstance().QueryBundleInfo(bundleName);
    if (oldBundleInfo != nullptr) {
        if (oldBundleInfo->versionCode > bundleInfo->versionCode) {
            return ERR_APPEXECFWK_INSTALL_FAILED_VERSION_DOWNGRADE;
        }
        char *oldAppId = BundleUtil::GetValueFromBundleJson(bundleName, JSON_SUB_KEY_APPID);
        if (oldAppId == nullptr) {
            return ERR_APPEXECFWK_INSTALL_FAILED_INTERNAL_ERROR;
        }
        if (strcmp(oldAppId, appId) != 0) {
            AdapterFree(oldAppId);
            return ERR_APPEXECFWK_INSTALL_FAILED_INCOMPATIBLE_SIGNATURE;
        }
        AdapterFree(oldAppId);
    }
    return ERR_OK;
}

bool GtBundleInstaller::RenameJsonFile(const char *fileName, const char *randStr)
{
    if (fileName == nullptr || randStr == nullptr) {
        return false;
    }

    char *tmpJsonPathComp[] = {
        const_cast<char *>(JSON_PATH), const_cast<char *>(fileName), const_cast<char *>(randStr),
        const_cast<char *>(JSON_SUFFIX)
    };
    char *tmpJsonPath = BundleUtil::Strscat(tmpJsonPathComp, sizeof(tmpJsonPathComp) / sizeof(char *));
    if (tmpJsonPath == nullptr) {
        return false;
    }

    if (!BundleUtil::IsFile(tmpJsonPath)) {
        AdapterFree(tmpJsonPath);
        return false;
    }

    char *jsonPathComp[] = {
        const_cast<char *>(JSON_PATH), const_cast<char *>(fileName), const_cast<char *>(JSON_SUFFIX)
    };
    char *jsonPath = BundleUtil::Strscat(jsonPathComp, sizeof(jsonPathComp) / sizeof(char *));
    if (jsonPath == nullptr) {
        AdapterFree(tmpJsonPath);
        return false;
    }

    if (!BundleUtil::RenameFile(tmpJsonPath, jsonPath)) {
        AdapterFree(jsonPath);
        AdapterFree(tmpJsonPath);
        return false;
    }

    AdapterFree(jsonPath);
    AdapterFree(tmpJsonPath);
    return true;
}
} // namespace OHOS