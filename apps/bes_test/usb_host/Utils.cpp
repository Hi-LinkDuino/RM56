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


#include "hal_trace.h"
#include "Utils.h"
#include "string.h"

void printfBytes(const char* s, const u8* data, int len)
{
    TRACE(2,"%s %d:",s,len);
    if (len > 256)
        len = 256;
    while (len-- > 0)
        TRACE(1," %02X",*data++);
    TRACE(0,"\n");
}

void printHexLine(const u8* d, int addr, int len)
{
    TRACE(1,"%04X ",addr);
    int i;
    for (i = 0; i < len; i++)
        TRACE(1,"%02X ",d[i]);
    for (;i < 16; i++)
        TRACE(0,"   ");
    char s[16+1];
    memset(s,0,sizeof(s));
    for (i = 0; i < len; i++)
    {
        int c = d[i];
        if (c < 0x20 || c > 0x7E)
            c = '.';
        s[i] = c;
    }
    TRACE(1,"%s\n",s);
}

void printHex(const u8* d, int len)
{
    int addr = 0;
    while (len)
    {
        int count = len;
        if (count > 16)
            count = 16;
        printHexLine(d+addr,addr,count);
        addr += 16;
        len -= count;
    }
}
