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

#include "UtSourceTest1.h"

namespace OHOS {
namespace Media {
namespace Plugin {
bool UtSourceTest1::available = true;

static std::shared_ptr<SourcePlugin> PluginCreator(const std::string &name)
{
    return std::make_shared<UtSourceTest1>(name);
}

static Status SourceRegister(const std::shared_ptr<Register> &reg)
{
    if (UtSourceTest1::available) {
        SourcePluginDef definition;
        definition.name = "UtSourceTest1";
        definition.description = "unit test source test1";
        definition.rank = 100; // 100
        definition.protocol.emplace_back(ProtocolType::FILE);
        definition.creator = PluginCreator;
        return reg->AddPlugin(definition);
    } else {
        return Status::ERROR_UNIMPLEMENTED;
    }
}

PLUGIN_DEFINITION(UtSourceTest1, LicenseType::APACHE_V2, SourceRegister, [] {});

Status OHOS::Media::Plugin::UtSourceTest1::SetSource(std::shared_ptr<MediaSource> source)
{
    return Status::OK;
}

Status UtSourceTest1::Read(std::shared_ptr<Buffer> &buffer, size_t expectedLen)
{
    return Status::OK;
}

Status UtSourceTest1::GetSize(size_t &size)
{
    return Status::OK;
}

Status UtSourceTest1::IsSeekable(bool& seekable)
{
    seekable = false;
    return Status::OK;
}

Seekable UtSourceTest1::GetSeekable()
{
    return Seekable::UNSEEKABLE;
}

Status UtSourceTest1::SeekTo(uint64_t offset)
{
    return Status::OK;
}

std::shared_ptr<Allocator> UtSourceTest1::GetAllocator()
{
    return std::shared_ptr<Allocator>();
}

Status UtSourceTest1::SetCallback(Callback* cb)
{
    return Status::OK;
}
}
}
}
