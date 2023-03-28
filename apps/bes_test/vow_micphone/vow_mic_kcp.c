#ifdef __VOW_MIC_KCP__

#include "hal_timer.h"
#include "hal_trace.h"
#include "string.h"
#include "cmsis_os.h"
#include "vow_mic_kcp.h"
#include "vow_data.h"
#include "ikcp.h"
#include "vow_data.h"

#define KCP_SESSION_NUM (MAX_STA)

typedef struct {
    uint32_t session_id;
    ikcpcb *g_mic_pkcp;
} kcp_session_t;

typedef struct {
    uint32_t conv;
    uint8_t  cmd;
    uint8_t  frag;
    uint16_t wnd;
    uint32_t ts;
    uint32_t sn;
    uint32_t una;
    uint32_t len;
}kcp_head_t;
#define IKCP_PUSH_DATA  81
#define IKCP_CMD_ACK    82

#define KCP_ACK_LEN     128

static kcp_session_t kcp_session[KCP_SESSION_NUM];

static osSemaphoreId wait_ack_sem;
osSemaphoreDef(wait_ack_sem);

static uint8_t kcp_ack_buffer[KCP_ACK_LEN];
static uint16_t kcp_recv_len = 0;

static osMutexId vow_kcp_mutex_id = NULL;
osMutexDef(_vow_kcp_mutex);

#define LOCK_VOW_KCP() \
    osMutexWait(vow_kcp_mutex_id, osWaitForever);
#define UNLOCK_VOW_KCP() \
    osMutexRelease(vow_kcp_mutex_id);

void vow_mutext_lock_init()
{
    vow_kcp_mutex_id = osMutexCreate(osMutex(_vow_kcp_mutex));
}

kcp_session_t * vow_get_kcp_object(uint32_t conv)
{
    int i = 0;
    for (i = 0; i< KCP_SESSION_NUM; i++) {
        if (kcp_session[i].g_mic_pkcp->conv == conv) {
            return &kcp_session[i];
        }
    }
    return NULL;
}

void vow_dump_kcp_info(kcp_head_t* head)
{
    TRACE(0, "************");
    TRACE(0, "####conv = %d", head->conv);
    TRACE(0, "####cmd  = %d", head->cmd);
    TRACE(0, "####wnd  = %d", head->wnd);
    TRACE(0, "####sn   = %d", head->sn);
    TRACE(0, "####una  = %d", head->una);
    TRACE(0, "####len  = %d", head->len);
    TRACE(0, "************");
}

void kcp_ack_recv()
{
    osSemaphoreRelease(wait_ack_sem);
}

void kcp_ack_wating()
{
    osSemaphoreWait(wait_ack_sem, osWaitForever);
}

void kcp_ack_init()
{
    wait_ack_sem = osSemaphoreCreate(osSemaphore(wait_ack_sem), 0);

    if (wait_ack_sem == NULL)
    {
        TRACE(0,"create event_pairing_sem fail\n");
        return ;
    }

    TRACE(0, "kcp sem init sucess");
}


void vow_kcp_update_clock(uint8_t id)
{
    uint32_t now_ms = GET_CURRENT_MS();
    ikcp_update(kcp_session[id].g_mic_pkcp, now_ms);
}

int vow_kcp_send_callback(const char *buf, int len, struct IKCPCB *kcp, void *user)
{
    uint8_t* macaddr = NULL;
    peer_info* senderInfo = vow_get_peer_info();

    if (senderInfo->recv_flags) {
        senderInfo->recv_flags = 0;
        TRACE(0,"kcp send ack: "MACSTR, MAC2STR(senderInfo->peermac));
        vow_data_xmit_v2(STATION_DEV_NUM, senderInfo->peermac , ETH_P_VOW, (const u8*)buf, len);
        return 0;
    }

    kcp_session_t* temp_session = (kcp_session_t*)user;
    macaddr = vow_get_goup_mac(temp_session->session_id);
    TRACE(0,"kcp send data"MACSTR, MAC2STR(macaddr));
    vow_data_xmit_v2(STATION_DEV_NUM, macaddr, ETH_P_VOW, (const u8*)buf, len);
    return 0;
}

int vow_parase_kcp_data(uint8_t* data, int len)
{
    int ret_size = 0;
    TRACE(0, "%s line = %d",__func__, __LINE__);

    kcp_head_t* kcp_head = (kcp_head_t* )data;
    kcp_session_t * temp_session = vow_get_kcp_object(kcp_head->conv);

    vow_dump_kcp_info(kcp_head);
    if (kcp_head->cmd == IKCP_CMD_ACK) {
        TRACE(0,"###%s recv ack",__func__);
        kcp_recv_len = len;
        memcpy(kcp_ack_buffer, data, len);
        kcp_ack_recv();
    } else if(kcp_head->cmd == IKCP_PUSH_DATA) {

        if (!temp_session) {
            return -1;
        }

        TRACE(0,"###%s recv data",__func__);
        ikcp_input(temp_session->g_mic_pkcp, (char *)data, len);
        ret_size = ikcp_recv(temp_session->g_mic_pkcp, (char *)data, len);
    } else { //IKCP_CMD_PUSH, IKCP_CMD_WASK,IKCP_CMD_WASK, IKCP_CMD_WINS
        TRACE(0,"###%s recv other cmd",__func__);
        ikcp_input(temp_session->g_mic_pkcp, (char *)data, len);
    }
    return ret_size;
}

/*
                                     1000         1001           1002
                                  session0     session1        session2
   device 0 ==> group_mac[0]          X           1000           1001
   device 1 ==> group_mac[1]         1000           X            1002
   device 2 ==> group_mac[2]         1001         1002             X
*/

void vow_kcp_send(uint8_t id, uint8_t* buf, uint32_t len)
{
    TRACE(0, "#####%s line = %d conv= %d",__func__, __LINE__, kcp_session[id].g_mic_pkcp->conv);

    LOCK_VOW_KCP();

    ikcp_send(kcp_session[id].g_mic_pkcp, (const char *)buf, len);
    osDelay(1);

    vow_kcp_update_clock(id);

    while (1) {
        TRACE(0,"######enter wairing");
        kcp_ack_wating();

        kcp_head_t* kcp_head = (kcp_head_t* )kcp_ack_buffer;
        TRACE(0, "###########recv ack conv =%d", kcp_head->conv);

        if(kcp_head->conv == kcp_session[id].g_mic_pkcp->conv) {
            ikcp_input(kcp_session[id].g_mic_pkcp, (char *)kcp_ack_buffer, kcp_recv_len);
            vow_kcp_update_clock(id);
            break;
        }
    }

    UNLOCK_VOW_KCP();
}

void kcp_task_handler(void* arg)
{
    int i = 0;
    while (1) {

        for (i=0; i < KCP_SESSION_NUM; i++) {
            vow_kcp_update_clock(i);
        }

        osDelay(5);
    }
}

static osThreadId kcp_hanle_tid;
osThreadDef(kcp_task_handler, osPriorityAboveNormal, 1, (8 * 1024), "kcp_app");

int kcp_task_init(void)
{
    TRACE(0,"enter %s %d", __func__, __LINE__);

    kcp_hanle_tid = osThreadCreate(osThread(kcp_task_handler), NULL);
    if (kcp_hanle_tid == NULL)
    {
        TRACE(0,"%s %d fail to create my_main task", __func__, __LINE__);
        return;
    }
    TRACE(0,"create kcp_hanle_tid  = 0x%X", (uint32_t)kcp_hanle_tid);

    return 0;
}


void vow_kcp_log_output(const char *log, struct IKCPCB *kcp, void *user)
{
    TRACE(0, "conv = %d ===> %s " ,kcp->conv, log);
}

int vow_kcp_init(void)
{
    TRACE(0, "%s line = %d",__func__, __LINE__);
    int i= 0;
    for(i = 0; i< KCP_SESSION_NUM; i++) {
        kcp_session[i].session_id = i;

        //create the kcp object, and the trans kcp_session as user id.
        kcp_session[i].g_mic_pkcp = ikcp_create(0x1000 + i, (void *)&kcp_session[i]);

        //set output function, it's will call by kcp protocal after add the kcp header
        kcp_session[i].g_mic_pkcp->output = vow_kcp_send_callback;
        kcp_session[i].g_mic_pkcp->writelog = vow_kcp_log_output;

        ikcp_nodelay(kcp_session[i].g_mic_pkcp, 1, 20, 2, 1);  //nodelay, interval, resend, nc
        ikcp_wndsize(kcp_session[i].g_mic_pkcp, 128, 128);
        kcp_session[i].g_mic_pkcp->logmask = ( IKCP_LOG_INPUT | IKCP_LOG_RECV | IKCP_LOG_IN_ACK);
    }

    kcp_ack_init();
    vow_mutext_lock_init();
    vow_reg_kcp_recv_callbak(vow_parase_kcp_data);
    kcp_task_init();
    TRACE(0, "%s line = %d finish",__func__, __LINE__);
    return 0;
}




#endif
