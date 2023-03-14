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

#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
void WifiP2pWfdInfo::SetWfdEnabled(bool value)
{
    wfdEnabled = value;
}

bool WifiP2pWfdInfo::GetWfdEnabled() const
{
    return wfdEnabled;
}

void WifiP2pWfdInfo::SetDeviceInfo(int info)
{
    deviceInfo = info;
}

int WifiP2pWfdInfo::GetDeviceInfo() const
{
    return deviceInfo;
}

void WifiP2pWfdInfo::SetCtrlPort(int port)
{
    ctrlPort = port;
}

int WifiP2pWfdInfo::GetCtrlPort() const
{
    return ctrlPort;
}

void WifiP2pWfdInfo::SetMaxThroughput(int throughput)
{
    maxThroughput = throughput;
}

int WifiP2pWfdInfo::GetMaxThroughput() const
{
    return maxThroughput;
}

bool WifiP2pWfdInfo::isSessionAvailable()
{
    return (deviceInfo & static_cast<int>(P2pDeviceType::SESSION_AVAILABLE)) != 0;
}

void WifiP2pWfdInfo::setSessionAvailable(bool enabled)
{
    if (enabled) {
        deviceInfo |= static_cast<int>(P2pDeviceType::SESSION_AVAILABLE_BIT1);
        deviceInfo &= ~(static_cast<int>(P2pDeviceType::SESSION_AVAILABLE_BIT2));
    } else {
        deviceInfo &= ~(static_cast<int>(P2pDeviceType::SESSION_AVAILABLE));
    }
}

void WifiP2pWfdInfo::GetDeviceInfoElement(std::string &subelement)
{
    char buf[32];
    subelement = DeviceInfoSubelemLenHex;
    if (snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%04x%04x%04x", deviceInfo, ctrlPort, maxThroughput) < 0) {
        subelement.clear();
        return;
    }
    subelement.append(buf);
}

void WifiP2pDevice::SetDeviceName(const std::string &setDeviceName)
{
    deviceName = setDeviceName;
}

const std::string &WifiP2pDevice::GetDeviceName() const
{
    return deviceName;
}

void WifiP2pDevice::SetDeviceAddress(const std::string &deviceAddress)
{
    mDeviceAddress = deviceAddress;
}

const std::string &WifiP2pDevice::GetDeviceAddress() const
{
    return mDeviceAddress;
}

void WifiP2pDevice::SetPrimaryDeviceType(const std::string &setPrimaryDeviceType)
{
    primaryDeviceType = setPrimaryDeviceType;
}

const std::string &WifiP2pDevice::GetPrimaryDeviceType() const
{
    return primaryDeviceType;
}

void WifiP2pDevice::SetSecondaryDeviceType(const std::string &deviceType)
{
    secondaryDeviceType = deviceType;
}

const std::string &WifiP2pDevice::GetSecondaryDeviceType() const
{
    return secondaryDeviceType;
}

void WifiP2pDevice::SetP2pDeviceStatus(P2pDeviceStatus setStatus)
{
    status = setStatus;
}

P2pDeviceStatus WifiP2pDevice::GetP2pDeviceStatus() const
{
    return status;
}

void WifiP2pDevice::SetWfdInfo(const WifiP2pWfdInfo &info)
{
    wfdInfo = info;
}

const WifiP2pWfdInfo &WifiP2pDevice::GetWfdInfo() const
{
    return wfdInfo;
}

void WifiP2pDevice::SetWpsConfigMethod(unsigned int wpsConfigMethod)
{
    supportWpsConfigMethods = wpsConfigMethod;
}

unsigned int WifiP2pDevice::GetWpsConfigMethod() const
{
    return supportWpsConfigMethods;
}

void WifiP2pDevice::SetDeviceCapabilitys(int capabilitys)
{
    deviceCapabilitys = capabilitys;
}

int WifiP2pDevice::GetDeviceCapabilitys() const
{
    return deviceCapabilitys;
}

void WifiP2pDevice::SetGroupCapabilitys(int capabilitys)
{
    groupCapabilitys = capabilitys;
}

int WifiP2pDevice::GetGroupCapabilitys() const
{
    return groupCapabilitys;
}

bool WifiP2pDevice::IsGroupOwner() const
{
    return (groupCapabilitys & static_cast<int>(P2pGroupCapability::PGC_GROUP_OWNER));
}

bool WifiP2pDevice::IsGroupLimit() const
{
    return (groupCapabilitys & static_cast<int>(P2pGroupCapability::PGC_GROUP_LIMIT));
}

bool WifiP2pDevice::IsDeviceLimit() const
{
    return (deviceCapabilitys & static_cast<int>(P2pDeviceCapability::PDC_DEVICE_LIMIT));
}

bool WifiP2pDevice::Isinviteable() const
{
    return (deviceCapabilitys & static_cast<int>(P2pDeviceCapability::PDC_INVITATION_PROCEDURE));
}

bool WifiP2pDevice::IsValid() const
{
    if (mDeviceAddress.empty()) {
        return false;
    }
    return true;
}

bool WifiP2pDevice::operator==(const WifiP2pDevice &cmp) const
{
    return mDeviceAddress == cmp.GetDeviceAddress();
}

bool WifiP2pDevice::operator!=(const WifiP2pDevice &cmp) const
{
    return !operator==(cmp);
}

bool WifiP2pDevice::WpsPbcSupported() const
{
    return (supportWpsConfigMethods & static_cast<int>(WpsConfigMethod::WPS_CFG_PUSHBUTTON)) != 0;
}

bool WifiP2pDevice::WpsDisplaySupported() const
{
    return (supportWpsConfigMethods & static_cast<int>(WpsConfigMethod::WPS_CFG_DISPLAY)) != 0;
}

bool WifiP2pDevice::WpKeypadSupported() const
{
    return (supportWpsConfigMethods & static_cast<int>(WpsConfigMethod::WPS_CFG_KEYPAD)) != 0;
}

bool WifiP2pDevice::isGroupOwner() const
{
    return (groupCapabilitys & static_cast<int>(P2pGroupCapability::PGC_GROUP_OWNER)) != 0;
}

bool WifiP2pGroupInfo::operator==(const WifiP2pGroupInfo &group) const
{
    return networkId == group.GetNetworkId();
}

bool WifiP2pGroupInfo::operator!=(const WifiP2pGroupInfo &group) const
{
    return !operator==(group);
}

void WifiP2pGroupInfo::SetIsGroupOwner(bool isGroupOwner)
{
    isP2pGroupOwner = isGroupOwner;
}

bool WifiP2pGroupInfo::IsGroupOwner() const
{
    return isP2pGroupOwner;
}

void WifiP2pGroupInfo::SetOwner(const WifiP2pDevice &setOwner)
{
    owner = setOwner;
}

const WifiP2pDevice &WifiP2pGroupInfo::GetOwner() const
{
    return owner;
}

void WifiP2pGroupInfo::SetPassphrase(const std::string &setPassphrase)
{
    passphrase = setPassphrase;
}

const std::string &WifiP2pGroupInfo::GetPassphrase() const
{
    return passphrase;
}

void WifiP2pGroupInfo::SetInterface(const std::string &setInterface)
{
    interface = setInterface;
}

const std::string &WifiP2pGroupInfo::GetInterface() const
{
    return interface;
}

void WifiP2pGroupInfo::SetGroupName(const std::string &newGroupName)
{
    groupName = newGroupName;
}

const std::string &WifiP2pGroupInfo::GetGroupName() const
{
    return groupName;
}

void WifiP2pGroupInfo::SetFrequency(int setFrequency)
{
    frequency = setFrequency;
}

int WifiP2pGroupInfo::GetFrequency() const
{
    return frequency;
}

void WifiP2pGroupInfo::SetIsPersistent(bool isPersistent)
{
    isP2pPersistent = isPersistent;
}

bool WifiP2pGroupInfo::IsPersistent() const
{
    return isP2pPersistent;
}

void WifiP2pGroupInfo::SetP2pGroupStatus(P2pGroupStatus newGroupStatus)
{
    groupStatus = newGroupStatus;
}

P2pGroupStatus WifiP2pGroupInfo::GetP2pGroupStatus() const
{
    return groupStatus;
}

void WifiP2pGroupInfo::SetNetworkId(int nwId)
{
    networkId = nwId;
}

const int &WifiP2pGroupInfo::GetNetworkId() const
{
    return networkId;
}

void WifiP2pGroupInfo::SetGoIpAddress(const std::string &ipAddr)
{
    goIpAddress = ipAddr;
}

const std::string &WifiP2pGroupInfo::GetGoIpAddress() const
{
    return goIpAddress;
}

void WifiP2pGroupInfo::AddClientDevice(const WifiP2pDevice &clientDevice)
{
    for (auto it = clientDevices.begin(); it != clientDevices.end(); ++it) {
        if (*it == clientDevice) {
            *it = clientDevice;
            return;
        }
    }
    clientDevices.push_back(clientDevice);
    return;
}

void WifiP2pGroupInfo::RemoveClientDevice(const WifiP2pDevice &clientDevice)
{
    for (auto it = clientDevices.begin(); it != clientDevices.end(); ++it) {
        if (*it == clientDevice) {
            clientDevices.erase(it);
            return;
        }
    }
    return;
}

bool WifiP2pGroupInfo::IsContainsDevice(const WifiP2pDevice &clientDevice) const
{
    if (clientDevice == owner) {
        return true;
    }
    for (auto it = clientDevices.begin(); it != clientDevices.end(); ++it) {
        if (*it == clientDevice) {
            return true;
        }
    }
    return false;
}

bool WifiP2pGroupInfo::IsClientDevicesEmpty() const
{
    return clientDevices.empty();
}

const std::vector<WifiP2pDevice> &WifiP2pGroupInfo::GetClientDevices() const
{
    return clientDevices;
}

void WifiP2pGroupInfo::SetClientDevices(const std::vector<WifiP2pDevice> &devices)
{
    clientDevices = devices;
}

void WifiP2pGroupInfo::ClearClientDevices()
{
    clientDevices.clear();
}

void WpsInfo::SetWpsMethod(WpsMethod wpsMethod)
{
    mWpsMethod = wpsMethod;
}

WpsMethod WpsInfo::GetWpsMethod() const
{
    return mWpsMethod;
}

void WpsInfo::SetBssid(const std::string &setBssid)
{
    bssid = setBssid;
}

const std::string &WpsInfo::GetBssid() const
{
    return bssid;
}

void WpsInfo::SetPin(const std::string &setPin)
{
    pin = setPin;
}

const std::string &WpsInfo::GetPin() const
{
    return pin;
}

void WifiP2pConfig::SetDeviceAddress(const std::string &deviceAddress)
{
    mDeviceAddress = deviceAddress;
}

const std::string &WifiP2pConfig::GetDeviceAddress() const
{
    return mDeviceAddress;
}

void WifiP2pConfig::SetGoBand(GroupOwnerBand setGoBand)
{
    goBand = setGoBand;
}

GroupOwnerBand WifiP2pConfig::GetGoBand() const
{
    return goBand;
}

void WifiP2pConfig::SetNetId(int setNetId)
{
    netId = setNetId;
}

int WifiP2pConfig::GetNetId() const
{
    return netId;
}

void WifiP2pConfig::SetPassphrase(const std::string &newPassphrase)
{
    passphrase = newPassphrase;
}

const std::string &WifiP2pConfig::GetPassphrase() const
{
    return passphrase;
}

void WifiP2pConfig::SetGroupOwnerIntent(int intent)
{
    groupOwnerIntent = intent;
}

int WifiP2pConfig::GetGroupOwnerIntent() const
{
    return groupOwnerIntent;
}

void WifiP2pConfig::SetGroupName(const std::string &setGroupName)
{
    groupName = setGroupName;
}

const std::string &WifiP2pConfig::GetGroupName() const
{
    return groupName;
}

void WifiP2pLinkedInfo::SetConnectState(P2pConnectedState setConnectState)
{
    connectState = setConnectState;
}

P2pConnectedState WifiP2pLinkedInfo::GetConnectState() const
{
    return connectState;
}

void WifiP2pLinkedInfo::SetIsGroupOwner(bool isGroupOwner)
{
    isP2pGroupOwner = isGroupOwner;
}

const bool &WifiP2pLinkedInfo::IsGroupOwner() const
{
    return isP2pGroupOwner;
}

void WifiP2pLinkedInfo::SetIsGroupOwnerAddress(const std::string &setGroupOwnerAddress)
{
    groupOwnerAddress = setGroupOwnerAddress;
}

const std::string &WifiP2pLinkedInfo::GetGroupOwnerAddress() const
{
    return groupOwnerAddress;
}

void WifiP2pServiceRequest::SetProtocolType(P2pServicerProtocolType serviceProtocolType)
{
    mProtocolType = serviceProtocolType;
}

P2pServicerProtocolType WifiP2pServiceRequest::GetProtocolType() const
{
    return mProtocolType;
}

void WifiP2pServiceRequest::SetTransactionId(unsigned char transactionId)
{
    mTransactionId = transactionId;
}

int WifiP2pServiceRequest::GetTransactionId() const
{
    return mTransactionId;
}

void WifiP2pServiceRequest::SetQuery(const std::vector<unsigned char> &query)
{
    mQuery = query;
}

const std::vector<unsigned char> &WifiP2pServiceRequest::GetQuery() const
{
    return mQuery;
}

std::vector<unsigned char> WifiP2pServiceRequest::GetTlv() const
{
    std::vector<unsigned char> ret;

    unsigned short length = PROTOCOL_SIZE + TRANSACTION_ID_SIZE + mQuery.size();
    if (length > 0xff) {
        unsigned char buf1 = length & 0x00ff;
        unsigned char buf2 = length >> CHAR_BIT;
        ret.push_back(buf1);
        ret.push_back(buf2);
    } else {
        ret.push_back(static_cast<unsigned char>(length));
        ret.push_back(0x00);
    }
    ret.push_back(static_cast<unsigned char>(mProtocolType));
    ret.push_back(mTransactionId);
    if (!mQuery.empty()) {
        for (auto it = mQuery.begin(); it != mQuery.end(); ++it) {
            ret.push_back(*it);
        }
    }

    return ret;
}

bool WifiP2pServiceRequest::operator==(const WifiP2pServiceRequest &cmp) const
{
    return mProtocolType == cmp.GetProtocolType() && mQuery == cmp.GetQuery();
}

void WifiP2pServiceResponse::SetProtocolType(P2pServicerProtocolType serviceProtocolType)
{
    mProtocolType = serviceProtocolType;
}

P2pServicerProtocolType WifiP2pServiceResponse::GetProtocolType() const
{
    return mProtocolType;
}

void WifiP2pServiceResponse::SetTransactionId(unsigned char transactionId)
{
    mTransactionId = transactionId;
}

unsigned char WifiP2pServiceResponse::GetTransactionId() const
{
    return mTransactionId;
}

void WifiP2pServiceResponse::SetServiceStatus(P2pServiceStatus serviceStatus)
{
    mServiceStatus = serviceStatus;
}

P2pServiceStatus WifiP2pServiceResponse::GetServiceStatus() const
{
    return mServiceStatus;
}

void WifiP2pServiceResponse::SetServiceName(const std::string &name)
{
    mSvrName = name;
}

const std::string &WifiP2pServiceResponse::GetServiceName() const
{
    return mSvrName;
}

void WifiP2pServiceResponse::SetData(const std::vector<unsigned char> &data)
{
    responseData = data;
}

const std::vector<unsigned char> &WifiP2pServiceResponse::GetData() const
{
    return responseData;
}

std::vector<unsigned char> WifiP2pServiceResponse::GetTlv() const
{
    std::vector<unsigned char> ret;
    unsigned short length = PROTOCOL_SIZE + TRANSACTION_ID_SIZE + SERVICE_STATUS_SIZE + responseData.size();
    if (length > 0xff) {
        unsigned char buf1 = length & 0x00ff;
        unsigned char buf2 = length >> CHAR_BIT;
        ret.push_back(buf1);
        ret.push_back(buf2);
    } else {
        ret.push_back(static_cast<unsigned char>(length));
        ret.push_back(0x00);
    }
    ret.push_back(static_cast<unsigned char>(mProtocolType));
    ret.push_back(mTransactionId);
    ret.push_back(static_cast<unsigned char>(mServiceStatus));
    if (!responseData.empty()) {
        for (auto it = responseData.begin(); it != responseData.end(); ++it) {
            ret.push_back(*it);
        }
    }
    return ret;
}

bool WifiP2pServiceResponse::operator==(const WifiP2pServiceResponse &cmp) const
{
    return mProtocolType == cmp.GetProtocolType() && mServiceStatus == cmp.GetServiceStatus() &&
           responseData == cmp.GetData();
}

void WifiP2pServiceInfo::SetServiceName(const std::string &name)
{
    serviceName = name;
}

const std::string &WifiP2pServiceInfo::GetServiceName() const
{
    return serviceName;
}

void WifiP2pServiceInfo::SetDeviceAddress(const std::string &deviceAddress)
{
    mDeviceAddress = deviceAddress;
}

const std::string &WifiP2pServiceInfo::GetDeviceAddress() const
{
    return mDeviceAddress;
}

void WifiP2pServiceInfo::SetServicerProtocolType(P2pServicerProtocolType type)
{
    mProtocolType = type;
}

P2pServicerProtocolType WifiP2pServiceInfo::GetServicerProtocolType() const
{
    return mProtocolType;
}

void WifiP2pServiceInfo::SetQueryList(const std::vector<std::string> &queryList)
{
    mQueryList = queryList;
}

const std::vector<std::string> &WifiP2pServiceInfo::GetQueryList() const
{
    return mQueryList;
}

bool WifiP2pServiceInfo::operator==(const WifiP2pServiceInfo &cmp) const
{
    return cmp.GetServiceName() == serviceName && cmp.GetDeviceAddress() == mDeviceAddress &&
           cmp.GetServicerProtocolType() == mProtocolType && cmp.GetQueryList() == mQueryList;
}

/**
 * @Description - For a request sent by the peer device, serviceInfo processes the query information in the request
 * packet and returns the processing result.
 * @param  Query - the query of service request.
 * @param  data - the data of response.
 * @return - P2pServiceStatus - result of processing.
 */
P2pServiceStatus WifiP2pServiceInfo::ProcessServiceRequest(
    const std::vector<unsigned char> &Query, std::vector<unsigned char> &data) const
{
    data = Query;
    data.push_back(0x00);
    data.push_back(static_cast<unsigned char>(mProtocolType));
    data.push_back(0x00);
    return P2pServiceStatus::PSRS_SUCCESS;
}

/**
 * @Description - Processes the data in the response returned by the peer device.
 * @param  data - the response received from the peer device.
 */
void WifiP2pServiceInfo::ProcessServiceResponse(const std::vector<unsigned char> &data) const
{
    if (data.empty()) {
        return;
    }
}

std::string WifiP2pServiceInfo::Bin2HexStr(std::vector<unsigned char> data)
{
    std::string dataString;
    for (auto bufData : data) {
        char buf[3];
        if (sprintf_s(buf, sizeof(buf), "%x", bufData & 0xff) < 0) {
            return "";
        }
        std::string aDataString = buf;
        if (aDataString.length() == 1) {
            dataString += std::string("0");
        }
        dataString += aDataString;
    }
    return dataString;
}

std::string WifiP2pServiceInfo::Bin2HexStr(std::string data)
{
    std::string dataString;
    for (auto bufData : data) {
        char buf[3];
        if (sprintf_s(buf, sizeof(buf), "%x", bufData & 0xff) < 0) {
            return "";
        }
        std::string aDataString = buf;
        if (aDataString.length() == 1) {
            dataString += std::string("0");
        }
        dataString += aDataString;
    }
    return dataString;
}

bool P2pVendorConfig::GetRandomMacSupport() const
{
    return randomMacSupport;
}

void P2pVendorConfig::SetRandomMacSupport(bool support)
{
    randomMacSupport = support;
}

bool P2pVendorConfig::GetIsAutoListen() const
{
    return isAutoListen;
}

void P2pVendorConfig::SetIsAutoListen(bool autoListen)
{
    isAutoListen = autoListen;
}

const std::string &P2pVendorConfig::GetDeviceName() const
{
    return deviceName;
}

void P2pVendorConfig::SetDeviceName(const std::string &name)
{
    deviceName = name;
}

const std::string &P2pVendorConfig::GetPrimaryDeviceType() const
{
    return primaryDeviceType;
}

void P2pVendorConfig::SetPrimaryDeviceType(const std::string &setPrimaryDeviceType)
{
    primaryDeviceType = setPrimaryDeviceType;
}

const std::string &P2pVendorConfig::GetSecondaryDeviceType() const
{
    return secondaryDeviceType;
}

void P2pVendorConfig::SetSecondaryDeviceType(const std::string &setSecondaryDeviceType)
{
    secondaryDeviceType = setSecondaryDeviceType;
}
}  // namespace Wifi
}  // namespace OHOS