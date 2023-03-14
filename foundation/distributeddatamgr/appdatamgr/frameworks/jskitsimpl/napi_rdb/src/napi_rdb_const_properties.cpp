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

#include "napi_rdb_const_properties.h"
#include "rdb_types.h"

using OHOS::DistributedRdb::SyncMode;
using OHOS::DistributedRdb::SubscribeMode;

namespace OHOS::RdbJsKit {
static napi_status SetNamedProperty(napi_env env, napi_value& obj, const std::string& name, int32_t value)
{
    napi_value property = nullptr;
    napi_status status = napi_create_int32(env, value, &property);
    if (status != napi_ok) {
        return status;
    }
    status = napi_set_named_property(env, obj, name.c_str(), property);
    if (status != napi_ok) {
        return status;
    }
    return status;
}

static napi_value ExportSyncMode(napi_env env)
{
    napi_value syncMode = nullptr;
    napi_create_object(env, &syncMode);
    (void)SetNamedProperty(env, syncMode, "SYNC_MODE_PUSH", (int32_t)SyncMode::PUSH);
    (void)SetNamedProperty(env, syncMode, "SYNC_MODE_PULL", (int32_t)SyncMode::PULL);
    napi_object_freeze(env, syncMode);
    return syncMode;
}

static napi_value ExportSubscribeType(napi_env env)
{
    napi_value subscribeType = nullptr;
    napi_create_object(env, &subscribeType);

    (void)SetNamedProperty(env, subscribeType, "SUBSCRIBE_TYPE_REMOTE", (int32_t)SubscribeMode::REMOTE);
    napi_object_freeze(env, subscribeType);
    return subscribeType;
}

napi_status InitConstProperties(napi_env env, napi_value exports)
{
    const napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("SyncMode", ExportSyncMode(env)),
        DECLARE_NAPI_PROPERTY("SubscribeType", ExportSubscribeType(env)),
    };

    size_t count = sizeof(properties) / sizeof(properties[0]);
    return napi_define_properties(env, exports, count, properties);
}
} // namespace OHOS::RdbJsKit