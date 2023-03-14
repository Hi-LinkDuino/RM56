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
#ifndef AVCTP_BR_ACT_H
#define AVCTP_BR_ACT_H
#include "avctp_int.h"
#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t AvctCbBrChConn(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChConnFail(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChBind(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChBindFail(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChDisconn(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChUnbind(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChCheckDisconn(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChConnInd(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChCloseInd(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChCloseCfm(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrChBusyInd(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrDiscardMsg(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrSendMsg(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrRevMsg(AvctCbDev *cbDev, const AvctEvtData *data);
extern uint16_t AvctCbBrDiscardRevMsg(AvctCbDev *cbDev, const AvctEvtData *data);

#ifdef __cplusplus
}
#endif

#endif /* AVCTP_BR_L2CAP_H */