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

#ifndef HCI_EVT_H
#define HCI_EVT_H

#include "packet.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HCI_FOREACH_EVT_CALLBACKS_START(x)                         \
    MutexLock(HciGetEventCallbackListLock());                      \
    ListNode *node_ = ListGetFirstNode(HciGetEventCallbackList()); \
    while (node_ != NULL) {                                        \
        x = ListGetNodeData(node_);                                \
        if ((x) != NULL) {

#define HCI_FOREACH_EVT_CALLBACKS_END \
    }                                 \
    node_ = ListGetNextNode(node_);   \
    }                                 \
    MutexUnlock(HciGetEventCallbackListLock());

void HciInitEvent();

void HciOnEvent(Packet *packet);

void HciCloseEvent();

List *HciGetEventCallbackList();
Mutex *HciGetEventCallbackListLock();

#ifdef __cplusplus
}
#endif

#endif  // HCI_EVT_H
