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

#include "UtAudioSinkTest2.h"

using namespace OHOS::Media::Plugin;

static std::shared_ptr<AudioSinkPlugin> PluginCreator(const std::string &name)
{
    return std::make_shared<UtAudioSinkTest2>(name);
}

static Status AudioSinkRegister(const std::shared_ptr<Register> &reg)
{
    AudioSinkPluginDef definition;
    definition.name = "UtAudioSinkTest2";
    definition.description = "unit test audio sink test2";
    definition.rank = 100; // 100
    definition.creator = PluginCreator;
    return reg->AddPlugin(definition);
}

PLUGIN_DEFINITION(UtAudioSinkTest2, LicenseType::APACHE_V2, AudioSinkRegister, [] {});

Status UtAudioSinkTest2::GetMute(bool &mute)
{
    return Status::OK;
}

Status UtAudioSinkTest2::SetMute(bool mute)
{
    return Status::OK;
}

Status UtAudioSinkTest2::GetVolume(float &volume)
{
    return Status::OK;
}

Status UtAudioSinkTest2::SetVolume(float volume)
{
    return Status::OK;
}

Status UtAudioSinkTest2::GetSpeed(float &speed)
{
    return Status::OK;
}

Status UtAudioSinkTest2::SetSpeed(float speed)
{
    return Status::OK;
}

Status UtAudioSinkTest2::Pause()
{
    return Status::OK;
}

Status UtAudioSinkTest2::Resume()
{
    return Status::OK;
}

Status UtAudioSinkTest2::GetLatency(uint64_t &ms)
{
    return Status::OK;
}

Status UtAudioSinkTest2::GetFrameSize(size_t &size)
{
    return Status::OK;
}

Status UtAudioSinkTest2::GetFrameCount(uint32_t &count)
{
    return Status::OK;
}

Status UtAudioSinkTest2::Write(const std::shared_ptr<Buffer> &input)
{
    return Status::OK;
}

Status UtAudioSinkTest2::Flush()
{
    return Status::OK;
}

Status UtAudioSinkTest2::Drain()
{
    return Status::OK;
}

std::shared_ptr<Allocator> UtAudioSinkTest2::GetAllocator()
{
    return std::shared_ptr<Allocator>();
}

Status UtAudioSinkTest2::SetCallback(Callback* cb)
{
    return Status::OK;
}
