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

#include "rfcomm_defs.h"

static List *g_serverList;

/**
 * @brief Create server list when RFCOMM initialize.
 *
 */
void RfcommCreateServerList()
{
    LOG_INFO("%{public}s", __func__);

    g_serverList = ListCreate(NULL);
}

/**
 * @brief Free server list when RFCOMM finalize.
 *
 */
void RfcommDestroyServerList()
{
    LOG_INFO("%{public}s", __func__);

    ListNode *node = NULL;
    RfcommServerInfo *server = NULL;

    if (g_serverList == NULL) {
        LOG_DEBUG("%{public}s Server list is NULL.", __func__);
        return;
    }

    // Release server information.
    node = ListGetFirstNode(g_serverList);
    while (node != NULL) {
        server = ListGetNodeData(node);
        free(server);
        node = ListGetNextNode(node);
    }

    // Free server list.
    ListDelete(g_serverList);
    g_serverList = NULL;
}

/**
 * @brief The function is used to register the server.
 *
 * @param scn       The server's number.
 * @param mtu       The maximum size of data received at a time.
 * @param eventMask The collection of events followed by upper layers.
 * @param callback  The callback function used by rfcomm to notify uppers of data or events.
 * @param context   The content passed in from the upper layer.
 *                  It will be brought back to the upper layer when callback is called.
 * @return The pointer of the new server in the server list.
 */
RfcommServerInfo *RfcommCreateServer(
    uint8_t scn, uint16_t mtu, uint32_t eventMask, RFCOMM_EventCallback callback, void *context)
{
    LOG_INFO("%{public}s scn:%hhu, mtu:%hu", __func__, scn, mtu);

    RfcommServerInfo *server = malloc(sizeof(RfcommServerInfo));
    if (server == NULL) {
        return NULL;
    }

    server->eventMask = eventMask;
    server->scn = scn;
    server->maxRevSize = mtu;
    server->context = context;
    server->callBack = callback;

    // Add the new server into server list.
    ListAddLast(g_serverList, server);

    return server;
}

/**
 * @brief Remove server node from server list and free the server resources.
 *
 * @param server The pointer of the server in the server list.
 */
void RfcommRemoveServer(RfcommServerInfo *server)
{
    LOG_INFO("%{public}s", __func__);

    ListRemoveNode(g_serverList, server);

    free(server);
}

/**
 * @brief This function is used to find the specified server using server number.
 *
 * @param scn The server number.
 * @return The pointer of the server in the server list.
 */
RfcommServerInfo *RfcommGetServerByScn(uint8_t scn)
{
    LOG_INFO("%{public}s scn:%hhu", __func__, scn);

    RfcommServerInfo *server = NULL;
    ListNode *node = NULL;

    if (g_serverList == NULL) {
        LOG_DEBUG("%{public}s Server list is NULL.", __func__);
        return NULL;
    }

    node = ListGetFirstNode(g_serverList);
    while (node != NULL) {
        server = ListGetNodeData(node);
        if (server->scn == scn) {
            return server;
        }
        node = ListGetNextNode(node);
    }

    return NULL;
}