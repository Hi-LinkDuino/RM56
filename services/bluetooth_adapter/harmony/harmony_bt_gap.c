#include "ohos_bt_def.h"
#include "me_api.h"
#include "string.h"
#include "bt_if.h"
#include "cmsis_os.h"
#include "harmony_utils.h"
#include "ohos_bt_gap.h"
#include "nvrecord_extension.h"
#include "nvrecord_env.h"
#include "nvrecord_bt.h"
#include "me_api.h"
#include "btapp.h"
#include "app_bt.h"
#ifdef IBRT
#include "app_tws_ibrt.h"
#include "app_ibrt_if.h"
#include "app_tws_ibrt_conn_api.h"
#endif
#include "private_vendor_api.h"
#include "bt_adaptor_dbg.h"
#include "factory_section.h"

#define SCAN_TIMER_IDLE             0
#define SCAN_TIMER_RUNNING          1

static void scan_mode_timer_handler(void const *param);
osTimerDef(scan_mode_timer, scan_mode_timer_handler);
static osTimerId scan_mode_timer_id = NULL;
uint8_t timer_run_state = SCAN_TIMER_IDLE;
extern uint8_t* factory_section_get_bt_address(void);
#if defined(IBRT) && defined(USE_BT_ADAPTER)
extern void app_register_is_accept_connect_callback(void * cb);
#endif

GapBtScanMode bt_scan_mode = OHOS_GAP_SCAN_MODE_NONE;

BtGapCallBacks *g_gap_cb_func = NULL;

static void enter_freeman_mode_timer_handler(void const *param);
osTimerDef(enter_freeman_mode_timer, enter_freeman_mode_timer_handler);
static osTimerId enter_freeman_mode_timer_id = NULL;
static bool g_ibrt_enter_freeman_mode = false;

static void scan_mode_timer_handler(void const *param)
{
    btif_me_set_accessible_mode(BTIF_BAM_NOT_ACCESSIBLE,NULL);
    timer_run_state = SCAN_TIMER_IDLE;
}

uint8_t ohos_scanmode_switch_to_bes(uint8_t scanMode)
{
    uint8_t bes_mode;
    switch(scanMode)
    {
        case OHOS_GAP_SCAN_MODE_NONE:
            bes_mode = BTIF_BAM_NOT_ACCESSIBLE;
        break;
        case OHOS_GAP_SCAN_MODE_CONNECTABLE:
            bes_mode = BTIF_BAM_CONNECTABLE_ONLY;
        break;
        case OHOS_GAP_SCAN_MODE_GENERAL_DISCOVERABLE:
            bes_mode = BTIF_BAM_DISCOVERABLE_ONLY;
        break;
        case OHOS_GAP_SCAN_MODE_LIMITED_DISCOVERABLE:
            bes_mode = BTIF_BAM_DISCOVERABLE_ONLY;
        break;
        case OHOS_GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE:
            bes_mode = BTIF_BAM_GENERAL_ACCESSIBLE;
        break;
        case OHOS_GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE:
            bes_mode = BTIF_BAM_GENERAL_ACCESSIBLE;
        break;        
        default:
            bes_mode = BTIF_BAM_NOT_ACCESSIBLE;
            break;
    }
    return bes_mode;
}

void set_bt_scan_limited_mode(int mode, int duration)
{
    static uint8_t timer_init_flag = 0;
    if (!timer_init_flag) {
        timer_init_flag = 1;
        scan_mode_timer_id = osTimerCreate(osTimer(scan_mode_timer), osTimerOnce, NULL);
    }
    switch(mode)
    {
        case OHOS_GAP_SCAN_MODE_LIMITED_DISCOVERABLE:
        case OHOS_GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE:
            if (scan_mode_timer_id != NULL) {
                if (timer_run_state == SCAN_TIMER_IDLE){
                    osTimerStart(scan_mode_timer_id, duration);
                    timer_run_state = SCAN_TIMER_RUNNING;
                }
                else{
                    osTimerStop(scan_mode_timer_id);
                    osTimerStart(scan_mode_timer_id, duration);
                }
            }
        break;
        default:
            if (timer_run_state == SCAN_TIMER_RUNNING){
                osTimerStop(scan_mode_timer_id);
                timer_run_state = SCAN_TIMER_IDLE;
            }
        break;
    }
}

bool EnableBt(void)
{
    if(g_gap_cb_func && g_gap_cb_func->stateChangeCallback){
        g_gap_cb_func->stateChangeCallback(OHOS_BT_TRANSPORT_DUAL_MONO,OHOS_GAP_STATE_TURN_ON);
    }
    return true;
}

bool DisableBt(void)
{
    btif_me_set_accessible_mode(BTIF_BAM_NOT_ACCESSIBLE,NULL);
    if(g_gap_cb_func && g_gap_cb_func->stateChangeCallback){
        g_gap_cb_func->stateChangeCallback(OHOS_BT_TRANSPORT_DUAL_MONO,OHOS_GAP_STATE_TURN_OFF);
    }

    return true;
}

BdAddr bdaddress;
BdAddr* GetLocalAddress(void)
{
    uint8_t *bt_addr = factory_section_get_bt_address();

    if (bt_addr)
        memcpy(bdaddress.addr,bt_addr,OHOS_BD_ADDR_LEN);
    return &bdaddress;
}

bool GetLocalName(unsigned char *localName, unsigned char *length)
{
    uint8_t* l_name = NULL;
     uint8_t * bt_addr = NULL;
#ifdef IBRT     
     ibrt_ctrl_t *p_ibrt_ctrl = app_tws_ibrt_get_bt_ctrl_ctx();
    if (length == NULL || localName == NULL || p_ibrt_ctrl == NULL){
        return false;
    }

    bt_addr = factory_section_get_bt_address();
    //factory_address != local_addr in tws mode
    if(memcmp(bt_addr, p_ibrt_ctrl->local_addr.address, 6)){
        struct nvrecord_env_t *nvrecord_env = NULL;
        nv_record_env_get(&nvrecord_env);

        TRACE(1, "in tws mode get tws_name");
        if(nvrecord_env){
            *length =  nvrecord_env->ibrt_mode.tws_name_len;
            memcpy(localName, nvrecord_env->ibrt_mode.tws_name, *length);
        }else{
            return false;
        }
    }else{
        l_name = factory_section_get_bt_name();
         if(l_name){
            *length = strlen((const char*)l_name);
            memcpy(localName, l_name, *length);        
        }else
            return false;
    }
#endif
    return true;
}

bool SetLocalName(unsigned char *localName, unsigned char length)
{
    if (localName == NULL){
        return false;
    }
    
    bt_set_local_dev_name((const unsigned char*)localName, strlen((const char*)localName) + 1);
    bt_stack_config((const unsigned char*)localName, strlen((const char*)localName) + 1);
    if(g_gap_cb_func && g_gap_cb_func->deviceNameChangedCallback){
        g_gap_cb_func->deviceNameChangedCallback(localName,strlen((const char*)localName) + 1);
    }

    factory_section_set_bt_name((const char*)localName, length);
#ifdef IBRT
    app_tws_ibrt_sync_tws_name(localName, length + 1);
#endif
    return true;
}

int GetBtScanMode(void)
{
    return bt_scan_mode;
}

bool SetBtScanMode(int mode, int duration)
{
    const btif_access_mode_info_t info = {0x800, 0x12, 0x800, 0x12};
    uint8_t status = BT_STS_SUCCESS,scanMode;
    if (bt_scan_mode != mode){
        bt_scan_mode = mode;
        if(g_gap_cb_func && g_gap_cb_func->scanModeChangedCallback){
            g_gap_cb_func->scanModeChangedCallback(bt_scan_mode);
        }
    }
    
    scanMode = ohos_scanmode_switch_to_bes(mode);
    set_bt_scan_limited_mode(mode, duration);
    status = btif_me_set_accessible_mode(scanMode,&info);
    return (bes_status_to_ohos(status) == OHOS_BT_STATUS_SUCCESS)?true:false;    
}

bool GetPariedDevNum(unsigned int *number)
{
	*number = nv_record_get_paired_dev_count();
    return true;
}


bool GetRemoteDevicesName(PairedDeviceInfo *devInfo)
{
    char *devName = NULL;
    if(devInfo == NULL){
        return false;
    }
#ifdef LINK_KEY_ENCRYPT_BY_CUSTOMER
    devName =(char*)app_get_current_remote_device_name();
#endif
    if(devName == NULL){
        return false;
    }
    memcpy(devInfo->deviceName,devName,OHOS_BD_NAME_LEN);
    return true;
}

bool GetPairedDevicesList(PairedDeviceInfo *devInfo, int *number)
{
    btif_device_record_t record;
    bt_status_t status = BT_STS_SUCCESS;
    *number = nv_record_get_paired_dev_count();
    for(int i=0; i<*number; i++)
    {
        status = nv_record_enum_dev_records(i, &record);
        if(status == BT_STS_SUCCESS){
            memcpy(devInfo[i].addr.addr, record.bdAddr.address,OHOS_BD_ADDR_LEN);
        }
    }
    return true;
}

bool RemovePair(const BdAddr addr)
{
    nv_record_ddbrec_delete((bt_bdaddr_t*)&addr);
    nv_record_flash_flush();
    return true;
}

bool RemoveAllPairs(void)
{
    btif_device_record_t record = {0};
    int paired_dev_count = nv_record_get_paired_dev_count();

    for (int i = paired_dev_count - 1; i >= 0; --i)
    {
        nv_record_enum_dev_records(i, &record);

        nv_record_ddbrec_delete(&record.bdAddr);
    }
    nv_record_flash_flush();
    return true;
}

bool IsAclConnected(BdAddr addr)
{
    struct BT_DEVICE_T *curr_device = NULL;
    for(int i=0;i<BT_DEVICE_NUM;i++)
    {
        curr_device = app_bt_get_device(i);
        if (curr_device)
        {
            if(!memcmp(curr_device->remote.address,addr.addr,OHOS_BD_ADDR_LEN))
                return curr_device->acl_is_connected;
        }
    }
    return false;
}

void bt_acl_connect_state_change(uint8_t type, bt_bdaddr_t* addr, uint32_t para)
{
#ifdef IBRT
    bt_bdaddr_t invaild_addr = {{0x00,0x00,0x00,0x00,0x00,0x00}};
    ibrt_link_type_e link_type = app_tws_ibrt_get_remote_link_type(addr);
    LOG_I("%s event:%d link_type:%d reason:%d", __func__, type, link_type, para);
    switch(type)
    {
        case BT_ACL_CONNECT:
            if (g_gap_cb_func && g_gap_cb_func->aclStateChangedCallbak) {
                if (link_type != TWS_LINK) {
                    g_gap_cb_func->aclStateChangedCallbak((BdAddr *)addr,OHOS_GAP_ACL_STATE_CONNECTED,para);
                }
            }
        break;

        case BT_ACL_DISCONNECT:
            if (g_gap_cb_func && g_gap_cb_func->aclStateChangedCallbak) {
                if (link_type != TWS_LINK) {
                    g_gap_cb_func->aclStateChangedCallbak((BdAddr *)addr,OHOS_GAP_ACL_STATE_DISCONNECTED,para);
                    break;
                }

                // only USER does freeman action, it shall delete nv_record elements.
                if (g_ibrt_enter_freeman_mode)
                {
                    struct nvrecord_env_t *nvrecord_env = NULL;
                    nv_record_env_get(&nvrecord_env);
                    if (nvrecord_env) {
                        nv_record_ddbrec_delete(&nvrecord_env->ibrt_mode.record.bdAddr);
                        nv_record_update_ibrt_info(STEREO_NONE, &invaild_addr);
                        nv_record_clean_tws_name();
                        nv_record_ddbrec_delete_all();
                        nv_record_flash_flush();
                    }
                }
                if (enter_freeman_mode_timer_id) {
                    osTimerStart(enter_freeman_mode_timer_id, 100);
                }
                g_ibrt_enter_freeman_mode = false;
            }
        break;
        default:
        break;
    }
#endif
}

void bt_pair_state_change(bt_bdaddr_t* addr, uint32_t para)
{
    if(g_gap_cb_func && g_gap_cb_func->pairStatusChangedCallback){
        g_gap_cb_func->pairStatusChangedCallback((BdAddr *)addr,para);
    }
}

uint8_t bt_is_accept_connect(const bt_bdaddr_t *bdAddr)
{
    bool is_conn = true;
    if(g_gap_cb_func && g_gap_cb_func->isAcceptConnOnSafeModeCallback){
        g_gap_cb_func->isAcceptConnOnSafeModeCallback((const BdAddr *)bdAddr,&is_conn);
    }

    return is_conn;
}

bool DisconnectRemoteDevice(BdAddr *addr)
{
#ifdef IBRT
    app_tws_ibrt_disconnect_mobile((bt_bdaddr_t *)addr);
#endif
    return true;
}

bool ConnectRemoteDevive(BdAddr *addr)
{
#ifdef IBRT
    app_ibrt_conn_remote_dev_connect_request((bt_bdaddr_t *)addr,OUTGOING_CONNECTION_REQ,true,0);
#endif
    return true;
}

int GapRegisterCallbacks(BtGapCallBacks *func)
{
    g_gap_cb_func = func;
    app_bt_acl_conn_callback_register(bt_acl_connect_state_change);
    app_bt_pair_callback_register(bt_pair_state_change);
#if defined(IBRT) && defined(USE_BT_ADAPTER)
    app_register_is_accept_connect_callback((void *)bt_is_accept_connect);
#endif
    return 0;
}

static void enter_freeman_mode_timer_handler(void const *param)
{
#ifdef IBRT
    app_ibrt_if_enter_freeman_pairing();
#endif
}

void GapSetNeedEnterFreemanMode(bool freeman)
{
    g_ibrt_enter_freeman_mode = freeman;
    if(enter_freeman_mode_timer_id == NULL)
        enter_freeman_mode_timer_id = osTimerCreate(osTimer(enter_freeman_mode_timer), osTimerOnce, NULL);
}

