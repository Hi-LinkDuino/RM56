/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "parameter.h"
#include <stdio.h>
#include <stdlib.h>

void ObtainProductParms()
{
    const char *bootloaderVersion = GetBootloaderVersion();
    if (bootloaderVersion != nullptr) {
        printf("The bootloaderVersion is [%s]\n", bootloaderVersion);
    }

    const char *securityPatchTag = GetSecurityPatchTag();
    if (securityPatchTag != nullptr) {
        printf("The Security Patch is [%s]\n", securityPatchTag);
    }

    const char *abiList = GetAbiList();
    if (abiList != nullptr) {
        printf("The AbiList is [%s]\n", abiList);
    }

    int sdkApiLevel = GetSdkApiVersion();
    if (sdkApiLevel != 0) {
        printf("The sdkApiLevel is [%d]\n", sdkApiLevel);
    }

    int firstApiLevel = GetFirstApiVersion();
    if (firstApiLevel != 0) {
        printf("The firstApiLevel is [%d]\n", firstApiLevel);
    }

    const char *incrementalVersion = GetIncrementalVersion();
    if (incrementalVersion != nullptr) {
        printf("The productSeries is [%s]\n", incrementalVersion);
    }

    const char *versionId = GetVersionId();
    if (versionId != nullptr) {
        printf("The VersionID is [%s]\n", versionId);
    }

    const char *buildType = GetBuildType();
    if (buildType != nullptr) {
        printf("The buildType is [%s]\n", buildType);
    }

    const char *buildUser = GetBuildUser();
    if (buildUser != nullptr) {
        printf("The buildUser is [%s]\n", buildUser);
    }

    const char *buildHost = GetBuildHost();
    if (buildHost != nullptr) {
        printf("The buildHost is [%s]\n", buildHost);
    }

    const char *buildTime = GetBuildTime();
    if (buildTime != nullptr) {
        printf("The buildTime is [%s]\n", buildTime);
    }

    const char *buildRootHash = GetBuildRootHash();
    if (buildRootHash != nullptr) {
        printf("The BuildRootHash is [%s]\n", buildRootHash);
    }	
}

int main()
{
    printf("******To Obtain Product Params Start******\n");
    const char *productType = GetDeviceType();
    if (productType != nullptr) {
        printf("The Product Type is [%s]\n", productType);
    }

    const char *manuFacture = GetManufacture();
    if (manuFacture != nullptr) {
        printf("The manuFacture is [%s]\n", manuFacture);
    }

    const char *brand = GetBrand();
    if (brand != nullptr) {
        printf("The brand is [%s]\n", brand);
    }

    const char *marketName = GetMarketName();
    if (marketName != nullptr) {
        printf("The marketName is [%s]\n", marketName);
    }

    const char *productSeries = GetProductSeries();
    if (productSeries != nullptr) {
        printf("The productSeries is [%s]\n", productSeries);
    }

    const char *softwareModel = GetSoftwareModel();
    if (softwareModel != nullptr) {
        printf("The softwareModel is [%s]\n", softwareModel);
    }

    const char *hardWareModel = GetHardwareModel();
    if (hardWareModel != nullptr) {
        printf("The HardwareModel is [%s]\n", hardWareModel);
    }

    const char *hardWareProfile = GetHardwareProfile();
    if (hardWareProfile != nullptr) {
        printf("The HardwareProfile is [%s]\n", hardWareProfile);
    }

    const char *serial = GetSerial();
    if (serial != nullptr) {
        printf("The serial is [%s]\n", serial);
    }

    const char *osName = GetOSFullName();
    if (osName != nullptr) {
        printf("The osName is [%s]\n", osName);
    }

    const char *displayVersion = GetDisplayVersion();
    if (displayVersion != nullptr) {
        printf("The OS Version is [%s]\n", displayVersion);
    }

    ObtainProductParms();	

    printf("******To Obtain Product Params End  ******\n");
    return 0;
}
