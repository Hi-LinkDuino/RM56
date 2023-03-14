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

#include "napi_values_bucket.h"

#include "js_logger.h"
#include "js_utils.h"
#include "value_object.h"

using namespace OHOS::AppDataMgrJsKit;
using namespace OHOS::NativeRdb;

__attribute__((visibility("default"))) napi_value NAPI_OHOS_Data_RdbJsKit_ValuesBucketProxy_NewInstance(
    napi_env env, ValuesBucket &valuesBucket)
{
    napi_value ret;
    NAPI_CALL(env, napi_create_object(env, &ret));
    std::map<std::string, ValueObject> valuesMap;
    valuesBucket.GetAll(valuesMap);
    std::map<std::string, ValueObject>::iterator it;
    for (it = valuesMap.begin(); it != valuesMap.end(); ++it) {
        std::string key = it->first;
        auto valueObject = it->second;
        napi_value value = nullptr;
        switch (valueObject.GetType()) {
            case ValueObjectType::TYPE_NULL: {
                    value = nullptr;
                } break;
            case ValueObjectType::TYPE_INT: {
                    int64_t intVal = 0;
                    valueObject.GetLong(intVal);
                    value = JSUtils::Convert2JSValue(env, intVal);
                } break;
            case ValueObjectType::TYPE_DOUBLE: {
                    double doubleVal = 0L;
                    valueObject.GetDouble(doubleVal);
                    value = JSUtils::Convert2JSValue(env, doubleVal);
                } break;
            case ValueObjectType::TYPE_BLOB: {
                    std::vector<uint8_t> blobVal;
                    valueObject.GetBlob(blobVal);
                    value = JSUtils::Convert2JSValue(env, blobVal);
                } break;
            case ValueObjectType::TYPE_BOOL: {
                    bool boolVal = false;
                    valueObject.GetBool(boolVal);
                    value = JSUtils::Convert2JSValue(env, boolVal);
                } break;
            default: {
                    std::string strVal = "";
                    valueObject.GetString(strVal);
                    value = JSUtils::Convert2JSValue(env, strVal);
                } break;
        }
        NAPI_CALL(env, napi_set_named_property(env, ret, key.c_str(), value));
    }

    return ret;
}

__attribute__((visibility("default"))) ValuesBucket *NAPI_OHOS_Data_RdbJsKit_ValuesBucketProxy_GetNativeObject(
    napi_env env, napi_value &arg)
{
    ValuesBucket *valuesBucket = new ValuesBucket;
    napi_value keys = 0;
    napi_get_property_names(env, arg, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    if (status != napi_ok) {
        LOG_DEBUG("ValuesBucket errr");
        return valuesBucket;
    }
    LOG_DEBUG("ValuesBucket num:%{public}u ", arrLen);
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value key;
        napi_get_element(env, keys, i, &key);
        std::string keyStr = JSUtils::Convert2String(env, key, JSUtils::DEFAULT_BUF_SIZE);
        napi_value value;
        napi_get_property(env, arg, key, &value);
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, value, &valueType);
        if (valueType == napi_string) {
            std::string valueString = JSUtils::Convert2String(env, value, JSUtils::DEFAULT_BUF_SIZE);
            valuesBucket->PutString(keyStr, valueString);
            LOG_DEBUG("ValueObject type napi_string");
        } else if (valueType == napi_number) {
            double valueNumber;
            napi_get_value_double(env, value, &valueNumber);
            valuesBucket->PutDouble(keyStr, valueNumber);
            LOG_DEBUG("ValueObject type napi_number");
        } else if (valueType == napi_boolean) {
            bool valueBool = false;
            napi_get_value_bool(env, value, &valueBool);
            valuesBucket->PutBool(keyStr, valueBool);
            LOG_DEBUG("ValueObject type napi_boolean");
        } else if (valueType == napi_null) {
            valuesBucket->PutNull(keyStr);
            LOG_DEBUG("ValueObject type napi_null");
        } else if (valueType == napi_object) {
            valuesBucket->PutBlob(keyStr, JSUtils::Convert2U8Vector(env, value));
            LOG_DEBUG("ValueObject type napi_object");
        } else {
            LOG_WARN("valuesBucket error");
        }
    }
    return valuesBucket;
}