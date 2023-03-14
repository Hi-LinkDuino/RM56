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
#ifndef WIFI_MOCK_WPA_CTRL_H
#define WIFI_MOCK_WPA_CTRL_H
#include "common/wpa_ctrl.h"
#ifdef  __cplusplus
extern "C" {
#endif
#include "utils/common.h"
#ifdef  __cplusplus
}
#endif
#include <string>
#include <vector>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

struct wpa_ctrl {
    int s;
};

void MockSetWpaExpectCmdResponse(const std::string &cmd, const std::string &response);
void MockEraseSupportedCmd(const std::string &cmd);
void MockInitGlobalCmd(void);
void MockInitStaSupportedCmd(void);
void MockInitApSupportedCmd(void);
void MockInitP2pSupportedCmd(void);
void MockWpaCallback(struct wpa_ctrl *send, const char *message);
#ifdef __cplusplus
}
#endif
#endif