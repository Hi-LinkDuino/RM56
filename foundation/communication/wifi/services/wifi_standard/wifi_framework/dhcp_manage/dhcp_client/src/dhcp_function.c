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
#include "dhcp_function.h"

#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include "securec.h"
#include "dhcp_define.h"

#undef LOG_TAG
#define LOG_TAG "WifiDhcpFunction"

bool Ip4StrConToInt(const char *strIp, uint32_t *uIp, bool bHost)
{
    if ((strIp == NULL) || (strlen(strIp) == 0)) {
        LOGE("Ip4StrConToInt failed, strIp == NULL or \"\"!");
        return false;
    }

    struct in_addr addr4;
    int nRet = inet_pton(AF_INET, strIp, &addr4);
    if (nRet != 1) {
        LOGE("Ip4StrConToInt strIp:%{private}s failed, inet_pton nRet:%{public}d!", strIp, nRet);
        if (nRet == 0) {
            LOGE("Ip4StrConToInt strIp:%{private}s not in presentation format!", strIp);
        } else {
            LOGE("Ip4StrConToInt strIp:%{private}s inet_pton does not contain a valid address family!", strIp);
        }
        return false;
    }

    if (bHost) {
        *uIp = ntohl(addr4.s_addr);
    } else {
        *uIp = addr4.s_addr;
    }

    return true;
}

char *Ip4IntConToStr(uint32_t uIp, bool bHost)
{
    char bufIp4[INET_ADDRSTRLEN] = {0};
    struct in_addr addr4;
    if (bHost) {
        addr4.s_addr = htonl(uIp);
    } else {
        addr4.s_addr = uIp;
    }
    const char *p = inet_ntop(AF_INET, &addr4, bufIp4, INET_ADDRSTRLEN);
    if (p == NULL) {
        LOGE("Ip4IntConToStr uIp:%{private}u failed, inet_ntop p == NULL!", uIp);
        return NULL;
    }

    char *strIp = (char *)malloc(INET_ADDRSTRLEN);
    if (strIp == NULL) {
        LOGE("Ip4IntConToStr uIp:%{private}u failed, strIp malloc failed!", uIp);
        return NULL;
    }
    if (strncpy_s(strIp, INET_ADDRSTRLEN, bufIp4, strlen(bufIp4)) != EOK) {
        LOGE("Ip4IntConToStr uIp:%{private}u failed, strIp strncpy_s failed!", uIp);
        free(strIp);
        strIp = NULL;
        return NULL;
    }

    return strIp;
}

bool Ip6StrConToChar(const char *strIp, uint8_t chIp[], size_t chlen)
{
    if ((strIp == NULL) || (strlen(strIp) == 0)) {
        LOGE("Ip6StrConToChar failed, strIp == NULL or \"\"!");
        return false;
    }

    struct in6_addr addr6;
    if (memset_s(&addr6, sizeof(addr6), 0, sizeof(addr6)) != EOK) {
        return false;
    }
    int nRet = inet_pton(AF_INET6, strIp, &addr6);
    if (nRet != 1) {
        LOGE("Ip6StrConToChar strIp:%{private}s failed, inet_pton nRet:%{public}d!", strIp, nRet);
        if (nRet == 0) {
            LOGE("Ip6StrConToChar strIp:%{private}s not in presentation format!", strIp);
        } else {
            LOGE("Ip6StrConToChar strIp:%{private}s inet_pton does not contain a valid address family!", strIp);
        }
        return false;
    }

    LOGI("Ip6StrConToChar strIp:%{private}s -> ", strIp);
    for (size_t i = 0; i < chlen; i++) {
        LOGI("Ip6StrConToChar addr6.s6_addr: %{private}zu - %{private}02x", i, addr6.s6_addr[i]);
        chIp[i] = addr6.s6_addr[i];
    }

    return true;
}

const char *MacChConToMacStr(const unsigned char *pChMac, size_t chLen, char *pStrMac, size_t strLen)
{
    if ((pChMac == NULL) || (chLen == 0)) {
        LOGE("MacChConToMacStr failed, pChMac == NULL or chLen == 0!");
        return NULL;
    }

    if ((pStrMac == NULL) || (strLen < (chLen * MAC_ADDR_CHAR_NUM))) {
        LOGE("MacChConToMacStr failed, pStrMac == NULL or strLen:%{public}d error!", (int)strLen);
        return NULL;
    }

    const unsigned char *pSrc = pChMac;
    const unsigned char *pSrcEnd = pSrc + chLen;
    char *pDest = pStrMac;
    for (; pSrc < pSrcEnd; pSrc++) {
        /* The first character of pStrMac starts with a letter, not ':'. */
        if (pSrc != pChMac) {
            *(pDest++) = ':';
        }
        pDest += snprintf_s(pDest, MAC_ADDR_CHAR_NUM, MAC_ADDR_CHAR_NUM - 1, "%.2x", *pSrc);
    }
    /* The last character of pStrMac ends with '\0'. */
    *(pDest++) = '\0';
    return pStrMac;
}

int GetLocalInterface(const char *ifname, int *ifindex, unsigned char *hwaddr, uint32_t *ifaddr4)
{
    if ((ifname == NULL) || (strlen(ifname) == 0) || hwaddr == NULL) {
        LOGE("GetLocalInterface() failed, ifname == NULL or hwaddr is NULL");
        return DHCP_OPT_FAILED;
    }

    int fd;
    struct ifreq iface;
    struct sockaddr_in *pSockIn = NULL;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOGE("GetLocalInterface() ifname:%{public}s failed, socket err:%{public}d!", ifname, errno);
        return DHCP_OPT_FAILED;
    }

    if (memset_s(&iface, sizeof(iface), 0, sizeof(iface)) != EOK) {
        LOGE("GetLocalInterface() ifname:%{public}s failed, memset_s error!", ifname);
        close(fd);
        return DHCP_OPT_FAILED;
    }
    if (strncpy_s(iface.ifr_name, sizeof(iface.ifr_name), ifname, strlen(ifname)) != EOK) {
        LOGE("GetLocalInterface() ifname:%{public}s failed, strncpy_s error!", ifname);
        close(fd);
        return DHCP_OPT_FAILED;
    }

    if (ioctl(fd, SIOCGIFINDEX, &iface) != 0) {
        LOGE("GetLocalInterface() %{public}s failed, SIOCGIFINDEX err:%{public}d!", ifname, errno);
        close(fd);
        return DHCP_OPT_FAILED;
    }
    *ifindex = iface.ifr_ifindex;

    if (ioctl(fd, SIOCGIFHWADDR, &iface) != 0) {
        LOGE("GetLocalInterface() %{public}s failed, SIOCGIFHWADDR err:%{public}d!", ifname, errno);
        close(fd);
        return DHCP_OPT_FAILED;
    }
    if (memcpy_s(hwaddr, MAC_ADDR_LEN, iface.ifr_hwaddr.sa_data, MAC_ADDR_LEN) != EOK) {
        LOGE("GetLocalInterface() ifname:%{public}s failed, memcpy_s error!", ifname);
        close(fd);
        return DHCP_OPT_FAILED;
    }

    if (ifaddr4 != NULL) {
        if (ioctl(fd, SIOCGIFADDR, &iface) < 0) {
            LOGE("GetLocalInterface() %{public}s failed, SIOCGIFADDR err:%{public}d!", ifname, errno);
            close(fd);
            return DHCP_OPT_FAILED;
        }
        pSockIn = (struct sockaddr_in *)&iface.ifr_addr;
        *ifaddr4 = pSockIn->sin_addr.s_addr;
    }

    close(fd);
    return DHCP_OPT_SUCCESS;
}

int GetLocalIp(const char *ifname, uint32_t *ifaddr4)
{
    if ((ifname == NULL) || (strlen(ifname) == 0)) {
        LOGE("GetLocalIp() failed, ifname == NULL or \"\"!");
        return DHCP_OPT_FAILED;
    }

    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    int family, s;
    char strIp[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        LOGE("GetLocalIp() ifname:%{public}s failed, getifaddrs error:%{public}d!", ifname, errno);
        return DHCP_OPT_FAILED;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, ifname) != 0) {
            continue;
        }

        if (ifa->ifa_addr == NULL) {
            LOGE("GetLocalIp() ifname:%{public}s failed, ifa->ifa_addr == NULL!", ifname);
            continue;
        }

        family = ifa->ifa_addr->sa_family;
        if ((family != AF_INET) && (family != AF_INET6)) {
            continue;
        }

        if (memset_s(strIp, sizeof(strIp), 0, sizeof(strIp)) != EOK) {
            return DHCP_OPT_FAILED;
        }
        s = getnameinfo(ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
            strIp, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
            LOGE("GetLocalIp() %{public}s failed, getnameinfo error:%{public}s!", ifname, gai_strerror(s));
            return DHCP_OPT_FAILED;
        }

        /* Output all ip with ifa_name is ifname and family is AF_INET or AF_INET6. */
        if (family == AF_INET) {
            uint32_t hostIp = 0;
            if (!Ip4StrConToInt(strIp, &hostIp, true)) {
                LOGE("GetLocalIp() %{public}s failed, Ip4StrConToInt strIp:%{private}s error!", ifname, strIp);
                return DHCP_OPT_FAILED;
            }
            LOGI("GetLocalIp() %{public}s, AF_INET str:%{private}s -> host:%{private}u.", ifname, strIp, hostIp);
            *ifaddr4 = hostIp;
        } else {
            LOGI("GetLocalIp() %{public}s, AF_INET6 strIp:%{private}s.", ifname, strIp);
        }
    }

    freeifaddrs(ifaddr);
    return DHCP_OPT_SUCCESS;
}

int SetLocalInterface(const char *ifname, uint32_t ifaddr4)
{
    if ((ifname == NULL) || (strlen(ifname) == 0)) {
        LOGE("SetLocalInterface() failed, ifname == NULL or \"\"!");
        return DHCP_OPT_FAILED;
    }

    char *cIp = Ip4IntConToStr(ifaddr4, true);
    if (cIp == NULL) {
        LOGE("SetLocalInterface() %{public}s failed, Ip4IntConToStr addr4:%{private}u failed!", ifname, ifaddr4);
        return DHCP_OPT_FAILED;
    }
    LOGI("SetLocalInterface() %{public}s, ifaddr4:%{private}u -> %{private}s.", ifname, ifaddr4, cIp);
    free(cIp);
    cIp = NULL;

    int fd;
    struct ifreq ifr;
    struct sockaddr_in sin;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOGE("SetLocalInterface() ifname:%{public}s failed, socket error:%{public}d!", ifname, errno);
        return DHCP_OPT_FAILED;
    }

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        close(fd);
        return DHCP_OPT_FAILED;
    }
    if (strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, strlen(ifname)) != EOK) {
        close(fd);
        return DHCP_OPT_FAILED;
    }

    if (memset_s(&sin, sizeof(sin), 0, sizeof(sin)) != EOK) {
        close(fd);
        return DHCP_OPT_FAILED;
    }
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(ifaddr4);
    if (memcpy_s(&ifr.ifr_addr, sizeof(ifr.ifr_addr), &sin, sizeof(struct sockaddr)) != EOK) {
        close(fd);
        return DHCP_OPT_FAILED;
    }

    /* Similar to the system command: ifconfig ifname ifaddr4. */
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        LOGE("SetLocalInterface() %{public}s failed, SIOCSIFADDR err:%{public}d!", ifname, errno);
        close(fd);
        return DHCP_OPT_FAILED;
    }

    close(fd);
    return DHCP_OPT_SUCCESS;
}

int InitPidfile(const char *pidDir, const char *pidFile, pid_t pid)
{
    if ((pidDir == NULL) || (strlen(pidDir) == 0) || (pidFile == NULL) || (strlen(pidFile) == 0)) {
        LOGE("InitPidfile() failed, pidDir or pidFile == NULL or \"\"!");
        return DHCP_OPT_FAILED;
    }
    LOGI("InitPidfile() pidDir:%{public}s, pidFile:%{public}s.", pidDir, pidFile);
    unlink(pidFile);

    int fd;
    if ((fd = open(pidFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
        LOGE("InitPidfile() failed, open pidFile:%{public}s error:%{public}d!", pidFile, errno);
        return DHCP_OPT_FAILED;
    }

    char buf[PID_MAX_LEN] = {0};
    if (snprintf_s(buf, PID_MAX_LEN, PID_MAX_LEN - 1, "%d", pid) < 0) {
        LOGE("InitPidfile() pidFile:%{public}s failed, snprintf_s error:%{public}d!", pidFile, errno);
        close(fd);
        return DHCP_OPT_FAILED;
    }
    ssize_t bytes;
    if ((bytes = write(fd, buf, strlen(buf))) <= 0) {
        LOGE("InitPidfile() failed, write pidFile:%{public}s error:%{public}d, bytes:%{public}zd!",
            pidFile, errno, bytes);
        close(fd);
        return DHCP_OPT_FAILED;
    }
    LOGI("InitPidfile() buf:%{public}s write pidFile:%{public}s, bytes:%{public}zd!", buf, pidFile, bytes);
    close(fd);

    if (chdir(pidDir) != 0) {
        LOGE("InitPidfile() failed, chdir pidDir:%{public}s error:%{public}d!", pidDir, errno);
        return DHCP_OPT_FAILED;
    }

    /* Set default permissions for the specified client process id files and directories. */
    umask(DEFAULT_UMASK);

    /* Change attribs to the specified client process id files: 644 (user=rw, group=r, other=r). */
    chmod(pidFile, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    return DHCP_OPT_SUCCESS;
}

pid_t GetPID(const char *pidFile)
{
    /* Check pidFile is or not exists. */
    struct stat sb;
    if (stat(pidFile, &sb) != 0) {
        LOGW("GetPID() pidFile:%{public}s stat error:%{public}d!", pidFile, errno);
        return -1;
    }
    LOGI("GetPID() pidFile:%{public}s stat st_size:%{public}d.", pidFile, (int)sb.st_size);

    int fd;
    if ((fd = open(pidFile, O_RDONLY)) < 0) {
        LOGE("GetPID() failed, open pidFile:%{public}s error!", pidFile);
        return -1;
    }

    lseek(fd, 0, SEEK_SET);

    char buf[PID_MAX_LEN] = {0};
    ssize_t bytes;
    if ((bytes = read(fd, buf, sb.st_size)) < 0) {
        LOGE("GetPID() failed, read pidFile:%{public}s error, bytes:%{public}zd!", pidFile, bytes);
        close(fd);
        return -1;
    }
    LOGI("GetPID() read pidFile:%{public}s, buf:%{public}s, bytes:%{public}zd.", pidFile, buf, bytes);
    close(fd);

    return atoi(buf);
}

int CreateDirs(const char *dirs, int mode)
{
    if ((dirs == NULL) || (strlen(dirs) == 0) || (strlen(dirs) >= DIR_MAX_LEN)) {
        LOGE("CreateDirs() dirs:%{public}s error!", dirs);
        return DHCP_OPT_FAILED;
    }

    int nSrcLen = (int)strlen(dirs);
    char strDir[DIR_MAX_LEN] = {0};
    if (strncpy_s(strDir, sizeof(strDir), dirs, strlen(dirs)) != EOK) {
        LOGE("CreateDirs() strncpy_s dirs:%{public}s failed!", dirs);
        return DHCP_OPT_FAILED;
    }
    if (strDir[nSrcLen - 1] != '/') {
        if (nSrcLen == (DIR_MAX_LEN - 1)) {
            LOGE("CreateDirs() dirs:%{public}s len:%{public}d error!", dirs, nSrcLen);
            return DHCP_OPT_FAILED;
        }
        if (strcat_s(strDir, sizeof(strDir), "/") != EOK) {
            LOGE("CreateDirs() strcat_s strDir:%{public}s failed!", strDir);
            return DHCP_OPT_FAILED;
        }
        nSrcLen++;
    }

    int i = (strDir[0] == '/') ? 1 : 0;
    for (; i <= nSrcLen - 1; i++) {
        if (strDir[i] == '/') {
            strDir[i] = 0;
            if ((access(strDir, F_OK) != 0) && (mkdir(strDir, mode) != 0)) {
                LOGE("CreateDirs() mkdir %{public}s %{public}.4o failed:%{public}d!", strDir, mode, errno);
                return DHCP_OPT_FAILED;
            }
            strDir[i] = '/';
        }
    }
    LOGI("CreateDirs() %{public}s %{public}.4o success.", dirs, mode);
    return DHCP_OPT_SUCCESS;
}
