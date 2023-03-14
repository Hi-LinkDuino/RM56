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
#include "input_controller.h"
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <securec.h>
#include "hdf_io_service_if.h"
#include "input_common.h"

#define ATTR_SBUF_LEN       1700
#define ABILITY_SBUF_LEN    360

InputDevManager *GetDevManager(void);

static bool FillSbufData(struct HdfSBuf *data, int32_t cmd, const void *in)
{
    bool ret = false;
    switch (cmd) {
        case SET_PWR_STATUS:
        case SET_GESTURE_MODE:
        case RUN_CAPAC_TEST:
            ret = HdfSbufWriteUint32(data, *(uint32_t *)(in));
            break;
        case RUN_EXTRA_CMD:
            ret = HdfSbufWriteString(data, ((InputExtraCmd *)in)->cmdCode);
            if (ret) {
                ret = HdfSbufWriteString(data, ((InputExtraCmd *)in)->cmdValue);
            }
            break;
        default:
            HDF_LOGE("%s: unknown cmd", __func__);
            break;
    }
    if (!ret) {
        HDF_LOGE("%s: cmd = %d sbuf write failed", __func__, cmd);
    }
    return ret;
}

static bool ReadStrBuf(struct HdfSBuf *reply, void *buf, uint32_t length)
{
    const char *tempStr = NULL;
    bool ret = false;

    tempStr = HdfSbufReadString(reply);
    if (tempStr != NULL) {
        if (strncpy_s(buf, length, tempStr, strlen(tempStr)) == EOK) {
            ret = true;
        } else {
            HDF_LOGE("%s: strncpy_s fail", __func__);
        }
    }
    return ret;
}

static bool ReadAttrBuf(struct HdfSBuf *data, void *attr)
{
    uint32_t len = 0;
    DevAttr *tmpAttr = NULL;

    if (!HdfSbufReadBuffer(data, (const void **)&tmpAttr, &len)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed, line: %d", __func__, __LINE__);
        return false;
    }
    if (memcpy_s(attr, sizeof(DevAttr), tmpAttr, sizeof(DevAttr)) != EOK) {
        HDF_LOGE("%s: memcpy_s failed, line: %d", __func__, __LINE__);
        return false;
    }
    return true;
}

static bool ReadAbilityBuf(struct HdfSBuf *data, void *ability)
{
    uint32_t len = 0;
    DevAbility *tmpAbility = NULL;

    if (!HdfSbufReadBuffer(data, (const void **)&tmpAbility, &len)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed, line: %d", __func__, __LINE__);
        return false;
    }
    if (memcpy_s(ability, sizeof(DevAbility), tmpAbility, sizeof(DevAbility)) != EOK) {
        HDF_LOGE("%s: memcpy_s failed, line: %d", __func__, __LINE__);
        return false;
    }
    return true;
}

static bool ObtainSbufData(struct HdfSBuf *reply, int32_t cmd, void *out, uint32_t length)
{
    bool ret = false;
    uint32_t tempInt = 0;
    switch (cmd) {
        case GET_PWR_STATUS:
        case GET_DEV_TYPE:
            ret = HdfSbufReadUint32(reply, &tempInt);
            if (ret) {
                *((uint32_t *)out) = tempInt;
            }
            break;
        case GET_CHIP_INFO:
        case GET_VENDOR_NAME:
        case GET_CHIP_NAME:
        case RUN_CAPAC_TEST:
            ret = ReadStrBuf(reply, out, length);
            break;
        case GET_DEV_ATTR:
            ret = ReadAttrBuf(reply, out);
            break;
        case GET_DEV_ABILITY:
            ret = ReadAbilityBuf(reply, out);
            break;
        default:
            HDF_LOGE("%s: unknown cmd", __func__);
            break;
    }
    if (!ret) {
        HDF_LOGE("%s: obtain data from sbuf failed", __func__);
    }
    return ret;
}

static int32_t InstanceCmdSbuf(struct HdfSBuf **data, struct HdfSBuf **reply, int32_t cmd)
{
    switch (cmd) {
        case GET_DEV_ATTR:
            *data = HdfSbufObtainDefaultSize();
            *reply = HdfSbufTypedObtainCapacity(SBUF_RAW, ATTR_SBUF_LEN);
            break;
        case GET_DEV_ABILITY:
            *data = HdfSbufObtainDefaultSize();
            *reply = HdfSbufTypedObtainCapacity(SBUF_RAW, ABILITY_SBUF_LEN);
            break;
        default:
            *data = HdfSbufObtainDefaultSize();
            *reply = HdfSbufObtainDefaultSize();
            break;
    }

    if ((*data == NULL) || (*reply == NULL)) {
        HDF_LOGE("%s: fail to obtain sbuf", __func__);
        goto EXIT;
    }
    return INPUT_SUCCESS;

EXIT:
    HdfSbufRecycle(*data);
    HdfSbufRecycle(*reply);
    *data = NULL;
    *reply = NULL;
    return INPUT_FAILURE;
}

static int32_t IoServiceOps(struct HdfIoService *service, int32_t cmd, const void *in, void *out, uint32_t outLen)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (InstanceCmdSbuf(&data, &reply, cmd) != INPUT_SUCCESS) {
        return INPUT_FAILURE;
    }

    if (in != NULL) {
        if (!FillSbufData(data, cmd, in)) {
            ret = INPUT_FAILURE;
            goto EXIT;
        }
    }
    ret = service->dispatcher->Dispatch(&service->object, cmd, data, reply);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: dispatch fail", __func__);
        goto EXIT;
    }

    if (out != NULL) {
        if (!ObtainSbufData(reply, cmd, out, outLen)) {
            ret = INPUT_FAILURE;
        }
    }

EXIT:
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    return ret;
}

static int32_t SetPowerStatus(uint32_t devIndex, uint32_t status)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (status >= INPUT_POWER_STATUS_UNKNOWN)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (IoServiceOps(pos->service, SET_PWR_STATUS, &status, NULL, 0)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: set power status failed", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't set power status", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetPowerStatus(uint32_t devIndex, uint32_t *status)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (status == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (IoServiceOps(pos->service, GET_PWR_STATUS, NULL, status, sizeof(uint32_t))) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get power status failed", __func__);
            return INPUT_FAILURE;
        }
        if (*status >= INPUT_POWER_STATUS_UNKNOWN) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: power status is unknown", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get power status", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetDeviceType(uint32_t devIndex, uint32_t *deviceType)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (deviceType == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (IoServiceOps(pos->service, GET_DEV_TYPE, NULL, deviceType, sizeof(uint32_t))) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get device type failed", __func__);
            return INPUT_FAILURE;
        }
        if (*deviceType >= INDEV_TYPE_UNKNOWN) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: device type is unknown", __func__);
            return INPUT_FAILURE;
        }
        pos->payload.devType = *deviceType;
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get device type", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetDevStringInfo(uint32_t devIndex, char *info, uint32_t len, uint32_t lenLimit, int32_t cmd)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    char *tempInfo = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (info == NULL) || (len < lenLimit)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        switch (cmd) {
            case GET_CHIP_NAME:
                tempInfo = pos->payload.chipName;
                break;
            case GET_CHIP_INFO:
                tempInfo = pos->payload.chipInfo;
                break;
            case GET_VENDOR_NAME:
                tempInfo = pos->payload.vendorName;
                break;
            default:
                pthread_mutex_unlock(&manager->mutex);
                HDF_LOGE("%s: cmd = %d invalid param", __func__, cmd);
                return INPUT_FAILURE;
        }

        if (IoServiceOps(pos->service, cmd, NULL, tempInfo, lenLimit)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get information fail", __func__);
            return INPUT_FAILURE;
        }

        if (strncpy_s(info, len, tempInfo, lenLimit - 1) != EOK) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: strncpy_s fail", __func__);
            return INPUT_FAILURE;
        }

        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGI("%s: device%u get information success", __func__, devIndex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get information", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetChipName(uint32_t devIndex, char *chipName, uint32_t length)
{
    return GetDevStringInfo(devIndex, chipName, length, CHIP_NAME_LEN, GET_CHIP_NAME);
}

static int32_t GetChipInfo(uint32_t devIndex, char *chipInfo, uint32_t length)
{
    return GetDevStringInfo(devIndex, chipInfo, length, CHIP_INFO_LEN, GET_CHIP_INFO);
}

static int32_t GetVendorName(uint32_t devIndex, char *vendorName, uint32_t length)
{
    return GetDevStringInfo(devIndex, vendorName, length, VENDOR_NAME_LEN, GET_VENDOR_NAME);
}

static int32_t SetGestureMode(uint32_t devIndex, uint32_t gestureMode)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if (devIndex >= MAX_INPUT_DEV_NUM) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (IoServiceOps(pos->service, SET_GESTURE_MODE, &gestureMode, NULL, 0)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: set gesture mode failed", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't set gesture mode", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t RunCapacitanceTest(uint32_t devIndex, uint32_t testType, char *result, uint32_t length)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    CapacitanceTestInfo testInfo;

    if (devIndex >= (MAX_INPUT_DEV_NUM) || (testType >= TEST_TYPE_UNKNOWN) ||
        (result == NULL) || (length < SELF_TEST_RESULT_LEN)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    (void)memset_s(&testInfo, sizeof(CapacitanceTestInfo), 0, sizeof(CapacitanceTestInfo));
    testInfo.testType = testType;
    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (IoServiceOps(pos->service, RUN_CAPAC_TEST, &testInfo.testType, testInfo.testResult, SELF_TEST_RESULT_LEN)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: run capacitance test failed", __func__);
            return INPUT_FAILURE;
        }

        if (strncpy_s(result, length, testInfo.testResult, SELF_TEST_RESULT_LEN - 1) != EOK) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: strncpy_s fail", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGI("%s: capacitance test result is %s", __func__, result);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't run capacitance test", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t RunExtraCommand(uint32_t devIndex, InputExtraCmd *cmdInfo)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (cmdInfo == NULL) || (cmdInfo->cmdCode == NULL) ||
        (cmdInfo->cmdValue == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (IoServiceOps(pos->service, RUN_EXTRA_CMD, cmdInfo, NULL, 0)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: run extra cmd failed", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%d doesn't exist, can't run extra cmd", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetDeviceAbility(uint32_t devIndex, DevAbility *ability)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if (devIndex >= MAX_INPUT_DEV_NUM) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        if (IoServiceOps(pos->service, GET_DEV_ABILITY, NULL, ability, sizeof(DevAbility))) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get dev ability failed", __func__);
            return INPUT_FAILURE;
        }

        if (memcpy_s(&pos->payload.abilitySet, sizeof(DevAbility), ability, sizeof(DevAbility)) != EOK) {
            HDF_LOGE("%s: memcpy_s failed in getting ability, line: %d", __func__, __LINE__);
            pthread_mutex_unlock(&manager->mutex);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }
    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get dev ability", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t UpdateAbilityInfo(uint32_t devIndex)
{
    int32_t ret;
    DeviceInfoNode node;

    ret = GetDeviceAbility(devIndex, &node.payload.abilitySet);
    INPUT_CHECK_RETURN(ret);
    return INPUT_SUCCESS;
}

static int32_t GetDeviceAttr(uint32_t devIndex, DevAttr *attr)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if (devIndex >= MAX_INPUT_DEV_NUM) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }

        if (IoServiceOps(pos->service, GET_DEV_ATTR, NULL, attr, sizeof(DevAttr))) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get dev attr failed", __func__);
            return INPUT_FAILURE;
        }

        if (memcpy_s(&pos->payload.attrSet, sizeof(DevAttr), attr, sizeof(DevAttr)) != EOK) {
            HDF_LOGE("%s: memcpy_s failed in getting attr", __func__);
            pthread_mutex_unlock(&manager->mutex);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }
    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get dev attr", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t UpdateAttrInfo(uint32_t devIndex)
{
    int32_t ret;
    DeviceInfoNode node;

    ret = GetDeviceAttr(devIndex, &node.payload.attrSet);
    INPUT_CHECK_RETURN(ret);
    return INPUT_SUCCESS;
}

static int32_t UpdateDevBaseInfo(uint32_t devIndex)
{
    int32_t ret;
    DeviceInfoNode node;

    ret = GetDeviceType(devIndex, &node.payload.devType);
    INPUT_CHECK_RETURN(ret);
    ret = GetChipName(devIndex, &node.payload.chipInfo[0], CHIP_INFO_LEN);
    INPUT_CHECK_RETURN(ret);
    ret = GetChipInfo(devIndex, &node.payload.vendorName[0], VENDOR_NAME_LEN);
    INPUT_CHECK_RETURN(ret);
    ret = GetVendorName(devIndex, &node.payload.chipName[0], CHIP_NAME_LEN);
    INPUT_CHECK_RETURN(ret);
    return INPUT_SUCCESS;
}

int32_t UpdateDevFullInfo(uint32_t devIndex)
{
    int32_t ret = UpdateDevBaseInfo(devIndex);
    INPUT_CHECK_RETURN(ret);
    ret = UpdateAbilityInfo(devIndex);
    INPUT_CHECK_RETURN(ret);
    ret = UpdateAttrInfo(devIndex);
    INPUT_CHECK_RETURN(ret);
    return INPUT_SUCCESS;
}

int32_t InstanceControllerHdi(InputController **controller)
{
    InputController *controllerHdi = (InputController *)malloc(sizeof(InputController));
    if (controllerHdi == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return INPUT_NOMEM;
    }

    (void)memset_s(controllerHdi, sizeof(InputController), 0, sizeof(InputController));

    controllerHdi->GetDeviceType = GetDeviceType;
    controllerHdi->SetPowerStatus = SetPowerStatus;
    controllerHdi->GetPowerStatus = GetPowerStatus;
    controllerHdi->GetChipInfo = GetChipInfo;
    controllerHdi->GetVendorName = GetVendorName;
    controllerHdi->GetChipName = GetChipName;
    controllerHdi->SetGestureMode = SetGestureMode;
    controllerHdi->RunCapacitanceTest = RunCapacitanceTest;
    controllerHdi->RunExtraCommand = RunExtraCommand;
    *controller = controllerHdi;
    return INPUT_SUCCESS;
}
