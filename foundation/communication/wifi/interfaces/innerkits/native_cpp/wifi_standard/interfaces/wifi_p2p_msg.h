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

#ifndef OHOS_WIFI_P2P_MSG_H
#define OHOS_WIFI_P2P_MSG_H

#include <string>
#include <vector>
#include <climits>
#include "securec.h"

namespace OHOS {
namespace Wifi {
constexpr int WIFI_STR_MAC_LENGTH = 17;
constexpr int MAX_PASSPHRASE_LENGTH = 63;
constexpr int DEVICE_NAME_LENGTH = 32;

enum class P2pGroupStatus { GS_CREATING, GS_CREATED, GS_STARTED, GS_REMOVING, GS_INVALID };
enum class P2pServiceStatus : unsigned char {
    PSRS_SUCCESS,
    PSRS_SERVICE_PROTOCOL_NOT_AVAILABLE,
    PSRS_REQUESTED_INFORMATION_NOT_AVAILABLE,
    PSRS_BAD_REQUEST,
};

enum class P2pServicerProtocolType : unsigned char {
    SERVICE_TYPE_ALL = 0,
    SERVICE_TYPE_BONJOUR = 1,
    SERVICE_TYPE_UP_NP = 2,
    SERVICE_TYPE_WS_DISCOVERY = 3,
    SERVICE_TYPE_VENDOR_SPECIFIC = 255,
};

enum class P2pActionCallback : unsigned char {
    DiscoverDevices,
    StopDiscoverDevices,
    DiscoverServices,
    StopDiscoverServices,
    PutLocalP2pService,
    DeleteLocalP2pService,
    RequestService,
    StartP2pListen,
    StopP2pListen,
    CreateGroup,
    RemoveGroup,
    DeleteGroup,
    P2pConnect,
    P2pCancelConnect,
    P2pSetDeviceName,
    CreateHid2dGroup,
    Hid2dConnect,
    UNKNOWN
};

enum class P2pState {
    P2P_STATE_NONE = 0,
    P2P_STATE_IDLE,
    P2P_STATE_STARTING,
    P2P_STATE_STARTED,
    P2P_STATE_CLOSING,
    P2P_STATE_CLOSED,
};

enum class P2pDiscoverState {
    P2P_DISCOVER_NONE = 0,
    P2P_DISCOVER_STARTING,
    P2P_DISCOVER_CLOSED,
};

enum class P2pConnectedState {
    P2P_DISCONNECTED = 0,
    P2P_CONNECTED,
};

enum class P2pWfdInfoType {
    WFD_SOURCE = 0x00,
    PRIMARY_SINK = 0x01,
    SECONDARY_SINK= 0x10,
    SOURCE_OR_PRIMARY_SINK= 0x11
};
enum class P2pDeviceType {
    DEVICE_TYPE = 0x3,
    COUPLED_SINK_SUPPORT_AT_SOURCE = 0x4,
    COUPLED_SINK_SUPPORT_AT_SINK = 0x8,
    SESSION_AVAILABLE = 0x30,
    SESSION_AVAILABLE_BIT1 = 0x10,
    SESSION_AVAILABLE_BIT2 = 0x20
};

enum class P2pDeviceStatus { PDS_CONNECTED, PDS_INVITED, PDS_FAILED, PDS_AVAILABLE, PDS_UNAVAILABLE };

enum class WpsMethod { WPS_METHOD_PBC, WPS_METHOD_DISPLAY, WPS_METHOD_KEYPAD, WPS_METHOD_LABEL, WPS_METHOD_INVALID };

enum class WpsConfigMethod {
    WPS_CFG_INVALID = 0,
    WPS_CFG_DISPLAY = 0x0008,
    WPS_CFG_PUSHBUTTON = 0x0080,
    WPS_CFG_KEYPAD = 0x0100,
};

enum class P2pDeviceCapability {
    PDC_SERVICE_DISCOVERY = 1,
    PDC_CLIENT_DISCOVERABILITY = 1 << 1,
    PDC_STARTED_CONCURRENT_OPER = 1 << 2,
    PDC_REMOVING_INFRA_MANAGED = 1 << 3,
    PDC_DEVICE_LIMIT = 1 << 4,
    PDC_INVITATION_PROCEDURE = 1 << 5
};

enum class P2pGroupCapability {
    PGC_GROUP_OWNER = 1,
    PGC_PERSISTENT_GROUP = 1 << 1,
    PGC_GROUP_LIMIT = 1 << 2,
    PGC_INTRA_BSS_DIST = 1 << 3,
    PGC_CROSS_CONN = 1 << 4,
    PGC_PERSISTENT_RECONN = 1 << 5,
    PGC_GROUP_FORMATION = 1 << 6,
    PGC_IP_ADDR_ALLOC = 1 << 7
};

enum class GroupOwnerBand { GO_BAND_AUTO, GO_BAND_2GHZ, GO_BAND_5GHZ };
const int MAX_WFD_SUBELEMS = 12;
const char DeviceInfoSubelemLenHex[] = {"0006"};
class WifiP2pWfdInfo {
public:
    WifiP2pWfdInfo() : wfdEnabled(false), deviceInfo(0), ctrlPort(0), maxThroughput(0)
    {}
    WifiP2pWfdInfo(int info, int port, int throughput)
        : wfdEnabled(true), deviceInfo(info), ctrlPort(port), maxThroughput(throughput)
    {}
    ~WifiP2pWfdInfo()
    {}
    void SetWfdEnabled(bool value);
    bool GetWfdEnabled() const;
    void SetDeviceInfo(int info);
    int GetDeviceInfo() const;
    void SetCtrlPort(int port);
    int GetCtrlPort() const;
    void SetMaxThroughput(int throughput);
    int GetMaxThroughput() const;
    bool isSessionAvailable();
    void setSessionAvailable(bool enabled);
    void GetDeviceInfoElement(std::string &subelement);

private:
    bool wfdEnabled;
    int deviceInfo;
    int ctrlPort;
    int maxThroughput;
};

class WifiP2pDevice {
public:
    WifiP2pDevice()
        : deviceName(""),
          mDeviceAddress(""),
          primaryDeviceType(""),
          secondaryDeviceType(""),
          status(P2pDeviceStatus::PDS_UNAVAILABLE),
          supportWpsConfigMethods(0),
          deviceCapabilitys(0),
          groupCapabilitys(0)
    {}
    ~WifiP2pDevice()
    {}
    void SetDeviceName(const std::string &setDeviceName);
    const std::string &GetDeviceName() const;
    void SetDeviceAddress(const std::string &deviceAddress);
    const std::string &GetDeviceAddress() const;
    void SetPrimaryDeviceType(const std::string &setPrimaryDeviceType);
    const std::string &GetPrimaryDeviceType() const;
    void SetSecondaryDeviceType(const std::string &deviceType);
    const std::string &GetSecondaryDeviceType() const;
    void SetP2pDeviceStatus(P2pDeviceStatus setStatus);
    P2pDeviceStatus GetP2pDeviceStatus() const;
    void SetWfdInfo(const WifiP2pWfdInfo &info);
    const WifiP2pWfdInfo &GetWfdInfo() const;
    void SetWpsConfigMethod(unsigned int wpsConfigMethod);
    unsigned int GetWpsConfigMethod() const;
    void SetDeviceCapabilitys(int capabilitys);
    int GetDeviceCapabilitys() const;
    void SetGroupCapabilitys(int capabilitys);
    int GetGroupCapabilitys() const;
    bool IsGroupOwner() const;
    bool IsGroupLimit() const;
    bool IsDeviceLimit() const;
    bool Isinviteable() const;
    bool IsValid() const;
    bool operator==(const WifiP2pDevice &cmp) const;
    bool operator!=(const WifiP2pDevice &cmp) const;
    bool WpsPbcSupported() const;
    bool WpsDisplaySupported() const;
    bool WpKeypadSupported() const;
    bool isGroupOwner() const;

private:
    std::string deviceName; /* the value range is 0 to 32 characters. */
    std::string mDeviceAddress; /* the device MAC address, the length is 17 characters. */
    std::string primaryDeviceType;
    std::string secondaryDeviceType;
    P2pDeviceStatus status;
    WifiP2pWfdInfo wfdInfo;
    unsigned int supportWpsConfigMethods;
    int deviceCapabilitys;
    int groupCapabilitys;
};

const int TEMPORARY_NET_ID = -1;
const int PERSISTENT_NET_ID = -2;
const int INVALID_NET_ID = -999;
class WifiP2pGroupInfo {
public:
    WifiP2pGroupInfo()
        : isP2pGroupOwner(false),
          networkId(INVALID_NET_ID),
          frequency(0),
          isP2pPersistent(0),
          groupStatus(P2pGroupStatus::GS_INVALID)
    {}
    ~WifiP2pGroupInfo()
    {}
    bool operator==(const WifiP2pGroupInfo &group) const;
    bool operator!=(const WifiP2pGroupInfo &group) const;
    void SetIsGroupOwner(bool isGroupOwner);
    bool IsGroupOwner() const;
    void SetOwner(const WifiP2pDevice &setOwner);
    const WifiP2pDevice &GetOwner() const;
    void SetPassphrase(const std::string &setPassphrase);
    const std::string &GetPassphrase() const;
    void SetInterface(const std::string &setInterface);
    const std::string &GetInterface() const;
    void SetGroupName(const std::string &newGroupName);
    const std::string &GetGroupName() const;
    void SetFrequency(int setFrequency);
    int GetFrequency() const;
    void SetIsPersistent(bool isPersistent);
    bool IsPersistent() const;
    void SetP2pGroupStatus(P2pGroupStatus newGroupStatus);
    P2pGroupStatus GetP2pGroupStatus() const;
    void SetNetworkId(int nwId);
    const int &GetNetworkId() const;
    void SetGoIpAddress(const std::string &ipAddr);
    const std::string &GetGoIpAddress() const;
    void AddClientDevice(const WifiP2pDevice &clientDevice);
    void RemoveClientDevice(const WifiP2pDevice &clientDevice);
    bool IsContainsDevice(const WifiP2pDevice &clientDevice) const;
    bool IsClientDevicesEmpty() const;
    const std::vector<WifiP2pDevice> &GetClientDevices() const;
    void SetClientDevices(const std::vector<WifiP2pDevice> &devices);
    void ClearClientDevices();

private:
    WifiP2pDevice owner;
    bool isP2pGroupOwner;
    std::string passphrase; /* the value ranges from 8 to 63. */
    std::string interface;
    std::string groupName;
    int networkId;
    int frequency; /* for example : freq=2412 to select 2.4 GHz channel 1.(Based on 2.4 GHz or 5 GHz) */
    bool isP2pPersistent;
    P2pGroupStatus groupStatus;
    std::vector<WifiP2pDevice> clientDevices;
    std::string goIpAddress;
};

class WpsInfo {
public:
    WpsInfo() : mWpsMethod(WpsMethod::WPS_METHOD_INVALID), bssid(""), pin("")
    {}
    ~WpsInfo()
    {}
    void SetWpsMethod(WpsMethod wpsMethod);
    WpsMethod GetWpsMethod() const;

    void SetBssid(const std::string &setBssid);
    const std::string &GetBssid() const;
    void SetPin(const std::string &setPin);
    const std::string &GetPin() const;

private:
    WpsMethod mWpsMethod;
    std::string bssid; /* the length is 17 characters. */
    std::string pin; /* the length is 4 or 8 characters. */
};

const int AUTO_GROUP_OWNER_VALUE = -1;
const int MIN_GROUP_OWNER_VALUE = 0;
const int MAX_GROUP_OWNER_VALUE = 15;
class WifiP2pConfig {
public:
    WifiP2pConfig()
        : mDeviceAddress(""),
          goBand(GroupOwnerBand::GO_BAND_AUTO),
          netId(-1),
          passphrase(""),
          groupOwnerIntent(AUTO_GROUP_OWNER_VALUE),
          groupName("")
    {}
    WifiP2pConfig(const WifiP2pConfig &config)
        : mDeviceAddress(config.GetDeviceAddress()),
          goBand(config.GetGoBand()),
          netId(config.GetNetId()),
          passphrase(config.GetPassphrase()),
          groupOwnerIntent(config.GetGroupOwnerIntent()),
          groupName(config.GetGroupName())
    {}
    ~WifiP2pConfig()
    {}
    void SetDeviceAddress(const std::string &deviceAddress);
    const std::string &GetDeviceAddress() const;
    void SetGoBand(GroupOwnerBand setGoBand);
    GroupOwnerBand GetGoBand() const;
    void SetNetId(int setNetId);
    int GetNetId() const;
    void SetPassphrase(const std::string &newPassphrase);
    const std::string &GetPassphrase() const;
    void SetGroupOwnerIntent(int intent);
    int GetGroupOwnerIntent() const;
    void SetGroupName(const std::string &setGroupName);
    const std::string &GetGroupName() const;

private:
    std::string mDeviceAddress; /* the device MAC address, the length is 17 characters. */
    GroupOwnerBand goBand;
    int netId; /* network id, when -2 means persistent and -1 means temporary, else need >= 0 */
    std::string passphrase; /* the value ranges from 8 to 63. */
    int groupOwnerIntent; /* the value is -1.(A value of -1 indicates the system can choose an appropriate value.) */
    std::string groupName; /* the value ranges from 1 to 32. */
};

class WifiP2pConfigInternal : public WifiP2pConfig {
public:
    WifiP2pConfigInternal(): WifiP2pConfig()
    {
        wpsInfo.SetWpsMethod(WpsMethod::WPS_METHOD_INVALID);
    }
    WifiP2pConfigInternal(WifiP2pConfig config): WifiP2pConfig(config)
    {
        wpsInfo.SetWpsMethod(WpsMethod::WPS_METHOD_INVALID);
    }
    ~WifiP2pConfigInternal()
    {}
    inline void SetWpsInfo(const WpsInfo &info)
    {
        wpsInfo = info;
    }
    inline const WpsInfo &GetWpsInfo() const
    {
        return wpsInfo;
    }

private:
    WpsInfo wpsInfo;
};

class WifiP2pLinkedInfo {
public:
    WifiP2pLinkedInfo() : connectState(P2pConnectedState::P2P_DISCONNECTED), isP2pGroupOwner(false)
    {}
    ~WifiP2pLinkedInfo()
    {}
    void SetConnectState(P2pConnectedState setConnectState);
    P2pConnectedState GetConnectState() const;
    void SetIsGroupOwner(bool isGroupOwner);
    const bool &IsGroupOwner() const;
    void SetIsGroupOwnerAddress(const std::string &setGroupOwnerAddress);
    const std::string &GetGroupOwnerAddress() const;

private:
    P2pConnectedState connectState;
    bool isP2pGroupOwner;
    std::string groupOwnerAddress; /* the length is 17 characters. */
};

const int SERVICE_TLV_LENGTH_SIZE = 2;
const int PROTOCOL_SIZE = 1;
const int TRANSACTION_ID_SIZE = 1;
const int SERVICE_STATUS_SIZE = 1;

class WifiP2pServiceRequest {
public:
    WifiP2pServiceRequest() : mProtocolType(P2pServicerProtocolType::SERVICE_TYPE_ALL), mTransactionId(0)
    {}
    WifiP2pServiceRequest(P2pServicerProtocolType protocolType, const std::string &data)
        : mProtocolType(protocolType),
          mTransactionId(0)
    {
        for (unsigned long i = 0; i < data.length(); ++i) {
            mQuery.push_back(data.at(i));
        }
    }
    ~WifiP2pServiceRequest()
    {}
    void SetProtocolType(P2pServicerProtocolType serviceProtocolType);
    P2pServicerProtocolType GetProtocolType() const;
    void SetTransactionId(unsigned char transactionId);
    int GetTransactionId() const;
    void SetQuery(const std::vector<unsigned char> &query);
    const std::vector<unsigned char> &GetQuery() const;

    std::vector<unsigned char> GetTlv() const;

    bool operator==(const WifiP2pServiceRequest &cmp) const;

private:
    P2pServicerProtocolType mProtocolType;
    unsigned char mTransactionId;
    std::vector<unsigned char> mQuery;
};

class WifiP2pServiceResponse {
public:
    WifiP2pServiceResponse()
        : mProtocolType(P2pServicerProtocolType::SERVICE_TYPE_ALL),
          mTransactionId(0),
          mServiceStatus(P2pServiceStatus::PSRS_BAD_REQUEST)
    {}
    WifiP2pServiceResponse(P2pServicerProtocolType protocolType, P2pServiceStatus serviceStatus,
        unsigned char transactionId, const std::vector<unsigned char> data)
        : mProtocolType(protocolType), mTransactionId(transactionId), mServiceStatus(serviceStatus), responseData(data)
    {}
    ~WifiP2pServiceResponse()
    {}
    void SetProtocolType(P2pServicerProtocolType serviceProtocolType);
    P2pServicerProtocolType GetProtocolType() const;
    void SetTransactionId(unsigned char transactionId);
    unsigned char GetTransactionId() const;
    void SetServiceStatus(P2pServiceStatus serviceStatus);
    P2pServiceStatus GetServiceStatus() const;
    void SetServiceName(const std::string &name);
    const std::string &GetServiceName() const;
    void SetData(const std::vector<unsigned char> &data);
    const std::vector<unsigned char> &GetData() const;
    std::vector<unsigned char> GetTlv() const;
    bool operator==(const WifiP2pServiceResponse &cmp) const;

protected:
    P2pServicerProtocolType mProtocolType;
    unsigned char mTransactionId;
    P2pServiceStatus mServiceStatus;
    std::string mSvrName;
    std::vector<unsigned char> responseData;
};

class WifiP2pServiceInfo {
public:
    WifiP2pServiceInfo()
        : mDeviceAddress("00:00:00:00:00:00"), mProtocolType(P2pServicerProtocolType::SERVICE_TYPE_VENDOR_SPECIFIC)
    {}
    explicit WifiP2pServiceInfo(std::vector<std::string> queryList) : mQueryList(queryList)
    {}
    ~WifiP2pServiceInfo()
    {}
    void SetServiceName(const std::string &name);
    const std::string &GetServiceName() const;
    void SetDeviceAddress(const std::string &deviceAddress);
    const std::string &GetDeviceAddress() const;
    void SetServicerProtocolType(P2pServicerProtocolType type);
    P2pServicerProtocolType GetServicerProtocolType() const;
    void SetQueryList(const std::vector<std::string> &queryList);
    const std::vector<std::string> &GetQueryList() const;
    bool operator==(const WifiP2pServiceInfo &cmp) const;
    /**
     * @Description - Pack all data into a P2P service request packet based on the data interface.
     * @return - WifiP2pServiceRequest
     */
    WifiP2pServiceRequest BuildRequest();
    P2pServiceStatus ProcessServiceRequest(
        const std::vector<unsigned char> &Query, std::vector<unsigned char> &data) const;
    void ProcessServiceResponse(const std::vector<unsigned char> &data) const;
    static std::string Bin2HexStr(std::vector<unsigned char> data);
    static std::string Bin2HexStr(std::string data);

private:
    std::string serviceName;
    std::string mDeviceAddress;
    P2pServicerProtocolType mProtocolType;
    std::vector<std::string> mQueryList;
};

class P2pVendorConfig {
public:
    P2pVendorConfig() : randomMacSupport(false), isAutoListen(false), primaryDeviceType("10-0050F204-5")
    {}
    ~P2pVendorConfig()
    {}
    bool GetRandomMacSupport() const;
    void SetRandomMacSupport(bool support);
    bool GetIsAutoListen() const;
    void SetIsAutoListen(bool autoListen);
    const std::string &GetDeviceName() const;
    void SetDeviceName(const std::string &name);
    const std::string &GetPrimaryDeviceType() const;
    void SetPrimaryDeviceType(const std::string &setPrimaryDeviceType);
    const std::string &GetSecondaryDeviceType() const;
    void SetSecondaryDeviceType(const std::string &setSecondaryDeviceType);

private:
    bool randomMacSupport;
    bool isAutoListen;
    std::string deviceName;
    std::string primaryDeviceType;
    std::string secondaryDeviceType;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
