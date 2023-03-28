/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: private interfaces
 */

#ifndef PRIVATE_VENDOR_H
#define PRIVATE_VENDOR_H

#include "ohos_bt_def.h"

typedef struct {
    char *stream;
    unsigned int length;
} PcmStreamInfo;

/*
 * @param: void
 * @return: void
 * function: ���ز�������ʼ����
 */
void StartLocalMediaPlay(void);

/*
 * @param: void
 * @return: void
 * function: ���ز�����ֹͣ����
 */
void StopLocalMediaPlay(void);

/*
 * @param: void
 * @return: void
 * function: �������ڷǲ��ŵ�״̬��֪ͨӦ�ò�������������callback��Ŀ����Ӧ�ò���߱��ز������Ƿ��ܲ���
 */
typedef void (*MediaStreamNotifyCallback)(void);

/*
 * @param1: ע�ắ����ַ
 * @return: int 0 - succee, not 0 - failure
 * function: ע�Ქ����������֪ͨ��callback����
 */
int RegisterMediaStreamNotifyCallback(MediaStreamNotifyCallback func);

/*
 * @param1: ����ַ
 * @param2: ������
 * @return: int 0 - succee, not 0 - failure
 * function: Ӧ�ò���HFP������pcm�������Э��ջ
 */
int AppSendHfpPcmStream(PcmStreamInfo *streamInfo);

/*
* @param1: ��Ҫ���ܵ�raw data
* @param2: 12�ֽڵļ�������
* @param3: raw data + 16�ֽڵ����ռ�������
* @return: int 0 - succee, not 0 - failure
* function: Э��ջ����Ӧ�ò��hks�����㷨����link key
*/
typedef int (*HksEnDecryptLinkKey)(unsigned char *input, const unsigned char* random, unsigned char *output);

/*
* @param1: Ӧ�ò���ܵĺ�����ַ
* @param2: Ӧ�ò���ܵĺ�����ַ
* @return: int 0 - succee, not 0 - failure
* function: Ӧ�ò�ע��ӽ��ܺ�����ַ��Э��ջ
*/
int RegisterEnDecryptCallback(HksEnDecryptLinkKey encryptCb, HksEnDecryptLinkKey decryptCb);
/*
 * ���½ӿ������������ֵĽӿڶ��� begin
 */
typedef enum {
    STEREO_MASTER,
    STEREO_SLAVE,
    STEREO_NONE
} StereoRole;

typedef struct {
    BdAddr addr;
    StereoRole role;
} StereoInfo;

typedef enum {
    WIFI_TO_A2DP = 0,
    WIFI_TO_BIS,
    A2DP_TO_BIS,
    UNKNOWN_STATUS
} StereoPlayMode;

typedef enum {
    LEFT_CHANNEL = 0,
    RIGHT_CHANNEL
} StereoChannelSet;

typedef enum {
    STEREO_SETUP_SUCCESS = 0x00,
    STEREO_SETUP_FAILED,
    STEREO_EXIT_SUCCESS,
    STEREO_EXIT_FAILED,
    STEREO_EXIT_REQUEST,
    STEREO_CONNECTION_TIMEOUT = 0x08,
    STEREO_UNKNOWN_REASON = 0xff,
} StereoEvent;

/*
 * @param: void
 * @return: void
 * function: �齨����������
 */
int SetUpStereo(StereoInfo *data);

/*
 * @param: void
 * @return: void
 * function: �������������
 */
void ExitStereo(void);

/*
 * @param: status - used for future
 * @return: void
 * function: ������ͨ���ж��ϱ���Ӧ�ò�Ľӿ�
 */
typedef void (*StereoStateChangedCallback)(StereoEvent status);

/*
 * @param: StereoStateChangedCallback - ������ͨ���жϵĴ�����
 * @return: void
 * function: ע��������ͨ���жϵĺ�����btЭ��ջ
 */
int RegisterStereoNotifyCallback(StereoStateChangedCallback func);

/*
 * @param1: buf - �������ݵ��׵�ַ
 * @param2: length - �������ݵĳ���
 * @return: int �ɹ�(0)/ʧ��(��0)
 * function: �����������������䷢�����ݽӿ�
 */
int StereoSendData(unsigned char *buf, unsigned int length);

/*
 * @param1: buf - �������ݵĻ�����
 * @param2: length - �������ݵĻ�������С
 * @return: int �ɹ�(0)/ʧ��(��0)
 * function: �����������������䷢�����ݽӿ�
 */
typedef int (*StereoReceiveData)(unsigned char *buf, unsigned int length);

/*
 * @param: StereoReceiveData - �������ݵ��׵�ַ
 * @return: int �ɹ�(0)/ʧ��(��0)
 * function: ע��������������������������ݵĴ�������btЭ��ջ
 */
int RegisterStereoDateNotifyCallback(StereoReceiveData func);

/*
 * @param: status - ��ǰ����״̬, see details {@link StereoPlayMode}
 * @return: void
 * function: App֪ͨЭ��ջ��ǰ�������ֲ���״̬�������¼������:
 *          1��WIFI_TO_A2DP
 *          2��WIFI_TO_BIS
 *          3��A2DP_TO_BIS
 *          4��UNKNOWN_STATUS
 */
void AppNotifyStereoPlayStatus(StereoPlayMode status);

/*
 * @param: addr - ��ǰ����������ֻ���ַ
 *         role - Ӧ�ò���øýӿڽ��������л�ʱ, ��̨���䵱ǰ�Ľ�ɫ(master/slave)
 * @return: int �ɹ�(0)/ʧ��(��0)
 * function: ����������������, Ӧ�ò���ô˽ӿڽ��������л�
 */
int StereoMasterSlaveSwitch(BdAddr *addr, StereoRole role);

/*
 * @param: addr - ���������������
 * @return: int �ɹ�(0)/ʧ��(��0)
 * function: �����������ɹ���, �������������������
 */
int StereoChannelSwitch(StereoChannelSet channel);

/*
 * @param: void
 * @return: BdAddr - ��ȡ����������ֵĵ�ַ
 * function: 1������ģʽ, ��ȡ����factory��mac��ַ
 *           2��TWSģʽ, ��ȡ����master��mac��ַ
 */
BdAddr *GetExternalDisplayAddress(void);

/*
 * ���Ͻӿ������������ֵĽӿڶ��� end
 */

void PeriodGetDeviceRssi(void);

#endif
