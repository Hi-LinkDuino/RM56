/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "di_service.h"

#include "class_creator.h"
#include "log.h"
#include "sdp.h"

namespace OHOS {
namespace bluetooth {
DIService::DIService() : utility::Context(PROFILE_NAME_DI, "1.2.1"), config_(DIConfig::GetInstance())
{}

DIService::~DIService()
{}

utility::Context *DIService::GetContext()
{
    return this;
}

void DIService::Enable()
{
    LOG_DEBUG("[DIService]::%{public}s", __FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&DIService::StartUp, this));
}

void DIService::Disable()
{
    LOG_DEBUG("[DIService]::%{public}s", __FUNCTION__);
    GetDispatcher()->PostTask(std::bind(&DIService::ShutDown, this));
}

void DIService::StartUp()
{
    LOG_DEBUG("[DIService]::%{public}s", __FUNCTION__);
    bool result = config_.LoadConfigFile();
    if (result) {
        LoadDeviceInfo();
    } else {
        LOG_ERROR("[DIService]:%{public}s() load config file failed", __FUNCTION__);
    }

    result = RegisterDIService();
    if (!result) {
        LOG_ERROR("[DIService]:%{public}s() RegisterDIService failed", __FUNCTION__);
    }

    GetContext()->OnEnable(PROFILE_NAME_DI, result);
}

void DIService::ShutDown()
{
    LOG_DEBUG("[DIService]::%{public}s", __FUNCTION__);

    bool result = DeregisterDIService();
    if (!result) {
        LOG_ERROR("[DIService]:%{public}s() DeregisterDIService failed", __FUNCTION__);
    }

    GetContext()->OnDisable(PROFILE_NAME_DI, result);
}

void DIService::LoadDeviceInfo()
{
    LOG_DEBUG("[DIService]::%{public}s", __FUNCTION__);

    priRecord_ = config_.GetPrimaryRecord();
    specId_ = config_.GetSpecificaitonId();
    vendorId_ = config_.GetVendorId();
    procId_ = config_.GetProductId();
    version_ = config_.GetVersion();
    venIdSrc_ = config_.GetVendorIdSource();

    LOG_DEBUG("Get Device info config:");
    LOG_DEBUG("Primary record is: %{public}d", priRecord_);
    LOG_DEBUG("Specification ID is: %u", specId_);
    LOG_DEBUG("Vendor ID is: %u", vendorId_);
    LOG_DEBUG("Product ID is: %u", procId_);
    LOG_DEBUG("Version is: %u", version_);
    LOG_DEBUG("Vender ID Source is: %u", venIdSrc_);
}

bool DIService::RegisterDIService()
{
    LOG_DEBUG("[DIService]::%{public}s", __FUNCTION__);

    uint32_t handle = SDP_CreateServiceRecord();
    if (handle == 0) {
        LOG_ERROR("%{public}s: SDP_CreateServiceRecord Failed!", __FUNCTION__);
        return false;
    }
    handle_ = handle;

    BtUuid classid;
    classid.type = BT_UUID_16;
    classid.uuid16 = UUID_SERVICE_CLASS_PNP_INFORMATION;
    int ret = SDP_AddServiceClassIdList(handle, &classid, CLASS_ID_NUMBER);
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddServiceClassIdList Failed", __FUNCTION__);
    }

    ret = SDP_AddAttribute(handle, ATTR_ID_SPECIFICATION_ID, SDP_TYPE_UINT_16, (void *)&specId_, sizeof(specId_));
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddAttribute ATTR_ID_SPECIFICATION_ID Failed", __FUNCTION__);
    }

    ret = SDP_AddAttribute(handle, ATTR_ID_VENDOR_ID, SDP_TYPE_UINT_16, (void *)&vendorId_, sizeof(vendorId_));
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddAttribute ATTR_ID_VENDOR_ID Failed", __FUNCTION__);
    }

    ret = SDP_AddAttribute(handle, ATTR_ID_PRODUCT_ID, SDP_TYPE_UINT_16, (void *)&procId_, sizeof(procId_));
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddAttribute ATTR_ID_PRODUCT_ID Failed", __FUNCTION__);
    }

    ret = SDP_AddAttribute(handle, ATTR_ID_VERSION, SDP_TYPE_UINT_16, (void *)&version_, sizeof(version_));
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddAttribute ATTR_ID_VERSION Failed", __FUNCTION__);
    }

    ret = SDP_AddAttribute(handle, ATTR_ID_PRIMARY_RECORD, SDP_TYPE_BOOL, (void *)&priRecord_, sizeof(priRecord_));
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddAttribute ATTR_ID_PRIMARY_RECORD Failed", __FUNCTION__);
    }

    ret = SDP_AddAttribute(handle, ATTR_ID_VENDOR_ID_SOURCE, SDP_TYPE_UINT_16, (void *)&venIdSrc_, sizeof(venIdSrc_));
    if (ret != BT_NO_ERROR) {
        LOG_WARN("%{public}s::SDP_AddAttribute ATTR_ID_VENDOR_ID_SOURCE Failed", __FUNCTION__);
    }

    ret = SDP_RegisterServiceRecord(handle);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s::SDP_RegisterServiceRecord Failed", __FUNCTION__);
        return false;
    }

    return true;
}

bool DIService::DeregisterDIService() const
{
    LOG_DEBUG("[DIService]::%{public}s", __FUNCTION__);
    int ret = SDP_DeregisterServiceRecord(handle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s::SDP_DeregisterServiceRecord Failed", __FUNCTION__);
        return false;
    }

    ret = SDP_DestroyServiceRecord(handle_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("%{public}s::SDP_DestroyServiceRecord Failed", __FUNCTION__);
        return false;
    }

    return true;
}

int DIService::Connect(const RawAddress &device)
{
    return 0;
}

int DIService::Disconnect(const RawAddress &device)
{
    return 0;
}

std::list<RawAddress> DIService::GetConnectDevices()
{
    std::list<RawAddress> addr;
    return addr;
}

int DIService::GetConnectState()
{
    return 0;
}

int DIService::GetMaxConnectNum()
{
    return 0;
}

REGISTER_CLASS_CREATOR(DIService);
}  // namespace bluetooth
}  // namespace OHOS