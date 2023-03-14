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

#ifndef SDP_SERVER_H
#define SDP_SERVER_H

#include "sdp.h"

#include <stdint.h>

#include "btstack.h"
#include "packet.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize sdp server
void SdpInitializeServer();
// Finalize sdp server
void SdpFinalizeServer();
void SdpParseClientRequest(uint16_t lcid, const Packet *data);
uint32_t SdpCreateServiceRecord();
int SdpDestroyServiceRecord(uint32_t handle);
int SdpRegisterServiceRecord(uint32_t handle);
int SdpDeregisterServiceRecord(uint32_t handle);
int SdpAddServiceClassIdList(uint32_t handle, const BtUuid *classid, uint16_t classidNumber);
int SdpAddServiceRecordState(uint32_t handle, uint32_t state);
int SdpAddServiceId(uint32_t handle, const BtUuid *serviceid);
int SdpAddProtocolDescriptorList(uint32_t handle, const SdpProtocolDescriptor *descriptor, uint16_t descriptorNumber);
int SdpAddAdditionalProtocolDescriptorList(
    uint32_t handle, const SdpAdditionalProtocolDescriptor *descriptorList, uint16_t descriptorListNumber);
int SdpAddBrowseGroupList(uint32_t handle, const BtUuid *browseUuid, uint16_t browseUuidNumber);
int SdpAddLanguageBaseAttributeIdList(
    uint32_t handle, const SdpLanguageBaseAttributeId *baseAttributeId, uint16_t baseAttributeIdNum);
int SdpAddServiceInfoTimeToLive(uint32_t handle, uint32_t value);
int SdpAddServiceAvailability(uint32_t handle, uint8_t value);
int SdpAddBluetoothProfileDescriptorList(
    uint32_t handle, const SdpProfileDescriptor *profileDescriptor, uint16_t profileDescriptorNum);
int SdpAddDocumentationUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen);
int SdpAddClientExecutableUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen);
int SdpAddIconUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen);
int SdpAddServiceName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen);
int SdpAddServiceDescription(
    uint32_t handle, uint16_t baseAttributeId, const char *description, uint16_t descriptionLen);
int SdpAddProviderName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen);
int SdpAddAttribute(
    uint32_t handle, uint16_t attributeId, SdpDataType type, void *attributeValue, uint16_t attributeValueLength);
int SdpAddSequenceAttribute(
    uint32_t handle, uint16_t attributeId, const uint8_t *attributeValue, uint16_t attributeValueLength);

#ifdef __cplusplus
}
#endif

#endif  // SDP_SERVER_H