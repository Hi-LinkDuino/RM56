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

#include "hci_evt_le.h"

#include <securec.h>

#include "btstack.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "hci/acl/hci_acl.h"
#include "hci/hci.h"
#include "hci/hci_def.h"
#include "hci/hci_error.h"

#include "hci_evt.h"

typedef void (*HciLeEventFunc)(const uint8_t *param, size_t length);

static void HciEventOnLeConnectionCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeConnectionCompleteEventParam))) {
        return;
    }

    HciLeConnectionCompleteEventParam *eventParam = (HciLeConnectionCompleteEventParam *)param;

    if (eventParam->status == HCI_SUCCESS) {
        HciAclOnConnectionComplete(eventParam->connectionHandle, TRANSPORT_LE);
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leConnectionComplete != NULL) {
        callbacks->leConnectionComplete(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeAdvertisingReportEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || length <= 1) {
        return;
    }

    int offset = 0;
    HciLeAdvertisingReportEventParam eventParam = {
        .numReports = param[offset],
        .reports = NULL,
    };
    offset += sizeof(uint8_t);

    if (eventParam.numReports == 0) {
        return;
    }

    HciLeAdvertisingReport *reports = MEM_MALLOC.alloc(sizeof(HciLeAdvertisingReport) * eventParam.numReports);
    if (reports != NULL) {
        for (uint8_t i = 0; i < eventParam.numReports; i++) {
            reports[i].eventType = param[offset];
            offset += sizeof(uint8_t);

            reports[i].addressType = param[offset];
            offset += sizeof(uint8_t);

            (void)memcpy_s(reports[i].address.raw, BT_ADDRESS_SIZE, param + offset, BT_ADDRESS_SIZE);
            offset += BT_ADDRESS_SIZE;

            reports[i].lengthData = param[offset];
            offset += sizeof(uint8_t);

            reports[i].data = (uint8_t *)(param + offset);
            offset += reports[i].lengthData;

            reports[i].rssi = param[offset];
            offset += sizeof(uint8_t);
        }

        eventParam.reports = reports;
    } else {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAdvertisingReport != NULL) {
        callbacks->leAdvertisingReport(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;

    if (reports != NULL) {
        MEM_MALLOC.free(reports);
    }
}

static void HciEventOnLeConnectionUpdateCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeConnectionUpdateCompleteEventParam))) {
        return;
    }

    HciLeConnectionUpdateCompleteEventParam *eventParam = (HciLeConnectionUpdateCompleteEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leConnectionUpdateComplete != NULL) {
        callbacks->leConnectionUpdateComplete(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadRemoteFeaturesCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeReadRemoteFeaturesCompleteEventParam))) {
        return;
    }

    HciLeReadRemoteFeaturesCompleteEventParam *eventParam = (HciLeReadRemoteFeaturesCompleteEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadRemoteFeaturesComplete != NULL) {
        callbacks->leReadRemoteFeaturesComplete(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeLongTermKeyRequestEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeLongTermKeyRequestEventParam))) {
        return;
    }

    HciLeLongTermKeyRequestEventParam *eventParam = (HciLeLongTermKeyRequestEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leLongTermKeyRequest != NULL) {
        callbacks->leLongTermKeyRequest(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeRemoteConnectionParameterRequestEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeRemoteConnectionParameterRequestEventParam))) {
        return;
    }

    HciLeRemoteConnectionParameterRequestEventParam *eventParam =
        (HciLeRemoteConnectionParameterRequestEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leRemoteConnectionParameterRequest != NULL) {
        callbacks->leRemoteConnectionParameterRequest(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeReadLocalP256PublicKeyCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeReadLocalP256PublicKeyCompleteEventParam))) {
        return;
    }

    HciLeReadLocalP256PublicKeyCompleteEventParam *eventParam = (HciLeReadLocalP256PublicKeyCompleteEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leReadLocalP256PublicKeyComplete != NULL) {
        callbacks->leReadLocalP256PublicKeyComplete(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeGenerateDHKeyCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeGenerateDHKeyCompleteEventParam))) {
        return;
    }

    HciLeGenerateDHKeyCompleteEventParam *eventParam = (HciLeGenerateDHKeyCompleteEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leGenerateDHKeyComplete != NULL) {
        callbacks->leGenerateDHKeyComplete(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeEnhancedConnectionCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeEnhancedConnectionCompleteEventParam))) {
        return;
    }

    HciLeEnhancedConnectionCompleteEventParam *eventParam = (HciLeEnhancedConnectionCompleteEventParam *)param;

    if (eventParam->status == HCI_SUCCESS) {
        HciAclOnConnectionComplete(eventParam->connectionHandle, TRANSPORT_LE);
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leEnhancedConnectionComplete != NULL) {
        callbacks->leEnhancedConnectionComplete(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciParseLeDirectedAdvertisingReport(
    HciLeDirectedAdvertisingReport *report, const uint8_t *param, int *offset)
{
    report->eventType = param[*offset];
    *offset += sizeof(uint8_t);

    report->addressType = param[*offset];
    *offset += sizeof(uint8_t);

    if (memcpy_s(report->address.raw, BT_ADDRESS_SIZE, param + *offset, BT_ADDRESS_SIZE) != EOK) {
        return;
    }
    *offset += BT_ADDRESS_SIZE;

    report->directAddressType = param[*offset];
    *offset += sizeof(uint8_t);

    (void)memcpy_s(report->directAddress.raw, BT_ADDRESS_SIZE, param + *offset, BT_ADDRESS_SIZE);
    *offset += BT_ADDRESS_SIZE;

    report->rssi = param[*offset];
    *offset += sizeof(uint8_t);
}

static void HciEventOnLeDirectedAdvertisingReportCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || length <= 1) {
        return;
    }

    int offset = 0;
    HciLeDirectedAdvertisingReportEventParam eventParam = {
        .numReports = param[offset],
        .reports = NULL,
    };

    if (eventParam.numReports == 0) {
        return;
    }

    HciLeDirectedAdvertisingReport *reports =
        MEM_MALLOC.alloc(sizeof(HciLeDirectedAdvertisingReport) * eventParam.numReports);
    if (reports != NULL) {
        for (uint8_t i = 0; i < eventParam.numReports; i++) {
            HciParseLeDirectedAdvertisingReport(reports + i, param, &offset);
        }

        eventParam.reports = reports;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leDirectedAdvertisingReport != NULL) {
        callbacks->leDirectedAdvertisingReport(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;

    if (reports != NULL) {
        MEM_MALLOC.free(reports);
    }
}

static void HciEventOnLePHYUpdateCompleteEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLePhyUpdateCompleteEventParam))) {
        return;
    }

    HciLePhyUpdateCompleteEventParam *eventParam = (HciLePhyUpdateCompleteEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePhyUpdateComplete != NULL) {
        callbacks->lePhyUpdateComplete(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciParseLeExtendedAdvertisingReport(
    HciLeExtendedAdvertisingReport *report, const uint8_t *param, int *offset)
{
    report->eventType = param[*offset];
    *offset += sizeof(uint16_t);

    report->addressType = param[*offset];
    *offset += sizeof(uint8_t);

    if (memcpy_s(report->address.raw, BT_ADDRESS_SIZE, param + *offset, BT_ADDRESS_SIZE) != EOK) {
        return;
    }
    *offset += BT_ADDRESS_SIZE;

    report->primaryPHY = param[*offset];
    *offset += sizeof(uint8_t);

    report->secondaryPHY = param[*offset];
    *offset += sizeof(uint8_t);

    report->advertisingSID = param[*offset];
    *offset += sizeof(uint8_t);

    report->txPower = param[*offset];
    *offset += sizeof(uint8_t);

    report->rssi = param[*offset];
    *offset += sizeof(uint8_t);

    report->periodicAdvertisingInterval = param[*offset];
    *offset += sizeof(uint16_t);

    report->directAddressType = param[*offset];
    *offset += sizeof(uint8_t);

    if (memcpy_s(report->directAddress.raw, BT_ADDRESS_SIZE, param + *offset, BT_ADDRESS_SIZE) != EOK) {
        return;
    }
    *offset += BT_ADDRESS_SIZE;

    report->dataLength = param[*offset];
    *offset += sizeof(uint8_t);

    report->data = (uint8_t *)(param + *offset);
    *offset += report->dataLength;
}

static void HciEventOnLeExtendedAdvertisingReportEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || length <= 1) {
        return;
    }

    int offset = 0;
    HciLeExtendedAdvertisingReportEventParam eventParam = {
        .numReports = param[offset],
        .reports = NULL,
    };
    offset += sizeof(uint8_t);

    if (eventParam.numReports == 0) {
        return;
    }

    HciLeExtendedAdvertisingReport *reports =
        MEM_MALLOC.alloc(sizeof(HciLeExtendedAdvertisingReport) * eventParam.numReports);
    if (reports != NULL) {
        for (uint8_t i = 0; i < eventParam.numReports; i++) {
            HciParseLeExtendedAdvertisingReport(reports + i, param, &offset);
        }

        eventParam.reports = reports;
    } else {
        return;
    }

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leExtendedAdvertisingReport != NULL) {
        callbacks->leExtendedAdvertisingReport(&eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;

    MEM_MALLOC.free(reports);
}

static void HciEventOnLeChannelSelectionAlgorithmEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeChannelSelectionAlgorithmEventParam))) {
        return;
    }

    HciLeChannelSelectionAlgorithmEventParam *eventParam = (HciLeChannelSelectionAlgorithmEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leChannelSelectionAlgorithm != NULL) {
        callbacks->leChannelSelectionAlgorithm(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeScanTimeoutEvent(const uint8_t *param, size_t length)
{
    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leScanTimeoutComplete != NULL) {
        callbacks->leScanTimeoutComplete();
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeAdvertisingSetTerminatedEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeAdvertisingSetTerminatedEventParam))) {
        return;
    }

    HciLeAdvertisingSetTerminatedEventParam *eventParam = (HciLeAdvertisingSetTerminatedEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leAdvertisingSetTerminated != NULL) {
        callbacks->leAdvertisingSetTerminated(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLeScanRequestReceivedEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeScanRequestReceivedEventParam))) {
        return;
    }

    HciLeScanRequestReceivedEventParam *eventParam = (HciLeScanRequestReceivedEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leScanRequestReceived != NULL) {
        callbacks->leScanRequestReceived(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLEDataLengthChangeEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLeDataLengthChangeEventParam))) {
        return;
    }

    HciLeDataLengthChangeEventParam *eventParam = (HciLeDataLengthChangeEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->leDataLengthChange != NULL) {
        callbacks->leDataLengthChange(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLEPeriodicAdvertisingSyncEstablishedEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLePeriodicAdvertisingSyncEstablishedEventParam))) {
        return;
    }

    HciLePeriodicAdvertisingSyncEstablishedEventParam *eventParam =
        (HciLePeriodicAdvertisingSyncEstablishedEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePeriodicAdvertisingSyncEstablished != NULL) {
        callbacks->lePeriodicAdvertisingSyncEstablished(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLEPeriodicAdvertisingReportEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLePeriodicAdvertisingReportEventParam))) {
        return;
    }

    HciLePeriodicAdvertisingReportEventParam *eventParam = (HciLePeriodicAdvertisingReportEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePeriodicAdvertisingReport != NULL) {
        callbacks->lePeriodicAdvertisingReport(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static void HciEventOnLEPeriodicAdvertisingSyncLostEvent(const uint8_t *param, size_t length)
{
    if (param == NULL || (length != sizeof(HciLePeriodicAdvertisingSyncLostEventParam))) {
        return;
    }

    HciLePeriodicAdvertisingSyncLostEventParam *eventParam = (HciLePeriodicAdvertisingSyncLostEventParam *)param;

    HciEventCallbacks *callbacks = NULL;
    HCI_FOREACH_EVT_CALLBACKS_START(callbacks);
    if (callbacks->lePeriodicAdvertisingSyncLost != NULL) {
        callbacks->lePeriodicAdvertisingSyncLost(eventParam);
    }
    HCI_FOREACH_EVT_CALLBACKS_END;
}

static HciLeEventFunc g_leEventMap[] = {
    NULL,                                                 // 0x00
    HciEventOnLeConnectionCompleteEvent,                  // 0x01
    HciEventOnLeAdvertisingReportEvent,                   // 0x02
    HciEventOnLeConnectionUpdateCompleteEvent,            // 0x03
    HciEventOnLeReadRemoteFeaturesCompleteEvent,          // 0x04
    HciEventOnLeLongTermKeyRequestEvent,                  // 0x05
    HciEventOnLeRemoteConnectionParameterRequestEvent,    // 0x06
    HciEventOnLEDataLengthChangeEvent,                    // 0x07
    HciEventOnLeReadLocalP256PublicKeyCompleteEvent,      // 0x08
    HciEventOnLeGenerateDHKeyCompleteEvent,               // 0x09
    HciEventOnLeEnhancedConnectionCompleteEvent,          // 0x0A
    HciEventOnLeDirectedAdvertisingReportCompleteEvent,   // 0x0B
    HciEventOnLePHYUpdateCompleteEvent,                   // 0x0C
    HciEventOnLeExtendedAdvertisingReportEvent,           // 0x0D
    HciEventOnLEPeriodicAdvertisingSyncEstablishedEvent,  // 0x0E
    HciEventOnLEPeriodicAdvertisingReportEvent,           // 0x0F
    HciEventOnLEPeriodicAdvertisingSyncLostEvent,         // 0x10
    HciEventOnLeScanTimeoutEvent,                         // 0x11
    HciEventOnLeAdvertisingSetTerminatedEvent,            // 0x12
    HciEventOnLeScanRequestReceivedEvent,                 // 0x13
    HciEventOnLeChannelSelectionAlgorithmEvent,           // 0x14
};

#define LESUBEVENTCODE_MAX 0x14

void HciEventOnLeMetaEvent(Packet *packet)
{
    Buffer *payloadBuffer = PacketContinuousPayload(packet);
    if (payloadBuffer == NULL) {
        return;
    }
    uint8_t *buf = (uint8_t *)BufferPtr(payloadBuffer);
    if (buf == NULL) {
        return;
    }
    size_t length = BufferGetSize(payloadBuffer);
    if (length < 1) {
        return;
    }

    if (buf[0] > LESUBEVENTCODE_MAX) {
        return;
    }

    HciLeEventFunc func = g_leEventMap[buf[0]];
    if (func != NULL) {
        if (length > 1) {
            func(buf + 1, length - 1);
        } else {
            func(NULL, 0);
        }
    }
}
