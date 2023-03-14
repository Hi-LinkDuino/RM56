/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#include "hal_sys_param.h"

#define OHOS_DEVICE_TYPE "liteWearable"
#define OHOS_DISPLAY_VERSION "OpenHarmony-2.2-Beta2"
#define OHOS_MANUFACTURE "Bestechnic"
#define OHOS_BRAND "Bestechnic"
#define OHOS_MARKET_NAME "****"
#define OHOS_PRODUCT_SERIES "****"
#define OHOS_PRODUCT_MODEL "****"
#define OHOS_SOFTWARE_MODEL "****"
#define OHOS_HARDWARE_MODEL "bes2600w"
#define OHOS_HARDWARE_PROFILE "aout:true,display:true"
#define OHOS_BOOTLOADER_VERSION "bootloader"
#define OHOS_ABI_LIST "****"
#define OHOS_SERIAL "1234567890"
#define OHOS_FIRST_API_VERSION  1
#define ETH_ALEN 6
#define MAC_BITS 4
#define HEX_A 0xa
#define MAC_HIGH_MASK 0xf0
#define MAC_LOW_MASK 0x0f
#define CHAR_NUM_OFFSET 0x30
#define CHAR_CAPITAL_OFFSET 0x37
#define STR_END_FLAG '\0'

typedef unsigned char               u8;

static char serialNumber[2*ETH_ALEN + 1];

const char* HalGetDeviceType(void)
{
    return OHOS_DEVICE_TYPE;
}

const char* HalGetManufacture(void)
{
    return OHOS_MANUFACTURE;
}

const char* HalGetBrand(void)
{
    return OHOS_BRAND;
}

const char* HalGetMarketName(void)
{
    return OHOS_MARKET_NAME;
}

const char* HalGetProductSeries(void)
{
    return OHOS_PRODUCT_SERIES;
}

const char* HalGetProductModel(void)
{
    return OHOS_PRODUCT_MODEL;
}

const char* HalGetSoftwareModel(void)
{
    return OHOS_SOFTWARE_MODEL;
}

const char* HalGetHardwareModel(void)
{
    return OHOS_HARDWARE_MODEL;
}

const char* HalGetHardwareProfile(void)
{
    return OHOS_HARDWARE_PROFILE;
}

static char Hex2Char(u8 hex)
{
    if (hex < HEX_A) {
        return hex + CHAR_NUM_OFFSET;
    } else {
        return hex + CHAR_CAPITAL_OFFSET;
    }
}

const char* HalGetSerial(void)
{
    char macAddr[ETH_ALEN];
    // as devboard has no production serial number, we just
    // use wifi mac address as device serial number.
    if (serialNumber[0] == STR_END_FLAG) {
        extern int bwifi_get_own_mac(u8 *addr);
        bwifi_get_own_mac(macAddr);
        int j = 0;
        for (int i = 0; i < ETH_ALEN; i++) {
            u8 lowFour, highFour;
            highFour = (macAddr[i] & MAC_HIGH_MASK) >> MAC_BITS;
            serialNumber[j] = Hex2Char(highFour);
            j++;
            lowFour = macAddr[i] & MAC_LOW_MASK;
            serialNumber[j] = Hex2Char(lowFour);
            j++;
        }
    }
    return serialNumber;
}

const char* HalGetBootloaderVersion(void)
{
    return OHOS_BOOTLOADER_VERSION;
}

const char* HalGetAbiList(void)
{
    return OHOS_ABI_LIST;
}

const char* HalGetDisplayVersion(void)
{
    return OHOS_DISPLAY_VERSION;
}

const char* HalGetIncrementalVersion(void)
{
    return INCREMENTAL_VERSION;
}

const char* HalGetBuildType(void)
{
    return BUILD_TYPE;
}

const char* HalGetBuildUser(void)
{
    return BUILD_USER;
}

const char* HalGetBuildHost(void)
{
    return BUILD_HOST;
}

const char* HalGetBuildTime(void)
{
    return BUILD_TIME;
}

int HalGetFirstApiVersion(void)
{
    return OHOS_FIRST_API_VERSION;
}

/**
 * @brief implement for js kvstorekit/filekit
 */
const char *GetDataPath()
{
    return "/data";
}
