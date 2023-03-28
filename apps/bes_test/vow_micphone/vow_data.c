#include "vow_data.h"
#include "vow_alsa.h"
#include "bwifi_interface.h"
#include "if_ether.h"

#ifdef __VOW_MIC_KCP__
#include "vow_mic_kcp.h"
static int kcp_input_len = -1;
#endif

void* data_alsa_handle =NULL;
static uint8_t multicast_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};//{0x01, 0x00, 0x5e, 0x2a, 0x2a, 0x2a};

static MIC_DATA_T send_mic_data = {0};

static uint8_t group_mac[MAX_STA][ETH_ALEN] = {
    {0x00,0x80,0x11,0x11,0x11,0x11},
    {0x00,0x80,0x22,0x22,0x22,0x22},
    {0x00,0x80,0x33,0x33,0x33,0x33},
};
static uint8_t g_peer_num = 0;
static peer_info peer_data = {0};

static vow_kcp_callback__t vow_kcp_recv_hook = NULL;

void vow_reg_kcp_recv_callbak(vow_kcp_callback__t cb)
{
    vow_kcp_recv_hook = cb;
}


#if LWIP_ETHERNETIF
extern void ethernetif_input(u16 devnum, void *p_buf, int size);
#endif

#define AUDIO_SAMPLE_RATE    16000
#define AUDIO_CH             1
#define AUDIO_DEPTH          16


uint8_t is_mac_address_exist(uint8_t* macaddr)
{
    int i = 0;
    for (i = 0; i< MAX_STA; i++) {
        if (memcmp(group_mac[i], macaddr, ETH_ALEN) == 0) {
            return 1;
        }
    }
    TRACE(0, "The mac is not in group, insert into list");
    return 0;
}

uint8_t* vow_get_goup_mac(uint8_t id)
{
    return group_mac[id];
}

void data_alsa_playback_init()
{
    data_alsa_handle = sink_alsa_open(AUDIO_SAMPLE_RATE, AUDIO_CH, AUDIO_DEPTH);
    sink_alsa_start(data_alsa_handle);
    TRACE(0,"##########alsa init finish");
}

int vow_data_xmit_v2(u8 dev_idx, const u8 *dest, u16 proto, const u8 *data, size_t data_len)
{
    int ret = 0;
    unsigned char **tx_buffer = NULL;
    struct ethhdr *p_ethhdr = NULL;
    uint8_t macaddr[6] = { 0 };

    bwifi_get_own_mac(macaddr);
    //TRACE(0,"%02x:%02x:%02x:%02x:%02x:%02x", macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
    tx_buffer = bwifi_get_data_buf();
    if(tx_buffer == NULL){
        TRACE(0, "%s malloc tx buffer fail",__func__);
        return -1;
    }

    p_ethhdr = (struct ethhdr *)*tx_buffer;
    memcpy((void*)&p_ethhdr->h_dest, dest, ETH_ALEN);
    memcpy((void*)&p_ethhdr->h_source,  macaddr, ETH_ALEN);
    p_ethhdr->h_proto = htons(proto);
    memcpy((char *)(p_ethhdr + 1), data, data_len);

    ret = bwifi_send_data_buf(dev_idx, tx_buffer, data_len);

    return ret;

}


void vow_send_data(uint8_t* data , size_t len)
{
    int i = 0;
    uint8_t macaddr[6] = { 0 };
    bwifi_get_own_mac(macaddr);

    memset(&send_mic_data, 0, MIC_PAYLOAD);
    send_mic_data.cmd_type = MIC_TYPE_DATA;
    send_mic_data.data_len = len;
    memcpy(send_mic_data.data, data, len);
    for (i = 0; i < MAX_STA; i++) {
#ifdef __VOW_MIC_KCP__
    if (memcmp(macaddr, group_mac[i], ETH_ALEN) ==0) {
        continue;
    }
    vow_kcp_send(i, (uint8_t*)&send_mic_data, len + sizeof(MIC_HEAD_T));
#else
    if (memcmp(macaddr, group_mac[i], ETH_ALEN) ==0) {
       continue;
    }
    vow_data_xmit_v2(STATION_INDEX, group_mac[i], ETH_P_VOW, (u8 *)&send_mic_data, len + sizeof(MIC_HEAD_T));
#endif
    }
}

void vow_send_cmd(uint8_t* data , size_t len)
{
    uint8_t macaddr[ETH_ALEN] = {0};
    bwifi_get_own_mac(macaddr);

    send_mic_data.cmd_type = MIC_TYPE_CMD;
    send_mic_data.data_len = ETH_ALEN;
    memcpy(send_mic_data.data, macaddr, ETH_ALEN);

    vow_data_xmit_v2(STATION_INDEX, multicast_addr, ETH_P_VOW, (uint8_t*)&send_mic_data, ETH_ALEN + sizeof(MIC_HEAD_T));
}

uint8_t vow_get_peer_num()
{
    return g_peer_num;
}

peer_info* vow_get_peer_info()
{
    return &peer_data;
}


void vow_data_recv(uint8_t dev_num, void *data, int len)
{
    MIC_DATA_T *frame = NULL;
    struct ethhdr* ethhdr = (struct ethhdr*)data;
    uint8_t* skip_eth_data = data + sizeof(struct ethhdr);
    TRACE(0, "%s len = %d",__func__, len);
    hal_trace_dump("0x%2x ", 1, 128, data);
    TRACE(0,"\r\n");

    if(ethhdr->h_proto != 0xBABA) {
#if LWIP_ETHERNETIF
        ethernetif_input(STATION_DEV_NUM, data, len);
        return ;
#else
//#error "must define the lwip LWIP_ETHERNETIF"
#endif
    }

#ifdef __VOW_MIC_KCP__
    peer_data.recv_flags = 1;
    memcpy(peer_data.peermac, ethhdr->h_source, ETH_ALEN);
    if (vow_kcp_recv_hook) {
        kcp_input_len = vow_kcp_recv_hook(skip_eth_data , len);
    }
    TRACE(0, "%s later kcp len = %d",__func__, kcp_input_len);
    hal_trace_dump("0x%2x ", 1, 64, skip_eth_data);
    TRACE(0,"\r\n");

    if (kcp_input_len <= 0) {
        goto EXIT;
    }

#endif

    frame = (MIC_DATA_T *)skip_eth_data;
    //todo something for data error
    if(frame->cmd_type == MIC_TYPE_DATA) {
        TRACE(0, "%s recv data len = %d ", __func__, frame->data_len);
        if (!data_alsa_handle) {
            TRACE(0,"%s alsa handle is NULL" , __func__);
            goto EXIT;
        }
        sink_alsa_write(data_alsa_handle, (uint8_t*)frame->data, frame->data_len);
    } else if (frame->cmd_type == MIC_TYPE_CMD) {
        if ( !is_mac_address_exist((uint8_t*)frame->data)) {
            memcpy(group_mac[g_peer_num++], (uint8_t*)frame->data, ETH_ALEN);
        }
    }
EXIT:
    TRACE(0,"#EXIT");
}


void vow_recv_callback()
{
    bwifi_reg_eth_input_handler(vow_data_recv);
}

#define SSID_INFO   "TP-LINK_5G_EFC3"
#define PWD_INFO    "BESsz@@2021!!"

void vow_data_handle_init()
{
    TRACE(0,"#####vow_data_handle_init");

#ifdef __VOW_MIC_KCP__
    vow_kcp_init();
#endif

    vow_recv_callback();
    data_alsa_playback_init();

#ifdef DEBUG_ONLY_RECV
    BWIFI_STATUS_T wifi_status  = BWIFI_STATUS_IDLE;

    while (wifi_status != BWIFI_STATUS_GOT_IP) {
        bwifi_connect_to_ssid((char*)SSID_INFO, NULL, 0, 0, NULL);
        wifi_status = bwifi_get_current_status();
        osDelay(500);
    }
    osDelay(3000);
#endif
}


