/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef XTS_ACTS_TOKEN_API_EXPECT_H
#define XTS_ACTS_TOKEN_API_EXPECT_H

#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <securec.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "feature.h"
#include "gtest/gtest.h"
#include "iunknown.h"
#include "perm_define.h"
#include "pms.h"
#include "pms_common.h"
#include "pms_inner.h"
#include "pms_interface.h"
#include "perm_operate.h"
#include "service.h"
#include "samgr_lite.h"

#define TEST_APP_ID "com.permission.test"
#define TEST_APP_ID2 "com.permission.test2"
#define SUBTEST_APP_ID "com.permission.subtest"
#define TEST_APP_SHORT "a"
#define TEST_APP_LONG "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define TEST_APP_PATH "/storage/app/etc/permissions/com.permission.test"
#define TEST_APP_INVALID "com.permission.!@#$%^&*()_+"
#define TEST_APP_NOT_EXIST "com.permission.onetwothree"
#define PERMISSION_UNDEFINED "ohos.permission.TEST"
#define PERMISSION_TOOLONG "ohos.permission.balabalabalabalabalabalabalabalabalabala"
#define PERMISSION_EMPTY ""
#define PERMISSION_UNSUPPORTED "!@#$%^&*()_+"
#define ETC_PREFIX "/storage/app/etc"
#define PERMISSION_PREFIX "/storage/app/etc/permissions"

const int SLEEP_TIME = 5;
const int TEST_TASKID = 1000;
const int SUBTEST_TASKID = 1001;
const int ABNORMAL_TASKID = -1;
const int SYS_PERM_NUM = 9;
const int ABNORMAL_PERM_NUM = 3;
const int MAX_PERM_NUM = 1024;
const int RET_OK = 0;
const int RET_NOK = -1;
const int RELI_TEST_TIME = 1000;
const int MAX_PID = 0x7fffffff;
const int MIN_PID = 0;
const int ABNORMAL_PKG_NUM = 5;
const int DIR_MODE = 777;
const int FLAG_NOT_EXIST = 123;
const int PERFORMANCE_RUN_TIMES = 10000;
const int MILLISECOND = 1000;

int IsUserGrant(const char *pname);

int CheckPermData(const PermissionTrans *pt, const PermissionSaved *ps,
                  const PermissionSaved *pre, int len, int plen);

void ClrPers(PermissionSaved *&permissions);

void CheckAppPermission(const char *identifier, int expRet, int expNum, const PermissionTrans *installPers,
                        const PermissionSaved *lastPers = nullptr, int lastLen = 0);

void SubTestWithoutCheck(const char *identifier, int taskID);

void CreateAppDir(void);

#endif // XTS_HITS_TOKEN_API_EXPECT_H