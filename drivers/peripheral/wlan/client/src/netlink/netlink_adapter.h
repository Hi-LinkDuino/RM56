/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NETLINK_ADAPTER_H
#define NETLINK_ADAPTER_H

#include <netlink/socket.h>
#include <netlink/msg.h>
#include <pthread.h>

enum ThreadStatus {
    THREAD_INIT,
    THREAD_STARTING,
    THREAD_RUN,
    THREAD_STOPPING,
    THREAD_STOP,
};

typedef int32_t (*RespHandler)(struct nl_msg *msg, void *data);

struct WifiThreadParam {
    struct nl_sock *eventSock;
    int familyId;
    enum ThreadStatus *status;
};
int32_t SendCmdSync(struct nl_msg *msg, const RespHandler handler, void *data);
void *EventThread(void *para);


#endif /* end of netlink_adapter.h */
