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

#include "audio_manager.h"
#include <dlfcn.h>
#include <limits.h>
#include <pthread.h>
#include <securec.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "audio_proxy_manager.h"
#include "audio_types.h"
#include "hdf_base.h"
#include "inttypes.h"
#include "hdf_io_service_if.h"
#include "hdf_service_status.h"
#include "ioservstat_listener.h"
#include "svcmgr_ioservice.h"
#include "pnp_message_report.h"

#define LOG_FUN_ERR(fmt, arg...) do { \
        printf("%s: [%s]: [%d]:[ERROR]:" fmt"\n", __FILE__, __func__, __LINE__, ##arg); \
    } while (0)

#define AUDIO_CHANNELCOUNT 2
#define AUDIO_SAMPLE_RATE_48K 48000
#define PATH_LEN 256

#define BUFFER_PERIOD_SIZE (4 * 1024)
#define DEEP_BUFFER_RENDER_PERIOD_SIZE 4096
#define DEEP_BUFFER_RENDER_PERIOD_COUNT 8
#define INT_32_MAX 0x7fffffff
#define PERIOD_SIZE 1024
#define AUDIO_BUFF_SIZE (1024 * 16)
#define PCM_8_BIT 8
#define PCM_16_BIT 16
#define AUDIO_TOTALSIZE_15M (1024 * 15)
#define AUDIO_RECORD_INTERVAL_512KB 512
#define MAX_SUPPORT_ADAPTER_NUM 8
#define FILE_CAPTURE_SIZE (1024 * 1024 * 3) // 3M
#define BUFFER_LEN 256
#define EXT_PARAMS_MAXLEN 107
#define ONE_MS 1000

struct StrParaCapture {
    struct AudioCapture *capture;
    FILE *file;
    struct AudioSampleAttributes attrs;
    uint64_t *replyBytes;
    char *frame;
    int32_t bufferSize;
};
struct AudioAdapter *g_adapter = NULL;
struct AudioDeviceDescriptor g_devDesc;
struct AudioSampleAttributes g_attrs;
struct AudioCapture *g_capture = NULL;
struct AudioManager *g_manager = NULL;
static struct AudioProxyManager *g_proxyManager = NULL;
static struct StrParaCapture g_str;
void *g_captureHandle;

pthread_t g_tids;
FILE *g_file;
char *g_frame;
void *g_handle;
char g_path[256];
#ifdef AUDIO_HAL_USER
void *g_sdkHandle;
int (*g_sdkInitSp)() = NULL;
void (*g_sdkExitSp)() = NULL;
#endif

enum AudioCaptureMode {
    CAPTURE_POLL   = 1,
    CAPTURE_INTERUPT,
};

int g_CaptureModeFlag = CAPTURE_POLL;

#ifndef __LITEOS__
int g_receiveFrameCount = 0;
uint64_t g_totalSize = 0;
struct ISvcMgrIoservice *g_servmgr = NULL;
struct ServiceStatusListener *g_listener = NULL;
#endif

enum CaptureMenuId {
    CAPTURE_START = 1,
    CAPTURE_STOP,
    CAPTURE_RESUME,
    CAPTURE_PAUSE,
    SET_CAPTURE_VOLUME,
    SET_CAPTURE_GAIN,
    SET_CAPTURE_MUTE,
    SET_CAPTURE_ATTRIBUTES,
    SET_CAPTURE_SLECET_SCENE,
    GET_CAPTURE_EXT_PARAMS,
    GET_CAPTURE_POSITION,
};

enum CaptureInputType {
    INPUT_INT = 0,
    INPUT_FLOAT,
    INPUT_UINT32,
};

typedef int32_t (*AudioCaptureOperation)(struct AudioCapture **);

struct ProcessCaptureMenuSwitchList {
    enum CaptureMenuId cmd;
    AudioCaptureOperation operation;
};

static int32_t g_closeEnd = 0;

void CleanStdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int32_t CheckInputName(int type, void *val)
{
    if (val == NULL) {
        return HDF_FAILURE;
    }
    printf("\n");
    int ret;
    int inputInt = 0;
    float inputFloat = 0.0;
    uint32_t inputUint = 0;
    switch (type) {
        case INPUT_INT:
            ret = scanf_s("%d", &inputInt);
            if (inputInt < 0 || inputInt > GET_CAPTURE_POSITION + 1) {
                LOG_FUN_ERR("Input failure");
                return HDF_FAILURE;
            }
            *(int *)val = inputInt;
            break;
        case INPUT_FLOAT:
            ret = scanf_s("%f", &inputFloat);
            *(float *)val = inputFloat;
            break;
        case INPUT_UINT32:
            ret = scanf_s("%u", &inputUint);
            if (inputUint > 0xFFFFFFFF || inputUint < 0) {
                return HDF_FAILURE;
            }
            *(uint32_t *)val = inputUint;
            break;
        default:
            ret = EOF;
            break;
    }
    if (ret == 0) {
        CleanStdin();
    } else if (ret == EOF) {
        LOG_FUN_ERR("Input failure occurs!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
void SystemInputFail()
{
    printf("please ENTER to go on...");
    while (getchar() != '\n') {
        continue;
    }
    printf("%c", getchar());
}
int32_t InitAttrsCapture(struct AudioSampleAttributes *attrs)
{
    if (attrs == NULL) {
        return HDF_FAILURE;
    }
    /* Initialization of audio parameters for playback */
    attrs->format = AUDIO_FORMAT_PCM_16_BIT;
    attrs->channelCount = AUDIO_CHANNELCOUNT;
    attrs->sampleRate = AUDIO_SAMPLE_RATE_48K;
    attrs->interleaved = 0;
    attrs->type = AUDIO_IN_MEDIA;
    attrs->period = BUFFER_PERIOD_SIZE;
    attrs->frameSize = PCM_16_BIT * attrs->channelCount / PCM_8_BIT;
    attrs->isBigEndian = false;
    attrs->isSignedData = true;
    attrs->startThreshold = DEEP_BUFFER_RENDER_PERIOD_SIZE / (attrs->frameSize);
    attrs->stopThreshold = INT_32_MAX;
    attrs->silenceThreshold = AUDIO_BUFF_SIZE;
    return 0;
}

int32_t InitDevDescCapture(struct AudioDeviceDescriptor *devDesc,
    uint32_t portId)
{
    if (devDesc == NULL) {
        return HDF_FAILURE;
    }
    /* Initialization of audio parameters for playback */
    devDesc->portId = portId;
    devDesc->pins = PIN_IN_MIC;
    devDesc->desc = NULL;
    return 0;
}

uint32_t PcmFormatToBits(enum AudioFormat format)
{
    switch (format) {
        case AUDIO_FORMAT_PCM_16_BIT:
            return PCM_16_BIT;
        case AUDIO_FORMAT_PCM_8_BIT:
            return PCM_8_BIT;
        default:
            return PCM_16_BIT;
    }
}

void StreamClose(int32_t sig)
{
    /* allow the stream to be closed gracefully */
    signal(sig, SIG_IGN);
    g_closeEnd = 1;
}

uint32_t PcmFramesToBytes(const struct AudioSampleAttributes attrs)
{
    return DEEP_BUFFER_RENDER_PERIOD_SIZE * attrs.channelCount * (PcmFormatToBits(attrs.format) >> 3);
}

#ifndef __LITEOS__
static int AudioPnpSvcThresholdMsgCheck(struct ServiceStatus *svcStatus, struct PnpReportMsg *pnpReportMsg)
{
    int ret;
    if (svcStatus == NULL || pnpReportMsg == NULL) {
        printf("input param is null!\n");
        return HDF_FAILURE;
    }

    ret = PnpReportMsgDeSerialize((uint8_t *)svcStatus->info, EVENT_REPORT, pnpReportMsg);
    if (ret != HDF_SUCCESS) {
        printf("PnpReportMsgParse fail \n");
        return HDF_FAILURE;
    }
    if (pnpReportMsg->eventMsg.eventType != EVENT_REPORT || pnpReportMsg->eventMsg.eventId != CAPTURE_THRESHOLD) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static void AudioPnpSvcEvenReceived(struct ServiceStatusListener *listener, struct ServiceStatus *svcStatus)
{
    struct StrParaCapture *strParam = NULL;
    struct AudioCapture *capture = NULL;
    uint32_t bufferSize = AUDIO_BUFF_SIZE;   // 16 * 1024 = 16KB, it needs to be calculated by audio parameters
    uint64_t replyBytes = 0;
    uint64_t requestBytes = AUDIO_BUFF_SIZE; // 16 * 1024 = 16KB
    int32_t index = 0;
    struct PnpReportMsg pnpReportMsg = {0};
    char *frame = NULL;

    if ((svcStatus == NULL) || (listener == NULL)) {
        printf("input param is null!\n");
        return ;
    }
    if (AudioPnpSvcThresholdMsgCheck(svcStatus, &pnpReportMsg) != HDF_SUCCESS) {
        printf("This event is not a threshold report event  \n");
        return ;
    }
    strParam = (struct StrParaCapture *)listener->priv;
    if (strParam == NULL) {
        printf("strParam is null \n");
        return ;
    }
    capture = strParam->capture;
    if (capture == NULL || capture->CaptureFrame == NULL || strParam->file == NULL) {
        printf("capture is null \n");
        return ;
    }
    frame = (char *)calloc(1, bufferSize);
    if (frame == NULL) {
        printf("calloc frame failed!\n");
        return ;
    }
    g_receiveFrameCount++;
    for (index = g_receiveFrameCount; index > 0; index--) {
        if (capture->CaptureFrame(capture, frame, requestBytes, &replyBytes) != HDF_SUCCESS) {
            printf("\nCaptureFrame fail\n");
        } else {
            fwrite(frame, (size_t)replyBytes, 1, strParam->file);
            g_receiveFrameCount--;
            g_totalSize += (replyBytes / PERIOD_SIZE); // 1024 = 1Kb
            if (g_totalSize % AUDIO_RECORD_INTERVAL_512KB == 0) { // 512KB
                printf("\nRecording,the audio file size is %"PRIu64"Kb\n", g_totalSize);
            }
        }
    }
    free(frame);
}

int RegisterListen(struct StrParaCapture *capture)
{
    int status;
    if (capture == NULL) {
        return -1;
    }
    g_totalSize = 0;
    g_receiveFrameCount = 0;

    g_servmgr = SvcMgrIoserviceGet(); // kernel
    g_listener = IoServiceStatusListenerNewInstance(); // kernel
    if (g_servmgr == NULL || g_listener == NULL) {
        printf("g_servmgr status g_listener is null .\n");
        return -1;
    }

    g_listener->callback = AudioPnpSvcEvenReceived;
    g_listener->priv = (void *)capture;

    status = g_servmgr->RegisterServiceStatusListener(g_servmgr, g_listener, DEVICE_CLASS_AUDIO);
    if (status != HDF_SUCCESS) {
        printf("RegisterServiceStatusListener file ret = %d.\n", status);
        return -1;
    }

    printf("RegisterListen success \n");
    return 0;
}

int UnRegisterListen(void)
{
    if (g_servmgr == NULL || g_listener == NULL) {
        printf("UnRegisterListen: input para is null!\n");
        return -1;
    }
    int32_t ret = g_servmgr->UnregisterServiceStatusListener(g_servmgr, g_listener);
    if (ret != HDF_SUCCESS) {
        printf("UnregisterServiceStatusListener file ret = %d.\n", ret);
        return -1;
    }

    printf("UnregisterServiceStatusListener success \n");
    return 0;
}
#endif

int32_t StopButtonCapture(struct AudioCapture **captureS)
{
    if (captureS == NULL) {
        return HDF_FAILURE;
    }
    if (!g_closeEnd) {
        g_closeEnd = true;
        usleep(100000); // sleep 100000us
    }
    struct AudioCapture *capture = *captureS;
    if (capture == NULL) {
        return HDF_FAILURE;
    }
    int ret = capture->control.Stop((AudioHandle)capture);
    if (ret < 0) {
        LOG_FUN_ERR("Stop capture!");
    }
    if (g_adapter == NULL || g_adapter->DestroyCapture == NULL) {
        return HDF_FAILURE;
    }
    ret = g_adapter->DestroyCapture(g_adapter, capture);
    if (ret < 0) {
        LOG_FUN_ERR("Capture already destroy!");
    }
    capture = NULL;
    g_capture = NULL;
    if (g_frame != NULL) {
        free(g_frame);
        g_frame = NULL;
    }
    if (g_file != NULL) {
        fclose(g_file);
        g_file = NULL;
    }
    if (g_CaptureModeFlag == CAPTURE_INTERUPT) {
#ifndef __LITEOS__
        ret = UnRegisterListen();
        if (ret < 0) {
            LOG_FUN_ERR("UnRegisterListen failed!");
        }
#endif
    }
    printf("Stop Successful\n");
    return HDF_SUCCESS;
}

int32_t FrameStartCaptureMmap(const AudioHandle param)
{
    if (param == NULL) {
        return HDF_FAILURE;
    }
    struct StrParaCapture *strParam = (struct StrParaCapture *)param;
    struct AudioCapture *capture = strParam->capture;
    struct AudioMmapBufferDescripter desc;
    // Modify file size
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(g_path, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    FILE *fp = fopen(pathBuf, "rb+");
    if (fp == NULL) {
        printf("Open file failed!\n");
        return HDF_FAILURE;
    }
    int fd = fileno(fp);
    if (fd == -1) {
        printf("fileno failed, fd is %d\n", fd);
        fclose(fp);
        return HDF_FAILURE;
    }
    ftruncate(fd, FILE_CAPTURE_SIZE);
    // Init param
    desc.memoryFd = fd;
    desc.isShareable = 1; // 1:Shareable ,0:Don't share
    desc.transferFrameSize = DEEP_BUFFER_RENDER_PERIOD_SIZE / 4; // One frame size 4 bit
    desc.offset = 0; // Recording must be 0
    // start
    if (capture == NULL || capture->attr.ReqMmapBuffer == NULL) {
        fclose(fp);
        return HDF_FAILURE;
    }
    int32_t ret = capture->attr.ReqMmapBuffer(capture, FILE_CAPTURE_SIZE, &desc);
    if (ret < 0) {
        printf("Request map fail,please check.\n");
        fclose(fp);
        return HDF_FAILURE;
    }
    ret = msync(desc.memoryAddress, FILE_CAPTURE_SIZE, MS_ASYNC);
    if (ret < 0) {
        printf("sync fail.\n");
    }
    munmap(desc.memoryAddress, FILE_CAPTURE_SIZE);
    fclose(fp);
    return HDF_SUCCESS;
}

int32_t FrameStartCapture(const AudioHandle param)
{
    if (param == NULL) {
        return HDF_FAILURE;
    }
    struct StrParaCapture *strParam = (struct StrParaCapture *)param;
    struct AudioCapture *capture = strParam->capture;
    FILE *file = strParam->file;
    uint32_t bufferSize = AUDIO_BUFF_SIZE;   // 16 * 1024 = 16KB, it needs to be calculated by audio parameters
    uint64_t replyBytes = 0;
    uint64_t totalSize = 0;
    uint64_t requestBytes = AUDIO_BUFF_SIZE; // 16 * 1024 = 16KB
    uint32_t failCount = 0;
    if (capture == NULL || capture->CaptureFrame == NULL || file == NULL) {
        return HDF_FAILURE;
    }
    char *frame = (char *)calloc(1, bufferSize);
    if (frame == NULL) {
        return HDF_FAILURE;
    }
    do {
        int32_t ret = capture->CaptureFrame(capture, frame, requestBytes, &replyBytes);
        if (ret < 0) {
            if (ret == HDF_ERR_INVALID_OBJECT) {
                LOG_FUN_ERR("Record already stop!");
                break;
            }
            usleep(ONE_MS);
            if (failCount++ >= 300000) { // Try 300000 times for CaptureFrame fail
                free(frame);
                return HDF_FAILURE;
            }
            continue;
        }
        failCount = 0;
        fwrite(frame, (size_t)replyBytes, 1, file);
        totalSize += (replyBytes / PERIOD_SIZE); // 1024 = 1Kb
        if (totalSize % AUDIO_RECORD_INTERVAL_512KB == 0) { // 512KB
            printf("\nRecording,the audio file size is %"PRIu64"Kb\n", totalSize);
        }
    } while ((totalSize <= AUDIO_TOTALSIZE_15M) && (!g_closeEnd)); // 15 * 1024 = 15M
    if (frame != NULL) {
        free(frame);
    }
    if (!g_closeEnd) {
        if (StopButtonCapture(&g_capture) < 0) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t CaptureChoiceModeAndRecording(struct StrParaCapture *StrParam, struct AudioCapture *capture)
{
    if (StrParam == NULL || capture == NULL) {
        LOG_FUN_ERR("InitCaptureStrParam is NULL");
        return HDF_FAILURE;
    }
    int32_t ret;
    (void)memset_s(StrParam, sizeof(struct StrParaCapture), 0, sizeof(struct StrParaCapture));
    StrParam->capture = capture;
    StrParam->file = g_file;
    StrParam->attrs = g_attrs;
    StrParam->frame = g_frame;
    if (g_CaptureModeFlag == CAPTURE_INTERUPT) {
#ifndef __LITEOS__
        ret = RegisterListen(&g_str);
        if (ret != 0) {
            printf("---RegisterListen faile--- \n");
            return HDF_FAILURE;
        }
#else
        printf("not support liteos!");
        return HDF_FAILURE;
#endif
    } else {
        pthread_attr_t tidsAttr;
        pthread_attr_init(&tidsAttr);
        pthread_attr_setdetachstate(&tidsAttr, PTHREAD_CREATE_DETACHED);
        ret = pthread_create(&g_tids, &tidsAttr, (void *)(&FrameStartCapture), &g_str);
        if (ret != 0) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t StartButtonCapture(struct AudioCapture **captureS)
{
    if (captureS == NULL || g_adapter == NULL || g_adapter->CreateCapture == NULL) {
        return HDF_FAILURE;
    }
    if (g_file != NULL) {
        LOG_FUN_ERR("the capture is playing,please stop first");
        return HDF_FAILURE;
    }
    g_closeEnd = false;
    struct AudioCapture *capture = NULL;
    int32_t ret = g_adapter->CreateCapture(g_adapter, &g_devDesc, &g_attrs, &capture);
    if (capture == NULL || ret < 0) {
        return HDF_FAILURE;
    }
    char *path = g_path;
    if (path == NULL) {
        return HDF_FAILURE;
    }
    g_file = fopen(g_path, "wb+");
    if (g_file == NULL) {
        printf("failed to open '%s'\n", g_path);
        return HDF_FAILURE;
    }
    ret = capture->control.Start((AudioHandle)capture);
    if (ret < 0) {
        if (g_file != NULL) {
            fclose(g_file);
            g_file = NULL;
        }
        return HDF_FAILURE;
    }
    uint32_t bufferSize = PcmFramesToBytes(g_attrs);
    g_frame = (char *)calloc(1, bufferSize);
    if (g_frame == NULL) {
        return HDF_FAILURE;
    }
    if (CaptureChoiceModeAndRecording(&g_str, capture) < 0) {
        LOG_FUN_ERR("CaptureChoiceModeAndRecording failed");
        return HDF_FAILURE;
    }
    *captureS = capture;
    printf("Start Successful\n");
    return HDF_SUCCESS;
}

int32_t SwitchAdapterCapture(struct AudioAdapterDescriptor *descs, const char *adapterNameCase,
    enum AudioPortDirection portFlag, struct AudioPort *capturePort, const int32_t size)
{
    struct AudioAdapterDescriptor *desc = NULL;
    int32_t index;
    uint32_t port;
    if (descs == NULL || adapterNameCase == NULL || capturePort == NULL) {
        return HDF_FAILURE;
    }
    for (index = 0; index < size; index++) {
        desc = &descs[index];
        if (desc == NULL) {
            continue;
        }
        if (desc->adapterName == NULL) {
            return HDF_FAILURE;
        }
        if (strcmp(desc->adapterName, adapterNameCase)) {
            printf("adapter name case = %s\n", adapterNameCase);
            continue;
        }
        for (port = 0; port < desc->portNum; port++) {
            // Only find out the port of out in the sound card
            if (desc->ports[port].dir == portFlag) {
                *capturePort = desc->ports[port];
                return index;
            }
        }
    }
    return HDF_FAILURE;
}
void PrintMenu1()
{
    printf(" ============== Play Capture Loading Mode ===========\n");
    printf("| 1. Capture Direct Loading                         |\n");
    printf("| 2. Capture Service Loading                        |\n");
    printf("| Note: switching is not supported in the MPI's     |\n");
    printf("|       version.                                    |\n");
    printf(" =================================================== \n");
}

int32_t SelectLoadingMode(char *resolvedPath, int32_t pathLen, char *func, int32_t funcpathLen)
{
    system("clear");
    int choice = 0;
    char *soPathHdi = NULL;
    char *soPathProxy = NULL;
    int32_t ret;
    soPathHdi = HDF_LIBRARY_FULL_PATH("libhdi_audio");
    soPathProxy = HDF_LIBRARY_FULL_PATH("libhdi_audio_client");
    PrintMenu1();
    printf("Please enter your choice:");
    ret = CheckInputName(INPUT_INT, (void *)&choice);
    if (ret < 0) return HDF_FAILURE;
    switch (choice) {
        case 1: // 1. Capture Direct Loading
            if (snprintf_s(resolvedPath, pathLen, pathLen - 1, "%s", soPathHdi) < 0) {
                LOG_FUN_ERR("snprintf_s failed!");
                return HDF_FAILURE;
            }
            if (snprintf_s(func, funcpathLen, funcpathLen - 1, "%s", "GetAudioManagerFuncs") < 0) {
                LOG_FUN_ERR("snprintf_s failed!");
                return HDF_FAILURE;
            }
            break;
        case 2: // 2. Capture Service Loading
            if (snprintf_s(resolvedPath, pathLen, pathLen - 1, "%s", soPathProxy) < 0) {
                LOG_FUN_ERR("snprintf_s failed!");
                return HDF_FAILURE;
            }
            if (snprintf_s(func, funcpathLen, funcpathLen - 1, "%s", "GetAudioProxyManagerFuncs") < 0) {
                return HDF_FAILURE;
            }
            break;
        default:
            printf("Input error,Switched to direct loading in for you,");
            SystemInputFail();
            if (snprintf_s(resolvedPath, pathLen, pathLen - 1, "%s", soPathHdi) < 0) {
                return HDF_FAILURE;
            }
            if (snprintf_s(func, funcpathLen, funcpathLen - 1, "%s", "GetAudioManagerFuncs") < 0) {
                return HDF_FAILURE;
            }
            break;
    }
    return HDF_SUCCESS;
}

int32_t GetCapturePassthroughManagerFunc(const char *adapterNameCase)
{
    if (adapterNameCase == NULL) {
        LOG_FUN_ERR("The Parameter is NULL");
        return HDF_FAILURE;
    }
    struct AudioManager *manager = NULL;
    struct AudioAdapterDescriptor *descs = NULL;
    enum AudioPortDirection port = PORT_OUT; // Set port information
    struct AudioPort capturePort;
    int32_t size = 0;
    int32_t ret;
    int32_t index;
    struct AudioManager *(*getAudioManager)() = NULL;
    getAudioManager = (struct AudioManager *(*)())(dlsym(g_captureHandle, "GetAudioManagerFuncs"));
    if (getAudioManager == NULL) {
        LOG_FUN_ERR("Get Audio Manager Funcs Fail");
        return HDF_FAILURE;
    }
    manager = getAudioManager();
    ret = manager->GetAllAdapters(manager, &descs, &size);
    int32_t param = size > MAX_SUPPORT_ADAPTER_NUM || size == 0 || descs == NULL || ret < 0;
    if (param) {
        LOG_FUN_ERR("Get All Adapters Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    index = SwitchAdapterCapture(descs, adapterNameCase, port, &capturePort, size);
    if (index < 0) {
        LOG_FUN_ERR("Not Switch Adapter Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    struct AudioAdapterDescriptor *desc = &descs[index];
    if (manager->LoadAdapter(manager, desc, &g_adapter) != 0) {
        LOG_FUN_ERR("Load Adapter Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    g_manager = manager;
    if (g_adapter == NULL) {
        LOG_FUN_ERR("load audio device failed");
        return HDF_FAILURE;
    }
    (void)g_adapter->InitAllPorts(g_adapter);
    if (InitAttrsCapture(&g_attrs) < 0) {
        g_manager->UnloadAdapter(g_manager, g_adapter);
        return HDF_FAILURE;
    }
    // Specify a hardware device
    if (InitDevDescCapture(&g_devDesc, capturePort.portId) < 0) {
        g_manager->UnloadAdapter(g_manager, g_adapter);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t GetCaptureProxyManagerFunc(const char *adapterNameCase)
{
    if (adapterNameCase == NULL) {
        LOG_FUN_ERR("The Parameter is NULL");
        return HDF_FAILURE;
    }
    struct AudioProxyManager *proxyManager = NULL;
    struct AudioAdapterDescriptor *descs = NULL;
    enum AudioPortDirection port = PORT_OUT; // Set port information
    struct AudioPort capturePort;
    int32_t size = 0;
    int32_t ret;
    int32_t index;
    struct AudioProxyManager *(*getAudioManager)() = NULL;
    getAudioManager = (struct AudioProxyManager *(*)())(dlsym(g_captureHandle, "GetAudioProxyManagerFuncs"));
    if (getAudioManager == NULL) {
        return HDF_FAILURE;
    }
    proxyManager = getAudioManager();
    ret = proxyManager->GetAllAdapters(proxyManager, &descs, &size);
    int32_t check = size > MAX_SUPPORT_ADAPTER_NUM || size == 0 || descs == NULL || ret < 0;
    if (check) {
        LOG_FUN_ERR("Get All Adapters Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    // Get qualified sound card and port
    index = SwitchAdapterCapture(descs, adapterNameCase, port, &capturePort, size);
    if (index < 0) {
        LOG_FUN_ERR("Not Switch Adapter Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    struct AudioAdapterDescriptor *desc = &descs[index];
    if (proxyManager->LoadAdapter(proxyManager, desc, &g_adapter) != 0) {
        LOG_FUN_ERR("Load Adapter Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    g_proxyManager = proxyManager;
    if (g_adapter == NULL) {
        LOG_FUN_ERR("load audio device failed");
        return HDF_FAILURE;
    }
    // Initialization port information, can fill through mode and other parameters
    (void)g_adapter->InitAllPorts(g_adapter);
    // User needs to set
    if (InitAttrsCapture(&g_attrs) < 0) {
        g_proxyManager->UnloadAdapter(g_proxyManager, g_adapter);
        return HDF_FAILURE;
    }
    // Specify a hardware device
    if (InitDevDescCapture(&g_devDesc, capturePort.portId) < 0) {
        g_proxyManager->UnloadAdapter(g_proxyManager, g_adapter);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InitParam()
{
    char resolvedPath[PATH_LEN] = {0};
    char func[PATH_LEN] = {0};
    if (SelectLoadingMode(resolvedPath, PATH_LEN, func, PATH_LEN) < 0) {
        return HDF_FAILURE;
    }
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(resolvedPath, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    g_captureHandle = dlopen(pathBuf, 1);
    if (g_captureHandle == NULL) {
        LOG_FUN_ERR("Open so Fail, reason:%s", dlerror());
        return HDF_FAILURE;
    }
    struct AudioPort audioPort;
    audioPort.dir = PORT_IN;
    audioPort.portId = 0;
    audioPort.portName = "AOP";
    char adapterNameCase[PATH_LEN] = "usb";
    if (strcmp(func, "GetAudioManagerFuncs") == 0) {
        if (GetCapturePassthroughManagerFunc(adapterNameCase) < 0) {
            LOG_FUN_ERR("GetCapturePassthroughManagerFunc Fail");
            return HDF_FAILURE;
        }
#ifdef AUDIO_HAL_USER
        char sdkResolvedPath[] = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_capture");
        g_sdkHandle = dlopen(sdkResolvedPath, 1);
        if (g_sdkHandle == NULL) {
            LOG_FUN_ERR("Open so Invalid, reason:%s", dlerror());
            return HDF_FAILURE;
        }
        g_sdkInitSp = (int32_t (*)())(dlsym(g_sdkHandle, "MpiSdkInit"));
        if (g_sdkInitSp == NULL) {
            LOG_FUN_ERR("Get sdk init Funcs Invalid");
            return HDF_FAILURE;
        }
        g_sdkExitSp = (void (*)())(dlsym(g_sdkHandle, "MpiSdkExit"));
        if (g_sdkExitSp == NULL) {
            LOG_FUN_ERR("Get sdk exit Funcs Invalid");
            return HDF_FAILURE;
        }
        g_sdkInitSp();
#endif
    } else {
        if (GetCaptureProxyManagerFunc(adapterNameCase) < 0) {
            LOG_FUN_ERR("GetCaptureProxyManagerFunc Fail");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t SetCaptureMute()
{
    int32_t val = 0;
    bool isMute = false;
    int32_t ret;
    if (g_capture == NULL || g_capture->volume.GetMute == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->volume.GetMute((AudioHandle)g_capture, &isMute);
    if (ret < 0) {
        LOG_FUN_ERR("The current mute state was not obtained!");
    }
    printf("Now %s ,Do you need to set mute status(1/0):\n", isMute ? "mute" : "not mute");
    ret = CheckInputName(INPUT_INT, (void *)&val);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    if (isMute != 0 && isMute != 1) {
        LOG_FUN_ERR("Invalid value,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_capture == NULL || g_capture->volume.SetMute == NULL) {
        LOG_FUN_ERR("Record already complete,Please record againand,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (val == 1) {
        ret = g_capture->volume.SetMute((AudioHandle)g_capture, !isMute);
    }
    return ret;
}

int32_t SetCaptureVolume()
{
    int32_t ret;
    float val = 0.5;
    if (g_capture == NULL || g_capture->volume.GetVolume == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->volume.GetVolume((AudioHandle)g_capture, &val);
    if (ret < 0) {
        LOG_FUN_ERR("Get current volume failed,");
        SystemInputFail();
        return ret;
    }
    printf("Now the volume is %f ,Please enter the volume value you want to set (0.0-1.0):\n", val);
    ret = CheckInputName(INPUT_FLOAT, (void *)&val);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    if (val < 0.0 || val > 1.0) {
        LOG_FUN_ERR("Invalid volume value,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_capture == NULL) {
        LOG_FUN_ERR("Record already complete,Please record againand,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_capture->volume.SetVolume == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->volume.SetVolume((AudioHandle)g_capture, val);
    if (ret < 0) {
        LOG_FUN_ERR("set volume fail,");
        SystemInputFail();
    }
    return ret;
}

int32_t SetCaptureGain()
{
    int32_t ret;
    float val = 1.0;
    if (g_capture == NULL || g_capture->volume.GetGain == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->volume.GetGain((AudioHandle)g_capture, &val);
    if (ret < 0) {
        LOG_FUN_ERR("Get current gain failed,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    printf("Now the gain is %f, Please enter the gain value you want to set (0.0-15.0):\n", val);
    ret = CheckInputName(INPUT_FLOAT, (void *)&val);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    // gain is 0.0 ~ 15.0
    if (val < 0.0 || val > 15.0) {
        LOG_FUN_ERR("Invalid gain value,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_capture == NULL) {
        LOG_FUN_ERR("Record already complete,Please record againand,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_capture->volume.SetGain == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->volume.SetGain((AudioHandle)g_capture, val);
    if (ret < 0) {
        LOG_FUN_ERR("Set capture gain failed,");
        SystemInputFail();
    }
    return ret;
}

int32_t SetCaptyrePause()
{
    int32_t ret;
    if (g_capture == NULL || g_capture->control.Pause == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->control.Pause((AudioHandle)g_capture);
    if (ret != 0) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t SetCaptureResume()
{
    int32_t ret;
    if (g_capture == NULL || g_capture->control.Resume == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->control.Resume((AudioHandle)g_capture);
    if (ret != 0) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void PrintAttributesFromat()
{
    printf(" ============= Capture Sample Attributes Fromat =============== \n");
    printf("| 1. Capture AUDIO_FORMAT_PCM_8_BIT                            |\n");
    printf("| 2. Capture AUDIO_FORMAT_PCM_16_BIT                           |\n");
    printf("| 3. Capture AUDIO_FORMAT_PCM_24_BIT                           |\n");
    printf("| 4. Capture AUDIO_FORMAT_PCM_32_BIT                           |\n");
    printf(" ============================================================== \n");
}

int32_t SelectAttributesFomat(uint32_t *fomat)
{
    if (fomat == NULL) {
        return HDF_FAILURE;
    }
    PrintAttributesFromat();
    printf("Please select audio format,If not selected, the default is 16bit:");
    int32_t ret;
    int val = 0;
    ret = CheckInputName(INPUT_INT, (void *)&val);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    switch (val) {
        case AUDIO_FORMAT_PCM_8_BIT:
            *fomat = AUDIO_FORMAT_PCM_8_BIT;
            break;
        case AUDIO_FORMAT_PCM_16_BIT:
            *fomat = AUDIO_FORMAT_PCM_16_BIT;
            break;
        case AUDIO_FORMAT_PCM_24_BIT:
            *fomat = AUDIO_FORMAT_PCM_24_BIT;
            break;
        case AUDIO_FORMAT_PCM_32_BIT:
            *fomat = AUDIO_FORMAT_PCM_32_BIT;
            break;
        default:
            *fomat = AUDIO_FORMAT_PCM_16_BIT;
            break;
    }
    return HDF_SUCCESS;
}

int32_t SetCaptureAttributes()
{
    int32_t ret;
    struct AudioSampleAttributes attrs;
    if (g_capture == NULL || g_capture->attr.GetSampleAttributes == NULL) {
        LOG_FUN_ERR("pointer is NULL");
        return HDF_FAILURE;
    }
    ret = g_capture->attr.GetSampleAttributes((AudioHandle)g_capture, &attrs);
    if (ret < 0) {
        LOG_FUN_ERR("GetCaptureAttributes failed\n");
    } else {
        printf("Current sample attributes:\n");
        printf("audioType is %u\nfomat is %u\nsampleRate is %u\nchannalCount is"
            "%u\nperiod is %u\nframesize is %u\nbigEndian is %u\nSignedData is %u\n",
            attrs.type, attrs.format, attrs.sampleRate, attrs.channelCount,
            attrs.period, attrs.frameSize, attrs.isBigEndian, attrs.isSignedData);
    }
    printf("Set Sample Attributes,");
    SystemInputFail();
    system("clear");
    printf("The sample attributes you want to set,Step by step, please.\n");
    ret = SelectAttributesFomat((uint32_t *)(&attrs.format));
    if (ret < 0) {
        LOG_FUN_ERR("SetCaptureAttributes format failed");
        return HDF_FAILURE;
    }
    printf("\nPlease input sample rate(48000,44100,32000...):");
    ret = CheckInputName(INPUT_UINT32, (void *)(&attrs.sampleRate));
    if (ret < 0) return HDF_FAILURE;
    printf("\nPlease input bigEndian(false=0/true=1):");
    ret = CheckInputName(INPUT_UINT32, (void *)(&attrs.isBigEndian));
    if (ret < 0) {
        return HDF_FAILURE;
    }
    if (g_capture == NULL || g_capture->attr.SetSampleAttributes == NULL) {
        LOG_FUN_ERR("Record already complete,Please record againand set the attrbutes,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    ret = g_capture->attr.SetSampleAttributes((AudioHandle)g_capture, &attrs);
    if (ret < 0) {
        LOG_FUN_ERR("Set capture attributes failed,");
        SystemInputFail();
    }
    return ret;
}

int32_t SelectCaptureScene()
{
    system("clear");
    int32_t ret;
    int val = 0;
    struct AudioSceneDescriptor scene;
    printf(" ====================  Select Scene ==================== \n");
    printf("0 is Mic.                                               |\n");
    printf("1 is Headphone mic.                                     |\n");
    printf(" ======================================================= \n");
    printf("Please input your choice:\n");
    ret = CheckInputName(INPUT_INT, (void *)&val);
    if (ret < 0 || (val != 0 && val != 1)) {
        LOG_FUN_ERR("Invalid value,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (val == 1) {
        scene.scene.id = 0;
        scene.desc.pins = PIN_IN_HS_MIC;
    } else {
        scene.scene.id = 0;
        scene.desc.pins = PIN_IN_MIC;
    }
    if (g_capture == NULL) {
        LOG_FUN_ERR("Record already stop,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_capture->scene.SelectScene == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->scene.SelectScene((AudioHandle)g_capture, &scene);
    if (ret < 0) {
        LOG_FUN_ERR("Select scene fail");
    }
    return ret;
}
int32_t GetCaptureExtParams()
{
    char keyValueList[BUFFER_LEN] = {0};
    int32_t ret;
    if (g_capture == NULL || g_capture->attr.GetExtraParams == NULL) {
        return HDF_FAILURE;
    }
    ret = g_capture->attr.GetExtraParams((AudioHandle)g_capture, keyValueList, EXT_PARAMS_MAXLEN);
    if (ret < 0) {
        LOG_FUN_ERR("Get EXT params failed!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    printf("keyValueList = %s\n", keyValueList);
    return HDF_SUCCESS;
}

int32_t GetCaptureMmapPosition()
{
    int32_t ret;
    if (g_capture == NULL || g_capture->attr.GetMmapPosition == NULL) {
        return HDF_FAILURE;
    }
    uint64_t frames = 0;
    struct AudioTimeStamp time;
    time.tvNSec = 0;
    time.tvSec = 0;
    ret = g_capture->attr.GetMmapPosition((AudioHandle)g_capture, &frames, &time);
    if (ret < 0) {
        LOG_FUN_ERR("Get current Mmap frames Position failed!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    printf("Now the Position is %"PRIu64"\n", frames);
    SystemInputFail();
    return HDF_SUCCESS;
}

void PrintMenu2()
{
    printf(" ================== Play Capture Menu ================== \n");
    printf("| 1. Capture Start                                      |\n");
    printf("| 2. Capture Stop                                       |\n");
    printf("| 3. Capture Resume                                     |\n");
    printf("| 4. Capture Pause                                      |\n");
    printf("| 5. Capture SetVolume                                  |\n");
    printf("| 6. Capture SetGain                                    |\n");
    printf("| 7. Capture SetMute                                    |\n");
    printf("| 8. Capture SetAttributes                              |\n");
    printf("| 9. Capture SelectScene                                |\n");
    printf("| 10. Capture GetExtParams                              |\n");
    printf("| 11. Capture getMmapPosition                           |\n");
    printf("| 12.Exit                                               |\n");
    printf(" ======================================================= \n");
}

static struct ProcessCaptureMenuSwitchList g_processCaptureMenuSwitchList[] = {
    {CAPTURE_START, StartButtonCapture},
    {CAPTURE_STOP, StopButtonCapture},
    {CAPTURE_RESUME, SetCaptureResume},
    {CAPTURE_PAUSE, SetCaptyrePause},
    {SET_CAPTURE_VOLUME, SetCaptureVolume},
    {SET_CAPTURE_GAIN, SetCaptureGain},
    {SET_CAPTURE_MUTE, SetCaptureMute},
    {SET_CAPTURE_ATTRIBUTES, SetCaptureAttributes},
    {SET_CAPTURE_SLECET_SCENE, SelectCaptureScene},
    {GET_CAPTURE_EXT_PARAMS, GetCaptureExtParams},
    {GET_CAPTURE_POSITION, GetCaptureMmapPosition},

};

void ProcessMenu(int32_t choice)
{
    int32_t i;
    if (choice == GET_CAPTURE_POSITION + 1) {
        LOG_FUN_ERR("Exit from application program!");
        return;
    }
    if (g_capture == NULL && choice != 1) {
        LOG_FUN_ERR("this capture already release,");
        SystemInputFail();
        return;
    }
    for (i = CAPTURE_START; i <= GET_CAPTURE_POSITION; ++i) {
        if ((choice == (int32_t)g_processCaptureMenuSwitchList[i - 1].cmd) &&
            (g_processCaptureMenuSwitchList[i - 1].operation != NULL)) {
            g_processCaptureMenuSwitchList[i - 1].operation(&g_capture);
        }
    }
}

void PrintMenu0(void)
{
    printf(" ============== Play Capture select ===========\n");
    printf("| 1. Capture Poll                             |\n");
    printf("| 2. Capture Interrupt                        |\n");
    printf(" ==============================================\n");
}

void Choice0(void)
{
    system("clear");
    int choice = 0;
    PrintMenu0();
    printf("Please enter your choice:");
    int32_t ret = CheckInputName(INPUT_INT, (void *)&choice);
    if (ret < 0) {
        return;
    }
    switch (choice) {
        case CAPTURE_POLL:
            g_CaptureModeFlag = CAPTURE_POLL;
            break;
        case CAPTURE_INTERUPT:
            g_CaptureModeFlag = CAPTURE_INTERUPT;
            break;
        default:
            printf("Input error,Switched to Poll mode in for you,");
            SystemInputFail();
            break;
    }
    return;
}


void Choice(void)
{
    int32_t option = 0;
    while (option < GET_CAPTURE_POSITION + 1 && option >= 0) {
        system("clear");
        PrintMenu2();
        printf("your choice is:\n");
        int32_t ret = CheckInputName(INPUT_INT, (void *)&option);
        if (ret < 0) {
            continue;
        }
        if (option < CAPTURE_START || option > GET_CAPTURE_POSITION + 1) {
            printf("You input is wrong,");
            option = 0;
            SystemInputFail();
            continue;
        }
        ProcessMenu(option);
    }
}

int32_t CheckAndOpenFile(int32_t argc, char const *argv[])
{
    if (argc < 2 || argv == NULL || argv[0] == NULL) { // The parameter number is not greater than 2
        printf("usage:[1]sample [2]/data/test.wav\n");
        return HDF_FAILURE;
    }
    int32_t ret;
    if (argv[1] == NULL || strlen(argv[1]) == 0) {
        return HDF_FAILURE;
    }
    ret = strncpy_s(g_path, PATH_LEN, argv[1], strlen(argv[1]) + 1);
    if (ret != 0) {
        LOG_FUN_ERR("copy fail");
        return HDF_FAILURE;
    }
    char *path = g_path;
    if (path == NULL) {
        return HDF_FAILURE;
    }
    FILE *file = fopen(path, "wb+");
    if (file == NULL) {
        printf("failed to open '%s',Please enter the correct file name \n", g_path);
        return HDF_FAILURE;
    }
    fclose(file);
    return HDF_SUCCESS;
}

int32_t main(int32_t argc, char const *argv[])
{
    int32_t ret = CheckAndOpenFile(argc, argv);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    bool soMode = false;
    if (InitParam()) { // init
        LOG_FUN_ERR("InitParam Fail");
        return HDF_FAILURE;
    }
    Choice0();

    Choice();
    if (g_capture != NULL && g_adapter != NULL) {
        StopButtonCapture(&g_capture);
    }
    if (g_manager != NULL) {
        if (g_manager->UnloadAdapter != NULL) {
            g_manager->UnloadAdapter(g_manager, g_adapter);
        }
        soMode = true;
    } else {
        if (g_proxyManager != NULL && g_proxyManager->UnloadAdapter != NULL) {
            g_proxyManager->UnloadAdapter(g_proxyManager, g_adapter);
        }
    }
#ifdef AUDIO_HAL_USER
        if (soMode) {
            g_sdkExitSp();
            if (g_sdkHandle != NULL) {
                dlclose(g_sdkHandle);
            }
        }
#endif
    dlclose(g_handle);
    printf("Record file path:%s\n", g_path);
    return 0;
}

