/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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


#ifndef _AT_H_
#define _AT_H_

#include "hal_trace.h"
#ifdef __NET_TELNET_SERVER_SUPPORT__
#include "telnetserver.h"
#endif

#ifdef __NET_TELNET_SERVER_SUPPORT__
#define cmd_printf(handler, fmt, ...) do {\
    printf(fmt, ##__VA_ARGS__);\
    telnet_printf(handler, fmt, ##__VA_ARGS__);\
} while (0)
#else
#define cmd_printf(handler, fmt, ...) do {\
    printf(fmt, ##__VA_ARGS__);\
} while (0)
#endif

//#define AT_DEBUG
#ifdef AT_DEBUG
#define AT_PRINT(fmt, ...) do {\
    cmd_printf(NULL, fmt, ##__VA_ARGS__);\
} while (0)
#else
#define AT_PRINT(fmt, ...)
#endif


typedef enum{
	ERROR_OK	=	 0,
	ERROR_CMD	=	-1,
	ERROR_ABORT =	-2,
	ERROR_FAILE =	-3,
	ERROR_ARG = 	-4,
}ERR_TYPE;

typedef enum{
    OTA_GET_BOOT_INFO,
	OTA_GET_BOOT_ZONE_NUM,
	OTA_SET_BOOT_ZONE_NUM,
	OTA_SET_ZONE_INFO,
	OTA_SET_BOOT_INFO_INVALIDE,
	OTA_COPY_ZONE,
	AG_OTA_COPY_ZONE
}OTA_ACTION;

#define RESP(handler, fmt, ...)          cmd_printf(handler, fmt, ##__VA_ARGS__)
#define RESP_OK(handler)               cmd_printf(handler, "+ok\r\n")
#define RESP_OK_EQU(handler, fmt, ...)   cmd_printf(handler, "+ok=" fmt, ##__VA_ARGS__)
#define RESP_ERROR(handler, err_type)    cmd_printf(handler, "+error=%d\r\n", err_type)
void str2mac(const char * szMac,unsigned char * pMac);
void add_factory_cmd();

#endif
