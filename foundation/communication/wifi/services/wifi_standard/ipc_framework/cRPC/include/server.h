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
#ifndef CRPC_SERVER_H
#define CRPC_SERVER_H

#include "common.h"
#include "context.h"
#include "evloop.h"
#include "hash_table.h"
#include "net.h"
#include "serial.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Node {
    Context *context;
    struct Node *next;
};

/* save register event and client contexts */
struct EventNode {
    int event;
    struct Node *head;
};

/* rpc server */
typedef struct RpcServer {
    int listenFd;
    EventLoop *loop;
    HashTable *clients;
    pthread_mutex_t mutex;
    int events[100];
    int nEvents;
    struct EventNode eventNode[100];
} RpcServer;

/**
 * @Description Create a Rpc Server object
 *
 * @param path - Unix domain communication file
 * @return RpcServer* - pointer to the Rpc Server or NULL if failed
 */
RpcServer *CreateRpcServer(const char *path);

/**
 * @Description Cyclic processing events
 *
 * @param server - RpcServer object's pointer
 * @return int - 0 Success ,other is failed
 */
int RunRpcLoop(RpcServer *server);

/**
 * @Description Release RpcServer
 *
 * @param server - RpcServer object's pointer
 */
void ReleaseRpcServer(RpcServer *server);

/**
 * @Description Register Callback function
 *
 * @param server - RpcServer object's pointer
 * @param event - event type
 * @param context - Communication Context
 * @return int - 0 Success
 */
int RegisterCallback(RpcServer *server, int event, Context *context);

/**
 * @Description UnRegister Callback function
 *
 * @param server - RpcServer object's pointer
 * @param event - event type
 * @param context - Communication Context
 * @return int - 0 Success
 */
int UnRegisterCallback(RpcServer *server, int event, const Context *context);

/**
 * @Description Adding Events to the Event Table
 *
 * @param server - RpcServer object's pointer
 * @param event - Event type
 * @return int - 0 Success; -1 Failed
 */
int EmitEvent(RpcServer *server, int event);

/**
 * @Description Differentiated Event Processing. This function must be implemented by users
 *              when the RPC server is implemented
 *
 * @param server - RpcServer object's pointer
 * @param context - Message context
 * @return int - 0 Success; -1 Failed
 */
int OnTransact(RpcServer *server, Context *context);

/**
 * @Description Processing callback events, This function must be implemented by users
 *              when the RPC server is implemented
 *
 * @param server - RpcServer object's pointer
 * @param event - Event type
 * @param context - Message context
 * @return int - 0 success; -1 failed
 */
int OnCallbackTransact(const RpcServer *server, int event, Context *context);

/**
 * @Description End processing callback events, This function must be implemented by users
 *              when the RPC server is implemented
 *
 * @param server - RpcServer object's pointer
 * @param event - Event type
 * @return int - 0 Success; -1 Failed
 */
int EndCallbackTransact(const RpcServer *server, int event);

#ifdef __cplusplus
}
#endif
#endif