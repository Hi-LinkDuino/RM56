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

#ifndef TRANSPORT_DEF_H
#define TRANSPORT_DEF_H

#include "dispatcher.h"
#include "transport.h"

namespace OHOS {
namespace bluetooth {
// Transport event declarations
// The event is triggered when connection failed.
static const int CONNECT_FAIL = 0x0001;
// The event is triggered when the disconnection process is successful.
static const int DISCONNECT_SUCCESS = 0x0002;
// The event is triggered when the disconnection process fails.
static const int DISCONNECT_FAIL = 0x0003;
// The event is triggered when peer or RFCOMM/L2CAP is available to receive data.
static const int EV_FC_ON = 0x0004;
// The event is triggered when peer or RFCOMM/L2CAP is not available to receive data.
static const int EV_FC_OFF = 0x0005;
// The event is triggered when L2CAP channel is on a collision.
static const int CONNECT_COLLISION = 0x0006;

static const int L2CAP_SEND_CONFIG_REQ = 0x01;
static const int L2CAP_RECV_CONFIG_RSP = 0x02;
static const int L2CAP_RECV_CONFIG_REQ = 0x04;
static const int L2CAP_CONFIG_COMPLETE = (L2CAP_SEND_CONFIG_REQ | L2CAP_RECV_CONFIG_RSP | L2CAP_RECV_CONFIG_REQ);

static const int L2CAP_CONFIG_FLUSHTTIMEOUT = 0xFFFF;
static const int L2CAP_CONFIG_FCS = 0x01;
static const int L2CAP_CONFIG_RFC_MODE = 0x03;

/**
 * @brief L2cap transport information.
 */
typedef struct {
    const RawAddress *addr;
    uint16_t lpsm;
    uint16_t rpsm;
    uint16_t mtu;
    DataTransportObserver &observer;
    utility::Dispatcher &dispatcher;
} L2capTransportInfo;
}  // namespace bluetooth
}  // namespace OHOS
#endif  // TRANSPORT_DEF_H