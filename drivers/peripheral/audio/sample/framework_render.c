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

#define LOG_FUN_ERR(fmt, arg...) do { \
        printf("%s: [%s]: [%d]:[ERROR]:" fmt"\n", __FILE__, __func__, __LINE__, ##arg); \
    } while (0)

#define MAX_SUPPORT_ADAPTER_NUM  8  // Number of sound cards supported
#define BUFFER_LEN 256
#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164
#define MOVE_LEFT_NUM 8
#define AUDIO_SAMPLE_RATE_8K 8000
#define AUDIO_CHANNELCOUNT 2
#define AUDIO_SAMPLE_RATE_48K 48000
#define PATH_LEN 256
#define DEEP_BUFFER_RENDER_PERIOD_SIZE 4096
#define DEEP_BUFFER_RENDER_PERIOD_COUNT 8
#define INT_32_MAX 0x7fffffff
#define PERIOD_SIZE 1024
#define EXT_PARAMS_MAXLEN 107

enum AudioPCMBit {
    PCM_8_BIT  = 8,       /* 8-bit PCM */
    PCM_16_BIT = 16,       /* 16-bit PCM */
    PCM_24_BIT = 24,       /* 24-bit PCM */
    PCM_32_BIT = 32,       /* 32-bit PCM */
};

enum RenderSoundCardMode {
    ACODEC = 1,
    SMARTPA = 2,
};

enum RenderLoadingMode {
    DIRECT = 1,
    SERVICE = 2,
};

struct AudioHeadInfo {
    uint32_t testFileRiffId;
    uint32_t testFileRiffSize;
    uint32_t testFileFmt;
    uint32_t audioFileFmtId;
    uint32_t audioFileFmtSize;
    uint16_t audioFileFormat;
    uint16_t audioChannelNum;
    uint32_t audioSampleRate;
    uint32_t audioByteRate;
    uint16_t audioBlockAlign;
    uint16_t audioBitsPerSample;
    uint32_t dataId;
    uint32_t dataSize;
};

struct StrPara {
    struct AudioRender *render;
    FILE *file;
    struct AudioSampleAttributes attrs;
    uint64_t *replyBytes;
    char *frame;
    int32_t bufferSize;
};

struct AudioRender *g_render = NULL;
struct AudioAdapter *g_adapter = NULL;
struct AudioManager *g_manager = NULL;
static struct AudioProxyManager *g_proxyManager = NULL;
struct AudioDeviceDescriptor g_devDesc;
struct AudioSampleAttributes g_attrs;
struct AudioPort g_audioPort;
struct AudioHeadInfo g_wavHeadInfo;
static struct StrPara g_str;

pthread_t g_tids;
char *g_frame = NULL;
void *g_handle;
FILE *g_file;

char g_path[256];
static int32_t g_closeEnd = 0;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_functionCond = PTHREAD_COND_INITIALIZER;
int g_waitSleep = 0;
#ifdef AUDIO_HAL_USER
void *g_sdkHandle;
int (*g_sdkInitSp)() = NULL;
void (*g_sdkExitSp)() = NULL;
#endif
enum RenderMenuId {
    RENDER_START = 1,
    RENDER_STOP,
    RENDER_RESUME,
    RENDER_PAUSE,
    SET_RENDER_VOLUME,
    SET_RENDER_GAIN,
    SET_RENDER_MUTE,
    SET_RENDER_ATTRIBUTES,
    SET_RENDER_SLECET_SCENE,
    GET_RENDER_EXT_PARAMS,
    GET_RENDER_POSITION,
};

enum RenderInputType {
    INPUT_INT = 0,
    INPUT_FLOAT,
    INPUT_UINT32,
};

typedef int32_t (*AudioRenderOperation)(struct AudioRender **);

struct ProcessRenderMenuSwitchList {
    enum RenderMenuId cmd;
    AudioRenderOperation operation;
};

void CleanStdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int32_t CheckInputName(int type, void *val)
{
    int ret;
    int inputInt = 0;
    float inputFloat = 0.0;
    uint32_t inputUint = 0;
    if (val == NULL) {
        return HDF_FAILURE;
    }
    printf("\n");
    switch (type) {
        case INPUT_INT:
            ret = scanf_s("%d", &inputInt);
            if (inputInt < 0 || inputInt > GET_RENDER_POSITION + 1) {
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

int32_t InitAttrs(struct AudioSampleAttributes *attrs)
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
    attrs->period = DEEP_BUFFER_RENDER_PERIOD_SIZE;
    attrs->frameSize = PCM_16_BIT * attrs->channelCount / PCM_8_BIT;
    attrs->isBigEndian = false;
    attrs->isSignedData = true;
    attrs->startThreshold = DEEP_BUFFER_RENDER_PERIOD_SIZE / (attrs->frameSize);
    attrs->stopThreshold = INT_32_MAX;
    attrs->silenceThreshold = 0;
    return HDF_SUCCESS;
}

int32_t InitDevDesc(struct AudioDeviceDescriptor *devDesc, uint32_t portId)
{
    if (devDesc == NULL) {
        return HDF_FAILURE;
    }
    /* Initialization of audio parameters for playback */
    devDesc->portId = portId;
    devDesc->pins = PIN_OUT_SPEAKER;
    devDesc->desc = NULL;
    return HDF_SUCCESS;
}

uint32_t StringToInt(const char *str)
{
    if (str == NULL || str[0] == '\0') {
        return 0;
    }
    uint32_t temp = str[0];
    int32_t strLength = strlen(str) - 1;
    for (int32_t i = strLength; i >= 0; i--) {
        temp <<= MOVE_LEFT_NUM;
        temp += str[i];
    }
    return temp;
}
int32_t WavHeadAnalysis(FILE *file, struct AudioSampleAttributes *attrs)
{
    if (file == NULL || attrs == NULL) {
        return HDF_FAILURE;
    }
    uint32_t ret;
    const char *audioRiffIdParam = "RIFF";
    const char *audioFileFmtParam = "WAVE";
    const char *aduioDataIdParam = "data";
    ret = fread(&g_wavHeadInfo, sizeof(g_wavHeadInfo), 1, file);
    if (ret != 1) {
        return HDF_FAILURE;
    }
    uint32_t audioRiffId = StringToInt(audioRiffIdParam);
    uint32_t audioFileFmt = StringToInt(audioFileFmtParam);
    uint32_t aduioDataId = StringToInt(aduioDataIdParam);
    if (g_wavHeadInfo.testFileRiffId != audioRiffId || g_wavHeadInfo.testFileFmt != audioFileFmt ||
        g_wavHeadInfo.dataId != aduioDataId) {
        return HDF_FAILURE;
        }
    attrs->channelCount = g_wavHeadInfo.audioChannelNum;
    attrs->sampleRate = g_wavHeadInfo.audioSampleRate;
    switch (g_wavHeadInfo.audioBitsPerSample) {
        case PCM_8_BIT: {
            attrs->format = AUDIO_FORMAT_PCM_8_BIT;
            break;
        }
        case PCM_16_BIT: {
            attrs->format = AUDIO_FORMAT_PCM_16_BIT;
            break;
        }
        case PCM_24_BIT: {
            attrs->format = AUDIO_FORMAT_PCM_24_BIT;
            break;
        }
        case PCM_32_BIT: {
            attrs->format = AUDIO_FORMAT_PCM_32_BIT;
            break;
        }
        default:
            return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t SwitchAdapter(struct AudioAdapterDescriptor *descs,
    const char *adapterNameCase, enum AudioPortDirection portFlag,
    struct AudioPort *renderPort, int32_t size)
{
    struct AudioAdapterDescriptor *desc = NULL;
    if (descs == NULL || adapterNameCase == NULL || renderPort == NULL) {
        return HDF_FAILURE;
    }
    uint32_t port;
    for (int32_t index = 0; index < size; index++) {
        desc = &descs[index];
        if (desc == NULL) {
            continue;
        }
        if (desc->adapterName == NULL) {
            return HDF_FAILURE;
        }
        if (strcmp(desc->adapterName, adapterNameCase)) {
            continue;
        }
        for (port = 0; port < desc->portNum; port++) {
            // Only find out the port of out in the sound card
            if (desc->ports[port].dir == portFlag) {
                *renderPort = desc->ports[port];
                return index;
            }
        }
    }
    return HDF_ERR_NOT_SUPPORT;
}

uint32_t PcmFormatToBits(enum AudioFormat formatBit)
{
    switch (formatBit) {
        case AUDIO_FORMAT_PCM_16_BIT:
            return PCM_16_BIT;
        case AUDIO_FORMAT_PCM_8_BIT:
            return PCM_8_BIT;
        default:
            return PCM_16_BIT;
    }
}

uint32_t PcmFramesToBytes(const struct AudioSampleAttributes attrs)
{
    return DEEP_BUFFER_RENDER_PERIOD_SIZE * attrs.channelCount * (PcmFormatToBits(attrs.format) >> 3);
}

static inline void FileClose(FILE **file)
{
    if ((file != NULL) && ((*file) != NULL)) {
        fclose(*file);
        *file = NULL;
    }
    return;
}

int32_t StopAudioFiles(struct AudioRender **renderS)
{
    if (renderS == NULL) {
        return HDF_FAILURE;
    }
    if (g_waitSleep) {
        pthread_mutex_lock(&g_mutex);
        g_waitSleep = 0;
        pthread_cond_signal(&g_functionCond);
        pthread_mutex_unlock(&g_mutex);
    }
    if (!g_closeEnd) {
        g_closeEnd = true;
        usleep(100000); // sleep 100000us
    }
    struct AudioRender *render = *renderS;
    if (render == NULL) {
        LOG_FUN_ERR("render is null");
        return HDF_FAILURE;
    }
    int32_t ret = render->control.Stop((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("Stop Render!");
    }
    if (g_adapter == NULL || g_adapter->DestroyRender == NULL) {
        return HDF_FAILURE;
    }
    ret = g_adapter->DestroyRender(g_adapter, render);
    if (ret < 0) {
        LOG_FUN_ERR("Destroy Render!");
    }
    render = NULL;
    g_render = NULL;
    if (g_frame != NULL) {
        free(g_frame);
        g_frame = NULL;
    }
    FileClose(&g_file);
    printf("Stop Successful\n");
    return ret;
}

bool PrepareStopAndUloadAdapter(void)
{
    bool soMode = false;

    if (g_render != NULL && g_adapter != NULL) {
        StopAudioFiles(&g_render);
    }

    if (g_manager != NULL) {
        soMode = true;
        if (g_manager->UnloadAdapter != NULL) {
            g_manager->UnloadAdapter(g_manager, g_adapter);
        }
    } else {
        if (g_proxyManager != NULL && g_proxyManager->UnloadAdapter != NULL) {
            g_proxyManager->UnloadAdapter(g_proxyManager, g_adapter);
        }
    }

    return soMode;
}

void StopRenderBySig(int32_t sig)
{
    bool soMode = false;

    soMode = PrepareStopAndUloadAdapter();
    dlclose(g_handle);
    g_closeEnd = 1;

#ifdef AUDIO_HAL_USER
    if (soMode) {
        g_sdkExitSp();
        if (g_sdkHandle != NULL) {
            dlclose(g_sdkHandle);
        }
    }
#endif

    (void)signal(sig, SIG_DFL);
    return;
}
static inline void ProcessCommonSig(void)
{
    (void)signal(SIGKILL, StopRenderBySig);
    (void)signal(SIGINT, StopRenderBySig);
    (void)signal(SIGTERM, StopRenderBySig);
    return;
}

int32_t FrameStartMmap(const AudioHandle param)
{
    if (param == NULL) {
        return HDF_FAILURE;
    }
    struct StrPara *strParam = (struct StrPara *)param;
    struct AudioRender *render = strParam->render;
    struct AudioMmapBufferDescripter desc;
    ProcessCommonSig();
    // get file length
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(g_path, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    FILE *fp = fopen(pathBuf, "rb+");
    if (fp == NULL) {
        printf("Open file failed!\n");
        return HDF_FAILURE;
    }
    int32_t ret = fseek(fp, 0, SEEK_END);
    if (ret != 0) {
        fclose(fp);
        return HDF_FAILURE;
    }
    int32_t reqSize = ftell(fp);
    // Converts a file pointer to a device descriptor
    int fd = fileno(fp);
    if (fd == -1) {
        printf("fileno failed, fd is %d\n", fd);
        fclose(fp);
        return HDF_FAILURE;
    }
    // Init param
    desc.memoryFd = fd;
    desc.isShareable = 1; // 1:Shareable ,0:Don't share
    desc.transferFrameSize = DEEP_BUFFER_RENDER_PERIOD_SIZE / 4; // One frame size 4 bit
    desc.offset = sizeof(g_wavHeadInfo);
    // start
    if (render == NULL || render->attr.ReqMmapBuffer == NULL) {
        fclose(fp);
        return HDF_FAILURE;
    }
    ret = render->attr.ReqMmapBuffer(render, reqSize, &desc);
    if (ret < 0 || reqSize <= 0) {
        printf("Request map fail,please check.\n");
        fclose(fp);
        return HDF_FAILURE;
    }
    munmap(desc.memoryAddress, reqSize);
    fclose(fp);
    ret = StopAudioFiles(&render);
    if (ret < 0) {
        LOG_FUN_ERR("StopAudioFiles File!");
    }
    return HDF_SUCCESS;
}

int32_t FrameStart(const AudioHandle param)
{
    if (param == NULL) {
        return HDF_FAILURE;
    }
    struct StrPara *strParam = (struct StrPara *)param;
    struct AudioRender *render = strParam->render;
    char *frame = strParam->frame;
    int32_t bufferSize = strParam->bufferSize;
    int32_t ret;
    int32_t readSize;
    int32_t remainingDataSize = g_wavHeadInfo.testFileRiffSize;
    uint32_t numRead;
    ProcessCommonSig();
    uint64_t replyBytes;
    if (g_file == NULL) {
        return HDF_FAILURE;
    }
    if (render == NULL || render->RenderFrame == NULL || frame == NULL) {
        return HDF_FAILURE;
    }
    do {
        readSize = (remainingDataSize > bufferSize) ? bufferSize : remainingDataSize;
        numRead = fread(frame, 1, readSize, g_file);
        if (numRead > 0) {
            ret = render->RenderFrame(render, frame, numRead, &replyBytes);
            if (ret == HDF_ERR_INVALID_OBJECT) {
                LOG_FUN_ERR("Render already stop!");
                break;
            }
            remainingDataSize -= numRead;
        }
        while (g_waitSleep) {
            printf("music pause now.\n");
            pthread_cond_wait(&g_functionCond, &g_mutex);
            printf("music resume now.\n");
        }
    } while (!g_closeEnd && numRead > 0 && remainingDataSize > 0);
    if (!g_closeEnd) {
        printf("\nPlay complete, please select input again\n");
        (void)StopAudioFiles(&render);
    }
    return HDF_SUCCESS;
}

int32_t InitPlayingAudioParam(struct AudioRender *render)
{
    if (render == NULL) {
        return HDF_FAILURE;
    }
    uint32_t bufferSize = PcmFramesToBytes(g_attrs);
    g_frame = (char *)calloc(1, bufferSize);
    if (g_frame == NULL) {
        return HDF_FAILURE;
    }
    (void)memset_s(&g_str, sizeof(struct StrPara), 0, sizeof(struct StrPara));
    g_str.render = render;
    g_str.bufferSize = bufferSize;
    g_str.frame = g_frame;
    return HDF_SUCCESS;
}

int32_t PlayingAudioFiles(struct AudioRender **renderS)
{
    if (renderS == NULL || g_adapter == NULL || g_adapter->CreateRender == NULL) {
        return HDF_FAILURE;
    }
    if (g_file != NULL) {
        LOG_FUN_ERR("the music is playing,please stop first");
        return HDF_FAILURE;
    }
    g_closeEnd = false;
    struct AudioRender *render = NULL;
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(g_path, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    g_file = fopen(pathBuf, "rb");
    if (g_file == NULL) {
        printf("failed to open '%s'\n", g_path);
        return HDF_FAILURE;
    }
    if (WavHeadAnalysis(g_file, &g_attrs) < 0) {
        LOG_FUN_ERR("Frame test is Fail");
        FileClose(&g_file);
        return HDF_FAILURE;
    }
    int32_t ret = g_adapter->CreateRender(g_adapter, &g_devDesc, &g_attrs, &render);
    if (render == NULL || ret < 0 || render->RenderFrame == NULL) {
        LOG_FUN_ERR("AudioDeviceCreateRender failed or RenderFrame is null");
        FileClose(&g_file);
        return HDF_FAILURE;
    }
    // Playing audio files
    if (render->control.Start((AudioHandle)render)) {
        LOG_FUN_ERR("Start Bind Fail!");
        FileClose(&g_file);
        return HDF_FAILURE;
    }
    if (InitPlayingAudioParam(render) < 0) {
        FileClose(&g_file);
        return HDF_FAILURE;
    }
    pthread_attr_t tidsAttr;
    pthread_attr_init(&tidsAttr);
    pthread_attr_setdetachstate(&tidsAttr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&g_tids, &tidsAttr, (void *)(&FrameStart), &g_str) != 0) {
        LOG_FUN_ERR("Create Thread Fail");
        FileClose(&g_file);
        return HDF_FAILURE;
    }
    *renderS = render;
    printf("Start Successful,Music is playing\n");
    return HDF_SUCCESS;
}

void PrintMenu0()
{
    printf(" ============= Play Render Sound Card Mode ==========\n");
    printf("| 1. Render Acodec                                  |\n");
    printf("| 2. Render SmartPA                                 |\n");
    printf(" =================================================== \n");
}

void PrintMenu1()
{
    printf(" ============== Play Render Loading Mode ===========\n");
    printf("| 1. Render Direct Loading                         |\n");
    printf("| 2. Render Service Loading                        |\n");
    printf("| Note: switching is not supported in the MPI's    |\n");
    printf("|       version.                                   |\n");
    printf(" ================================================== \n");
}

int32_t SwitchInternalOrExternal(char *adapterNameCase, int32_t nameLen)
{
    system("clear");
    int choice = 0;
    PrintMenu0();
    printf("Please enter your choice:");
    int32_t ret = CheckInputName(INPUT_INT, (void *)&choice);
    if (ret < 0) return HDF_FAILURE;
    switch (choice) {
        case ACODEC:
            snprintf_s(adapterNameCase, nameLen, nameLen - 1, "%s", "usb");
            break;
        case SMARTPA:
            snprintf_s(adapterNameCase, nameLen, nameLen - 1, "%s", "hdmi");
            break;
        default:
            printf("Input error,Switched to Acodec in for you,");
            SystemInputFail();
            snprintf_s(adapterNameCase, nameLen, nameLen - 1, "%s", "usb");
            break;
    }
    return HDF_SUCCESS;
}

int32_t SelectLoadingMode(char *resolvedPath, int32_t pathLen, char *func, int32_t funcpathLen)
{
    system("clear");
    int choice = 0;
    char *soPathHdi = NULL;
    char *soPathProxy = NULL;
    soPathHdi = HDF_LIBRARY_FULL_PATH("libhdi_audio");
    soPathProxy = HDF_LIBRARY_FULL_PATH("libhdi_audio_client");
    PrintMenu1();
    printf("Please enter your choice:");
    int32_t ret = CheckInputName(INPUT_INT, (void *)&choice);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    switch (choice) {
        case DIRECT:
            if (snprintf_s(resolvedPath, pathLen, pathLen - 1, "%s", soPathHdi) < 0) {
                return HDF_FAILURE;
            }
            if (snprintf_s(func, funcpathLen, funcpathLen - 1, "%s", "GetAudioManagerFuncs") < 0) {
                return HDF_FAILURE;
            }
            break;
        case SERVICE:
            if (snprintf_s(resolvedPath, pathLen, pathLen - 1, "%s", soPathProxy) < 0) {
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

int32_t GetRenderPassthroughManagerFunc(const char *adapterNameCase)
{
    if (adapterNameCase == NULL) {
        LOG_FUN_ERR("The Parameter is NULL");
        return HDF_FAILURE;
    }
    struct AudioManager *manager = NULL;
    struct AudioAdapterDescriptor *descs = NULL;
    enum AudioPortDirection port = PORT_OUT; // Set port information
    struct AudioPort renderPort;
    int32_t size = 0;
    int32_t ret;
    int32_t index;
    struct AudioManager *(*getAudioManager)(void) = NULL;
    getAudioManager = (struct AudioManager *(*)())(dlsym(g_handle, "GetAudioManagerFuncs"));
    if (getAudioManager == NULL) {
        LOG_FUN_ERR("Get Audio Manager Funcs Fail");
        return HDF_FAILURE;
    }
    manager = getAudioManager();
    ret = manager->GetAllAdapters(manager, &descs, &size);
    int32_t check = size > MAX_SUPPORT_ADAPTER_NUM || size == 0 || descs == NULL || ret < 0;
    if (check) {
        LOG_FUN_ERR("Get All Adapters Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    index = SwitchAdapter(descs, adapterNameCase, port, &renderPort, size);
    if (index < 0) {
        LOG_FUN_ERR("Not Switch Adapter Invalid");
        return HDF_ERR_NOT_SUPPORT;
    }
    struct AudioAdapterDescriptor *desc = &descs[index];
    if (manager->LoadAdapter(manager, desc, &g_adapter) != 0) {
        LOG_FUN_ERR("Load Adapter Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    g_manager = manager;
    if (g_adapter == NULL) {
        LOG_FUN_ERR("load audio device Invalid");
        return HDF_FAILURE;
    }
    (void)g_adapter->InitAllPorts(g_adapter);
    if (InitAttrs(&g_attrs) < 0) {
        g_manager->UnloadAdapter(g_manager, g_adapter);
        return HDF_FAILURE;
    }
    // Specify a hardware device
    if (InitDevDesc(&g_devDesc, renderPort.portId) < 0) {
        g_manager->UnloadAdapter(g_manager, g_adapter);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t GetRenderProxyManagerFunc(const char *adapterNameCase)
{
    if (adapterNameCase == NULL) {
        LOG_FUN_ERR("The Parameter is NULL");
        return HDF_FAILURE;
    }
    struct AudioProxyManager *proxyManager = NULL;
    struct AudioAdapterDescriptor *descs = NULL;
    enum AudioPortDirection port = PORT_OUT; // Set port information
    struct AudioPort renderPort;
    int32_t size = 0;
    int32_t ret;
    int32_t index;
    struct AudioProxyManager *(*getAudioManager)(void) = NULL;
    getAudioManager = (struct AudioProxyManager *(*)())(dlsym(g_handle, "GetAudioProxyManagerFuncs"));
    if (getAudioManager == NULL) {
        return HDF_FAILURE;
    }
    proxyManager = getAudioManager();
    ret = proxyManager->GetAllAdapters(proxyManager, &descs, &size);
    int32_t temp = size > MAX_SUPPORT_ADAPTER_NUM || size == 0 || descs == NULL || ret < 0;
    if (temp) {
        LOG_FUN_ERR("Get All Adapters Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    // Get qualified sound card and port
    index = SwitchAdapter(descs, adapterNameCase, port, &renderPort, size);
    if (index < 0) {
        LOG_FUN_ERR("Not Switch Adapter Fail");
        return HDF_ERR_NOT_SUPPORT;
    }
    struct AudioAdapterDescriptor *desc = &descs[index];
    if (proxyManager->LoadAdapter(proxyManager, desc, &g_adapter)) {
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
    if (InitAttrs(&g_attrs) < 0) {
        g_proxyManager->UnloadAdapter(g_proxyManager, g_adapter);
        return HDF_FAILURE;
    }
    // Specify a hardware device
    if (InitDevDesc(&g_devDesc, renderPort.portId) < 0) {
        g_proxyManager->UnloadAdapter(g_proxyManager, g_adapter);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t InitParam()
{
    /* Internal and external switch,begin */
    char adapterNameCase[PATH_LEN] = {0};
    if (SwitchInternalOrExternal(adapterNameCase, PATH_LEN) < 0) {
        return HDF_FAILURE;
    }
    char resolvedPath[PATH_LEN] = {0}; // Select loading mode,begin
    char func[PATH_LEN] = {0};
    if (SelectLoadingMode(resolvedPath, PATH_LEN, func, PATH_LEN) < 0) {
        return HDF_FAILURE;
    }
    /* Select loading mode,end */
    g_audioPort.dir = PORT_OUT;
    g_audioPort.portId = 0;
    g_audioPort.portName = "AOP";
    g_handle = dlopen(resolvedPath, 1);
    if (g_handle == NULL) {
        LOG_FUN_ERR("Open so Fail, reason:%s", dlerror());
        return HDF_FAILURE;
    }
    if (strcmp(func, "GetAudioManagerFuncs") == 0) {
        if (GetRenderPassthroughManagerFunc(adapterNameCase) < 0) {
            LOG_FUN_ERR("GetPassthroughManagerFunc Fail");
            return HDF_FAILURE;
        }
#ifdef AUDIO_HAL_USER
        char sdkResolvedPath[] = HDF_LIBRARY_FULL_PATH("libhdi_audio_interface_lib_render");
        g_sdkHandle = dlopen(sdkResolvedPath, 1);
        if (g_sdkHandle == NULL) {
            LOG_FUN_ERR("Open so Fail, reason:%s", dlerror());
            return HDF_FAILURE;
        }
        g_sdkInitSp = (int32_t (*)())(dlsym(g_sdkHandle, "MpiSdkInit"));
        if (g_sdkInitSp == NULL) {
            LOG_FUN_ERR("Get sdk init Funcs Fail");
            return HDF_FAILURE;
        }
        g_sdkExitSp = (void (*)())(dlsym(g_sdkHandle, "MpiSdkExit"));
        if (g_sdkExitSp == NULL) {
            LOG_FUN_ERR("Get sdk exit Funcs Fail");
            return HDF_FAILURE;
        }
        g_sdkInitSp();
#endif
    } else {
        if (GetRenderProxyManagerFunc(adapterNameCase) < 0) {
            LOG_FUN_ERR("GetProxyManagerFunc Fail");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t SetRenderMute()
{
    int32_t val;
    bool isMute = false;
    int32_t ret;
    if (g_render == NULL || g_render->volume.GetMute == NULL) {
        return HDF_FAILURE;
    }
    ret = g_render->volume.GetMute((AudioHandle)g_render, &isMute);
    if (ret < 0) {
        LOG_FUN_ERR("The current mute state was not obtained!");
    }
    printf("Now %s ,Do you need to set mute status(1/0):", isMute ? "mute" : "not mute");
    ret = CheckInputName(INPUT_INT, (void *)&val);
    if (ret < 0) {
        return HDF_FAILURE;
    }
    if (isMute != 0 && isMute != 1) {
        LOG_FUN_ERR("Invalid value!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_render == NULL || g_render->volume.SetMute == NULL) {
        LOG_FUN_ERR("Music already stop!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (val == 1) {
        ret = g_render->volume.SetMute((AudioHandle)g_render, !isMute);
    }
    return ret;
}

int32_t SetRenderVolume()
{
    int32_t ret;
    float val = 0.0;
    if (g_render == NULL || g_render->volume.GetVolume == NULL) {
        return HDF_FAILURE;
    }
    ret = g_render->volume.GetVolume((AudioHandle)g_render, &val);
    if (ret < 0) {
        LOG_FUN_ERR("Get current volume failed!");
        SystemInputFail();
        return ret;
    }
    printf("Now the volume is %f ,Please enter the volume value you want to set (0.0-1.0):", val);
    ret = CheckInputName(INPUT_FLOAT, (void *)&val);
    if (ret < 0) return HDF_FAILURE;
    if (val < 0.0 || val > 1.0) {
        LOG_FUN_ERR("Invalid volume value!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_render == NULL || g_render->volume.SetVolume == NULL) {
        LOG_FUN_ERR("Music already stop!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    ret = g_render->volume.SetVolume((AudioHandle)g_render, val);
    if (ret < 0) {
        LOG_FUN_ERR("set volume fail!");
        SystemInputFail();
    }
    return ret;
}

int32_t GetRenderGain()
{
    int32_t ret;
    float val = 1.0;
    if (g_render == NULL || g_render->volume.GetGain == NULL) {
        return HDF_FAILURE;
    }
    ret = g_render->volume.GetGain((AudioHandle)g_render, &val);
    if (ret < 0) {
        LOG_FUN_ERR("Get current gain failed!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    printf("Now the gain is %f,", val);
    SystemInputFail();
    return HDF_SUCCESS;
}

int32_t SetRenderPause()
{
    if (g_waitSleep) {
        LOG_FUN_ERR("Already pause,not need pause again!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    int32_t ret;
    if (g_render == NULL || g_render->control.Pause == NULL) {
        return HDF_FAILURE;
    }
    ret = g_render->control.Pause((AudioHandle)g_render);
    if (ret != 0) {
        return HDF_FAILURE;
    }
    printf("Pause success!\n");
    g_waitSleep = 1;
    return HDF_SUCCESS;
}
int32_t SetRenderResume()
{
    if (!g_waitSleep) {
        LOG_FUN_ERR("Now is Playing,not need resume!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    int32_t ret;
    if (g_render == NULL || g_render->control.Resume == NULL) {
        return HDF_FAILURE;
    }
    ret = g_render->control.Resume((AudioHandle)g_render);
    if (ret != 0) {
        return HDF_FAILURE;
    }
    printf("resume success!\n");
    pthread_mutex_lock(&g_mutex);
    g_waitSleep = 0;
    pthread_cond_signal(&g_functionCond);
    pthread_mutex_unlock(&g_mutex);
    return HDF_SUCCESS;
}
void PrintAttributesFromat()
{
    printf(" ============= Render Sample Attributes Fromat =============== \n");
    printf("| 1. Render AUDIO_FORMAT_PCM_8_BIT                            |\n");
    printf("| 2. Render AUDIO_FORMAT_PCM_16_BIT                           |\n");
    printf("| 3. Render AUDIO_FORMAT_PCM_24_BIT                           |\n");
    printf("| 4. Render AUDIO_FORMAT_PCM_32_BIT                           |\n");
    printf(" ============================================================= \n");
}
int32_t SelectAttributesFomat(uint32_t *fomat)
{
    if (fomat == NULL) {
        LOG_FUN_ERR("fomat is null!");
        return HDF_FAILURE;
    }
    PrintAttributesFromat();
    printf("Please select audio format,If not selected, the default is 16bit:");
    int32_t ret;
    int val = 0;
    ret = CheckInputName(INPUT_INT, (void *)&val);
    if (ret < 0) return HDF_FAILURE;
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
int32_t SetRenderAttributes()
{
    int32_t ret;
    struct AudioSampleAttributes attrs;
    if (g_render == NULL || g_render->attr.GetSampleAttributes == NULL) {
        LOG_FUN_ERR("The pointer is null!");
        return HDF_FAILURE;
    }
    ret = g_render->attr.GetSampleAttributes((AudioHandle)g_render, &attrs);
    if (ret < 0) {
        LOG_FUN_ERR("GetRenderAttributes failed!");
    } else {
        printf("Current sample attributes:\n");
        printf("audioType is %u\nfomat is %u\nsampleRate is %u\nchannalCount is"
            "%u\nperiod is %u\nframesize is %u\nbigEndian is %u\nSignedData is %u\n",
            attrs.type, attrs.format, attrs.sampleRate,
            attrs.channelCount, attrs.period, attrs.frameSize,
            attrs.isBigEndian, attrs.isSignedData);
    }
    printf("Set Sample Attributes,");
    SystemInputFail();
    system("clear");
    printf("The sample attributes you want to set,Step by step, please.\n");
    ret = SelectAttributesFomat((uint32_t *)(&attrs.format));
    if (ret < 0) {
        LOG_FUN_ERR("SetRenderAttributes format failed!");
        return HDF_FAILURE;
    }
    printf("\nPlease input sample rate(48000,44100,32000...):");
    ret = CheckInputName(INPUT_UINT32, (void *)(&attrs.sampleRate));
    if (ret < 0) {
        return HDF_FAILURE;
    }
    printf("\nPlease input bigEndian(false=0/true=1):");
    ret = CheckInputName(INPUT_UINT32, (void *)(&attrs.isBigEndian));
    if (ret < 0) {
        return HDF_FAILURE;
    }
    if (g_render == NULL || g_render->attr.SetSampleAttributes == NULL) {
        LOG_FUN_ERR("Music already complete,Please replay and set the attrbutes!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    ret = g_render->attr.SetSampleAttributes((AudioHandle)g_render, &attrs);
    if (ret < 0) {
        LOG_FUN_ERR("Set render attributes failed!");
        SystemInputFail();
    }
    return ret;
}

int32_t SelectRenderScene()
{
    system("clear");
    int32_t ret;
    int val = 0;
    struct AudioSceneDescriptor scene;
    printf(" =================== Select Scene ===================== \n");
    printf("0 is Speaker.                                          |\n");
    printf("1 is HeadPhones.                                       |\n");
    printf(" ====================================================== \n");
    printf("Please input your choice:\n");
    ret = CheckInputName(INPUT_INT, (void *)&val);
    if (ret < 0 || (val != 0 && val != 1)) {
        LOG_FUN_ERR("Invalid value!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (val == 1) {
        scene.scene.id = 0;
        scene.desc.pins = PIN_OUT_HEADSET;
    } else {
        scene.scene.id = 0;
        scene.desc.pins = PIN_OUT_SPEAKER;
    }
    if (g_render == NULL) {
        LOG_FUN_ERR("Music already stop,");
        SystemInputFail();
        return HDF_FAILURE;
    }
    if (g_render->scene.SelectScene == NULL) {
        return HDF_FAILURE;
    }
    ret = g_render->scene.SelectScene((AudioHandle)g_render, &scene);
    if (ret < 0) {
        LOG_FUN_ERR("Select scene fail\n");
    }
    return ret;
}
int32_t GetExtParams()
{
    char keyValueList[BUFFER_LEN] = {0};
    int32_t ret;
    if (g_render == NULL || g_render->attr.GetExtraParams == NULL) {
        return HDF_FAILURE;
    }
    ret = g_render->attr.GetExtraParams((AudioHandle)g_render, keyValueList, EXT_PARAMS_MAXLEN);
    if (ret < 0) {
        LOG_FUN_ERR("Get EXT params failed!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    printf("keyValueList = %s\n", keyValueList);
    return HDF_SUCCESS;
}

int32_t GetRenderMmapPosition()
{
    int32_t ret;
    if (g_render == NULL || g_render->attr.GetMmapPosition == NULL) {
        return HDF_FAILURE;
    }
    uint64_t frames = 0;
    struct AudioTimeStamp time;
    time.tvNSec = 0;
    time.tvSec = 0;
    ret = g_render->attr.GetMmapPosition((AudioHandle)g_render, &frames, &time);
    if (ret < 0) {
        LOG_FUN_ERR("Get current Mmap frames Position failed!");
        SystemInputFail();
        return HDF_FAILURE;
    }
    printf("Now the Position is %"PRIu64"\n", frames);
    return HDF_SUCCESS;
}

void PrintMenu2()
{
    printf(" ================== Play Render Menu ================== \n");
    printf("| 1. Render Start                                      |\n");
    printf("| 2. Render Stop                                       |\n");
    printf("| 3. Render Resume                                     |\n");
    printf("| 4. Render Pause                                      |\n");
    printf("| 5. Render SetVolume                                  |\n");
    printf("| 6. Render GetGain                                    |\n");
    printf("| 7. Render SetMute                                    |\n");
    printf("| 8. Render SetAttributes                              |\n");
    printf("| 9. Render SelectScene                                |\n");
    printf("| 10. Render getEXtParams                              |\n");
    printf("| 11. Render getMmapPosition                           |\n");
    printf("| 12.Exit                                              |\n");
    printf(" ====================================================== \n");
}

static struct ProcessRenderMenuSwitchList g_processRenderMenuSwitchList[] = {
    {RENDER_START, PlayingAudioFiles},
    {RENDER_STOP, StopAudioFiles},
    {RENDER_RESUME, SetRenderResume},
    {RENDER_PAUSE, SetRenderPause},
    {SET_RENDER_VOLUME, SetRenderVolume},
    {SET_RENDER_GAIN, GetRenderGain},
    {SET_RENDER_MUTE, SetRenderMute},
    {SET_RENDER_ATTRIBUTES, SetRenderAttributes},
    {SET_RENDER_SLECET_SCENE, SelectRenderScene},
    {GET_RENDER_EXT_PARAMS, GetExtParams},
    {GET_RENDER_POSITION, GetRenderMmapPosition},
};

void ProcessMenu(int32_t choice)
{
    int32_t i;
    if (choice == GET_RENDER_POSITION + 1) {
        LOG_FUN_ERR("Exit from application program!");
        return;
    }
    if (g_render == NULL && choice != 1) {
        LOG_FUN_ERR("This render already release!");
        SystemInputFail();
        return;
    }
    for (i = RENDER_START; i <= GET_RENDER_POSITION; ++i) {
        if ((choice == (int32_t)g_processRenderMenuSwitchList[i - 1].cmd) &&
            (g_processRenderMenuSwitchList[i - 1].operation != NULL)) {
            g_processRenderMenuSwitchList[i - 1].operation(&g_render);
        }
    }
}

void Choice(void)
{
    int32_t choice = 0;
    int ret;
    while (choice < GET_RENDER_POSITION + 1 && choice >= 0) {
        system("clear");
        PrintMenu2();
        printf("your choice is:\n");
        ret = CheckInputName(INPUT_INT, (void *)&choice);
        if (ret < 0) {
            continue;
        }
        if (choice < RENDER_START || choice > GET_RENDER_POSITION + 1) {
            LOG_FUN_ERR("You input is wrong!");
            choice = 0;
            SystemInputFail();
            continue;
        }
        ProcessMenu(choice);
    }
}

int32_t main(int32_t argc, char const *argv[])
{
    int32_t ret = 0;
    if (argc < 2 || argv == NULL || argv[0] == NULL) { // The parameter number is not greater than 2
        printf("usage:[1]sample [2]/data/test.wav\n");
        return 0;
    }
    if (argv[1] == NULL || strlen(argv[1]) == 0) {
        return HDF_FAILURE;
    }
    ret = strncpy_s(g_path, PATH_LEN, argv[1], strlen(argv[1]) + 1);
    if (ret != 0) {
        LOG_FUN_ERR("strncpy_s failed!");
        return HDF_FAILURE;
    }
    char pathBuf[PATH_MAX] = {'\0'};
    if (realpath(g_path, pathBuf) == NULL) {
        return HDF_FAILURE;
    }
    FILE *file = fopen(pathBuf, "rb");
    if (file == NULL) {
        printf("Failed to open '%s',Please enter the correct file name \n", g_path);
        return HDF_FAILURE;
    }
    fclose(file);
    bool soMode = false;
    if (InitParam()) { // init
        LOG_FUN_ERR("InitParam Fail!");
        return HDF_FAILURE;
    }

    Choice();
    soMode = PrepareStopAndUloadAdapter();
#ifdef AUDIO_HAL_USER
    if (soMode) {
        g_sdkExitSp();
        if (g_sdkHandle != NULL) {
            dlclose(g_sdkHandle);
        }
    }
#endif
    dlclose(g_handle);
    return 0;
}
