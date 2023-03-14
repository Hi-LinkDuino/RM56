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
#ifndef AVCTP_CTRL_H
#define AVCTP_CTRL_H
#include "avctp_int.h"
#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t AvctCbCtrlEvt(AvctCbDev *cbDev, uint8_t event, const AvctEvtData *data);
extern AvctCbDev *AvctGetCbDevByAddress(const BtAddr *peerAddr);
extern void AvctMakeSignleMsgHead(const Packet *pkt, uint8_t label, uint8_t cr, uint16_t pid);
extern void AvctMakeRejMsgHead(const Packet *pkt, uint8_t label, uint16_t pid);

#ifdef __cplusplus
}
#endif
#endif /* AVCTP_CTRL_H */