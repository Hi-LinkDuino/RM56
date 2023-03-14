/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "distributed_module_info.h"

#include <fcntl.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_MODULE_NAME = "moduleName";
const std::string JSON_KEY_ABILITIES = "abilities";
}
bool DistributedModuleInfo::ReadFromParcel(Parcel &parcel)
{
    MessageParcel *messageParcel = reinterpret_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        APP_LOGE("Type conversion failed");
        return false;
    }
    uint32_t length = messageParcel->ReadUint32();
    if (length == 0) {
        APP_LOGE("Invalid data length");
        return false;
    }
    const char *data = reinterpret_cast<const char *>(messageParcel->ReadRawData(length));
    if (!data) {
        APP_LOGE("Fail to read raw data, length = %{public}d", length);
        return false;
    }
    nlohmann::json jsonObject = nlohmann::json::parse(data, nullptr, false);
    if (jsonObject.is_discarded()) {
        APP_LOGE("failed to parse DistributedModuleInfo");
        return false;
    }
    *this = jsonObject.get<DistributedModuleInfo>();
    return true;
}

bool DistributedModuleInfo::Marshalling(Parcel &parcel) const
{
    MessageParcel *messageParcel = reinterpret_cast<MessageParcel *>(&parcel);
    if (!messageParcel) {
        APP_LOGE("Type conversion failed");
        return false;
    }
    nlohmann::json json = *this;
    std::string str = json.dump();
    if (!messageParcel->WriteUint32(str.size() + 1)) {
        APP_LOGE("Failed to write data size");
        return false;
    }
    if (!messageParcel->WriteRawData(str.c_str(), str.size() + 1)) {
        APP_LOGE("Failed to write data");
        return false;
    }
    return true;
}

DistributedModuleInfo *DistributedModuleInfo::Unmarshalling(Parcel &parcel)
{
    DistributedModuleInfo *info = new (std::nothrow) DistributedModuleInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        APP_LOGW("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

void DistributedModuleInfo::Dump(const std::string &prefix, int fd)
{
    APP_LOGI("called dump DistributedModuleInfo");
    if (fd < 0) {
        APP_LOGE("dump DistributedModuleInfo fd error");
        return;
    }
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        APP_LOGE("dump DistributedModuleInfo fcntl error %{public}d", errno);
        return;
    }
    uint uflags = static_cast<uint>(flags);
    uflags &= O_ACCMODE;
    if ((uflags == O_WRONLY) || (uflags == O_RDWR)) {
        nlohmann::json jsonObject = *this;
        std::string result;
        result.append(prefix);
        result.append(jsonObject.dump(Constants::DUMP_INDENT));
        int ret = TEMP_FAILURE_RETRY(write(fd, result.c_str(), result.size()));
        if (ret < 0) {
            APP_LOGE("dump DistributedModuleInfo write error %{public}d", errno);
        }
    }
}

void to_json(nlohmann::json& jsonObject, const DistributedModuleInfo& distributedModuleInfo)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_MODULE_NAME, distributedModuleInfo.moduleName},
        {JSON_KEY_ABILITIES, distributedModuleInfo.abilities},
    };
}

void from_json(const nlohmann::json& jsonObject, DistributedModuleInfo& distributedModuleInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_MODULE_NAME,
        distributedModuleInfo.moduleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<DistributedAbilityInfo>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_ABILITIES,
        distributedModuleInfo.abilities,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::OBJECT);
}
}  // namespace AppExecFwk
}  // namespace OHOS
