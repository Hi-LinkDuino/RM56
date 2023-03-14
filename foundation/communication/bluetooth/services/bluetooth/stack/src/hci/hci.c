/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "hci.h"

#include "btm/btm_thread.h"
#include "btstack.h"
#include "log.h"
#include "packet.h"
#include "platform/include/allocator.h"
#include "platform/include/event.h"
#include "platform/include/module.h"
#include "platform/include/queue.h"
#include "platform/include/reactor.h"
#include "platform/include/semaphore.h"
#include "platform/include/thread.h"

#include "acl/hci_acl.h"
#include "cmd/hci_cmd.h"
#include "evt/hci_evt.h"
#include "hdi_wrapper.h"
#include "hci_def.h"
#include "hci_failure.h"
#include "hci_internal.h"
#include "hci_vendor_if.h"

#define HCI_TX_QUEUE_SIZE INT32_MAX
#define HCI_RX_QUEUE_SIZE INT32_MAX

static BtHciCallbacks g_hdiCallacks;

static Queue *g_hciTxQueue = NULL;
static Queue *g_hciRxQueue = NULL;

static ReactorItem *g_hciTxReactorItem = NULL;
static ReactorItem *g_hciRxReactorItem = NULL;

static Semaphore *g_waitHdiInit;
static BtInitStatus g_hdiInitStatus = UNKNOWN;

static HDILib *g_hdiLib = NULL;

static bool g_transmissionCapture = false;
static void (*g_transmissionCallback)(uint8_t type, const uint8_t *data, uint16_t length) = NULL;

static Thread *g_hciTxThread = NULL;

// Function Declare
static void HciSendPacketCallback(void *param);
static void HciRecvPacketCallback(void *param);

static void HciFreePacket(void *packet)
{
    HciPacket *hciPacket = packet;
    if (hciPacket != NULL) {
        PacketFree(hciPacket->packet);
    }
    MEM_MALLOC.free(packet);
}

static int HciInitQueue()
{
    int result = BT_NO_ERROR;

    do {
        g_hciTxQueue = QueueCreate(HCI_TX_QUEUE_SIZE);
        if (g_hciTxQueue != NULL) {
            Reactor *reactor = ThreadGetReactor(g_hciTxThread);
            g_hciTxReactorItem =
                ReactorRegister(reactor, QueueGetDequeueFd(g_hciTxQueue), NULL, HciSendPacketCallback, NULL);
        } else {
            result = BT_OPERATION_FAILED;
            break;
        }
        g_hciRxQueue = QueueCreate(HCI_RX_QUEUE_SIZE);
        if (g_hciRxQueue != NULL) {
            Reactor *reactor = ThreadGetReactor(BTM_GetProcessingThread());
            g_hciRxReactorItem =
                ReactorRegister(reactor, QueueGetDequeueFd(g_hciRxQueue), NULL, HciRecvPacketCallback, NULL);
        } else {
            result = BT_OPERATION_FAILED;
            break;
        }
    } while (0);

    return result;
}

NO_SANITIZE("cfi") static int HciInitHal()
{
    int result = BT_NO_ERROR;

    g_waitHdiInit = SemaphoreCreate(0);
    int ret = g_hdiLib->hdiInit(&g_hdiCallacks);
    if (ret == SUCCESS) {
        SemaphoreWait(g_waitHdiInit);
        if (g_hdiInitStatus != SUCCESS) {
            LOG_ERROR("HdiInited failed: %{public}d", g_hdiInitStatus);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("hdiInit failed: %{public}d", ret);
        result = BT_OPERATION_FAILED;
    }
    SemaphoreDelete(g_waitHdiInit);
    g_waitHdiInit = NULL;

    return result;
}

static void HciCloseQueue()
{
    if (g_hciTxReactorItem != NULL) {
        ReactorUnregister(g_hciTxReactorItem);
        g_hciTxReactorItem = NULL;
    }

    if (g_hciRxReactorItem != NULL) {
        ReactorUnregister(g_hciRxReactorItem);
        g_hciRxReactorItem = NULL;
    }

    if (g_hciTxQueue != NULL) {
        QueueDelete(g_hciTxQueue, HciFreePacket);
        g_hciTxQueue = NULL;
    }

    if (g_hciRxQueue != NULL) {
        QueueDelete(g_hciRxQueue, HciFreePacket);
        g_hciRxQueue = NULL;
    }
}

int HCI_Initialize()
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);
    int result;

    do {
        g_hdiLib = LoadHdiLib();
        if (g_hdiLib == NULL) {
            result = BT_OPERATION_FAILED;
            break;
        }

        HciInitFailure();
        HciInitCmd();
        HciInitEvent();
        HciInitAcl();
        HciVendorInit();

        g_hciTxThread = ThreadCreate("HciTx");
        if (g_hciTxThread == NULL) {
            result = BT_OPERATION_FAILED;
            break;
        }

        result = HciInitQueue();
        if (result != BT_NO_ERROR) {
            break;
        }

        result = HciInitHal();
    } while (0);

    if (result != BT_NO_ERROR) {
        if (g_hdiLib != NULL) {
            UnloadHdiLib(g_hdiLib);
            g_hdiLib = NULL;
        }

        HciCloseQueue();

        if (g_hciTxThread != NULL) {
            ThreadDelete(g_hciTxThread);
            g_hciTxThread = NULL;
        }
    }
    LOG_DEBUG("%{public}s end", __FUNCTION__);
    return result;
}

static void CleanTxPacket()
{
    if (g_hciTxQueue != NULL) {
        HciPacket *packet = QueueTryDequeue(g_hciTxQueue);
        while (packet != NULL) {
            HciFreePacket(packet);
            packet = QueueTryDequeue(g_hciTxQueue);
        }
    }
}

static void CleanRxPacket()
{
    if (g_hciRxQueue != NULL) {
        HciPacket *packet = QueueTryDequeue(g_hciRxQueue);
        while (packet != NULL) {
            HciFreePacket(packet);
            packet = QueueTryDequeue(g_hciTxQueue);
        }
    }
}

static void WaitRxTaskCompleteTask(void *context)
{
    Event *taskCompleteEvent = (Event *)context;
    if (taskCompleteEvent != NULL) {
        EventSet(taskCompleteEvent);
    }
}

static void WaitRxTaskComplete()
{
    const int taskTimeout = 1000;

    Event *taskCompleteEvent = EventCreate(true);
    if (taskCompleteEvent != NULL) {
        ThreadPostTask(BTM_GetProcessingThread(), WaitRxTaskCompleteTask, taskCompleteEvent);
        EventWait(taskCompleteEvent, taskTimeout);
        EventDelete(taskCompleteEvent);
    }
}

NO_SANITIZE("cfi") void HCI_Close()
{
    LOG_DEBUG("%{public}s start", __FUNCTION__);

    CleanTxPacket();

    if (g_hciTxReactorItem != NULL) {
        ReactorUnregister(g_hciTxReactorItem);
        g_hciTxReactorItem = NULL;
    }

    if (g_hciTxThread != NULL) {
        ThreadDelete(g_hciTxThread);
        g_hciTxThread = NULL;
    }

    if (g_hdiLib != NULL && g_hdiLib->hdiClose != NULL) {
        g_hdiLib->hdiClose();
    }

    CleanRxPacket();

    if (g_hciRxReactorItem != NULL) {
        ReactorUnregister(g_hciRxReactorItem);
        g_hciRxReactorItem = NULL;
    }

    WaitRxTaskComplete();

    if (g_hciTxQueue != NULL) {
        QueueDelete(g_hciTxQueue, HciFreePacket);
        g_hciTxQueue = NULL;
    }

    if (g_hciRxQueue != NULL) {
        QueueDelete(g_hciRxQueue, HciFreePacket);
        g_hciRxQueue = NULL;
    }

    HciCloseCmd();
    HciCloseEvent();
    HciCloseAcl();
    HciCloseFailure();
    HciVendorClose();

    if (g_hdiLib != NULL) {
        UnloadHdiLib(g_hdiLib);
        g_hdiLib = NULL;
    }

    LOG_DEBUG("%{public}s end", __FUNCTION__);
}

static void HciOnHDIInited(BtInitStatus status)
{
    g_hdiInitStatus = status;
    SemaphorePost(g_waitHdiInit);
}

static void HciOnReceivedHciPacket(BtPacketType type, const BtPacket *btPacket)
{
    if (g_transmissionCapture && g_transmissionCallback != NULL) {
        uint8_t transType = (type == PACKET_TYPE_EVENT) ? TRANSMISSON_TYPE_C2H_EVENT : TRANSMISSON_TYPE_C2H_DATA;
        g_transmissionCallback(transType, btPacket->data, btPacket->size);
    }

    HciPacket *hciPacket = MEM_MALLOC.alloc(sizeof(HciPacket));
    if (hciPacket != NULL) {
        switch (type) {
            case PACKET_TYPE_ACL:
                hciPacket->type = C2H_ACLDATA;
                break;
            case PACKET_TYPE_EVENT:
                hciPacket->type = C2H_EVENT;
                break;
            default:
                break;
        }

        hciPacket->packet = PacketMalloc(0, 0, btPacket->size);
        PacketPayloadWrite(hciPacket->packet, btPacket->data, 0, btPacket->size);

        QueueEnqueue(g_hciRxQueue, hciPacket);
    }
}

NO_SANITIZE("cfi") static void HciSendPacketCallback(void *param)
{
    HciPacket *packet = QueueTryDequeue(g_hciTxQueue);
    if (packet != NULL) {
        BtPacket btPacket;
        btPacket.size = PacketSize(packet->packet);
        btPacket.data = MEM_MALLOC.alloc(btPacket.size);
        PacketRead(packet->packet, btPacket.data, 0, btPacket.size);

        int result;

        if (g_hdiLib == NULL) {
            LOG_ERROR("HDI is invalid.");
            return;
        }

        switch (packet->type) {
            case H2C_CMD:
                result = g_hdiLib->hdiSendHciPacket(PACKET_TYPE_CMD, &btPacket);
                break;
            case H2C_ACLDATA:
                result = g_hdiLib->hdiSendHciPacket(PACKET_TYPE_ACL, &btPacket);
                break;
            case H2C_SCODATA:
                result = g_hdiLib->hdiSendHciPacket(PACKET_TYPE_SCO, &btPacket);
                break;
            default:
                result = UNKNOWN;
                break;
        }

        if (result != SUCCESS) {
            LOG_ERROR("Send packet to HDI failed: %{public}d", result);
        } else {
            if (g_transmissionCapture && g_transmissionCallback != NULL) {
                uint8_t type = (packet->type == H2C_CMD) ? TRANSMISSON_TYPE_H2C_CMD : TRANSMISSON_TYPE_H2C_DATA;
                g_transmissionCallback(type, btPacket.data, btPacket.size);
            }
        }

        MEM_MALLOC.free(btPacket.data);
        HciFreePacket(packet);
    }
}

static void HciRecvPacketCallback(void *param)
{
    HciPacket *packet = QueueTryDequeue(g_hciRxQueue);
    if (packet != NULL) {
        switch (packet->type) {
            case C2H_ACLDATA:
                HciOnAclData(packet->packet);
                break;
            case C2H_EVENT:
                HciOnEvent(packet->packet);
                break;
            case C2H_SCODATA:
                // NOT IMPLETEMENTED
                break;
            default:
                break;
        }
        HciFreePacket(packet);
    }
}

void HciPushToTxQueue(HciPacket *packet)
{
    QueueEnqueue(g_hciTxQueue, packet);
}

int HCI_SetTransmissionCaptureCallback(void (*onTransmission)(uint8_t type, const uint8_t *data, uint16_t length))
{
    g_transmissionCallback = onTransmission;
    return BT_NO_ERROR;
}

int HCI_EnableTransmissionCapture()
{
    g_transmissionCapture = true;
    return BT_NO_ERROR;
}

int HCI_DisableTransmissionCapture()
{
    g_transmissionCapture = false;
    return BT_NO_ERROR;
}

static BtHciCallbacks g_hdiCallacks = {
    .OnInited = HciOnHDIInited,
    .OnReceivedHciPacket = HciOnReceivedHciPacket,
};
