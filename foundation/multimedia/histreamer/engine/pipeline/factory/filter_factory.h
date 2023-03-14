/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_FILTER_FACTORY_H
#define HISTREAMER_PIPELINE_FILTER_FACTORY_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "pipeline/core/filter.h"

namespace OHOS {
namespace Media {
using InstanceGenerator = std::function<std::shared_ptr<Pipeline::Filter>(const std::string&)>;

class FilterFactory {
public:
    ~FilterFactory() = default;

    FilterFactory(const FilterFactory&) = delete;

    FilterFactory operator=(const FilterFactory&) = delete;

    static FilterFactory& Instance();

    void Init();

    void RegisterGenerator(const std::string& name, const InstanceGenerator& generator);

    std::shared_ptr<Pipeline::Filter> CreateFilter(const std::string& filterName, const std::string& aliasName);

    template <typename T>
    std::shared_ptr<T> CreateFilterWithType(const std::string& filterName, const std::string& aliasName)
    {
        auto filter = CreateFilter(filterName, aliasName);
        auto typedFilter = std::dynamic_pointer_cast<T>(filter);
        return typedFilter;
    }

    template <typename T>
    void RegisterFilter(const std::string& name)
    {
        RegisterGenerator(name, [](const std::string& aliaName) { return std::make_shared<T>(aliaName); });
    }

private:
    FilterFactory() = default;

    std::unordered_map<std::string, InstanceGenerator> generators;
};

template <typename T>
class AutoRegisterFilter {
public:
    explicit AutoRegisterFilter(const std::string& name)
    {
        FilterFactory::Instance().RegisterFilter<T>(name);
    }

    AutoRegisterFilter(const std::string& name, const InstanceGenerator& generator)
    {
        FilterFactory::Instance().RegisterGenerator(name, generator);
    }

    ~AutoRegisterFilter() = default;
};
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PIPELINE_FILTER_FACTORY_H
