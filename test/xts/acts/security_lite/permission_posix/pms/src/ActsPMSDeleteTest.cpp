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

class ActsPMSDeleteTest : public testing::Test {
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
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0200
 * @tc.name       The application is unloaded and the permission data is deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0200, Function | MediumTest | Level0)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0210
 * @tc.name       Unload unknown applications
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0210, Function | MediumTest | Level2)
{
    int ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0230
 * @tc.name       Delete multiple application permissions
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0230, Function | MediumTest | Level3)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = SaveOrUpdatePermissions(TEST_APP_ID2, g_systemPers, 0, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID2);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
    CheckAppPermission(TEST_APP_ID2, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0250
 * @tc.name       After the application permissions are loaded, the permissions can be deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0250, Function | MediumTest | Level3)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
    ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0260
 * @tc.name       After the application permissions are unloaded, the permissions can be deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0260, Function | MediumTest | Level2)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0270
 * @tc.name       After the application permissions are checked, the permissions can be deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0270, Function | MediumTest | Level2)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    for (int i = 0; i < SYS_PERM_NUM; i++) {
        g_interface->GrantPermission(TEST_APP_ID, g_systemPers[i].name);
    }
    ret = LoadPermissions(TEST_APP_ID, TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "load ret = " << ret << endl;
    ret = CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
    ret = UnLoadPermissions(TEST_TASKID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "unload ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0280
 * @tc.name       After the application permissions are queryed, the permissions can be deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0280, Function | MediumTest | Level4)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_SUCCESS, SYS_PERM_NUM, g_systemPers);
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0290
 * @tc.name       After the application permissions are granted, the permissions can be deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0290, Function | MediumTest | Level2)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    for (int i = 0; i < SYS_PERM_NUM; i++) {
        ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[i].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    }
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0300
 * @tc.name       After the application permissions are revoked, the permissions can be deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0300, Function | MediumTest | Level2)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    for (int i = 0; i < SYS_PERM_NUM; i++) {
        ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[i].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    }
    ret = DeletePermissions(TEST_APP_ID);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    CheckAppPermission(TEST_APP_ID, PERM_ERRORCODE_FILE_NOT_EXIST, 0, g_systemPers);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0310
 * @tc.name       Another application permission change does not affect delete permission
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0310, Function | MediumTest | Level3)
{
    int pid = fork();
    int status = 0;
    if (pid != 0) {
        SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
        int ret = DeletePermissions(TEST_APP_ID);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
        waitpid(pid, &status, 0);
    } else {
        SubTestWithoutCheck(SUBTEST_APP_ID, SUBTEST_TASKID);
        exit(0);
    }
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0420
 * @tc.name       The permission file is abnormal and can be deleted
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0420, Function | MediumTest | Level4)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    int fd = access(TEST_APP_PATH, F_OK);
    if (fd == 0) {
        unlink(TEST_APP_PATH);
    }
    fd = open(TEST_APP_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd >= 0) {
        write(fd, PERMISSION_UNSUPPORTED, strlen(PERMISSION_UNSUPPORTED));
        close(fd);
    }
    DeletePermissions(TEST_APP_ID);
    fd = open(TEST_APP_PATH, O_RDONLY);
    EXPECT_LT(fd, 0);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0500
 * @tc.name       Can delete application permission with long and short package names
 * @tc.desc       [C- SECURITY -0800]
 */
HWTEST_F(ActsPMSDeleteTest, testSecPMPMS_0500, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_SHORT, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    SaveOrUpdatePermissions(TEST_APP_LONG, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = DeletePermissions(TEST_APP_SHORT);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
    ret = DeletePermissions(TEST_APP_LONG);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "delete ret = " << ret << endl;
}