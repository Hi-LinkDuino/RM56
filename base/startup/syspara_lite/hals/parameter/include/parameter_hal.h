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

#ifndef STARTUP_SYSPARAM_PARAMETER_HAL_API_H
#define STARTUP_SYSPARAM_PARAMETER_HAL_API_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

const char *HalGetDeviceType();
const char *HalGetProductModel();
const char *HalGetManufacture();
const char *HalGetBrand();
const char *HalGetMarketName();
const char *HalGetProductSeries();
const char *HalGetSoftwareModel();
const char *HalGetHardwareModel();
const char *HalGetHardwareProfile();
const char *HalGetSerial();
const char *HalGetAbiList();
const char *HalGetBootloaderVersion();
const char *HalGetSecurityPatchTag();
const char *HalGetDisplayVersion();
const char *HalGetIncrementalVersion();
const char *HalGetOsReleaseType();
const char *HalGetSdkApiVersion();
const char *HalGetBuildRootHash();
const char *HalGetSdkApiLevel();
const char *HalGetOSName();
const char *HalGetBuildType();
const char *HalGetBuildUser();
const char *HalGetBuildHost();
const char *HalGetBuildTime();

int HalGetDevUdid(char *udid, int size);
int HalGetFirstApiVersion();
int HalGetParameter(const char *key, const char *def, char *value, unsigned int len);
int HalSetParameter(const char *key, const char *value);
int HalGetIntParameter(const char *key, int def);

int HalWaitParameter(const char *key, const char *value, int timeout);
unsigned int HalFindParameter(const char *name);
unsigned int HalGetParameterCommitId(unsigned int handle);
int HalGetParameterName(unsigned int handle, char *name, unsigned int len);
int HalGetParameterValue(unsigned int handle, char *value, unsigned int len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // STARTUP_SYSPARAM_PARAMETER_HAL_API_H
