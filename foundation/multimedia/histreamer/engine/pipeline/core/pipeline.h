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

#ifndef HISTREAMER_PIPELINE_CORE_PIPELINE_H
#define HISTREAMER_PIPELINE_CORE_PIPELINE_H

#include <initializer_list>
#include <memory>

#include "filter.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
class Pipeline : public Filter {
public:
    ~Pipeline() override = default;

    /**
     * 添加Filter到Pipeline
     *
     * 添加后的Filter，才会被初始化，才能使用。
     *
     * @param filters 要添加的filter
     * @return
     */
    virtual ErrorCode AddFilters(std::initializer_list<Filter*> filters) = 0;

    virtual ErrorCode RemoveFilter(Filter* filter) = 0;

    virtual ErrorCode RemoveFilterChain(Filter* firstFilter) = 0;

    /**
     * 连接Filter的默认端口
     *
     * 前一个Filter的默认输出端口，与后一个Filter的默认输入端口连接
     *
     * @param filters 要连接的filter
     * @return
     */
    virtual ErrorCode LinkFilters(std::initializer_list<Filter*> filters) = 0;

    /**
     * 连接两个端口
     *
     * 端口所属的Filter必须已经添加到Pipeline，否则报错
     *
     * @param port1 前一个节点的输出端口
     * @return
     */
    virtual ErrorCode LinkPorts(std::shared_ptr<OutPort> port1, std::shared_ptr<InPort> port2) = 0;
};
} // namespace Pipeline
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_PIPELINE_CORE_PIPELINE_H