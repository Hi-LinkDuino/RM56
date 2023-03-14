/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_netbuf.h"
#include "hdf_log.h"
#include "hdf_wifi_test.h"

#define DEFAULT_NETBUF_SIZE     100
#define DEFAULT_OP_SIZE         10
#define DEFAULT_HEAD_SIZE       10
#define DEFAULT_TAIL_SIZE       20
#define DEFAULT_MAPPING_QUEUE   10
#define DEFAULT_TRIM_SIZE       3
#define DEFAULT_SECTION_SIZE    10

#define MAX_MEM_ALLOC_SIZE 0x7fffffff

typedef int32_t (*pTestCaseFunc)(void);

/*
* @tc.name: HdfNetBufTest001
* @tc.desc: Alloc net buffer
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest001(void)
{
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }
    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest002
* @tc.desc: Alloc net buffer for invalid size
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest002(void)
{
    NetBuf *nb = NetBufAlloc(MAX_MEM_ALLOC_SIZE);
    if (nb == NULL) {
        return HDF_SUCCESS;
    }
    NetBufFree(nb);
    return HDF_FAILURE;
}

/*
* @tc.name: HdfNetBufTest003
* @tc.desc: NetBufPush test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest003(void)
{
    void *tmp = NULL;
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        return HDF_FAILURE;
    }

    tmp = NetBufPush(nb, E_DATA_BUF, DEFAULT_OP_SIZE);
    if (tmp == NULL) {
        NetBufFree(nb);
        HDF_LOGE("NetBuf Push fail");
        return HDF_FAILURE;
    }

    if (NetBufGetRoom(nb, E_DATA_BUF) != DEFAULT_OP_SIZE) {
        NetBufFree(nb);
        HDF_LOGE("NetBuf datasize error.");
        return HDF_FAILURE;
    }

    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest004
* @tc.desc: NetBufPush test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest004(void)
{
    void *tmp = NULL;
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        return HDF_FAILURE;
    }

    tmp = NetBufPush(nb, E_DATA_BUF, DEFAULT_OP_SIZE);
    if (tmp == NULL) {
        NetBufFree(nb);
        HDF_LOGE("NetBuf push data fail");
        return HDF_FAILURE;
    }

    tmp = NetBufPush(nb, E_HEAD_BUF, DEFAULT_OP_SIZE);
    if (tmp == NULL) {
        NetBufFree(nb);
        HDF_LOGE("NetBuf push head fail");
        return HDF_FAILURE;
    }

#ifdef __LITEOS__
    if (NetBufGetRoom(nb, E_HEAD_BUF) != DEFAULT_OP_SIZE || NetBufGetRoom(nb, E_DATA_BUF) != 0) {
#else
    if (NetBufGetRoom(nb, E_HEAD_BUF) < DEFAULT_OP_SIZE) {
#endif
        NetBufFree(nb);
        HDF_LOGE("NetBuf datasize error.");
        return HDF_FAILURE;
    }

    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest005
* @tc.desc: NetBufPop test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest005(void)
{
    void *tmp = NULL;
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        return HDF_FAILURE;
    }

    NetBufPush(nb, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb, E_HEAD_BUF, DEFAULT_OP_SIZE);
    tmp = NetBufPop(nb, E_HEAD_BUF, DEFAULT_OP_SIZE);
    if (tmp == NULL) {
        NetBufFree(nb);
        HDF_LOGE("NetBuf Pull fail");
        return HDF_FAILURE;
    }

#ifdef __LITEOS__
    if (NetBufGetRoom(nb, E_DATA_BUF) != DEFAULT_OP_SIZE || NetBufGetRoom(nb, E_HEAD_BUF) != 0) {
#else
    if (NetBufGetRoom(nb, E_DATA_BUF) < DEFAULT_OP_SIZE) {
#endif
        NetBufFree(nb);
        HDF_LOGE("NetBuf datasize error.");
        return HDF_FAILURE;
    }

    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest006
* @tc.desc: NetBufPop test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest006(void)
{
    void *tmp = NULL;
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        return HDF_FAILURE;
    }

    NetBufPush(nb, E_DATA_BUF, DEFAULT_OP_SIZE);
    tmp = NetBufPop(nb, E_DATA_BUF, DEFAULT_OP_SIZE);
    if (tmp == NULL) {
        NetBufFree(nb);
        HDF_LOGE("NetBuf Pull fail");
        return HDF_FAILURE;
    }

#ifdef __LITEOS__
    if (NetBufGetRoom(nb, E_HEAD_BUF) != DEFAULT_OP_SIZE || NetBufGetRoom(nb, E_DATA_BUF) != 0) {
#else
    if (NetBufGetRoom(nb, E_HEAD_BUF) < DEFAULT_OP_SIZE) {
#endif
        NetBufFree(nb);
        HDF_LOGE("NetBuf datasize error.");
        return HDF_FAILURE;
    }

    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest007
* @tc.desc: NetBufGetRoom test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest007(void)
{
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        return HDF_FAILURE;
    }

#ifdef __LITEOS__
    if (NetBufGetRoom(nb, E_HEAD_BUF) != 0) {
        NetBufFree(nb);
        HDF_LOGE("NetBufGetRoom head fail");
        return HDF_FAILURE;
    }
#endif

    if (NetBufGetRoom(nb, E_DATA_BUF) != 0) {
        NetBufFree(nb);
        HDF_LOGE("NetBufGetRoom data fail");
        return HDF_FAILURE;
    }

#ifdef __LITEOS__
    if (NetBufGetRoom(nb, E_TAIL_BUF) != DEFAULT_NETBUF_SIZE) {
#else
    if (NetBufGetRoom(nb, E_TAIL_BUF) < DEFAULT_NETBUF_SIZE) {
#endif
        NetBufFree(nb);
        HDF_LOGE("NetBufGetRoom tail fail");
        return HDF_FAILURE;
    }

    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest008
* @tc.desc: NetBufGetRoom test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest008(void)
{
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        return HDF_FAILURE;
    }

    NetBufPush(nb, E_DATA_BUF, DEFAULT_OP_SIZE);

#ifdef __LITEOS__
    if (NetBufGetRoom(nb, E_TAIL_BUF) != (DEFAULT_NETBUF_SIZE - DEFAULT_OP_SIZE)) {
#else
    if (NetBufGetRoom(nb, E_TAIL_BUF) < (DEFAULT_NETBUF_SIZE - DEFAULT_OP_SIZE)) {
#endif
        NetBufFree(nb);
        HDF_LOGE("NetBufTailRoom fail");
        return HDF_FAILURE;
    }

    NetBufFree(nb);

    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest009
* @tc.desc: NetBufExpandRoom test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest009(void)
{
    int32_t ret;
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        return HDF_FAILURE;
    }

    ret = NetBufResizeRoom(nb, DEFAULT_HEAD_SIZE, DEFAULT_TAIL_SIZE);
    if (ret != 0) {
        NetBufFree(nb);
        HDF_LOGE("NetBufExpandRoom fail");
        return HDF_FAILURE;
    }

#ifdef __LITEOS__
    if (NetBufGetRoom(nb, E_HEAD_BUF) != DEFAULT_HEAD_SIZE) {
#else
    if (NetBufGetRoom(nb, E_HEAD_BUF) < DEFAULT_HEAD_SIZE) {
#endif
        NetBufFree(nb);
        HDF_LOGE("NetBufHeadRoom fail");
        return HDF_FAILURE;
    }

    if (NetBufGetRoom(nb, E_TAIL_BUF) < (DEFAULT_NETBUF_SIZE + DEFAULT_TAIL_SIZE)) {
        NetBufFree(nb);
        HDF_LOGE("NetBufTailRoom fail, tailroom:%d", NetBufGetRoom(nb, E_TAIL_BUF));
        return HDF_FAILURE;
    }

    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest010
* @tc.desc: NetBufConcat test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest010(void)
{
    uint32_t ret;
    NetBuf *dst = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    NetBuf *src = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (dst == NULL || src == NULL) {
        NetBufFree(src);
        NetBufFree(dst);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(dst, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(src, E_DATA_BUF, DEFAULT_OP_SIZE);

    ret = NetBufConcat(dst, src);
    if (ret != HDF_SUCCESS) {
        NetBufFree(src);
        NetBufFree(dst);
        HDF_LOGE("NetBufConcat fail");
        return HDF_FAILURE;
    }

    if (NetBufGetDataLen(dst) != (DEFAULT_OP_SIZE + DEFAULT_OP_SIZE)) {
        NetBufFree(dst);
        HDF_LOGE("NetBufGetLen fail");
        return HDF_FAILURE;
    }

    NetBufFree(dst);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest011
* @tc.desc: NetBufConcat test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest011(void)
{
    uint32_t ret, dstDataLen;
    NetBuf *dst = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    NetBuf *src = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (dst == NULL || src == NULL) {
        NetBufFree(src);
        NetBufFree(dst);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    /* push large enough because there is maybe padding bytes exists */
    dstDataLen = DEFAULT_OP_SIZE * DEFAULT_SECTION_SIZE;
    NetBufPush(dst, E_DATA_BUF, dstDataLen);
    NetBufPush(src, E_DATA_BUF, DEFAULT_NETBUF_SIZE - 1);

    ret = NetBufConcat(dst, src);
    if (ret == HDF_SUCCESS) {
        NetBufFree(dst);
        HDF_LOGE("NetBufConcat success");
        return HDF_FAILURE;
    }

    if (NetBufGetDataLen(dst) != dstDataLen) {
        NetBufFree(src);
        NetBufFree(dst);
        HDF_LOGE("NetBufGetLen fail");
        return HDF_FAILURE;
    }

    NetBufFree(src);
    NetBufFree(dst);
    return HDF_SUCCESS;
}

#ifdef __LITEOS__
/*
* @tc.name: HdfNetBufTest012
* @tc.desc: NetBuf2Pbuf test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest012(void)
{
    struct pbuf *lwipBuf = NULL;
    NetBuf *nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb == NULL) {
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(nb, E_DATA_BUF, DEFAULT_OP_SIZE);

    lwipBuf = NetBuf2Pbuf(nb);
    if (lwipBuf == NULL) {
        NetBufFree(nb);
        HDF_LOGE("NetBuf2Pbuf fail");
        return HDF_FAILURE;
    }

    pbuf_free(lwipBuf);
    NetBufFree(nb);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufTest013
* @tc.desc: Pbuf2NetBuf test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufTest013(void)
{
    struct pbuf *lwipBuf = NULL;
    NetBuf *nb = NULL;

    lwipBuf = pbuf_alloc(PBUF_RAW, DEFAULT_NETBUF_SIZE, PBUF_RAM);
    if (lwipBuf == NULL) {
        HDF_LOGE("pbuf_alloc fail");
        return HDF_FAILURE;
    }

    nb = Pbuf2NetBuf(NULL, lwipBuf);
    if (nb == NULL) {
        pbuf_free(lwipBuf);
        NetBufFree(nb);
        HDF_LOGE("Pbuf2NetBuf fail");
        return HDF_FAILURE;
    }

    pbuf_free(lwipBuf);
    NetBufFree(nb);
    return HDF_SUCCESS;
}
#endif

/*
* @tc.name: HdfNetBufQueueTest001
* @tc.desc: NetBufQueueInit test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest001(void)
{
    NetBufQueue q;

    NetBufQueueInit(&q);

    if (!NetBufQueueIsEmpty(&q)) {
        HDF_LOGE("NetBufQueueIsEmpty fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest002
* @tc.desc: NetBufQueueSize test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest002(void)
{
    NetBufQueue q;

    NetBufQueueInit(&q);

    if (NetBufQueueSize(&q) != 0) {
        HDF_LOGE("NetBufQueueIsEmpty fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest003
* @tc.desc: NetBufQueueEnqueue test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest003(void)
{
    NetBufQueue q;
    NetBuf *nb1 = NULL;
    NetBuf *nb2 = NULL;
    uint32_t size = 0;

    NetBufQueueInit(&q);

    nb1 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb2 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb1 == NULL || nb2 == NULL) {
        NetBufFree(nb1);
        NetBufFree(nb2);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(nb1, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb2, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufQueueEnqueue(&q, nb1);
    size++;
    NetBufQueueEnqueue(&q, nb2);
    size++;
    if (NetBufQueueSize(&q) != size || NetBufQueueIsEmpty(&q)) {
        HDF_LOGE("NetBufQueueIsEmpty fail");
        NetBufQueueClear(&q);
        return HDF_FAILURE;
    }

    NetBufQueueClear(&q);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest004
* @tc.desc: NetBufQueueAtHead test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest004(void)
{
    NetBufQueue q;
    NetBuf *nb = NULL;
    NetBuf *nb1 = NULL;
    NetBuf *nb2 = NULL;

    NetBufQueueInit(&q);

    nb1 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb2 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb1 == NULL || nb2 == NULL) {
        HDF_LOGE("NetBufAlloc fail");
        NetBufFree(nb1);
        NetBufFree(nb2);
        return HDF_FAILURE;
    }

    NetBufPush(nb1, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb2, E_DATA_BUF, DEFAULT_OP_SIZE + DEFAULT_OP_SIZE);
    NetBufQueueEnqueue(&q, nb1);
    NetBufQueueEnqueue(&q, nb2);

    nb = NetBufQueueAtHead(&q);
    if (NetBufGetDataLen(nb) != DEFAULT_OP_SIZE) {
        HDF_LOGE("NetBufGetLen fail, len:%d", NetBufGetDataLen(nb));
        NetBufQueueClear(&q);
        return HDF_FAILURE;
    }

    NetBufQueueClear(&q);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest005
* @tc.desc: NetBufQueueAtTail test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest005(void)
{
    NetBufQueue q;
    NetBuf *nb = NULL;
    NetBuf *nb1 = NULL;
    NetBuf *nb2 = NULL;

    NetBufQueueInit(&q);

    nb1 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb2 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb1 == NULL || nb2 == NULL) {
        NetBufFree(nb1);
        NetBufFree(nb2);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(nb1, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb2, E_DATA_BUF, DEFAULT_OP_SIZE + DEFAULT_OP_SIZE);
    NetBufQueueEnqueue(&q, nb1);
    NetBufQueueEnqueue(&q, nb2);

    nb = NetBufQueueAtTail(&q);
    if (NetBufGetDataLen(nb) != (DEFAULT_OP_SIZE + DEFAULT_OP_SIZE)) {
        HDF_LOGE("NetBufGetLen fail, len:%d", NetBufGetDataLen(nb));
        NetBufQueueClear(&q);
        return HDF_FAILURE;
    }

    NetBufQueueClear(&q);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest006
* @tc.desc: NetBufQueueDequeue test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest006(void)
{
    NetBufQueue q;
    NetBuf *nb1 = NULL;
    NetBuf *nb2 = NULL;

    NetBufQueueInit(&q);
    if (NetBufQueueIsEmpty(&q) != 1) {
        HDF_LOGE("NetBufQueueIsEmpty fail");
        return HDF_FAILURE;
    }

    nb1 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb2 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb1 == NULL || nb2 == NULL) {
        NetBufFree(nb1);
        NetBufFree(nb2);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(nb1, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb2, E_DATA_BUF, DEFAULT_OP_SIZE + DEFAULT_OP_SIZE);
    NetBufQueueEnqueue(&q, nb1);
    NetBufQueueEnqueue(&q, nb2);

    nb1 = NetBufQueueDequeue(&q);
    nb2 = NetBufQueueDequeue(&q);
    if (nb1 == NULL || nb2 == NULL) {
        HDF_LOGE("NetBufQueueDequeue fail");
        NetBufQueueClear(&q);
        return HDF_FAILURE;
    }

    if (NetBufQueueSize(&q) != 0 || !NetBufQueueIsEmpty(&q)) {
        HDF_LOGE("NetBufQueueSize fail, queue size:%d", NetBufQueueSize(&q));
        NetBufFree(nb1);
        NetBufFree(nb2);
        NetBufQueueClear(&q);
        return HDF_FAILURE;
    }

    if (NetBufGetDataLen(nb1) != DEFAULT_OP_SIZE ||
        NetBufGetDataLen(nb2) != (DEFAULT_OP_SIZE + DEFAULT_OP_SIZE)) {
        HDF_LOGE("NetBufGetLen fail, len:%d", NetBufGetDataLen(nb1));
        NetBufFree(nb1);
        NetBufFree(nb2);
        return HDF_FAILURE;
    }

    NetBufFree(nb1);
    NetBufFree(nb2);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest007
* @tc.desc: NetBufQueueEnqueueHead test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest007(void)
{
    NetBufQueue q;
    NetBuf *nb1 = NULL;
    NetBuf *nb2 = NULL;
    uint32_t size = 0;

    NetBufQueueInit(&q);

    nb1 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb2 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb1 == NULL || nb2 == NULL) {
        NetBufFree(nb1);
        NetBufFree(nb2);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(nb1, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb2, E_DATA_BUF, DEFAULT_OP_SIZE + DEFAULT_OP_SIZE);
    NetBufQueueEnqueueHead(&q, nb1);
    size++;
    NetBufQueueEnqueueHead(&q, nb2);
    size++;
    if (NetBufQueueSize(&q) != size || NetBufQueueIsEmpty(&q)) {
        HDF_LOGE("NetBufQueueSize fail, queue size:%d", NetBufQueueSize(&q));
        NetBufQueueClear(&q);
        return HDF_FAILURE;
    }

    nb1 = NetBufQueueDequeueTail(&q);
    nb2 = NetBufQueueDequeueTail(&q);

    if (NetBufQueueSize(&q) != 0 || !NetBufQueueIsEmpty(&q)) {
        HDF_LOGE("NetBufQueueSize fail, queue size:%d", NetBufQueueSize(&q));
        NetBufQueueClear(&q);
        return HDF_FAILURE;
    }

    if (NetBufGetDataLen(nb1) != DEFAULT_OP_SIZE) {
        HDF_LOGE("NetBufGetLen fail, len:%d", NetBufGetDataLen(nb1));
        NetBufFree(nb1);
        NetBufFree(nb2);
        return HDF_FAILURE;
    }

    if (NetBufGetDataLen(nb2) != (DEFAULT_OP_SIZE + DEFAULT_OP_SIZE)) {
        HDF_LOGE("NetBufGetLen fail, len:%d", NetBufGetDataLen(nb2));
        NetBufFree(nb1);
        NetBufFree(nb2);
        return HDF_FAILURE;
    }

    NetBufFree(nb1);
    NetBufFree(nb2);
    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest008
* @tc.desc: NetBufQueuePurge test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest008(void)
{
    NetBufQueue q;
    NetBuf *nb1 = NULL;
    NetBuf *nb2 = NULL;

    NetBufQueueInit(&q);

    nb1 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb2 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb1 == NULL || nb2 == NULL) {
        NetBufFree(nb1);
        NetBufFree(nb2);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(nb1, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb2, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufQueueEnqueue(&q, nb1);
    NetBufQueueEnqueue(&q, nb2);

    NetBufQueueClear(&q);

    if (NetBufQueueSize(&q) != 0 || !NetBufQueueIsEmpty(&q)) {
        HDF_LOGE("NetBufQueueSize fail, queue size:%d", NetBufQueueSize(&q));
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

/*
* @tc.name: HdfNetBufQueueTest009
* @tc.desc: NetBufQueueSpliceInit test
* @tc.type: FUNC
* @tc.require: AR000DT1UD
*/
int32_t HdfNetBufQueueTest009(void)
{
    NetBufQueue q;
    NetBufQueue add;
    NetBuf *nb1 = NULL;
    NetBuf *nb2 = NULL;
    NetBuf *nb = NULL;
    uint32_t size = 0;

    NetBufQueueInit(&q);
    NetBufQueueInit(&add);

    nb1 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb2 = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    nb = NetBufAlloc(DEFAULT_NETBUF_SIZE);
    if (nb1 == NULL || nb2 == NULL || nb == NULL) {
        NetBufFree(nb1);
        NetBufFree(nb2);
        NetBufFree(nb);
        HDF_LOGE("NetBufAlloc fail");
        return HDF_FAILURE;
    }

    NetBufPush(nb1, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb2, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufPush(nb, E_DATA_BUF, DEFAULT_OP_SIZE);
    NetBufQueueEnqueue(&q, nb1);
    size++;
    NetBufQueueEnqueue(&q, nb2);
    size++;
    NetBufQueueEnqueue(&add, nb);

    NetBufQueueConcat(&q, &add);
    size++;
    if (NetBufQueueSize(&q) != size) {
        NetBufQueueClear(&q);
        HDF_LOGE("NetBufQueueSize fail, queue size:%d", NetBufQueueSize(&q));
        return HDF_FAILURE;
    }

    NetBufQueueClear(&q);
    return HDF_SUCCESS;
}

// add test case
pTestCaseFunc g_hdfNetBufTestCaseLists[] = {
    HdfNetBufTest001,
    HdfNetBufTest002,
    HdfNetBufTest003,
    HdfNetBufTest004,
    HdfNetBufTest005,
    HdfNetBufTest006,
    HdfNetBufTest007,
    HdfNetBufTest008,
    HdfNetBufTest009,
    HdfNetBufTest010,
    HdfNetBufTest011,
#ifdef __LITEOS__
    HdfNetBufTest012,
    HdfNetBufTest013,
#endif
};

// add test case
pTestCaseFunc g_hdfNetBufQueueTestCaseLists[] = {
    HdfNetBufQueueTest001,
    HdfNetBufQueueTest002,
    HdfNetBufQueueTest003,
    HdfNetBufQueueTest004,
    HdfNetBufQueueTest005,
    HdfNetBufQueueTest006,
    HdfNetBufQueueTest007,
    HdfNetBufQueueTest008,
    HdfNetBufQueueTest009,
};

// HDFNetBuf test case Entry
int32_t HdfNetBufTest(void)
{
    int32_t ret, i;

    for (i = 0; i < sizeof(g_hdfNetBufTestCaseLists) / sizeof(g_hdfNetBufTestCaseLists[0]); ++i) {
        if (g_hdfNetBufTestCaseLists[i] != NULL) {
            ret = g_hdfNetBufTestCaseLists[i]();
            HDF_LOGI("HdfTest:HdfNetBuf test_case[%d] result[%s-%d]",
                i + 1, (ret != HDF_SUCCESS) ? "failed" : "pass", ret);
            if (ret != HDF_SUCCESS) {
                return HDF_FAILURE;
            }
        }
    }

    return HDF_SUCCESS;
}

// HdfNetBufQueue test case Entry
int32_t HdfNetBufQueueTest(void)
{
    int32_t ret, i;

    for (i = 0; i < sizeof(g_hdfNetBufQueueTestCaseLists) / sizeof(g_hdfNetBufQueueTestCaseLists[0]); ++i) {
        if (g_hdfNetBufQueueTestCaseLists[i] != NULL) {
            ret = g_hdfNetBufQueueTestCaseLists[i]();
            HDF_LOGI("HdfTest:HdfNetBufQueue test_case[%d] result[%s-%d]",
                i + 1, (ret != HDF_SUCCESS) ? "failed" : "pass", ret);
            if (ret != HDF_SUCCESS) {
                return HDF_FAILURE;
            }
        }
    }

    return HDF_SUCCESS;
}
