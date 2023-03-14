/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "js_plugin_util.h"

#include <cinttypes>
#include <cstring>

#include "hilog_wrapper.h"
#include "napi/native_engine/native_value.h"
#include "securec.h"

namespace OHOS::Ace::Napi {
constexpr int NAPI_ACE_ERR_NO_ERROR = 0;
constexpr int ACE_ARGS_TWO = 2;
constexpr int ACE_PARAM0 = 0;
constexpr int ACE_PARAM1 = 1;

bool AceToJson(napi_env env, napi_value param, Json::Value& jsonObject);
napi_value ParseJsonToKVObject(napi_env env, Json::Value& jsonObject);
napi_value ParseJsonItem(napi_env env, Json::Value& item);

bool AceIsTypeForNapiValue(napi_env env, napi_value param, napi_valuetype expectType)
{
    napi_valuetype valueType = napi_undefined;

    if (napi_typeof(env, param, &valueType) != napi_ok) {
        return false;
    }

    return valueType == expectType;
}

bool AceIsArrayForNapiValue(napi_env env, napi_value param, uint32_t& arraySize)
{
    bool isArray = false;
    arraySize = 0;

    if (napi_is_array(env, param, &isArray) != napi_ok || isArray == false) {
        return false;
    }

    if (napi_get_array_length(env, param, &arraySize) != napi_ok) {
        return false;
    }
    return true;
}

napi_value AceWrapVoidToJS(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

napi_value AceWrapUndefinedToJS(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value AceCreateJSObject(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_object(env, &result));
    return result;
}

napi_value AceWrapInt32ToJS(napi_env env, int32_t value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, value, &result));
    return result;
}

int AceUnwrapInt32FromJS(napi_env env, napi_value param, int defaultValue)
{
    int value = defaultValue;
    if (napi_get_value_int32(env, param, &value) == napi_ok) {
        return value;
    } else {
        return defaultValue;
    }
}

bool AceUnwrapInt32FromJS2(napi_env env, napi_value param, int& value)
{
    bool result = false;
    if (napi_get_value_int32(env, param, &value) == napi_ok) {
        result = true;
    }
    return result;
}

napi_value AceWrapLongToJS(napi_env env, int64_t value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, value, &result));
    return result;
}

int64_t AceUnwrapLongFromJS(napi_env env, napi_value param, int64_t defaultValue)
{
    int value = 0;
    if (napi_get_value_int32(env, param, &value) == napi_ok) {
        return value;
    } else {
        return defaultValue;
    }
}

bool AceUnwrapLongFromJS2(napi_env env, napi_value param, int64_t& value)
{
    bool result = false;
    int natValue = 0;
    if (napi_get_value_int32(env, param, &natValue) == napi_ok) {
        value = natValue;
        result = true;
    }
    return result;
}

napi_value AceWrapInt64ToJS(napi_env env, int64_t value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int64(env, value, &result));
    return result;
}

int64_t AceUnwrapInt64FromJS(napi_env env, napi_value param, int64_t defaultValue)
{
    int64_t value = defaultValue;
    if (napi_get_value_int64(env, param, &value) == napi_ok) {
        return value;
    } else {
        return defaultValue;
    }
}

bool AceUnwrapInt64FromJS2(napi_env env, napi_value param, int64_t& value)
{
    bool result = false;
    if (napi_get_value_int64(env, param, &value) == napi_ok) {
        result = true;
    }
    return result;
}

napi_value AceWrapBoolToJS(napi_env env, bool value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, value, &result));
    return result;
}

bool AceUnwrapBoolFromJS(napi_env env, napi_value param, bool defaultValue)
{
    bool value = defaultValue;
    if (napi_get_value_bool(env, param, &value) == napi_ok) {
        return value;
    } else {
        return defaultValue;
    }
}

bool AceUnwrapBoolFromJS2(napi_env env, napi_value param, bool& value)
{
    bool result = false;
    if (napi_get_value_bool(env, param, &value) == napi_ok) {
        result = true;
    }
    return result;
}

napi_value AceWrapDoubleToJS(napi_env env, double value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_double(env, value, &result));
    return result;
}

double AceUnwrapDoubleFromJS(napi_env env, napi_value param, double defaultValue)
{
    double value = defaultValue;
    if (napi_get_value_double(env, param, &value) == napi_ok) {
        return value;
    } else {
        return defaultValue;
    }
}

bool AceUnwrapDoubleFromJS2(napi_env env, napi_value param, double& value)
{
    bool result = false;
    if (napi_get_value_double(env, param, &value) == napi_ok) {
        result = true;
    }
    return result;
}

napi_value AceWrapStringToJS(napi_env env, const std::string& value)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &result));
    return result;
}

std::string AceUnwrapStringFromJS(napi_env env, napi_value param, const std::string& defaultValue)
{
    size_t size = 0;
    if (napi_get_value_string_utf8(env, param, nullptr, 0, &size) != napi_ok) {
        return defaultValue;
    }

    std::string value("");
    if (size == 0) {
        return defaultValue;
    }

    char* buf = new (std::nothrow) char[size + 1];
    if (buf == nullptr) {
        return value;
    }
    if (memset_s(buf, size + 1, 0, size + 1) != EOK) {
        return value;
    }

    bool rev = napi_get_value_string_utf8(env, param, buf, size + 1, &size) == napi_ok;
    if (rev) {
        value = buf;
    } else {
        value = defaultValue;
    }

    delete[] buf;
    buf = nullptr;
    return value;
}

bool AceUnwrapStringFromJS2(napi_env env, napi_value param, std::string& value)
{
    value = "";
    size_t size = 0;
    if (napi_get_value_string_utf8(env, param, nullptr, 0, &size) != napi_ok) {
        return false;
    }

    if (size == 0) {
        return true;
    }

    char* buf = new (std::nothrow) char[size + 1];
    if (buf == nullptr) {
        return false;
    }
    if (memset_s(buf, (size + 1), 0, (size + 1)) != EOK) {
        return false;
    }

    bool rev = napi_get_value_string_utf8(env, param, buf, size + 1, &size) == napi_ok;
    if (rev) {
        value = buf;
    }
    delete[] buf;
    buf = nullptr;
    return rev;
}

napi_value AceWrapArrayInt32ToJS(napi_env env, const std::vector<int>& value)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = 0;

    NAPI_CALL(env, napi_create_array(env, &jsArray));
    for (uint32_t i = 0; i < value.size(); i++) {
        jsValue = nullptr;
        if (napi_create_int32(env, value[i], &jsValue) == napi_ok) {
            if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
                index++;
            }
        }
    }
    return jsArray;
}

bool AceUnwrapArrayInt32FromJS(napi_env env, napi_value param, std::vector<int>& value)
{
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    int natValue = 0;

    if (!AceIsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        natValue = 0;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!AceUnwrapInt32FromJS2(env, jsValue, natValue)) {
            return false;
        }

        value.push_back(natValue);
    }
    return true;
}

napi_value AceWrapArrayLongToJS(napi_env env, const std::vector<int64_t>& value)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = 0;

    NAPI_CALL(env, napi_create_array(env, &jsArray));
    for (uint32_t i = 0; i < value.size(); i++) {
        jsValue = nullptr;
        if (napi_create_int32(env, (int)(value[i]), &jsValue) == napi_ok) {
            if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
                index++;
            }
        }
    }
    return jsArray;
}

bool AceUnwrapArrayLongFromJS(napi_env env, napi_value param, std::vector<int64_t>& value)
{
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    int64_t natValue = 0;

    if (!AceIsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        natValue = 0;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!AceUnwrapLongFromJS2(env, jsValue, natValue)) {
            return false;
        }

        value.push_back(natValue);
    }
    return true;
}


napi_value AceWrapArrayInt64ToJS(napi_env env, const std::vector<int64_t>& value)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = 0;

    NAPI_CALL(env, napi_create_array(env, &jsArray));
    for (uint32_t i = 0; i < value.size(); i++) {
        jsValue = nullptr;
        if (napi_create_int64(env, value[i], &jsValue) == napi_ok) {
            if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
                index++;
            }
        }
    }
    return jsArray;
}

bool AceUnwrapArrayInt64FromJS(napi_env env, napi_value param, std::vector<int64_t>& value)
{
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    int64_t natValue = 0;

    if (!AceIsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        natValue = 0;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!AceUnwrapInt64FromJS2(env, jsValue, natValue)) {
            return false;
        }

        value.push_back(natValue);
    }
    return true;
}

napi_value AceWrapArrayDoubleToJS(napi_env env, const std::vector<double>& value)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = 0;

    NAPI_CALL(env, napi_create_array(env, &jsArray));
    for (uint32_t i = 0; i < value.size(); i++) {
        jsValue = nullptr;
        if (napi_create_double(env, value[i], &jsValue) == napi_ok) {
            if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
                index++;
            }
        }
    }
    return jsArray;
}

bool AceUnwrapArrayDoubleFromJS(napi_env env, napi_value param, std::vector<double>& value)
{
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    double natValue = 0;

    if (!AceIsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        natValue = 0;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!AceUnwrapDoubleFromJS2(env, jsValue, natValue)) {
            return false;
        }

        value.push_back(natValue);
    }
    return true;
}

napi_value AceWrapArrayBoolToJS(napi_env env, const std::vector<bool>& value)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = 0;

    NAPI_CALL(env, napi_create_array(env, &jsArray));
    for (uint32_t i = 0; i < value.size(); i++) {
        jsValue = nullptr;
        if (napi_get_boolean(env, value[i], &jsValue) == napi_ok) {
            if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
                index++;
            }
        }
    }
    return jsArray;
}

bool AceUnwrapArrayBoolFromJS(napi_env env, napi_value param, std::vector<bool>& value)
{
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    bool natValue = 0;

    if (!AceIsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        natValue = 0;
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!AceUnwrapBoolFromJS2(env, jsValue, natValue)) {
            return false;
        }

        value.push_back(natValue);
    }
    return true;
}

napi_value AceWrapArrayStringToJS(napi_env env, const std::vector<std::string>& value)
{
    napi_value jsArray = nullptr;
    napi_value jsValue = nullptr;
    uint32_t index = 0;

    NAPI_CALL(env, napi_create_array(env, &jsArray));
    for (uint32_t i = 0; i < value.size(); i++) {
        jsValue = nullptr;
        if (napi_create_string_utf8(env, value[i].c_str(), NAPI_AUTO_LENGTH, &jsValue) == napi_ok) {
            if (napi_set_element(env, jsArray, index, jsValue) == napi_ok) {
                index++;
            }
        }
    }
    return jsArray;
}

bool AceUnwrapArrayStringFromJS(napi_env env, napi_value param, std::vector<std::string>& value)
{
    uint32_t arraySize = 0;
    napi_value jsValue = nullptr;
    std::string natValue("");

    if (!AceIsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    value.clear();
    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        natValue = "";
        if (napi_get_element(env, param, i, &jsValue) != napi_ok) {
            return false;
        }

        if (!AceUnwrapStringFromJS2(env, jsValue, natValue)) {
            return false;
        }

        value.push_back(natValue);
    }
    return true;
}

bool AceKVObjectToString(napi_env env, napi_value param, std::string& value)
{
    if (!AceIsTypeForNapiValue(env, param, napi_object)) {
        return false;
    }

    napi_value globalValue;
    napi_get_global(env, &globalValue);
    napi_value jsonValue;
    napi_get_named_property(env, globalValue, "JSON", &jsonValue);
    napi_value stringifyValue;
    napi_get_named_property(env, jsonValue, "stringify", &stringifyValue);
    napi_value funcArgv[1] = { param };
    napi_value returnValue;
    napi_call_function(env, jsonValue, stringifyValue, 1, funcArgv, &returnValue);
    auto nativeValue = reinterpret_cast<NativeValue*>(returnValue);
    auto resultValue = nativeValue->ToString();
    auto nativeString = reinterpret_cast<NativeString*>(resultValue->GetInterface(NativeString::INTERFACE_ID));
    size_t len = nativeString->GetLength() + 1;
    std::unique_ptr<char[]> paramsChar = std::make_unique<char[]>(len);
    size_t ret = 0;
    napi_get_value_string_utf8(env, returnValue, paramsChar.get(), len, &ret);
    value = paramsChar.get();
    return true;
}

napi_value AceStringToKVObject(napi_env env, const std::string& jsonString)
{
    if (jsonString.empty()) {
        return nullptr;
    }

    napi_value globalValue;
    napi_get_global(env, &globalValue);
    napi_value jsonValue;
    napi_get_named_property(env, globalValue, "JSON", &jsonValue);
    napi_value parseValue;
    napi_get_named_property(env, jsonValue, "parse", &parseValue);
    napi_value gridInfoNApi;
    napi_create_string_utf8(env, jsonString.c_str(), NAPI_AUTO_LENGTH, &gridInfoNApi);
    napi_value funcArgv[1] = { gridInfoNApi };
    napi_value result;
    napi_call_function(env, jsonValue, parseValue, 1, funcArgv, &result);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, result, &valueType);
    if (valueType != napi_object) {
        return nullptr;
    }
    return result;
}

void AceInitComplexArrayData(ACEComplexArrayData& value)
{
    value.intList.clear();
    value.longList.clear();
    value.boolList.clear();
    value.doubleList.clear();
    value.stringList.clear();
}

void AceTranscomplex(ACEComplexArrayData& value)
{
    if (value.intList.size() > 0) {
        for (size_t j = 0; j < value.intList.size(); j++) {
            value.doubleList.push_back(value.intList[j]);
        }
        value.intList.clear();
    }
}

void AceUnwrapNumberComplexFromJS(napi_env env, napi_value jsValue, bool& isDouble, ACEComplexArrayData& value)
{
    int32_t elementInt32 = 0;
    double elementDouble = 0.0;
    if (isDouble) {
        if (napi_get_value_double(env, jsValue, &elementDouble) == napi_ok) {
            value.doubleList.push_back(elementDouble);
        }
    } else {
        bool isReadValue32 = napi_get_value_int32(env, jsValue, &elementInt32) == napi_ok;
        bool isReadDouble = napi_get_value_double(env, jsValue, &elementDouble) == napi_ok;
        if (isReadValue32 && isReadDouble) {
            if (abs(elementDouble - elementInt32 * 1.0) > 0.0) {
                isDouble = true;
                AceTranscomplex(value);
                value.doubleList.push_back(elementDouble);
            } else {
                value.intList.push_back(elementInt32);
            }
        } else if (isReadValue32) {
            value.intList.push_back(elementInt32);
        } else if (isReadDouble) {
            isDouble = true;
            AceTranscomplex(value);
            value.doubleList.push_back(elementDouble);
        }
    }
}

bool AceUnwrapArrayComplexFromJS(napi_env env, napi_value param, ACEComplexArrayData& value)
{
    uint32_t arraySize = 0;
    if (!AceIsArrayForNapiValue(env, param, arraySize)) {
        return false;
    }

    AceInitComplexArrayData(value);
    napi_valuetype valueType = napi_undefined;
    napi_value jsValue = nullptr;
    bool isDouble = false;

    for (uint32_t i = 0; i < arraySize; i++) {
        jsValue = nullptr;
        valueType = napi_undefined;
        NAPI_CALL_BASE(env, napi_get_element(env, param, i, &jsValue), false);
        NAPI_CALL_BASE(env, napi_typeof(env, jsValue, &valueType), false);
        switch (valueType) {
            case napi_string: {
                std::string elementValue("");
                if (AceUnwrapStringFromJS2(env, jsValue, elementValue)) {
                    value.stringList.push_back(elementValue);
                } else {
                    return false;
                }
                break;
            }
            case napi_boolean: {
                bool elementValue = false;
                NAPI_CALL_BASE(env, napi_get_value_bool(env, jsValue, &elementValue), false);
                value.boolList.push_back(elementValue);
                break;
            }
            case napi_number:
                AceUnwrapNumberComplexFromJS(env, jsValue, isDouble, value);
                break;
            default:
                break;
        }
    }
    return true;
}

bool AceIsSameFuncFromJS(ACECallbackInfo& left, ACECallbackInfo& right)
{
    if (left.env == nullptr && right.env == nullptr) {
        return true;
    }
    if (left.env != right.env) {
        return false;
    }

    bool result = false;
    napi_value leftFunc = nullptr;
    napi_value rightFunc = nullptr;
    napi_get_reference_value(left.env, left.callback, &leftFunc);
    napi_get_reference_value(right.env, right.callback, &rightFunc);
    napi_strict_equals(left.env, leftFunc, rightFunc, &result);
    return result;
}

/**
 * @brief Indicates the specified attribute exists in the object passed by JS.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 *
 * @return Returns true if the attribute exists, else returns false.
 */
bool AceIsExistsByPropertyName(napi_env env, napi_value jsObject, const char* propertyName)
{
    bool result = false;
    if (napi_has_named_property(env, jsObject, propertyName, &result) == napi_ok) {
        return result;
    } else {
        return false;
    }
}

/**
 * @brief Get the JSValue of the specified name from the JS object.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 * @param expectType Indicates expected JS data type.
 *
 * @return Return the property value of the specified property name int jsObject on success, otherwise return
 * nullptr.
 */
napi_value AceGetPropertyValueByPropertyName(
    napi_env env, napi_value jsObject, const char* propertyName, napi_valuetype expectType)
{
    napi_value value = nullptr;
    if (AceIsExistsByPropertyName(env, jsObject, propertyName) == false) {
        return nullptr;
    }

    if (napi_get_named_property(env, jsObject, propertyName, &value) != napi_ok) {
        return nullptr;
    }

    if (!AceIsTypeForNapiValue(env, value, expectType)) {
        return nullptr;
    }

    return value;
}

bool AceSetPropertyValueByPropertyName(napi_env env, napi_value jsObject, const char* propertyName,
    napi_value value)
{
    if (value != nullptr && propertyName != nullptr) {
        NAPI_CALL_BASE(env, napi_set_named_property(env, jsObject, propertyName, value), false);
        return true;
    }
    return false;
}

/**
 * @brief Get the native number(int32) from the JSObject of the given property name.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 * @param value Indicates the returned native value.
 *
 * @return Return true if successful, else return false.
 */
bool AceUnwrapInt32ByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, int32_t& value)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, jsObject, propertyName, napi_number);
    if (jsValue != nullptr) {
        return AceUnwrapInt32FromJS2(env, jsValue, value);
    } else {
        return false;
    }
}

/**
 * @brief Get the native number(double) from the JSObject of the given property name.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 * @param value Indicates the returned native value.
 *
 * @return Return true if successful, else return false.
 */
bool AceUnwrapDoubleByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, double& value)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, jsObject, propertyName, napi_number);
    if (jsValue != nullptr) {
        return AceUnwrapDoubleFromJS2(env, jsValue, value);
    } else {
        return false;
    }
}

/**
 * @brief Get the native boolean from the JSObject of the given property name.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 * @param value Indicates the returned native value.
 *
 * @return Return true if successful, else return false.
 */
bool AceUnwrapBooleanByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, bool& value)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, jsObject, propertyName, napi_boolean);
    if (jsValue != nullptr) {
        return AceUnwrapBoolFromJS2(env, jsValue, value);
    } else {
        return false;
    }
}

bool AceUnwrapBooleanArrayByPropertyName(
    napi_env env, napi_value jsObject, const char* propertyName, std::vector<bool>& value)
{
    napi_value jsArray = AceGetPropertyValueByPropertyName(env, jsObject, propertyName, napi_object);
    if (jsArray == nullptr) {
        return false;
    }

    return AceUnwrapArrayBoolFromJS(env, jsArray, value);
}

/**
 * @brief Get the native string from the JSObject of the given property name.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 * @param value Indicates the returned native value.
 *
 * @return Return true if successful, else return false.
 */
bool AceUnwrapStringByPropertyName(napi_env env, napi_value jsObject, const char* propertyName,
    std::string& value)
{
    napi_value jsValue = AceGetPropertyValueByPropertyName(env, jsObject, propertyName, napi_string);
    if (jsValue != nullptr) {
        return AceUnwrapStringFromJS2(env, jsValue, value);
    } else {
        return false;
    }
}

bool AceUnwrapStringArrayByPropertyName(
    napi_env env, napi_value jsObject, const char* propertyName, std::vector<std::string>& value)
{
    napi_value jsArray = AceGetPropertyValueByPropertyName(env, jsObject, propertyName, napi_object);
    if (jsArray == nullptr) {
        return false;
    }

    return AceUnwrapArrayStringFromJS(env, jsArray, value);
}

void AceClearThreadReturnData(ACEThreadReturnData* data)
{
    if (data != nullptr) {
        data->data_type = TACENativeValueType::NVT_NONE;
        data->int32_value = 0;
        data->bool_value = false;
        data->str_value = "";
        data->double_value = 0.0;
    }
}

napi_value AceGetCallbackErrorValue(napi_env env, int errCode)
{
    napi_value jsObject = nullptr;
    napi_value jsValue = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &jsValue));
    NAPI_CALL(env, napi_create_object(env, &jsObject));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "code", jsValue));
    return jsObject;
}

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 *
 * @return Return a pointer to AsyncPermissionCallbackInfo on success, nullptr on failure
 */
ACEAsyncJSCallbackInfo* AceCreateAsyncJSCallbackInfo(napi_env env)
{
    HILOG_INFO("%{public}s called.", __func__);

    napi_value global = 0;
    NAPI_CALL(env, napi_get_global(env, &global));

    napi_value abilityObj = 0;
    NAPI_CALL(env, napi_get_named_property(env, global, "ability", &abilityObj));
    Ability *ability = nullptr;
    if (abilityObj) {
        NAPI_CALL(env, napi_get_value_external(env, abilityObj, (void **)&ability));
    }

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = new (std::nothrow) ACEAsyncJSCallbackInfo {
        .cbInfo = {
            .env = env,
            .callback = nullptr,
        },
        .ability = ability,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };

    if (asyncCallbackInfo != nullptr) {
        AceClearThreadReturnData(&asyncCallbackInfo->native_data);
    }
    return asyncCallbackInfo;
}

void AceFreeAsyncJSCallbackInfo(ACEAsyncJSCallbackInfo** asyncCallbackInfo)
{
    if (asyncCallbackInfo == nullptr) {
        return;
    }
    if (*asyncCallbackInfo == nullptr) {
        return;
    }

    if ((*asyncCallbackInfo)->cbInfo.callback != nullptr && (*asyncCallbackInfo)->cbInfo.env != nullptr) {
        napi_delete_reference((*asyncCallbackInfo)->cbInfo.env, (*asyncCallbackInfo)->cbInfo.callback);
        (*asyncCallbackInfo)->cbInfo.callback = nullptr;
        (*asyncCallbackInfo)->cbInfo.env = nullptr;
    }

    delete (*asyncCallbackInfo);
    *asyncCallbackInfo = nullptr;
}

/**
 * @brief Convert local data to JS data.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data The local data.
 * @param value the JS data.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
bool AceWrapThreadReturnData(napi_env env, const ACEThreadReturnData* data, napi_value* value)
{
    if (data == nullptr || value == nullptr) {
        return false;
    }

    switch (data->data_type) {
        case TACENativeValueType::NVT_UNDEFINED:
            NAPI_CALL_BASE(env, napi_get_undefined(env, value), false);
            break;
        case TACENativeValueType::NVT_INT32:
            NAPI_CALL_BASE(env, napi_create_int32(env, data->int32_value, value), false);
            break;
        case TACENativeValueType::NVT_BOOL:
            NAPI_CALL_BASE(env, napi_get_boolean(env, data->bool_value, value), false);
            break;
        case TACENativeValueType::NVT_STRING:
            NAPI_CALL_BASE(env, napi_create_string_utf8(env, data->str_value.c_str(), NAPI_AUTO_LENGTH, value), false);
            break;
        default:
            NAPI_CALL_BASE(env, napi_get_null(env, value), false);
            break;
    }
    return true;
}

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param param Parameter list.
 * @param callback Point to asynchronous processing of data.
 *
 * @return Return true successfully, otherwise return false.
 */
bool AceCreateAsyncCallback(napi_env env, napi_value param, ACEAsyncJSCallbackInfo* callback)
{
    HILOG_INFO("%{public}s called.", __func__);

    if (param == nullptr || callback == nullptr) {
        HILOG_INFO("%{public}s called, param or callback is null.", __func__);
        return false;
    }

    callback->cbInfo.callback = AceCreateCallbackRefFromJS(env, param);
    if (callback->cbInfo.callback == nullptr) {
        HILOG_INFO("%{public}s, create ref failed.", __func__);
        return false;
    }

    return true;
}

napi_ref AceCreateCallbackRefFromJS(napi_env env, napi_value param)
{
    if (env == nullptr || param == nullptr) {
        HILOG_INFO("%{public}s called, env or param is null", __func__);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, param, &valueType));

    if (valueType != napi_function) {
        HILOG_INFO("%{public}s called, Param is invalid.", __func__);
        return nullptr;
    }

    napi_ref callbackRef = nullptr;
    NAPI_CALL(env, napi_create_reference(env, param, 1, &callbackRef));
    return callbackRef;
}

/**
 * @brief Asynchronous callback processing.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 * @param param other param.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value AceExecuteAsyncCallbackWork(napi_env env, ACEAsyncJSCallbackInfo* asyncCallbackInfo,
    const ACEAsyncParamEx* param)
{
    HILOG_INFO("%{public}s called.", __func__);
    if (asyncCallbackInfo == nullptr || param == nullptr) {
        HILOG_INFO("%{public}s called, asyncCallbackInfo or param is null", __func__);
        return nullptr;
    }

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, param->resource.c_str(), NAPI_AUTO_LENGTH, &resourceName));

    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            param->execute,
            param->complete,
            (void*)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    return AceWrapVoidToJS(env);
}

/**
 * @brief Asynchronous promise processing.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param asyncCallbackInfo Process data asynchronously.
 * @param param other param.
 *
 * @return Return JS data successfully, otherwise return nullptr.
 */
napi_value AceExecutePromiseCallbackWork(napi_env env, ACEAsyncJSCallbackInfo* asyncCallbackInfo,
    const ACEAsyncParamEx* param)
{
    HILOG_INFO("%{public}s called.", __func__);
    if (asyncCallbackInfo == nullptr || param == nullptr) {
        HILOG_INFO("%{public}s called, asyncCallbackInfo or param is null", __func__);
        return nullptr;
    }

    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, param->resource.c_str(), NAPI_AUTO_LENGTH, &resourceName));

    napi_deferred deferred = 0;
    napi_value promise = 0;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    asyncCallbackInfo->deferred = deferred;
    NAPI_CALL(env,
        napi_create_async_work(env,
            nullptr,
            resourceName,
            param->execute,
            param->complete,
            (void*)asyncCallbackInfo,
            &asyncCallbackInfo->asyncWork));

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    return promise;
}

/**
 * @brief The callback at the end of the asynchronous callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void AceCompleteAsyncCallbackWork(napi_env env, napi_status status, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = (ACEAsyncJSCallbackInfo*)data;
    if (asyncCallbackInfo == nullptr) {
        HILOG_INFO("%{public}s called, asyncCallbackInfo is null", __func__);
        return;
    }

    napi_value callback = 0;
    napi_value undefined = 0;
    napi_get_undefined(env, &undefined);
    napi_value callResult = 0;
    napi_value revParam[ACE_ARGS_TWO] = {nullptr};

    revParam[ACE_PARAM0] = AceGetCallbackErrorValue(env, asyncCallbackInfo->error_code);
    AceWrapThreadReturnData(env, &asyncCallbackInfo->native_data, &revParam[ACE_PARAM1]);

    if (asyncCallbackInfo->cbInfo.callback != nullptr) {
        napi_get_reference_value(env, asyncCallbackInfo->cbInfo.callback, &callback);
        napi_call_function(env, undefined, callback, ACE_ARGS_TWO, revParam, &callResult);
        napi_delete_reference(env, asyncCallbackInfo->cbInfo.callback);
    }

    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    delete asyncCallbackInfo;
    asyncCallbackInfo = nullptr;
}

/**
 * @brief The callback at the end of the Promise callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void AceCompletePromiseCallbackWork(napi_env env, napi_status status, void* data)
{
    HILOG_INFO("%{public}s called.", __func__);

    ACEAsyncJSCallbackInfo* asyncCallbackInfo = (ACEAsyncJSCallbackInfo*)data;
    if (asyncCallbackInfo == nullptr) {
        HILOG_INFO("%{public}s called, asyncCallbackInfo is null", __func__);
        return;
    }

    napi_value result = 0;
    if (asyncCallbackInfo->error_code == NAPI_ACE_ERR_NO_ERROR) {
        AceWrapThreadReturnData(env, &asyncCallbackInfo->native_data, &result);
        napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
    } else {
        result = AceGetCallbackErrorValue(env, asyncCallbackInfo->error_code);
        napi_reject_deferred(env, asyncCallbackInfo->deferred, result);
    }
    napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
    delete asyncCallbackInfo;
    asyncCallbackInfo = nullptr;
}
}  // namespace OHOS::Ace::Napi
