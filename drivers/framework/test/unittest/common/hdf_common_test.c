/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "hdf_io_service_if.h"
#include "hdf_uhdf_test.h"

static struct HdfIoService *g_testService = NULL;
static struct HdfSBuf *g_msg = NULL;
static struct HdfSBuf *g_reply = NULL;

void HdfTestOpenService(void)
{
    g_testService = HdfIoServiceBind(HDF_TEST_SERVICE_NAME);
    g_msg = HdfSbufObtainDefaultSize();
    if (g_msg == NULL) {
        printf("fail to obtain sbuf data\n\r");
        return;
    }
    g_reply = HdfSbufObtainDefaultSize();
    if (g_reply == NULL) {
        printf("fail to obtain sbuf reply\n\r");
        HdfSbufRecycle(g_msg);
        return;
    }
}

void HdfTestCloseService(void)
{
    if (g_msg != NULL) {
        HdfSbufRecycle(g_msg);
        g_msg = NULL;
    };
    if (g_reply != NULL) {
        HdfSbufRecycle(g_reply);
        g_reply = NULL;
    };
    if (g_testService != NULL) {
        HdfIoServiceRecycle(g_testService);
        g_testService = NULL;
    };
}

int HdfTestSendMsgToService(struct HdfTestMsg *msg)
{
    int ret;
    struct HdfTestMsg *testReply = NULL;
    unsigned int len;
    CHECK_TEST_NULL_PTR_RETURN(g_testService);
    CHECK_TEST_NULL_PTR_RETURN(g_msg);
    CHECK_TEST_NULL_PTR_RETURN(g_reply);
    CHECK_TEST_NULL_PTR_RETURN(msg);

    if (!HdfSbufWriteBuffer(g_msg, msg, sizeof(*msg))) {
        printf("HdfTestSendMsgToService g_msg write failed\n\r");
    }

    ret = g_testService->dispatcher->Dispatch(&g_testService->object, 0, g_msg, g_reply);
    if (ret != HDF_SUCCESS) {
        printf("HdfTestSendMsgToService fail to send service call\n\r");
        return ret;
    }

    if (!HdfSbufReadBuffer(g_reply, (const void **)&testReply, &len)) {
        printf("HdfTestSendMsgToService g_reply read failed\n\r");
    }

    if (testReply == NULL) {
        printf("HdfTestSendMsgToService testReply is null\n\r");
        ret = -1;
    } else {
        ret = testReply->result;
    }
    HdfSbufFlush(g_msg);
    HdfSbufFlush(g_reply);

    return ret;
}

