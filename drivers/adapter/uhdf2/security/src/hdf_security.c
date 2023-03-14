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

#include "hdf_security.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include "hcs_tree_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sec.h"
#include "pal_if.h"
#include "securec.h"

#define HDF_LOG_TAG hdf_security
#define HDF_SECURE_PATH "/dev/hdf_secure"
#define HDF_SEC_PERM_ELEM_NAME "permission"
#define HDF_SEC_HOST_NAME "hostName"
#define MAX_RANDOM_BYTE_LEN 8
#define RAND_OPS_SUCCESS 1
#define MAX_SEED_LEN 64
#define HDF_TRANSFORM_STEP 2
#define HDF_SECURE_INVALID_PERM 0
#define HDF_SECURE_SET_ALL_ID 0
#define HDF_SECURE_SET_CUR_ID 1
#define HDF_SECURE_DEL_CUR_ID 2

#define BITS_PER_LONG 64
#define BIT_MASK(nr) (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr) ((nr) / BITS_PER_LONG)

const static char *g_hdfSecStrArray[PAL_MAX_TYPE] = {
    [PAL_I2C_TYPE] = "i2c",
    [PAL_SPI_TYPE] = "spi",
    [PAL_GPIO_TYPE] = "gpio",
    [PAL_PINCTRL_TYPE] = "pinctl",
    [PAL_CLOCK_TYPE] = "clock",
    [PAL_REGULATOR_TYPE] = "regulator",
    [PAL_MIPI_TYPE] = "mipi",
    [PAL_UART_TYPE] = "uart",
    [PAL_SDIO_TYPE] = "sdio",
    [PAL_MDIO_TYPE] = "mdio",
    [PAL_APB_TYPE] = "apb",
    [PAL_PCIE_TYPE] = "pcie",
    [PAL_PCM_TYPE] = "pcm",
    [PAL_I2S_TYPE] = "i2s",
    [PAL_PWM_TYPE] = "pwm",
    [PAL_DMA_TYPE] = "dma",
    [PAL_EFUSE_TYPE] = "efuse",
    [PAL_FLASH_TYPE] = "flash",
    [PAL_EMMC_TYPE] = "emmc",
    [PAL_RTC_TYPE] = "rtc",
    [PAL_ADC_TYPE] = "adc",
    [PAL_WDT_TYPE] = "wdt",
    [PAL_I3C_TYPE] = "i3c",
};

static Map g_indexMap;

Map *HdfSecGetHashMap()
{
    static Map hdfHashMap = { 0 };
    if (hdfHashMap.nodeSize == 0) {
        HDF_LOGI("Init hdfHashMap");
        MapInit(&hdfHashMap);
    }
    return &hdfHashMap;
}

static void InitStrToIndexMap()
{
    MapInit(&g_indexMap);
    for (int i = 0; i < PAL_MAX_TYPE; ++i) {
        MapSet(&g_indexMap, g_hdfSecStrArray[i], &i, sizeof(int *));
    }
}

static void HdfSetBit(uint32_t nr, volatile uint64_t *addr)
{
    unsigned long mask = BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
    *p |= mask;
}

static int32_t HdfGetRandom(int numBytes, uint8_t *random)
{
    if (RAND_bytes(random, numBytes) != RAND_OPS_SUCCESS) {
        HDF_LOGE("%{public}s: RAND_bytes failed", __FUNCTION__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void HdfCalculatePerms(const char *permStr, volatile uint64_t *perms)
{
    uint32_t *result = NULL;
    result = (uint32_t *)MapGet(&g_indexMap, permStr);
    if (result == NULL) {
        HDF_LOGE("%{public}s: %{public}s is not exist in map", __FUNCTION__, permStr);
        return;
    }
    HdfSetBit(*result, perms);
}

static int32_t HdfSecCheckParameters(const char *id)
{
    if (id == NULL) {
        HDF_LOGE("HdfSecCheckParameters failed, id is null");
        return HDF_FAILURE;
    }
    uint32_t len = (uint32_t)strlen(id);
    if (len >= (ID_MAX_SIZE - 1)) {
        HDF_LOGE("HdfSecCheckParameters failed, id length is %{public}u", len);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t HdfUpdateSecurityId(const char *id, const uint64_t perms, const int32_t isSetCurrentSecId)
{
    if (HdfSecCheckParameters(id) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s check id failed", __FUNCTION__);
        return HDF_FAILURE;
    }
    struct SecInfo secInfo = {
        .secId = { 0 },
        .secMap = { 0 }
    };
    int32_t ret = strncpy_s(secInfo.secId, ID_MAX_SIZE, id, ID_MAX_SIZE - 1);
    if (ret != EOK) {
        HDF_LOGE("%{public}s strncpy_s id failed", __FUNCTION__);
        return HDF_FAILURE;
    }
    secInfo.secMap[0] = perms;

    char path[PATH_MAX + 1] = { 0 };
    if (realpath(HDF_SECURE_PATH, path) == NULL) {
        HDF_LOGE("file %{public}s is invalid", HDF_SECURE_PATH);
        return HDF_FAILURE;
    }
    int32_t fd = open(path, O_WRONLY);
    if (fd < 0) {
        HDF_LOGE("open %{public}s failed, errno is %{public}d", HDF_SECURE_PATH, errno);
        return HDF_FAILURE;
    }

    if (isSetCurrentSecId == 0) {
        ret = ioctl(fd, HDF_SECURE_SET_INFO, &secInfo);
    } else if (isSetCurrentSecId == 1) {
        ret = ioctl(fd, HDF_SECURE_SET_CURRENT_ID, &secInfo);
    } else {
        ret = ioctl(fd, HDF_SECURE_DELETE_INFO, &secInfo);
    }

    close(fd);
    fd = -1;
    if (ret != 0) {
        HDF_LOGE("%{public}s ioctl error, ret is %{public}d", __FUNCTION__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t HdfRegisterSecurityId(const char *id, const uint64_t perm)
{
    return HdfUpdateSecurityId(id, perm, HDF_SECURE_SET_ALL_ID);
}

int32_t HdfRegisterAllDevSecId(const struct DeviceResourceNode *hostRoot)
{
    if (hostRoot == NULL) {
        HDF_LOGE("device_node linked list completed");
        return HDF_FAILURE;
    }

    uint64_t random = 0;
    int32_t ret = HdfGetRandom(MAX_RANDOM_BYTE_LEN, (uint8_t *)&random);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s HdfGetRandom failed", __FUNCTION__);
        return HDF_FAILURE;
    }

    InitStrToIndexMap();

    struct DeviceResourceNode *driverRoot = hostRoot->child;
    const char *permStr = NULL;
    volatile uint64_t perms = HDF_SECURE_INVALID_PERM;
    while (driverRoot != NULL) {
        const struct DeviceResourceNode *deviceRoot = driverRoot->child;
        while (deviceRoot != NULL) {
            int32_t permNums = HcsGetElemNum(deviceRoot, HDF_SEC_PERM_ELEM_NAME);
            HDF_LOGD("name is %{public}s, permNums is %{public}d", deviceRoot->name, permNums);
            for (int i = 0; i < permNums; ++i) {
                HcsGetStringArrayElem(deviceRoot, HDF_SEC_PERM_ELEM_NAME, i, &permStr, NULL);
                HdfCalculatePerms(permStr, &perms);
            }
            deviceRoot = deviceRoot->sibling;
        }
        driverRoot = driverRoot->sibling;
    }
    if (perms != HDF_SECURE_INVALID_PERM) {
        const char *hostName = NULL;
        HcsGetString(hostRoot, HDF_SEC_HOST_NAME, &hostName, NULL);
        ret = HdfRegisterSecurityId(hostName, perms);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("HdfCreateSecurityId failed for %{public}s", hostRoot->name);
        }
        perms = HDF_SECURE_INVALID_PERM;
    }
    return HDF_SUCCESS;
}

int32_t HdfSetCurrentHostSecurity(const char *hostName, int32_t procId)
{
    return HdfUpdateSecurityId(hostName, procId, HDF_SECURE_SET_CUR_ID);
}

void HdfDelSecurity(const char *hostName)
{
    if (hostName == NULL) {
        return;
    }
    int32_t ret = HdfUpdateSecurityId(hostName, 0, HDF_SECURE_DEL_CUR_ID);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s delete %{public}s security failed", __func__, hostName);
    }
}
