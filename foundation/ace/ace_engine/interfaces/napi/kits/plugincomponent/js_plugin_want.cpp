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

#include "js_plugin_want.h"

#include "hilog_wrapper.h"
#include "js_plugin_util.h"
#include "ohos/aafwk/base/array_wrapper.h"
#include "ohos/aafwk/base/bool_wrapper.h"
#include "ohos/aafwk/base/byte_wrapper.h"
#include "ohos/aafwk/base/double_wrapper.h"
#include "ohos/aafwk/base/float_wrapper.h"
#include "ohos/aafwk/base/int_wrapper.h"
#include "ohos/aafwk/base/long_wrapper.h"
#include "ohos/aafwk/base/short_wrapper.h"
#include "ohos/aafwk/base/string_wrapper.h"
#include "ohos/aafwk/base/zchar_wrapper.h"
#include "ohos/aafwk/content/want_params_wrapper.h"

namespace OHOS::Ace::Napi {
/**
 * @brief Init param of wantOptions.
 *
 * @param flagMap Indicates flag of list in Want .
 */
void AceInnerInitWantOptionsData(std::map<std::string, unsigned int>& flagMap)
{
    flagMap.emplace("authReadUriPermission", Want::FLAG_AUTH_READ_URI_PERMISSION);
    flagMap.emplace("authWriteUriPermission", Want::FLAG_AUTH_WRITE_URI_PERMISSION);
    flagMap.emplace("abilityForwardResult", Want::FLAG_ABILITY_FORWARD_RESULT);
    flagMap.emplace("abilityContinuation", Want::FLAG_ABILITY_CONTINUATION);
    flagMap.emplace("notOhosComponent", Want::FLAG_NOT_OHOS_COMPONENT);
    flagMap.emplace("abilityFormEnabled", Want::FLAG_ABILITY_FORM_ENABLED);
    flagMap.emplace("authPersistableUriPermission", Want::FLAG_AUTH_PERSISTABLE_URI_PERMISSION);
    flagMap.emplace("authPrefixUriPermission", Want::FLAG_AUTH_PREFIX_URI_PERMISSION);
    flagMap.emplace("abilitySliceMultiDevice", Want::FLAG_ABILITYSLICE_MULTI_DEVICE);
    flagMap.emplace("startForegroundAbility", Want::FLAG_START_FOREGROUND_ABILITY);
    flagMap.emplace("installOnDemand", Want::FLAG_INSTALL_ON_DEMAND);
    flagMap.emplace("abilitySliceForwardResult", Want::FLAG_ABILITYSLICE_FORWARD_RESULT);
    flagMap.emplace("installWithBackgroundMode", Want::FLAG_INSTALL_WITH_BACKGROUND_MODE);
}

napi_value AceWrapElementName(napi_env env, const ElementName& elementName)
{
    HILOG_INFO("%{public}s called.", __func__);

    napi_value jsObject = nullptr;
    NAPI_CALL(env, napi_create_object(env, &jsObject));

    napi_value jsValue = nullptr;
    HILOG_INFO("%{public}s called. deviceID=%{public}s", __func__, elementName.GetDeviceID().c_str());
    NAPI_CALL(env, napi_create_string_utf8(env, elementName.GetDeviceID().c_str(), NAPI_AUTO_LENGTH, &jsValue));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "deviceId", jsValue));

    jsValue = nullptr;
    HILOG_INFO("%{public}s called. GetBundleName=%{public}s", __func__, elementName.GetBundleName().c_str());
    NAPI_CALL(env, napi_create_string_utf8(env, elementName.GetBundleName().c_str(), NAPI_AUTO_LENGTH, &jsValue));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "bundleName", jsValue));

    jsValue = nullptr;
    HILOG_INFO("%{public}s called. GetAbilityName=%{public}s", __func__, elementName.GetAbilityName().c_str());
    NAPI_CALL(env, napi_create_string_utf8(env, elementName.GetAbilityName().c_str(), NAPI_AUTO_LENGTH, &jsValue));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "abilityName", jsValue));

    return jsObject;
}

bool AceUnwrapElementName(napi_env env, napi_value param, ElementName& elementName)
{
    HILOG_INFO("%{public}s called.", __func__);

    std::string natValue("");
    if (AceUnwrapStringByPropertyName(env, param, "deviceId", natValue)) {
        elementName.SetDeviceID(natValue);
    }

    natValue = "";
    if (AceUnwrapStringByPropertyName(env, param, "bundleName", natValue)) {
        elementName.SetBundleName(natValue);
    }

    natValue = "";
    if (AceUnwrapStringByPropertyName(env, param, "abilityName", natValue)) {
        elementName.SetAbilityName(natValue);
    }
    return true;
}
bool AceInnerWrapWantParamsChar(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IChar *ao = AAFwk::IChar::Query(value);
    if (ao != nullptr) {
        std::string natValue(static_cast<Char *>(ao)->ToString());
        HILOG_INFO("%{public}s called. key=%{public}s, natValue=%{public}s", __func__, key.c_str(), natValue.c_str());
        napi_value jsValue = AceWrapStringToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsString(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IString *ao = AAFwk::IString::Query(value);
    if (ao != nullptr) {
        std::string natValue = AAFwk::String::Unbox(ao);
        HILOG_INFO("%{public}s called. key=%{public}s, natValue=%{public}s", __func__, key.c_str(), natValue.c_str());
        napi_value jsValue = AceWrapStringToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsBool(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IBoolean *bo = AAFwk::IBoolean::Query(value);
    if (bo != nullptr) {
        bool natValue = AAFwk::Boolean::Unbox(bo);
        napi_value jsValue = AceWrapBoolToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsByte(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IByte *bo = AAFwk::IByte::Query(value);
    if (bo != nullptr) {
        int32_t intValue = static_cast<int32_t>(AAFwk::Byte::Unbox(bo));
        napi_value jsValue = AceWrapInt32ToJS(env, intValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsShort(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IShort *ao = AAFwk::IShort::Query(value);
    if (ao != nullptr) {
        short natValue = AAFwk::Short::Unbox(ao);
        napi_value jsValue = AceWrapInt32ToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsInt32(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IInteger *ao = AAFwk::IInteger::Query(value);
    if (ao != nullptr) {
        int natValue = AAFwk::Integer::Unbox(ao);
        HILOG_INFO("%{public}s called. key=%{public}s, natValue=%{public}d", __func__, key.c_str(), natValue);
        napi_value jsValue = AceWrapInt32ToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsInt64(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::ILong *ao = AAFwk::ILong::Query(value);
    if (ao != nullptr) {
        int64_t natValue = AAFwk::Long::Unbox(ao);
        napi_value jsValue = AceWrapInt64ToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsFloat(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IFloat *ao = AAFwk::IFloat::Query(value);
    if (ao != nullptr) {
        float natValue = AAFwk::Float::Unbox(ao);
        napi_value jsValue = AceWrapDoubleToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsDouble(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    auto value = wantParams.GetParam(key);
    AAFwk::IDouble *ao = AAFwk::IDouble::Query(value);
    if (ao != nullptr) {
        double natValue = AAFwk::Double::Unbox(ao);
        napi_value jsValue = AceWrapDoubleToJS(env, natValue);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}

bool AceInnerWrapWantParamsWantParams(
    napi_env env, napi_value jsObject, const std::string& key, const AAFwk::WantParams& wantParams)
{
    HILOG_INFO("%{public}s called. key=%{public}s", __func__, key.c_str());
    auto value = wantParams.GetParam(key);
    AAFwk::IWantParams *o = AAFwk::IWantParams::Query(value);
    if (o != nullptr) {
        AAFwk::WantParams wp = AAFwk::WantParamWrapper::Unbox(o);
        napi_value jsValue = AceWrapWantParams(env, wp);
        if (jsValue != nullptr) {
            NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
            return true;
        }
    }
    return false;
}
bool AceInnerWrapWantParamsArrayChar(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    HILOG_INFO("%{public}s called.", __func__);
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<std::string> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IChar *iValue = AAFwk::IChar::Query(iface);
            if (iValue != nullptr) {
                std::string str(static_cast<Char *>(iValue)->ToString());
                HILOG_INFO("%{public}s called. str=%{public}s", __func__, str.c_str());
                natArray.push_back(str);
            }
        }
    }

    napi_value jsValue = AceWrapArrayStringToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArrayString(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<std::string> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IString *iValue = AAFwk::IString::Query(iface);
            if (iValue != nullptr) {
                natArray.push_back(AAFwk::String::Unbox(iValue));
            }
        }
    }

    napi_value jsValue = AceWrapArrayStringToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArrayBool(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<bool> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IBoolean *iValue = AAFwk::IBoolean::Query(iface);
            if (iValue != nullptr) {
                natArray.push_back(AAFwk::Boolean::Unbox(iValue));
            }
        }
    }

    napi_value jsValue = AceWrapArrayBoolToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArrayShort(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<int> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IShort *iValue = AAFwk::IShort::Query(iface);
            if (iValue != nullptr) {
                natArray.push_back(AAFwk::Short::Unbox(iValue));
            }
        }
    }

    napi_value jsValue = AceWrapArrayInt32ToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}
bool AceInnerWrapWantParamsArrayByte(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<int> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IByte *iValue = AAFwk::IByte::Query(iface);
            if (iValue != nullptr) {
                int intValue = static_cast<int>(AAFwk::Byte::Unbox(iValue));
                natArray.push_back(intValue);
            }
        }
    }

    napi_value jsValue = AceWrapArrayInt32ToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArrayInt32(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<int> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IInteger *iValue = AAFwk::IInteger::Query(iface);
            if (iValue != nullptr) {
                natArray.push_back(AAFwk::Integer::Unbox(iValue));
            }
        }
    }

    napi_value jsValue = AceWrapArrayInt32ToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArrayInt64(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<int64_t> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::ILong *iValue = AAFwk::ILong::Query(iface);
            if (iValue != nullptr) {
                natArray.push_back(AAFwk::Long::Unbox(iValue));
            }
        }
    }

    napi_value jsValue = AceWrapArrayInt64ToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArrayFloat(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<double> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IFloat *iValue = AAFwk::IFloat::Query(iface);
            if (iValue != nullptr) {
                natArray.push_back(AAFwk::Float::Unbox(iValue));
            }
        }
    }

    napi_value jsValue = AceWrapArrayDoubleToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArrayDouble(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    long size = 0;
    if (ao->GetLength(size) != ERR_OK) {
        return false;
    }

    std::vector<double> natArray;
    for (int64_t i = 0; i < size; i++) {
        sptr<AAFwk::IInterface> iface = nullptr;
        if (ao->Get(i, iface) == ERR_OK) {
            AAFwk::IDouble *iValue = AAFwk::IDouble::Query(iface);
            if (iValue != nullptr) {
                natArray.push_back(AAFwk::Double::Unbox(iValue));
            }
        }
    }

    napi_value jsValue = AceWrapArrayDoubleToJS(env, natArray);
    if (jsValue != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, key.c_str(), jsValue), false);
        return true;
    }
    return false;
}

bool AceInnerWrapWantParamsArray(napi_env env, napi_value jsObject, const std::string& key,
    sptr<AAFwk::IArray>& ao)
{
    HILOG_INFO("%{public}s called. key=%{public}s", __func__, key.c_str());
    if (AAFwk::Array::IsStringArray(ao)) {
        return AceInnerWrapWantParamsArrayString(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsBooleanArray(ao)) {
        return AceInnerWrapWantParamsArrayBool(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsShortArray(ao)) {
        return AceInnerWrapWantParamsArrayShort(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsIntegerArray(ao)) {
        return AceInnerWrapWantParamsArrayInt32(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsLongArray(ao)) {
        return AceInnerWrapWantParamsArrayInt64(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsFloatArray(ao)) {
        return AceInnerWrapWantParamsArrayFloat(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsByteArray(ao)) {
        return AceInnerWrapWantParamsArrayByte(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsCharArray(ao)) {
        return AceInnerWrapWantParamsArrayChar(env, jsObject, key, ao);
    } else if (AAFwk::Array::IsDoubleArray(ao)) {
        return AceInnerWrapWantParamsArrayDouble(env, jsObject, key, ao);
    } else {
        return false;
    }
}

napi_value AceWrapWantParams(napi_env env, const AAFwk::WantParams& wantParams)
{
    HILOG_INFO("%{public}s called.", __func__);
    napi_value jsObject = nullptr;
    NAPI_CALL(env, napi_create_object(env, &jsObject));

    napi_value jsValue = nullptr;
    const std::map<std::string, sptr<AAFwk::IInterface>> paramList = wantParams.GetParams();
    for (auto iter = paramList.begin(); iter != paramList.end(); iter++) {
        jsValue = nullptr;
        if (AAFwk::IString::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsString(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IBoolean::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsBool(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IShort::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsShort(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IInteger::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsInt32(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::ILong::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsInt64(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IFloat::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsFloat(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IDouble::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsDouble(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IChar::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsChar(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IByte::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsByte(env, jsObject, iter->first, wantParams);
        } else if (AAFwk::IArray::Query(iter->second) != nullptr) {
            AAFwk::IArray *ao = AAFwk::IArray::Query(iter->second);
            if (ao != nullptr) {
                sptr<AAFwk::IArray> array(ao);
                AceInnerWrapWantParamsArray(env, jsObject, iter->first, array);
            }
        } else if (AAFwk::IWantParams::Query(iter->second) != nullptr) {
            AceInnerWrapWantParamsWantParams(env, jsObject, iter->first, wantParams);
        }
    }
    return jsObject;
}

bool AceInnerSetWantParamsArrayString(
    const std::string& key, const std::vector<std::string>& value, AAFwk::WantParams& wantParams)
{
    size_t size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IString);
    if (ao != nullptr) {
        for (size_t i = 0; i < size; i++) {
            ao->Set(i, AAFwk::String::Box(value[i]));
        }
        wantParams.SetParam(key, ao);
        return true;
    } else {
        return false;
    }
}

bool AceInnerSetWantParamsArrayInt(const std::string& key, const std::vector<int>& value,
    AAFwk::WantParams& wantParams)
{
    size_t size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IInteger);
    if (ao != nullptr) {
        for (size_t i = 0; i < size; i++) {
            ao->Set(i, AAFwk::Integer::Box(value[i]));
        }
        wantParams.SetParam(key, ao);
        return true;
    } else {
        return false;
    }
}

bool AceInnerSetWantParamsArrayLong(const std::string& key, const std::vector<int64_t>& value,
    AAFwk::WantParams& wantParams)
{
    size_t size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_ILong);
    if (ao != nullptr) {
        for (size_t i = 0; i < size; i++) {
            ao->Set(i, AAFwk::Long::Box(value[i]));
        }
        wantParams.SetParam(key, ao);
        return true;
    } else {
        return false;
    }
}

bool AceInnerSetWantParamsArrayBool(const std::string& key, const std::vector<bool>& value,
    AAFwk::WantParams& wantParams)
{
    size_t size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IBoolean);
    if (ao != nullptr) {
        for (size_t i = 0; i < size; i++) {
            ao->Set(i, AAFwk::Boolean::Box(value[i]));
        }
        wantParams.SetParam(key, ao);
        return true;
    } else {
        return false;
    }
}

bool AceInnerSetWantParamsArrayDouble(
    const std::string& key, const std::vector<double>& value, AAFwk::WantParams& wantParams)
{
    size_t size = value.size();
    sptr<AAFwk::IArray> ao = new (std::nothrow) AAFwk::Array(size, AAFwk::g_IID_IDouble);
    if (ao != nullptr) {
        for (size_t i = 0; i < size; i++) {
            ao->Set(i, AAFwk::Double::Box(value[i]));
        }
        wantParams.SetParam(key, ao);
        return true;
    } else {
        return false;
    }
}

bool AceInnerUnwrapWantParamsArray(napi_env env, const std::string& key, napi_value param,
    AAFwk::WantParams& wantParams)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEComplexArrayData natArrayValue;
    if (!AceUnwrapArrayComplexFromJS(env, param, natArrayValue)) {
        return false;
    }

    if (natArrayValue.stringList.size() > 0) {
        return AceInnerSetWantParamsArrayString(key, natArrayValue.stringList, wantParams);
    }
    if (natArrayValue.intList.size() > 0) {
        return AceInnerSetWantParamsArrayInt(key, natArrayValue.intList, wantParams);
    }
    if (natArrayValue.longList.size() > 0) {
        return AceInnerSetWantParamsArrayLong(key, natArrayValue.longList, wantParams);
    }
    if (natArrayValue.boolList.size() > 0) {
        return AceInnerSetWantParamsArrayBool(key, natArrayValue.boolList, wantParams);
    }
    if (natArrayValue.doubleList.size() > 0) {
        return AceInnerSetWantParamsArrayDouble(key, natArrayValue.doubleList, wantParams);
    }

    return false;
}

bool AceInnerUnwrapWantParams(napi_env env, const std::string& key, napi_value param,
    AAFwk::WantParams& wantParams)
{
    HILOG_INFO("%{public}s called.", __func__);
    AAFwk::WantParams wp;

    if (AceUnwrapWantParams(env, param, wp)) {
        sptr<AAFwk::IWantParams> pWantParams = AAFwk::WantParamWrapper::Box(wp);
        if (pWantParams != nullptr) {
            wantParams.SetParam(key, pWantParams);
            return true;
        }
    }
    return false;
}

void AceUnwrapParamsNumber(napi_env env, napi_value jsProValue, const std::string& key,
    AAFwk::WantParams& wantParams)
{
    int32_t natValue32 = 0;
    double natValueDouble = 0.0;
    bool isReadValue32 = false;
    bool isReadDouble = false;
    if (napi_get_value_int32(env, jsProValue, &natValue32) == napi_ok) {
        HILOG_INFO("%{public}s called. Property value=%{public}d.", __func__, natValue32);
        isReadValue32 = true;
    }

    if (napi_get_value_double(env, jsProValue, &natValueDouble) == napi_ok) {
        HILOG_INFO("%{public}s called. Property value=%{public}lf.", __func__, natValueDouble);
        isReadDouble = true;
    }

    if (isReadValue32 && isReadDouble) {
        if (abs(natValueDouble - natValue32 * 1.0) > 0.0) {
            wantParams.SetParam(key, AAFwk::Double::Box(natValueDouble));
        } else {
            wantParams.SetParam(key, AAFwk::Integer::Box(natValue32));
        }
    } else if (isReadValue32) {
        wantParams.SetParam(key, AAFwk::Integer::Box(natValue32));
    } else if (isReadDouble) {
        wantParams.SetParam(key, AAFwk::Double::Box(natValueDouble));
    }
}

bool AceUnwrapWantParams(napi_env env, napi_value param, AAFwk::WantParams& wantParams)
{
    HILOG_INFO("%{public}s called.", __func__);

    if (!AceIsTypeForNapiValue(env, param, napi_object)) {
        return false;
    }

    napi_valuetype jsValueType = napi_undefined;
    napi_value jsProNameList = nullptr;
    uint32_t jsProCount = 0;

    NAPI_CALL_BASE(env, napi_get_property_names(env, param, &jsProNameList), false);
    NAPI_CALL_BASE(env, napi_get_array_length(env, jsProNameList, &jsProCount), false);
    HILOG_INFO("%{public}s called. Property size=%{public}d.", __func__, jsProCount);

    napi_value jsProName = nullptr;
    napi_value jsProValue = nullptr;
    for (uint32_t index = 0; index < jsProCount; index++) {
        NAPI_CALL_BASE(env, napi_get_element(env, jsProNameList, index, &jsProName), false);

        std::string strProName = AceUnwrapStringFromJS(env, jsProName);
        HILOG_INFO("%{public}s called. Property name=%{public}s.", __func__, strProName.c_str());
        NAPI_CALL_BASE(env, napi_get_named_property(env, param, strProName.c_str(), &jsProValue), false);
        NAPI_CALL_BASE(env, napi_typeof(env, jsProValue, &jsValueType), false);

        switch (jsValueType) {
            case napi_string: {
                std::string natValue = AceUnwrapStringFromJS(env, jsProValue);
                wantParams.SetParam(strProName, AAFwk::String::Box(natValue));
                break;
            }
            case napi_boolean: {
                bool natValue = false;
                NAPI_CALL_BASE(env, napi_get_value_bool(env, jsProValue, &natValue), false);
                wantParams.SetParam(strProName, AAFwk::Boolean::Box(natValue));
                break;
            }
            case napi_number:
                AceUnwrapParamsNumber(env, jsProValue, strProName, wantParams);
                break;
            case napi_object: {
                bool isArray = false;
                if (napi_is_array(env, jsProValue, &isArray) == napi_ok) {
                    isArray ? AceInnerUnwrapWantParamsArray(env, strProName, jsProValue, wantParams) :
                        AceInnerUnwrapWantParams(env, strProName, jsProValue, wantParams);
                }
                break;
            }
            default:
                break;
        }
    }
    return true;
}

napi_value AceInnerWrapWantOptions(napi_env env, const Want& want)
{
    HILOG_INFO("%{public}s called.", __func__);
    napi_value jsObject = nullptr;
    std::map<std::string, unsigned int> flagMap;
    AceInnerInitWantOptionsData(flagMap);
    unsigned int flags = want.GetFlags();
    bool natValue = false;
    napi_value jsValue = nullptr;

    NAPI_CALL(env, napi_create_object(env, &jsObject));
    for (auto iter = flagMap.begin(); iter != flagMap.end(); iter++) {
        jsValue = nullptr;
        natValue = ((flags & iter->second) == iter->second);
        if (napi_get_boolean(env, natValue, &jsValue) == napi_ok) {
            AceSetPropertyValueByPropertyName(env, jsObject, iter->first.c_str(), jsValue);
        }
    }

    return jsObject;
}

bool AceInnerUnwrapWantOptions(napi_env env, napi_value param, const char *propertyName, Want& want)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, param, propertyName, napi_object);
    if (jsValue == nullptr) {
        return false;
    }

    bool natValue = false;
    unsigned int flags = 0;
    std::map<std::string, unsigned int> flagMap;
    AceInnerInitWantOptionsData(flagMap);
    for (auto iter = flagMap.begin(); iter != flagMap.end(); iter++) {
        natValue = false;
        if (AceUnwrapBooleanByPropertyName(env, jsValue, iter->first.c_str(), natValue)) {
            if (natValue) {
                flags |= iter->second;
            }
        }
    }

    want.SetFlags(flags);
    return true;
}

napi_value AceWrapWant(napi_env env, const Want& want)
{
    HILOG_INFO("%{public}s called.", __func__);
    napi_value jsObject = nullptr;
    napi_value jsValue = nullptr;

    NAPI_CALL(env, napi_create_object(env, &jsObject));

    napi_value jsElementName = AceWrapElementName(env, want.GetElement());
    if (jsElementName == nullptr) {
        HILOG_INFO("%{public}s called. Invoke AceWrapElementName failed.", __func__);
        return nullptr;
    }

    jsValue = AceGetPropertyValueByPropertyName(env, jsElementName, "deviceId", napi_string);
    AceSetPropertyValueByPropertyName(env, jsObject, "deviceId", jsValue);

    jsValue = nullptr;
    jsValue = AceGetPropertyValueByPropertyName(env, jsElementName, "bundleName", napi_string);
    AceSetPropertyValueByPropertyName(env, jsObject, "bundleName", jsValue);

    jsValue = nullptr;
    jsValue = AceGetPropertyValueByPropertyName(env, jsElementName, "abilityName", napi_string);
    AceSetPropertyValueByPropertyName(env, jsObject, "abilityName", jsValue);

    jsValue = nullptr;
    jsValue = AceWrapStringToJS(env, want.GetUriString());
    AceSetPropertyValueByPropertyName(env, jsObject, "uri", jsValue);

    jsValue = nullptr;
    jsValue = AceWrapStringToJS(env, want.GetType());
    AceSetPropertyValueByPropertyName(env, jsObject, "type", jsValue);

    jsValue = nullptr;
    jsValue = AceWrapInt32ToJS(env, want.GetFlags());
    AceSetPropertyValueByPropertyName(env, jsObject, "flags", jsValue);

    jsValue = nullptr;
    jsValue = AceWrapStringToJS(env, want.GetAction());
    AceSetPropertyValueByPropertyName(env, jsObject, "action", jsValue);

    jsValue = nullptr;
    jsValue = AceWrapWantParams(env, want.GetParams());
    AceSetPropertyValueByPropertyName(env, jsObject, "parameters", jsValue);

    jsValue = nullptr;
    jsValue = AceWrapArrayStringToJS(env, want.GetEntities());
    AceSetPropertyValueByPropertyName(env, jsObject, "entities", jsValue);

    return jsObject;
}

bool AceUnwrapWant(napi_env env, napi_value param, Want& want)
{
    HILOG_INFO("%{public}s called.", __func__);

    if (!AceIsTypeForNapiValue(env, param, napi_object)) {
        HILOG_INFO("%{public}s called. Params is invalid.", __func__);
        return false;
    }

    napi_value jsValue = AceGetPropertyValueByPropertyName(env, param, "parameters", napi_object);
    if (jsValue != nullptr) {
        AAFwk::WantParams wantParams;
        if (AceUnwrapWantParams(env, jsValue, wantParams)) {
            want.SetParams(wantParams);
        }
    }

    std::string natValueString("");
    if (AceUnwrapStringByPropertyName(env, param, "action", natValueString)) {
        want.SetAction(natValueString);
    }

    std::vector<std::string> natValueStringList;
    if (AceUnwrapStringArrayByPropertyName(env, param, "entities", natValueStringList)) {
        for (size_t i = 0; i < natValueStringList.size(); i++) {
            want.AddEntity(natValueStringList[i]);
        }
    }

    natValueString = "";
    if (AceUnwrapStringByPropertyName(env, param, "uri", natValueString)) {
        want.SetUri(natValueString);
    }

    int32_t flags = 0;
    if (AceUnwrapInt32ByPropertyName(env, param, "flags", flags)) {
        want.SetFlags(flags);
    }

    ElementName natElementName;
    AceUnwrapElementName(env, param, natElementName);
    want.SetElementName(natElementName.GetDeviceID(), natElementName.GetBundleName(), natElementName.GetAbilityName());

    natValueString = "";
    if (AceUnwrapStringByPropertyName(env, param, "type", natValueString)) {
        want.SetType(natValueString);
    }

    return true;
}
}  // namespace OHOS::Ace::Napi
