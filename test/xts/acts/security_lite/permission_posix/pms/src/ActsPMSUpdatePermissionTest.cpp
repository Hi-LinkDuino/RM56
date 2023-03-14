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

#include "ActsPMSTest.h"
#include <sys/time.h>

using namespace std;
using namespace testing::ext;

static PmsInnerApi *g_interface;
static PermissionTrans g_systemPers[] = {
    {
        "ohos.permission.CAMERA",
        "for CAMERA use",
        INUSE,
    },
    {
        "ohos.permission.RECORD_AUDIO",
        "for RECORD_AUDIO use",
        ALWAYS,
    },
    {
        "ohos.permission.READ_MEDIA_AUDIO",
        "for READ_MEDIA_AUDIO use",
        INUSE,
    },
    {
        "ohos.permission.READ_MEDIA_IMAGES",
        "for READ_MEDIA_IMAGES use",
        ALWAYS,
    },
    {
        "ohos.permission.READ_MEDIA_VIDEO",
        "for READ_MEDIA_VIDEO use",
        INUSE,
    },
    {
        "ohos.permission.WRITE_MEDIA_AUDIO",
        "for WRITE_MEDIA_AUDIO use",
        ALWAYS,
    },
    {
        "ohos.permission.WRITE_MEDIA_IMAGES",
        "for WRITE_MEDIA_IMAGES use",
        INUSE,
    },
    {
        "ohos.permission.WRITE_MEDIA_VIDEO",
        "for WRITE_MEDIA_VIDEO use",
        ALWAYS,
    },
    {
        "ohos.permission.MODIFY_AUDIO_SETTINGS",
        "for MODIFY_AUDIO_SETTINGS use",
        INUSE,
    },
};

class ActsPMSUpdatePermissionTest : public testing::Test
{
protected:
    static void SetUpTestCase(void)
    {
        CreateAppDir();
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(PERMISSION_SERVICE, PERM_INNER);
        iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&g_interface);
        setuid(0);
    }
    static void TearDownTestCase(void) {}
    virtual void SetUp() {}
    virtual void TearDown()
    {
        DeletePermissions(TEST_APP_ID);
        DeletePermissions(TEST_APP_ID2);
        DeletePermissions(SUBTEST_APP_ID);
        UnLoadPermissions(TEST_TASKID);
        UnLoadPermissions(SUBTEST_TASKID);
    }
};

long GetCurrentTimeMillis()
{
    struct timeval current;
    gettimeofday(&current, NULL);
    return current.tv_sec * MILLISECOND + current.tv_usec / MILLISECOND;
}

/**
 * @tc.number     Security_AppSecurity_PermissionManager_L1_UpdatePermissionFlags_001
 * @tc.name       Update permission flags
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUpdatePermissionTest, testSecPMUpdatePMFlags_001, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    if (permissions != NULL)
    {
        EXPECT_EQ(permissions[0].flags, PMS_FLAG_DEFAULT) << "flags =" << permissions[0].flags << endl;
    }
    ClrPers(permissions);

    ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, PMS_FLAG_DEFAULT);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "update perms flag ret =" << ret << endl;

    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    ClrPers(permissions);

    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     Security_AppSecurity_PermissionManager_L1_UpdatePermissionFlags_002
 * @tc.name       Update permission flags (No reminds after rejection)
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUpdatePermissionTest, testSecPMUpdatePMFlags_002, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    if (permissions != NULL)
    {
        EXPECT_EQ(permissions[0].flags, PMS_FLAG_DEFAULT) << "flags =" << permissions[0].flags << endl;
    }
    ClrPers(permissions);

    ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, PMS_FLAG_USER_FIXED);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "update perms flag ret =" << ret << endl;

    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    ClrPers(permissions);

    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     Security_AppSecurity_PermissionManager_L1_UpdatePermissionFlags_003
 * @tc.name       Update permission flags (Name does not exist)
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUpdatePermissionTest, testSecPMUpdatePMFlags_003, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    if (permissions != NULL)
    {
        EXPECT_EQ(permissions[0].flags, PMS_FLAG_DEFAULT) << "flags =" << permissions[0].flags << endl;
    }
    ClrPers(permissions);

    // pkg name not exist
    ret = g_interface->UpdatePermissionFlags(TEST_APP_NOT_EXIST, g_systemPers[0].name, PMS_FLAG_USER_FIXED);
    EXPECT_EQ(ret, PERM_ERRORCODE_FILE_NOT_EXIST) << "update perms flag ret =" << ret << endl;

    // pkg perm not exist
    ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, PERMISSION_UNDEFINED, PMS_FLAG_USER_FIXED);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "update perms flag ret =" << ret << endl;

    // pkg flag not exist
    ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, FLAG_NOT_EXIST);
    EXPECT_EQ(ret, PERM_ERRORCODE_INVALID_PARAMS) << "update perms flag ret =" << ret << endl;
    ClrPers(permissions);

    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     Security_AppSecurity_PermissionManager_L1_UpdatePermissionFlags_004
 * @tc.name       Update permission flags (Name empty)
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUpdatePermissionTest, testSecPMUpdatePMFlags_004, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    if (permissions != NULL)
    {
        EXPECT_EQ(permissions[0].flags, PMS_FLAG_DEFAULT) << "flags =" << permissions[0].flags << endl;
    }
    ClrPers(permissions);

    // pkg name empty
    ret = g_interface->UpdatePermissionFlags(NULL, g_systemPers[0].name, PMS_FLAG_USER_FIXED);
    EXPECT_EQ(ret, PERM_ERRORCODE_INVALID_PARAMS) << "update perms flag ret =" << ret << endl;

    // pkg perm empty
    ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, NULL, PMS_FLAG_USER_FIXED);
    EXPECT_EQ(ret, PERM_ERRORCODE_INVALID_PARAMS) << "update perms flag ret =" << ret << endl;

    // pkg flag empty
    ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, NULL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "update perms flag ret =" << ret << endl;
    ClrPers(permissions);

    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     Security_AppSecurity_PermissionManager_L1_UpdatePermissionFlags_005
 * @tc.name       Update permission flags (Name invalid)
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUpdatePermissionTest, testSecPMUpdatePMFlags_005, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    if (permissions != NULL)
    {
        EXPECT_EQ(permissions[0].flags, PMS_FLAG_DEFAULT) << "flags =" << permissions[0].flags << endl;
    }
    ClrPers(permissions);

    // pkg name invalid
    ret = g_interface->UpdatePermissionFlags(TEST_APP_INVALID, g_systemPers[0].name, PMS_FLAG_USER_FIXED);
    EXPECT_EQ(ret, PERM_ERRORCODE_FILE_NOT_EXIST) << "update perms flag ret =" << ret << endl;

    // pkg perm invalid
    ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, PERMISSION_UNSUPPORTED, PMS_FLAG_USER_FIXED);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "update perms flag ret =" << ret << endl;
    ClrPers(permissions);

    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     Security_AppSecurity_PermissionManager_L1_UpdatePermissionFlags_006
 * @tc.name       Updatepermissionflags interface stability test
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUpdatePermissionTest, testSecPMUpdatePMFlags_006, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    if (permissions != NULL)
    {
        EXPECT_EQ(permissions[0].flags, PMS_FLAG_DEFAULT) << "flags =" << permissions[0].flags << endl;
    }
    ClrPers(permissions);

    for (int i = 0; i < PERFORMANCE_RUN_TIMES / 2; i++)
    {
        ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, PMS_FLAG_DEFAULT);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "update perms flag ret =" << ret << endl;
        ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, PMS_FLAG_USER_FIXED);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "update perms flag ret =" << ret << endl;
    }
    ClrPers(permissions);

    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     Security_AppSecurity_PermissionManager_L1_UpdatePermissionFlags_007
 * @tc.name       Updatepermissionflags interface performace test
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUpdatePermissionTest, testSecPMUpdatePMFlags_007, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);

    PermissionSaved *permissions = NULL;
    int permNum = 0;
    int ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret=" << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum =" << permNum << endl;
    if (permissions != NULL)
    {
        EXPECT_EQ(permissions[0].flags, PMS_FLAG_DEFAULT) << "flags =" << permissions[0].flags << endl;
    }
    ClrPers(permissions);

    long startTime = GetCurrentTimeMillis();
    for (int i = 0; i < PERFORMANCE_RUN_TIMES / 2; i++)
    {
        ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, PMS_FLAG_DEFAULT);
        ret = g_interface->UpdatePermissionFlags(TEST_APP_ID, g_systemPers[0].name, PMS_FLAG_USER_FIXED);
    }
    long timeCost = GetCurrentTimeMillis() - startTime;
    long timeAvg = timeCost / 10000;
    EXPECT_LT(timeAvg, 11) << "avg times =" << timeAvg << endl;
    ClrPers(permissions);

    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}