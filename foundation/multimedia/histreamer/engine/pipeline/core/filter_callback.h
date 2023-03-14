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

#ifndef HISTREAMER_PIPELINE_CORE_FILTER_CALLBACK_H
#define HISTREAMER_PIPELINE_CORE_FILTER_CALLBACK_H

#include <string>
#include <vector>

#include "plugin/common/any.h"
#include "error_code.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class Filter;

// FilterCallback用于同步处理完再返回的场景
// Filter Callback Types
enum class FilterCallbackType { PORT_ADDED, PORT_REMOVE };

enum class PortType { IN, OUT };

struct PortDesc {
    std::string name;
    bool isPcm;
};

struct PortInfo {
    PortType type;
    std::vector<PortDesc> ports;
};

class FilterCallback {
public:
    virtual ~FilterCallback() = default;
    virtual ErrorCode OnCallback(const FilterCallbackType& type, Filter* filter, const Plugin::Any& parameter) = 0;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_PIPELINE_CORE_FILTER_CALLBACK_H
