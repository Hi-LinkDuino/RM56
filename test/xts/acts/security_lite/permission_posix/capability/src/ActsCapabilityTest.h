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
#ifndef XTS_ACTS_SECURITY_LITE_PERMISSSION_POSIX_CAPABILITY_SRC_ACTSCAPABILITYTEST_H
#define XTS_ACTS_SECURITY_LITE_PERMISSSION_POSIX_CAPABILITY_SRC_ACTSCAPABILITYTEST_H

#include <cstddef>
#include <sys/stat.h>

constexpr int FALSE = -1;
constexpr int NUM0 = 0;
constexpr int NUM1 = 1;
constexpr int NUM2 = 2;
constexpr int NUM3 = 3;
constexpr int NUM5 = 5;
constexpr int NUM20 = 20;
constexpr int NUM80 = 80;
constexpr int NUM100 = 100;
constexpr int NUM500 = 500;
constexpr int NUM600 = 600;
constexpr int NUM700 = 700;
constexpr int NUM1000 = 1000;
constexpr int NUM3000 = 3000;
constexpr int NUM5000 = 5000;
constexpr int NUM10000 = 10000;

constexpr int INIT_PID = 1;
constexpr int KPROCESS_PID = 2;
constexpr int SHELL_PID = 3;
constexpr int HILOGCAT_PID = 4;
constexpr int FOUNDATION_PID = 5;
constexpr int BUNDLE_DAEMON_PID = 6;
constexpr int APPSPAWN_PID = 7;
constexpr int MEDIA_SERVER_PID = 8;
constexpr int WMS_SERVER_OR_AI_SERVER_PID = 9;
constexpr int HIVIEW_PID = 10;
constexpr int OTHER_PID = 12;
constexpr int INIT_PID_2 = 1;
constexpr int KPROCESS_PID_2 = 2;
constexpr int SHELL_PID_2 = 8;
constexpr int HILOGCAT_PID_2 = 10;
constexpr int FOUNDATION_PID_2 = 3;
constexpr int BUNDLE_DAEMON_PID_2 = 4;
constexpr int APPSPAWN_PID_2 = 5;
constexpr int MEDIA_SERVER_PID_2 = 6;
constexpr int WMS_SERVER_OR_AI_SERVER_PID_2 = 7;
constexpr int HIVIEW_PID_2 = 10;
constexpr int OTHER_PID_2 = 12;

constexpr unsigned int INIT_CAP = 0x02e83def;
constexpr unsigned int KPROCESS_CAP = 0x02e83def;
constexpr unsigned int SHELL_CAP = 0x02e83def;
constexpr unsigned int HILOGCAT_CAP = 0x00000000;
constexpr unsigned int FOUNDATION_CAP = 0x00003c00;
constexpr unsigned int BUNDLE_DAEMON_CAP = 0x00000007;
constexpr unsigned int APPSPAWN_CAP = 0x008009c4;
constexpr unsigned int MEDIA_SERVER_CAP = 0x00000000;
constexpr unsigned int WMS_SERVER_OR_AI_SERVER_CAP = 0x00000000;
constexpr unsigned int HIVIEW_CAP = 0x00000000;
constexpr unsigned int LINUX_FULL_CAP = 0xffffffff;
constexpr unsigned int OHOS_FULL_CAP = 0x02e83def;
constexpr unsigned int NO_CAP = 0x00000000;
constexpr unsigned int ONLY_SETPCAP_CAP = 0x00000100;

constexpr int CAP_NUM = 2;
constexpr int INVALID_CAP_TO_INDEX = 40;
constexpr int MAX_PATH_SIZE = 256;
constexpr int INVAILD_PID = 65536;
constexpr int SLEEP_NUM = 100000;
constexpr int LONG_SLEEP_NUM = 2000000;
constexpr int PID_MAX = 4194305;

constexpr mode_t ZERO = 0000;
constexpr mode_t NORWX = 0001;
constexpr mode_t RWX = 0777;

constexpr uid_t UID0 = 0;
constexpr uid_t UID1 = 1;
constexpr uid_t UID555 = 555;
constexpr uid_t UID1000 = 1000;
constexpr uid_t UID10000 = 10000;
constexpr uid_t UID20000 = 20000;
constexpr gid_t GID0 = 0;
constexpr gid_t GID1 = 1;
constexpr gid_t GID555 = 555;
constexpr gid_t GID1000 = 1000;
constexpr gid_t GID10000 = 10000;
constexpr gid_t GID20000 = 20000;
constexpr gid_t GROUPLIST[NUM3] = { 500, 500, 500 };

constexpr size_t SIZE512 = 512;

// Creating Folders and Files for the Test
#define CAPDIR0                  "CAPDIR0"                        // DIR0/
#define CAPDIR0_CAPFILE0         "CAPDIR0_CAPFILE0"               // ├── DIR0_FILE0
#define CAPDIR0_CAPFILE1         "CAPDIR0_CAPFILE1"               // ├── DIR0_FILE1
#define CAPDIR0_CAPDIR0          "CAPDIR0_CAPDIR0"                // ├── DIR0_DIR0/
#define CAPDIR0_CAPDIR1          "CAPDIR0_CAPDIR1"                // └── DIR0_DIR1/

extern "C" {
#define LOG(format, ...) printf("%s:%d:\n" format "\n", __FILE__, __LINE__, ##__VA_ARGS__);
}

void Sigac(int i);

void ChildSleep();

int CapInit();

int DropCAPCHOWN();

int DropCAPDACOVERRIDE();

int DropCAPDACREADSEARCH();

int DropCAPDACOVERRIDEAndREADSEARCH();

int DropCAPFOWNER();

int DropCAPKILL();

int DropCAPSETGID();

int DropCAPSETUID();

int DropCAPSETPCAP();

int DropCAPSYSNICE();

int DropCAPSYSTIME();

int DropAllCAP();

int RemoveDir(const char *dirname);

int SetUidGid(uid_t uid, gid_t gid);

timespec CompareTime(timespec start, timespec end);

char *GetCurrentPath();

int CheckFsMount(const char *topDir, const char *topDirMountInfo);

#endif
