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

#include "filter_factory.h"

namespace OHOS {
namespace Media {
FilterFactory& FilterFactory::Instance()
{
    static FilterFactory instance;
    return instance;
}

void FilterFactory::Init()
{
}

void FilterFactory::RegisterGenerator(const std::string& name, const InstanceGenerator& generator)
{
    auto result = generators.emplace(name, generator);
    if (!result.second) {
        result.first->second = generator;
    }
}

std::shared_ptr<Pipeline::Filter> FilterFactory::CreateFilter(const std::string& filterName,
    const std::string& aliasName)
{
    auto it = generators.find(filterName);
    if (it != generators.end()) {
        return it->second(aliasName);
    }
    return nullptr;
}
} // namespace Media
} // namespace OHOS
