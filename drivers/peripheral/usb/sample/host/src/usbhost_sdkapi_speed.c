/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "usbhost_sdkapi_speed.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <osal_sem.h>
#include <osal_thread.h>
#include <inttypes.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "usb_ddk_interface.h"
#include "hdf_usb_pnp_manage.h"

#define HDF_LOG_TAG   USB_HOST_ACM

static unsigned int g_speedFlag = 0;
static uint64_t g_recv_count = 0;
static uint64_t g_send_count = 0;
static uint64_t g_byteTotal = 0;
static bool g_writeOrRead = TEST_WRITE;
static bool g_printData = false;

static void AcmTestBulkCallback(struct UsbRequest *req);
static int32_t SerialBegin(struct AcmDevice *acm);

static int32_t AcmDbAlloc(struct AcmDevice *acm)
{
    int32_t i, dbn;
    struct AcmDb *db = NULL;
    dbn = 0;
    i = 0;
    for (;;) {
        db = &acm->db[dbn];
        if (!db->use) {
            db->use = 1;
            db->len = 0;
            return dbn;
        }
        dbn = (dbn + 1) % TEST_CYCLE;
        if (++i >= TEST_CYCLE)
            return -1;
    }
}

static int32_t AcmDbIsAvail(struct AcmDevice *acm)
{
    int32_t i, n;
    n = TEST_CYCLE;
    for (i = 0; i < TEST_CYCLE; i++)
        n -= acm->db[i].use;
    return n;
}

static UsbInterfaceHandle *InterfaceIdToHandle(const struct AcmDevice *acm, uint8_t id)
{
    UsbInterfaceHandle *devHandle = NULL;

    if (id == 0xFF) {
        devHandle = acm->ctrDevHandle;
    } else {
        for (int32_t i = 0; i < acm->interfaceCnt; i++) {
            if (acm->iface[i]->info.interfaceIndex == id) {
                devHandle = acm->devHandle[i];
                break;
            }
        }
    }
    return devHandle;
}

static int32_t AcmStartDb(struct AcmDevice *acm,
    struct AcmDb *db, struct UsbPipeInfo *pipe)
{
    int32_t rc;
    rc = UsbSubmitRequestAsync(db->request);
    if (rc < 0) {
        HDF_LOGE("UsbSubmitRequestAsync failed, ret=%d \n", rc);
        db->use = 0;
    }
    return rc;
}

static int32_t AcmDataBufAlloc(struct AcmDevice *acm)
{
    int32_t i;
    struct AcmDb *db;
    for (db = &acm->db[0], i = 0; i < TEST_CYCLE; i++, db++) {
        db->buf = OsalMemCalloc(acm->dataSize);
        if (!db->buf) {
            while (i > 0) {
                --i;
                --db;
                OsalMemFree(db->buf);
                db->buf = NULL;
            }
            return -HDF_ERR_MALLOC_FAIL;
        } else {
            memset_s(db->buf, acm->dataSize, 'a', acm->dataSize);
            db->instance = acm;
        }
    }
    return 0;
}


static void AcmTestBulkCallback(struct UsbRequest *req)
{
    if (req == NULL) {
        printf("req is null\r\n");
        return;
    }
    int32_t status = req->compInfo.status;
    struct AcmDb *db  = (struct AcmDb *)req->compInfo.userData;
    struct itimerval new_value, old_value;

    if (status == 0) {
        if (g_byteTotal == 0) {
            new_value.it_value.tv_sec = TEST_PRINT_TIME;
            new_value.it_value.tv_usec = 0;
            new_value.it_interval.tv_sec = TEST_PRINT_TIME;
            new_value.it_interval.tv_usec = 0;
            setitimer(ITIMER_REAL, &new_value, &old_value);
        }
        g_recv_count++;
        g_byteTotal += req->compInfo.actualLength;
    } else {
        printf("error status=%d\r\n", status);
    }

    if (g_printData) {
        for (unsigned int i = 0; i < req->compInfo.actualLength; i++) {
            printf("%c", req->compInfo.buffer[i]);
        }
        fflush(stdout);
    } else if (g_recv_count % TEST_RECV_COUNT == 0) {
        printf("#");
        fflush(stdout);
    }

    db->use = 0;
    if (!g_speedFlag) {
        if (SerialBegin(db->instance) != HDF_SUCCESS) {
            HDF_LOGW("%s:%d SerialBegin error!", __func__, __LINE__);
        }
        g_send_count++;
    }

    return;
}

static int32_t SerialBegin(struct AcmDevice *acm)
{
    uint32_t size = acm->dataSize;
    int32_t ret;
    struct AcmDb *db = NULL;
    int32_t dbn;
    if (AcmDbIsAvail(acm)) {
        dbn = AcmDbAlloc(acm);
    } else {
        HDF_LOGE("no buf\n");
        return 0;
    }
    if (dbn < 0) {
        HDF_LOGE("AcmDbAlloc failed\n");
        return HDF_FAILURE;
    }
    db = &acm->db[dbn];
    db->len = acm->dataSize;
    ret = AcmStartDb(acm, db, NULL);
    return size;
}


static struct UsbInterface *GetUsbInterfaceById(const struct AcmDevice *acm,
    uint8_t interfaceIndex)
{
    return UsbClaimInterface(acm->session, acm->busNum, acm->devAddr, interfaceIndex);
}

static struct UsbPipeInfo *EnumePipe(const struct AcmDevice *acm,
    uint8_t interfaceIndex, UsbPipeType pipeType, UsbPipeDirection pipeDirection)
{
    uint8_t i;
    int32_t ret;
    struct UsbInterfaceInfo *info = NULL;
    UsbInterfaceHandle *interfaceHandle = NULL;
    if (USB_PIPE_TYPE_CONTROL == pipeType) {
        info = &acm->ctrIface->info;
        interfaceHandle = acm->ctrDevHandle;
    } else {
        info = &acm->iface[interfaceIndex]->info;
        interfaceHandle = InterfaceIdToHandle(acm, info->interfaceIndex);
    }

    for (i = 0;  i <= info->pipeNum; i++) {
        struct UsbPipeInfo p;
        ret = UsbGetPipeInfo(interfaceHandle, info->curAltSetting, i, &p);
        if (ret < 0) {
            continue;
        }
        if ((p.pipeDirection == pipeDirection) && (p.pipeType == pipeType)) {
            struct UsbPipeInfo *pi = OsalMemCalloc(sizeof(*pi));
            if (pi == NULL) {
                HDF_LOGE("%s: Alloc pipe failed", __func__);
                return NULL;
            }
            p.interfaceId = info->interfaceIndex;
            *pi = p;
            return pi;
        }
    }
    return NULL;
}

static struct UsbPipeInfo *GetPipe(const struct AcmDevice *acm,
    UsbPipeType pipeType, UsbPipeDirection pipeDirection)
{
    uint8_t i;
    if (acm == NULL) {
        HDF_LOGE("%s: invalid parmas", __func__);
        return NULL;
    }
    for (i = 0; i < acm->interfaceCnt; i++) {
        struct UsbPipeInfo *p = NULL;
        if (!acm->iface[i]) {
            continue;
        }
        p = EnumePipe(acm, i, pipeType, pipeDirection);
        if (p == NULL) {
            continue;
        }
        return p;
    }
    return NULL;
}


void SignalHandler(int32_t signo)
{
    static uint32_t sigCnt = 0;
    struct itimerval new_value, old_value;
    double speed = 0;
    switch (signo) {
        case SIGALRM:
            sigCnt++;
            if (sigCnt * TEST_PRINT_TIME >= TEST_TIME) {
                g_speedFlag = 1;
                break;
            }
            speed = (g_byteTotal * TEST_FLOAT_COUNT) / (sigCnt * TEST_PRINT_TIME * TEST_BYTE_COUNT * TEST_BYTE_COUNT);
            printf("\nSpeed:%f MB/s\n", speed);
            new_value.it_value.tv_sec = TEST_PRINT_TIME;
            new_value.it_value.tv_usec = 0;
            new_value.it_interval.tv_sec = TEST_PRINT_TIME;
            new_value.it_interval.tv_usec = 0;
            setitimer(ITIMER_REAL, &new_value, &old_value);
            break;
        case SIGINT:
            g_speedFlag = 1;
            break;
        default:
            break;
    }
}

static void ShowHelp(const char *name)
{
    printf(">> usage:\n");
    printf(">>      %s [<busNum> <devAddr>]  <ifaceNum> <w>/<r> [printdata]> \n", name);
    printf("\n");
}

struct AcmDevice *CheckParam(int32_t argc, const char *argv[])
{
    int32_t busNum = 1;
    int32_t devAddr = 2;
    int32_t ifaceNum = 3;
    struct AcmDevice *acm = NULL;

    if (argc == TEST_SIX_TYPE) {
        busNum = atoi(argv[TEST_ONE_TYPE]);
        devAddr = atoi(argv[TEST_TWO_TYPE]);
        ifaceNum = atoi(argv[TEST_THREE_TYPE]);
        g_writeOrRead = (strncmp(argv[TEST_FOUR_TYPE], "r", TEST_ONE_TYPE)) ? TEST_WRITE : TEST_READ;
        if (g_writeOrRead == TEST_READ) {
            g_printData = (strncmp(argv[TEST_FIVE_TYPE], "printdata", TEST_ONE_TYPE)) ? false : true;
        }
    } else if (argc == TEST_FIVE_TYPE) {
        busNum = atoi(argv[TEST_ONE_TYPE]);
        devAddr = atoi(argv[TEST_TWO_TYPE]);
        ifaceNum = atoi(argv[TEST_THREE_TYPE]);
        g_writeOrRead = (strncmp(argv[TEST_FOUR_TYPE], "r", TEST_ONE_TYPE)) ? TEST_WRITE : TEST_READ;
    } else if (argc == TEST_THREE_TYPE) {
        ifaceNum = atoi(argv[TEST_ONE_TYPE]);
        g_writeOrRead = (strncmp(argv[TEST_TWO_TYPE], "r", TEST_ONE_TYPE)) ? TEST_WRITE : TEST_READ;
    } else {
        printf("Error: parameter error!\n\n");
        ShowHelp(argv[0]);
        goto END;
    }

    acm = (struct AcmDevice *)OsalMemCalloc(sizeof(*acm));
    if (acm == NULL) {
        HDF_LOGE("%s: Alloc usb serial device failed", __func__);
        goto END;
    }
    acm->busNum = busNum;
    acm->devAddr = devAddr;
    acm->interfaceCnt = 1;
    acm->interfaceIndex[0] = ifaceNum;
END:
    return acm;
}

int32_t InitUsbDdk(struct AcmDevice *acm)
{
    int32_t ret;

    ret = UsbInitHostSdk(NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: UsbInitHostSdk failed", __func__);
        ret = HDF_ERR_IO;
        goto END;
    }

    for (int32_t i = 0; i < acm->interfaceCnt; i++) {
        acm->iface[i] = GetUsbInterfaceById((const struct AcmDevice *)acm, acm->interfaceIndex[i]);
    }

    for (int32_t i = 0; i < acm->interfaceCnt; i++) {
        if (acm->iface[i]) {
            acm->devHandle[i] = UsbOpenInterface(acm->iface[i]);
            if (acm->devHandle[i] == NULL) {
                HDF_LOGE("%s: UsbOpenInterface null", __func__);
            }
        } else {
            ret = HDF_FAILURE;
            goto END;
        }
    }
    if (g_writeOrRead == TEST_WRITE) {
        acm->dataPipe = GetPipe(acm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_OUT);
    } else {
        acm->dataPipe = GetPipe(acm, USB_PIPE_TYPE_BULK, USB_PIPE_DIRECTION_IN);
    }
    if (acm->dataPipe == NULL) {
        HDF_LOGE("dataPipe is NULL\n");
    }

    acm->dataSize = TEST_LENGTH;
    if (AcmDataBufAlloc(acm) < 0) {
        HDF_LOGE("%s:%d AcmDataBufAlloc fail", __func__, __LINE__);
    }
END:
    return ret;
}

int32_t FillRequest(struct AcmDevice *acm)
{
    for (int32_t i = 0; i < TEST_CYCLE; i++) {
        struct AcmDb *snd = &(acm->db[i]);
        snd->request = UsbAllocRequest(InterfaceIdToHandle(acm, acm->dataPipe->interfaceId), 0, acm->dataSize);
        if (snd->request == NULL) {
            HDF_LOGE("%s:%d snd request fail", __func__, __LINE__);
        }
        int32_t rc;
        acm->transmitting++;
        struct UsbRequestParams parmas = {};
        parmas.interfaceId = acm->dataPipe->interfaceId;
        parmas.pipeAddress = acm->dataPipe->pipeAddress;
        parmas.pipeId = acm->dataPipe->pipeId;
        parmas.callback = AcmTestBulkCallback;
        parmas.requestType = USB_REQUEST_PARAMS_DATA_TYPE;
        parmas.timeout = USB_CTRL_SET_TIMEOUT;
        parmas.dataReq.numIsoPackets = 0;
        parmas.userData = (void *)snd;
        parmas.dataReq.length = acm->dataSize;
        parmas.dataReq.buffer = snd->buf;
        parmas.dataReq.directon = (acm->dataPipe->pipeDirection >> USB_PIPE_DIR_OFFSET) & 0x1;
        snd->dbNum = acm->transmitting;
        rc = UsbFillRequest(snd->request, InterfaceIdToHandle(acm, acm->dataPipe->interfaceId), &parmas);
        if (rc != HDF_SUCCESS) {
            HDF_LOGE("%s:UsbFillRequest faile,ret=%d \n", __func__, rc);
            return rc;
        }
    }

    return HDF_SUCCESS;
}

int32_t main(int32_t argc, char *argv[])
{
    struct timeval time;
    int32_t i;
    int32_t ret;
    struct AcmDevice *acm = NULL;

    acm = CheckParam(argc, (const char **)argv);
    if (acm == NULL) {
        ret = HDF_FAILURE;
        goto END;
    }

    ret = InitUsbDdk(acm);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    ret = FillRequest(acm);
    if (ret != HDF_SUCCESS) {
        goto END;
    }

    if (signal(SIGINT, SignalHandler) == SIG_ERR) {
        HDF_LOGE("signal SIGINT failed");
        return HDF_FAILURE;
    }
    if (signal(SIGALRM, SignalHandler) == SIG_ERR) {
        HDF_LOGE("signal SIGINT failed");
        return HDF_FAILURE;
    }
    gettimeofday(&time, NULL);

    printf("test SDK API [%s]\n", g_writeOrRead?"write":"read");
    for (i = 0; i < TEST_CYCLE; i++) {
        if (SerialBegin(acm) != HDF_SUCCESS) {
            HDF_LOGW("%s:%d SerialBegin error!", __func__, __LINE__);
        }
        g_send_count++;
    }

    while (!g_speedFlag) {
        OsalMSleep(TEST_SLEEP_TIME);
    }
END:
    if (ret != HDF_SUCCESS) {
        printf("please check whether usb drv so is existing or not,like acm,ecm,if not, remove it and test again!\n");
    }
    return ret;
}
