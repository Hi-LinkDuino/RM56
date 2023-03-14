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

#include "hci_cmd.h"

#include <securec.h>

#include "btm/btm_thread.h"
#include "btstack.h"
#include "platform/include/alarm.h"
#include "platform/include/allocator.h"
#include "platform/include/bt_endian.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "platform/include/queue.h"

#include "hci/acl/hci_acl.h"
#include "hci/hci.h"
#include "hci/hci_def.h"
#include "hci/hci_error.h"
#include "hci/hci_failure.h"
#include "hci/hci_internal.h"

#include "hci_cmd_failure.h"

#define MAX_QUEUE_SIZE 32

#define CMD_TIMEOUT (10 * 1000)

#pragma pack(1)
typedef struct {
    uint16_t opCode;
    uint8_t parameterTotalLength;
} HciCmdHeader;
#pragma pack()

static uint8_t g_numberOfHciCmd = 1;
static Mutex *g_lockNumberOfHciCmd = NULL;

static Queue *g_cmdCache = NULL;

static List *g_processingCmds = NULL;
static Mutex *g_lockProcessingCmds = NULL;

// Function declare
static void HciFreeCmd(void *cmd);

void HciInitCmd()
{
    g_cmdCache = QueueCreate(MAX_QUEUE_SIZE);
    g_processingCmds = ListCreate(HciFreeCmd);

    g_numberOfHciCmd = 1;
    g_lockNumberOfHciCmd = MutexCreate();
    g_lockProcessingCmds = MutexCreate();
}

void HciCloseCmd()
{
    if (g_lockProcessingCmds != NULL) {
        MutexDelete(g_lockProcessingCmds);
        g_lockProcessingCmds = NULL;
    }

    if (g_lockNumberOfHciCmd != NULL) {
        MutexDelete(g_lockNumberOfHciCmd);
        g_lockNumberOfHciCmd = NULL;
    }

    if (g_cmdCache != NULL) {
        QueueDelete(g_cmdCache, HciFreeCmd);
        g_cmdCache = NULL;
    }

    if (g_processingCmds != NULL) {
        ListDelete(g_processingCmds);
        g_processingCmds = NULL;
    }
}

static int HciCmdPushToTxQueue(HciCmd *cmd)
{
    int result = BT_NO_ERROR;
    HciPacket *hciPacket = MEM_MALLOC.alloc(sizeof(HciPacket));
    if (hciPacket != NULL) {
        hciPacket->type = H2C_CMD;
        hciPacket->packet = cmd->packet;
        cmd->packet = NULL;
        HciPushToTxQueue(hciPacket);
    } else {
        result = BT_NO_MEMORY;
    }
    return result;
}

static void HciCmdTimeoutTask(void *context)
{
    HciCmd *pCmd = NULL;
    uint16_t opCode = 0;

    MutexLock(g_lockProcessingCmds);
    ListNode *node = ListGetFirstNode(g_processingCmds);
    while (node != NULL) {
        pCmd = ListGetNodeData(node);
        if (pCmd == context) {
            opCode = pCmd->opCode;
            break;
        } else {
            pCmd = NULL;
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_lockProcessingCmds);

    if (opCode == 0) {
        return;
    }

    HciCmdOnCommandStatus(opCode, HCI_TIMEOUT);

    HciOnCmdTimeout();
}

static void HciCmdOnCmdTimeout(void *parameter)
{
    Thread *thread = BTM_GetProcessingThread();
    if (thread != NULL) {
        ThreadPostTask(thread, HciCmdTimeoutTask, parameter);
    }
}

void HciSetNumberOfHciCmd(uint8_t numberOfHciCmd)
{
    MutexLock(g_lockNumberOfHciCmd);

    g_numberOfHciCmd = numberOfHciCmd;

    HciCmd *cmd = NULL;

    while (g_numberOfHciCmd > 0) {
        cmd = QueueTryDequeue(g_cmdCache);
        if (cmd != NULL) {
            int result = HciCmdPushToTxQueue(cmd);
            if (result == BT_NO_ERROR) {
                g_numberOfHciCmd--;

                MutexLock(g_lockProcessingCmds);
                AlarmSet(cmd->alarm, CMD_TIMEOUT, HciCmdOnCmdTimeout, cmd);
                ListAddLast(g_processingCmds, cmd);
                MutexUnlock(g_lockProcessingCmds);
            } else {
                HciFreeCmd(cmd);
            }
        } else {
            // No more cmd
            break;
        }
    }

    MutexUnlock(g_lockNumberOfHciCmd);
}

static Packet *HciCreateCmdPacket(uint16_t opCode)
{
    Packet *packet = PacketMalloc(sizeof(HciCmdHeader), 0, 0);
    Buffer *headerBuffer = PacketHead(packet);
    HciCmdHeader *header = (HciCmdHeader *)BufferPtr(headerBuffer);
    if (header != NULL) {
        header->opCode = opCode;
        header->parameterTotalLength = 0;
    }

    return packet;
}

static Packet *HciCreateCmdPacketWithParam(uint16_t opCode, const void *param, uint8_t paramLength)
{
    Packet *packet = PacketMalloc(sizeof(HciCmdHeader), 0, paramLength);
    Buffer *headerBuffer = PacketHead(packet);
    HciCmdHeader *header = (HciCmdHeader *)BufferPtr(headerBuffer);
    if (header != NULL) {
        header->opCode = opCode;
        header->parameterTotalLength = paramLength;
    }
    PacketPayloadWrite(packet, param, 0, paramLength);
    return packet;
}

HciCmd *HciAllocCmd(uint16_t opCode, const void *param, size_t paramLength)
{
    HciCmd *cmd = MEM_MALLOC.alloc(sizeof(HciCmd));
    if (cmd != NULL) {
        cmd->opCode = opCode;
        if (param != NULL && paramLength > 0) {
            cmd->param = MEM_MALLOC.alloc(paramLength);
            if (cmd->param != NULL) {
                (void)memcpy_s(cmd->param, paramLength, param, paramLength);
            }
            cmd->packet = HciCreateCmdPacketWithParam(opCode, param, paramLength);
        } else {
            cmd->packet = HciCreateCmdPacket(opCode);
            cmd->param = NULL;
        }
        cmd->alarm = AlarmCreate(NULL, false);
    }
    return cmd;
}

static void HciFreeCmd(void *cmd)
{
    HciCmd *hciCmd = (HciCmd *)cmd;
    if (hciCmd != NULL) {
        if (hciCmd->alarm != NULL) {
            AlarmCancel(hciCmd->alarm);
            AlarmDelete(hciCmd->alarm);
        }
        if (hciCmd->param != NULL) {
            MEM_MALLOC.free(hciCmd->param);
            hciCmd->param = NULL;
        }
        if (hciCmd->packet != NULL) {
            PacketFree(hciCmd->packet);
        }
    }
    MEM_MALLOC.free(cmd);
}

int HciSendCmd(HciCmd *cmd)
{
    int result = BT_NO_ERROR;

    MutexLock(g_lockNumberOfHciCmd);

    if (g_numberOfHciCmd > 0) {
        result = HciCmdPushToTxQueue(cmd);
        if (result == BT_NO_ERROR) {
            g_numberOfHciCmd--;

            MutexLock(g_lockProcessingCmds);
            AlarmSet(cmd->alarm, CMD_TIMEOUT, HciCmdOnCmdTimeout, cmd);
            ListAddLast(g_processingCmds, cmd);
            MutexUnlock(g_lockProcessingCmds);
        }
    } else {
        QueueEnqueue(g_cmdCache, cmd);
    }

    MutexUnlock(g_lockNumberOfHciCmd);

    return result;
}

void HciCmdOnCommandStatus(uint16_t opCode, uint8_t status)
{
    HciCmd *cmd = NULL;

    MutexLock(g_lockProcessingCmds);

    ListNode *node = ListGetFirstNode(g_processingCmds);
    while (node != NULL) {
        cmd = ListGetNodeData(node);
        if (cmd != NULL) {
            if (opCode == cmd->opCode) {
                break;
            }
            cmd = NULL;
        }

        node = ListGetNextNode(node);
    }

    void *param = NULL;

    if (cmd != NULL) {
        AlarmCancel(cmd->alarm);

        param = cmd->param;
        cmd->param = NULL;

        ListRemoveNode(g_processingCmds, cmd);
    }

    MutexUnlock(g_lockProcessingCmds);

    if (cmd != NULL && opCode == HCI_DISCONNECT && status == HCI_SUCCESS) {
        HciDisconnectParam *discParam = (HciDisconnectParam *)param;
        HciAclOnDisconnectStatus(discParam->connectionHandle);
    }

    if (cmd != NULL && status != HCI_SUCCESS) {
        HciOnCmdFailed(opCode, status, param);
    }

    if (param != NULL) {
        MEM_MALLOC.free(param);
        param = NULL;
    }
}

void HciCmdOnCommandComplete(uint16_t opCode)
{
    HciCmd *cmd = NULL;

    MutexLock(g_lockProcessingCmds);

    ListNode *node = ListGetFirstNode(g_processingCmds);
    while (node != NULL) {
        cmd = ListGetNodeData(node);
        if (cmd != NULL) {
            if (opCode == cmd->opCode) {
                break;
            }
            cmd = NULL;
        }

        node = ListGetNextNode(node);
    }

    if (cmd != NULL) {
        AlarmCancel(cmd->alarm);
        ListRemoveNode(g_processingCmds, cmd);
    }

    MutexUnlock(g_lockProcessingCmds);
}
