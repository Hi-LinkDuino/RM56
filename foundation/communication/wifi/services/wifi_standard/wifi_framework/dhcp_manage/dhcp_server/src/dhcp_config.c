/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "dhcp_config.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "dhcp_logger.h"
#include "common_util.h"
#include "address_utils.h"
#include "dhcp_ipv4.h"
#include "securec.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServerConfig"

#define FILE_LINE_LEN_MAX       1024
#define FILE_LINE_DELIMITER     "="

static int SetEnableConfigInfo(DhcpConfig *dhcpConfig, const char *pKey, const char *pValue)
{
    if ((dhcpConfig == NULL) || (pKey == NULL) || (pValue == NULL)) {
        LOGE("SetEnableConfigInfo param dhcpConfig or pKey or pValue is NULL!");
        return RET_FAILED;
    }

    uint32_t uValue = (uint32_t)atoi(pValue);
    if ((uValue != 0) && (uValue != 1)) {
        LOGE("enable:%s error", pValue);
        return RET_FAILED;
    }

    if (strcmp(pKey, "distribution") == 0) {
        dhcpConfig->distribution = uValue;
    } else if (strcmp(pKey, "broadcast") == 0) {
        dhcpConfig->broadcast = uValue;
    }
    return RET_SUCCESS;
}

static int SetTimeConfigInfo(DhcpConfig *dhcpConfig, const char *pKey, const char *pValue)
{
    if ((dhcpConfig == NULL) || (pKey == NULL) || (pValue == NULL)) {
        LOGE("SetTimeConfigInfo param dhcpConfig or pKey or pValue is NULL!");
        return RET_FAILED;
    }

    uint32_t uValue = 0;
    if ((uValue = (uint32_t)atoi(pValue)) == 0) {
        LOGE("atoi failed, time:%s", pValue);
        return RET_FAILED;
    }

    if (strcmp(pKey, "leaseTime") == 0) {
        dhcpConfig->leaseTime = uValue;
    } else if (strcmp(pKey, "renewalTime") == 0) {
        dhcpConfig->renewalTime = uValue;
    } else if (strcmp(pKey, "rebindingTime") == 0) {
        dhcpConfig->rebindingTime = uValue;
    }
    return RET_SUCCESS;
}

static int SetNetConfigInfo(DhcpConfig *dhcpConfig, const char *pKey, const char *pValue, int common)
{
    if ((dhcpConfig == NULL) || (pKey == NULL) || (pValue == NULL)) {
        LOGE("SetNetConfigInfo param dhcpConfig or pKey or pValue is NULL!");
        return RET_FAILED;
    }

    uint32_t uValue = 0;
    if ((uValue = ParseIpAddr(pValue)) == 0) {
        LOGE("ParseIpAddr failed, ip:%s", pValue);
        return RET_FAILED;
    }

    if (((strcmp(pKey, "serverId") == 0) && common) || ((strcmp(pKey, "server") == 0) && !common)) {
        dhcpConfig->serverId = uValue;
    } else if (strcmp(pKey, "gateway") == 0) {
        dhcpConfig->gateway = uValue;
    } else if (strcmp(pKey, "netmask") == 0) {
        dhcpConfig->netmask = uValue;
    }
    return RET_SUCCESS;
}

static int SetIpAddressPool(DhcpConfig *dhcpConfig, const char *pValue)
{
    if ((dhcpConfig == NULL) || (pValue == NULL)) {
        LOGE("SetIpAddressPool param dhcpConfig or pValue is NULL!");
        return RET_FAILED;
    }

    char *pSrc = (char *)pValue;
    char *pSave = NULL;
    char *pTok = strtok_r(pSrc, ",", &pSave);
    if (((pTok == NULL) || (strlen(pTok) == 0)) || ((pSave == NULL) || (strlen(pSave) == 0))) {
        LOGE("strtok_r pTok or pSave NULL or len is 0!");
        return RET_FAILED;
    }

    uint32_t begin;
    if (((begin = ParseIpAddr(pTok)) == 0)) {
        LOGE("ParseIpAddr begin:%s failed!", pTok);
        return RET_FAILED;
    }
    dhcpConfig->pool.beginAddress = begin;
    uint32_t end;
    if (((end = ParseIpAddr(pSave)) == 0)) {
        LOGE("ParseIpAddr end:%s failed!", pSave);
        return RET_FAILED;
    }
    dhcpConfig->pool.endAddress = end;
    return RET_SUCCESS;
}

static int SetDnsInfo(DhcpConfig *dhcpConfig, const char *pValue)
{
    if ((dhcpConfig == NULL) || (pValue == NULL)) {
        LOGE("SetDnsInfo param dhcpConfig or pValue is NULL!");
        return RET_FAILED;
    }

    char *pSrc = (char *)pValue;
    char *pSave = NULL;
    char *pTok = strtok_r(pSrc, ",", &pSave);
    if ((pTok == NULL) || (strlen(pTok) == 0)) {
        LOGE("strtok_r pTok NULL or len is 0!");
        return RET_FAILED;
    }

    DhcpOption optDns = {DOMAIN_NAME_SERVER_OPTION, 0, {0}};
    if (GetOption(&dhcpConfig->options, optDns.code) != NULL) {
        RemoveOption(&dhcpConfig->options, optDns.code);
    }

    uint32_t dnsAddress;
    while (pTok != NULL) {
        if ((dnsAddress = ParseIpAddr(pTok)) == 0) {
            LOGE("ParseIpAddr %s failed, code:%d", pTok, optDns.code);
            return RET_FAILED;
        }
        if (AppendAddressOption(&optDns, dnsAddress) != RET_SUCCESS) {
            LOGW("failed append dns option.");
        }
        pTok = strtok_r(NULL, ",", &pSave);
    }
    PushBackOption(&dhcpConfig->options, &optDns);
    return RET_SUCCESS;
}

static int SetIfnameInfo(DhcpConfig *dhcpConfig, const char *pValue)
{
    if ((dhcpConfig == NULL) || (pValue == NULL)) {
        LOGE("SetIfnameInfo dhcpConfig or pValue is NULL!");
        return RET_FAILED;
    }
    if (strlen(pValue) >= IFACE_NAME_SIZE) {
        LOGE("ifname:%s too long!", pValue);
        return RET_FAILED;
    }
    if (memset_s(dhcpConfig->ifname, IFACE_NAME_SIZE, '\0', IFACE_NAME_SIZE) != EOK ||
        strncpy_s(dhcpConfig->ifname, IFACE_NAME_SIZE, pValue, strlen(pValue)) != EOK) {
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

static int SetDhcpConfig(DhcpConfig *dhcpConfig, const char *strLine, int common)
{
    if ((strLine == NULL) || (strlen(strLine) == 0)) {
        LOGE("SetDhcpConfig param strLine is NULL or len = 0!");
        return RET_FAILED;
    }

    char *pSrc = (char *)strLine;
    char *pSave = NULL;
    char *pTok = strtok_r(pSrc, FILE_LINE_DELIMITER, &pSave);
    if (pTok == NULL) {
        LOGE("strtok_r pTok NULL!");
        return RET_FAILED;
    }
    if (strcmp(pTok, "interface") == 0) {
        return SetIfnameInfo(dhcpConfig, pSave);
    } else if (strcmp(pTok, "dns") == 0) {
        return SetDnsInfo(dhcpConfig, pSave);
    } else if (strcmp(pTok, "pool") == 0) {
        return SetIpAddressPool(dhcpConfig, pSave);
    } else if ((((strcmp(pTok, "serverId") == 0) && common) || ((strcmp(pTok, "server") == 0) && !common)) ||
        (strcmp(pTok, "gateway") == 0) || (strcmp(pTok, "netmask") == 0)) {
        return SetNetConfigInfo(dhcpConfig, pTok, pSave, common);
    } else if ((strcmp(pTok, "leaseTime") == 0) || (strcmp(pTok, "renewalTime") == 0) ||
               (strcmp(pTok, "rebindingTime") == 0)) {
        return SetTimeConfigInfo(dhcpConfig, pTok, pSave);
    } else if ((strcmp(pTok, "distribution") == 0) || (strcmp(pTok, "broadcast") == 0)) {
        return SetEnableConfigInfo(dhcpConfig, pTok, pSave);
    } else {
        LOGD("invalid key:%s", pTok);
        return RET_SUCCESS;
    }
}

static int ParseConfigFile(const char *configFile, const char *ifname, DhcpConfig *dhcpConfig)
{
    if ((configFile == NULL) || (strlen(configFile) == 0) || (dhcpConfig == NULL)) {
        LOGE("ParseConfigFile param configFile or dhcpConfig is NULL or len = 0!");
        return RET_FAILED;
    }

    FILE *fp = fopen(configFile, "r");
    if (fp == NULL) {
        LOGE("fopen %s failed, err:%d", configFile, errno);
        return RET_FAILED;
    }

    int bComm = 1;
    int bValid = 1;
    char strLine[FILE_LINE_LEN_MAX] = {0};
    while (fgets(strLine, FILE_LINE_LEN_MAX, fp) != NULL) {
        if ((strchr(strLine, '#') != NULL) || (strchr(strLine, '=') == NULL) ||
            !RemoveSpaceCharacters(strLine, FILE_LINE_LEN_MAX)) {
            if (memset_s(strLine, FILE_LINE_LEN_MAX, 0, FILE_LINE_LEN_MAX) != EOK) {
                break;
            }
            continue;
        }
        if (memcmp(strLine, "interface", strlen("interface")) == 0) {
            bComm = 0;
            bValid = 0;
            if ((ifname == NULL) || (strlen(ifname) == 0) || (strstr(strLine, ifname) != NULL)) {
                LOGI("%s %s find ifname:%s", configFile, strLine, ((ifname == NULL) ? "" : ifname));
                bValid = 1;
            } else {
                LOGI("%s %s no find ifname:%s", configFile, strLine, ifname);
            }
        }
        if (bValid && SetDhcpConfig(dhcpConfig, strLine, bComm) != RET_SUCCESS) {
            LOGE("set dhcp config %s %s failed", configFile, strLine);
            fclose(fp);
            return RET_FAILED;
        }
        if (memset_s(strLine, FILE_LINE_LEN_MAX, 0, FILE_LINE_LEN_MAX) != EOK) {
            break;
        }
    }
    fclose(fp);
    return RET_SUCCESS;
}

static int CheckDhcpConfig(DhcpConfig *config)
{
    if (config == NULL) {
        LOGE("CheckDhcpConfig param config is null");
        return DHCP_FALSE;
    }
    if ((strlen(config->ifname) > 0) && ((config->serverId == 0))) {
        LOGE("failed to config serverId or netmask");
        return DHCP_FALSE;
    }

    if (config->renewalTime == 0) {
        config->renewalTime = config->leaseTime * DHCP_RENEWAL_MULTIPLE;
    }
    if (config->rebindingTime == 0) {
        config->rebindingTime = config->leaseTime * DHCP_REBIND_MULTIPLE;
    }
    return DHCP_TRUE;
}

int LoadConfig(const char *configFile, const char *ifname, DhcpConfig *config)
{
    if ((configFile == NULL) || (strlen(configFile) == 0) || (ifname == NULL) || (strlen(ifname) == 0) ||
        (config == NULL)) {
        LOGE("LoadConfig param configFile or ifname or config is NULL or len = 0!");
        return RET_FAILED;
    }

    /* Default config. */
    config->leaseTime = DHCP_LEASE_TIME;
    config->distribution = 1;
    config->broadcast = 1;

    /* Set file config. */
    if (ParseConfigFile(configFile, ifname, config) != RET_SUCCESS) {
        LOGE("parse config file %s error!", configFile);
        return RET_FAILED;
    }
    LOGI("parse config file %s success", configFile);

    if (!CheckDhcpConfig(config)) {
        LOGE("check dhcp config failed");
        return RET_FAILED;
    }

    if ((strlen(config->ifname) == 0) && SetIfnameInfo(config, ifname) != RET_SUCCESS) {
        LOGE("set ifname %s error!", ifname);
        return RET_FAILED;
    }
    return RET_SUCCESS;
}
