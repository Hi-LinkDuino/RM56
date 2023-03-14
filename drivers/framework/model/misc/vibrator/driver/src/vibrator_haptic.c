/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "vibrator_haptic.h"
#include <securec.h>
#include "device_resource_if.h"
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_mem.h"
#include "vibrator_driver.h"

#define HDF_LOG_TAG    hdf_vibrator_driver

#define VIBRATOR_HAPTIC_STACK_SIZE    0x4000
#define VIBRATOR_HAPTIC_SEQ_MAX       1024
#define VIBRATOR_HAPTIC_SEQ_SIZE       4
#define VIBRATOR_HAPTIC_SEQ_NAME_MAX    48
#define VIBRATOR_HAPTIC_SEQ_SIZE_MAX    (4 * VIBRATOR_HAPTIC_SEQ_MAX)

struct VibratorHapticData *g_vibratorHapticData = NULL;

static struct VibratorHapticData *GetHapticData(void)
{
    return g_vibratorHapticData;
}

static struct VibratorEffectNode *MallocEffectNode(int32_t seqSize)
{
    uint32_t *seq = NULL;
    struct VibratorEffectNode *node = NULL;

    if (seqSize <= 0 || seqSize > VIBRATOR_HAPTIC_SEQ_SIZE_MAX) {
        HDF_LOGE("%s: malloc ", __func__);
        return NULL;
    }

    seq = (uint32_t *)OsalMemCalloc(seqSize);
    if (seq == NULL) {
        HDF_LOGE("%s: malloc seq fail", __func__);
        return NULL;
    }

    node = (struct VibratorEffectNode *)OsalMemCalloc(sizeof(*node));
    if (node == NULL) {
        HDF_LOGE("%s: malloc seq fail", __func__);
        OsalMemFree(seq);
        return NULL;
    }
    node->seq = seq;

    return node;
}

static int32_t ParserHapticEffect(struct DeviceResourceIface *parser, const struct DeviceResourceNode *hapticNode)
{
    int32_t ret;
    int32_t count;
    struct VibratorEffectNode *effectNode = NULL;
    const struct DeviceResourceNode *childNode = NULL;
    struct VibratorHapticData *hapticData = GetHapticData();

    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);

    (void)OsalMutexLock(&hapticData->mutex);
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(hapticNode, childNode) {
        if ((childNode == NULL) || (childNode->name == NULL)) {
            break;
        }

        count = parser->GetElemNum(childNode, "seq");
        if (count <= 1 || count > VIBRATOR_HAPTIC_SEQ_MAX) {
            HDF_LOGE("%s: haptic [%s] parser seq count fail", __func__, childNode->name);
            continue;
        }

        effectNode = MallocEffectNode(count * VIBRATOR_HAPTIC_SEQ_SIZE);
        if (effectNode == NULL) {
            HDF_LOGE("%s: malloc effect effectNode fail", __func__);
            continue;
        }
        effectNode->num = count;
        ret = parser->GetString(childNode, "effectName", &effectNode->effect, NULL);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: parser %s effectName failed", __func__, "effectName");
            OsalMemFree(effectNode->seq);
            OsalMemFree(effectNode);
            return HDF_FAILURE;
        }
        ret = parser->GetUint32(childNode, "type", &effectNode->type, 0);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: parser %s type failed", __func__, "type");
            OsalMemFree(effectNode->seq);
            OsalMemFree(effectNode);
            return HDF_FAILURE;
        }

        ret = parser->GetUint32Array(childNode, "seq", effectNode->seq, count, 0);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: parser %s seq failed", __func__, "seq");
            OsalMemFree(effectNode->seq);
            OsalMemFree(effectNode);
            return HDF_FAILURE;
        }

        DListInsertTail(&effectNode->node, &hapticData->effectSeqHead);
    }
    (void)OsalMutexUnlock(&hapticData->mutex);

    if (DListIsEmpty(&hapticData->effectSeqHead)) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t ParserVibratorHapticConfig(const struct DeviceResourceNode *node)
{
    bool supportPresetFlag = false;
    struct DeviceResourceIface *parser = NULL;
    const struct DeviceResourceNode *vibratorAttrNode = NULL;
    const struct DeviceResourceNode *vibratorHapticNode = NULL;
    struct VibratorHapticData *hapticData = GetHapticData();

    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(node, HDF_ERR_INVALID_PARAM);

    parser = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(parser, HDF_ERR_INVALID_PARAM);
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(parser->GetChildNode, HDF_ERR_INVALID_PARAM);

    // get haptic type
    vibratorAttrNode = parser->GetChildNode(node, "vibratorAttr");
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(vibratorAttrNode, HDF_ERR_INVALID_PARAM);
    supportPresetFlag = parser->GetBool(vibratorAttrNode, "supportPreset");

    (void)OsalMutexLock(&hapticData->mutex);
    hapticData->supportHaptic = supportPresetFlag;
    (void)OsalMutexUnlock(&hapticData->mutex);

    if (!supportPresetFlag) {
        HDF_LOGD("%s: vibrator not support effect", __func__);
        return HDF_SUCCESS;
    }

    // malloc haptic resource
    vibratorHapticNode = parser->GetChildNode(node, "vibratorHapticConfig");
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(vibratorHapticNode, HDF_ERR_INVALID_PARAM);
    if (ParserHapticEffect(parser, vibratorHapticNode) != HDF_SUCCESS) {
        HDF_LOGE("%s: vibrator get effect fail", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static uint32_t ProcessHapticTime(struct VibratorHapticData *hapticData)
{
    uint32_t duration;
    int32_t num = 2; // Determine whether the number is an even numner or an odd number.

    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);
    if ((hapticData->currentSeqIndex < 0) || (hapticData->currentSeqIndex >= hapticData->seqCount)) {
        return 0;
    }

    if (hapticData->currentSeqIndex % num == 0) {
        StartTimeVibrator();
    } else {
        StopVibrator();
    }

    hapticData->currentSeqIndex++;
    if (hapticData->currentSeqIndex >= hapticData->seqCount) {
        return 0;
    }

    duration = hapticData->currentEffectSeq[hapticData->currentSeqIndex] == 0 ?
        VIBRATOR_MIN_WAIT_TIME : hapticData->currentEffectSeq[hapticData->currentSeqIndex];
    return duration;
}

static uint32_t ProcessHapticEffect(struct VibratorHapticData *hapticData)
{
    uint32_t effect;
    uint32_t duration;

    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);

    if ((hapticData->currentSeqIndex < 0) || ((hapticData->currentSeqIndex + 1) >= hapticData->seqCount)) {
        HDF_LOGE("%s: seq index invalid para", __func__);
        return 0;
    }

    hapticData->currentSeqIndex++;
    effect = hapticData->currentEffectSeq[hapticData->currentSeqIndex];
    SetEffectVibrator(effect);

    hapticData->currentSeqIndex++;
    if (hapticData->currentSeqIndex >= hapticData->seqCount) {
        HDF_LOGE("%s: seq index exceed max value", __func__);
        return 0;
    }

    duration = hapticData->currentEffectSeq[hapticData->currentSeqIndex] == 0 ?
        VIBRATOR_MIN_WAIT_TIME : hapticData->currentEffectSeq[hapticData->currentSeqIndex];
    return duration;
}

void HapticTimerEntry(uintptr_t para)
{
    int32_t ret;
    struct VibratorHapticData *hapticData = NULL;
    uint32_t duration;

    hapticData = (struct VibratorHapticData *)para;
    CHECK_VIBRATOR_NULL_PTR_RETURN(hapticData);

    if (hapticData->effectType == VIBRATOR_TYPE_TIME) {
        duration = ProcessHapticTime(hapticData);
    }

    if (hapticData->effectType == VIBRATOR_TYPE_EFFECT) {
        duration = ProcessHapticEffect(hapticData);
    }

    duration = ((duration > 0) && (duration < VIBRATOR_MIN_WAIT_TIME)) ? VIBRATOR_MIN_WAIT_TIME : duration;
    if ((duration > 0) && (OsalTimerSetTimeout(&hapticData->timer, duration) == HDF_SUCCESS)) {
        return;
    }

    if (hapticData->timer.realTimer != NULL) {
        ret = OsalTimerDelete(&hapticData->timer);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: delete haptic timer fail!", __func__);
        }
    }

    return;
}

static int32_t GetHapticSeqByEffect(struct VibratorEffectCfg *effectCfg)
{
    struct VibratorHapticData *hapticData = NULL;
    struct VibratorEffectNode *pos = NULL;
    struct VibratorEffectNode *tmp = NULL;

    hapticData = GetHapticData();
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);

    (void)OsalMutexLock(&hapticData->mutex);
    hapticData->seqCount = 0;
    hapticData->currentEffectSeq = NULL;
    hapticData->currentSeqIndex = 0;

    if (effectCfg->cfgMode == VIBRATOR_MODE_ONCE) {
        hapticData->duration[VIBRATOR_TIME_DELAY_INDEX] = VIBRATOR_MIN_WAIT_TIME;
        hapticData->duration[VIBRATOR_TIME_DURATION_INDEX] = effectCfg->duration;
        hapticData->effectType = VIBRATOR_TYPE_TIME;
        hapticData->seqCount = VIBRATOR_TIME_INDEX_BUTT;
        hapticData->currentEffectSeq = &hapticData->duration[VIBRATOR_TIME_DELAY_INDEX];
        (void)OsalMutexUnlock(&hapticData->mutex);
        return HDF_SUCCESS;
    }

    if ((effectCfg->cfgMode == VIBRATOR_MODE_PRESET) && (effectCfg->effect != NULL)) {
        DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &hapticData->effectSeqHead, struct VibratorEffectNode, node) {
            if (strcmp(effectCfg->effect, pos->effect) == 0 && pos->seq != NULL) {
                hapticData->effectType = (int32_t)pos->type;
                HDF_LOGE("%s: pos_num = %d", __func__, pos->num);
                hapticData->seqCount = pos->num;
                hapticData->currentEffectSeq = &(pos->seq[0]);
                break;
            }
        }
        if ((hapticData->seqCount <= 1) || (hapticData->seqCount > VIBRATOR_MAX_HAPTIC_SEQ)) {
            HDF_LOGE("%s: not find effect type!", __func__);
            (void)OsalMutexUnlock(&hapticData->mutex);
            return HDF_ERR_INVALID_PARAM;
        }
        (void)OsalMutexUnlock(&hapticData->mutex);
        return HDF_SUCCESS;
    }

    HDF_LOGE("%s: not support effect type!", __func__);
    (void)OsalMutexUnlock(&hapticData->mutex);
    return HDF_ERR_NOT_SUPPORT;
}

int32_t StartHaptic(struct VibratorEffectCfg *effectCfg)
{
    int32_t ret;
    uint32_t duration;
    struct VibratorHapticData *hapticData = GetHapticData();
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(effectCfg, HDF_FAILURE);
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);

    if ((effectCfg->cfgMode == VIBRATOR_MODE_PRESET) && (!hapticData->supportHaptic)) {
        HDF_LOGE("%s: vibrator no support haptic!", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = GetHapticSeqByEffect(effectCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get haptic seq fail!", __func__);
        return ret;
    }

    duration = hapticData->currentEffectSeq[0] < VIBRATOR_MIN_WAIT_TIME ?
        VIBRATOR_MIN_WAIT_TIME : hapticData->currentEffectSeq[0];

    if (OsalTimerCreate(&hapticData->timer, duration, HapticTimerEntry, (uintptr_t)hapticData) != HDF_SUCCESS) {
        HDF_LOGE("%s: create vibrator timer fail!", __func__);
        return HDF_FAILURE;
    }

    if (OsalTimerStartLoop(&hapticData->timer) != HDF_SUCCESS) {
        HDF_LOGE("%s: start vibrator timer fail!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StopHaptic()
{
    int32_t ret;
    struct VibratorHapticData *hapticData = GetHapticData();

    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);

    if (hapticData->timer.realTimer != NULL) {
        HDF_LOGE("%s: delete vibrator Timer!", __func__);
        (void)OsalMutexLock(&hapticData->mutex);
        ret = OsalTimerDelete(&hapticData->timer);
        (void)OsalMutexUnlock(&hapticData->mutex);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: delete vibrator timer fail!", __func__);
            return ret;
        }
    }

    StopVibrator();
    return HDF_SUCCESS;
}

int32_t CreateVibratorHaptic(struct HdfDeviceObject *device)
{
    struct VibratorHapticData *hapticData = NULL;
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(device, HDF_FAILURE);

    hapticData = (struct VibratorHapticData *)OsalMemCalloc(sizeof(*hapticData));
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_ERR_MALLOC_FAIL);
    g_vibratorHapticData = hapticData;
    hapticData->supportHaptic = false;

    if (OsalMutexInit(&hapticData->mutex) != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to init mutex", __func__);
        goto EXIT;
    }

    DListHeadInit(&hapticData->effectSeqHead);

    // get haptic hcs
    if (ParserVibratorHapticConfig(device->property) != HDF_SUCCESS) {
        HDF_LOGE("%s: parser haptic config fail!", __func__);
        goto EXIT;
    }

    return HDF_SUCCESS;
EXIT:
    OsalMemFree(hapticData);
    return HDF_FAILURE;
}

static void FreeHapticConfig()
{
    struct VibratorHapticData *hapticData = GetHapticData();
    struct VibratorEffectNode *pos = NULL;
    struct VibratorEffectNode *tmp = NULL;

    if (hapticData == NULL) {
        return;
    }

    (void)OsalMutexLock(&hapticData->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &hapticData->effectSeqHead, struct VibratorEffectNode, node) {
        if (pos->seq != NULL) {
            OsalMemFree(pos->seq);
            pos->seq = NULL;
        }
        pos->effect = NULL;
        DListRemove(&pos->node);
        OsalMemFree(pos);
    }
    (void)OsalMutexUnlock(&hapticData->mutex);
}

int32_t DestroyVibratorHaptic()
{
    struct VibratorHapticData *hapticData = GetHapticData();
    CHECK_VIBRATOR_NULL_PTR_RETURN_VALUE(hapticData, HDF_FAILURE);

    if (hapticData->supportHaptic) {
        FreeHapticConfig();
    }

    (void)OsalMutexDestroy(&hapticData->mutex);

    return HDF_SUCCESS;
}
