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

#include "UtAudioSinkTest1.h"

using namespace OHOS::Media::Plugin;

static std::shared_ptr<AudioSinkPlugin> PluginCreator(const std::string &name)
{
    return std::make_shared<UtAudioSinkTest1>(name);
}

static Status AudioSinkRegister(const std::shared_ptr<Register> &reg)
{
    AudioSinkPluginDef definition;
    definition.name = "UtAudioSinkTest1";
    definition.description = "unit test audio sink test1";
    definition.rank = 100; // 100
    definition.creator = PluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(UtAudioSinkTest1, LicenseType::APACHE_V2, AudioSinkRegister, [] {});

Status UtAudioSinkTest1::GetMute(bool &mute)
{
    return Status::OK;
}

Status UtAudioSinkTest1::SetMute(bool mute)
{
    return Status::OK;
}

Status UtAudioSinkTest1::GetVolume(float &volume)
{
    return Status::OK;
}

Status UtAudioSinkTest1::SetVolume(float volume)
{
    return Status::OK;
}

Status UtAudioSinkTest1::GetSpeed(float &speed)
{
    return Status::OK;
}

Status UtAudioSinkTest1::SetSpeed(float speed)
{
    return Status::OK;
}

Status UtAudioSinkTest1::Pause()
{
    return Status::OK;
}

Status UtAudioSinkTest1::Resume()
{
    return Status::OK;
}

Status UtAudioSinkTest1::GetLatency(uint64_t &ms)
{
    return Status::OK;
}

Status UtAudioSinkTest1::GetFrameSize(size_t &size)
{
    return Status::OK;
}

Status UtAudioSinkTest1::GetFrameCount(uint32_t &count)
{
    return Status::OK;
}

Status UtAudioSinkTest1::Write(const std::shared_ptr<Buffer> &input)
{
    return Status::OK;
}

Status UtAudioSinkTest1::Flush()
{
    return Status::OK;
}

Status UtAudioSinkTest1::Drain()
{
    return Status::OK;
}

std::shared_ptr<Allocator> UtAudioSinkTest1::GetAllocator()
{
    return std::shared_ptr<Allocator>();
}

Status UtAudioSinkTest1::SetCallback(Callback* cb)
{
    return Status::OK;
}

