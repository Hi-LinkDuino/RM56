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

#include "di_config.h"

#include "bt_def.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
DIConfig &DIConfig::GetInstance()
{
    static DIConfig instance;
    return instance;
}

DIConfig::DIConfig() : config_(AdapterDeviceInfo::GetInstance())
{}

DIConfig::~DIConfig()
{}

bool DIConfig::LoadConfigFile() const
{
    /// Load Device info Config File.
    bool ret = config_->Load();
    if (!ret) {
        LOG_ERROR("[DIConfig]::%{public}s failed!", __func__);
    }

    return ret;
}

int DIConfig::GetSpecificaitonId() const
{
    int specId = 0;
    if (!config_->GetValue(SECTION_BREDR, PROPERTY_SPECIFICATION_ID, specId)) {
        LOG_ERROR("[DIConfig]::%{public}s failed!", __func__);
    }

    return specId;
}

int DIConfig::GetVendorId() const
{
    int vendorId = 0;
    if (!config_->GetValue(SECTION_BREDR, PROPERTY_VENDOR_ID, vendorId)) {
        LOG_ERROR("[DIConfig]::%{public}s failed!", __func__);
    }

    return vendorId;
}

int DIConfig::GetProductId() const
{
    int productId = 0;
    if (!config_->GetValue(SECTION_BREDR, PROPERTY_PRODUCT_ID, productId)) {
        LOG_ERROR("[DIConfig]::%{public}s failed!", __func__);
    }

    return productId;
}

int DIConfig::GetVersion() const
{
    int version = 0;
    if (!config_->GetValue(SECTION_BREDR, PROPERTY_VERSION, version)) {
        LOG_ERROR("[DIConfig]::%{public}s failed!", __func__);
    }

    return version;
}

bool DIConfig::GetPrimaryRecord() const
{
    bool primaryRecord = false;
    if (!config_->GetValue(SECTION_BREDR, PROPERTY_PRIMARY_RECORD, primaryRecord)) {
        LOG_ERROR("[DIConfig]::%{public}s failed!", __func__);
    }

    return primaryRecord;
}

int DIConfig::GetVendorIdSource() const
{
    int vendorIdSrc = 0;
    if (!config_->GetValue(SECTION_BREDR, PROPERTY_VENDOR_ID_SOURCE, vendorIdSrc)) {
        LOG_ERROR("[DIConfig]::%{public}s failed!", __func__);
    }

    return vendorIdSrc;
}
}  // namespace bluetooth
}  // namespace OHOS