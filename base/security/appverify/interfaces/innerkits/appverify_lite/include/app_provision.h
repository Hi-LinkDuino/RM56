/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SECURITY_APP_PROVISION_H
#define SECURITY_APP_PROVISION_H

#include "app_verify_pub.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define APP_STORE "Huawei App Store"

#define DEBUG_TYPE "debug"

#define RELEASE_TYPE "release"

#define MAX_UDID_NUM 100

#define UDID_VERIFY_BYTES 64
int ParseProfile(const char *buf, int len, ProfileProf *pf);
void ProfFreeData(ProfileProf *pf);
int VerifyProfileContent(const ProfileProf *pf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
