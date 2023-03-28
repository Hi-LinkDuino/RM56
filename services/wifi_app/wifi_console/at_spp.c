#ifdef DEBUG_SPP_LOG

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "at_spp.h"
#include "app_spp.h"
#include "sdp_api.h"
#include "spp_api.h"
// #include "at_printf.h"
// #include "at.h"
#include "kfifo.h"
#include "console.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "ohos_bt_def.h"

#define SPP_MAX_PACKET_NUM 20
#define RFCOMM_CHANNEL_FOR_SPP RFCOMM_CHANNEL_16
#define SPP_ERR_FAILURE (unsigned int)(-1)
#define SPP_ERR_SUCCESS 0
#define SPP_CONNECTED 1
#define SPP_DISCONNECTED 0

#define SPP_UUID128         16
#define SPP_MAX_PACKET_NUMBER       20
#define SPP_RFCOMM_CHANNEL      RFCOMM_CHANNEL_18//RFCOMM_CHANNEL_17

#define SPP_ATT_CLASS_ID   17//3
#define SPP_ATTRIB_PROTO_HEADER_UUID 12
#define SPP_ATTRIB_PROTO_HEADER_ATTRIB_ID 3
#define SPP_ATTRIB_PROTO_HEADER_PARAM_LEN 5

#define SPP_ATTRIB_ROFILE_HEADER_TYPE 8
#define SPP_ATTRIB_ROFILE_HEADER_SIZE 6
#define SPP_ATTRIB_ROFILE_HEADER_DATA 0x0102

#define SPP_SERVICE_NAME_SIZE 5
#define SPP_SERVICE_PORT_NUM 0
#define SPP_SERVICE_NAME_OFFSET 0x0100

#define SPP_DISCOVER_CHECK_TIME 12000
#define SPP_TX_QUEUE_MSG_LEN 256 //512
#define SPP_TX_QUEUE_MSG_NUM 20//50  //200
#define SPP_RX_QUEUE_MSG_LEN 128 //512
#define SPP_RX_QUEUE_MSG_NUM 10//20 //50
#define SPP_RX_BUFF_SIZE 128//1024
#define SPP_WRITE_DELAY_TIME 2
#define SPP_TX_BUSY 1
#define SPP_TX_IDLE 0
#define QUEUE_INFO 0
#define LOS_NOT_WAIT 0
#define EMPTY_ID 0


#define SPP_TX_STACK_SIZE (2*1024)
#define SPP_RX_STACK_SIZE (1*1024)

static const uint8_t g_sppServerUuid128[SPP_UUID128] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x01, 0x11, 0x00, 0x00
};

static const uint8_t g_custmizeSppClassId[] = {
    SDP_ATTRIB_HEADER_8BIT(SPP_ATT_CLASS_ID),
    SDP_UUID_128BIT(g_sppServerUuid128),
};

static const uint8_t g_custmizeSppProtoDesclist[] = {
    SDP_ATTRIB_HEADER_8BIT(SPP_ATTRIB_PROTO_HEADER_UUID),
    SDP_ATTRIB_HEADER_8BIT(SPP_ATTRIB_PROTO_HEADER_ATTRIB_ID),
    SDP_UUID_16BIT(PROT_L2CAP),
    SDP_ATTRIB_HEADER_8BIT(SPP_ATTRIB_PROTO_HEADER_PARAM_LEN),
    SDP_UUID_16BIT(PROT_RFCOMM),
    SDP_UINT_8BIT(SPP_RFCOMM_CHANNEL),
};

static const uint8_t g_custmizeSppProfileDescList[] = {
    SDP_ATTRIB_HEADER_8BIT(SPP_ATTRIB_ROFILE_HEADER_TYPE),
    SDP_ATTRIB_HEADER_8BIT(SPP_ATTRIB_ROFILE_HEADER_SIZE),
    SDP_UUID_16BIT(BTIF_SC_SERIAL_PORT),
    SDP_UINT_16BIT(SPP_ATTRIB_ROFILE_HEADER_DATA),
};

static const uint8_t g_custmizeSppServiceName[] = {
    SDP_TEXT_8BIT(SPP_SERVICE_NAME_SIZE),
    'a','l','e','x','\0'
};

static sdp_attribute_t g_custmizeSppAttributes[] = {
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, g_custmizeSppClassId),
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, g_custmizeSppProtoDesclist),
    SDP_ATTRIBUTE(AID_BT_PROFILE_DESC_LIST, g_custmizeSppProfileDescList),
    SDP_ATTRIBUTE((AID_SERVICE_NAME + SPP_SERVICE_NAME_OFFSET), g_custmizeSppServiceName),
};

extern bt_bdaddr_t* g_spp_remote_addr;
static struct spp_device *g_sppDev = NULL;
static uint8_t g_sppRxBuf[SPP_RECV_BUFFER_SIZE];
osMutexDef(serviceSppMutex);
static btif_sdp_record_t *g_sppSdpRecord = NULL;
static struct spp_service *g_sppService = NULL;

static osMessageQueueId_t g_sppTxQueueID;
static osMessageQueueId_t g_sppRxQueueID;
static bool g_sppTxStatus = SPP_TX_IDLE;

static osTimerId g_sppTimerId = NULL;
static void SppDiscoverChecker(void);
osTimerDef(BT_DISCOVER_CHECKER, SppDiscoverChecker);

extern struct kfifo console_kfifo;
extern void app_bt_accessmode_set_by_AT_command(btif_accessible_mode_t mode);
int spp_rx_totalLen = 0;

static int SppRxMsgWrite(const char *msgAddr, uint16_t len)
{
    int ret = SPP_ERR_FAILURE;
    if(g_sppRxQueueID) {
        ret = osMessageQueuePut(g_sppRxQueueID, msgAddr, 0, 0);
    }
    return ret;
}

static void SppHandleCallBack(uint8_t deviceId, const struct spp_device *localDev, const struct spp_callback_parms *info)
{
    switch(info->event) {
        case BTIF_SPP_EVENT_REMDEV_CONNECTED:
            g_sppDev->spp_connected_flag = SPP_CONNECTED;
            break;
        case BTIF_SPP_EVENT_REMDEV_DISCONNECTED:
            g_sppTxStatus = SPP_TX_IDLE;
            SppDiscoverChecker();
            g_sppDev->spp_connected_flag = SPP_DISCONNECTED;
            break;
        case BTIF_SPP_EVENT_DATA_SENT:
            g_sppTxStatus = SPP_TX_IDLE;
            break;
        default:
            break;
    }
}

static int SppHandleDataEventFunc(uint8_t deviceId, void *localDev, uint8_t process, const uint8_t *recvData, unsigned short recvDataLen)
{
    int msgLen, rxMsgLen;
    int ret;
    uint8_t *data = NULL;
    if(recvData == NULL) {
        return -1;
    }

    printf("SppHandleDataEventFunc!....\n");

    ret = 0;
    data = recvData;
    msgLen = recvDataLen;
    spp_rx_totalLen = recvDataLen;
    // while (msgLen > 0) {
    //     rxMsgLen = (msgLen >= SPP_RX_QUEUE_MSG_LEN) ? SPP_RX_QUEUE_MSG_LEN : (msgLen % SPP_RX_QUEUE_MSG_LEN);
    //     ret += SppRxMsgWrite(data, rxMsgLen);
    //     msgLen -= rxMsgLen;
    //     data += rxMsgLen;
    // }
    printf("%s, %d\n", __func__, spp_rx_totalLen);
    for (int i = 0; i < spp_rx_totalLen; i++ ) {
        printf("0x%x ",data[i]);
    }
    printf("%d\n", __LINE__);
    return (int)ret;
}

void RxSppTaskBody(void *param)
{
    int ret;
    bool isRepeat = false;

    char *sppRxMsg = (char *)malloc(SPP_RX_BUFF_SIZE);
    if(sppRxMsg == NULL) {
        goto osThreadErrExitRx;
    }

    memset(sppRxMsg, 0, SPP_RX_BUFF_SIZE);

    osMessageQueueAttr_t mq_attr;
    mq_attr.name = NULL;
    mq_attr.attr_bits = 0U;
    mq_attr.cb_mem = NULL;
    mq_attr.cb_size = 0U;
    mq_attr.mq_mem = NULL;
    mq_attr.mq_size = 0U;
    g_sppRxQueueID = osMessageQueueNew(SPP_RX_QUEUE_MSG_NUM, SPP_RX_QUEUE_MSG_LEN, &mq_attr);
    if(!g_sppRxQueueID)
    {
        goto osThreadErrExitRx;
    }

    while(1) {

        ret = osMessageQueueGet(g_sppRxQueueID, sppRxMsg, NULL, osWaitForever);
        if(ret != 0)
        {
            continue;
        }
        if(spp_rx_totalLen >= SPP_RX_BUFF_SIZE) {
            spp_rx_totalLen = 0;
            isRepeat = false;
            memset(sppRxMsg,  0, SPP_RX_BUFF_SIZE);
            continue;
        }

        printf("%s, %d\n", __func__, spp_rx_totalLen);
        for (int i = 0; i < spp_rx_totalLen; i++ ) {
            printf("0x%x ",sppRxMsg[i]);
        }
        printf("%d\n", __LINE__);

    }

osThreadErrExitRx:
    if (sppRxMsg != NULL) {
        free(sppRxMsg);
        sppRxMsg = NULL;
    }

    if(g_sppRxQueueID) {
        osMessageQueueDelete(g_sppRxQueueID);
    }

}

static void SppDiscoverChecker(void)
{
    unsigned char currentActiveLinks = btif_me_get_activeCons();
    if(currentActiveLinks == 0)
    {
        app_bt_accessmode_set_by_AT_command(0x3);
    }
}
// #if 0
static unsigned int SppEnvSetup(osMutexId sppMid)
{
    unsigned int ret;
    btif_sdp_record_param_t param;

    param.attrs = &g_custmizeSppAttributes[0];
    param.attr_count = ARRAY_SIZE(g_custmizeSppAttributes);
    param.COD = BTIF_COD_MAJOR_PERIPHERAL;
    ret = btif_sdp_record_setup(g_sppSdpRecord, &param);
    if ( ret != BT_STS_SUCCESS ){
        return SPP_ERR_FAILURE;
    }

    if(g_sppService == NULL) {
        g_sppService = btif_create_spp_service();
    }

    if(g_sppService == NULL) {
        return SPP_ERR_FAILURE;
    }

    g_sppService->rf_service.serviceId = SPP_RFCOMM_CHANNEL;
    g_sppService->numPorts = SPP_SERVICE_PORT_NUM;
    ret = btif_spp_service_setup(g_sppDev, g_sppService, g_sppSdpRecord);
    if(ret != BT_STS_SUCCESS) {
        return SPP_ERR_FAILURE;
    }

    g_sppDev->portType = BTIF_SPP_SERVER_PORT;
    g_sppDev->app_id = app_spp_alloc_server_id();
    g_sppDev->spp_handle_data_event_func = SppHandleDataEventFunc;
    ret = btif_spp_init_device(g_sppDev, SPP_MAX_PACKET_NUM, sppMid);
    if (ret != BT_STS_SUCCESS) {
        return SPP_ERR_FAILURE;
    }

    ret = btif_spp_open(g_sppDev, NULL, SppHandleCallBack);
    if(ret != BT_STS_SUCCESS) {
        return SPP_ERR_FAILURE;
    }
    return SPP_ERR_SUCCESS;
}
// #else
#define TEST_PORT_SPP 0x1101
static const uint8_t SppTestSearchReq[] = {
    /* First parameter is the search pattern in data element format. It
    * is a list of 3 UUIDs.
    */
    /* Data Element Sequence, 9 bytes */
    SDP_ATTRIB_HEADER_8BIT(9),
    SDP_UUID_16BIT(TEST_PORT_SPP),
    SDP_UUID_16BIT(PROT_L2CAP),  /* L2CAP UUID in Big Endian */
    SDP_UUID_16BIT(PROT_RFCOMM), /* UUID for RFCOMM in Big Endian */
    /* The second parameter is the maximum number of bytes that can be
    * be received for the attribute list.
    */
    0x00,
    0x64, /* Max number of bytes for attribute is 100 */
    SDP_ATTRIB_HEADER_8BIT(9),
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST),
    SDP_UINT_16BIT(AID_BT_PROFILE_DESC_LIST),
    SDP_UINT_16BIT(AID_ADDITIONAL_PROT_DESC_LISTS)
};
static unsigned int SppCliSetup(osMutexId mid)
{
#if 1
    printf("[%s]", __func__);
    bt_status_t status;
    // osMutexId mid;

    if(g_sppDev == NULL)
        g_sppDev = app_create_spp_device();
    //app_spp_init_tx_buf(NULL);
    btif_spp_init_rx_buf(g_sppDev, g_sppRxBuf, SPP_RECV_BUFFER_SIZE);

    // mid = osMutexCreate(osMutex(client_spp_test_mutex));
    // if (!mid)
    // {
    //     ASSERT(0, "cannot create mutex");
    // }
    g_sppDev->portType = BTIF_SPP_CLIENT_PORT;
    g_sppDev->app_id = 0x8000;
    g_sppDev->spp_handle_data_event_func = SppHandleDataEventFunc;

    btif_spp_init_device(g_sppDev, SPP_MAX_PACKET_NUM, mid);
    btif_remote_device_t* RemDev = btif_me_get_remote_device_by_bdaddr(g_spp_remote_addr);

    g_sppDev->sppDev.type.client.rfcommServiceSearchRequestPtr = (uint8_t*)SppTestSearchReq;
    g_sppDev->sppDev.type.client.rfcommServiceSearchRequestLen = sizeof(SppTestSearchReq);
    status  = btif_spp_open(g_sppDev, RemDev, SppHandleCallBack);
    printf("%s status is %d", __func__, status);
#endif
    return SPP_ERR_SUCCESS;
}
// #endif

void TxSppTaskBody(void *param)
{
    int ret;
    char *sppTxBuff = (char *)malloc(SPP_TX_QUEUE_MSG_LEN);
    if (sppTxBuff == NULL)
    {
        goto osThreadErrExit;
    }
    memset(sppTxBuff,  0, SPP_TX_QUEUE_MSG_LEN);


    osMessageQueueAttr_t mq_attr;
    mq_attr.name = NULL;
    mq_attr.attr_bits = 0U;
    mq_attr.cb_mem = NULL;
    mq_attr.cb_size = 0U;
    mq_attr.mq_mem = NULL;
    mq_attr.mq_size = 0U;
    g_sppTxQueueID = osMessageQueueNew(SPP_TX_QUEUE_MSG_NUM, SPP_TX_QUEUE_MSG_LEN, &mq_attr);
    if(!g_sppTxQueueID)
    {
        goto osThreadErrExit;
    }

    g_sppTxStatus = SPP_TX_IDLE;
    while(1) {
        if (g_sppTxStatus == SPP_TX_BUSY) {
            osDelay(SPP_WRITE_DELAY_TIME);
            continue;
        }

        int msgLen = SPP_TX_QUEUE_MSG_LEN;
        ret = osMessageQueueGet(g_sppTxQueueID, sppTxBuff, NULL, osWaitForever);
        if(ret != 0)
        {
            continue;
        }

        g_sppTxStatus = SPP_TX_BUSY;
        btif_spp_write(g_sppDev, sppTxBuff, &msgLen);
    }

osThreadErrExit:
    if(sppTxBuff != NULL){
        free(sppTxBuff);
        sppTxBuff = NULL;
    }

    if(g_sppTxQueueID) {
        osMessageQueueDelete(g_sppTxQueueID);
    }
}

osThreadDef(TxSppTaskBody, osPriorityNormal, 1, SPP_TX_STACK_SIZE, "AtSppTX");
osThreadDef(RxSppTaskBody, osPriorityNormal, 1, SPP_RX_STACK_SIZE, "AtSppRX");

int SppThreadInit(void)
{
    if (g_sppTimerId == NULL) {
        g_sppTimerId = osTimerCreate(osTimer(BT_DISCOVER_CHECKER), osTimerPeriodic, NULL);
        if(g_sppTimerId == NULL) {
            return -1;
        }
        osTimerStart(g_sppTimerId, SPP_DISCOVER_CHECK_TIME);
    }
    osThreadId TxTid = osThreadCreate(osThread(TxSppTaskBody), NULL);
    // osThreadId RxTid = osThreadCreate(osThread(RxSppTaskBody), NULL);
    if(TxTid == NULL /*|| RxTid == NULL*/) {
        return -1;
    }
    return 0;
}

bt_bdaddr_t* g_spp_remote_addr = NULL;
//AppInitCustmizeSpp
unsigned int AtSppInitApi(void)
{
    printf("AtSppInitApi into!");
    //RESP_OK_EQU("AtSppInitApi into!\n");
    bt_status_t ret;
    app_bt_accessmode_set_by_AT_command(0x3);
    if(g_sppDev == NULL) {
        g_sppDev = app_create_spp_device();
        if(g_sppDev == NULL) {
            printf("app_create_spp_device SppInitFailed!");
            return SPP_ERR_FAILURE;
        }
    }
    
    ret = btif_spp_init_rx_buf(g_sppDev, g_sppRxBuf, SPP_RECV_BUFFER_SIZE);
    if(ret != BT_STS_SUCCESS) {
        printf("btif_spp_init_rx_buf SppInitFailed!");
        goto SppInitFailed;
    }

    if(g_sppDev->creditMutex == NULL) {
        g_sppDev->creditMutex = osMutexCreate(osMutex(serviceSppMutex));
    }

    if(g_sppDev->creditMutex == NULL) {
        printf("osMutexCreate SppInitFailed!");
        goto SppInitFailed;
    }

    g_sppSdpRecord = btif_sdp_create_record();
    if(g_sppSdpRecord == NULL)
    {
        printf("btif_sdp_create_record SppInitFailed!");
        goto SppInitFailed;
    }

    if(g_spp_remote_addr){
        ret = SppCliSetup(g_sppDev->creditMutex);
        if(ret == SPP_ERR_FAILURE){
            printf("SppEnvSetup SppInitFailed!");
            goto SppInitFailed;
        }
    } else {
        ret = SppEnvSetup(g_sppDev->creditMutex);
        if(ret == SPP_ERR_FAILURE){
            printf("SppEnvSetup SppInitFailed!");
            goto SppInitFailed;
        }
    }

    int retThr = SppThreadInit();
    if(retThr == SPP_ERR_FAILURE) {
        printf("SppThreadInit SppInitFailed!");
        goto SppInitFailed;
    }

    return SPP_ERR_SUCCESS;

SppInitFailed:
    if(g_sppDev != NULL) {
        if(g_sppSdpRecord != NULL) {
            btif_sdp_free_record(g_sppSdpRecord);
            g_sppSdpRecord = NULL;
        }
        if(g_sppDev->creditMutex != NULL){
            osMutexDelete(g_sppDev->creditMutex);
            g_sppDev->creditMutex = NULL;
        }
        btif_destroy_spp_device(g_sppDev);
        g_sppDev = NULL;
    }
    return SPP_ERR_FAILURE;
}

unsigned int SppSendData(const char *sendData, unsigned short dataLen)
{
    unsigned int status = SPP_ERR_FAILURE;
    unsigned short txMsgLen;
    int i, msgLen;
    if(g_sppDev == NULL || g_sppDev->spp_connected_flag != SPP_CONNECTED)
    {
        return SPP_ERR_SUCCESS;
    }

    if(sendData == NULL)
    {
        return SPP_ERR_SUCCESS;
    }

    msgLen = dataLen;

    /*push msg to queue. if there is missing data, the queue size can be increased.*/
    int maxQueueNum = dataLen / SPP_TX_QUEUE_MSG_LEN;
    if(maxQueueNum > SPP_TX_QUEUE_MSG_NUM) {
        return SPP_ERR_FAILURE;
    }

    if(g_sppTxQueueID) {
        for (i = 0; i <= maxQueueNum; i++){
            txMsgLen = (msgLen >= SPP_TX_QUEUE_MSG_LEN) ? SPP_TX_QUEUE_MSG_LEN : (msgLen % SPP_TX_QUEUE_MSG_LEN);
            status = osMessageQueuePut(g_sppTxQueueID, &sendData[i * SPP_TX_QUEUE_MSG_LEN], 0, 0);
            if (status != 0)
            {
                break;
            }
            msgLen -=SPP_TX_QUEUE_MSG_LEN;
        }
    }
    return (int)status;
}

#endif