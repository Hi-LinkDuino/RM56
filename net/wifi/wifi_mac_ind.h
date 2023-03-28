/***************************************************************************
 *
 * Copyright 2015-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __WIFI_MAC_IND_H__
#define __WIFI_MAC_IND_H__

#include "bwifi_hal.h"
#include "skbuff.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct wifi_init_param wifi_param;
typedef void (*wifi_mac_alive_handler)(int period, bool low_memory);
typedef int (*wifi_mac_rx_data_handler)(struct sk_buff *skb);
#ifdef __SNIFFER_MODE__
typedef void (*wifi_mac_rx_sniffer_handler)(struct sk_buff *skb, uint8 rssi);
#endif

extern void wifi_mac_reg_rx_event_handler(evt_recv_handler cb);
extern void wifi_mac_unreg_rx_event_handler();
extern void wifi_mac_reg_alive_handler(wifi_mac_alive_handler cb);
extern void wifi_mac_unreg_alive_handler();
extern void wifi_mac_reg_rx_data_handler(wifi_mac_rx_data_handler cb);
extern void wifi_mac_unreg_rx_data_handler();

#ifdef CSI_REPORT
extern void wifi_mac_reg_rx_csi_handler(csi_recv_handler cb);
extern void wifi_mac_unreg_rx_csi_handler();
#endif

#ifdef __SNIFFER_MODE__
extern void wifi_mac_reg_rx_sniffer_handler(wifi_mac_rx_sniffer_handler cb);
extern void wifi_mac_unreg_rx_sniffer_handler();
#endif

extern void wifi_mac_ind_event(BWIFI_SYSTEM_EVENT_T *event);
#ifdef CSI_REPORT
extern void wifi_mac_ind_csi(uint8 *data, int len);
#endif
extern void wifi_mac_ind_alive(int period, bool low_memory);
extern int wifi_mac_ind_data(struct sk_buff *skb);
#ifdef __SNIFFER_MODE__
extern void wifi_mac_ind_sniffer_pkt(struct sk_buff *skb, uint8 rssi);
#endif

#ifdef __cplusplus
}
#endif

#endif

