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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RESOURCE_WEB_JAVASCRIPT_VALUE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RESOURCE_WEB_JAVASCRIPT_VALUE_H

namespace OHOS::Ace {
enum class WebHitTestType : int {
    EDIT = 0,
    EMAIL,
    HTTP,
    HTTP_IMG,
    IMG,
    MAP,
    PHONE,
    UNKNOWN
};

enum class WebJavaScriptBridgeError : int {
    NO_ERROR = 0,
    UNKNOWN_OBJECT_ID,
    OBJECT_IS_GONE,
    METHOD_NOT_FOUND,
    ACCESS_TO_OBJECT_GET_CLASS_IS_BLOCKED,
    EXCEPTION_RAISED,
    NON_ASSIGNABLE_TYPES,
    RENDER_FRAME_DELETED
};

union WebJSValueUnion {
    int n;
    double f;
    bool b;
};

class WebJavaScriptValue {
public:
    enum class Type : unsigned char {
        NONE = 0,
        BOOLEAN,
        INTEGER,
        DOUBLE,
        STRING,
        BINARY,
        DICTIONARY,
        LIST
    };

    WebJavaScriptValue(Type type) : type_(type) {}

    ~WebJavaScriptValue() = default;

    bool GetBoolean()
    {
        return data_.b;
    }

    void SetBoolean(bool value)
    {
        data_.b = value;
    }

    void SetString(std::string value)
    {
        str_ = value;
    }

    std::string GetString()
    {
        return str_;
    }

    void SetDouble(double value)
    {
        data_.f = value;
    }

    double GetDouble()
    {
        return data_.f;
    }

    void SetInt(int value)
    {
        data_.n = value;
    }

    int GetInt()
    {
        return data_.n;
    }

    void SetJsonString(std::string value)
    {
        json_ = value;
    }

    std::string GetJsonString()
    {
        return json_;
    }

    Type GetType()
    {
        return type_;
    }

    void SetType(Type type)
    {
        type_ = type;
    }

    int error_ = 0;

private:
    Type type_ = Type::NONE;
    WebJSValueUnion data_;
    std::string str_;
    std::string json_;
};
using WebJSValue = WebJavaScriptValue;
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RESOURCE_WEB_JAVASCRIPT_VALUE_H
