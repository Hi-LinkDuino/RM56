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

#include "UtCodecTest2.h"

using namespace OHOS::Media::Plugin;

static std::shared_ptr<CodecPlugin> PluginCreator(const std::string &name)
{
    return std::make_shared<UtCodecTest2>(name);
}

static Status CodecRegister(const std::shared_ptr<Register> &reg)
{
    CodecPluginDef definition;
    definition.name = "UtCodecTest2";
    definition.description = "unit test codec test2";
    definition.rank = 100; // 100
    definition.creator = PluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(UtCodecTest2, LicenseType::APACHE_V2, CodecRegister, [] {});

Status UtCodecTest2::QueueInputBuffer(const std::shared_ptr<Buffer> &inputBuffer, int32_t timeoutMs)
{
    return Status::OK;
}

Status UtCodecTest2::QueueOutputBuffer(const std::shared_ptr<Buffer> &outputBuffers, int32_t timeoutMs)
{
    return Status::OK;
}

Status UtCodecTest2::Flush()
{
    return Status::OK;
}

Status UtCodecTest2::SetDataCallback(DataCallback* dataCallback)
{
    return Status::OK;
}

std::shared_ptr<Allocator> UtCodecTest2::GetAllocator()
{
    return std::shared_ptr<Allocator>();
}

Status UtCodecTest2::SetCallback(Callback* cb)
{
    return Status::OK;
}
