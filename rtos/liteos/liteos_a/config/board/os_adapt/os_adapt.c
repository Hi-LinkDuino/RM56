/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "target_config.h"
#include "los_typedef.h"

#include "stdlib.h"
#include "stdio.h"
#include "los_process_pri.h"
#ifdef LOSCFG_FS_VFS
#include "disk.h"
#endif
#include "los_rootfs.h"
#ifdef LOSCFG_SHELL
#include "shell.h"
#include "shcmd.h"
#endif

#ifdef LOSCFG_DRIVERS_MEM
#include "los_dev_mem.h"
#endif

#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_UART
#include "console.h"
#include "soc/uart.h"
#endif

#ifdef LOSCFG_DRIVERS_HDF
#include "devmgr_service_start.h"
#endif

#ifdef RTOS
#include "cmsis_os.h"
#endif

#ifdef LOSCFG_DRIVERS_NETDEV
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"

#define SLEEP_TIME_MS 60
#define NETIF_SETUP_OVERTIME 100
void net_init(void)
{
extern void tcpip_init(tcpip_init_done_fn initfunc, void *arg);
    tcpip_init(NULL, NULL);
    PRINTK("Ethernet start.");

#ifdef LOSCFG_DRIVERS_HIGMAC
    extern int ethnet_higmac_init(void);
    (void)ethnet_higmac_init();
#endif

#ifdef LOSCFG_DRIVERS_HIETH_SF
    extern int ethnet_hieth_init(void);
    (void)ethnet_hieth_init();
    struct netif *pnetif = NULL;

    extern void get_defaultNetif(struct netif **pnetif);
    get_defaultNetif(&pnetif);

    (void)netifapi_netif_set_up(pnetif);
    unsigned int overtime = 0;
    do {
        LOS_Msleep(SLEEP_TIME_MS);
        overtime++;
        if (overtime > NETIF_SETUP_OVERTIME) {
            PRINTK("netif_is_link_up overtime!\n");
            break;
        }
    } while (netif_is_link_up(pnetif) == 0);
#endif

    extern struct netif *VirtnetInit(void);
    struct netif *p = VirtnetInit();
    if (p) {
        netif_set_default(p);
        (void)netifapi_netif_set_up(p);
    }
}
#endif

#ifdef LOSCFG_DRIVERS_MEM
int mem_dev_register(void)
{
    return DevMemRegister();
}
#endif

extern int a7_dsp_main(void);
static void dsp_main_thread(const void *argument);
osThreadDef(dsp_main_thread, osPriorityAboveNormal, 1, (64*1024), "dsp_main_thread");
static void dsp_main_thread(const void *argument)
{
    PRINTK("%s: enter\n", __func__);
    a7_dsp_main();
    PRINTK("%s: exit\n", __func__);
    osThreadExit();
}

void SystemInit(void)
{
#ifdef LOSCFG_FS_PROC
    PRINTK("proc fs init ...\n");
    extern void ProcFsInit(void);
    ProcFsInit();
#endif

#ifdef LOSCFG_DRIVERS_MEM
    PRINTK("mem dev init ...\n");
    extern int mem_dev_register(void);
    mem_dev_register();
#endif

    PRINTK("Date:%s.\n", __DATE__);
    PRINTK("Time:%s.\n", __TIME__);

#ifdef LOSCFG_DRIVERS_NETDEV
    PRINTK("net init ...\n");
    net_init();
    PRINTK("\n************************************************************\n");
#endif

#ifdef LOSCFG_DRIVERS_HDF
    PRINTK("DeviceManagerStart start ...\n");
    if (DeviceManagerStart()) {
        PRINT_ERR("No drivers need load by hdf manager!");
    }
    PRINTK("DeviceManagerStart end ...\n");
#endif

#ifdef LOSCFG_PLATFORM_ROOTFS
    PRINTK("OsMountRootfs start ...\n");
    OsMountRootfs();
    PRINTK("OsMountRootfs end ...\n");
#endif

#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_UART
    PRINTK("virtual_serial_init start ...\n");
    if (virtual_serial_init(TTY_DEVICE) != 0) {
        PRINT_ERR("virtual_serial_init failed");
    }
    PRINTK("virtual_serial_init end ...\n");
    PRINTK("system_console_init start ...\n");
    if (system_console_init(SERIAL) != 0) {
        PRINT_ERR("system_console_init failed\n");
    }
    PRINTK("system_console_init end ...\n");
#endif
    PRINTK("OsUserInitProcess start ...\n");
    if (OsUserInitProcess()) {
        PRINT_ERR("Create user init process faialed!\n");
        return;
    }

    /* init dsp main thread */
    osThreadId thread_id = osThreadCreate(osThread(dsp_main_thread), NULL);
    if(thread_id == NULL) {
        PRINTK("Create dsp_main_thread fail\n");
    }

    PRINTK("OsUserInitProcess end ...\n");
    return;
}
