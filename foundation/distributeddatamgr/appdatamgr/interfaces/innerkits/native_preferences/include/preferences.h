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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "preferences_observer.h"
#include "preferences_value.h"

namespace OHOS {
namespace NativePreferences {
class Preferences {
public:
    virtual ~Preferences() {}

    static const unsigned int MAX_KEY_LENGTH = 80;

    static const unsigned int MAX_VALUE_LENGTH = 8 * 1024;

    virtual int GetInt(const std::string &key, int defValue) = 0;

    virtual std::string GetString(const std::string &key, const std::string &defValue) = 0;

    virtual bool GetBool(const std::string &key, bool defValue) = 0;

    virtual float GetFloat(const std::string &key, float defValue) = 0;

    virtual double GetDouble(const std::string &key, double defValue) = 0;

    virtual int64_t GetLong(const std::string &key, int64_t defValue) = 0;

    virtual std::set<std::string> GetStringSet(const std::string &key, std::set<std::string> &defValue) = 0;

    virtual std::map<std::string, PreferencesValue> GetAll() = 0;

    virtual bool HasKey(const std::string &key) = 0;

    virtual int PutInt(const std::string &key, int value) = 0;

    virtual int PutString(const std::string &key, const std::string &value) = 0;

    virtual int PutBool(const std::string &key, bool value) = 0;

    virtual int PutLong(const std::string &key, int64_t value) = 0;

    virtual int PutFloat(const std::string &key, float value) = 0;

    virtual int PutDouble(const std::string &key, double value) = 0;

    virtual int PutStringSet(const std::string &key, const std::set<std::string> &value) = 0;

    virtual int Delete(const std::string &key) = 0;

    virtual int Clear() = 0;

    virtual void Flush() = 0;

    virtual int FlushSync() = 0;

    virtual void RegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) = 0;

    virtual void UnRegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) = 0;
};
} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // End of #ifndef PREFERENCES_H
