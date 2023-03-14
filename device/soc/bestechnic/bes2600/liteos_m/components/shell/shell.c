/*
 * Copyright (c) 2021 bestechnic (Shanghai) Technologies CO., LIMITED.
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
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "shmsg.h"
#include "uart_if.h"
#include "hal_trace.h"

#define SHELL_UART_ID 0

extern void RegisterCustomShellCmd(void);
extern int rf_parse_command(char *argstr);
extern int btrf_parse_command(char *argstr);
extern int run_command (void *handler, char *cmd);
extern UINT32 OsShellInit(VOID);

static void ShellTask(void *unused)
{
    OsShellInit();
    RegisterCustomShellCmd(); // must after OsShellInit
    DevHandle dev = UartOpen(SHELL_UART_ID);
    if (dev == NULL) {
        printf("failed to open uart%d!\n", SHELL_UART_ID);
        return;
    }
    printf("%s: uart%d open succeed\n", __func__, SHELL_UART_ID);
    char buf[128] = {0};
    int i = 0;
    while (1) {
        if (UartRead(dev, (uint8_t *)&buf[i], 1) == 1) {
            if (buf[i] == '\r' || buf[i] == '\n') {
                if (i == 0) {
                    continue;
                }
                buf[i] = '\0';
                printf("%s: %s\n", __func__, buf);
                if (strncmp(buf, "besphy", 6) == 0) {
                    if (rf_parse_command(buf) < 0) {
                        printf("rf_parse_command failed\r\n");
                    }
                } else if (strncmp(buf, "btrf", 4) == 0) {
                    if (btrf_parse_command(buf) < 0) {
                        printf("btrf_parse_command failed\r\n");
                    }
                } else if (strncmp(buf, "AT", 2) == 0) {
                    if (run_command(NULL, buf) < 0) {
                        printf("run_command failed\r\n");
                    }
                } else {
                    ExecCmdline(buf);
                }
                i = 0;
            } else {
                i = (i + 1) % sizeof(buf);
            }
        } else {
            osDelay(1);
        }
    }
    UartClose(dev);
}

void ShellEntry(void)
{
    osThreadAttr_t attr = {0};
    attr.name = "Shell";
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)ShellTask, NULL, &attr) == NULL) {
        printf("failed to create ShellTask task!\n");
    }
}

SYS_SERVICE_INIT(ShellEntry);
