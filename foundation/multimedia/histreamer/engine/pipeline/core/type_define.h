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

#ifndef HISTREAMER_FOUNDATION_TYPE_DEFINE_H
#define HISTREAMER_FOUNDATION_TYPE_DEFINE_H

#include <memory>

#include "plugin/common/plugin_caps.h"
#include "plugin/common/plugin_tags.h"
#include "plugin/common/plugin_buffer.h"
#include "utils/buffer_pool.h"

namespace OHOS {
namespace Media {
using Capability = Plugin::Capability;
using CapabilitySet = Plugin::CapabilitySet;
using CapabilityID = Plugin::Capability::Key;
using Tag = Plugin::Tag;
using Allocator = Plugin::Allocator;
using AVBuffer = Plugin::Buffer;
using AVBufferPtr = std::shared_ptr<AVBuffer>;
using AVBufferPool = BufferPool<AVBuffer>;
using AVBufferPoolPtr = std::shared_ptr<BufferPool<AVBuffer>>;
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_TYPE_DEFINE_H
