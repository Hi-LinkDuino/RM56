/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_io_service_if.h"
#include "hdf_uhdf_test.h"
#include "osal_test_case_def.h"

using namespace testing::ext;

#ifndef __LITEOS__
static const char *OSAL_FW_PATH = "/lib/firmware";
static const int OSAL_FW_PATH_MODE = 0x777;
#endif

#define OSAL_TEST_FUNC_DEFINE(subCmd) do { \
    struct HdfTestMsg msg = { TEST_OSAL_ITEM, subCmd, -1 }; \
    printf("OSAL test enter cmd:%d\n\r", subCmd); \
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg)); \
} while (0)

class OsalTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
private:
    static const int OSAL_TEST_TIME = 30;
    static const int OSAL_WAIT_END_TIME = 5;
};

static void OsalCreateDir(bool flag)
{
#if defined(__LITEOS__)
    (void)flag;
#else
    int ret;
    if (flag) {
        ret = mkdir(OSAL_FW_PATH, OSAL_FW_PATH_MODE);
    } else {
        ret = rmdir(OSAL_FW_PATH);
    }
    printf("%s %d %d", __func__, ret, flag);
#endif
}
void OsalTest::SetUpTestCase()
{
    HdfTestOpenService();
    OsalCreateDir(true);
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_START);
    sleep(OSAL_TEST_TIME);
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_END);
    sleep(OSAL_WAIT_END_TIME);
}

void OsalTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void OsalTest::SetUp()
{
}

void OsalTest::TearDown()
{
}

/**
  * @tc.name: OsalGetMutex001
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_CREATE);
}

/**
  * @tc.name: OsalGetMutex002
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_LOCK_TIMEOUT);
}

/**
  * @tc.name: OsalGetMutex003
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_UNLOCK);
}

/**
  * @tc.name: OsalGetMutex004
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_LOCK_FOREVER);
}

/**
  * @tc.name: OsalGetMutex005
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_DESTROY);
}

/**
  * @tc.name: OsalGetMutex006
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_VISIT_DESTROY);
}

/**
  * @tc.name: OsalGetMutex007
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_STRESS_TEST);
}

/**
  * @tc.name: OsalGetMutex008
  * @tc.desc: mutex function test
  * @tc.type: FUNC
  * @tc.require: AR000F868U
  */
HWTEST_F(OsalTest, OsalGetMutex008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MUTEX_OTHER);
}

/**
  * @tc.name: OsalGetSpin001
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_CREATE);
}

/**
  * @tc.name: OsalGetSpin002
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK);
}

/**
  * @tc.name: OsalGetSpin003
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_UNLOCK);
}

/**
  * @tc.name: OsalGetSpin004
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK_IRQ);
}

/**
  * @tc.name: OsalGetSpin005
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_UNLOCK_IRQ);
}

/**
  * @tc.name: OsalGetSpin006
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK_IRQ_SAVE);
}

/**
  * @tc.name: OsalGetSpin007
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_UNLOCK_IRQ_RESTORE);
}

/**
  * @tc.name: OsalGetSpin008
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_DESTROY);
}

/**
  * @tc.name: OsalGetSpin009
  * @tc.desc: Spin function test
  * @tc.type: FUNC
  * @tc.require: AR000F8690
  */
HWTEST_F(OsalTest, OsalGetSpin009, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_LOCK_AFTER_DESTROY);
}

/**
  * @tc.name: OsalGetSpin010
  * @tc.desc: LOG function test
  * @tc.type: FUNC
  * @tc.require: AR000F8695
  */
HWTEST_F(OsalTest, OsalGetSpin010, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_STRESS_TEST);
}

/**
  * @tc.name: OsalGetSpin011
  * @tc.desc: LOG function test
  * @tc.type: FUNC
  * @tc.require: AR000F8695
  */
HWTEST_F(OsalTest, OsalGetSpin011, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SPIN_OTHER);
}

/**
  * @tc.name: OsalGetTime001
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTest, OsalGetTime001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_SLEEP);
}
/**
  * @tc.name: OsalGetTime002
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTest, OsalGetTime002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_MSLEEP);
}
/**
  * @tc.name: OsalGetTime003
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTest, OsalGetTime003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_USLEEP);
}
/**
  * @tc.name: OsalGetTime004
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTest, OsalGetTime004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_GETTIME);
}
/**
  * @tc.name: OsalGetTime005
  * @tc.desc: time function test
  * @tc.type: FUNC
  * @tc.require: AR000F8696
  */
HWTEST_F(OsalTest, OsalGetTime005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIME_DIFFTIME);
}

/**
  * @tc.name: OsalGetIrq001
  * @tc.desc: irq function test
  * @tc.type: FUNC
  * @tc.require: AR000F868Q
  */
HWTEST_F(OsalTest, OsalGetIrq001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_IRQ_UNREG_ABNORMAL);
}
/**
  * @tc.name: OsalGetIrq002
  * @tc.desc: irq function test
  * @tc.type: FUNC
  * @tc.require: AR000F868Q
  */
HWTEST_F(OsalTest, OsalGetIrq002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_IRQ_REG_NORMAL);
}
/**
  * @tc.name: OsalGetIrq003
  * @tc.desc: irq function test
  * @tc.type: FUNC
  * @tc.require: AR000F868Q
  */
HWTEST_F(OsalTest, OsalGetIrq003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_IRQ_UNREG_NORMAL);
}
/**
  * @tc.name: OsalGetIrq004
  * @tc.desc: irq function test
  * @tc.type: FUNC
  * @tc.require: AR000F868Q
  */
HWTEST_F(OsalTest, OsalGetIrq004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_IRQ_REG_NORMAL_STRESS);
}
/**
  * @tc.name: OsalGetIrq005
  * @tc.desc: irq function test
  * @tc.type: FUNC
  * @tc.require: AR000F868Q
  */
HWTEST_F(OsalTest, OsalGetIrq005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_IRQ_UNREG_NORMAL_STRESS);
}

/**
  * @tc.name: OsalGetTimer001
  * @tc.desc: timer function test
  * @tc.type: FUNC
  * @tc.require: AR000F868T
  */
HWTEST_F(OsalTest, OsalGetTimer001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIMER_CREATE_ONCE);
}
/**
  * @tc.name: OsalGetTimer002
  * @tc.desc: timer function test
  * @tc.type: FUNC
  * @tc.require: AR000F868T
  */
HWTEST_F(OsalTest, OsalGetTimer002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIMER_CREATE_LOOP);
}
/**
  * @tc.name: OsalGetTimer003
  * @tc.desc: timer function test
  * @tc.type: FUNC
  * @tc.require: AR000F868T
  */
HWTEST_F(OsalTest, OsalGetTimer003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIMER_RUN_CHECK);
}
/**
  * @tc.name: OsalGetTimer004
  * @tc.desc: timer function test
  * @tc.type: FUNC
  * @tc.require: AR000F868T
  */
HWTEST_F(OsalTest, OsalGetTimer004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIMER_PARA_CHECK);
}
/**
  * @tc.name: OsalGetTimer005
  * @tc.desc: timer function test
  * @tc.type: FUNC
  * @tc.require: AR000F868T
  */
HWTEST_F(OsalTest, OsalGetTimer005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIMER_PERIOD_CHECK);
}
/**
  * @tc.name: OsalGetTimer006
  * @tc.desc: timer function test
  * @tc.type: FUNC
  * @tc.require: AR000F868T
  */
HWTEST_F(OsalTest, OsalGetTimer006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIMER_MODIFY_CHECK);
}
/**
  * @tc.name: OsalGetTimer007
  * @tc.desc: timer function test
  * @tc.type: FUNC
  * @tc.require: AR000F868T
  */
HWTEST_F(OsalTest, OsalGetTimer007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TIMER_STOP_CHECK);
}

/**
  * @tc.name: OsalGetThread001
  * @tc.desc: thread function test
  * @tc.type: FUNC
  * @tc.require: AR000F868S
  */
HWTEST_F(OsalTest, OsalGetThread001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_THREAD_CREATE);
}
/**
  * @tc.name: OsalGetThread002
  * @tc.desc: thread function test
  * @tc.type: FUNC
  * @tc.require: AR000F868S
  */
HWTEST_F(OsalTest, OsalGetThread002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_THREAD_PARA_CHECK);
}
/**
  * @tc.name: OsalGetThread003
  * @tc.desc: thread function test
  * @tc.type: FUNC
  * @tc.require: AR000F868S
  */
HWTEST_F(OsalTest, OsalGetThread003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_THREAD_RUN_CHECK);
}

/**
  * @tc.name: OsalGetFW001
  * @tc.desc: FW function test
  * @tc.type: FUNC
  * @tc.require: AR000F8693
  */
HWTEST_F(OsalTest, OsalGetFW001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FW_REQUEST);
}
/**
  * @tc.name: OsalGetFW002
  * @tc.desc: FW function test
  * @tc.type: FUNC
  * @tc.require: AR000F8693
  */
HWTEST_F(OsalTest, OsalGetFW002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FW_READ);
}
/**
  * @tc.name: OsalGetFW003
  * @tc.desc: FW function test
  * @tc.type: FUNC
  * @tc.require: AR000F8693
  */
HWTEST_F(OsalTest, OsalGetFW003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FW_DATA_CHECK);
}
/**
  * @tc.name: OsalGetFW004
  * @tc.desc: FW function test
  * @tc.type: FUNC
  * @tc.require: AR000F8693
  */
HWTEST_F(OsalTest, OsalGetFW004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FW_SEEK);
}
/**
  * @tc.name: OsalGetFW005
  * @tc.desc: FW function test
  * @tc.type: FUNC
  * @tc.require: AR000F8693
  */
HWTEST_F(OsalTest, OsalGetFW005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FW_RELEASE);
}
/**
  * @tc.name: OsalGetFW006
  * @tc.desc: FW function test
  * @tc.type: FUNC
  * @tc.require: AR000F8693
  */
HWTEST_F(OsalTest, OsalGetFW006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FW_READ_AFTER_RELEASE);
}

/**
  * @tc.name: OsalGetMemory001
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTest, OsalGetMemory001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_BIG);
}
/**
  * @tc.name: OsalGetMemory002
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTest, OsalGetMemory002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_SMALL);
}
/**
  * @tc.name: OsalGetMemory003
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTest, OsalGetMemory003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_BIG_STRESS);
}
/**
  * @tc.name: OsalGetMemory004
  * @tc.desc: memory function test
  * @tc.type: FUNC
  * @tc.require: AR000F868R
  */
HWTEST_F(OsalTest, OsalGetMemory004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_MALLOC_SMALL_STRESS);
}

/**
  * @tc.name: OsalGetList001
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_INIT);
}
/**
  * @tc.name: OsalGetList002
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_EMPTY);
}
/**
  * @tc.name: OsalGetList003
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_COUNT_CHECK);
}
/**
  * @tc.name: OsalGetList004
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_TAIL);
}
/**
  * @tc.name: OsalGetList005
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_HEAD);
}
/**
  * @tc.name: OsalGetList006
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_REMOVE);
}
/**
  * @tc.name: OsalGetList007
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_TRAVERSAL);
}
/**
  * @tc.name: OsalGetList008
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_TRAVERSAL_REMOVE);
}
/**
  * @tc.name: OsalGetList009
  * @tc.desc: list function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetList009, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_LIST_STRESS);
}

/**
  * @tc.name: OsalGetSem001
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTest, OsalGetSem001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_CREATE);
}
/**
  * @tc.name: OsalGetSem002
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTest, OsalGetSem002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_POST);
}
/**
  * @tc.name: OsalGetSem003
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTest, OsalGetSem003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_POST_RESULT);
}
/**
  * @tc.name: OsalGetSem004
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTest, OsalGetSem004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_WAIT_TIMEOUT);
}
/**
  * @tc.name: OsalGetSem005
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTest, OsalGetSem005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_WAIT_FOREVER);
}
/**
  * @tc.name: OsalGetSem006
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTest, OsalGetSem006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_DESTROY);
}
/**
  * @tc.name: OsalGetSem007
  * @tc.desc: sem function test
  * @tc.type: FUNC
  * @tc.require: AR000F868V
  */
HWTEST_F(OsalTest, OsalGetSem007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_SEM_VISIT_AFTER_DESTROY);
}

/**
  * @tc.name: OsalGetWork001
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_WORK_QUEUE_CREATE);
}
/**
  * @tc.name: OsalGetWork002
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_WORK_CREATE);
}
/**
  * @tc.name: OsalGetWork003
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_DELAY_WORK_CREATE);
}
/**
  * @tc.name: OsalGetWork004
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_WORK_ADD);
}
/**
  * @tc.name: OsalGetWork005
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_DELAY_WORK_ADD);
}
/**
  * @tc.name: OsalGetWork006
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_WORK_RUN_CHECK);
}
/**
  * @tc.name: OsalGetWork007
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_DELAY_WORK_RUN_CHECK);
}
/**
  * @tc.name: OsalGetWork008
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_WORK_CANCEL);
}
/**
  * @tc.name: OsalGetWork009
  * @tc.desc: work function test
  * @tc.type: FUNC
  * @tc.require: AR000F8692
  */
HWTEST_F(OsalTest, OsalGetWork009, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_DELAY_WORK_CANCEL);
}

/**
  * @tc.name: OsalGetAtomic001
  * @tc.desc: atomic function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetAtomic001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_ATOMIC_SET);
}
/**
  * @tc.name: OsalGetAtomic002
  * @tc.desc: atomic function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetAtomic002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_ATOMIC_READ);
}
/**
  * @tc.name: OsalGetAtomic003
  * @tc.desc: atomic function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetAtomic003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_ATOMIC_INC);
}
/**
  * @tc.name: OsalGetAtomic004
  * @tc.desc: atomic function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetAtomic004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_ATOMIC_DEC);
}

/**
  * @tc.name: OsalGetBits001
  * @tc.desc: bits function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetBits001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_BIT);
}
/**
  * @tc.name: OsalGetBits002
  * @tc.desc: bits function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetBits002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_SET_BIT);
}
/**
  * @tc.name: OsalGetBits003
  * @tc.desc: bits function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetBits003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_CLR_BIT);
}
/**
  * @tc.name: OsalGetBits004
  * @tc.desc: bits function test
  * @tc.type: FUNC
  * @tc.require: AR000F8691
  */
HWTEST_F(OsalTest, OsalGetBits004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_CLR_BIT);
}

/**
  * @tc.name: OsalGetFile001
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_OPEN_RD);
}
/**
  * @tc.name: OsalGetFile002
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile002, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_READ_RD);
}
/**
  * @tc.name: OsalGetFile003
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile003, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_WRITE_RD);
}
/**
  * @tc.name: OsalGetFile004
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile004, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_SEEK_RD);
}
/**
  * @tc.name: OsalGetFile005
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile005, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_CLOSE_RD);
}
/**
  * @tc.name: OsalGetFile006
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile006, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_OPEN_RDWR);
}
/**
  * @tc.name: OsalGetFile007
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile007, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_READ_RDWR);
}
/**
  * @tc.name: OsalGetFile008
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile008, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_WRITE_RDWR);
}
/**
  * @tc.name: OsalGetFile009
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile009, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_SEEK_RDWR);
}
/**
  * @tc.name: OsalGetFile010
  * @tc.desc: file function test
  * @tc.type: FUNC
  * @tc.require: AR000F8694
  */
HWTEST_F(OsalTest, OsalGetFile010, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_FILE_CLOSE_RDWR);
}

/**
  * @tc.name: OsalGetAll001
  * @tc.desc: all function test
  * @tc.type: FUNC
  * @tc.require: SR000F868P
  */
HWTEST_F(OsalTest, OsalGetAll001, TestSize.Level3)
{
    OSAL_TEST_FUNC_DEFINE(OSAL_TEST_ALL);
}
