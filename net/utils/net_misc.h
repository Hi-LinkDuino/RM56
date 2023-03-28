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
#ifndef htons
#if CPU_ARCH_BIG_ENDIAN
#define htons(x) ((uint16_t)(x))
#define ntohs(x) ((uint16_t)(x))
#define htonl(x) ((uint32_t)(x))
#define ntohl(x) ((uint32_t)(x))
#else
#define htons(x) ((uint16_t)((((x) & (uint16_t)0x00ffU) << 8) | (((x) & (uint16_t)0xff00U) >> 8)))
#define ntohs(x) htons(x)
#define htonl(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                     (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                     (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                     (((x) & (uint32_t)0xff000000UL) >> 24))
#define ntohl(x) htonl(x)
#endif
#endif

static inline int net_is_zero_mac(const unsigned char *addr)
{
    return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

static inline bool net_is_multicast_mac(const unsigned char * addr)
{
	return addr[0] & 0x01;
}

static inline bool net_is_valid_mac(const unsigned char * addr)
{
	return !net_is_zero_mac(addr) && !net_is_multicast_mac(addr);
}

static inline bool bt_is_valid_mac(const unsigned char * addr)
{
	return !net_is_zero_mac(addr) && !(addr[5] & 0x01);
}