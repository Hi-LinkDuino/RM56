/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "acts_pms_test.h"

using namespace std;
using namespace testing::ext;

static PmsInnerApi *g_interface;
static PermissionTrans g_systemPers[] = {
    {
        "ohos.permission.CAMERA",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.RECORD_AUDIO",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.READ_MEDIA_AUDIO",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.READ_MEDIA_IMAGES",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.READ_MEDIA_VIDEO",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.WRITE_MEDIA_AUDIO",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.WRITE_MEDIA_IMAGES",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.WRITE_MEDIA_VIDEO",
        "for test use",
        INUSE,
    },
    {
        "ohos.permission.MODIFY_AUDIO_SETTINGS",
        "for test use",
        INUSE,
    },
};

void CreateAppDir(void)
{
    int ret = mkdir(APP_PREFIX, DIR_MODE);
    if (ret) {
        printf("create dir %s error\n", APP_PREFIX);
    }
    ret = mkdir(ETC_PREFIX, DIR_MODE);
    if (ret) {
        printf("create dir %s error\n", ETC_PREFIX);
    }
    ret = mkdir(PERMISSION_PREFIX, DIR_MODE);
    if (ret) {
        printf("create dir %s error\n", PERMISSION_PREFIX);
    }
}

class ActsPMSCheckTest : public testing::Test {
public:
    static void InitInterface()
    {
        if (g_interface != NULL) {
            return;
        }
        SetUpTestCase();
    }

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


/*
 ** @tc.name: testSecPMPMS_001
 ** @tc.desc: check permission tp large and small PIDS
 ** @tc.type: FUNC
 ** @tc.require: test
 */

HWTEST_F(ActsPMSCheckTest, testSecPMPMS_001, Function | MediumTest | Level0)
{
    SaveOrUpdatePermissions(TEST_APP_ID, g_systemPers, SYS_PERM_NUM, FIRST_INSTALL);
    LoadPermissions(TEST_APP_ID, MAX_PID);
    g_interface->GrantRuntimePermission(MAX_PID, g_systemPers[0].name);
    int ret = g_interface->CheckPermission(MAX_PID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkpermission ret= " << ret << endl;
    UnLoadPermissions(MAX_PID);
    LoadPermissions(TEST_APP_ID, MIN_PID);
    g_interface->GrantRuntimePermission(MIN_PID, g_systemPers[0].name);
    ret = g_interface->CheckPermission(MIN_PID, g_systemPers[0].name);
    EXPECT_EQ(ret, GRANTED) << "checkpermission ret= " << ret << endl;
    UnLoadPermissions(MIN_PID);
    DeletePermissions(TEST_APP_ID);
}
