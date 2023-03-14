/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "liteipc_adapter.h"
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "liteipc.h"
#include "securec.h"
#include "sys_binder.h"
#include "utils_list.h"

#define ALIGN_SZ 4
#define IPC_IO_ALIGN(sz) (((sz) + ALIGN_SZ - 1) & (~(ALIGN_SZ - 1)))
#define SET_MAX_THREADS 4
#define BINDER_DRIVER "/dev/binder"
#define IPC_IO_DATA_TEMP 200
#define READ_BUFFER_SIZE 32

#define NS_PER_MS  1000000ULL
#define NS_PER_SEC 1000000000ULL
static UTILS_DL_LIST *g_tempHandles = NULL;
static UTILS_DL_LIST *g_funcPairs = NULL;
static pthread_mutex_t g_funcPairsMutex = PTHREAD_MUTEX_INITIALIZER;
static uint32_t g_samgrId = 0;
static uint32_t g_handle = 0;
static bool g_isStartLoop = false;
__thread const IpcContext *g_tlsContext = NULL;
__thread IpcIo **g_tlsReply = NULL;
__thread uint8_t g_tlsReplyData[IPC_IO_DATA_MAX] = {0};
bool g_init = 0;
struct TransactData {
    uint32_t cmd;
    struct binder_transaction_data btd;
}__attribute__((packed));

struct FreeData {
    uint32_t cmd;
    binder_uintptr_t buffer;
}__attribute__((packed));

static void RemoveDeathCallback(uint32_t handle);
typedef struct {
    pthread_mutex_t mutex;
    int32_t handleId;
    bool threadWorking;
    UTILS_DL_LIST apis;
    UTILS_DL_LIST testaments;
} IpcCallbackCb;

IpcCallbackCb g_ipcCallbackCb = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .handleId = -1,
    .threadWorking = false
};

typedef struct {
    bool usedFlag;
    IpcMsgHandler hdler;
    void* arg;
} IpcMsgHandlerPair;

typedef struct {
    UTILS_DL_LIST list;
    uint32_t handle;
    IpcMsgHandlerPair hdlerPairs[MAX_DEATHCB_PER_SVC];
    size_t size;
    bool isCallbackRuning;
} Testament;

typedef struct {
    UTILS_DL_LIST list;
    uint32_t token;
    IpcMsgHandlerPair hdlerPair;
} AnonymousApi;

typedef struct {
    uint32_t num;
    void* msg;
    IpcIo io;
    IpcMsgHandlerPair* cbs;
    const IpcContext* context;
} HdlerArg;

struct BinderDeath {
    IpcMsgHandler func;
    uint32_t token;
};

struct FuncPair {
    UTILS_DL_LIST list;
    pthread_t tid;
    IpcMsgHandler func;
    void* argv;
};

struct BinderThreadDesc {
    const IpcContext* context;
    IpcMsgHandler func;
    void* argv;
};

#define LOG(level, format, ...)                      \
    do {                                             \
        printf("[%s : %d]", __FUNCTION__, __LINE__); \
        printf(format, ##__VA_ARGS__);               \
        printf("\n");                                \
    } while (0)

#define RETURN_IF_IPC_IOCTL_FAILED(retVal)              \
    do {                                                \
        if (ret < 0) {                                  \
            LOG(ERROR, "Liteipc driver ioctl failed."); \
            return (retVal);                            \
        }                                               \
    } while (0)

static inline void InitCallbackCb()
{
    UtilsListInit(&g_ipcCallbackCb.apis);
    UtilsListInit(&g_ipcCallbackCb.testaments);
    g_init = 1;
}

static int32_t StartCallbackDispatch(const IpcContext *context);

static void SaveFuncPair(const struct FuncPair* func)
{
    if (g_funcPairs == NULL) {
        return;
    }
    struct FuncPair* node = NULL;
    struct FuncPair* next = NULL;
    if (pthread_mutex_lock(&g_funcPairsMutex) != 0) {
        LOG(ERROR, "Get save fun pair mutex failed.");
        return;
    }
    uint32_t curTid = (uint32_t)pthread_self();
    UTILS_DL_LIST_FOR_EACH_ENTRY_SAFE(node, next, g_funcPairs, struct FuncPair, list)
    {
        if (node->tid != curTid) {
            continue;
        }
        node->func = func->func;
        node->argv = func->argv;
        (void)pthread_mutex_unlock(&g_funcPairsMutex);
        return;
    }

    node = calloc(1, sizeof(struct FuncPair));
    if (node == NULL) {
        (void)pthread_mutex_unlock(&g_funcPairsMutex);
        return;
    }
    node->func = func->func;
    node->argv = func->argv;
    node->tid = curTid;
    UtilsListAdd(g_funcPairs, &node->list);
    (void)pthread_mutex_unlock(&g_funcPairsMutex);
}

static bool GetFuncPair(struct FuncPair* func, uint32_t target)
{
    if (g_funcPairs == NULL) {
        return false;
    }
    struct FuncPair* node = NULL;
    struct FuncPair* next = NULL;
    if (pthread_mutex_lock(&g_funcPairsMutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return false;
    }
    UTILS_DL_LIST_FOR_EACH_ENTRY_SAFE(node, next, g_funcPairs, struct FuncPair, list)
    {
        if (node->tid != target) {
            continue;
        }
        func->func = node->func;
        func->argv = node->argv;
        (void)pthread_mutex_unlock(&g_funcPairsMutex);
        return true;
    }
    (void)pthread_mutex_unlock(&g_funcPairsMutex);
    return false;
}

static int32_t CheckIpcIo(IpcIo* data)
{
    uint32_t totalSize;
    /* It's OK if data is null */
    if (data == NULL) {
        return LITEIPC_OK;
    }
    if ((IpcIoAvailable(data) == false) ||
        (data->bufferCur == NULL) ||
        (data->bufferBase == NULL) ||
        (data->offsetsCur == NULL) ||
        (data->offsetsBase == NULL)) {
        return LITEIPC_EINVAL;
    }
    totalSize = data->bufferCur- data->bufferBase + ((char*)data->offsetsCur - (char*)data->offsetsBase);
    if (totalSize > IPC_IO_DATA_MAX) {
        LOG(ERROR, "IpcIo data too big, please use IpcIoPushDataBuff to push big data.");
        return LITEIPC_EINVAL;
    }
    return LITEIPC_OK;
}

static int BinderSetMaxthreads(const IpcContext* context, int threads)
{
    if (context == NULL) {
        return LITEIPC_EINTNL;
    }
    int ret = ioctl(context->fd, BINDER_SET_MAX_THREADS, &threads);
    return ret;
}

IpcContext* OpenLiteIpc(size_t mmapSize)
{
    if ((mmapSize == 0) || (mmapSize > MMAP_MAX_SIZE)) {
        LOG(ERROR, "MmapSize not available");
        return NULL;
    }
    IpcContext* context = (IpcContext *)malloc(sizeof(IpcContext));
    if (context == NULL) {
        return NULL;
    }
    context->fd = open(BINDER_DRIVER, O_RDONLY);
    if (context->fd < 0) {
        LOG(ERROR, "Open liteipc driver failed error %s.", strerror(errno));
        goto OPEN_ERR;
    }

    context->mmapAddr = mmap(NULL, mmapSize, PROT_READ, MAP_PRIVATE, context->fd, 0);
    if (context->mmapAddr == MAP_FAILED) {
        LOG(ERROR, "Mmap failed.(size=%zu)", mmapSize);
        goto MMAP_ERR;
    }

    context->mmapSize = mmapSize;
    InitCallbackCb();
    if (BinderSetMaxthreads(context, SET_MAX_THREADS) != LITEIPC_OK) {
        LOG(ERROR, "set max threads failed");
        goto MALLOC_ERR;
    }
    return context;

MALLOC_ERR:
    munmap(context->mmapAddr, mmapSize);
MMAP_ERR:
    close(context->fd);
OPEN_ERR:
    free(context);
    return NULL;
}

void ResetLiteIpc()
{
    g_tempHandles = NULL;
    g_funcPairs = NULL;
    pthread_mutex_init(&(g_funcPairsMutex), NULL);
    g_samgrId = 0;
    g_handle = 0;
    g_isStartLoop = false;
    g_tlsContext = NULL;
    g_tlsReply = NULL;
    pthread_mutex_init(&(g_ipcCallbackCb.mutex), NULL);
    g_ipcCallbackCb.handleId = -1;
    g_ipcCallbackCb.threadWorking = false;
    InitCallbackCb();
}

void CloseLiteIpc(IpcContext* context)
{
    if (context == NULL) {
        return;
    }
    munmap(context->mmapAddr, context->mmapSize);
    close(context->fd);
    free(context);
}

int32_t SetSaManager(const IpcContext* context, size_t maxMsgSize)
{
    if (context == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINTNL;
    }
    int ret = ioctl(context->fd, BINDER_SET_CONTEXT_MGR, 0);
    if (ret == LITEIPC_OK) {
        g_samgrId = (uint32_t)pthread_self();
        g_tempHandles = calloc(1, sizeof(UTILS_DL_LIST));
        if (g_tempHandles == NULL) {
            LOG(ERROR, "calloc failed.");
            return LITEIPC_EINTNL;
        }
        UtilsListInit(g_tempHandles);
    }
    return ret;
}

int32_t AddServiceAccess(SvcIdentity sid, pid_t pid)
{
    (void)sid;
    (void)pid;
    return LITEIPC_OK;
}

int32_t GetCode(const void* ipcMsg, uint32_t* code)
{
    if ((ipcMsg == NULL) || (code == NULL)) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    const IpcMsg* in = (const IpcMsg*)ipcMsg;
    *code = in->code;
    return LITEIPC_OK;
}

pid_t GetCallingTid(const void* ipcMsg)
{
    if (ipcMsg == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    const IpcMsg* in = (const IpcMsg*)ipcMsg;
    return (pid_t)in->taskID;
}

static int BinderWrite(const IpcContext *context, void *data, size_t len)
{
    struct binder_write_read bwr = {0};
    int res;

    bwr.write_size = len;
    bwr.write_consumed = 0;
    bwr.write_buffer = (uintptr_t)data;
    bwr.read_size = 0;
    bwr.read_consumed = 0;
    bwr.read_buffer = 0;
    res = ioctl(context->fd, BINDER_WRITE_READ, &bwr);
    if (res < 0) {
        LOG(ERROR, "ioctl failed ret = %d", res);
    }
    return res;
}

static void* BinderThreadRoutine(void *argv);

static void TxnToIpc(const struct binder_transaction_data *txn, IpcMsg* data)
{
    data->target.handle = txn->target.handle;
    data->target.token = txn->target.handle;
    data->code = txn->code;
    data->flag = txn->flags;
    data->processID = txn->sender_pid;
    data->userID = txn->sender_euid;
    data->dataSz = txn->data_size;
    data->data = (void *)txn->data.ptr.buffer;
    data->freeData = (void *)txn->data.ptr.buffer;
    data->offsets = (void *)txn->data.ptr.offsets;
    data->spObjNum = txn->offsets_size / sizeof(size_t);
}

static void TryCallBack(const IpcContext* context, void* msg, IpcIo* data, void* arg);
static void GetIpcCallback(IpcMsg* msg, HdlerArg* arg);

static bool IsValidCB(IpcMsg* ipcMsg)
{
    HdlerArg tArg = {0};
    GetIpcCallback(ipcMsg, &tArg);
    if (tArg.num == 0) {
        return false;
    }
    return true;
}

static void HandleSpawnLooper(const IpcContext *context, const struct FuncPair *funcPair)
{
    pthread_t thread;
    struct BinderThreadDesc *btd = (struct BinderThreadDesc *)malloc(sizeof(struct BinderThreadDesc));
    if (btd == NULL) {
        return;
    }
    (void)memset_s(btd, sizeof(struct BinderThreadDesc), 0, sizeof(struct BinderThreadDesc));
    
    btd->context = context;
    btd->func = funcPair->func;
    btd->argv = funcPair->argv;
    int ret = pthread_create(&thread, NULL, BinderThreadRoutine, btd);
    if (ret != 0) {
        free(btd);
        btd = NULL;
    }
}

static void HandleTransaction(const IpcContext *context, const struct binder_transaction_data *txn)
{
    IpcMsg *data = (IpcMsg*)calloc(1, sizeof(IpcMsg));
    if (data == NULL) {
        LOG(ERROR, "HandleTransaction calloc failed!");
        return;
    }
    data->type = MT_REQUEST;
    TxnToIpc(txn, data);
    IpcIo io = {0};
    IpcIoInitFromMsg(&io, data);
    uint32_t token = IpcIoPopUint32(&io);
    SvcIdentity *remote = IpcIoPopSvc(&io);
    if (remote == NULL) {
        LOG(ERROR, "HandleTransaction remote NULL!");
        return;
    }
    g_handle = remote->handle;
    data->data = io.bufferCur;
    data->offsets = io.offsetsCur;
    data->dataSz = io.bufferLeft;
    data->spObjNum = io.offsetsLeft;
    if (IsValidCB(data)) {
        TryCallBack(context, data, &io, NULL);
        free(remote);
        return;
    }
    data->target.token = token;
    struct FuncPair pair;
    if (GetFuncPair(&pair, txn->cookie)) {
        pair.func(context, data, &io, pair.argv);
    } else if (GetFuncPair(&pair, g_samgrId)) {
        data->target.handle = remote->handle;
        pair.func(context, data, &io, pair.argv);
    } else {
        LOG(ERROR, "BR_TRANSACTION should not happen!");
    }
    free(remote);
}

static void HandleReply(IpcIo* bio, const struct binder_transaction_data *txn, uintptr_t* buffer)
{
    if (buffer == NULL) {
        return;
    }
    if (bio == NULL) {
        *buffer = 0;
        return;
    }
    IpcMsg *data = (IpcMsg*)calloc(1, sizeof(IpcMsg));
    if (data == NULL) {
        LOG(ERROR, "HandleReply calloc failed!");
        *buffer = 0;
        return;
    }

    TxnToIpc(txn, data);
    IpcIoInitFromMsg(bio, data);
    IpcIoPopUint32(bio);
    SvcIdentity *sid = IpcIoPopSvc(bio);
    if (sid != NULL) {
        free(sid);
        sid = NULL;
    }
    if (buffer != NULL) {
        *buffer = (uintptr_t)data;
    }
}

static void HandleDeadBinder(const IpcContext *context, uintptr_t ptr)
{
    struct BinderDeath *death = (struct BinderDeath *)(uintptr_t) *(binder_uintptr_t *)ptr;
    IpcMsg data = {0};
    data.type = MT_DEATH_NOTIFY;
    data.target.token = death->token;
    data.target.handle = death->token;
    death->func(context, &data, NULL, NULL);
}

static void BinderRefDone(const IpcContext *context, const struct binder_ptr_cookie *ptrCookie,
                          enum binder_driver_command_protocol cmd)
{
    struct {
        uint32_t cmd;
        struct binder_ptr_cookie payload;
    } __attribute__((packed)) data;

    data.cmd = cmd;
    data.payload.ptr = ptrCookie->ptr;
    data.payload.cookie = ptrCookie->cookie;
    BinderWrite(context, &data, sizeof(data));
}

static int BinderParse(const IpcContext *context, void *bio, uintptr_t ptr,
                       size_t size, struct FuncPair *funcPair, uintptr_t* buffer)
{
    int ret = 1;
    g_tlsContext = context;
    uintptr_t end = ptr + (uintptr_t) size;
    while (ptr < end) {
        uint32_t cmd = *(uint32_t *) ptr;
        ptr += sizeof(uint32_t);
        switch (cmd) {
            case BR_NOOP:
                break;
            case BR_TRANSACTION_COMPLETE:
                break;
            case BR_ACQUIRE: {
                struct binder_ptr_cookie *ptrCookie = (struct binder_ptr_cookie *) ptr;
                if ((end - ptr) < sizeof(*ptrCookie)) {
                    return -1;
                }
                BinderRefDone(context, ptrCookie, BC_ACQUIRE_DONE);
                ptr += sizeof(*ptrCookie);
                break;
            }
            case BR_INCREFS: {
                struct binder_ptr_cookie *ptrCookie = (struct binder_ptr_cookie *) ptr;
                if ((end - ptr) < sizeof(*ptrCookie)) {
                    return -1;
                }
                BinderRefDone(context, ptrCookie, BC_INCREFS_DONE);
                ptr += sizeof(*ptrCookie);
                break;
            }
            case BR_RELEASE:
            case BR_DECREFS:
                ptr += sizeof(struct binder_ptr_cookie);
                break;
            case BR_SPAWN_LOOPER: {
                HandleSpawnLooper(context, funcPair);
                break;
            }
            case BR_TRANSACTION: {
                struct binder_transaction_data *txn = (struct binder_transaction_data *) ptr;
                if ((end - ptr) < sizeof(*txn)) {
                    return -1;
                }
                HandleTransaction(context, txn);
                ptr += sizeof(*txn);
                break;
            }
            case BR_REPLY: {
                struct binder_transaction_data *txn = (struct binder_transaction_data *) ptr;
                if ((end - ptr) < sizeof(*txn)) {
                    return -1;
                }
                HandleReply(bio, txn, buffer);
                ptr += sizeof(*txn);
                ret = 0;
                break;
            }
            case BR_DEAD_BINDER: {
                HandleDeadBinder(context, ptr);
                ptr += sizeof(binder_uintptr_t);
                break;
            }
            case BR_FAILED_REPLY:
                ret = -1;
                break;
            case BR_DEAD_REPLY:
                ret = LITEIPC_DEAD_OBJECT;
                break;
            default:
                ret = -1;
        }
    }
    return ret;
}

int32_t StartLoop(const IpcContext* context, IpcMsgHandler func, void* arg)
{
    if (context == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }
    if (g_funcPairs == NULL) {
        g_funcPairs = calloc(1, sizeof(UTILS_DL_LIST));
        if (g_funcPairs == NULL) {
            LOG(ERROR, "Invalid parameter, null pointer.");
            return LITEIPC_EINVAL;
        }
        UtilsListInit(g_funcPairs);
    }
    struct binder_write_read bwr = {0};
    uint32_t readbuf[READ_BUFFER_SIZE] = {0};
    struct FuncPair funcPair = {0};
    funcPair.func = func;
    funcPair.argv = arg;
    SaveFuncPair(&funcPair);

    readbuf[0] = BC_ENTER_LOOPER;
    BinderWrite(context, readbuf, sizeof(uint32_t));
    for (;;) {
        bwr.read_size = sizeof(readbuf);
        bwr.read_consumed = 0;
        bwr.read_buffer = (uintptr_t) readbuf;
        int ret = ioctl(context->fd, BINDER_WRITE_READ, &bwr);

        if (ret < 0) {
            LOG(ERROR, "ioctl failed ret = %d", ret);
            break;
        }
        ret = BinderParse(context, 0, (uintptr_t)readbuf, bwr.read_consumed, &funcPair, NULL);
        if (ret == 0) {
            LOG(ERROR, "reply failed!");
            break;
        }
        if (ret < 0) {
            LOG(ERROR, "io error %d", ret);
            break;
        }
    }

    return LITEIPC_OK;
}

static void BinderThreadLoop(const IpcContext* context, IpcMsgHandler func, void* arg)
{
    if (context == NULL || arg == NULL) {
        return;
    }
    struct binder_write_read bwr = {0};
    uint32_t readbuf[READ_BUFFER_SIZE] = {0};
    struct FuncPair funcPair = {0};
    funcPair.func = func;
    funcPair.argv = arg;

    readbuf[0] = BC_REGISTER_LOOPER;

    BinderWrite(context, readbuf, sizeof(uint32_t));
    for (;;) {
        bwr.read_size = sizeof(readbuf);
        bwr.read_consumed = 0;
        bwr.read_buffer = (uintptr_t) readbuf;
        int ret = ioctl(context->fd, BINDER_WRITE_READ, &bwr);

        if (ret < 0) {
            LOG(ERROR, "ioctl failed ret = %d", ret);
            break;
        }
        ret = BinderParse(context, 0, (uintptr_t)readbuf, bwr.read_consumed, &funcPair, NULL);
        if (ret == 0) {
            LOG(ERROR, "reply failed");
            break;
        }
        if (ret < 0) {
            LOG(ERROR, "io error ret = %d", ret);
            break;
        }
    }
}

static void* BinderThreadRoutine(void *argv)
{
    struct BinderThreadDesc *btd = (struct BinderThreadDesc *)argv;
    BinderThreadLoop(btd->context, btd->func, btd->argv);
    free(argv);
    return NULL;
}

static void ToTransData(SvcIdentity sid, uint32_t code, IpcIo* data, IpcFlag flag, struct TransactData *buf)
{
    buf->btd.target.ptr = 0;
    buf->btd.target.handle = sid.handle;
    buf->btd.code = code;
    buf->btd.flags = flag;
    buf->btd.cookie = 0;
    buf->btd.sender_pid = getpid();
    buf->btd.sender_euid = getuid();
    if ((data != NULL) && (data->bufferBase != NULL)) {
        (data->bufferBase)[0] = sid.token;
    }
    buf->btd.data_size = (data == NULL) ? 0 : (data->bufferCur - data->bufferBase);
    buf->btd.data.ptr.buffer = (data == NULL) ? 0 : (binder_uintptr_t)data->bufferBase;
    buf->btd.offsets_size = (data == NULL) ? 0 : ((char*)data->offsetsCur - (char*)data->offsetsBase);
    buf->btd.offsets_size = IPC_IO_ALIGN(buf->btd.offsets_size);
    buf->btd.data.ptr.offsets = (data == NULL) ? 0 : (binder_uintptr_t)data->offsetsBase;
}

static IpcContext* GetIpcContext(const IpcContext* context, const IpcContext* tlsContext)
{
    const IpcContext *tmpContext = NULL;
    if (tlsContext != NULL) {
        tmpContext = tlsContext;
    } else if (context != NULL) {
        tmpContext = context;
    }
    return (IpcContext*)tmpContext;
}

static int32_t HandleInternelRequest(const IpcContext *context, SvcIdentity sid, uint32_t code,
                                     IpcIo* data, IpcIo* reply, IpcFlag flag)
{
    AnonymousApi* node = (AnonymousApi*)sid.token;
    if (node == NULL) {
        LOG(ERROR, "Invalid parameter, node null pointer.");
        return LITEIPC_EINVAL;
    }
    IpcMsg msg = {
        .type = MT_REQUEST,
        .target = sid,
        .code = code,
        .flag = flag,
        .dataSz = (data == NULL) ? 0 : (data->bufferCur - data->bufferBase),
        .data = (data == NULL) ? NULL : data->bufferBase,
        .offsets = (data == NULL) ? NULL : data->offsetsBase,
        .spObjNum = (data == NULL) ? 0 : ((char*)data->offsetsCur - (char*)data->offsetsBase) / sizeof(size_t)
    };
    IpcIo tmp;
    IpcIoInitFromMsg(&tmp, &msg);
    IpcIoPopUint32(&tmp);
    SvcIdentity *sidTmp = IpcIoPopSvc(&tmp);
    if (sidTmp != NULL) {
        free(sidTmp);
        sidTmp = NULL;
    }
    msg.data = tmp.bufferCur;
    msg.offsets = tmp.offsetsCur;
    msg.dataSz = tmp.bufferLeft;
    msg.spObjNum = tmp.offsetsLeft;
    if (reply != NULL) {
        g_tlsReply = &reply;
    }
    node->hdlerPair.hdler(context, &msg, &tmp, node->hdlerPair.arg);
    return LITEIPC_OK;
}

int32_t SendRequest(const IpcContext *context, SvcIdentity sid, uint32_t code,
                    IpcIo* data, IpcIo* reply, IpcFlag flag, uintptr_t* buffer)
{
    const IpcContext *tmpContext = GetIpcContext(context, sid.ipcContext);
    if (tmpContext == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }
    if ((flag > LITEIPC_FLAG_ONEWAY) || ((flag == LITEIPC_FLAG_DEFAULT) && (buffer == NULL))) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    if (sid.handle == MIN_BINDER_HANDLE) {
        return HandleInternelRequest(tmpContext, sid, code, data, reply, flag);
    }
    int ret;
    if (!g_isStartLoop) {
        ret = StartCallbackDispatch(tmpContext);
        if (ret != LITEIPC_OK) {
            LOG(ERROR, "StartCallbackDispatch failed.");
        }
    }
    struct TransactData buf;
    struct binder_write_read bwr;
    ret = CheckIpcIo(data);
    if (ret != LITEIPC_OK) {
        LOG(ERROR, "CheckIpcIo failed.");
        return ret;
    }
    buf.cmd = BC_TRANSACTION;
    IpcIo tempIo;
    if (data == NULL) {
        data = &tempIo;
        uint8_t tmpData[IPC_IO_DATA_TEMP];
        IpcIoInit(data, tmpData, IPC_IO_DATA_TEMP, 0);
    }
    ToTransData(sid, code, data, flag, &buf);
    bwr.write_size = sizeof(buf);
    bwr.write_consumed = 0;
    bwr.write_buffer = (uintptr_t)&buf;
    uint32_t readbuf[READ_BUFFER_SIZE] = {0};
    if (flag != LITEIPC_FLAG_ONEWAY) {
        while (1) {
            bwr.read_size = sizeof(readbuf);
            bwr.read_consumed = 0;
            bwr.read_buffer = (uintptr_t)readbuf;
            ioctl(tmpContext->fd, BINDER_WRITE_READ, &bwr);
            ret = BinderParse(tmpContext, reply, (uintptr_t)readbuf, bwr.read_consumed, NULL, buffer);
            if (ret == 0) {
                break;
            }
        }
    } else {
        bwr.read_size = sizeof(readbuf);
        bwr.read_consumed = 0;
        bwr.read_buffer = (uintptr_t)readbuf;
        ioctl(tmpContext->fd, BINDER_WRITE_READ, &bwr);
        ret = BinderParse(tmpContext, reply, (uintptr_t)readbuf, bwr.read_consumed, NULL, NULL);
        if (ret == 1) {
            ret = 0;
        }
    }
    return ret;
}

int32_t SendReply(const IpcContext* context, void* ipcMsg, IpcIo* reply)
{
    const IpcContext *tmpContext = NULL;
    if (context != NULL) {
        tmpContext = context;
    } else if (g_tlsContext != NULL) {
        tmpContext = g_tlsContext;
    } else {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }
    (void)FreeBuffer(tmpContext, ipcMsg);

    int32_t ret = CheckIpcIo(reply);
    if (ret != LITEIPC_OK) {
        LOG(ERROR, "CheckIpcIo failed.");
        return ret;
    }

    if (g_tlsReply != NULL && *g_tlsReply != NULL) {
        IpcIo tmp;
        IpcMsg msg = {
            .dataSz = (reply == NULL) ? 0 : (reply->bufferCur - reply->bufferBase),
            .data = (reply == NULL) ? NULL : (void*)reply->bufferBase,
            .offsets = (reply == NULL) ? NULL : reply->offsetsBase,
            .spObjNum = (reply == NULL) ? 0 : ((char*)reply->offsetsCur - (char*)reply->offsetsBase) / sizeof(size_t)
        };
        IpcIoInitFromMsg(&tmp, &msg);

        size_t objectsSize = tmp.offsetsLeft * sizeof(size_t);
        (*g_tlsReply)->bufferCur = (*g_tlsReply)->bufferBase = (char*)g_tlsReplyData + objectsSize;
        (*g_tlsReply)->offsetsCur = (*g_tlsReply)->offsetsBase = (size_t*)g_tlsReplyData;
        if (memcpy_s((*g_tlsReply)->bufferBase, IPC_IO_DATA_MAX, tmp.bufferBase, tmp.bufferLeft) != EOK) {
            return LITEIPC_EINTNL;
        }
        if (memcpy_s((*g_tlsReply)->offsetsBase, objectsSize, tmp.offsetsBase, objectsSize) != EOK) {
            return LITEIPC_EINTNL;
        }
        (*g_tlsReply)->bufferLeft = tmp.bufferLeft;
        (*g_tlsReply)->offsetsLeft = tmp.offsetsLeft;
        (*g_tlsReply)->flag = (reply == NULL) ? 1 : reply->flag;

        IpcIoPopUint32(*g_tlsReply);
        SvcIdentity *sid = IpcIoPopSvc(*g_tlsReply);
        if (sid != NULL) {
            free(sid);
            sid = NULL;
        }
        g_tlsReply = NULL;
        return LITEIPC_OK;
    }

    struct TransactData buf = {0};
    buf.cmd = BC_REPLY;
    SvcIdentity sid = {0};
    IpcIo tempIo;
    if (reply == NULL) {
        reply = &tempIo;
        uint8_t tmpData[IPC_IO_DATA_TEMP];
        IpcIoInit(reply, tmpData, IPC_IO_DATA_TEMP, 0);
    }
    ToTransData(sid, 0, reply, 0, &buf);
    ret = BinderWrite(tmpContext, &buf, sizeof(buf));
    return ret;
}

static void CallbackBatchHandler(void* arg)
{
    HdlerArg* hdlerArg = (HdlerArg*)arg;
    IpcMsg* msg = hdlerArg->msg;
    uint32_t i;

    for (i = 0; i < hdlerArg->num; i++) {
        if (hdlerArg->cbs[i].usedFlag == true) {
            (hdlerArg->cbs[i].hdler)(hdlerArg->context, hdlerArg->msg, &hdlerArg->io, hdlerArg->cbs[i].arg);
        }
    }
    if (msg->type == MT_DEATH_NOTIFY) {
        RemoveDeathCallback(msg->target.token);
        FreeBuffer(NULL, msg);
    }
    free(arg);
    return;
}

static void GetIpcCallback(IpcMsg* msg, HdlerArg* arg)
{
    if (pthread_mutex_lock(&g_ipcCallbackCb.mutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return;
    }
    IpcIoInitFromMsg(&arg->io, msg);
    arg->msg = msg;

    AnonymousApi* node = NULL;
    UTILS_DL_LIST_FOR_EACH_ENTRY(node, &g_ipcCallbackCb.apis, AnonymousApi, list)
    {
        if (node->token == msg->target.token) {
            arg->num = 1;
            arg->cbs = &node->hdlerPair;
            (void)pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
            return;
        }
    }
    arg->num = 0;
    arg->cbs = NULL;
    (void)pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
}

static void GetDeathCallback(IpcMsg* msg, HdlerArg* arg)
{
    if (pthread_mutex_lock(&g_ipcCallbackCb.mutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return;
    }
    IpcIoInitFromMsg(&arg->io, msg);
    arg->msg = msg;

    Testament* node = NULL;
    UTILS_DL_LIST_FOR_EACH_ENTRY(node, &g_ipcCallbackCb.testaments, Testament, list)
    {
        if (node->handle == msg->target.token) {
            node->isCallbackRuning = true;
            arg->num = MAX_DEATHCB_PER_SVC;
            arg->cbs = node->hdlerPairs;
            (void)pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
            return;
        }
    }
    arg->num = 0;
    arg->cbs = NULL;
    (void)pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
}

static void TryCallBack(const IpcContext* context, void* msg, IpcIo* data, void* arg)
{
    IpcMsg* ipcMsg = (IpcMsg *)msg;
    HdlerArg* tArg = (HdlerArg*)malloc(sizeof(HdlerArg));
    if (tArg == NULL) {
        goto ERROR_MALLOC;
    }

    switch (ipcMsg->type) {
        case MT_DEATH_NOTIFY:
            GetDeathCallback(ipcMsg, tArg);
            break;
        case MT_REQUEST:
            GetIpcCallback(ipcMsg, tArg);
            break;
        default:
            LOG(ERROR, "Callback thread received an unrecognized message.(type=%d)", ipcMsg->type);
            goto ERROR_MSG;
    }

    if (tArg->num == 0) {
        goto ERROR_MSG;
    }
    tArg->context = context;
    CallbackBatchHandler(tArg);
    return;
ERROR_MSG:
    free(tArg);
ERROR_MALLOC:
    if ((ipcMsg->type == MT_REQUEST) && (ipcMsg->flag == LITEIPC_FLAG_DEFAULT)) {
        SendReply(context, ipcMsg, data);
    } else {
        (void)FreeBuffer(context, ipcMsg);
    }
}

static void* CallbackDispatch(void* arg)
{
    IpcContext *ipcContext = (IpcContext*)arg;
    if (ipcContext == NULL) {
        return (void*)(intptr_t)LITEIPC_EINTNL;
    }
    g_ipcCallbackCb.threadWorking = true;
    if (StartLoop(ipcContext, (IpcMsgHandler)TryCallBack, NULL) != LITEIPC_OK) {
        return (void*)(intptr_t)LITEIPC_EINTNL;
    }
    return (void*)(intptr_t)LITEIPC_OK;
}

static int32_t StartCallbackDispatch(const IpcContext *context)
{
    g_isStartLoop = true;
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    pthread_t tid;
    int32_t ret = pthread_create(&tid, &threadAttr, CallbackDispatch, (void *)context);
    if (ret != 0) {
        LOG(ERROR, "Create callback dispatch thread failed.");
        g_isStartLoop = false;
        return LITEIPC_EINTNL;
    }
    struct timespec spark, now;
    clock_gettime(CLOCK_REALTIME, &spark);
    sched_yield();
    while (!g_ipcCallbackCb.threadWorking) {
        clock_gettime(CLOCK_REALTIME, &now);
        if (now.tv_sec - spark.tv_sec > 1) {
            LOG(ERROR, "Wait callback thread starting timeout.");
            return LITEIPC_EINTNL;
        }
        sched_yield();
    }
    return LITEIPC_OK;
}

int32_t RegisterIpcCallback(IpcMsgHandler func, uint32_t mode, uint32_t timeoutMs, SvcIdentity* sid, void* arg)
{
    if (g_init == 0) {
        InitCallbackCb();
    }
    if ((func == NULL) || (sid == NULL) || (mode > RESET_AFTER_USE) || (timeoutMs < IPC_TIMEOUT_MIN)) {
        return LITEIPC_EINVAL;
    }

    if (pthread_mutex_lock(&g_ipcCallbackCb.mutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return LITEIPC_EINTNL;
    }

    int ret = LITEIPC_OK;
    AnonymousApi* node = (AnonymousApi*)malloc(sizeof(AnonymousApi));
    if (node == NULL) {
        LOG(ERROR, "Malloc failed.(size=%zu)", sizeof(AnonymousApi));
        ret = LITEIPC_EINTNL;
        goto ERROR;
    }
    (void)memset_s(node, sizeof(AnonymousApi), 0, sizeof(AnonymousApi));

    node->token = (uint32_t)(uintptr_t)node;
    node->hdlerPair.usedFlag = true;
    node->hdlerPair.hdler = func;
    node->hdlerPair.arg = arg;
    UtilsListAdd(&g_ipcCallbackCb.apis, &node->list);
    sid->token = node->token;
    sid->handle = 0;
    sid->cookie = node->token;
ERROR:
    pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
    return ret;
}

int32_t UnregisterIpcCallback(SvcIdentity sid)
{
    if (pthread_mutex_lock(&g_ipcCallbackCb.mutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return LITEIPC_EINTNL;
    }

    AnonymousApi* node = NULL;
    AnonymousApi* next = NULL;
    UTILS_DL_LIST_FOR_EACH_ENTRY_SAFE(node, next, &g_ipcCallbackCb.apis, AnonymousApi, list)
    {
        if (node->token == sid.token) {
            UtilsListDelete(&node->list);
            free(node);
            pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
            return LITEIPC_OK;
        }
    }

    LOG(ERROR, "Input sid not exist.");
    pthread_mutex_unlock(&g_ipcCallbackCb.mutex);

    return LITEIPC_OK;
}

static inline uint32_t SetDeathHandlerPair(Testament* node, uint32_t index, IpcMsgHandler func, void* arg)
{
    node->hdlerPairs[index].usedFlag = true;
    node->hdlerPairs[index].hdler = func;
    node->hdlerPairs[index].arg = arg;
    return index;
}

static void BinderLinkToDeath(const SvcIdentity *sid, IpcMsgHandler callback, const IpcContext* context)
{
    struct BinderDeath *bd;
    struct {
        uint32_t cmd;
        struct binder_handle_cookie payload;
    } __attribute__((packed)) data;
    bd = calloc(1, sizeof(struct BinderDeath));
    if (bd == NULL) {
        return;
    }
    bd->func = (void *)callback;
    bd->token = sid->handle;
    data.cmd = BC_REQUEST_DEATH_NOTIFICATION;
    data.payload.handle = sid->handle;
    data.payload.cookie = (binder_uintptr_t)bd;
    BinderWrite(context, &data, sizeof(data));
}

int32_t RegisterDeathCallback(const IpcContext* context, SvcIdentity sid, IpcMsgHandler func, void* arg, uint32_t* cbId)
{
    int ret = LITEIPC_OK;
    Testament* node = NULL;

    if ((func == NULL) || (cbId == NULL)) {
        return LITEIPC_EINVAL;
    }

    const IpcContext *tmpContext = GetIpcContext(context, sid.ipcContext);
    if (tmpContext == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    if (pthread_mutex_lock(&g_ipcCallbackCb.mutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return LITEIPC_EINTNL;
    }

    UTILS_DL_LIST_FOR_EACH_ENTRY(node, &g_ipcCallbackCb.testaments, Testament, list)
    {
        if (node->handle != sid.handle) {
            continue;
        }

        if (node->size == MAX_DEATHCB_PER_SVC) {
            LOG(ERROR, "Reach max num on this svc:%d", sid.handle);
            ret = LITEIPC_EINTNL;
            goto UNLOCK_RETURN;
        }
        for (int i = 0; i < MAX_DEATHCB_PER_SVC; i++) {
            if (node->hdlerPairs[i].usedFlag == false) {
                node->size++;
                *cbId = SetDeathHandlerPair(node, i, func, arg);
                break;
            }
        }
        goto UNLOCK_RETURN;
    }
    node = (Testament*)malloc(sizeof(Testament));
    if (node == NULL) {
        LOG(ERROR, "Malloc failed.(size=%zu)", sizeof(Testament));
        ret = LITEIPC_ENOMEM;
        goto UNLOCK_RETURN;
    }
    (void)memset_s(node, sizeof(Testament), 0, sizeof(Testament));
    node->handle = sid.handle;
    node->size = 1;
    *cbId = SetDeathHandlerPair(node, 0, func, arg);
    UtilsListAdd(&g_ipcCallbackCb.testaments, &node->list);
    BinderLinkToDeath(&sid, (IpcMsgHandler)TryCallBack, tmpContext);

UNLOCK_RETURN:
    pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
    return ret;
}

int32_t UnregisterDeathCallback(SvcIdentity sid, uint32_t cbId)
{
    Testament* node = NULL;
    Testament* next = NULL;
    if (cbId >= MAX_DEATHCB_PER_SVC) {
        LOG(ERROR, "Wrong cbId:%u.", cbId);
        return LITEIPC_EINVAL;
    }
    if (pthread_mutex_lock(&g_ipcCallbackCb.mutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return LITEIPC_EINTNL;
    }
    UTILS_DL_LIST_FOR_EACH_ENTRY_SAFE(node, next, &g_ipcCallbackCb.testaments, Testament, list)
    {
        if (node->handle == sid.handle) {
            if ((node->isCallbackRuning == false) && (node->hdlerPairs[cbId].usedFlag == true)) {
                node->hdlerPairs[cbId].usedFlag = false;
                node->size--;
                if (node->size == 0) {
                    UtilsListDelete(&node->list);
                    free(node);
                }
            }
            pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
            return LITEIPC_OK;
        }
    }
    LOG(ERROR, "Input sid not exist.");
    pthread_mutex_unlock(&g_ipcCallbackCb.mutex);

    return LITEIPC_OK;
}

static void RemoveDeathCallback(uint32_t handle)
{
    Testament* node = NULL;
    Testament* next = NULL;

    if (pthread_mutex_lock(&g_ipcCallbackCb.mutex) != 0) {
        LOG(ERROR, "Get callback mutex failed.");
        return;
    }
    UTILS_DL_LIST_FOR_EACH_ENTRY_SAFE(node, next, &g_ipcCallbackCb.testaments, Testament, list)
    {
        if (node->handle == handle) {
            UtilsListDelete(&node->list);
            free(node);
            break;
        }
    }
    pthread_mutex_unlock(&g_ipcCallbackCb.mutex);
}

int32_t GetToken(const void* ipcMsg, uint32_t* token)
{
    if ((ipcMsg == NULL) || (token == NULL)) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    const IpcMsg* in = (const IpcMsg*)ipcMsg;
    *token = in->target.token;
    return LITEIPC_OK;
}

int32_t GetFlag(const void* ipcMsg, uint32_t* flag)
{
    if ((ipcMsg == NULL) || (flag == NULL)) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    const IpcMsg* in = (const IpcMsg*)ipcMsg;
    *flag = in->flag;
    return LITEIPC_OK;
}

pid_t GetCallingPid(const void* ipcMsg)
{
    if (ipcMsg == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    const IpcMsg* in = (const IpcMsg*)ipcMsg;
    return (pid_t)in->processID;
}

pid_t GetCallingUid(const void* ipcMsg)
{
    if (ipcMsg == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    const IpcMsg* in = (const IpcMsg*)ipcMsg;
    return (pid_t)in->userID;
}

pid_t GetCallingGid(const void* ipcMsg)
{
    if (ipcMsg == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        return LITEIPC_EINVAL;
    }

    const IpcMsg* in = (const IpcMsg*)ipcMsg;
    return (pid_t)in->gid;
}

int32_t FreeBuffer(const IpcContext* context, void* ptr)
{
    if (ptr == NULL) {
        return LITEIPC_OK;
    }

    IpcMsg* ipcMsg = (IpcMsg*)ptr;
    if (ipcMsg->freeData == NULL) {
        return LITEIPC_OK;
    }

    const IpcContext *tmpContext = GetIpcContext(context, g_tlsContext);
    if (tmpContext == NULL) {
        LOG(ERROR, "Invalid parameter, null pointer.");
        free(ptr);
        ptr = NULL;
        return LITEIPC_EINTNL;
    }
    struct FreeData data = {0};
    data.cmd = BC_FREE_BUFFER;
    data.buffer = (binder_uintptr_t)ipcMsg->freeData;
    int ret = BinderWrite(tmpContext, &data, sizeof(data));
    free(ptr);
    ptr = NULL;
    RETURN_IF_IPC_IOCTL_FAILED(LITEIPC_EBADF);
    return ret;
}

int32_t GenServiceHandle(SvcIdentity* sid, pid_t tid)
{
    if (g_tempHandles == NULL || sid == NULL) {
        return LITEIPC_EINVAL;
    }
    sid->handle = g_handle;
    return LITEIPC_OK;
}

int32_t BinderAcquire(const IpcContext* context, uint32_t handle)
{
    if (context == NULL) {
        return LITEIPC_EINVAL;
    }
    uint32_t cmd[2];
    cmd[0] = BC_ACQUIRE;
    cmd[1] = handle;
    int ret;
    ret = BinderWrite(context, &cmd, sizeof(cmd));
    return ret;
}

int32_t BinderRelease(const IpcContext* context, uint32_t handle)
{
    if (context == NULL) {
        return LITEIPC_EINVAL;
    }
    uint32_t cmd[2];
    cmd[0] = BC_RELEASE;
    cmd[1] = handle;
    int ret;
    ret = BinderWrite(context, &cmd, sizeof(cmd));
    return ret;
}
