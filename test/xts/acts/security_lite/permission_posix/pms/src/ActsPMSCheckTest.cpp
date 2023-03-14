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

class ActsPMSCheckTest : public testing::Test {
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
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1390
 * @tc.name       check permissions to large and small PIDs
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(ActsPMSCheckTest, testSecPMPMS_1390, Function | MediumTest | Level2)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, MAX_PID);
    g_interface->GrantRuntimePermission(MAX_PID, g_systemPers[0].name);
    int ret = g_interface->CheckPermission(MAX_PID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(MAX_PID);
    LoadPermissions(TEST_APP_ID, MIN_PID);
    g_interface->GrantRuntimePermission(MIN_PID, g_systemPers[1].name);
    ret = g_interface->CheckPermission(MIN_PID, g_systemPers[1].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(MIN_PID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1400
 * @tc.name       check system_grant permissions
 * @tc.desc       [C- SECURITY -1100]
 */
HWTEST_F(ActsPMSCheckTest, testSecPMPMS_1400, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    int ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[SYS_PERM_NUM - 1].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1410
 * @tc.name       check granted user_grant permissions
 * @tc.desc       [C- SECURITY -1100]
 */
HWTEST_F(ActsPMSCheckTest, testSecPMPMS_1410, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    g_interface->GrantRuntimePermission(TEST_TASKID, g_systemPers[0].name);
    int ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}

/**
 * @tc.number     SUB_SEC_AppSEC_PermissionMgmt_PMS_1420
 * @tc.name       check user_grant permissions
 * @tc.desc       [C- SECURITY -1100]
 */
HWTEST_F(ActsPMSCheckTest, testSecPMPMS_1420, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, TEST_TASKID);
    int ret = g_interface->CheckPermission(TEST_TASKID, g_systemPers[0].name);
    EXPECT_EQ(ret, NOT_GRANTED) << "checkperm ret = " << ret << endl;
    UnLoadPermissions(TEST_TASKID);
    DeletePermissions(TEST_APP_ID);
}