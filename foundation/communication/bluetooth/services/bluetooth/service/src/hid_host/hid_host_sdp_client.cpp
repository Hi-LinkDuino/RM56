/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <cstring>

#include "hid_host_service.h"
#include "hid_host_sdp_client.h"

namespace OHOS {
namespace bluetooth {
HidHostSdpClient::HidHostSdpClient(std::string address)
{
    currentAddr_ = address;
}

HidHostSdpClient::~HidHostSdpClient()
{
}

void HidHostSdpClient::SdpCallback(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum, void *context)
{
    int result = HID_HOST_SDP_FAILD;
    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    if (context == nullptr) {
        HidHostMessage event(HID_HOST_SDP_CMPL_EVT, result);
        event.dev_ = address;
        HidHostService::GetService()->PostEvent(event);
        return;
    }
    HidHostSdpClient *sdpClient = static_cast<HidHostSdpClient *>(context);
    sdpClient->SdpCallback_(addr, serviceAry, serviceNum);
}

void HidHostSdpClient::SdpCallback_(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum)
{
    LOG_DEBUG("[HIDH SDP]%{public}s()", __FUNCTION__);
    int result = HID_HOST_SDP_FAILD;
    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    if (serviceNum > 0) {
        if (serviceAry[0].serviceName != nullptr) {
            hidInf_.serviceName = serviceAry[0].serviceName;
        } else {
            hidInf_.serviceName = "";
        }
        if (serviceAry[0].serviceDescription != nullptr) {
            hidInf_.serviceDescription = serviceAry[0].serviceDescription;
        } else {
            hidInf_.serviceDescription = "";
        }
        if (serviceAry[0].providerName != nullptr) {
            hidInf_.providerName = serviceAry[0].providerName;
        } else {
            hidInf_.providerName = "";
        }
        if (!ParseHidDescInfo(serviceAry)) {
            return;
        }
        for (uint16_t i = 0; i < serviceAry[0].attributeNumber; i++) {
            SdpAttribute attribute = serviceAry[0].attribute[i];
            if (attribute.attributeId == ATTR_ID_HID_COUNTRY_CODE &&
                attribute.attributeValueLength == HID_HOST_ATTRIBUTE_LENGTH_UINT8) {
                hidInf_.ctryCode = *static_cast<uint8_t*>(serviceAry[0].attribute[i].attributeValue);
            } else {
                LOG_ERROR("[HIDH SDP]%{public}s() attribute length is error!", __FUNCTION__);
            }
        }
        result = HID_HOST_SDP_SUCCESS;
        isSdpDone_ = true;
        printHidSdpInfo();
    }
    SendSdpComplete(result);
}

bool HidHostSdpClient::ParseHidDescInfo(const SdpService *serviceAry)
{
    int result = HID_HOST_SDP_FAILD;
    for (uint16_t i = 0; i < serviceAry[0].sequenceAttributeNumber; i++) {
        SdpSequenceAttribute attribute = serviceAry[0].sequenceAttribute[i];
        uint8_t offset = CheckAttributeValueLengthAvalid(attribute);
        if (offset != 0) {
            uint8_t *attributeValue = attribute.attributeValue + offset;
            uint8_t type = attributeValue[0] >> HID_SDP_DESCRIPTOR_SIZE_BIT;
            uint8_t size = attributeValue[0] & SDP_SIZE_MASK;
            uint16_t descLength = 0;
            if ((type  == SDP_DE_TYPE_STRING) && (size == SDP_DE_SIZE_VAR_8)) {
                descLength = attributeValue[1];
                offset = SDP_UINT8_LENGTH + 1;
            } else if ((type  == SDP_DE_TYPE_STRING) && (size == SDP_DE_SIZE_VAR_16)) {
                offset = 1;
                descLength = (attributeValue[offset] << ONE_BYTE_OFFSET) | attributeValue[offset + 1];
                offset = SDP_UINT16_LENGTH + 1;
            } else {
                LOG_ERROR("[HIDH SDP]%{public}s() error type or size!", __FUNCTION__);
                SendSdpComplete(result);
                offset = 0;
                return false;
            }
            if (descLength <= 0) {
                LOG_ERROR("[HIDH SDP]%{public}s() length is 0!", __FUNCTION__);
                SendSdpComplete(result);
                offset = 0;
                return false;
            }
            hidInf_.descInfo = std::make_unique<uint8_t[]>(descLength);
            if (memcpy_s(hidInf_.descInfo.get(), descLength, attributeValue + offset, descLength) != EOK) {
                LOG_ERROR("[HIDH SDP]%{public}s() memcpy error", __FUNCTION__);
                SendSdpComplete(result);
                return false;
            }
            hidInf_.descLength = descLength;
            return true;
        }
    }
    LOG_ERROR("[HIDH SDP]%{public}s() not find hid descriptor list!", __FUNCTION__);
    SendSdpComplete(result);
    return false;
}

uint8_t HidHostSdpClient::CheckAttributeValueLengthAvalid(SdpSequenceAttribute attribute)
{
    int result = HID_HOST_SDP_FAILD;
    uint8_t offset = 0;
    if (attribute.attributeId == ATTR_ID_HID_DESCRIPTOR_LIST) {
        if (attribute.attributeValueLength > DESCRIPTOR_LIST_SDP_HEAD_LENGTH) {
            if ((attribute.attributeValue[0] & SDP_SIZE_MASK) == SDP_DE_SIZE_VAR_8) {
                offset = DESCRIPTOR_LIST_SDP_OFFSET;
            } else if ((attribute.attributeValue[0] & SDP_SIZE_MASK) == SDP_DE_SIZE_VAR_16) {
                offset = DESCRIPTOR_LIST_SDP_OFFSET + 1;
            } else {
                LOG_ERROR("[HIDH SDP]%{public}s() first error size!", __FUNCTION__);
                SendSdpComplete(result);
                return offset;
            }
        } else {
            LOG_ERROR("[HIDH SDP]%{public}s() attribute length is error!", __FUNCTION__);
            SendSdpComplete(result);
            return offset;
        }
    } else {
        LOG_ERROR("[HIDH SDP]%{public}s() attribute id is invalid!", __FUNCTION__);
    }
    return offset;
}

void HidHostSdpClient::SendSdpComplete(int result)
{
    HidHostMessage event(HID_HOST_SDP_CMPL_EVT, result);
    event.dev_ = currentAddr_;
    HidHostService::GetService()->PostEvent(event);
}

void HidHostSdpClient::printHidSdpInfo()
{
    LOG_DEBUG("[HIDH SDP]%{public}s()", __FUNCTION__);
    LOG_DEBUG("[HIDH SDP]supTimeout:0x%{public}x,MaxLatency:0x%{public}x,MinTout:0x%{public}x,hparsVer:0x%{public}x",
        hidInf_.supTimeout, hidInf_.ssrMaxLatency, hidInf_.ssrMinTout, hidInf_.hparsVer);
    LOG_DEBUG("[HIDH SDP]relNum:0x%{public}x,ctryCode:0x%{public}x,subClass:0x%{public}x,descLength:%{public}d",
        hidInf_.relNum, hidInf_.ctryCode, hidInf_.subClass, hidInf_.descLength);
    LOG_DEBUG("[HIDH SDP]serviceName:%{public}s,serviceDescription:%{public}s,providerName:%{public}s",
        hidInf_.serviceName.c_str(), hidInf_.serviceDescription.c_str(), hidInf_.providerName.c_str());
}

void HidHostSdpClient::SdpPnpCallback(const BtAddr *addr, const SdpService *serviceAry,
    uint16_t serviceNum, void *context)
{
    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    int result = HID_HOST_SDP_FAILD;
    if (context == nullptr) {
        HidHostMessage event(HID_HOST_SDP_CMPL_EVT, result);
        event.dev_ = address;
        HidHostService::GetService()->PostEvent(event);
        return;
    }
    HidHostSdpClient *sdpClient = static_cast<HidHostSdpClient *>(context);
    sdpClient->SdpPnpCallback_(addr, serviceAry, serviceNum);
}

void HidHostSdpClient::SdpPnpCallback_(const BtAddr *addr, const SdpService *serviceAry, uint16_t serviceNum)
{
    LOG_DEBUG("[HIDH SDP]%{public}s()", __FUNCTION__);
    std::string address = RawAddress::ConvertToString(addr->addr).GetAddress();
    int result = HID_HOST_SDP_FAILD;

    if (serviceNum > 0) {
        for (uint16_t i = 0; i < serviceAry[0].attributeNumber; i++) {
            SdpAttribute attribute = serviceAry[0].attribute[i];
            if (attribute.attributeId == SDP_ATTRIBUTE_VENDOR_ID &&
                attribute.attributeValueLength == HID_HOST_ATTRIBUTE_LENGTH_UINT16) {
                pnpInf_.vendorId = *static_cast<uint16_t*>(serviceAry[0].attribute[i].attributeValue);
                LOG_DEBUG("[HIDH SDP]%{public}s():vendorId = 0x%{public}x", __FUNCTION__, pnpInf_.vendorId);
            }

            if (attribute.attributeId == SDP_ATTRIBUTE_PRODUCT_ID &&
                attribute.attributeValueLength == HID_HOST_ATTRIBUTE_LENGTH_UINT16) {
                pnpInf_.productId = *static_cast<uint16_t*>(serviceAry[0].attribute[i].attributeValue);
                LOG_DEBUG("[HIDH SDP]%{public}s():productId = 0x%{public}x", __FUNCTION__, pnpInf_.productId);
            }

            if (attribute.attributeId == SDP_ATTRIBUTE_VERSION &&
                attribute.attributeValueLength == HID_HOST_ATTRIBUTE_LENGTH_UINT16) {
                pnpInf_.version = *static_cast<uint16_t*>(serviceAry[0].attribute[i].attributeValue);
                LOG_DEBUG("[HIDH SDP]%{public}s():version = 0x%{public}x", __FUNCTION__, pnpInf_.version);
            }
        }
        result = HID_HOST_SDP_SUCCESS;
        isPnpSdpDone_ = true;
    }
    HidHostMessage event(HID_HOST_SDP_CMPL_EVT, result);
    event.dev_ = currentAddr_;
    HidHostService::GetService()->PostEvent(event);
}

int HidHostSdpClient::DoDiscovery(const std::string &remoteAddr)
{
    if (isPnpSdpDone_) {
        return DoHidDiscovery(remoteAddr);
    }
    return DoPnpDiscovery(remoteAddr);
}

int HidHostSdpClient::DoPnpDiscovery(const std::string &remoteAddr)
{
    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(remoteAddr);
    rawAddr.ConvertToUint8(address.addr);

    BtUuid classid[HID_HOST_CLASSID_NUM];
    classid[0].type = BT_UUID_16;
    classid[0].uuid16 = HID_HOST_UUID_SERVCLASS_PNP;
    SdpUuid sdpUUid;
    sdpUUid.uuidNum = HID_HOST_CLASSID_NUM;
    sdpUUid.uuid = &classid[0];

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_RANGE;
    attributeIdList.attributeIdRange.start = 0x0000;
    attributeIdList.attributeIdRange.end = 0xFFFF;

    int ret = SDP_ServiceSearchAttribute(&address, &sdpUUid, attributeIdList, this, SdpPnpCallback);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[HIDH SDP]%{public}s():SDP_ServiceSearchAttribute failed!", __FUNCTION__);
    }
    currentAddr_ = remoteAddr;
    return ret;
}

int HidHostSdpClient::DoHidDiscovery(const std::string &remoteAddr)
{
    BtAddr address;
    address.type = BT_PUBLIC_DEVICE_ADDRESS;
    RawAddress rawAddr(remoteAddr);
    rawAddr.ConvertToUint8(address.addr);

    BtUuid classid[HID_HOST_CLASSID_NUM];
    classid[0].type = BT_UUID_16;
    classid[0].uuid16 = HID_HOST_UUID_SERVCLASS_HID;
    SdpUuid sdpUUid;
    sdpUUid.uuidNum = HID_HOST_CLASSID_NUM;
    sdpUUid.uuid = &classid[0];

    SdpAttributeIdList attributeIdList;
    attributeIdList.type = SDP_TYPE_LIST;
    int attributeIdNumber = 0;
    attributeIdList.attributeIdList.attributeId[attributeIdNumber++] =
        SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE + SDP_ATTRIBUTE_PROVIDER_NAME;
    attributeIdList.attributeIdList.attributeId[attributeIdNumber++] =
        SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE + SDP_ATTRIBUTE_SERVICE_NAME;
    attributeIdList.attributeIdList.attributeId[attributeIdNumber++] =
        SDP_ATTRIBUTE_PRIMARY_LANGUAGE_BASE + SDP_ATTRIBUTE_DESCRIPTOR;
    attributeIdList.attributeIdList.attributeId[attributeIdNumber++] = ATTR_ID_HID_COUNTRY_CODE;
    attributeIdList.attributeIdList.attributeId[attributeIdNumber++] = ATTR_ID_HID_DESCRIPTOR_LIST;
    attributeIdList.attributeIdList.attributeIdNumber = attributeIdNumber;

    int ret = SDP_ServiceSearchAttribute(&address, &sdpUUid, attributeIdList, this, SdpCallback);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[HIDH SDP]%{public}s():SDP_ServiceSearchAttribute failed!", __FUNCTION__);
    }
    return ret;
}

bool HidHostSdpClient::CheckIsSdpDone()
{
    return isSdpDone_;
}

PnpInformation& HidHostSdpClient::GetRemoteSdpPnpInfo()
{
    return pnpInf_;
}

HidInformation& HidHostSdpClient::GetRemoteSdpHidInfo()
{
    return hidInf_;
}
}  // namespace bluetooth
}  // namespace OHOS
