/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    bt_gap_adp.c
 * @brief:   vendor bluetooth apis defination
 * @author:  
 * @date:    2021/12/10
 * @version: 1.0
 */

#include "tg_bluetooth.h"
#include "bt_adaptor_dbg.h"
#ifdef IBRT
#include "app_ibrt_customif_cmd.h"
#include "app_tws_ctrl_thread.h"
#include "hal_trace.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt.h"
#endif
#include "nvrecord_env.h"
#include "nvrecord_bt.h"
#include "bt_adaptor_dbg.h"
#include "factory_section.h"
#include "me_api.h"
#include "ddbif.h"
#include "bt_drv_interface.h"
#include "app_bt.h"
#include "app_bt_func.h"
#include "besbt.h"
#include "bt_service_utils.h"

TG_BT_GAP_IMPL_CB_FUNC_T g_bt_gap_cb = {0};
static bool m_gapOn = false;
static bool m_gapInitializedBefor = false;

/* GAP */
/**
 * @brief gap register callback functions
 * @param[in] func:  callback function set.
 * @return: None
 */
void tg_bt_gapCallbackRegister(TG_BT_GAP_IMPL_CB_FUNC_T *func)
{
    g_bt_gap_cb.gapGapScanCB = func->gapGapScanCB;
    g_bt_gap_cb.gapPairCB = func->gapPairCB;
    g_bt_gap_cb.gapUnpairCB = func->gapUnpairCB;
}

// static void  btStackInit(){
//     LOG_E("%s \n", __func__);
    
//     if(!m_gapInitializedBefor){
//         // app_bt_init();
//         // btdrv_start_bt();
//     }
    
//     if(!m_gapInitializedBefor){
//         // BesbtInit();
//         // app_wait_stack_ready();
//     }
// }

/**
 * @brief gap on off bluetooth
 * @param[in] fg_on:  on/off flag, 1 means on; 0 means off.
 *            len:  random length
 * @return: Zero on success or error code otherwise
 */
int32_t tg_bt_gapSetOnOff(bool fg_on)
{
    if(fg_on){
        if(!m_gapOn)
            //just set bluetooth to accessible mode
            //app_bt_init();
        m_gapInitializedBefor = true;
    }else{
        if(m_gapOn){
            //just set bluetooth to not accessible mode
            // do nothing
            // app_set_accessmode(BTIF_BAM_NOT_ACCESSIBLE);
        }
    }
    m_gapOn = fg_on;
    LOG_E("gapSetOnOff:%d", fg_on);
    return 0;
}

/**
 * @brief gap get local device info
 * @param[in] local_dev_info:  local device info.
 * @return: None
 */
void tg_bt_gapGetLocalDevInfo(TG_BT_DEV_INFO *local_dev_info)
{
    LOG_E("%s Get local device info:", __func__);
    unsigned char * localName = (unsigned char *)bt_get_local_name();
    memset(local_dev_info->name, 0, TG_NAME_MAX_LEN);
    memcpy(local_dev_info->name, localName, strlen((char *)localName)>TG_NAME_MAX_LEN?(TG_NAME_MAX_LEN - 1):strlen((char *)localName));
    btAddressToMac((unsigned char *)local_dev_info->bdAddr, bt_get_local_address());    
}

/**
 * @brief gap get remote device info
 * @param[in] remote_dev_info:  remote device info.
 * @return: None
 */
void tg_bt_gapGetRemoteDevInfo(TG_BT_DEV_INFO *remote_dev_info)
{
    LOG_E("%s Get remote device info:", __func__);
    btif_remote_device_t *remoteDev = app_bt_get_connected_mobile_device_ptr();
    bt_bdaddr_t * bAddr = NULL;
    if(remoteDev){
        bAddr = btif_me_get_remote_device_bdaddr(remoteDev);
        btAddressToMac((unsigned char *)remote_dev_info->bdAddr, bAddr->address);
    }
}

/**
 * @brief gap set scan mode
 * @param[in] conn_flag:  page scan mode, 1 means on, 0 means off.
 * @param[in] disc_flag:  inquiry scan mode, 1 means on, 0 means off.
 * @return: None
 */
void tg_bt_gapSetScanMode(bool conn_flag, bool disc_flag)
{
    unsigned char accessMode = (conn_flag << 1) | disc_flag;
    app_set_accessmode(accessMode);
    LOG_E("gapSetScanMode:%d %d", conn_flag, disc_flag);
}

/**
 * @brief gap set bluetooth device name
 * @param[in] set_name:  string of bluetooth device name.
 * @return: Zero on success or error code otherwise
 */
extern int factory_section_set_bt_name(const char *name,int len);
extern int bt_stack_config(const unsigned char *dev_name, uint8_t len);
int32_t tg_bt_gapSetName(const int8_t *set_name)
{
    bt_status_t ret = 0;
    bt_status_t len = 0;
    unsigned char name[64] = {0};
    unsigned char *factory_name = factory_section_get_bt_name();

    len = strlen((char *)set_name)+1;
    strncpy((char *)name, (char *)set_name, len);

    if(memcmp(name, factory_name, len))
    {
        //bt and ble use the same name.
        ret = factory_section_set_bt_name((const char *)name, len);
        if(ret != 0){
            LOG_E("set btname failed,factory bin is burn?\n");
        }
        // ret = factory_section_set_ble_name(name, len);
        // if(ret != 0){
        //     LOG_E("set blename failed,factory bin is burn?\n");
        // }
    }

    ret = btif_me_set_local_device_name(name, len);
    if(ret == 2)
    {
        LOG_E("%s set_local_device_name: %s success!", __func__, name);
    }else{
        LOG_E("%s set_local_device_name: %s fail!", __func__, name);
    }
    ret = bt_stack_config(name, len);
    LOG_E("bt_stack_config ret:%d", ret);

    return 0;
}

/**
 * @brief gap start inquiry
 * @param: None
 * @return: None
 */
void tg_bt_startInquiry(void)
{
    LOG_E("BT start Inquiry!");
    btif_me_inquiry(BTIF_BT_IAC_GIAC, 30 , 0);
}

/**
 * @brief gap stop inquiry
 * @param: None
 * @return: None
 */ 
void tg_bt_stopInquiry(void)
{
    LOG_E("BT stop Inquiry!");
    btif_me_cancel_inquiry();
}

/**
 * @brief gap unpair device
 * @param[in] mac:  buffer to store random value.
 * @return: None
 */
void tg_bt_gapUnPair(const int8_t *mac)
{
    bt_status_t status;
    btif_remote_device_t *remoteDev = app_bt_get_connected_mobile_device_ptr();
    bt_bdaddr_t btAddr;
    macToBtAddress((uint8_t *)mac, (uint8_t *)btAddr.address);

    bt_bdaddr_t * rbtAddr = NULL;
    rbtAddr = btif_me_get_remote_device_bdaddr(remoteDev);
    if(rbtAddr != NULL){
        if(!memcmp(btAddr.address, rbtAddr->address, 6)){
        app_bt_MeDisconnectLink(remoteDev);
    }
    }

    status = ddbif_delete_record(&btAddr);
    if(status)
        LOG_E("gapUnPair failed:%d", status);

    if(g_bt_gap_cb.gapUnpairCB){
        if(status)
            g_bt_gap_cb.gapUnpairCB(TG_BT_UNPAIR_FAIL, (int8_t *)mac);
        else
            g_bt_gap_cb.gapUnpairCB(TG_BT_UNPAIR_SUCCESS, (int8_t *)mac);
    }
}

void gapPairResultNotify(unsigned char pairResult, btif_remote_device_t* remDev)
{
    char    mac[TG_BDADDR_MAX_LEN];
    TG_BT_PAIR_RESULT pResult;
    bt_bdaddr_t* btAddr = NULL;

    LOG_E("I AM %s pairResult = %d",__func__,pairResult);
    if(pairResult)
        pResult = TG_BT_PAIR_SUCCESS;
    else
        pResult = TG_BT_PAIR_FAIL;

    if(g_bt_gap_cb.gapPairCB){
        btAddressToMac((unsigned char *)mac, btAddr->address);
        g_bt_gap_cb.gapPairCB(pResult, (int8_t *)mac);
    }
}

static bool bt_get_enable = false;
/**
 * @brief bluetooth enable function
 * @param[in] enable:  1 means enable, 0 means disable.
 * @return: None
 */
extern void bes_bt_init(void);
extern bool app_is_stack_ready(void);
static int __tg_bt_inited = 0;
static void _tg_bt_init_main(void *argument)
{
    TRACE(0,"%s do bes_bt_init", __FUNCTION__);
    bes_bt_init();
    __tg_bt_inited = 1;
    TRACE(0,"%s bes_bt_init done", __FUNCTION__);
}
static osThreadId my_bes_bt_init_id;
osThreadDef(_tg_bt_init_main, osPriorityAboveNormal, 1, (4*1024), "_tg_bt_init_main");
static void _tg_bt_init(void)
{
    my_bes_bt_init_id = osThreadCreate(osThread(_tg_bt_init_main), NULL);
    if (my_bes_bt_init_id == NULL){
        TRACE(0,"%s %d fail to create _tg_bt_init_main task",__func__,__LINE__);
        return;
    }
    TRACE(0,"create _tg_bt_init_main task = 0x%X", (uint32_t)my_bes_bt_init_id);
    while (__tg_bt_inited == 0) {
        osDelay(200);
        TRACE(0,"%s wait bt ready!", __FUNCTION__);
    }
}
void tg_bt_enable(int8_t enable)
{
    if(!app_is_stack_ready())
    {
        TRACE(0,"BT not init yet!!! enter %s: enable = %d", __func__, enable);
        // do init in osTheadNew task for signal ops
        _tg_bt_init();
    }
    else
    {
        TRACE(0,"%s() bt already init\n",__func__);
    }
}

/**
 * @brief bluetooth get status function
 * @return: None
 */
bool tg_bt_get_enable(void)
{
    bt_get_enable = false;
    if(!app_is_stack_ready())
    {
        return bt_get_enable;
    }
    else
    {
        bt_get_enable = true;
    }
    LOG_E("enter %s: status = %d", __func__, bt_get_enable);
    return bt_get_enable;
}

/**
 * @brief bluetooth set or get function
 * @param[in] set:  1 means set, 0 means get.
 * @param[in] en:  1 means enable, 0 means disable.
 * @return: None
 */
bool tg_bt_set_get_enable(bool set, bool en)
{
    LOG_E("enter %s, set = %d, en = %d", __func__, set, en);
    if(set)
    {
        //set
        tg_bt_enable(en);
        return 0;
    }
    else
    {
        //get
        return tg_bt_get_enable();
    }
}

// int32_t tg_bt_startScan(void)
// {
//     app_set_accessmode(BTIF_BAM_GENERAL_ACCESSIBLE);
//     LOG_E("%s: change bt mode to scan mode!", __func__);

//     return 0;
// }

// int32_t tg_bt_stopScan(void)
// {
//     app_set_accessmode(BTIF_BAM_NOT_ACCESSIBLE);
//     LOG_E("%s: change bt mode to scan mode!", __func__);

//     return 0;
// }

#ifdef USE_BT_ADAPTER_TESTER
extern int32_t tg_bt_initGatts(void);
extern void tg_bt_gatt_serviceTest(void);
void tg_bt_gap_test(void)
{
    tg_bt_gapSetOnOff(true);
    tg_bt_initGatts();
    tg_bt_gatt_serviceTest();
}
#endif

