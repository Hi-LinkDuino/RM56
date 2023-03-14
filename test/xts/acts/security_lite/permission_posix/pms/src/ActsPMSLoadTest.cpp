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

class ActsPMSLoadTest : public testing::Test {
protected:
    static void SetUpTestCase(void)
    {
        CreateAppDir();
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(PERMISSION_SERVICE, PERM_INNER);
        iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **) &g_interface);
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

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0510
 * @tc.name       Can load application permission with long and short package names
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0510, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_SHORT, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    SaveOrUpdatePermissions(TEST_APP_LONG, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = LoadPermissions(TEST_APP_SHORT, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = LoadPermissions(TEST_APP_LONG, SUBTEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    UnLoadPermissions(SUBTEST_TASKID);
    DeletePermissions(TEST_APP_SHORT);
    DeletePermissions(TEST_APP_LONG);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0520
 * @tc.name       Load unauthorized permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0520, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    for (int i = 0; i < SYS_PERM_NUM - 1; i++) {
        ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[i].name);
        EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << ", index = " << i <<endl;
    }
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0530
 * @tc.name       Load authorized permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0530, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    for (int i = 1; i < SYS_PERM_NUM - 1; i++) {
        ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[i].name);
        EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << ", index = " << i <<endl;
    }
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0540
 * @tc.name       Load system_grant permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0540, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0550
 * @tc.name       Load user_grant permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0550, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int i;
    for (i = 0; i < SYS_PERM_NUM; i++) {
        g_interface->GrantPermission(TEST_APP_ID, g_systemPers[i].name);
    }
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    for (int i = 0; i < SYS_PERM_NUM; i++) {
        ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[i].name);
        EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << ", index = " << i <<endl;
    }
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0560
 * @tc.name       Load permissions to large and small PIDs
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0560, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = LoadPermissions(TEST_APP_ID, MAX_PID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(MAX_PID);
    ret = LoadPermissions(TEST_APP_ID, MIN_PID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(MIN_PID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0570
 * @tc.name       Load nonexistent application permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0570, Function | MediumTest | Level2)
{
    int ret = LoadPermissions(TEST_APP_ID, MAX_PID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0580
 * @tc.name       Load loaded application permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0580, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0630
 * @tc.name       Duplicate load application permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0630, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0650
 * @tc.name       Load multiple application permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0650, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    SaveOrUpdatePermissions(TEST_APP_ID2, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = LoadPermissions(TEST_APP_ID2, SUBTEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    UnLoadPermissions(SUBTEST_TASKID);
    DeletePermissions(TEST_APP_ID);
    DeletePermissions(TEST_APP_ID2);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0670
 * @tc.name       Load immediately after saving permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0670, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0680
 * @tc.name       Load immediately after updating permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0680, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 0, FIRST_INSTALL);
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, UPDATE);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0690
 * @tc.name       Load immediately after deleting permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0690, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    DeletePermissions(TEST_APP_ID);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0700
 * @tc.name       Duplicate load after deleting permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0700, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0710
 * @tc.name       Load immediately after querying permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0710, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0720
 * @tc.name       Load permission after grant runtime permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0720, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0730
 * @tc.name       Load immediately after granting permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0730, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0740
 * @tc.name       Load immediately after revoking permissions
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0740, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0830
 * @tc.name       Another application permission change does not affect load permission
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0830, Function | MediumTest | Level3)
{
    int pid = fork();
    int status = 0;
    if (pid != 0) {
        SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
        int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
        UnLoadPermissions(TEST_TASKID);
        DeletePermissions(TEST_APP_ID);
        waitpid(pid, &status, 0);
    } else {
        SubTestWithoutCheck(SUBTEST_APP_ID, SUBTEST_TASKID);
        exit(0);
    }
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0850
 * @tc.name       Permission to load exception package names
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0850, Function | MediumTest | Level4)
{
    int ret = LoadPermissions(NULL, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_INVALID_PARAMS) << "load ret = " << ret << endl;
    ret = LoadPermissions(SUBTEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = LoadPermissions(PERMISSION_EMPTY, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_STAT_FAIL) << "load ret = " << ret << endl;
    ret = LoadPermissions(PERMISSION_TOOLONG, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = LoadPermissions(PERMISSION_UNSUPPORTED, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0860
 * @tc.name       Permission to load exception pid
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0860, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = LoadPermissions(TEST_APP_ID, ABNORMAL_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_INVALID_PARAMS) << "load ret = " << ret << endl;
    UnLoadPermissions(ABNORMAL_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0880
 * @tc.name       Permission to load exception data
 * @tc.desc       [C- SECURITY -0900]
 */
HWTEST_F(ActsPMSLoadTest, testSecPMPMS_0880, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int fd = access(TEST_APP_PATH, F_OK);
    if (fd == 0) {
        unlink(TEST_APP_PATH);
    }
    fd = open(TEST_APP_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd >= 0) {
        write(fd, PERMISSION_UNSUPPORTED, strlen(PERMISSION_UNSUPPORTED));
        close(fd);
    }
    int ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_JSONPARSE_FAIL) << "load ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}
