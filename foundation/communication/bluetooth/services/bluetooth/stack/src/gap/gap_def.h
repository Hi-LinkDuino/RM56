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
#ifndef GAP_DEF_H
#define GAP_DEF_H

#include "gap.h"
#include "gap_le.h"

#include "alarm.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GAP_BREDR_SUPPORT
#define GAP_LE_SUPPORT

#define BT_ADDR_FMT "%02X:%02X:**:**:**:%02X"
#define BT_ADDR_FMT_OUTPUT(addr) (addr)[5], (addr)[4], (addr)[0]

#define BT_ADDR_NULL \
    {                \
        {0}, 0       \
    }

#define BT_COD_SIZE 3
#define COD_UINT_TO_ARRAY(cod)                                      \
    {                                                               \
        ((cod)&0xff), (((cod) >> 8) & 0xff), (((cod) >> 16) & 0xff) \
    }
#define COD_ARRAY_TO_UINT(cod) ((uint32_t)(cod)[0] | (uint32_t)((cod)[1] << 8) | (uint32_t)((cod)[2] << 16))

#define LAP_GENERAL_INQUIRY_ACCESS 0x9E8B33
#define LAP_LIMITED_INQUIRY_ACCESS 0x9E8B00

#define NO_SCANS_ENABLED 0
#define INQUIRY_SCAN_ENABLED_PAGE_SCAN_DISABLED 1
#define INQUIRY_SCAN_DISABLED_PAGE_SCAN_ENABLED 2
#define INQUIRY_SCAN_ENABLED_PAGE_SCAN_ENABLED 3

#define CURRENT_IAC_MAX_NUM 0x02
#define EXTENDED_SCAN_PHY_MAX_NUM 3

#define COD_LIMITED_DISCOVERABLE_BIT (1 << 13)

#define GAP_EIR_FEC_REQUIRED 0x00

#define GAP_INVALID_ACL_HANDLE 0xFFFF

#define GAP_HCI_DISC_REASON_AUTH_FAIL 0x05

#define GAP_DISC_ACL_WAIT_TIME 4000
#define GAP_USE_ACL_CONNECTION_TIME 30000
#define GAP_SEC_WAIT_ENCRYPT_TIME 2000
#define GAP_WAIT_EX_ADV_TERMINATED 1500

#define GAP_LINK_KEY_TYPE_UNKNOWN 0xFF
#define AUTHENTICATION_UNKNOWN_MITM 0xFF

#define AUTH_REQ_DEFAULT (SMP_AUTH_REQ_BONDING | SMP_AUTH_REQ_BIT_MITM | SMP_AUTH_REQ_BIT_SC)

#define LE_KEY_TYPE_UNAUTHENTICATION SMP_AUTH_FLAG_NO
#define LE_KEY_TYPE_AUTHENTICATION SMP_AUTH_FLAG_YES
#define LE_KEY_TYPE_UNKNOWN 0xFF

#ifdef GAP_BREDR_SUPPORT

#define INQUIRY_RESULT_SAVE_MAX 40

#define UNSUPPORT_SECURE_CONNECTIONS 0x00
#define SUPPORT_SECURE_CONNECTIONS 0x01

#define GAP_PAIR_RETRY_COUNT 5
#define GAP_PAIR_RETRY_WAIT_TIME 500

enum ScanModeStatus {
    GAP_SCANMODE_STATUS_IDLE,
    GAP_SCANMODE_STATUS_CLOSING,
    GAP_SCANMODE_STATUS_SETTING,
};

enum InquiryStatus {
    GAP_INQUIRY_STATUS_IDLE,
    GAP_INQUIRY_STATUS_START,
    GAP_INQUIRY_STATUS_CANCEL,
};

enum RemoteNameStatus {
    GAP_REMOTE_NAME_STATUS_IDLE,
    GAP_REMOTE_NAME_STATUS_START,
    GAP_REMOTE_NAME_STATUS_CANCEL,
};

enum DeviceSecurityStatus {
    GAP_DEV_SEC_STATUS_IDLE,
    GAP_DEV_SEC_STATUS_ACTION,
    GAP_DEV_SEC_STATUS_WAIT_DISC,
};

enum DeviceAuthenticationStatus {
    GAP_AUTH_STATUS_IDLE,
    GAP_AUTH_STATUS_ACTION,
    GAP_AUTH_STATUS_WAIT_RETRY,
    GAP_AUTH_STATUS_RETRY,
};

enum DeviceEncryptionStatus {
    GAP_ENC_STATUS_IDLE,
    GAP_ENC_STATUS_ACTION,
};

enum DeviceSecurityEvent {
    GAP_SEC_EVENT_CONNECT_COMP,
    GAP_SEC_EVENT_FEATURE_COMP,
    GAP_SEC_EVENT_UNAUTH_SUCCESS,
    GAP_SEC_EVENT_AUTH_SUCCESS,
    GAP_SEC_EVENT_AUTH_FAILED,
    GAP_SEC_EVENT_ENC_SUCCESS,
    GAP_SEC_EVENT_ENC_FAILED,
    GAP_SEC_EVENT_WAIT_ENC_TIMEOUT,
    GAP_SEC_EVENT_ACL_DISCONNECT,
};

enum SecurityRequestStatus {
    GAP_SEC_REQ_STATUS_WAIT_CONNECT,
    GAP_SEC_REQ_STATUS_WAIT_FEATURE,
    GAP_SEC_REQ_STATUS_WAIT_ENCRYPT,
    GAP_SEC_REQ_STATUS_WAIT_ACTION,
    GAP_SEC_REQ_STATUS_ACTION,
    GAP_SEC_REQ_STATUS_SUCCESS,
    GAP_SEC_REQ_STATUS_FAILED,
};

enum EncryptionRequestStatus {
    GAP_SET_ENCRYPTION_STATUS_IDLE,
    GAP_SET_ENCRYPTION_STATUS_SETTING,
};

enum LESecurityActionStatus {
    GAP_LE_SECURITY_STATUS_IDLE,
    GAP_LE_SECURITY_STATUS_PAIR,
};

enum LEConnParamUpdateStatus {
    GAP_LE_CONN_PARAM_UPDATE_IDLE,
    GAP_LE_CONN_PARAM_UPDATE_RECV_L2CAP,
    GAP_LE_CONN_PARAM_UPDATE_RECV_HCI,
};

typedef struct {
    uint8_t status;
    GapSetScanModeResultCallback callback;
    void *context;
    GapDiscoverModeInfo discoverMode;
    GapConnectableModeInfo connectableMode;
} ScanModeBlock;

typedef struct {
    BtAddr addr;
    uint8_t repetitionMode;
    uint16_t clockOffset;
} InquiryResultInfo;

typedef struct {
    uint8_t status;
} InquiryBlock;

typedef struct {
    uint8_t status;
} RemoteNameBlock;

typedef struct {
    List *registerlist;
    List *requestlist;
} ProfileSecurityBlock;

typedef struct {
    BtAddr addr;
    GapServiceSecurityInfo info;
    uint16_t securityMode;
} ProfileSecurityInfo;

typedef struct {
    List *devicelist;
} ConnectionInfoBlock;

typedef struct {
    BtAddr addr;
    bool needAuthentication;
    bool needUnauthentication;
    bool needEncryption;
    bool needAuthorization;
    bool needMITM;
    GapServiceSecurityInfo info;
    void *context;
    bool doCallback;
    GapSecurityResultCallback callback;
    uint8_t status;
    uint8_t hciStatus;
    uint8_t retryCount;
    Alarm *waitRetryalarm;
} RequestSecInfo;

typedef struct {
    BtAddr addr;
    Alarm *alarm;
    uint16_t handle;
    uint8_t status;
    uint8_t authenticationStatus;
    uint8_t encryptionStatus;
    uint8_t linkkeyType;
    uint8_t localIOCap;
    uint8_t remoteAuthReq;
    bool inDedicatedBonding;
    bool isAuthentication;
    bool isEncryption;
    bool supportSSP;
    RequestSecInfo *actionReq;
    Alarm *aclAlarm;
    Alarm *waitEncryptAlarm;
} DeviceInfo;

typedef struct {
    uint8_t status;
    GapEncryptionChangeCallback callback;
    void *context;
} EncryptionBlock;

typedef struct {
    bool isEnable;
    uint8_t bondableMode;
    uint32_t classOfDevice;
    GAP_SecurityMode secMode;
    GapOOBData oobData192;
    GapOOBData oobData256;
    bool keyMissingRetry;
    ScanModeBlock scanModeBlock;
    InquiryBlock inquiryBlock;
    RemoteNameBlock remoteNameBlock;
    ProfileSecurityBlock profileSecBlock;
    ConnectionInfoBlock connectionInfoBlock;
    EncryptionBlock encryptionBlock;
} GapBredrMng;

#endif

#ifdef GAP_LE_SUPPORT

typedef struct {
    BtAddr addr;
    BtAddr identityAddr;
    uint8_t role;
    uint8_t bondableMode;
    uint8_t minEncKeySize;
    GAP_LeSecMode1Level mode1Level;
    GAP_LeSecMode2Level mode2Level;
    uint8_t IRK[GAP_IRK_SIZE];
} LeLocalInfo;

typedef struct {
    uint8_t result;
    GAP_LeSecurityStatus secReqStatus;
    GapLeRequestSecurityResult callback;
    void *context;
} LeSecurityReq;

typedef struct {
    uint8_t status;
    uint8_t id;
} LeConnParamUpdateReq;

typedef struct {
    BtAddr addr;
    uint16_t handle;
    uint8_t role;
    uint8_t securityStatus;
    uint8_t encryptionStatus;
    uint8_t pairMethod;
    uint8_t keyType;
    uint8_t keySize;
    bool isRemoteSecurityRequest;
    bool isLocalSecurityRequest;
    BtAddr peerAddr;
    BtAddr ownAddr;
    Alarm *alarm;
    bool ownAddrUpdated;
    LeSecurityReq *securityReq;
    LeConnParamUpdateReq *paramUpdateReq;
    bool localSigningExists;
    bool remoteSigningExists;
    LeSignKey localSigningInfo;
    LeSignKey remoteSigningInfo;
} LeDeviceInfo;

typedef struct {
    List *deviceList;
} LeConnectionInfoBlock;

typedef struct {
    bool isPairing;
    BtAddr addr;
} LeBondBlock;

typedef struct {
    List *RequestList;
} LeSignatureBlock;

typedef struct {
    struct {
        bool processing;
        GenResPriAddrResult callback;
        void *context;
    } generationInfo;

    List *reportRPAResolveList;
} LeRandomAddressBlock;

typedef struct {
    uint8_t advHandle;
    uint8_t randomAddress[BT_ADDRESS_SIZE];
} LeExAdvInfo;

typedef struct {
    List *exAdvInfoList;
    uint8_t exAdvMaxNumber;
    uint16_t exAdvDataMaxLen;
} LeExAdvBlock;

typedef struct {
    bool isEnable;
    LeLocalInfo local;
    LeBondBlock bondBlock;
    LeConnectionInfoBlock connectionInfoBlock;
    LeSignatureBlock signatureBlock;
    LeRandomAddressBlock randomAddressBlock;
    LeExAdvBlock exAdvBlock;
} GapLeMng;

#endif

typedef struct {
    int traceLevel;
#ifdef GAP_BREDR_SUPPORT
    GapBredrMng bredr;
#endif
#ifdef GAP_LE_SUPPORT
    GapLeMng le;
#endif
} GapMng;

#ifdef __cplusplus
}
#endif

#endif
