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

#include "config.h"
#include "alsa_audio.h"

#define VOLUME_CONTROL_NUMS (9)

char *g_volumeControlsNameTable[VOLUME_CONTROL_NUMS] = {
    "Earpiece Playback Volume",
    "Speaker Playback Volume",
    "DACL Playback Volume",
    "DACR Playback Volume",
    "DACL Capture Volume",
    "DACR Capture Volume",
    "Headphone Playback Volume",
    "PCM Playback Volume",
    "Mic Capture Volume",
};
const struct PathRouteCfgTable *g_tinyalsaRouteTable;
struct mixer* g_mixerPlayback;
struct mixer* g_mixerCapture;
#define PCM_DEVICE0   (0)
#define PCM_DEVICE2_P (5)
#define PCM_MAX (PCM_DEVICE2_P)
#define SOUND_CTL_PREFIX    "/dev/snd/controlC%d"

struct pcm* g_pcm[PCM_MAX + 1];
struct TinyalsaSndCardCfg g_sndCardCfgList[] = {
    {
        .sndCardName = "rockchiphdmi",
    }
};

int32_t IsPlaybackRoute(unsigned route)
{
    switch (route) {
        case DEV_IN_MAIN_MIC_CAPTURE_ROUTE:
        case DEV_IN_HANDS_FREE_MIC_CAPTURE_ROUTE:
            return false;
        case DEV_OUT_HDMI_NORMAL_ROUTE:
        case DEV_OUT_HEADPHONE_NORMAL_ROUTE:
        case DEV_OUT_SPEAKER_NORMAL_ROUTE:
        case DEV_OUT_SPEAKER_HEADPHONE_NORMAL_ROUTE:
            return true;
        default:
            LOG_FUN_ERR("IsPlaybackRoute() Error route %d", route);
            return -EINVAL;
    }
}

const struct PathRoute *get_route_config(unsigned route)
{
    LOG_PARA_INFO("get_route_config() route %d", route);

    if (!g_tinyalsaRouteTable) {
        LOG_FUN_ERR("get_route_config() pathRouteMap is NULL!");
        return NULL;
    }
    switch (route) {
        case DEV_OUT_HDMI_NORMAL_ROUTE:
            return &(g_tinyalsaRouteTable->HdmiNormal);
        case DEV_OUT_HEADPHONE_NORMAL_ROUTE:
            return &(g_tinyalsaRouteTable->headphoneNormal);
        case DEV_OUT_SPEAKER_NORMAL_ROUTE:
            return &(g_tinyalsaRouteTable->speakNormal);
        case DEV_OUT_SPEAKER_HEADPHONE_NORMAL_ROUTE:
            return &(g_tinyalsaRouteTable->speakerHeadphoneNormal);
        case DEV_IN_HANDS_FREE_MIC_CAPTURE_ROUTE:
            return &(g_tinyalsaRouteTable->handsFreeMicCapture);
        case DEV_IN_MAIN_MIC_CAPTURE_ROUTE:
            return &(g_tinyalsaRouteTable->mainMicCapture);
        default:
            LOG_FUN_ERR("get_route_config() Error route %d", route);
            return NULL;
    }
}

int32_t MixerCtlGetIntValue(struct mixer_ctl *contorl)
{
    struct snd_ctl_elem_value elemValue;
    int32_t ret;

    if (contorl == NULL || contorl->info == NULL) {
        return -EINVAL;
    }
    if (contorl->info->type != SNDRV_CTL_ELEM_TYPE_INTEGER) {
        return -EINVAL;
    }
    (void)memset_s(&elemValue, sizeof(elemValue), 0, sizeof(elemValue));
    elemValue.id.numid = contorl->info->id.numid;
    ret = ioctl(contorl->mixer->fd, SNDRV_CTL_IOCTL_ELEM_READ, &elemValue);
    if (ret < 0) {
        LOG_FUN_ERR("read contorls value failed");
        return ret;
    }
    return elemValue.value.integer.value[0];
}

struct mixer_ctl *mixer_get_control(struct mixer *mixer, const char *name)
{
    int32_t count = mixer->count;
    int32_t num = 0;
    while (num < count) {
        int32_t check = (mixer->info[num].id.index == 0) &&
            (!strcmp(name, (char*) mixer->info[num].id.name));
        if (check) {
            return mixer->ctl + num;
        }
        num++;
    }
    return NULL;
}

int32_t TinyalsaSetElemValue(int32_t fd, struct snd_ctl_elem_value *elemValue)
{
    int32_t ret = ioctl(fd, SNDRV_CTL_IOCTL_ELEM_WRITE, elemValue);
    if (ret < 0) {
        return -1;
    }
    return 0;
}
int32_t TinyalsaGetElemInfo(int32_t fd, struct snd_ctl_elem_info *elemInfo)
{
    int32_t ret = ioctl(fd, SNDRV_CTL_IOCTL_ELEM_INFO, elemInfo);
    if (ret < 0) {
        return -1;
    }
    return 0;
}
int32_t mixer_ctl_enumerated_select(struct mixer_ctl *ctl, const char *value)
{
    struct snd_ctl_elem_value ev;
    (void)memset_s(&ev, sizeof(ev), 0, sizeof(ev));
    if (ctl->info->type != SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
        return -1;
    }
    int32_t fd = ctl->mixer->fd;
    uint32_t max = ctl->info->value.enumerated.items;
    uint32_t num = 0;
    int32_t matchFalg = false;
    while (num < max) {
        if (!strcmp(value, ctl->ename[num])) {
            matchFalg = true;
            break;
        }
        num++;
    }
    if (!matchFalg) {
        return -1;
    }
    ev.value.enumerated.item[0] = num;
    ev.id.numid = ctl->info->id.numid;
    int32_t ret = TinyalsaSetElemValue(fd, &ev);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int32_t GetCtlMinValue(struct snd_ctl_elem_info *elemInfo, int64_t *min)
{
    snd_ctl_elem_type_t type =  elemInfo->type;
    if (type == SNDRV_CTL_ELEM_TYPE_BOOLEAN || type == SNDRV_CTL_ELEM_TYPE_INTEGER) {
        *min = elemInfo->value.integer.min;
    } else if (type == SNDRV_CTL_ELEM_TYPE_INTEGER64) {
        *min = elemInfo->value.integer64.min;
    } else {
        return -1;
    }
    return 0;
}

int32_t GetCtlMaxValue(struct snd_ctl_elem_info *elemInfo, int64_t *max)
{
    snd_ctl_elem_type_t type =  elemInfo->type;
    if (type == SNDRV_CTL_ELEM_TYPE_BOOLEAN || type == SNDRV_CTL_ELEM_TYPE_INTEGER) {
        *max = elemInfo->value.integer.max;
    } else if (type == SNDRV_CTL_ELEM_TYPE_INTEGER64) {
        *max = elemInfo->value.integer64.max;
    } else {
        return -1;
    }
    return 0;
}

int32_t mixer_get_ctl_minmax(struct mixer_ctl *ctl, int64_t *min, int64_t *max)
{
    if (ctl == NULL || ctl->info == NULL) {
        return -EINVAL;
    }
    struct snd_ctl_elem_info *elemInfo = ctl->info;
    int32_t ret = GetCtlMinValue(elemInfo, min);
    if (ret != 0) {
        return -EINVAL;
    }
    ret = GetCtlMaxValue(elemInfo, max);
    if (ret != 0) {
        return -EINVAL;
    }
    return 0;
}

void mixer_ctl_value_check(struct mixer_ctl *ctl, int64_t *value)
{
    int64_t min;
    int64_t max;
    int32_t ret = mixer_get_ctl_minmax(ctl, &min, &max);
    if (ret < 0) {
        LOG_FUN_ERR("mixer_ctl_value_check() mixer_get_ctl_minmax error");
        return;
    }
    if (*value > max) {
        *value = max;
    }
    if (*value < min) {
        *value = min;
    }
    return;
}

int32_t mixer_ctl_set_int_double(struct mixer_ctl *ctl, int64_t left, int64_t right)
{
    struct snd_ctl_elem_value elemValue;
    struct snd_ctl_elem_info *elemInfo = ctl->info;
    snd_ctl_elem_type_t type =  elemInfo->type;
    uint32_t n;

    (void)memset_s(&elemValue, sizeof(elemValue), 0, sizeof(elemValue));
    elemValue.id.numid = elemInfo->id.numid;
    mixer_ctl_value_check(ctl, &left);
    int64_t value = left;

    switch (type) {
        case SNDRV_CTL_ELEM_TYPE_BOOLEAN:
            for (n = 0; n < elemInfo->count; n++) {
                elemValue.value.integer.value[n] = !!value;
                mixer_ctl_value_check(ctl, &right);
                value = right;
            }
            break;
        case SNDRV_CTL_ELEM_TYPE_INTEGER: {
            for (n = 0; n < elemInfo->count; n++) {
                elemValue.value.integer.value[n] = (int32_t)value;
                mixer_ctl_value_check(ctl, &right);
                value = right;
            }
            break;
        }
        case SNDRV_CTL_ELEM_TYPE_INTEGER64: {
            for (n = 0; n < elemInfo->count; n++) {
                elemValue.value.integer64.value[n] = value;
                mixer_ctl_value_check(ctl, &right);
                value = right;
            }
            break;
        }
        case SNDRV_CTL_ELEM_TYPE_ENUMERATED:
            return mixer_ctl_enumerated_select(ctl, ctl->ename[value]);
        default:
            return -1;
    }
    return TinyalsaSetElemValue(ctl->mixer->fd, &elemValue);
}

int32_t set_controls(struct mixer *mixer, const struct RouteCfgInfo *ctls, const unsigned ctls_count)
{
    struct mixer_ctl *ctl;

    LOG_PARA_INFO("set_controls() ctls_count %d", ctls_count);
    if (!ctls || ctls_count <= 0) {
        LOG_FUN_ERR("set_controls() ctls is NULL");
        return 0;
    }

    for (int32_t i = 0; i < ctls_count; i++) {
        ctl = mixer_get_control(mixer, ctls[i].controlName);
        if (!ctl) {
            return -EINVAL;
        }

        if (ctl->info->type != SNDRV_CTL_ELEM_TYPE_BOOLEAN &&
            ctl->info->type != SNDRV_CTL_ELEM_TYPE_INTEGER &&
            ctl->info->type != SNDRV_CTL_ELEM_TYPE_INTEGER64 &&
            ctl->info->type != SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
            LOG_FUN_ERR("set_controls() ctl %s is not a type of INT or ENUMERATED", ctls[i].controlName);
            return -EINVAL;
        }

        if (ctls[i].stringVal) {
            if (ctl->info->type != SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
                LOG_FUN_ERR("set_controls() ctl %s is not a type of ENUMERATED", ctls[i].controlName);
                return -EINVAL;
            }
            if (mixer_ctl_enumerated_select(ctl, ctls[i].stringVal) != 0) {
                LOG_FUN_ERR("set_controls() Can not set ctl %s to %s", ctls[i].controlName, ctls[i].stringVal);
                return -EINVAL;
            }
            LOG_PARA_INFO("set_controls() set ctl %s to %s", ctls[i].controlName, ctls[i].stringVal);
        } else {
            if (mixer_ctl_set_int_double(ctl, ctls[i].intVal[0], ctls[i].intVal[1]) != 0) {
                LOG_FUN_ERR("set_controls() can not set ctl %s to %d", ctls[i].controlName, ctls[i].intVal[0]);
                return -EINVAL;
            }
            LOG_PARA_INFO("set_controls() set ctl %s to %d", ctls[i].controlName, ctls[i].intVal[0]);
        }
    }
    return 0;
}

#define TINYALSA_PATH_LEN  (256)
#define SND_CARD_ID_LEN    (32)
char* GetSndCardId(int32_t card, uint32_t *length)
{
    char path[TINYALSA_PATH_LEN] = {0};
    int32_t readLen = 0;
    char *sndCardId = (char *)malloc(SND_CARD_ID_LEN);
    if (sndCardId == NULL) {
        LOG_FUN_ERR("sndCardId malloc failed");
        return NULL;
    }
    (void)memset_s(sndCardId, SND_CARD_ID_LEN, 0, SND_CARD_ID_LEN);

    int32_t ret = sprintf_s(path, sizeof(path) - 1, "/proc/asound/card%d/id", card);
    if (ret < 0) {
        LOG_FUN_ERR("sndCardId path sprintf_s failed");
        free(sndCardId);
        sndCardId = NULL;
        return NULL;
    }
    FILE* fp = fopen(path, "rt");
    if (fp) {
        readLen = fread(sndCardId, sizeof(char), SND_CARD_ID_LEN, fp);
        if (fclose(fp)) {
            LOG_FUN_ERR("fclose(%s) failed", path);
        }
    }
    sndCardId[readLen - 1] = '\0';
    *length = readLen;
    return sndCardId;
}

void MatchRouteTableInfo(char *sndCardId, uint32_t *length)
{
    uint32_t cardIdLength = *length;
    unsigned count = sizeof(g_sndCardCfgList) / sizeof(struct TinyalsaSndCardCfg);
    for (int32_t i = 0; i < count; i++) {
        if (strncmp(g_sndCardCfgList[i].sndCardName, sndCardId, cardIdLength) == 0) {
            g_tinyalsaRouteTable = GetHdmiConfigTable();
            LOG_PARA_INFO("Get route table for sound card0 %s", sndCardId);
        }
    }

    if (!g_tinyalsaRouteTable) {
        g_tinyalsaRouteTable = GetDefaultConfigTable();
        LOG_FUN_ERR("Can not get config table for sound card0 %s, so get default config table.", sndCardId);
    }
    return;
}

int32_t SndCardRouteTableInit(int32_t card)
{
    uint32_t sndCardIdLength = 0;
    char *sndCardId = GetSndCardId(card, &sndCardIdLength);
    if (sndCardId == NULL) {
        LOG_FUN_ERR("getSndCardId failed");
        return -1;
    }

    LOG_FUN_ERR("Sound card%d is %s length is %d", card, sndCardId, sndCardIdLength);
    MatchRouteTableInfo(sndCardId, &sndCardIdLength);
    free(sndCardId);
    sndCardId = NULL;
    for (int32_t i = PCM_DEVICE0; i < PCM_MAX; i++) {
        g_pcm[i] = NULL;
    }
    return 0;
}

int32_t RouteSetControls(unsigned route)
{
    struct mixer* mMixer;

    if (route >= MAX_ROUTE) {
        LOG_FUN_ERR("route_set_controls() route %d error!", route);
        return -EINVAL;
    }

    LOG_PARA_INFO("route_set_controls() set route %d", route);
    mMixer = IsPlaybackRoute(route) ? g_mixerPlayback : g_mixerCapture;
    if (!mMixer) {
        LOG_FUN_ERR("route_set_controls() mMixer is NULL!");
        return -EINVAL;
    }

    const struct PathRoute *routeInfo = get_route_config(route);
    if (!routeInfo) {
        LOG_FUN_ERR("route_set_controls() Can not get config of route = %d", route);
        return -EINVAL;
    }

    if (routeInfo->ctlsNums > 0) {
        set_controls(mMixer, routeInfo->controls, routeInfo->ctlsNums);
    }
    return 0;
}
void free_mixer_ctl_ename(struct mixer_ctl *ctl)
{
    if (ctl->ename == NULL) {
        return;
    }
    uint32_t max = ctl->info->value.enumerated.items;
    uint32_t itemNums = 0;
    while (itemNums < max) {
        if (ctl->ename[itemNums]) {
            free(ctl->ename[itemNums]);
            ctl->ename[itemNums] = NULL;
        }
        itemNums++;
    }
    if (ctl->ename) {
        free(ctl->ename);
        ctl->ename = NULL;
    }
    return;
}

// mixer_ctl.tlv  mixer_ctl.ename
void FreeMixerCtlObject(struct mixer_ctl *ctl, uint32_t count)
{
    uint32_t ctlNums = 0;
    while (ctlNums < count) {
        if (ctl[ctlNums].tlv) {
            free(ctl[ctlNums].tlv);
            ctl[ctlNums].tlv = NULL;
        }
        free_mixer_ctl_ename(&(ctl[ctlNums]));
        ctlNums++;
    }
    return;
}

void mixer_close_legacy(struct mixer *mixer)
{
    if (mixer == NULL) {
        return;
    }
    // close fd
    if (mixer->fd >= 0) {
        close(mixer->fd);
    }
    // free mixer_ctl
    if (mixer->ctl) {
        FreeMixerCtlObject(mixer->ctl, mixer->count);
        free(mixer->ctl);
        mixer->ctl = NULL;
    }
    // free mixer_info
    if (mixer->info) {
        free(mixer->info);
        mixer->info = NULL;
    }
    // free mixer
    free(mixer);
    mixer = NULL;
    return;
}

struct mixer *MixerInit(uint32_t count)
{
    if (count <= 0) {
        LOG_FUN_ERR("count <=0, mixer->ctl and mixer->info can not malloc");
        return NULL;
    }
    struct mixer *mixer = calloc(1, sizeof(*mixer));
    if (!mixer) {
        return NULL;
    }
    mixer->ctl = calloc(count, sizeof(struct mixer_ctl));
    if (mixer->ctl == NULL) {
        mixer_close_legacy(mixer);
        return NULL;
    }
    mixer->info = calloc(count, sizeof(struct snd_ctl_elem_info));
    if (mixer->info == NULL) {
        mixer_close_legacy(mixer);
        return NULL;
    }
    return mixer;
}

int32_t CtleNamesInit(struct snd_ctl_elem_info *elemInfo, struct mixer *mixer, int32_t nums)
{
    struct snd_ctl_elem_info tempInfo;
    if (elemInfo->type != SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
        return 0;
    }
    uint32_t items = elemInfo->value.enumerated.items;
    if (items <= 0) {
        LOG_FUN_ERR("items <=0, **enames can not malloc");
        return NULL;
    }
    const int32_t enamesLen = 4; // 32bit pointer length is 4
    char **enames = calloc(items, enamesLen);
    if (enames == NULL) {
        LOG_FUN_ERR("enumerated.items name malloc failed");
        return -1;
    }

    mixer->ctl[nums].ename = enames;
    uint32_t i = 0;
    while (i < items) {
        (void)memset_s(&tempInfo, sizeof(tempInfo), 0, sizeof(tempInfo));
        tempInfo.id.numid = elemInfo->id.numid;
        tempInfo.value.enumerated.item = i;
        TinyalsaGetElemInfo(mixer->fd, &tempInfo);

        uint32_t nameLen = strlen(tempInfo.value.enumerated.name) + 1;
        enames[i] = (char *)malloc(nameLen);
        if (!enames[i]) {
            LOG_FUN_ERR("enumerated.items enames malloc failed");
            return -1;
        }
        (void)memset_s(enames[i], nameLen, 0, nameLen);
        int32_t ret = strncpy_s(enames[i], nameLen, tempInfo.value.enumerated.name, nameLen - 1);
        if (ret != 0) {
            free(enames[i]);
            return -1;
        }
        i++;
    }
    return 0;
}

int32_t MixerCtlsInit(struct mixer *mixer, struct snd_ctl_elem_id *elemId)
{
    uint32_t nums = 0;
    uint32_t count = mixer->count;
    struct snd_ctl_elem_info *elemInfo = NULL;
    while (nums < count) {
        elemInfo = mixer->info + nums;
        elemInfo->id.numid = elemId[nums].numid;
        int32_t ret = TinyalsaGetElemInfo(mixer->fd, elemInfo);
        if (ret < 0) {
            return -1;
        }
        mixer->ctl[nums].info = elemInfo;
        mixer->ctl[nums].mixer = mixer;
        ret = CtleNamesInit(elemInfo, mixer, nums);
        if (ret < 0) {
            return -1;
        }
        nums++;
    }
    return 0;
}

struct mixer *mixer_open_legacy(int32_t card)
{
    char dname[sizeof(SOUND_CTL_PREFIX) + 20];
    int32_t ret = sprintf_s(dname, sizeof(dname),  SOUND_CTL_PREFIX, card);
    if (ret < 0) {
        LOG_FUN_ERR("sound card contorl node path sprintf failed");
        return NULL;
    }
    int32_t fd = open(dname, O_RDWR);
    if (fd < 0) {
        LOG_FUN_ERR("Can not open %s for card %d", dname, card);
        return NULL;
    }
    struct snd_ctl_elem_list elemList;
    (void)memset_s(&elemList, sizeof(elemList), 0, sizeof(elemList));
    if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_LIST, &elemList) < 0) {
        close(fd);
        return NULL;
    }

    struct mixer *mixer = MixerInit(elemList.count);
    if (mixer == NULL) {
        close(fd);
        return NULL;
    }
    mixer->count = elemList.count;
    mixer->fd = fd;

    struct snd_ctl_elem_id *elemId = calloc(elemList.count, sizeof(struct snd_ctl_elem_id));
    if (elemId == NULL) {
        mixer_close_legacy(mixer);
        close(fd);
        return NULL;
    }
    elemList.space = mixer->count;
    elemList.pids = elemId;
    if (ioctl(fd, SNDRV_CTL_IOCTL_ELEM_LIST, &elemList) < 0) {
        free(elemId);
        mixer_close_legacy(mixer);
        close(fd);
        return NULL;
    }
    ret = MixerCtlsInit(mixer, elemId);
    if (ret < 0) {
        free(elemId);
        mixer_close_legacy(mixer);
        close(fd);
        return NULL;
    }
    free(elemId);
    return mixer;
}

int32_t RoutePcmClose(unsigned route)
{
    if (route != DEV_OFF_PLAYBACK_OFF_ROUTE &&
        route != DEV_OFF_CAPTURE_OFF_ROUTE) {
        LOG_FUN_ERR("RoutePcmClose() is not a off route");
        return 0;
    }
    // set controls
    if (IsPlaybackRoute(route) ? g_mixerPlayback : g_mixerCapture) {
        RouteSetControls(route);
    }
    // close mixer
    if (route == DEV_OFF_PLAYBACK_OFF_ROUTE) {
        if (g_mixerPlayback) {
            mixer_close_legacy(g_mixerPlayback);
            g_mixerPlayback = NULL;
        }
    } else if (route == DEV_OFF_CAPTURE_OFF_ROUTE) {
        if (g_mixerCapture) {
            mixer_close_legacy(g_mixerCapture);
            g_mixerCapture = NULL;
        }
    }
    return 0;
}

int32_t mixer_ctl_set_int(struct mixer_ctl *ctl, int64_t value)
{
    return mixer_ctl_set_int_double(ctl, value, value);
}

snd_ctl_elem_type_t mixer_ctl_get_value_prehandler(struct mixer_ctl *ctl,
    struct snd_ctl_elem_value *elemValue, uint32_t id)
{
    if (ctl == NULL) {
        return -1;
    }

    struct snd_ctl_elem_info *elemInfo = ctl->info;
    if ((elemInfo == NULL) || (elemInfo->count <= id)) {
        return -1;
    }
    snd_ctl_elem_type_t type =  elemInfo->type;
    (void)memset_s(elemValue, sizeof(struct snd_ctl_elem_value), 0, sizeof(struct snd_ctl_elem_value));
    elemValue->id.numid = elemInfo->id.numid;
    int32_t ret = ioctl(ctl->mixer->fd, SNDRV_CTL_IOCTL_ELEM_READ, elemValue);
    if (ret < 0) {
        return ret;
    }
    return type;
}
int32_t mixer_bool_ctl_get_value(struct mixer_ctl *ctl, uint32_t id)
{
    struct snd_ctl_elem_value elemValue;

    if (mixer_ctl_get_value_prehandler(ctl, &elemValue, id) != SNDRV_CTL_ELEM_TYPE_BOOLEAN) {
        return -1;
    }
    return !!elemValue.value.integer.value[id];
}

int32_t mixer_integer_ctl_get_value(struct mixer_ctl *ctl, uint32_t id)
{
    struct snd_ctl_elem_value elemValue;

    if (mixer_ctl_get_value_prehandler(ctl, &elemValue, id) != SNDRV_CTL_ELEM_TYPE_INTEGER) {
        return -1;
    }
    return elemValue.value.integer.value[id];
}

int32_t mixer_byte_ctl_get_value(struct mixer_ctl *ctl, uint32_t id)
{
    struct snd_ctl_elem_value elemValue;

    if (mixer_ctl_get_value_prehandler(ctl, &elemValue, id) != SNDRV_CTL_ELEM_TYPE_BOOLEAN) {
        return -1;
    }
    return elemValue.value.bytes.data[id];
}

int32_t mixer_enum_ctl_get_value(struct mixer_ctl *ctl, uint32_t id)
{
    struct snd_ctl_elem_value elemValue;

    if (mixer_ctl_get_value_prehandler(ctl, &elemValue, id) != SNDRV_CTL_ELEM_TYPE_ENUMERATED) {
        return -1;
    }
    return elemValue.value.enumerated.item[id];
}

int32_t set_voice_volume(const char *ctlName, float volume)
{
    struct mixer* mMixer = g_mixerPlayback;

    if (mMixer == NULL || ctlName[0] == '\0') {
        return 0;
    }
    struct mixer_ctl *ctl = mixer_get_control(mMixer, ctlName);
    if (ctl == NULL) {
        return 0;
    }

    LOG_PARA_INFO("set_voice_volume volume %f", volume);
    return mixer_ctl_set_int(ctl, volume);
}

int32_t set_capture_voice_volume(const char *ctlName, float volume)
{
    struct mixer* mMixer = g_mixerCapture;
    int64_t volMin;
    int64_t volMax;
    if (mMixer == NULL || ctlName[0] == '\0') {
        return 0;
    }
    struct mixer_ctl *ctl = mixer_get_control(mMixer, ctlName);
    if (ctl == NULL) {
        return 0;
    }
    if (mixer_get_ctl_minmax(ctl, &volMin, &volMax) < 0) {
        LOG_FUN_ERR("mixer_get_dB_range() get control min max value fail");
        return 0;
    }
    LOG_PARA_INFO("set_capture_voice_volume vol_min , vol_max");
    return mixer_ctl_set_int(ctl, volume);
}

int32_t RouteSetVoiceVolume(float volume)
{
    int32_t ret = -1;
    const char *mixer_l_ctl_name = "DACL Playback Volume";
    const char *mixer_r_ctl_name = "DACR Playback Volume";
    LOG_PARA_INFO("RouteSetVoiceVolume %f", volume);
    int32_t ret_l = set_voice_volume(mixer_l_ctl_name, volume);
    int32_t ret_r = set_voice_volume(mixer_r_ctl_name, volume);
    if (ret_l && ret_r) {
        ret = 0;
    }
    return ret;
}

int32_t RouteSetCaptureVoiceVolume(float volume)
{
    int32_t ret = -1;
    const char *mixer_l_ctl_name = "DACL Capture Volume";
    const char *mixer_r_ctl_name = "DACR Capture Volume";
    int32_t ret_l = set_capture_voice_volume(mixer_l_ctl_name, volume);
    int32_t ret_r = set_capture_voice_volume(mixer_r_ctl_name, volume);
    if (ret_l && ret_r) {
        LOG_FUN_ERR("RouteSetCaptureVoiceVolume fail");
        ret = 0;
    }
    return ret;
}

int32_t RouteGetVoiceVolume(char *ctlName, bool isPlayback)
{
    struct mixer* mixer = NULL;
    if (isPlayback) {
        mixer = g_mixerPlayback;
    } else {
        mixer = g_mixerCapture;
    }
    struct mixer_ctl *ctl = NULL;
    if (mixer == NULL || ctlName[0] == '\0') {
        return 0;
    }

    ctl = mixer_get_control(mixer, ctlName);
    if (ctl == NULL) {
        return 0;
    }
    return MixerCtlGetIntValue(ctl);
}

int32_t RouteGetVoiceMinMaxStep(int64_t *volMin, int64_t *volMax, char *ctlName, bool isPlayback)
{
    struct mixer* mMixer = NULL;
    if (isPlayback) {
        mMixer = g_mixerPlayback;
    } else {
        mMixer = g_mixerCapture;
    }
    if (mMixer == NULL || ctlName[0] == '\0') {
        return 0;
    }
    struct mixer_ctl *ctl = mixer_get_control(mMixer, ctlName);
    if (ctl == NULL) {
        return 0;
    }
    if (mixer_get_ctl_minmax(ctl, volMin, volMax) < 0) {
        LOG_FUN_ERR("mixer_get_dB_range() get control min max value fail");
        return 0;
    }
    return 1;
}

int32_t MixerOpenLegacy(bool isPlayBack, int32_t card)
{
    // update mMixer
    if (isPlayBack) {
        if (g_mixerPlayback == NULL) {
            g_mixerPlayback = mixer_open_legacy(card);
        }
    } else {
        if (g_mixerCapture == NULL) {
            g_mixerCapture = mixer_open_legacy(card);
        }
    }
    return 0;
}

void RoutePcmCardOpen(int32_t card, uint32_t route)
{
    int32_t isPlayback;

    if (route >= MAX_ROUTE) {
        LOG_FUN_ERR("route_pcm_card_open() route %d error!", route);
        return;
    }
    if (card < 0) {
        LOG_FUN_ERR("route_pcm_card_open() card %d error!", card);
        return;
    }

    LOG_PARA_INFO("route_pcm_card_open(card %d, route %d)", card, route);
    isPlayback = IsPlaybackRoute(route);
    if (!g_tinyalsaRouteTable) {
        SndCardRouteTableInit(card);
    }

    const struct PathRoute *routeInfo = get_route_config(route);
    if (!routeInfo) {
        LOG_FUN_ERR("route_pcm_open() Can not get config of route");
        return;
    }

    if (isPlayback) {
        RoutePcmClose(DEV_OFF_PLAYBACK_OFF_ROUTE);
    } else {
        RoutePcmClose(DEV_OFF_CAPTURE_OFF_ROUTE);
    }
    MixerOpenLegacy(isPlayback, card);
    // set controls
    if (routeInfo->ctlsNums > 0) {
        RouteSetControls(route);
    }
    return;
}
