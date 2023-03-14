/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <string_ex.h>
#include <unistd.h>

#include "hdf_log.h"
#include "hdf_object_manager.h"
#include "hdf_sbuf_ipc.h"
#include "hdf_remote_adapter.h"

#define HDF_LOG_TAG hdf_remote_adapter

static constexpr int32_t THREAD_POOL_BASE_THREAD_COUNT = 1;
static int32_t g_remoteThreadMax = THREAD_POOL_BASE_THREAD_COUNT;

HdfRemoteServiceStub::HdfRemoteServiceStub(struct HdfRemoteService *service)
    : IPCObjectStub(std::u16string(u"")), service_(service)
{
}

int HdfRemoteServiceStub::OnRemoteRequest(uint32_t code,
    OHOS::MessageParcel &data, OHOS::MessageParcel &reply, OHOS::MessageOption &option)
{
    (void)option;
    if (service_ == nullptr) {
        return HDF_ERR_INVALID_OBJECT;
    }

    int ret = HDF_FAILURE;
    struct HdfSBuf *dataSbuf = ParcelToSbuf(&data);
    struct HdfSBuf *replySbuf = ParcelToSbuf(&reply);

    struct HdfRemoteDispatcher *dispatcher = service_->dispatcher;
    if (dispatcher != nullptr && dispatcher->Dispatch != nullptr) {
        ret = dispatcher->Dispatch((HdfRemoteService *)service_->target, code, dataSbuf, replySbuf);
    } else {
        HDF_LOGE("dispatcher or dispatcher->Dispatch is null, flags is: %{public}d", option.GetFlags());
    }

    HdfSbufRecycle(dataSbuf);
    HdfSbufRecycle(replySbuf);
    return ret;
}

HdfRemoteServiceStub::~HdfRemoteServiceStub()
{
}

HdfDeathNotifier::HdfDeathNotifier(struct HdfRemoteService *service, struct HdfDeathRecipient *recipient)
    : recipient_(recipient), service_(service)
{
}

HdfDeathNotifier::~HdfDeathNotifier()
{
}

void HdfDeathNotifier::OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject> &) /* who = 0 */
{
    if (recipient_ != nullptr) {
        recipient_->OnRemoteDied(recipient_, service_);
    }
}

static int HdfRemoteAdapterOptionalDispatch(struct HdfRemoteService *service, int code,
    HdfSBuf *data, HdfSBuf *reply, bool sync)
{
    if (service == nullptr) {
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;

    if (reply == nullptr) {
        static OHOS::MessageParcel dummyReply;
        dummyReply.FlushBuffer();
        replyParcel = &dummyReply;
    } else if (SbufToParcel(reply, &replyParcel)) {
        HDF_LOGE("%{public}s:invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (SbufToParcel(data, &dataParcel)) {
        HDF_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int flag = sync ? OHOS::MessageOption::TF_SYNC : OHOS::MessageOption::TF_ASYNC;
    OHOS::MessageOption option(flag);
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    if (dataParcel != nullptr) {
        OHOS::sptr<OHOS::IRemoteObject> remote = holder->remote_;
        if (remote != nullptr) {
            return remote->SendRequest(code, *dataParcel, *replyParcel, option);
        }
    }
    return HDF_FAILURE;
}

static int HdfRemoteAdapterDispatch(struct HdfRemoteService *service,
    int code, HdfSBuf *data, HdfSBuf *reply)
{
    return HdfRemoteAdapterOptionalDispatch(service, code, data, reply, true);
}

static int HdfRemoteAdapterDispatchAsync(struct HdfRemoteService *service,
    int code, HdfSBuf *data, HdfSBuf *reply)
{
    return HdfRemoteAdapterOptionalDispatch(service, code, data, reply, false);
}

HdfRemoteServiceHolder::HdfRemoteServiceHolder() : remote_(nullptr), deathRecipient_(nullptr)
{
    service_.object_.objectId = HDF_OBJECT_ID_REMOTE_SERVICE;
    service_.dispatcher = nullptr;
    service_.target = nullptr;
}

bool HdfRemoteServiceHolder::SetInterfaceDescriptor(const char *desc)
{
    if (remote_ == nullptr || desc == nullptr) {
        return false;
    }
    std::u16string newDesc = OHOS::Str8ToStr16(std::string(desc));
    if (newDesc.empty()) {
        HDF_LOGE("failed to set interface des, error on cover str8 to str16, %{public}s", desc);
        return false;
    }
    (const_cast<std::u16string *>(&remote_->descriptor_))->assign(newDesc);

    return true;
}

void HdfRemoteAdapterAddDeathRecipient(
    struct HdfRemoteService *service, struct HdfDeathRecipient *recipient)
{
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    if (holder == nullptr) {
        return;
    }
    OHOS::sptr<OHOS::IRemoteObject> remote = holder->remote_;
    if (remote == nullptr) {
        return;
    }
    if (holder->deathRecipient_ != nullptr) {
        remote->RemoveDeathRecipient(holder->deathRecipient_);
    }
    holder->deathRecipient_ = new HdfDeathNotifier(service, recipient);
    remote->AddDeathRecipient(holder->deathRecipient_);
}

void HdfRemoteAdapterRemoveDeathRecipient(
    struct HdfRemoteService *service, struct HdfDeathRecipient *recipient)
{
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    if (holder == nullptr) {
        return;
    }
    OHOS::sptr<OHOS::IRemoteObject> remote = holder->remote_;
    if (remote == nullptr) {
        return;
    }
    if (holder->deathRecipient_ != nullptr) {
        remote->RemoveDeathRecipient(holder->deathRecipient_);
        holder->deathRecipient_ = nullptr;
    }
}

struct HdfRemoteService *HdfRemoteAdapterBind(OHOS::sptr<OHOS::IRemoteObject> binder)
{
    struct HdfRemoteService *remoteService = nullptr;
    static HdfRemoteDispatcher dispatcher = {
        .Dispatch = HdfRemoteAdapterDispatch,
        .DispatchAsync = HdfRemoteAdapterDispatchAsync,
    };

    struct HdfRemoteServiceHolder *holder = new HdfRemoteServiceHolder();
    if (holder != nullptr) {
        holder->remote_ = binder;
        remoteService = &holder->service_;
        remoteService->dispatcher = &dispatcher;
        remoteService->index = (uint64_t)binder.GetRefPtr();
        return remoteService;
    }
    return nullptr;
}

struct HdfRemoteService *HdfRemoteAdapterObtain(void)
{
    struct HdfRemoteServiceHolder *holder = new HdfRemoteServiceHolder();
    holder->remote_ = new HdfRemoteServiceStub(&holder->service_);
    return &holder->service_;
}

void HdfRemoteAdapterRecycle(struct HdfRemoteService *object)
{
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(object);
    if (holder != nullptr) {
        if (holder->remote_ != nullptr) {
            holder->remote_ = nullptr;
        }
        delete holder;
    }
}

int HdfRemoteAdapterAddService(const char *name, struct HdfRemoteService *service)
{
    if (name == nullptr || service == nullptr) {
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::sptr<OHOS::IServiceRegistry> sr = OHOS::ServiceRegistry::GetInstance();
    if (sr == nullptr) {
        HDF_LOGE("failed to get service registry");
        return HDF_FAILURE;
    }
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    int ret = sr->AddService(OHOS::Str8ToStr16(name), holder->remote_);
    if (ret == 0) {
        (void)OHOS::IPCSkeleton::GetInstance().SetMaxWorkThreadNum(g_remoteThreadMax++);
    }
    return ret;
}

struct HdfRemoteService *HdfRemoteAdapterGetService(const char *name)
{
    if (name == nullptr) {
        return nullptr;
    }

    OHOS::sptr<OHOS::IServiceRegistry> sr = OHOS::ServiceRegistry::GetInstance();
    if (sr == nullptr) {
        HDF_LOGE("failed to get service registry");
        return nullptr;
    }
    OHOS::sptr<OHOS::IRemoteObject> remote = sr->GetService(OHOS::Str8ToStr16(name));
    if (remote != nullptr) {
        return HdfRemoteAdapterBind(remote);
    }
    return nullptr;
}

int HdfRemoteAdapterAddSa(int32_t saId, struct HdfRemoteService *service)
{
    if (service == nullptr) {
        return HDF_ERR_INVALID_PARAM;
    }

    auto saManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    const int32_t waitTimes = 50;
    const int32_t sleepInterval = 20000;
    int32_t timeout = waitTimes;
    while (saManager == nullptr && (timeout-- > 0)) {
        HDF_LOGI("waiting for samgr...");
        usleep(sleepInterval);
        saManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    }

    if (saManager == nullptr) {
        HDF_LOGE("failed to get sa manager, waiting timeot");
        return HDF_FAILURE;
    }
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    int ret = saManager->AddSystemAbility(saId, holder->remote_);
    (void)OHOS::IPCSkeleton::GetInstance().SetMaxWorkThreadNum(g_remoteThreadMax++);
    HDF_LOGI("add sa %{public}d, ret = %{public}s", saId, (ret == 0) ? "succ" : "fail");

    return HDF_SUCCESS;
}

struct HdfRemoteService *HdfRemoteAdapterGetSa(int32_t saId)
{
    auto saManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        HDF_LOGE("failed to get sa manager");
        return nullptr;
    }
    OHOS::sptr<OHOS::IRemoteObject> remote = saManager->GetSystemAbility(saId);
    constexpr int32_t waitTimes = 50;
    constexpr int32_t sleepInterval = 20000;
    int32_t timeout = waitTimes;
    while (remote == nullptr && (timeout-- > 0)) {
        HDF_LOGD("waiting for saId %{public}d", saId);
        usleep(sleepInterval);
        remote = saManager->GetSystemAbility(saId);
    }
    if (remote != nullptr) {
        return HdfRemoteAdapterBind(remote);
    } else {
        HDF_LOGE("failed to get sa %{public}d", saId);
    }
    return nullptr;
}

bool HdfRemoteAdapterSetInterfaceDesc(struct HdfRemoteService *service, const char *desc)
{
    if (service == nullptr || desc == nullptr) {
        return false;
    }
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    return holder->SetInterfaceDescriptor(desc);
}

bool HdfRemoteAdapterWriteInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data)
{
    if (service == nullptr || data == nullptr) {
        return false;
    }
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    OHOS::MessageParcel *parcel = nullptr;

    if (SbufToParcel(data, &parcel) != HDF_SUCCESS) {
        HDF_LOGE("failed to write interface token, SbufToParcel error");
        return false;
    }

    if (holder->remote_ == nullptr) {
        HDF_LOGE("failed to write interface token, holder->remote is nullptr");
        return false;
    }
    if (holder->remote_->GetObjectDescriptor().empty()) {
        HDF_LOGE("failed to write interface token, empty token");
        return false;
    }
    return parcel->WriteInterfaceToken(holder->remote_->GetObjectDescriptor());
}

bool HdfRemoteAdapterCheckInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data)
{
    if (service == nullptr || data == nullptr) {
        return false;
    }
    struct HdfRemoteServiceHolder *holder = reinterpret_cast<struct HdfRemoteServiceHolder *>(service);
    if (holder == nullptr || holder->remote_ == nullptr) {
        return false;
    }
    OHOS::MessageParcel *parcel = nullptr;

    if (SbufToParcel(data, &parcel) != HDF_SUCCESS) {
        return false;
    }
    auto desc = parcel->ReadInterfaceToken();
    if (desc.empty()) {
        HDF_LOGE("failed to check interface, empty token");
        return false;
    }
    if (holder->remote_->GetObjectDescriptor() != desc) {
        std::string descStr8 = OHOS::Str16ToStr8(desc);
        HDF_LOGE("calling unknown interface: %{public}s", descStr8.c_str());
        return false;
    }

    return true;
}
