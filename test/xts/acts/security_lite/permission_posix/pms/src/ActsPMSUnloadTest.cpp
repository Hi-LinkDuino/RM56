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

class ActsPMSUnloadTest : public testing::Test {
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
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0590
 * @tc.name       Unload permissions to large and small PIDs
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0590, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, MAX_PID);
    int ret = UnLoadPermissions(MAX_PID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    LoadPermissions(TEST_APP_ID, MIN_PID);
    ret = UnLoadPermissions(MIN_PID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0600
 * @tc.name       Unload permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0600, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0610
 * @tc.name       Unload permissions not loaded
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0610, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0620
 * @tc.name       Unload permissions not installed
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0620, Function | MediumTest | Level4)
{
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0640
 * @tc.name       Duplicate unload application permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0640, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0660
 * @tc.name       Unload multiple application permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0660, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    SaveOrUpdatePermissions(TEST_APP_ID2, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    LoadPermissions(TEST_APP_ID2, SUBTEST_TASKID);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = UnLoadPermissions(SUBTEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
    DeletePermissions(TEST_APP_ID2);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0750
 * @tc.name       Unload immediately after installing permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0750, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 0, FIRST_INSTALL);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0760
 * @tc.name       Unload immediately after updating permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0760, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 0, UPDATE);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0770
 * @tc.name       Unload immediately after deleting permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0770, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    DeletePermissions(TEST_APP_ID);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0780
 * @tc.name       Unload after loading and deleting permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0780, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0790
 * @tc.name       Unload immediately after querying permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0790, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    CheckPermission(TEST_TASKID, g_systemPers[0].name);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0800
 * @tc.name       Unload permission after grant runtime permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0800, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0810
 * @tc.name       Unload immediately after granting permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0810, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0820
 * @tc.name       Unload immediately after revoking permissions
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0820, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0840
 * @tc.name       Another application permission change does not affect unload permission
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0840, Function | MediumTest | Level3)
{
    int pid = fork();
    int status = 0;
    if (pid != 0) {
        SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
        LoadPermissions(TEST_APP_ID, TEST_TASKID);
        int ret = UnLoadPermissions(TEST_TASKID);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
        DeletePermissions(TEST_APP_ID);
        waitpid(pid, &status, 0);
    } else {
        SubTestWithoutCheck(SUBTEST_APP_ID, SUBTEST_TASKID);
        exit(0);
    }
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0900
 * @tc.name       Permission to unload exception data
 * @tc.desc       [C- SECURITY -1000]
 */
HWTEST_F(ActsPMSUnloadTest, testSecPMPMS_0900, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int fd = access(TEST_APP_PATH, F_OK);
    if (fd == 0) {
        unlink(TEST_APP_PATH);
    }
    fd = open(TEST_APP_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd >= 0) {
        write(fd, PERMISSION_UNSUPPORTED, strlen(PERMISSION_UNSUPPORTED));
        close(fd);
    }
    int ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}