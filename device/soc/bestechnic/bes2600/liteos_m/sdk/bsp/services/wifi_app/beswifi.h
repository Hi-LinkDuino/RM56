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

#ifndef BESWIFI_H
#define BESWIFI_H

#ifdef __cplusplus
extern "C" {
#endif


int bes_wifi_init(void);

int bes_wifi_connect(const char *ssid, const char *passwd, unsigned char *bssid);


#ifdef __cplusplus
}
#endif
#endif /* BESWIFI_H */
