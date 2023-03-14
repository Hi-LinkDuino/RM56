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

class ActsPMSRevokeRuntimeTest : public testing::Test {
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

/*
 * @tc.name: testSecPMPMS_126
 * @tc.desc: revoke permissions to large and small PIDs
 * @tc.type: FUNC
 * @tc.require: AR000E07N7
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_126, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, MAX_PID);
    int ret = g_interface->RevokeRuntimePermission(MAX_PID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(MAX_PID, g_systemPers[0].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(MAX_PID);
    LoadPermissions(TEST_APP_ID, MIN_PID);
    ret = g_interface->RevokeRuntimePermission(MIN_PID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(MIN_PID, g_systemPers[1].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(MIN_PID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_127
 * @tc.desc: revoke permissions while not load permiossion
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_127, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_TASKID_NOT_EXIST) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_128
 * @tc.desc: revoke runtime app permissions
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_128, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_129
 * @tc.desc: revoke permissions without request
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_129, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, 1, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[1].name);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_130
 * @tc.desc: revoke system_grant permissions
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_130, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_131
 * @tc.desc: revoke user_grant permissions
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_131, Function | MediumTest | Level1)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret;
    for (int i = 0; i < SYS_PERM_NUM - 1; i++) {
        ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[i].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
        ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[i].name);
        EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    }
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_132
 * @tc.desc: revoke unknown permissions
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_132, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, PERMISSION_UNDEFINED);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, PERMISSION_UNDEFINED);
    EXPECT_EQ(ret, RET_NOK) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_133
 * @tc.desc: revoke runtime permissions after revoke permission
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_133, Function | MediumTest | Level3)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    g_interface->RevokePermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_134
 * @tc.desc: revoke runtime permissions after grant permission
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_134, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    g_interface->GrantPermission(TEST_APP_ID, g_systemPers[0].name);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_135
 * @tc.desc: revoke runtime permissions continuously
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_135, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret;
    for (int i = 0; i < RELI_TEST_TIME; i++) {
        ret = g_interface->RevokeRuntimePermission(TEST_TASKID, g_systemPers[0].name);
        EXPECT_EQ(ret, PERM_ERRORCODE_SUCCESS) << "revoke ret = " << ret << endl;
        if (ret != PERM_ERRORCODE_SUCCESS) {
            break;
        }
    }
    ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/*
 * @tc.name: testSecPMPMS_136
 * @tc.desc: revoke permissions of unnormal task
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_136, Function | MediumTest | Level3)
{
    int ret = g_interface->RevokeRuntimePermission(ABNORMAL_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, PERM_ERRORCODE_TASKID_NOT_EXIST) << "revoke ret = " << ret << endl;
}

/*
 * @tc.name: testSecPMPMS_137
 * @tc.desc: revoke permissions of unnormal permission
 * @tc.type: FUNC
 * @tc.require: AR000F4FSI
 */
HWTEST_F(ActsPMSRevokeRuntimeTest, testSecPMPMS_137, Function | MediumTest | Level4)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->RevokeRuntimePermission(TEST_TASKID, PERMISSION_UNSUPPORTED);
    EXPECT_EQ(ret, PERM_ERRORCODE_PERM_NOT_EXIST) << "revoke ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}