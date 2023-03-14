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
static PermissionTrans g_unDefPer = {
    PERMISSION_UNDEFINED,
    "for undefined use",
    INUSE,
};

class ActsPMSGrantTest : public testing::Test {
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
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1040
 * @tc.name       grant one permission while not load permiossion
 * @tc.desc       [C- SECURITY -0400]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1040, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1050
 * @tc.name       grant permissions while load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1050, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1060
 * @tc.name       grant opened permissions while load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1060, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1070
 * @tc.name       grant unapplied permissions while load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1070, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 1, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "grant ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, 1) << "permNum = " << permNum << endl;
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1080
 * @tc.name       grant system_grant permissions while load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1080, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 0, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_FILE_NOT_EXIST) << "grant ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_FILE_NOT_EXIST) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, 0) << "permNum = " << permNum << endl;
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1090
 * @tc.name       grant user_grant permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1090, Function | MediumTest | Level1)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    int i;
    for (i = 0; i < SYS_PERM_NUM; i++) {
        ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[i].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    }
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        for (i = 0; i < SYS_PERM_NUM; i++) {
            EXPECT_EQ(permissions[i].granted, GRANTED);
        }
    }
    ClrPers(permissions);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1100
 * @tc.name       grant undefined permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1100, Function | MediumTest | Level1)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = g_interface->GrantPermission(TEST_APP_ID, g_unDefPer.name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1220
 * @tc.name       grant permissions after grant runtime permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1220, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    int ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1230
 * @tc.name       grant permissions after revoke permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1230, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, GRANTED);
    }
    ClrPers(permissions);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1240
 * @tc.name       grant permissions continuously
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1240, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret;
    for (int i = 0; i < RELI_TEST_TIME; i++) {
        ret = g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
        if (ret != PERM_ERRORCODE_SUCCESS) {
            break;
        }
    }
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, GRANTED);
    }
    ClrPers(permissions);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1310
 * @tc.name       grant permissions of unnormal task
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1310, Function | MediumTest | Level4)
{
    int ret = g_interface->GrantPermission(PERMISSION_UNSUPPORTED, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_FILE_NOT_EXIST) << "grant ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1340
 * @tc.name       grant permissions of unnormal permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantTest, testSecPMPMS_1340, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = g_interface->GrantPermission(TEST_APP_ID, PERMISSION_UNSUPPORTED);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}