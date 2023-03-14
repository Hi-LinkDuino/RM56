/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "alsa_audio.h"
static struct DevInfo devIn[SND_IN_SOUND_CARD_MAX];
static struct DevInfo devOut[SND_OUT_SOUND_CARD_MAX];

struct DevProcInfo SPEAKER_OUT_NAME[] = {
    {"realtekrt5616c", NULL},
    {"realtekrt5651co", "rt5651-aif1"},
    {"realtekrt5670c", NULL},
    {"realtekrt5672c", NULL},
    {"realtekrt5678co", NULL},
    {"rkhdmianalogsnd", NULL},
    {"rockchipcx2072x", NULL},
    {"rockchipes8316c", NULL},
    {"rockchipes8323c", NULL},
    {"rockchipes8388c", NULL},
    {"rockchipes8396c", NULL},
    {"rockchiprk", NULL},
    {"rockchiprk809co", NULL},
    {"rockchiprk817co", NULL},
    {"rockchiprt5640c", "rt5640-aif1"},
    {"rockchiprt5670c", NULL},
    {"rockchiprt5672c", NULL},
    {NULL, NULL}, /* Note! Must end with NULL, else will cause crash */
};

struct DevProcInfo HDMI_OUT_NAME[] = {
    {"realtekrt5651co", "i2s-hifi"},
    {"realtekrt5670co", "i2s-hifi"},
    {"rkhdmidpsound", NULL},
    {"hdmisound", NULL},
    {"rockchiphdmi", NULL},
    {"rockchiprt5640c", "i2s-hifi"},
    {NULL, NULL}, /* Note! Must end with NULL, else will cause crash */
};


struct DevProcInfo SPDIF_OUT_NAME[] = {
    {"ROCKCHIPSPDIF", "dit-hifi"},
    {"rockchipspdif", NULL},
    {"rockchipcdndp", NULL},
    {NULL, NULL}, /* Note! Must end with NULL, else will cause crash */
};

struct DevProcInfo BT_OUT_NAME[] = {
    {"rockchipbt", NULL},
    {NULL, NULL}, /* Note! Must end with NULL, else will cause crash */
};

struct DevProcInfo MIC_IN_NAME[] = {
    {"realtekrt5616c", NULL},
    {"realtekrt5651co", "rt5651-aif1"},
    {"realtekrt5670c", NULL},
    {"realtekrt5672c", NULL},
    {"realtekrt5678co", NULL},
    {"rockchipes8316c", NULL},
    {"rockchipes8323c", NULL},
    {"rockchipes8396c", NULL},
    {"rockchipes7210", NULL},
    {"rockchipes7243", NULL},
    {"rockchiprk", NULL},
    {"rockchiprk809co", NULL},
    {"rockchiprk817co", NULL},
    {"rockchiprt5640c", NULL},
    {"rockchiprt5670c", NULL},
    {"rockchiprt5672c", NULL},
    {NULL, NULL}, /* Note! Must end with NULL, else will cause crash */
};


struct DevProcInfo HDMI_IN_NAME[] = {
    {"realtekrt5651co", "tc358749x-audio"},
    {"hdmiin", NULL},
    {NULL, NULL}, /* Note! Must end with NULL, else will cause crash */
};

struct DevProcInfo BT_IN_NAME[] = {
    {"rockchipbt", NULL},
    {NULL, NULL}, /* Note! Must end with NULL, else will cause crash */
};
static struct pcm_config g_renderPcmCfg;

struct RenderParamcheckList {
    enum pcm_param param;
    uint32_t value;
    char *paramName;
    char *paramUnit;
};
static void SetDefaultDevInfo(struct DevInfo *info, int32_t size, int32_t rid)
{
    for (int32_t i = 0; i < size; i++) {
        if (rid) {
            info[i].id = NULL;
        }
        info[i].card = (int32_t)SND_OUT_SOUND_CARD_UNKNOWN;
    }
}

static int32_t name_match(const char* dst, const char* src)
{
    int32_t score = 0;
    if (!strcmp(dst, src)) {
        score = 100; // match 100%, total equal
    } else  if (strstr(dst, src)) {
        score = 50; //  match 50%, part equal
    }

    return score;
}

static bool dev_id_match(const char *info, const char *did)
{
    const char *deli = "id:";
    char *id;
    int32_t idx = 0;

    if (!did) {
        return true;
    }
    if (!info) {
        return false;
    }
    /* find str like-> id: ff880000.i2s-rt5651-aif1 rt5651-aif1-0 */
    id = strstr(info, deli);
    if (!id) {
        return false;
    }
    id += strlen(deli);
    while (id[idx] != '\0') {
        if (id[idx] == '\r' || id[idx] == '\n') {
            id[idx] = '\0';
            break;
        }
        idx++;
    }
    if (strstr(id, did)) {
        LOG_PARA_INFO("match dai!!!: %s %s", id, did);
        return true;
    }
    return false;
}

static bool GetSpecifiedDevicesCheck(struct DevInfo *devinfo, int32_t card,
    const char *id, struct DevProcInfo *match, int32_t *index)
{
    int32_t i = 0;
    int32_t better = 0;
    /* parse card id */
    if (!match) {
        return true; /* match any */
    }
    while (match[i].cid) {
        int32_t score = name_match(id, match[i].cid);
        if (score > better) {
            better = score;
            *index = i;
        }
        i++;
    }

    if (*index < 0) {
        return false;
    }
    if (!match[*index].cid) {
        return false;
    }
    if (!match[*index].did) { /* no exist dai info, exit */
        devinfo->card = card;
        devinfo->device = 0;
        LOG_PARA_INFO("%s card, got card=%d,device=%d", devinfo->id,
            devinfo->card, devinfo->device);
        return true;
    }
    return true;
}

static bool GetSpecifiedOutDev(struct DevInfo *devinfo, int32_t card,
    const char *id, struct DevProcInfo *match)
{
    int32_t device;
    char deviceInfoPath[32];
    char info[256];
    size_t len;
    FILE* file = NULL;
    int32_t index = -1;
    bool ret = GetSpecifiedDevicesCheck(devinfo, card, id, match, &index);
    if (!ret) {
        return false;
    }
    /* parse device id */
    for (device = 0; device < SNDRV_DEVICES; device++) {
        int32_t ret = sprintf_s(deviceInfoPath, sizeof(deviceInfoPath) - 1,
            "proc/asound/card%d/pcm%dp/info", card, device);
        if (ret < 0) {
            LOG_PARA_INFO("out card %d devices %d info path sprintf failed", card, device);
            break;
        }
        if (access(deviceInfoPath, 0)) {
            LOG_PARA_INFO("No exist %s, break and finish parsing", deviceInfoPath);
            break;
        }
        file = fopen(deviceInfoPath, "r");
        if (!file) {
            LOG_PARA_INFO("Could reading %s property", deviceInfoPath);
            continue;
        }
        len = fread(info, sizeof(char), sizeof(info) / sizeof(char), file);
        if (fclose(file)) {
            LOG_FUN_ERR("fclose(%s) failed", deviceInfoPath);
        }
        if (len == 0 || len > sizeof(info) / sizeof(char)) {
            continue;
        }
        info[len - 1] = '\0';
        /* parse device dai */
        if (dev_id_match(info, match[index].did)) {
            devinfo->card = card;
            devinfo->device = device;
            LOG_PARA_INFO("%s card, got card=%d,device=%d", devinfo->id,
                devinfo->card, devinfo->device);
            return true;
        }
    }
    return false;
}

static bool GetSpecifiedInDev(struct DevInfo *devinfo, int32_t card,
    const char *id, struct DevProcInfo *match)
{
    int32_t device;
    char deviceInfoPath[32];
    char info[256];
    size_t len;
    FILE* file = NULL;
    int32_t index = -1;
    bool ret = GetSpecifiedDevicesCheck(devinfo, card, id, match, &index);
    if (!ret) {
        return false;
    }
    /* parse device id */
    for (device = 0; device < SNDRV_DEVICES; device++) {
        int32_t ret = sprintf_s(deviceInfoPath, sizeof(deviceInfoPath) - 1,
            "proc/asound/card%d/pcm%dc/info", card, device);
        if (ret < 0) {
            LOG_PARA_INFO("in card %d devices %d info path sprintf failed", card, device);
            break;
        }
        if (access(deviceInfoPath, 0)) {
            LOG_PARA_INFO("No exist %s, break and finish parsing", deviceInfoPath);
            break;
        }
        file = fopen(deviceInfoPath, "r");
        if (!file) {
            LOG_PARA_INFO("Could reading %s property", deviceInfoPath);
            continue;
        }
        len = fread(info, sizeof(char), sizeof(info) / sizeof(char), file);
        if (fclose(file)) {
            LOG_FUN_ERR("fclose(%s) failed", deviceInfoPath);
        }
        if (len == 0 || len > sizeof(info) / sizeof(char)) {
            continue;
        }
        info[len - 1] = '\0';
        /* parse device dai */
        if (dev_id_match(info, match[index].did)) {
            devinfo->card = card;
            devinfo->device = device;
            LOG_PARA_INFO("%s card, got card=%d,device=%d", devinfo->id,
                devinfo->card, devinfo->device);
            return true;
        }
    }
    return false;
}

static void dumpdev_info(const char *tag, struct DevInfo  *devInfo, int32_t count)
{
    LOG_PARA_INFO("dump %s device info", tag);
    for (int32_t i = 0; i < count; i++) {
        if (devInfo[i].id && devInfo[i].card != SND_OUT_SOUND_CARD_UNKNOWN) {
            LOG_PARA_INFO("dev_info %s  card=%d, device:%d",
                devInfo[i].id, devInfo[i].card, devInfo[i].device);
        }
    }
}

void ReadInSoundCard(void)
{
    char sndCardId[32];
    char CardIdInfo[20];
    size_t cardIdLen;
    FILE* file = NULL;
    devIn[SND_IN_SOUND_CARD_MIC].id = "MIC";
    devIn[SND_IN_SOUND_CARD_BT].id = "BT";
    SetDefaultDevInfo(devIn, SND_IN_SOUND_CARD_UNKNOWN, 0);
    for (int32_t card = 0; card < SNDRV_CARDS; card++) {
        int32_t ret = sprintf_s(sndCardId, sizeof(sndCardId) - 1, "proc/asound/card%d/id", card);
        if (ret < 0) {
            LOG_PARA_INFO("in card %d idinfo path sprintf failed", card);
            break;
        }
        if (access(sndCardId, 0)) {
            LOG_PARA_INFO("No exist %s, break and finish parsing", sndCardId);
                break;
        }
        file = fopen(sndCardId, "r");
        if (!file) {
            LOG_PARA_INFO("Could reading %s property", sndCardId);
            continue;
        }
        cardIdLen = fread(CardIdInfo, sizeof(char), sizeof(CardIdInfo) / sizeof(char), file);
        if (fclose(file)) {
            LOG_FUN_ERR("fclose(%s) failed", sndCardId);
        }
        if (cardIdLen == 0 || cardIdLen > sizeof(CardIdInfo) / sizeof(char)) {
            continue;
        }
        CardIdInfo[cardIdLen - 1] = '\0';
        GetSpecifiedInDev(&devIn[SND_IN_SOUND_CARD_MIC], card, CardIdInfo, MIC_IN_NAME);
        /* set HDMI audio input info if need hdmi audio input */
        GetSpecifiedInDev(&devIn[SND_IN_SOUND_CARD_HDMI], card, CardIdInfo, HDMI_IN_NAME);
        GetSpecifiedInDev(&devIn[SND_IN_SOUND_CARD_BT], card, CardIdInfo, BT_IN_NAME);
    }
    dumpdev_info("in", devIn, SND_IN_SOUND_CARD_MAX);
    return;
}

void ReadOutSoundCard(void)
{
    char sndCardId[32];
    char CardIdInfo[20];
    size_t cardIdLen;
    FILE* file = NULL;
    devOut[SND_OUT_SOUND_CARD_SPEAKER].id = "SPEAKER";
    devOut[SND_OUT_SOUND_CARD_HDMI].id = "HDMI";
    devOut[SND_OUT_SOUND_CARD_SPDIF].id = "SPDIF";
    devOut[SND_OUT_SOUND_CARD_BT].id = "BT";
    SetDefaultDevInfo(devOut, SND_OUT_SOUND_CARD_UNKNOWN, 0);
    for (int32_t card = 0; card < SNDRV_CARDS; card++) {
        int32_t ret = sprintf_s(sndCardId, sizeof(sndCardId) - 1, "proc/asound/card%d/id", card);
        if (ret < 0) {
            LOG_PARA_INFO("out card %d idinfo path sprintf failed", card);
            break;
        }
        if (access(sndCardId, 0)) {
            LOG_PARA_INFO("No exist %s, break and finish parsing", sndCardId);
            break;
        }
        file = fopen(sndCardId, "r");
        if (!file) {
            LOG_PARA_INFO("Could reading %s property", sndCardId);
            continue;
        }
        cardIdLen = fread(CardIdInfo, sizeof(char), sizeof(CardIdInfo) / sizeof(char), file);
        if (fclose(file)) {
            LOG_FUN_ERR("fclose(%s) failed", sndCardId);
        }
        if (cardIdLen == 0 || cardIdLen > sizeof(CardIdInfo) / sizeof(char)) {
            continue;
        }
        CardIdInfo[cardIdLen - 1] = '\0';
        LOG_PARA_INFO("card%d id:%s", card, CardIdInfo);
        GetSpecifiedOutDev(&devOut[SND_OUT_SOUND_CARD_SPEAKER], card, CardIdInfo, SPEAKER_OUT_NAME);
        GetSpecifiedOutDev(&devOut[SND_OUT_SOUND_CARD_HDMI], card, CardIdInfo, HDMI_OUT_NAME);
        GetSpecifiedOutDev(&devOut[SND_OUT_SOUND_CARD_SPDIF], card, CardIdInfo, SPDIF_OUT_NAME);
        GetSpecifiedOutDev(&devOut[SND_OUT_SOUND_CARD_BT], card, CardIdInfo, BT_OUT_NAME);
    }
    dumpdev_info("out", devOut, SND_OUT_SOUND_CARD_MAX);
    return;
}

int32_t GetOutDevInfo(int32_t index, struct DevInfo* devInfo)
{
    for (int32_t i = 0; i < SND_OUT_SOUND_CARD_MAX; i++) {
        if (i == index) {
            devInfo->card = devOut[i].card;
            devInfo->device = devOut[i].device;
            return true;
        }
    }
    return false;
}

int32_t GetInDevInfo(int32_t index, struct DevInfo* devInfo)
{
    for (int32_t i = 0; i < SND_IN_SOUND_CARD_MAX; i++) {
        if (i == index) {
            devInfo->card = devIn[i].card;
            devInfo->device = devIn[i].device;
            return true;
        }
    }
    return false;
}

int32_t AudioRenderParamCheck(struct pcm_params *params, uint32_t param, uint32_t value,
    char *paramName, char *paramUnit)
{
    int32_t paramIsOk = true;

    uint32_t min = pcm_params_get_min(params, param);
    uint32_t max = pcm_params_get_max(params, param);
    if ((value < min) || (value > max))  {
        LOG_FUN_ERR("device supports %s\t min=%u%s \t max=%u%s, pls check it.\n", paramName,
            min, paramUnit, max, paramUnit);
        paramIsOk = false;
    }

    return paramIsOk;
}

int32_t IsPlayable(uint32_t card, uint32_t device, uint32_t bits)
{
    struct pcm_params *params;
    int32_t paramIsOk = true;

    params = pcm_params_get(card, device, PCM_OUT);
    if (params == NULL) {
        LOG_FUN_ERR("Unable to open PCM device (/dev/snd/pcmC%uD%up)\n", card, device);
        return 0;
    }
    struct RenderParamcheckList g_RenderParamcheckList[] = {
        {PCM_PARAM_RATE, g_renderPcmCfg.rate, "Sample rate", "Hz"},
        {PCM_PARAM_CHANNELS, g_renderPcmCfg.channels, "Sample", "channels"},
        {PCM_PARAM_SAMPLE_BITS, bits, "Bitrate", "bits"},
        {PCM_PARAM_PERIOD_SIZE, g_renderPcmCfg.period_size, "Period size", "frames"},
        {PCM_PARAM_PERIODS, g_renderPcmCfg.period_count, "Period count", "periods"},
    };
    int32_t checkNums = sizeof(g_RenderParamcheckList) / sizeof(struct RenderParamcheckList);
    for (int32_t i = 0; i < checkNums; i++) {
        paramIsOk &= AudioRenderParamCheck(params, g_RenderParamcheckList[i].param, g_RenderParamcheckList[i].value,
            g_RenderParamcheckList[i].paramName, g_RenderParamcheckList[i].paramUnit);
    }

    pcm_params_free(params);
    return paramIsOk;
}

void initRenderFormat(uint32_t bits)
{
    switch (bits) {
        case TINYALSAPCM_32_BIT:
            g_renderPcmCfg.format = PCM_FORMAT_S32_LE;
            return;
        case TINYALSAPCM_24_BIT:
            g_renderPcmCfg.format = PCM_FORMAT_S24_3LE;
            return;
        case TINYALSAPCM_8_BIT:
            g_renderPcmCfg.format = PCM_FORMAT_S8;
            return;
        default:
        case TINYALSAPCM_16_BIT:
            g_renderPcmCfg.format = PCM_FORMAT_S16_LE;
            return;
    }
}

void TinyAlsaPlayParamInit(uint32_t channels, uint32_t rate,
    uint32_t periodSize, uint32_t periodCount)
{
    (void)memset_s(&g_renderPcmCfg, sizeof(struct pcm_config), 0, sizeof(struct pcm_config));
    g_renderPcmCfg.channels = channels;
    g_renderPcmCfg.rate = rate;
    g_renderPcmCfg.period_size = periodSize;
    g_renderPcmCfg.period_count = periodCount;

    g_renderPcmCfg.start_threshold = 0;
    g_renderPcmCfg.stop_threshold = 0;
    g_renderPcmCfg.silence_threshold = 0;
    return;
}

void RenderSample(struct pcm **pcm, struct PcmRenderParam *param)
{
    TinyAlsaPlayParamInit(param->channels, param->rate, param->periodSize, param->periodCount);
    initRenderFormat(param->bits);
    if (!IsPlayable(param->card, param->device, param->bits)) {
        return;
    }

    *pcm = pcm_open(param->card, param->device, PCM_OUT, &g_renderPcmCfg);
    if (((*pcm) == NULL) || !pcm_is_ready(*pcm)) {
        LOG_FUN_ERR("Unable to open PCM device (/dev/snd/pcmC%uD%up):(%s)\n",
            param->card, param->device, pcm_get_error(*pcm));
        return;
    }
    LOG_FUN_ERR("Playing sample: %u ch, %u hz, %u\n", param->channels, param->rate, param->bits);
}

uint32_t CaptureSample(struct pcm **pcm, struct PcmCaptureParam *param)
{
    struct pcm_config config;
    (void)memset_s(&config, sizeof(config), 0, sizeof(config));
    config.channels = param->channels;
    config.rate = param->rate;
    config.period_size = param->periodSize;
    config.period_count = param->periodCount;
    config.format = param->format;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
    *pcm = pcm_open(param->card, param->device, PCM_IN, &config);
    if (((*pcm) == NULL) || !pcm_is_ready(*pcm)) {
        LOG_FUN_ERR("Unable to open PCM device (/dev/snd/pcmC%uD%uc):(%s)\n",
            param->card, param->device, pcm_get_error(*pcm));
        return -1;
    }
    return 0;
}
