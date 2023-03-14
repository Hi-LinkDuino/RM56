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

#ifndef OHOS_NAPI_ACE_PLUGIN_UTIL_H
#define OHOS_NAPI_ACE_PLUGIN_UTIL_H

#include "js_plugin_data.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS::Ace::Napi {
bool AceIsTypeForNapiValue(napi_env env, napi_value param, napi_valuetype expectType);
bool AceIsArrayForNapiValue(napi_env env, napi_value param, uint32_t& arraySize);

napi_value AceWrapVoidToJS(napi_env env);
napi_value AceWrapUndefinedToJS(napi_env env);

napi_value AceCreateJSObject(napi_env env);

napi_value AceWrapInt32ToJS(napi_env env, int32_t value);
int AceUnwrapInt32FromJS(napi_env env, napi_value param, int defaultValue = 0);
bool AceUnwrapInt32FromJS2(napi_env env, napi_value param, int& value);

napi_value AceWrapLongToJS(napi_env env, int64_t value);
int64_t AceUnwrapLongFromJS(napi_env env, napi_value param, int64_t defaultValue = 0);
bool AceUnwrapLongFromJS2(napi_env env, napi_value param, int64_t& value);

napi_value AceWrapInt64ToJS(napi_env env, int64_t value);
int64_t AceUnwrapInt64FromJS(napi_env env, napi_value param, int64_t defaultValue = 0);
bool AceUnwrapInt64FromJS2(napi_env env, napi_value param, int64_t& value);

napi_value AceWrapBoolToJS(napi_env env, bool value);
bool AceUnWrapBoolFromJS(napi_env env, napi_value param, bool defaultValue = false);
bool AceUnwrapBoolFromJS2(napi_env env, napi_value param, bool& value);

napi_value AceWrapDoubleToJS(napi_env env, double value);
double AceUnWrapDoubleFromJS(napi_env env, napi_value param, double defaultValue = 0.0);
bool AceUnWrapDoubleFromJS2(napi_env env, napi_value param, double& value);

napi_value AceWrapStringToJS(napi_env env, const std::string& value);
std::string AceUnwrapStringFromJS(napi_env env, napi_value param, const std::string& defaultValue = "");
bool AceUnwrapStringFromJS2(napi_env env, napi_value param, std::string& value);

napi_value AceWrapArrayInt32ToJS(napi_env env, const std::vector<int>& value);
bool AceUnwrapArrayInt32FromJS(napi_env env, napi_value param, std::vector<int>& value);

napi_value AceWrapArrayLongToJS(napi_env env, const std::vector<int64_t>& value);
bool AceUnwrapArrayLongFromJS(napi_env env, napi_value param, std::vector<int64_t>& value);

napi_value AceWrapArrayInt64ToJS(napi_env env, const std::vector<int64_t>& value);
bool AceUnwrapArrayInt64FromJS(napi_env env, napi_value param, std::vector<int64_t>& value);

napi_value AceWrapArrayDoubleToJS(napi_env env, const std::vector<double>& value);
bool AceUnwrapArrayDoubleFromJS(napi_env env, napi_value param, std::vector<double>& value);

napi_value AceWrapArrayBoolToJS(napi_env env, const std::vector<bool>& value);
bool AceUnwrapArrayBoolFromJS(napi_env env, napi_value param, std::vector<bool>& value);

napi_value AceWrapArrayStringToJS(napi_env env, const std::vector<std::string>& value);
bool AceUnwrapArrayStringFromJS(napi_env env, napi_value param, std::vector<std::string>& value);

bool AceKVObjectToString(napi_env env, napi_value param, std::string& value);
napi_value AceStringToKVObject(napi_env env, const std::string& jsonString);

bool AceUnwrapArrayComplexFromJS(napi_env env, napi_value param, ACEComplexArrayData& value);

bool AceIsSameFuncFromJS(ACECallbackInfo& left, ACECallbackInfo& right);
/**
 * @brief Indicates the specified attribute exists in the object passed by JS.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 *
 * @return Returns true if the attribute exists, else returns false.
 */
bool AceIsExistsByPropertyName(napi_env env, napi_value jsObject, const char* propertyName);

/**
 * @brief Get the JSValue of the specified name from the JS object.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param jsObject Indicates object passed by JS.
 * @param propertyName Indicates the name of the property.
 * @param expectType Indicates expected JS data type.
 *
 * @return Return the property value of the specified property name int jsObject on success, otherwise return nullptr.
 */
napi_value AceGetPropertyValueByPropertyName(
    napi_env env, napi_value jsObject, const char* propertyName, napi_valuetype expectType);

bool AceSetPropertyValueByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, napi_value value);

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
bool AceUnwrapInt32ByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, int32_t& value);

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
bool AceUnwrapDoubleByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, double& value);

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
bool AceUnwrapBooleanByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, bool& value);
bool AceUnwrapBooleanArrayByPropertyName(
    napi_env env, napi_value jsObject, const char* propertyName, std::vector<bool>& value);

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
bool AceUnwrapStringByPropertyName(napi_env env, napi_value jsObject, const char* propertyName, std::string& value);
bool AceUnwrapStringArrayByPropertyName(
    napi_env env, napi_value jsObject, const char* propertyName, std::vector<std::string>& value);

void AceClearThreadReturnData(ACEThreadReturnData* data);

napi_value AceGetCallbackErrorValue(napi_env env, int errCode);

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 *
 * @return Return a pointer to ACEAsyncJSCallbackInfo on success, nullptr on failure
 */
ACEAsyncJSCallbackInfo* AceCreateAsyncJSCallbackInfo(napi_env env);
void AceFreeAsyncJSCallbackInfo(ACEAsyncJSCallbackInfo** asyncCallbackInfo);

/**
 * @brief Convert local data to JS data.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data The local data.
 * @param value the JS data.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
bool AceWrapThreadReturnData(napi_env env, const ACEThreadReturnData* data, napi_value* value);

/**
 * @brief Create asynchronous data.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param param Parameter list.
 * @param callback Point to asynchronous processing of data.
 *
 * @return Return true successfully, otherwise return false.
 */
bool AceCreateAsyncCallback(napi_env env, napi_value param, ACEAsyncJSCallbackInfo* callback);

napi_ref AceCreateCallbackRefFromJS(napi_env env, napi_value param);

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
    const ACEAsyncParamEx* param);

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
    const ACEAsyncParamEx* param);

/**
 * @brief The callback at the end of the asynchronous callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void AceCompleteAsyncCallbackWork(napi_env env, napi_status status, void* data);

/**
 * @brief The callback at the end of the Promise callback.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param data Point to asynchronous processing of data.
 */
void AceCompletePromiseCallbackWork(napi_env env, napi_status status, void* data);
}  // namespace OHOS::Ace::Napi
#endif  // OHOS_NAPI_ACE_PLUGIN_UTIL_H
