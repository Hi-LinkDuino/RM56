/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef BTM_CONTROLLER_H
#define BTM_CONTROLLER_H

#include <stdbool.h>

#include "hci/hci_def.h"

#ifdef __cplusplus
extern "C" {
#endif

int BtmInitController();
void BtmCloseController();

bool BtmIsControllerSupportedReadLocalSupportedCodecsCommand();
bool BtmIsControllerSupportedEnhancedSetupSynchronousConnection();
bool BtmIsControllerSupportedEnhancedAcceptSynchronousConnection();
bool BtmIsControllerSupportedLeSetPrivacyMode();

bool BtmGetLocalSupportedFeature(HciLmpFeatures *lmpFeature);

int BtmGetWhiteListSize(uint8_t *whiteListSize);
int BtmGetResolvingListSize(uint8_t *resolvingListSize);

#ifdef __cplusplus
}
#endif

#endif