/**
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
 *
 */

#include "hctest.h"
#include <ohos_init.h>
#include <samgr_lite.h>
#include "samgr_api_test.h"

static int g_initIndex = 0;
static int g_coreInit[API_PRI_MAX] = {0};
static int g_sysRun[API_PRI_MAX] = {0};
static int g_sysServiceInit[API_PRI_MAX] = {0};
static int g_sysFeatureInit[API_PRI_MAX] = {0};
static int g_sysExServiceInit[API_PRI_MAX] = {0};
static int g_sysExFeatureInit[API_PRI_MAX] = {0};
static int g_appServiceInit[API_PRI_MAX] = {0};
static int g_appFeatureInit[API_PRI_MAX] = {0};

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is startup
 * @param        : module name is bootstrap
 * @param        : test suit name is SamgrApiTestSuite
 */
LITE_TEST_SUIT(startup, bootstrap, SamgrApiTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL SamgrApiTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL SamgrApiTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

static void PrintInitIndex(const int initArr[])
{
    for (int i = 0; i < API_PRI_MAX; i++) {
        if (i == API_PRI_DEFAULT) {
            printf("default: %d", initArr[i]);
            continue;
        }
        printf("%d: %d", i, initArr[i]);
    }
}

static void CoreInitDef(void)
{
    g_coreInit[API_PRI_DEFAULT] = g_initIndex;
}

static void CoreInit0(void)
{
    g_coreInit[API_PRI0] = g_initIndex;
}
static void CoreInit1(void)
{
    g_coreInit[API_PRI1] = g_initIndex;
}
static void CoreInit2(void)
{
    g_coreInit[API_PRI2] = g_initIndex;
}
static void CoreInit3(void)
{
    g_coreInit[API_PRI3] = g_initIndex;
}
static void CoreInit4(void)
{
    g_coreInit[API_PRI4] = g_initIndex;
}

static void SysRunDef(void)
{
    g_sysRun[API_PRI_DEFAULT] = g_initIndex;
}
static void SysRun0(void)
{
    g_sysRun[API_PRI0] = g_initIndex;
}
static void SysRun1(void)
{
    g_sysRun[API_PRI1] = g_initIndex;
}
static void SysRun2(void)
{
    g_sysRun[API_PRI2] = g_initIndex;
}
static void SysRun3(void)
{
    g_sysRun[API_PRI3] = g_initIndex;
}
static void SysRun4(void)
{
    g_sysRun[API_PRI4] = g_initIndex;
}

static void SysServiceInitDef(void)
{
    g_sysServiceInit[API_PRI_DEFAULT] = g_initIndex;
}
static void SysServiceInit0(void)
{
    g_sysServiceInit[API_PRI0] = g_initIndex;
}
static void SysServiceInit1(void)
{
    g_sysServiceInit[API_PRI1] = g_initIndex;
}
static void SysServiceInit2(void)
{
    g_sysServiceInit[API_PRI2] = g_initIndex;
}
static void SysServiceInit3(void)
{
    g_sysServiceInit[API_PRI3] = g_initIndex;
}
static void SysServiceInit4(void)
{
    g_sysServiceInit[API_PRI4] = g_initIndex;
}

static void SysFeatureInitDef(void)
{
    g_sysFeatureInit[API_PRI_DEFAULT] = g_initIndex;
}
static void SysFeatureInit0(void)
{
    g_sysFeatureInit[API_PRI0] = g_initIndex;
}
static void SysFeatureInit1(void)
{
    g_sysFeatureInit[API_PRI1] = g_initIndex;
}
static void SysFeatureInit2(void)
{
    g_sysFeatureInit[API_PRI2] = g_initIndex;
}
static void SysFeatureInit3(void)
{
    g_sysFeatureInit[API_PRI3] = g_initIndex;
}
static void SysFeatureInit4(void)
{
    g_sysFeatureInit[API_PRI4] = g_initIndex;
}

static void SysExSerInitDef(void)
{
    g_sysExServiceInit[API_PRI_DEFAULT] = g_initIndex;
}
static void SysExSerInit0(void)
{
    g_sysExServiceInit[API_PRI0] = g_initIndex;
}
static void SysExSerInit1(void)
{
    g_sysExServiceInit[API_PRI1] = g_initIndex;
}
static void SysExSerInit2(void)
{
    g_sysExServiceInit[API_PRI2] = g_initIndex;
}
static void SysExSerInit3(void)
{
    g_sysExServiceInit[API_PRI3] = g_initIndex;
}
static void SysExSerInit4(void)
{
    g_sysExServiceInit[API_PRI4] = g_initIndex;
}

static void SysExFeaInitDef(void)
{
    g_sysExFeatureInit[API_PRI_DEFAULT] = g_initIndex;
}
static void SysExFeaInit0(void)
{
    g_sysExFeatureInit[API_PRI0] = g_initIndex;
}
static void SysExFeaInit1(void)
{
    g_sysExFeatureInit[API_PRI1] = g_initIndex;
}
static void SysExFeaInit2(void)
{
    g_sysExFeatureInit[API_PRI2] = g_initIndex;
}
static void SysExFeaInit3(void)
{
    g_sysExFeatureInit[API_PRI3] = g_initIndex;
}
static void SysExFeaInit4(void)
{
    g_sysExFeatureInit[API_PRI4] = g_initIndex;
}

static void AppServiceInitDef(void)
{
    g_appServiceInit[API_PRI_DEFAULT] = g_initIndex;
}
static void AppServiceInit0(void)
{
    g_appServiceInit[API_PRI0] = g_initIndex;
}
static void AppServiceInit1(void)
{
    g_appServiceInit[API_PRI1] = g_initIndex;
}
static void AppServiceInit2(void)
{
    g_appServiceInit[API_PRI2] = g_initIndex;
}
static void AppServiceInit3(void)
{
    g_appServiceInit[API_PRI3] = g_initIndex;
}
static void AppServiceInit4(void)
{
    g_appServiceInit[API_PRI4] = g_initIndex;
}

static void AppFeatureInitDef(void)
{
    g_appFeatureInit[API_PRI_DEFAULT] = g_initIndex;
}
static void AppFeatureInit0(void)
{
    g_appFeatureInit[API_PRI0] = g_initIndex;
}
static void AppFeatureInit1(void)
{
    g_appFeatureInit[API_PRI1] = g_initIndex;
}
static void AppFeatureInit2(void)
{
    g_appFeatureInit[API_PRI2] = g_initIndex;
}
static void AppFeatureInit3(void)
{
    g_appFeatureInit[API_PRI3] = g_initIndex;
}
static void AppFeatureInit4(void)
{
    g_appFeatureInit[API_PRI4] = g_initIndex;
}

// 接口注册后的启动顺序依赖hos_init.h中的定义的数字大小

// 第0个执行，内部按照0~4优先级执行，default优先级为2
CORE_INIT(CoreInitDef);
CORE_INIT_PRI(CoreInit0, 0);
CORE_INIT_PRI(CoreInit1, 1);
CORE_INIT_PRI(CoreInit4, 4);
CORE_INIT_PRI(CoreInit2, 2);
CORE_INIT_PRI(CoreInit3, 3);

// 最后一个执行，内部按照0~4优先级执行，default优先级为2
SYS_RUN(SysRunDef);
SYS_RUN_PRI(SysRun0, 0);
SYS_RUN_PRI(SysRun1, 1);
SYS_RUN_PRI(SysRun4, 4);
SYS_RUN_PRI(SysRun2, 2);
SYS_RUN_PRI(SysRun3, 3);

// 第1个执行，内部按照0~4优先级执行，default优先级为2
SYS_SERVICE_INIT(SysServiceInitDef);
SYS_SERVICE_INIT_PRI(SysServiceInit0, 0);
SYS_SERVICE_INIT_PRI(SysServiceInit1, 1);
SYS_SERVICE_INIT_PRI(SysServiceInit4, 4);
SYS_SERVICE_INIT_PRI(SysServiceInit2, 2);
SYS_SERVICE_INIT_PRI(SysServiceInit3, 3);

// 第2个执行，内部按照0~4优先级执行，default优先级为2
SYS_FEATURE_INIT(SysFeatureInitDef);
SYS_FEATURE_INIT_PRI(SysFeatureInit0, 0);
SYS_FEATURE_INIT_PRI(SysFeatureInit2, 2);
SYS_FEATURE_INIT_PRI(SysFeatureInit1, 1);
SYS_FEATURE_INIT_PRI(SysFeatureInit3, 3);
SYS_FEATURE_INIT_PRI(SysFeatureInit4, 4);

// 第3个执行，注意和APP_SERVICE_INIT_XXX并列，谁先注册先执行谁，内部按照0~4优先级执行，default优先级为2
SYSEX_SERVICE_INIT(SysExSerInitDef);
SYSEX_SERVICE_INIT_PRI(SysExSerInit0, 0);
SYSEX_SERVICE_INIT_PRI(SysExSerInit1, 1);
SYSEX_SERVICE_INIT_PRI(SysExSerInit2, 2);
SYSEX_SERVICE_INIT_PRI(SysExSerInit3, 3);
SYSEX_SERVICE_INIT_PRI(SysExSerInit4, 4);

// 第4个执行，注意和APP_FEATURE_INIT_XXX并列，谁先注册先执行谁，内部按照0~4优先级执行，default优先级为2
SYSEX_FEATURE_INIT(SysExFeaInitDef);
SYSEX_FEATURE_INIT_PRI(SysExFeaInit0, 0);
SYSEX_FEATURE_INIT_PRI(SysExFeaInit1, 1);
SYSEX_FEATURE_INIT_PRI(SysExFeaInit2, 2);
SYSEX_FEATURE_INIT_PRI(SysExFeaInit3, 3);
SYSEX_FEATURE_INIT_PRI(SysExFeaInit4, 4);

// 第3个执行，注意和SYSEX_SERVICE_INIT_XXX并列，谁先注册先执行谁，内部按照0~4优先级执行，default优先级为2
APP_SERVICE_INIT(AppServiceInitDef);
APP_SERVICE_INIT_PRI(AppServiceInit0, 0);
APP_SERVICE_INIT_PRI(AppServiceInit4, 4);
APP_SERVICE_INIT_PRI(AppServiceInit3, 3);
APP_SERVICE_INIT_PRI(AppServiceInit2, 2);
APP_SERVICE_INIT_PRI(AppServiceInit1, 1);

// 第4个执行，注意和SYSEX_FEATURE_INIT_XXX并列，谁先注册先执行谁，内部按照0~4优先级执行，default优先级为2
APP_FEATURE_INIT(AppFeatureInitDef);
APP_FEATURE_INIT_PRI(AppFeatureInit0, 0);
APP_FEATURE_INIT_PRI(AppFeatureInit1, 1);
APP_FEATURE_INIT_PRI(AppFeatureInit3, 3);
APP_FEATURE_INIT_PRI(AppFeatureInit4, 4);
APP_FEATURE_INIT_PRI(AppFeatureInit2, 2);

/**
 * @tc.number    : SUB_START_Bootstrap_Samgr_0100
 * @tc.name      : check total init index
 * @tc.desc      : check total init index, return success
 */
LITE_TEST_CASE(SamgrApiTestSuite, TestStartBootstrapSamgr001, Function | MediumTest | Level0)
{
    printf("[TestStartBootstrapSamgr001:%d]", g_initIndex);
    TEST_ASSERT_EQUAL_INT(g_initIndex, SAMGR_API_GROUP_NUM*SAMGR_API_CALL_NUM);
};

/**
 * @tc.number    : SUB_START_Bootstrap_Samgr_0200
 * @tc.name      : check core init index
 * @tc.desc      : check core init, return success
 */
LITE_TEST_CASE(SamgrApiTestSuite, TestStartBootstrapSamgr002, Function | MediumTest | Level0)
{
    printf("g_coreInit");
    PrintInitIndex(g_coreInit);
    TEST_ASSERT_EQUAL_INT(g_coreInit[API_PRI0], GET_INIT_INDEX(CORE_INIT_E, API_PRI0));
};

/**
 * @tc.number    : SUB_START_Bootstrap_Samgr_0300
 * @tc.name      : check system service init index
 * @tc.desc      : check system service init, return success
 */
LITE_TEST_CASE(SamgrApiTestSuite, TestStartBootstrapSamgr003, Function | MediumTest | Level0)
{
    printf("g_sysServiceInit");
    PrintInitIndex(g_sysServiceInit);
    TEST_ASSERT_EQUAL_INT(g_sysServiceInit[API_PRI0], GET_INIT_INDEX(SYS_SERVICE_INIT_E, API_PRI0));
};

/**
 * @tc.number    : SUB_START_Bootstrap_Samgr_0400
 * @tc.name      : check system feature init index
 * @tc.desc      : check system feature init, return success
 */
LITE_TEST_CASE(SamgrApiTestSuite, TestStartBootstrapSamgr004, Function | MediumTest | Level0)
{
    printf("g_sysFeatureInit");
    PrintInitIndex(g_sysFeatureInit);
    TEST_ASSERT_EQUAL_INT(g_sysFeatureInit[API_PRI0], GET_INIT_INDEX(SYS_FEATURE_INIT_E, API_PRI0));
};

/**
 * @tc.number    : SUB_START_Bootstrap_Samgr_0500
 * @tc.name      : check system extend service and app service init index
 * @tc.desc      : check system extend service and app service init, return success
 */
LITE_TEST_CASE(SamgrApiTestSuite, TestStartBootstrapSamgr005, Function | MediumTest | Level0)
{
    printf("g_sysExServiceInit");
    PrintInitIndex(g_sysExServiceInit);
    printf("g_appServiceInit");
    PrintInitIndex(g_appServiceInit);
    TEST_ASSERT_EQUAL_INT(g_sysExServiceInit[API_PRI0], GET_INIT_INDEX(APP_SERVICE_INIT_E, 0));
};

/**
 * @tc.number    : SUB_START_Bootstrap_Samgr_0600
 * @tc.name      : check system extend feature and app feature init index
 * @tc.desc      : check system extend feature and app feature init, return success
 */
LITE_TEST_CASE(SamgrApiTestSuite, TestStartBootstrapSamgr006, Function | MediumTest | Level0)
{
    printf("g_sysExFeatureInit");
    PrintInitIndex(g_sysExFeatureInit);
    printf("g_appFeatureInit");
    PrintInitIndex(g_appFeatureInit);
    TEST_ASSERT_EQUAL_INT(g_sysExFeatureInit[API_PRI0], GET_INIT_INDEX(APP_FEATURE_INIT_E, 0));
};

/**
 * @tc.number    : SUB_START_Bootstrap_Samgr_0700
 * @tc.name      : check system run index
 * @tc.desc      : check system run, return success
 */
LITE_TEST_CASE(SamgrApiTestSuite, TestStartBootstrapSamgr007, Function | MediumTest | Level0)
{
    printf("g_sysRun");
    PrintInitIndex(g_sysRun);
    TEST_ASSERT_EQUAL_INT(g_sysRun[API_PRI0], GET_INIT_INDEX(SYS_RUN_E, API_PRI0));
};

RUN_TEST_SUITE(SamgrApiTestSuite);
