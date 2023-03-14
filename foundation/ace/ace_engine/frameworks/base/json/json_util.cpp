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

#include "base/json/json_util.h"

#include "cJSON.h"

namespace OHOS::Ace {

JsonValue::JsonValue(JsonObject* object) : object_(object) {}

JsonValue::JsonValue(JsonObject* object, bool isRoot) : object_(object), isRoot_(isRoot) {}

JsonValue::~JsonValue()
{
    if (object_ != nullptr && isRoot_) {
        cJSON_Delete(object_);
    }
    object_ = nullptr;
}

bool JsonValue::IsBool() const
{
    return cJSON_IsBool(object_);
}

bool JsonValue::IsNumber() const
{
    return cJSON_IsNumber(object_);
}

bool JsonValue::IsString() const
{
    return cJSON_IsString(object_);
}

bool JsonValue::IsArray() const
{
    return cJSON_IsArray(object_);
}

bool JsonValue::IsObject() const
{
    return cJSON_IsObject(object_);
}

bool JsonValue::IsValid() const
{
    return (object_ != nullptr) && !cJSON_IsInvalid(object_);
}

bool JsonValue::IsNull() const
{
    return (object_ == nullptr) || cJSON_IsNull(object_);
}

bool JsonValue::Contains(const std::string& key) const
{
    return cJSON_HasObjectItem(object_, key.c_str());
}

bool JsonValue::GetBool() const
{
    return cJSON_IsTrue(object_) != 0;
}

bool JsonValue::GetBool(const std::string& key, bool defaultValue) const
{
    if (Contains(key) && GetValue(key)->IsBool()) {
        return GetValue(key)->GetBool();
    }
    return defaultValue;
}

int32_t JsonValue::GetInt() const
{
    return static_cast<int32_t>((object_ == nullptr) ? 0 : object_->valuedouble);
}

uint32_t JsonValue::GetUInt() const
{
    return static_cast<uint32_t>((object_ == nullptr) ? 0 : object_->valuedouble);
}

double JsonValue::GetDouble() const
{
    return (object_ == nullptr) ? 0.0 : object_->valuedouble;
}

double JsonValue::GetDouble(const std::string& key, double defaultVal) const
{
    auto value = GetValue(key);
    if (value && value->IsNumber()) {
        return value->GetDouble();
    }
    return defaultVal;
}

std::string JsonValue::GetString() const
{
    return ((object_ == nullptr) || (object_->valuestring == nullptr)) ? "" : std::string(object_->valuestring);
}

std::unique_ptr<JsonValue> JsonValue::GetNext() const
{
    if (object_ == nullptr) {
        return std::make_unique<JsonValue>(nullptr);
    }
    return std::make_unique<JsonValue>(object_->next);
}

std::unique_ptr<JsonValue> JsonValue::GetChild() const
{
    if (object_ == nullptr) {
        return std::make_unique<JsonValue>(nullptr);
    }
    return std::make_unique<JsonValue>(object_->child);
}

std::string JsonValue::GetKey() const
{
    return ((object_ == nullptr) || (object_->string == nullptr)) ? "" : std::string(object_->string);
}
std::unique_ptr<JsonValue> JsonValue::GetValue(const std::string& key) const
{
    return std::make_unique<JsonValue>(cJSON_GetObjectItem(object_, key.c_str()));
}

std::unique_ptr<JsonValue> JsonValue::GetObject(const std::string& key) const
{
    if (Contains(key) && GetValue(key)->IsObject()) {
        return GetValue(key);
    }
    return std::make_unique<JsonValue>();
}

int32_t JsonValue::GetArraySize() const
{
    return cJSON_GetArraySize(object_);
}

std::unique_ptr<JsonValue> JsonValue::GetArrayItem(int32_t index) const
{
    return std::make_unique<JsonValue>(cJSON_GetArrayItem(object_, index));
}

bool JsonValue::Put(const char* key, const char* value)
{
    if (!value || !key) {
        return false;
    }

    cJSON* child = cJSON_CreateString(value);
    if (child == nullptr) {
        return false;
    }
    cJSON_AddItemToObject(object_, key, child);
    return true;
}

const JsonObject* JsonValue::GetJsonObject() const
{
    return object_;
}

bool JsonValue::Put(const char* key, const std::unique_ptr<JsonValue>& value)
{
    if (!value || !key) {
        return false;
    }
    cJSON* jsonObject = cJSON_Duplicate(value->GetJsonObject(), true);
    if (jsonObject == nullptr) {
        return false;
    }

    cJSON_AddItemToObject(object_, key, jsonObject);
    return true;
}

// add item to array
bool JsonValue::Put(const std::unique_ptr<JsonValue>& value)
{
    if (!value) {
        return false;
    }
    cJSON* jsonObject = cJSON_Duplicate(value->GetJsonObject(), true);
    if (jsonObject == nullptr) {
        return false;
    }

    cJSON_AddItemToArray(object_, jsonObject);
    return true;
}

bool JsonValue::Put(const char* key, size_t value)
{
    if (key == nullptr) {
        return false;
    }

    cJSON* child = cJSON_CreateNumber(static_cast<double>(value));
    if (child == nullptr) {
        return false;
    }
    cJSON_AddItemToObject(object_, key, child);
    return true;
}

bool JsonValue::Put(const char* key, int32_t value)
{
    if (key == nullptr) {
        return false;
    }

    cJSON* child = cJSON_CreateNumber(static_cast<double>(value));
    if (child == nullptr) {
        return false;
    }
    cJSON_AddItemToObject(object_, key, child);
    return true;
}

bool JsonValue::Put(const char* key, int64_t value)
{
    return Put(key, static_cast<double>(value));
}

bool JsonValue::Put(const char* key, double value)
{
    if (key == nullptr) {
        return false;
    }

    cJSON* child = cJSON_CreateNumber(value);
    if (child == nullptr) {
        return false;
    }
    cJSON_AddItemToObject(object_, key, child);
    return true;
}

bool JsonValue::Replace(const char* key, double value)
{
    if (key == nullptr) {
        return false;
    }

    cJSON* child = cJSON_CreateNumber(value);
    if (child == nullptr) {
        return false;
    }
    if (!cJSON_ReplaceItemInObject(object_, key, child)) {
        cJSON_Delete(child);
        return false;
    }
    return true;
}

bool JsonValue::Put(const char* key, bool value)
{
    if (key == nullptr) {
        return false;
    }

    cJSON* child = cJSON_CreateBool(value);
    if (child == nullptr) {
        return false;
    }
    cJSON_AddItemToObject(object_, key, child);
    return true;
}

bool JsonValue::Replace(const char* key, bool value)
{
    if (key == nullptr) {
        return false;
    }

    cJSON* child = cJSON_CreateBool(value);
    if (child == nullptr) {
        return false;
    }
    if (!cJSON_ReplaceItemInObject(object_, key, child)) {
        cJSON_Delete(child);
        return false;
    }
    return true;
}

bool JsonValue::Replace(const char* key, const char* value)
{
    if ((value == nullptr) || (key == nullptr)) {
        return false;
    }

    cJSON* child = cJSON_CreateString(value);
    if (child == nullptr) {
        return false;
    }
    if (!cJSON_ReplaceItemInObject(object_, key, child)) {
        cJSON_Delete(child);
        return false;
    }
    return true;
}

bool JsonValue::Replace(const char* key, int32_t value)
{
    if (key == nullptr) {
        return false;
    }

    cJSON* child = cJSON_CreateNumber(static_cast<double>(value));
    if (child == nullptr) {
        return false;
    }
    if (!cJSON_ReplaceItemInObject(object_, key, child)) {
        cJSON_Delete(child);
        return false;
    }
    return true;
}

bool JsonValue::Replace(const char* key, const std::unique_ptr<JsonValue>& value)
{
    if ((value == nullptr) || (key == nullptr)) {
        return false;
    }
    cJSON* jsonObject = cJSON_Duplicate(value->GetJsonObject(), true);
    if (jsonObject == nullptr) {
        return false;
    }

    if (!cJSON_ReplaceItemInObject(object_, key, jsonObject)) {
        cJSON_Delete(jsonObject);
        return false;
    }
    return true;
}

bool JsonValue::Delete(const char* key)
{
    if (key == nullptr) {
        return false;
    }
    cJSON_DeleteItemFromObject(object_, key);
    return true;
}

std::string JsonValue::ToString()
{
    std::string result;
    if (!object_) {
        return result;
    }

    // It is null-terminated.
    char* unformatted = cJSON_PrintUnformatted(object_);
    if (unformatted != nullptr) {
        result = unformatted;
        cJSON_free(unformatted);
    }
    return result;
}

std::string JsonValue::GetString(const std::string& key, const std::string& defaultVal) const
{
    auto value = GetValue(key);
    if (value && value->IsString()) {
        return value->GetString();
    }
    return defaultVal;
}

int32_t JsonValue::GetInt(const std::string& key, int32_t defaultVal) const
{
    auto value = GetValue(key);
    if (value && value->IsNumber()) {
        return value->GetInt();
    }
    return defaultVal;
}

uint32_t JsonValue::GetUInt(const std::string& key, uint32_t defaultVal) const
{
    auto value = GetValue(key);
    if (value && value->IsNumber()) {
        return value->GetUInt();
    }
    return defaultVal;
}

std::unique_ptr<JsonValue> JsonUtil::ParseJsonData(const char* data, const char** parseEnd)
{
    return std::make_unique<JsonValue>(cJSON_ParseWithOpts(data, parseEnd, true), true);
}

std::unique_ptr<JsonValue> JsonUtil::ParseJsonString(const std::string& content, const char** parseEnd)
{
    return ParseJsonData(content.c_str(), parseEnd);
}

std::unique_ptr<JsonValue> JsonUtil::Create(bool isRoot)
{
    return std::make_unique<JsonValue>(cJSON_CreateObject(), isRoot);
}

std::unique_ptr<JsonValue> JsonUtil::CreateArray(bool isRoot)
{
    return std::make_unique<JsonValue>(cJSON_CreateArray(), isRoot);
}

} // namespace OHOS::Ace
