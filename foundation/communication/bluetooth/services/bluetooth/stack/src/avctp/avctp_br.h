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
#ifndef AVCTP_BR_H
#define AVCTP_BR_H
#include "avctp_int.h"
#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t AvctCbBrEvt(AvctCbDev *cbDev, uint8_t event, const AvctEvtData *data);
extern void AvctCbChDealloc(AvctCbCh **cbBr);
extern uint16_t AvctBrConnectInitiate(AvctCbConn *cbConn);

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_BR_H */