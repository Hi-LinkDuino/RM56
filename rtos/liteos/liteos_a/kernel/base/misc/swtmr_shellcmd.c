/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "los_config.h"
#ifdef LOSCFG_SHELL_CMD_DEBUG
#include "stdlib.h"
#include "los_swtmr_pri.h"
#include "shcmd.h"
#include "shell.h"


#define SWTMR_STRLEN  12

LITE_OS_SEC_DATA_MINOR STATIC CHAR g_shellSwtmrMode[][SWTMR_STRLEN] = {
    "Once",
    "Period",
    "NSD",
    "OPP",
};

LITE_OS_SEC_DATA_MINOR STATIC CHAR g_shellSwtmrStatus[][SWTMR_STRLEN] = {
    "UnUsed",
    "Created",
    "Ticking",
};

STATIC VOID OsPrintSwtmrMsg(const SWTMR_CTRL_S *swtmr)
{
    UINT32 ticks = 0;
    (VOID)LOS_SwtmrTimeGet(swtmr->usTimerID, &ticks);

    PRINTK("0x%08x  "
           "%-7s  "
           "%-6s   "
           "%-6u   "
           "%-6u  "
           "0x%08x  "
           "%p\n",
           swtmr->usTimerID % LOSCFG_BASE_CORE_SWTMR_LIMIT,
           g_shellSwtmrStatus[swtmr->ucState],
           g_shellSwtmrMode[swtmr->ucMode],
           swtmr->uwInterval,
           ticks,
           swtmr->uwArg,
           swtmr->pfnHandler);
}

STATIC INLINE VOID OsPrintSwtmrMsgHead(VOID)
{
    PRINTK("\r\nSwTmrID     State    Mode    Interval  Count   Arg         handlerAddr\n");
    PRINTK("----------  -------  ------- --------- ------- ----------  --------\n");
}

LITE_OS_SEC_TEXT_MINOR UINT32 OsShellCmdSwtmrInfoGet(INT32 argc, const UINT8 **argv)
{
#define OS_ALL_SWTMR_MASK 0xffffffff
    SWTMR_CTRL_S *swtmr = g_swtmrCBArray;
    SWTMR_CTRL_S *swtmr1 = g_swtmrCBArray;
    UINT16 index;
    size_t timerID;
    UINT16 num = 0;
    CHAR *endPtr = NULL;

    if (argc > 1) {
        PRINTK("\nUsage: swtmr [ID]\n");
        return OS_ERROR;
    }

    if (argc == 0) {
        timerID = OS_ALL_SWTMR_MASK;
    } else {
        timerID = strtoul((CHAR *)argv[0], &endPtr, 0);
        if ((endPtr == NULL) || (*endPtr != 0) || (timerID > LOSCFG_BASE_CORE_SWTMR_LIMIT)) {
            PRINTK("\nswtmr ID can't access %s.\n", argv[0]);
            return OS_ERROR;
        }
    }

    for (index = 0; index < LOSCFG_BASE_CORE_SWTMR_LIMIT; index++, swtmr1++) {
        if (swtmr1->ucState == 0) {
            num = num + 1;
        }
    }

    if (num == LOSCFG_BASE_CORE_SWTMR_LIMIT) {
        PRINTK("\r\nThere is no swtmr was created!\n");
        return OS_ERROR;
    }

    if (timerID == OS_ALL_SWTMR_MASK) {
        for (index = 0; index < LOSCFG_BASE_CORE_SWTMR_LIMIT; index++, swtmr++) {
            if (swtmr->ucState != 0) {
                OsPrintSwtmrMsgHead();
                OsPrintSwtmrMsg(swtmr);
            }
        }
    } else {
        for (index = 0; index < LOSCFG_BASE_CORE_SWTMR_LIMIT; index++, swtmr++) {
            if ((timerID == (size_t)(swtmr->usTimerID % LOSCFG_BASE_CORE_SWTMR_LIMIT)) && (swtmr->ucState != 0)) {
                OsPrintSwtmrMsgHead();
                OsPrintSwtmrMsg(swtmr);
                return LOS_OK;
            }
        }
        PRINTK("\r\nThe SwTimerID is not exist.\n");
    }
    return LOS_OK;
}

SHELLCMD_ENTRY(swtmr_shellcmd, CMD_TYPE_EX, "swtmr", 1, (CmdCallBackFunc)OsShellCmdSwtmrInfoGet);

#endif /* LOSCFG_SHELL */
