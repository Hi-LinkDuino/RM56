/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "ActsCapabilityTest.h"
#include <csignal>
#include <dirent.h>
#include <securec.h>
#include <sys/capability.h>
#include <unistd.h>

void Sigac(int i)
{
    _exit(0);
}

void ChildSleep()
{
    signal(SIGXFSZ, Sigac);
    usleep(LONG_SLEEP_NUM);
    exit(0);
}

int CapInit()
{
    // Init capabilities
    struct __user_cap_header_struct capheader;
    (void)memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM];
    (void)memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    capdata[0].permitted = LINUX_FULL_CAP;
    capdata[0].effective = LINUX_FULL_CAP;
    capdata[0].inheritable = LINUX_FULL_CAP;
    // Set capabilities
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPCHOWN()
{
    struct __user_cap_header_struct capheader = { 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPCHOWN memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPCHOWN memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_CHOWN
    capdata[CAP_TO_INDEX(CAP_CHOWN)].permitted &= ~CAP_TO_MASK(CAP_CHOWN);
    capdata[CAP_TO_INDEX(CAP_CHOWN)].effective &= ~CAP_TO_MASK(CAP_CHOWN);
    capdata[CAP_TO_INDEX(CAP_CHOWN)].inheritable &= ~CAP_TO_MASK(CAP_CHOWN);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPDACOVERRIDE()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPDACOVERRIDE memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPDACOVERRIDE memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_DAC_OVERRIDE
    capdata[CAP_TO_INDEX(CAP_DAC_OVERRIDE)].permitted &= ~CAP_TO_MASK(CAP_DAC_OVERRIDE);
    capdata[CAP_TO_INDEX(CAP_DAC_OVERRIDE)].effective &= ~CAP_TO_MASK(CAP_DAC_OVERRIDE);
    capdata[CAP_TO_INDEX(CAP_DAC_OVERRIDE)].inheritable &= ~CAP_TO_MASK(CAP_DAC_OVERRIDE);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPDACREADSEARCH()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPDACREADSEARCH memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPDACREADSEARCH memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_DAC_READ_SEARCH
    capdata[CAP_TO_INDEX(CAP_DAC_READ_SEARCH)].permitted &= ~CAP_TO_MASK(CAP_DAC_READ_SEARCH);
    capdata[CAP_TO_INDEX(CAP_DAC_READ_SEARCH)].effective &= ~CAP_TO_MASK(CAP_DAC_READ_SEARCH);
    capdata[CAP_TO_INDEX(CAP_DAC_READ_SEARCH)].inheritable &= ~CAP_TO_MASK(CAP_DAC_READ_SEARCH);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPDACOVERRIDEAndREADSEARCH()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPDACOVERRIDEAndREADSEARCH memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPDACOVERRIDEAndREADSEARCH memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_DAC_OVERRIDE and CAP_DAC_READ_SEARCH
    capdata[CAP_TO_INDEX(CAP_DAC_READ_SEARCH)].permitted &= ~CAP_TO_MASK(CAP_DAC_READ_SEARCH);
    capdata[CAP_TO_INDEX(CAP_DAC_READ_SEARCH)].effective &= ~CAP_TO_MASK(CAP_DAC_READ_SEARCH);
    capdata[CAP_TO_INDEX(CAP_DAC_READ_SEARCH)].inheritable &= ~CAP_TO_MASK(CAP_DAC_READ_SEARCH);
    capdata[CAP_TO_INDEX(CAP_DAC_OVERRIDE)].permitted &= ~CAP_TO_MASK(CAP_DAC_OVERRIDE);
    capdata[CAP_TO_INDEX(CAP_DAC_OVERRIDE)].effective &= ~CAP_TO_MASK(CAP_DAC_OVERRIDE);
    capdata[CAP_TO_INDEX(CAP_DAC_OVERRIDE)].inheritable &= ~CAP_TO_MASK(CAP_DAC_OVERRIDE);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPFOWNER()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPFOWNER memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPFOWNER memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_FOWNER
    capdata[CAP_TO_INDEX(CAP_FOWNER)].permitted &= ~CAP_TO_MASK(CAP_FOWNER);
    capdata[CAP_TO_INDEX(CAP_FOWNER)].effective &= ~CAP_TO_MASK(CAP_FOWNER);
    capdata[CAP_TO_INDEX(CAP_FOWNER)].inheritable &= ~CAP_TO_MASK(CAP_FOWNER);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPKILL()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPKILL memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPKILL memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_KILL
    capdata[CAP_TO_INDEX(CAP_KILL)].permitted &= ~CAP_TO_MASK(CAP_KILL);
    capdata[CAP_TO_INDEX(CAP_KILL)].effective &= ~CAP_TO_MASK(CAP_KILL);
    capdata[CAP_TO_INDEX(CAP_KILL)].inheritable &= ~CAP_TO_MASK(CAP_KILL);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPSETGID()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPSETGID memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPSETGID memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_SETGID
    capdata[CAP_TO_INDEX(CAP_SETGID)].permitted &= ~CAP_TO_MASK(CAP_SETGID);
    capdata[CAP_TO_INDEX(CAP_SETGID)].effective &= ~CAP_TO_MASK(CAP_SETGID);
    capdata[CAP_TO_INDEX(CAP_SETGID)].inheritable &= ~CAP_TO_MASK(CAP_SETGID);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPSETUID()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPSETUID memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPSETUID memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_SETUID
    capdata[CAP_TO_INDEX(CAP_SETUID)].permitted &= ~CAP_TO_MASK(CAP_SETUID);
    capdata[CAP_TO_INDEX(CAP_SETUID)].effective &= ~CAP_TO_MASK(CAP_SETUID);
    capdata[CAP_TO_INDEX(CAP_SETUID)].inheritable &= ~CAP_TO_MASK(CAP_SETUID);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPSETPCAP()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPSETPCAP memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPSETPCAP memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_SETPCAP
    capdata[CAP_TO_INDEX(CAP_SETPCAP)].permitted &= ~CAP_TO_MASK(CAP_SETPCAP);
    capdata[CAP_TO_INDEX(CAP_SETPCAP)].effective &= ~CAP_TO_MASK(CAP_SETPCAP);
    capdata[CAP_TO_INDEX(CAP_SETPCAP)].inheritable &= ~CAP_TO_MASK(CAP_SETPCAP);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPSYSNICE()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPSYSNICE memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPSYSNICE memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_SYS_NICE
    capdata[CAP_TO_INDEX(CAP_SYS_NICE)].permitted &= ~CAP_TO_MASK(CAP_SYS_NICE);
    capdata[CAP_TO_INDEX(CAP_SYS_NICE)].effective &= ~CAP_TO_MASK(CAP_SYS_NICE);
    capdata[CAP_TO_INDEX(CAP_SYS_NICE)].inheritable &= ~CAP_TO_MASK(CAP_SYS_NICE);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropCAPSYSTIME()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropCAPSYSTIME memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropCAPSYSTIME memset_s failed");
        return FALSE;
    };
    // Drop the capabilities of CAP_SYS_TIME
    capdata[CAP_TO_INDEX(CAP_SYS_TIME)].permitted &= ~CAP_TO_MASK(CAP_SYS_TIME);
    capdata[CAP_TO_INDEX(CAP_SYS_TIME)].effective &= ~CAP_TO_MASK(CAP_SYS_TIME);
    capdata[CAP_TO_INDEX(CAP_SYS_TIME)].inheritable &= ~CAP_TO_MASK(CAP_SYS_TIME);
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int DropAllCAP()
{
    struct __user_cap_header_struct capheader = { 0, 0 };
    errno_t result = memset_s(&capheader, sizeof(struct __user_cap_header_struct),
        0, sizeof(struct __user_cap_header_struct));
    if (result != EOK) {
        LOG("DropAllCAP memset_s failed");
        return FALSE;
    };
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;
    struct __user_cap_data_struct capdata[CAP_NUM] = { { 0 }, { 0 } };
    result = memset_s(capdata, CAP_NUM * sizeof(struct __user_cap_data_struct),
        0xff, CAP_NUM * sizeof(struct __user_cap_data_struct));
    if (result != EOK) {
        LOG("DropAllCAP memset_s failed");
        return FALSE;
    };
    // Drop all the capabilities
    capdata[0].permitted = NO_CAP;
    capdata[0].effective = NO_CAP;
    capdata[0].inheritable = NO_CAP;
    int ret = capset(&capheader, &capdata[0]);
    if (ret != 0) {
        return FALSE;
    }
    return 0;
}

int RemoveDir(const char *dirname)
{
    int ret;
    char curDir[] = ".";
    char upDir[] = "..";
    char updirname[SIZE512];
    DIR *dir = nullptr;
    struct dirent *dp = nullptr;
    struct stat upDirStat = { 0 };
    // Init capabilities
    CapInit();
    // The directory transferred by the parameter does not exist
    if (access(dirname, F_OK != 0)) {
        return 0;
    } else {
        chmod(dirname, RWX);
    }
    // Open the current-level directory
    dir = opendir(dirname);
    while ((dp = readdir(dir)) != nullptr) {
        // The system skips the judgment and continues looping when the . or .. directory is encountered
        if ((strcmp(curDir, dp->d_name) == 0) || (strcmp(upDir, dp->d_name) == 0)) {
            continue;
        }
        // Combine the upper-level directory content
        int spr = sprintf_s(updirname, SIZE512, "%s/%s", dirname, dp->d_name);
        if (spr == FALSE) {
            closedir(dir);
            return FALSE;
        }
        // Obtains the status of the upper-level, file or directory
        stat(updirname, &upDirStat);
        // Directory files, recursively deleting contents in the directory
        if (upDirStat.st_mode & S_IFDIR) {
            // Invoke the function recursively to delete the content of the upper-level directory
            RemoveDir(updirname);
        } else {
            // Common files, directly deleting
            ret = unlink(updirname);
            if (ret != 0) {
                // Failed to delete the file
                perror("Failed to delete the file");
                LOG("ErrInfo: The file name is %s", updirname);
                closedir(dir);
                return FALSE;
            }
        }
    }
    // Close the current-level directory
    closedir(dir);
    // Delete the empty directory
    ret = rmdir(dirname);
    if (ret != 0) {
        // Failed to delete the empty directory
        perror("Failed to delete the empty directory");
        LOG("ErrInfo: The directory name is %s", dirname);
        return FALSE;
    }
    return 0;
}

int SetUidGid(uid_t uid, gid_t gid)
{
    // Set the process uid and gid
    int retsetuid = setuid(uid);
    if (retsetuid != 0) {
        LOG("ErrInfo: Set uid=%d failed, now uid=%d", uid, getuid());
        return FALSE;
    }
    int retsetgid = setgid(gid);
    if (retsetgid != 0) {
        LOG("ErrInfo: Set gid=%d failed, now gid=%d", gid, getgid());
        return FALSE;
    }
    return 0;
}

timespec CompareTime(timespec start, timespec end)
{
    // Compare time 'start' and time 'end'
    timespec ret = { 0 };
    int tp = 1000000000;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        ret.tv_sec = end.tv_sec - start.tv_sec - 1;
        ret.tv_nsec = tp + end.tv_nsec - start.tv_nsec;
    } else {
        ret.tv_sec = end.tv_sec - start.tv_sec;
        ret.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return ret;
}

char *GetCurrentPath()
{
    // Obtain the current working directory
    static char path[MAX_PATH_SIZE];
    getcwd(path, MAX_PATH_SIZE);
    return path;
}

// Check topDir file system, 0 is exist, -1 is non-exist
int CheckFsMount(const char *topDir, const char *topDirMountInfo)
{
    const int lenMax = 100;
    int len;
    char buf[lenMax] = {0};
    const char mountInfoFile[] = "/proc/mounts";
    // check topDir exist
    if (access(topDir, F_OK) != 0) {
        LOG("ErrInfo: '%s' not accessable, Test Stop!", topDir);
        return -1;
    }
    FILE *fp = fopen(mountInfoFile, "r");
    if (fp != nullptr) {
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            len = strlen(buf);
            if (strstr(buf, topDirMountInfo) != nullptr) {
                fclose(fp);
                return 0;
            }
        }
        fclose(fp);
    }
    LOG("'%s' is not a '%s' not mount properly, Test Stop! please change another file type!", topDir, topDirMountInfo);
    return -1;
}