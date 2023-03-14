/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_ACELITE_LOCALIZATION_MODULE_H
#define OHOS_ACELITE_LOCALIZATION_MODULE_H

#include "acelite_config.h"
#include "non_copyable.h"
#if (FEATURE_LOCALIZATION_MODULE == 1)
#include "cjson_parser.h"
#include "presets/preset_module.h"
namespace OHOS {
namespace ACELite {
class LocalizationModule final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LocalizationModule);
    void Clear();

    void Init();

    static LocalizationModule *GetInstance()
    {
        static LocalizationModule localizationModule;
        return &localizationModule;
    }
    static jerry_value_t Translate(const jerry_value_t func,
                                   const jerry_value_t context,
                                   const jerry_value_t *args,
                                   const jerry_length_t argNum)
    {
#ifdef LOCALIZATION_PLURAL
        return GetValueByKey(args, argNum, false);
#else
        return GetValueByKey(args, argNum);
#endif
    }

    static jerry_value_t TranslatePlural(const jerry_value_t func,
                                         const jerry_value_t context,
                                         const jerry_value_t *args,
                                         const jerry_length_t argNum)
    {
#ifdef LOCALIZATION_PLURAL
        return GetValueByKey(args, argNum, true);
#else
        return GetValueByKey(args, argNum);
#endif
    }

#ifdef LOCALIZATION_PLURAL
    static jerry_value_t GetValueByKey(const jerry_value_t *args, const jerry_length_t argsNum, bool isPlural);
#else
    static jerry_value_t GetValueByKey(const jerry_value_t *args, const jerry_length_t argsNum);
#endif

private:
    LocalizationModule() : parser_(nullptr) {}
    ~LocalizationModule()
    {
        Clear();
    }
    CJSONParser *parser_;
};
} // namespace ACELite
} // namespace OHOS
#endif // LOCALIZATION_MODULE

namespace OHOS {
namespace ACELite {
class LocalModule final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LocalModule);
    LocalModule() = default;
    ~LocalModule() = default;
    static void Load()
    {
#if (FEATURE_LOCALIZATION_MODULE == 1)
        LocalizationModule *localizationModule = LocalizationModule::GetInstance();
        localizationModule->Init();
#endif
    }
    static void Clear()
    {
#if (FEATURE_LOCALIZATION_MODULE == 1)
        LocalizationModule *localization = LocalizationModule::GetInstance();
        localization->Clear();
#endif
    }
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_LOCALIZATION_MODULE_H
