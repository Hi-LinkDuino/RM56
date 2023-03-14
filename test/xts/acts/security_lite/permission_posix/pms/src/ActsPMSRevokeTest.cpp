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

class ActsPMSRevokeTest : public testing::Test {
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
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1110
 * @tc.name       revoke permissions while not load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1110, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
        EXPECT_EQ(permissions[1].granted, NOT_GRANTED);
    }
    ClrPers(permissions);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1120
 * @tc.name       revoke closed permissions while load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1120, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1130
 * @tc.name       revoke permissions while load permiossion
 * @tc.desc       [C- SECURITY -0500]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1130, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1140
 * @tc.name       revoke unapplied permissions while load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1140, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 1, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "revoke ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1150
 * @tc.name       revoke system_grant permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1150, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1160
 * @tc.name       revoke user_grant permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1160, Function | MediumTest | Level1)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    int i;
    for (i = 0; i < SYS_PERM_NUM; i++) {
        g_interface->GrantPermission(TEST_APP_ID, g_systemPers[i].name);
    }
    for (i = 0; i < SYS_PERM_NUM; i++) {
        ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[i].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    }
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        for (i = 0; i < SYS_PERM_NUM; i++) {
            EXPECT_EQ(permissions[i].granted, NOT_GRANTED);
        }
    }
    ClrPers(permissions);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1170
 * @tc.name       revoke undefined permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1170, Function | MediumTest | Level1)
{
    int ret = SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "save ret = " << ret << endl;
    ret = g_interface->GrantPermission(TEST_APP_ID, g_unDefPer.name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1260
 * @tc.name       revoke permissions after grant runtime permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1260, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    int ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
    }
    ClrPers(permissions);
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1270
 * @tc.name       revoke permissions after revoke permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1270, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    int ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    PermissionSaved* permissions = NULL;
    int permNum = 0;
    ret = g_interface->QueryPermission(TEST_APP_ID, &permissions, &permNum);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "query ret = " << ret << endl;
    EXPECT_EQ(permNum, SYS_PERM_NUM) << "permNum = " << permNum << endl;
    if (permissions != NULL) {
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
    }
    ClrPers(permissions);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1280
 * @tc.name       revoke permissions continuously
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1280, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret;
    for (int i = 0; i < RELI_TEST_TIME; i++) {
        ret = g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
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
        EXPECT_EQ(permissions[0].granted, NOT_GRANTED);
    }
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1320
 * @tc.name       revoke permissions of unnormal task
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1320, Function | MediumTest | Level4)
{
    int ret = g_interface->RevokePermission(PERMISSION_UNSUPPORTED, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_FILE_NOT_EXIST) << "revoke ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1350
 * @tc.name       grant permissions of unnormal permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSRevokeTest, testSecPMPMS_1350, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = g_interface->RevokePermission(TEST_APP_ID, PERMISSION_UNSUPPORTED);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}