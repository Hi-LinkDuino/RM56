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
#ifndef AVCTP_CONN_H
#define AVCTP_CONN_H
#include "avctp_int.h"
#ifdef __cplusplus
extern "C" {
#endif

extern AvctCbConn *AvctCbConnAlloc(const AvctConnectParam *connParm);
extern void AvctCbConnDealloc(AvctCbConn *cbConn);
extern AvctCbConn *AvctGetCbConnByPid(const AvctCbDev *cbDev, uint16_t pid);
extern AvctCbConn *AvctGetCbConnByConnId(uint8_t connId);
extern void AvctCbConnEvtCallback(const AvctCbConn *cbConn, uint8_t event, uint16_t result, const BtAddr *peerAddr);

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_CONN_H */