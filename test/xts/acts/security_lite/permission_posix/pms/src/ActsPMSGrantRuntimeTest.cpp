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

class ActsPMSGrantRuntimeTest : public testing::Test {
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
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0960
 * @tc.name       grant permissions to large and small PIDs
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_0960, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, MAX_PID);
    int ret = g_interface->GrantRuntimePermission(MAX_PID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(MAX_PID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(MAX_PID);
    LoadPermissions(TEST_APP_ID, MIN_PID);
    ret = g_interface->GrantRuntimePermission(MIN_PID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(MIN_PID, g_systemPers[1].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(MIN_PID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0970
 * @tc.name       grant permissions while not load permiossion
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_0970, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_TASKID_NOT_EXIST) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0980
 * @tc.name       grant runtime app permissions
 * @tc.desc       [C- SECURITY -0400]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_0980, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_0990
 * @tc.name       grant opened permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_0990, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[1].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1000
 * @tc.name       grant permissions without request
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1000, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 1, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1010
 * @tc.name       grant system_grant permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1010, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1020
 * @tc.name       grant user_grant permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1020, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret;
    for (int i = 0; i < SYS_PERM_NUM - 1; i++) {
        ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[i].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
        ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[i].name);
        EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    }
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1030
 * @tc.name       grant unknown permissions
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1030, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, PERMISSION_UNDEFINED);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, PERMISSION_UNDEFINED);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1180
 * @tc.name       grant runtime permissions after grant permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1180, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1190
 * @tc.name       grant runtime permissions after revoke permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1190, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1200
 * @tc.name       grant runtime permissions continuously
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1200, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret;
    for (int i = 0; i < RELI_TEST_TIME; i++) {
        ret = g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "grant ret = " << ret << endl;
        if (ret != PERM_ERRORCODE_SUCCESS) {
            break;
        }
    }
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1300
 * @tc.name       grant permissions of unnormal task
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1300, Function | MediumTest | Level3)
{
    int ret = g_interface->GrantRuntimePermission(ABNORMAL_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_TASKID_NOT_EXIST) << "grant ret = " << ret << endl;
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1330
 * @tc.name       grant permissions of unnormal permission
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSGrantRuntimeTest, testSecPMPMS_1330, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->GrantRuntimePermission(TEST_TASKID, PERMISSION_UNSUPPORTED);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "grant ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}