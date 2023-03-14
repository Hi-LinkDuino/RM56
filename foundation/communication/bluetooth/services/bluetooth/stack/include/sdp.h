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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief This file is a part of BTStack.
 *
 */

/**
 * @file sdp.h
 *
 * @brief Service discovery protocol (SDP) interface.
 *
 */
#ifndef SDP_H
#define SDP_H

#include <stdlib.h>
#include <stdbool.h>

#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define service class UUIDs */
#define SDP_PUBLIC_BROWSE_GROUP_ROOT_UUID 0x1002

/* Define service attribute IDs. */
#define SDP_ATTRIBUTE_SERVICE_RECORD_HANDLE 0x0000
#define SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST 0x0001
#define SDP_ATTRIBUTE_SERVICE_RECORD_STATE 0x0002
#define SDP_ATTRIBUTE_SERVICE_ID 0x0003
#define SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST 0x0004
#define SDP_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESCRIPTOR_LIST 0x000D
#define SDP_ATTRIBUTE_BROWSE_GROUP_LIST 0x0005
#define SDP_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST 0x0006
#define SDP_ATTRIBUTE_SERVICE_INFO_TIME_TO_LIVE 0x0007
#define SDP_ATTRIBUTE_SERVICE_AVAILABILITY 0x0008
#define SDP_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST 0x0009
#define SDP_ATTRIBUTE_DOCUMENTATION_URL 0x000A
#define SDP_ATTRIBUTE_CLIENT_EXECUTABLE_URL 0x000B
#define SDP_ATTRIBUTE_ICON_URL 0x000C
#define SDP_ATTRIBUTE_SERVICE_NAME 0x0000
#define SDP_ATTRIBUTE_DESCRIPTOR 0x0001
#define SDP_ATTRIBUTE_PROVIDER_NAME 0x0002

/* Define base attribute ID value for the primary language. */
#define SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE 0x0100

/* Define protocol UUIDs. */
#define UUID_PROTOCOL_SDP 0x0001
#define UUID_PROTOCOL_RFCOMM 0x0003
#define UUID_PROTOCOL_ATT 0x0007
#define UUID_PROTOCOL_OBEX 0x0008
#define UUID_PROTOCOL_AVCTP 0x0017
#define UUID_PROTOCOL_AVDTP 0x0019
#define UUID_PROTOCOL_AVCTP_BROWSING 0x001B
#define UUID_PROTOCOL_L2CAP 0x0100

/* Define specified buffer size. */
#define SDP_PROTOCOL_PARAMETER_MAX_COUNT 4
#define SDP_PROTOCOL_DESCRIPTOR_MAX_COUNT 4
#define SDP_ATTRIBUTE_ID_LIST_MAX_COUNT 12

/* Data type. */
typedef enum {
    SDP_TYPE_BOOL,
    SDP_TYPE_UINT_8,
    SDP_TYPE_UINT_16,
    SDP_TYPE_UINT_32,
    SDP_TYPE_INT_8,
    SDP_TYPE_INT_16,
    SDP_TYPE_INT_32,
    SDP_TYPE_UUID_16,
    SDP_TYPE_UUID_32,
    SDP_TYPE_UUID_128,
    SDP_TYPE_TEXT,
    SDP_TYPE_URL,
} SdpDataType;

/**
 *  @brief Protocol patameter
 */
typedef struct {
    SdpDataType type;
    uint32_t value;
} SdpProtocolParameter;

/**
 *  @brief Protocol descriptor
 */
typedef struct {
    BtUuid protocolUuid;
    uint16_t parameterNumber;
    SdpProtocolParameter parameter[SDP_PROTOCOL_PARAMETER_MAX_COUNT];
} SdpProtocolDescriptor;

/**
 *  @brief Additional protocol descriptor
 */
typedef struct {
    uint16_t protocolDescriptorNumber;
    SdpProtocolDescriptor parameter[SDP_PROTOCOL_DESCRIPTOR_MAX_COUNT];
} SdpAdditionalProtocolDescriptor;

/**
 *  @brief Language attribute
 */
typedef struct {
    uint16_t languageIdentifier;
    uint16_t characterEncodingIdentifier;
    uint16_t baseAttributeId;
} SdpLanguageBaseAttributeId;

/**
 *  @brief Profile descriptor
 */
typedef struct {
    BtUuid profileUuid;
    uint16_t versionNumber;
} SdpProfileDescriptor;

/**
 * @brief  Create service record.
 *
 * @return Returns service record handle value.
 */
uint32_t BTSTACK_API SDP_CreateServiceRecord();

/**
 * @brief Destroy service record.
 *
 * @param handle Service record handle.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_DestroyServiceRecord(uint32_t handle);

/**
 * @brief Register service record.
 *
 * @param handle Service record handle.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_RegisterServiceRecord(uint32_t handle);

/**
 * @brief Deregister service record.
 *
 * @param handle Service record handle.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_DeregisterServiceRecord(uint32_t handle);

/**
 * @brief Add ServiceClassIDList Attribute to service record. The ServiceClassIDList shall contain
 *        at least one service class UUID. Refer to charter 5.1.2 of Core 5.0.
 *
 * @param handle  Service record handle.
 * @param classid The list of UUIDs representing the service classes.
 * @param classidNumber The number of classid.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddServiceClassIdList(uint32_t handle, const BtUuid *classid, uint16_t classidNumber);

/**
 * @brief Add ServiceRecordState Attribute to service record. Refer to charter 5.1.3 of Core 5.0.
 *
 * @param handle Service record handle.
 * @param state  Service record state.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddServiceRecordState(uint32_t handle, uint32_t state);

/**
 * @brief Add ServiceRecordState Attribute to service record. Refer to charter 5.1.4 of Core 5.0.
 *
 * @param handle    Service record handle.
 * @param serviceid The UUID that uniquely identifies the service.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddServiceId(uint32_t handle, const BtUuid *serviceid);

/**
 * @brief Add ProtocolDescriptorList Attribute to record. The ProtocolDescriptorList attribute
 *        describes one or more protocol stacks that can be used to gain access to the service
 *        described by the service record.Refer to charter 5.1.5 of Core 5.0.
 *
 * @param handle     Service record handle.
 * @param descriptor One or more protocol descriptor.
 * @param descriptorNumber The number of protocol.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddProtocolDescriptorList(
    uint32_t handle, const SdpProtocolDescriptor *descriptor, uint16_t descriptorNumber);

/**
 * @brief Add AdditionalProtocolDescriptorList Attribute to record. The AdditionalProtocolDescriptorLists
 *        attribute supports services that require more channels in addition to the service described
 *        in ProtocolDescriptorList. Refer to charter 5.1.6 of Core 5.0.
 *
 * @param handle           Service record handle.
 * @param descriptorList   A sequence of ProtocolDescriptorList-elements.
 * @param descriptorNumber The number of ProtocolDescriptorList-element.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddAdditionalProtocolDescriptorList(
    uint32_t handle, const SdpAdditionalProtocolDescriptor *descriptorList, uint16_t descriptorListNumber);

/**
 * @brief Add BrowseGroupList Attribute to record. The top-level browse group ID, called PublicBrowseRoot
 *        and representing the root of the browsing hierarchy, has the default value 0x1002 (UUID16).
 *        Refer to charter 5.1.7 of Core 5.0.
 *
 * @param handle           Service record handle.
 * @param browseUuid       The list of UUIDs that represents a browse group.
 * @param browseUuidNumber The number of UUIDs
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddBrowseGroupList(uint32_t handle, const BtUuid *browseUuid, uint16_t browseUuidNumber);

/**
 * @brief Add LanguageBaseAttributeIDList Attribute to record. The LanguageBaseAttributeIDList attribute
 *        is a list in which each member containsa language identifier, a character encoding identifier,
 *        and a base attribute ID for each of the natural languages used in the service record.
 *        Refer to charter 5.1.8 of Core 5.0.
 *
 * @param handle                Service record handle.
 * @param baseAttributeId       The list of the natural languages' attribute IDs.
 * @param baseAttributeIdNum the number of the natural languages used in the service record.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddLanguageBaseAttributeIdList(
    uint32_t handle, const SdpLanguageBaseAttributeId *baseAttributeId, uint16_t baseAttributeIdNum);

/**
 * @brief Add ServiceInfoTimeToLive Attribute to record. The ServiceTimeToLive attribute is a 32-bit
 *        integer that contains the number of seconds for which the information in a service record
 *        is expected to remain valid and unchanged. Refer to charter 5.1.9 of Core 5.0.
 *
 * @param handle Service record handle.
 * @param value  Time interval in seconds.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddServiceInfoTimeToLive(uint32_t handle, uint32_t value);

/**
 * @brief Add ServiceAvailability Attribute to record. The ServiceAvailability attribute is an 8-bit unsigned
 *        integer that represents therelative ability of the service to accept additional clients.
 *        Refer to charter 5.1.10 of Core 5.0.
 *
 * @param handle Service record handle.
 * @param value  The relative ability of the service (idle - 0xFF, busy - 0x00).
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddServiceAvailability(uint32_t handle, uint8_t value);

/**
 * @brief Add BluetoothProfileDescriptorList Attribute to record. The BluetoothProfileDescriptorList attribute
 *        consists of a data element sequence in which each element is a profile descriptor contains information
 *        about a Bluetooth profile to which the service represented by this service record conforms.
 *        Refer to charter 5.1.11 of Core 5.0.
 *
 * @param handle                  Service record handle.
 * @param profileDescriptor       The list of UUIDs and a 16-bit profile version number.
 * @param profileDescriptorNum The number of profile descriptors.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddBluetoothProfileDescriptorList(
    uint32_t handle, const SdpProfileDescriptor *profileDescriptor, uint16_t profileDescriptorNum);

/**
 * @brief Add DocumentationURL Attribute to record. The DocumentationURL attribute is a URL which points to
 *        documentation on the service described by a service record. Refer to charter 5.1.12 of Core 5.0.
 *
 * @param handle Service record handle.
 * @param url    The documentation url.
 * @param urlLen The length of url.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddDocumentationUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen);

/**
 * @brief Add ClientExecutableURL Attribute to record. The ClientExecutableURL attribute is a URL that refers to
 *        the location of an application that can be used to utilize the service described by the service record.
 *        Refer to charter 5.1.13 of Core 5.0.
 *
 * @param handle Service record handle.
 * @param url    The client executable url.
 * @param urlLen The length of url.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddClientExecutableUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen);

/**
 * @brief Add IconURL Attribute to record. The IconURL attribute is a URL that refers to the location of an icon that
 *        can be used to represent the service described by the service record. Refer to charter 5.1.14 of Core 5.0.
 *
 * @param handle Service record handle.
 * @param url    The icon url.
 * @param urlLen The length of url.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddIconUrl(uint32_t handle, const uint8_t *url, uint16_t urlLen);

/**
 * @brief Add ServiceName Attribute to record. The ServiceName attribute is a string containing the name of
 *        the service represented by a service record. Refer to charter 5.1.15 of Core 5.0.
 *
 * @param handle          Service record handle.
 * @param baseAttributeId The attribute ID base (default 0x0100).
 * @param name            The service name.
 * @param nameLen         The length of name.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddServiceName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen);

/**
 * @brief Add ServiceDescription Attribute to record. The ServiceDescription attribute is a string
 *        containing a brief description of the service. Refer to charter 5.1.16 of Core 5.0.
 *
 * @param handle          Service record handle.
 * @param baseAttributeId The attribute ID base (default 0x0100).
 * @param description     A brief description of the service.
 * @param descriptionLen  The length of description.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddServiceDescription(
    uint32_t handle, uint16_t baseAttributeId, const char *description, uint16_t descriptionLen);

/**
 * @brief Add ProviderName Attribute to record. This ProviderName attribute is a string containing the name of
 *        the person or organization providing the service. Refer to charter 5.1.17 of Core 5.0.
 *
 * @param handle          Service record handle.
 * @param baseAttributeId The attribute ID base (default 0x0100).
 * @param name            The name of the person or organization providing the service.
 * @param nameLen         The length of name.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddProviderName(uint32_t handle, uint16_t baseAttributeId, const char *name, uint16_t nameLen);

/**
 * @brief Add attribute undefined in Core 5.0 specification. Refer to charter 2.2 of Core 5.0.
 *
 * @param handle               Service record handle.
 * @param attributeId          Attribute undefined in Core 5.0.
 * @param type                 The type of attribute value.
 * @param attributeValue       Attribute value.
 * @param attributeValueLength The length of attribute value.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddAttribute(
    uint32_t handle, uint16_t attributeId, SdpDataType type, void *attributeValue, uint16_t attributeValueLength);

/**
 * @brief Add attribute, which value is sequence, undefined in Core 5.0 specification. Refer to charter 2.2 of
 * Core 5.0.
 *
 * @param handle               Service record handle.
 * @param attributeId          Attribute undefined in Core 5.0.
 * @param attributeValue       Attribute value.
 * @param attributeValueLength The length of attribute value.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful, otherwise the operation fails.
 */
int BTSTACK_API SDP_AddSequenceAttribute(
    uint32_t handle, uint16_t attributeId, const uint8_t *attributeValue, uint16_t attributeValueLength);

/**
 *  @brief Type of AttributeID list.
 */
typedef enum {
    SDP_TYPE_LIST,
    SDP_TYPE_RANGE,
} SdpAttributeIdListType;

/**
 *  @brief Uuid list.
 */
typedef struct {
    uint16_t uuidNum;
    BtUuid *uuid;
} SdpUuid;

/**
 *  @brief The list is either some attribute IDs or a range of attribute IDs.
 */
typedef struct {
    SdpAttributeIdListType type;
    union {
        struct {
            uint16_t attributeIdNumber;
            uint16_t attributeId[SDP_ATTRIBUTE_ID_LIST_MAX_COUNT];
        } attributeIdList;

        struct {
            uint16_t start;
            uint16_t end;
        } attributeIdRange;
    };
} SdpAttributeIdList;

/**
 *  @brief Get undefined attribute in Core 5.0.
 */
typedef struct {
    uint16_t attributeId;
    SdpDataType type;
    uint16_t attributeValueLength;
    void *attributeValue;
} SdpAttribute;

/**
 *  @brief Add undefined attribute in Core 5.0, which value is sequence.
 */
typedef struct {
    uint16_t attributeId;
    uint16_t attributeValueLength;
    uint8_t *attributeValue;
} SdpSequenceAttribute;

/**
 *  @brief Callback all attribute value.
 */
typedef struct {
    uint32_t handle;

    uint16_t classIdNumber;
    BtUuid *classId;

    uint32_t state;
    BtUuid serviceId;

    uint16_t descriptorNumber;
    SdpProtocolDescriptor *descriptor;

    uint16_t descriptorListNumber;
    SdpAdditionalProtocolDescriptor *descriptorList;

    uint16_t browseUuidNumber;
    BtUuid *browseUuid;

    uint16_t baseAttributeIdNumber;
    SdpLanguageBaseAttributeId *baseAttributeId;

    uint32_t serviceInfoTimeToLive;
    uint8_t serviceAvailability;

    uint16_t profileDescriptorNumber;
    SdpProfileDescriptor *profileDescriptor;

    char *documentationUrl;
    char *clientExecutableUrl;
    char *iconUrl;

    char *serviceName;
    char *serviceDescription;
    char *providerName;

    uint16_t attributeNumber;
    SdpAttribute *attribute;

    uint16_t sequenceAttributeNumber;
    SdpSequenceAttribute *sequenceAttribute;
} SdpService;

/**
 * @brief The SDP client generates an SDP_ServiceSearchRequest to locate service records that match the service
 *        search pattern given as the first parameter of the PDU. Refer to charter 4.5.1 and 4.5.2 of Core 5.0.
 *
 * @param addr      The remote bluetooth device address.
 * @param uuidArray The list of uuids.
 * @param uuidNum   The number of uuids.
 * @param context   The context from upper layer.
 * @param ServiceSearchCb callback.
 * @return Returns <b>BT_NO_ERROE</b> if the operation is successful; returns <b>false</b> if the operation fails.
 */
int BTSTACK_API SDP_ServiceSearch(const BtAddr *addr, const SdpUuid *uuidArray, void *context,
    void (*serviceSearchCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context));

/**
 * @brief The SDP client generates an SDP_ServiceAttributeRequest to retrieve specified attribute values
 *        from a specific service record. Refer to charter 4.6.1 and 4.6.2 of Core 5.0.

 * @param addr   The remote bluetooth device address.
 * @param handle A service record handle.
 * @param attributeIdList The list is either an attribute ID or a range of attribute IDs. Note that
 *                        all attributes can be requested by specifying a range of 0x0000-0xFFFF.
 * @param context The context from upper layer.
 * @param ServiceAttributeCb callback
 * @return Returns <b>BT_NO_ERROE</b> if the operation is successful; returns <b>false</b> if the operation fails.
 */
int BTSTACK_API SDP_ServiceAttribute(const BtAddr *addr, uint32_t handle, SdpAttributeIdList attributeIdList,
    void *context, void (*serviceAttributeCb)(const BtAddr *addr, const SdpService *service, void *context));

/**
 * @brief The SDP_ServiceSearchAttributeRequest transaction combines the capabilities of the SDP_ServiceSearchRequest
 *        and the SDP_ServiceAttributeRequest into a single request. Refer to charter 4.7.1 and 4.7.2 of Core 5.0.
 *
 * @param addr      The remote bluetooth device address.
 * @param uuidArray The list of uuids.
 * @param attributeIdList The list is either an attribute ID or a range of attribute IDs. Note that
 *                        all attributes can be requested by specifying a range of 0x0000-0xFFFF.
 * @param context   The context from upper layer.
 * @param ServiceSearchAttributeCb callback.
 * @return Returns <b>BT_NO_ERROE</b> if the operation is successful; returns <b>false</b> if the operation fails.
 */
int BTSTACK_API SDP_ServiceSearchAttribute(const BtAddr *addr, const SdpUuid *uuidArray,
    SdpAttributeIdList attributeIdList, void *context,
    void (*searchAttributeCb)(const BtAddr *addr, const SdpService *serviceArray, uint16_t serviceNum, void *context));

/**
 * @brief Browse all record handles of remote device. Refer to charter 2.6 of Core 5.0.
 *
 * @param addr    The remote bluetooth device address.
 * @param context The context from upper layer.
 * @param ServiceBrowseCb callback.
 * @return Returns <b>BT_NO_ERROE</b> if the operation is successful; returns <b>false</b> if the operation fails.
 */
int BTSTACK_API SDP_ServiceBrowse(const BtAddr *addr, void *context,
    void (*ServiceBrowseCb)(const BtAddr *addr, const uint32_t *handleArray, uint16_t handleNum, void *context));

#ifdef __cplusplus
}
#endif

#endif  // SDP_H