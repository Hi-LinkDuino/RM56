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

#include "plugin_manager.h"
#include "interface/audio_sink_plugin.h"
#include "interface/codec_plugin.h"
#include "interface/demuxer_plugin.h"
#include "interface/source_plugin.h"
#include "interface/video_sink_plugin.h"
#include "plugin_register.h"
#include "plugin_wrapper.h"

#include <utility>

namespace OHOS {
namespace Media {
namespace Plugin {
PluginManager::PluginManager()
{
    Init();
}

std::set<std::string> PluginManager::ListPlugins(PluginType type)
{
    return pluginRegister_->ListPlugins(type);
}

std::shared_ptr<PluginInfo> PluginManager::GetPluginInfo(PluginType type, const std::string& name)
{
    std::shared_ptr<PluginRegInfo> regInfo = pluginRegister_->GetPluginRegInfo(type, name);
    if (regInfo && regInfo->info && regInfo->info->pluginType == type) {
        return regInfo->info;
    }
    return {};
}

int32_t PluginManager::Sniffer(const std::string& name, std::shared_ptr<DataSourceHelper> source)
{
    if (source == nullptr) {
        return 0;
    }
    std::shared_ptr<PluginRegInfo> regInfo = pluginRegister_->GetPluginRegInfo(PluginType::DEMUXER, name);
    if (!regInfo) {
        return 0;
    }
    if (regInfo->info->pluginType == PluginType::DEMUXER) {
        return regInfo->sniffer(name, std::make_shared<DataSourceWrapper>(regInfo->packageDef->pkgVersion, source));
    }
    return 0;
}

void PluginManager::EnablePackage(PluginType type, const std::string& name)
{
    return pluginRegister_->EnablePackage(type, name);
}

void PluginManager::DisablePackage(PluginType type, const std::string& name)
{
    return pluginRegister_->DisablePackage(type, name);
}

bool PluginManager::IsPackageExist(PluginType type, const std::string& name)
{
    return pluginRegister_->IsPackageExist(type, name);
}

void PluginManager::PrintRegisteredPluginInfo()
{
    pluginRegister_->PrintRegisteredPluginInfo();
}

int PluginManager::GetAllRegisteredPluginCount()
{
    return pluginRegister_->GetAllRegisteredPluginCount();
}

int PluginManager::GetRegisteredPluginCountByPackageName(std::string name)
{
    return pluginRegister_->GetRegisteredPluginCountByPackageName(name);
}

void PluginManager::Init()
{
    pluginRegister_ = std::make_shared<PluginRegister>();
    pluginRegister_->RegisterPlugins();
}

std::shared_ptr<Demuxer> PluginManager::CreateDemuxerPlugin(const std::string& name)
{
    return CreatePlugin<Demuxer, DemuxerPlugin>(name, PluginType::DEMUXER);
}

std::shared_ptr<Muxer> PluginManager::CreateMuxerPlugin(const std::string& name)
{
    return CreatePlugin<Muxer, MuxerPlugin>(name, PluginType::MUXER);
}

std::shared_ptr<Source> PluginManager::CreateSourcePlugin(const std::string& name)
{
    return CreatePlugin<Source, SourcePlugin>(name, PluginType::SOURCE);
}

std::shared_ptr<Codec> PluginManager::CreateCodecPlugin(const std::string& name)
{
    return CreatePlugin<Codec, CodecPlugin>(name, PluginType::CODEC);
}

std::shared_ptr<AudioSink> PluginManager::CreateAudioSinkPlugin(const std::string& name)
{
    return CreatePlugin<AudioSink, AudioSinkPlugin>(name, PluginType::AUDIO_SINK);
}

std::shared_ptr<VideoSink> PluginManager::CreateVideoSinkPlugin(const std::string& name)
{
    return CreatePlugin<VideoSink, VideoSinkPlugin>(name, PluginType::VIDEO_SINK);
}
std::shared_ptr<OutputSink> PluginManager::CreateOutputSinkPlugin(const std::string& name)
{
    return CreatePlugin<OutputSink, OutputSinkPlugin>(name, PluginType::OUTPUT_SINK);
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS