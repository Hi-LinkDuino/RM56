/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup ��������
 * @{
 *
 * @brief ��������ģ���ṩʹ����������Ļ������ܽӿڡ�
 *
 * ͨ����ģ�����ʵ����������Ĵ򿪡��رգ��ṩ���õ���������\n
 * ���磺BT-GAP��BLE��BLE-GATT��BT-������HFP��A2DP��AVRCP��MAP��PBAP��\n
 *
 * @since 3
 */

/**
 * @file ohos_bt_gatt.h
 *
 * @brief Gatt��صĹ��ܽӿڶ��塣
 *
 * ����Gatt�õ��Ļ������ݽṹ������ӿڣ���㲥/ɨ��ȡ�\n
 *
 * @since 3
 */


#ifndef OHOS_BT_GATT_H
#define OHOS_BT_GATT_H

#include "ohos_bt_def.h"

/**
 * @brief �㲥���˲���
 *
 * ���ù㲥������ɨ�跽��ɨ���������������ʱ�Ƿ�ͨ�����������ˡ�\n
 *
 * @since 3
 */
typedef enum {
    /** �������������豸��ɨ����������� */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = 0x00,
    /** �������������豸���������󣬽������������ɨ������ */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = 0x01,
    /** �������������豸��ɨ�����󣬽�������������������� */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = 0x02,
    /** �������������ɨ��������������� */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = 0x03,
} BleAdvFilter;

/**
 * @brief �㲥����
 *
 * ���ù㲥�����͹㲥���ݵ����͡�\n
 *
 * @since 3
 */
typedef enum {
    /** �����ӿ�ɨ��Ƕ���㲥(Ĭ��) */
    OHOS_BLE_ADV_IND = 0x00,
    /** �����Ӹ�ռ�ձȶ���㲥 */
    OHOS_BLE_ADV_DIRECT_IND_HIGH = 0x01,
    /** ��ɨ��Ƕ���㲥 */
    OHOS_BLE_ADV_SCAN_IND = 0x02,
    /** �������ӷǶ���㲥 */
    OHOS_BLE_ADV_NONCONN_IND = 0x03,
    /** �����ӵ�ռ�ձȶ���㲥 */
    OHOS_BLE_ADV_DIRECT_IND_LOW  = 0x04,
} BleAdvType;

/**
 * @brief IO����ģʽ
 *
 * @since 3
 */
typedef enum {
    /** DisplayOnly, ֻ�������ʾ������ */
    OHOS_BLE_IO_CAP_OUT = 0x00,
    /** DisplayYesNo, ֻ������YES��NO������������ʾ���� */
    OHOS_BLE_IO_CAP_IO,
    /** KeyboardOnly, ��������0-9��ȷ�ϼ���YES/NO��������û����ʾ������ */
    OHOS_BLE_IO_CAP_IN,
    /** NoInputNoOutput, û�������������� */
    OHOS_BLE_IO_CAP_NONE,
    /** KeyboardDisplay, ��������0-9��ȷ�ϼ���YES/NO������������ʾ������ */
    OHOS_BLE_IO_CAP_KBDISP
} BleIoCapMode;

/**
 * @brief ��Ȩ����ģʽ
 *
 * @since 3
 */
typedef enum {
    /** no-boding */
    OHOS_BLE_AUTH_NO_BOND = 0x00,
    /** boding */
    OHOS_BLE_AUTH_BOND,
    /** ��MITM */
    OHOS_BLE_AUTH_REQ_MITM,
    /** ����ȫ���� */
    OHOS_BLE_AUTH_REQ_SC_ONLY,
    /** ��ȫ���Ӻ�bonding */
    OHOS_BLE_AUTH_REQ_SC_BOND,
    /** ��ȫ���Ӻ�MITM */
    OHOS_BLE_AUTH_REQ_SC_MITM,
    /** ��ȫ���Ӻ�MITM���Լ�bonding */
    OHOS_BLE_AUTH_REQ_SC_MITM_BOND
} BleAuthReqMode;

/**
 * @brief BLEɨ������
 *
 * @since 3
 */
typedef enum {
    /** ����ɨ�裬���ᷢ��ɨ������ */
    OHOS_BLE_SCAN_TYPE_PASSIVE = 0x00,
    /** ����ɨ�裬���ܻᷢ��ɨ������ */
    OHOS_BLE_SCAN_TYPE_ACTIVE,
} BleScanType;

/**
 * @brief BLEɨ��оƬ���˲���
 *
 * @since 3
 */
typedef enum {
    /** �������й㲥�����Բ��Ƿ������豸�Ķ���㲥(Ĭ��) */
    OHOS_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL = 0x00,
    /** ֻ���ܰ��������豸�Ĺ㲥�����Բ��Ƿ������豸�Ķ���㲥 */
    OHOS_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST,
    /** �������еķǶ���㲥����ַ�ǿɽ���˽�е�ַ�Ĺ㲥�����͵Ķ���㲥�Լ��������豸�Ķ���㲥 */
    OHOS_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL_AND_RPA,
    /** ���ܰ������е����зǶ���㲥����ַ�ǿɽ���˽�е�ַ�Ĺ㲥�����͵Ķ���㲥�Լ��������豸�Ķ���㲥 */
    OHOS_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST_AND_RPA
} BleScanFilterPolicy;

/**
 * @brief BLEɨ�����㲥����
 *
 * @since 3
 */
typedef enum {
    /** ��չ�Ĳ������Ӳ���ɨ��Ƕ���㲥 */
    OHOS_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE = 0x00,
    /** ��չ�Ĳ������Ӳ���ɨ�趨��㲥 */
    OHOS_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED = 0x04,
    /** ��չ�Ŀ����ӷǶ���㲥 */
    OHOS_BLE_EVT_CONNECTABLE = 0x01,
    /** ��չ�Ŀ����Ӷ���㲥 */
    OHOS_BLE_EVT_CONNECTABLE_DIRECTED = 0x05,
    /** ��չ�Ŀ�ɨ��Ƕ���㲥 */
    OHOS_BLE_EVT_SCANNABLE = 0x02,
    /** ��չ�Ŀ�ɨ�趨��㲥 */
    OHOS_BLE_EVT_SCANNABLE_DIRECTED = 0x06,

    /** ��ͳ�Ĳ������ӷǶ���㲥 */
    OHOS_BLE_EVT_LEGACY_NON_CONNECTABLE = 0x10,
    /** ��ͳ�Ŀ�ɨ��Ƕ���㲥 */
    OHOS_BLE_EVT_LEGACY_SCANNABLE = 0x12,
    /** ��ͳ�Ŀ����ӿ�ɨ��Ƕ���㲥 */
    OHOS_BLE_EVT_LEGACY_CONNECTABLE = 0x13,
    /** ��ͳ�Ŀ����Ӷ���㲥 */
    OHOS_BLE_EVT_LEGACY_CONNECTABLE_DIRECTED = 0x15,
    /** ��ͳ����ADV_SCAN_IND��Ӧ��ɨ����Ӧ */
    OHOS_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN = 0x1A,
    /** ��ͳ����ADV_IND��Ӧ��ɨ����Ӧ */
    OHOS_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV = 0x1B
} BleScanResultEvtType;

/**
 * @brief BLEɨ��������������
 *
 * @since 3
 */
typedef enum {
    /** �������ݻ����һ��Ƭ�� */
    OHOS_BLE_DATA_COMPLETE = 0x00,
    /** �����������ݣ��������ݼ������� */
    OHOS_BLE_DATA_INCOMPLETE_MORE_TO_COME = 0x01,
    /** ���ضϲ����������ݣ�û�и���������� */
    OHOS_BLE_DATA_INCOMPLETE_TRUNCATED = 0x02,
} BleScanResultDataStatus;

/**
 * @brief BLEɨ������ַ����
 *
 * @since 3
 */
typedef enum {
    /** �����豸��ַ */
    OHOS_BLE_PUBLIC_DEVICE_ADDRESS = 0x00,
    /** ����豸��ַ */
    OHOS_BLE_RANDOM_DEVICE_ADDRESS = 0x01,
    /** Public Identity Address */
    OHOS_BLE_PUBLIC_IDENTITY_ADDRESS = 0x02,
    /** Random (static) Identity Address */
    OHOS_BLE_RANDOM_STATIC_IDENTITY_ADDRESS = 0x03,
    /** ���ɽ�������豸��ַ */
    OHOS_BLE_UNRESOLVABLE_RANDOM_DEVICE_ADDRESS = 0xFE,
    /** �޵�ַ(�����㲥) */
    OHOS_BLE_NO_ADDRESS = 0xFF,
} BleScanResultAddrType;

/**
 * @brief BLEɨ����PHY����
 *
 * @since 3
 */
typedef enum {
    /** �޹㲥�� */
    OHOS_BLE_SCAN_PHY_NO_PACKET = 0x00,
    /** 1M PHY */
    OHOS_BLE_SCAN_PHY_1M = 0x01,
    /** 2M PHY */
    OHOS_BLE_SCAN_PHY_2M = 0x02,
    /** Coded PHY */
    OHOS_BLE_SCAN_PHY_CODED = 0x03
} BleScanResultPhyType;

/**
 * @brief ����BLE�㲥/ɨ����Ӧ����
 *
 * @since 3
 */
typedef struct {
    /** �㲥���ݳ��� */
    unsigned short advLength;
    /** �㲥���� */
    char *advData;
    /** ɨ����Ӧ���ݳ��� */
    unsigned short scanRspLength;
    /** ɨ����Ӧ���� */
    char *scanRspData;
} BleConfigAdvData;

/**
 * @brief ����BLE�㲥����
 *
 * @since 3
 */
typedef struct {
    /** ��С�Ĺ㲥���. [N * 0.625ms] */
    int minInterval;
    /** ���Ĺ㲥���. [N * 0.625ms] */
    int maxInterval;
    /** �㲥���� */
    BleAdvType advType;
    /** ���˵�ַ���� */
    unsigned char ownAddrType;
    /** �Զ˵�ַ���� */
    unsigned char peerAddrType;
    /** �Զ˵�ַ */
    BdAddr peerAddr;
    /** 37/38/39�����㲥ͨ��ѡ��bitMap�����磺0x01-37ͨ��ʹ��, 0x07-37/38/39����ͨ����ʹ�� */
    int channelMap;
    /** ���������˲��� */
    BleAdvFilter advFilterPolicy;
    /** ���͹��ʣ���λdbm */
    int txPower;
    /** �㲥��������ʱ����[N * 10 ms] */
    int duration;
} BleAdvParams;

/**
 * @brief ����BLEɨ�����
 *
 * @since 3
 */
typedef struct {
    /** ɨ������[N * 0.625 ms] */
    unsigned short scanInterval;
    /** ɨ�贰����[N * 0.625 ms] */
    unsigned short scanWindow;
    /** ɨ�����ͣ��ο�{@link BleScanType} */
    unsigned char scanType;
    /** PHY���ͣ�0x01��ʾ1M phy�� 0x01��ʾ2M phy�� 0x02��ʾLE Coded phy */
    unsigned char scanPhy;
    /** ɨ����˲��ԣ��ο�{@link BleScanFilterPolicy} */
    unsigned char scanFilterPolicy;
} BleScanParams;

/**
 * @brief ���ù㲥/ɨ����Ӧ����
 *
 * ���ṩ��ϵͳӦ��ʹ�ã���������Ӧ�ÿ��š�\n
 *
 * @since 3
 */
typedef struct {
    /** �㲥���� */
    unsigned char *advData;
    /** �㲥���ݳ��� */
    unsigned int advDataLen;
    /** ɨ����Ӧ���� */
    unsigned char *rspData;
    /** ɨ����Ӧ���ݳ��� */
    unsigned int rspDataLen;
} StartAdvRawData;

/**
 * @brief ����ɨ��������
 *
 * @since 3
 */
typedef struct {
    /** �㲥���ͣ��ο�{@link BleScanResultEvtType} */
    unsigned char eventType;
    /** ɨ��������״̬���ο�{@link BleScanResultDataStatus} */
    unsigned char dataStatus;
    /** ��ַ���ͣ��ο�{@link BleScanResultAddrType}��0xFE����ѡ */
    unsigned char addrType;
    /** ��ַ */
    BdAddr addr;
    /** ���㲥PHY���ͣ��ο�{@link BleScanResultPhyType}��ֻ��ȡֵ0x01��0x03 */
    unsigned char primaryPhy;
    /** ���㲥PHY���ͣ��ο�{@link BleScanResultPhyType}����ȡȫ��ö��ֵ */
    unsigned char secondaryPhy;
    /** Value of the Advertising SID subfield in the ADI field of the PDU or, for scan responses,
       in the ADI field of the original scannable advertisement, 0xFF->No ADI field provided */
    unsigned char advSid;
    /** ���͹��ʣ���Χ: -127 ~ +20dBm, ����Ϊ0x7F��ʾ�ֶ���Ч */
    char txPower;
    /** rssi����Χ: -127 ~ +20dBm, ����Ϊ0x7F��ʾ�ֶ���Ч */
    char rssi;
    /** ���ڹ㲥�����[N * 1.25 ms]������Ϊ0x00��ʾ�ֶ���Ч */
    unsigned short periodicAdvInterval;
    /** ����㲥��ַ���ͣ��ο�{@link BleScanResultAddrType}��0xFF����ȡ */
    unsigned char directAddrType;
    /** ����㲥��ַ������㲥����Ч */
    BdAddr directAddr;
    /** �㲥���ݳ��� */
    unsigned char advLen;
    /** �㲥���� */
    unsigned char *advData;
} BtScanResultData;

/**
 * @brief �����㲥��Ļص����ο�{@link BleStartAdv}
 *
 * @since 3
 */
typedef void (*AdvEnableCallback)(int advId, int status);

/**
 * @brief ֹͣ�㲥��Ļص����ο�{@link BleStopAdv}
 *
 * @since 3
 */
typedef void (*AdvDisableCallback)(int advId, int status);

/**
 * @brief ���ù㲥���ݺ�Ļص����ο�{@link BleSetAdvData}
 *
 * @since 3
 */
typedef void (*AdvDataCallback)(int advId, int status);

/**
 * @brief ���¹㲥������Ļص����ο�{@link BleUpdateAdv}
 *
 * @since 3
 */
typedef void (*AdvUpdateCallback)(int advId, int status);

/**
 * @brief �յ���ȫ�������󣬵���{@link BleGattSecurityRsp}���谲ȫ�������Ȩ��
 *
 * @since 3
 */
typedef void (*SecurityRespondCallback)(const BdAddr *bdAddr);

/**
 * @brief �յ�ɨ�����ص����ϲ�
 *
 * @since 3
 */
typedef void (*ScanResultCallback)(BtScanResultData *scanResultdata);

/**
 * @brief ɨ�����������ɺ�ص�
 *
 * @since 3
 */
typedef void (*ScanParameterSetCompletedCallback)(int clientId, int status);

/**
 * @brief Gatt�ص������ӿڶ���
 *
 * @since 3
 */
typedef struct {
    /** �����㲥��� */
    AdvEnableCallback advEnableCb;
    /** ֹͣ�㲥��� */
    AdvDisableCallback advDisableCb;
    /** ���ù㲥������� */
    AdvDataCallback advDataCb;
    /** ���¹㲥������� */
    AdvUpdateCallback advUpdateCb;
    /** �յ���ȫ�������� */
    SecurityRespondCallback securityRespondCb;
    /** �յ�ɨ���� */
    ScanResultCallback scanResultCb;
    /** ɨ������������ */
    ScanParameterSetCompletedCallback scanParamSetCb;
} BtGattCallbacks;

/**
 * @brief ��ʼ������Э��ջ��
 *
 * @param void
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int InitBtStack(void);

/**
 * @brief ʹ������Э��ջ��
 *
 * @param void
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int EnableBtStack(void);

/**
 * @brief ȥʹ������Э��ջ��
 *
 * @param void
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int DisableBtStack(void);

/**
 * @brief ���������豸���ơ�
 *
 * @param name ��������
 * @param len �����������ݳ���
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int SetDeviceName(const char *name, unsigned int len);

/**
 * @brief ���ù㲥���ݡ�
 *
 * @param advId �㲥ID���ɹ㲥���ϲ����
 * @param data �㲥���ݣ��ο�{@link BleConfigAdvData}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleSetAdvData(int advId, const BleConfigAdvData *data);

/**
 * @brief ��ʼ���͹㲥��
 *
 * @param advId �㲥ID���ɹ㲥���ϲ����
 * @param param �㲥�������ο�{@link BleAdvParams}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleStartAdv(int advId, const BleAdvParams *param);

/**
 * @brief ֹͣ���͹㲥��
 *
 * @param advId �㲥ID���ɷ��͹㲥ʱ�㲥���ϲ����
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleStopAdv(int advId);

/**
 * @brief ���¹㲥������
 *
 * @param advId �㲥ID���ɷ��͹㲥ʱ�㲥���ϲ����
 * @param param �㲥�������ο�{@link BleAdvParams}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleUpdateAdv(int advId, const BleConfigAdvData *data, const BleAdvParams *param);

/**
 * @brief ���ð�ȫIO������
 *
 * @param mode ����ģʽ���ο�{@link BleIoCapMode}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleSetSecurityIoCap(BleIoCapMode mode);

/**
 * @brief ���ð�ȫ���Ӽ�Ȩ����ģʽ��
 *
 * @param mode ��Ȩģʽ���ο�{@link BleAuthReqMode}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleSetSecurityAuthReq(BleAuthReqMode mode);

/**
 * @brief ��Ӧ��ȫ��������
 *
 * @param bdAddr �豸��ַ
 * @param accept true��ʾ��������false��ʾ�ܾ�����
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleGattSecurityRsp(BdAddr bdAddr, bool accept);

/**
 * @brief ��ȡ�豸MAC��ַ��
 *
 * @param mac ���Σ��豸MAC��ַ
 * @param len ��ַ����
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int ReadBtMacAddr(unsigned char *mac, unsigned int len);

/**
 * @brief ����ɨ�������
 *
 * @param clientId client�ӿ�ID��clientע��ʱ��ȡ
 * @param param ɨ��������ο�{@link BleScanParams}
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleSetScanParameters(int clientId, BleScanParams *param);

/**
 * @brief ����ɨ�衣
 *
 * @param void
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleStartScan(void);

/**
 * @brief ֹͣɨ�衣
 *
 * @param void
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleStopScan(void);

/**
 * @brief ע��Gatt�ص�������
 *
 * @param func �ص�����ԭ�ͣ��ο�{@link BtGattCallbacks}��
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleGattRegisterCallbacks(BtGattCallbacks *func);

/**
 * @brief �����㲥��չ�ӿڡ�
 *
 * ��ɹ㲥��������/�㲥��������/�����㲥��\n
 * ���ṩ��ϵͳӦ��ʹ�ã���������Ӧ�ÿ��š�\n
 *
 * @param advId �㲥ID
 * @param rawData �㲥���ݣ��ο�{@link StartAdvRawData}��
 * @param advParam �㲥�������ο�{@link BleAdvParams}��
 * @return �ɹ�����{@link OHOS_BT_STATUS_SUCCESS}��ʧ�ܷ��ش����룬��������뺬��ο�{@link BtStatus}����
 * @since 3
 */
int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam);
#endif
/** @} */
