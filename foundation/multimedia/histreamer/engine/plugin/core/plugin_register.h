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

#ifndef HISTREAMER_PLUGIN_REGISTER_H
#define HISTREAMER_PLUGIN_REGISTER_H

#include <functional>
#include <map>
#include <set>
#include <utility>
#include "common/any.h"
#include "interface/audio_sink_plugin.h"
#include "interface/codec_plugin.h"
#include "interface/demuxer_plugin.h"
#include "interface/plugin_base.h"
#include "interface/source_plugin.h"
#include "plugin_loader.h"

#include "plugin_info.h"

namespace OHOS {
namespace Media {
namespace Plugin {
struct PluginRegInfo {
    std::shared_ptr<PackageDef> packageDef;
    std::shared_ptr<PluginInfo> info;
    PluginCreatorFunc<PluginBase> creator;
    DemuxerPluginSnifferFunc sniffer;
    std::shared_ptr<PluginLoader> loader;
};

class PluginRegister {
public:
    PluginRegister() = default;
    PluginRegister(const PluginRegister&) = delete;
    PluginRegister operator=(const PluginRegister&) = delete;
    ~PluginRegister();

    std::set<std::string> ListPlugins(PluginType type);

    int GetAllRegisteredPluginCount();

    std::shared_ptr<PluginRegInfo> GetPluginRegInfo(PluginType type, const std::string& name);

    void EnablePackage(PluginType type, const std::string& name);

    void DisablePackage(PluginType type, const std::string& name);

    int GetRegisteredPluginCountByPackageName(std::string& name);

    bool IsPackageExist(PluginType type, const std::string& name);
    void PrintRegisteredPluginInfo();

    int GetPackageCounts(std::string& name);

    void RegisterPlugins();

private:
    void RegisterStaticPlugins();
    void RegisterDynamicPlugins();
    void RegisterPluginsFromPath(const char* libDirPath);
    void UnregisterAllPlugins();
    void EraseRegisteredPluginsByPackageName(std::string name);
    void EraseRegisteredPluginsByLoader(const std::shared_ptr<PluginLoader>& loader);
    void SaveDisabledPackage(std::pair<std::string, std::shared_ptr<PluginRegInfo>> info);
    void RecoverDisabledPackage(PluginType type, std::string name);

private:
    using REGISTERED_TABLE = std::map<PluginType, std::map<std::string, std::shared_ptr<PluginRegInfo>>>;

    struct RegisterData {
        std::map<PluginType, std::set<std::string>> registerNames;
        REGISTERED_TABLE registerTable;
        std::vector<std::pair<std::string, std::shared_ptr<PluginRegInfo>>> disabledPackage;
        bool IsPluginExist(PluginType type, const std::string& name);
    };

    struct RegisterImpl : PackageRegister {
        RegisterImpl(std::shared_ptr<RegisterData> data, std::shared_ptr<PluginLoader> loader = nullptr)
            : pluginLoader(std::move(loader)), registerData(std::move(data)) {}

        ~RegisterImpl() override = default;

        Status AddPlugin(const PluginDefBase& def) override;

        Status AddPackage(const PackageDef& def) override;

        Status SetPackageDef(const PackageDef& def);

        std::shared_ptr<PluginRegInfo> BuildRegInfo(const PluginDefBase& definition);

        void SetPluginInfo(std::shared_ptr<PluginInfo>& pluginInfo, const PluginDefBase& definition);

        Status InitSourceInfo(std::shared_ptr<PluginRegInfo>& reg, const PluginDefBase& definition);

        static Status SourceCapabilityConvert(std::shared_ptr<PluginInfo>& info, const PluginDefBase& def);

        Status InitDemuxerInfo(std::shared_ptr<PluginRegInfo>& reg, const PluginDefBase& definition);

        static Status DemuxerCapabilityConvert(std::shared_ptr<PluginInfo>& info, const PluginDefBase& def);

        Status InitMuxerInfo(std::shared_ptr<PluginRegInfo>& reg, const PluginDefBase& def);

        Status InitCodecInfo(std::shared_ptr<PluginRegInfo>& reg, const PluginDefBase& def);

        static Status CodecCapabilityConvert(std::shared_ptr<PluginInfo>& info, const PluginDefBase& def);

        Status InitAudioSinkInfo(std::shared_ptr<PluginRegInfo>& reg, const PluginDefBase& def);

        static Status AudioSinkCapabilityConvert(std::shared_ptr<PluginInfo>& info, const PluginDefBase& def);

        Status InitVideoSinkInfo(std::shared_ptr<PluginRegInfo>& reg, const PluginDefBase& def);

        static Status VideoSinkCapabilityConvert(std::shared_ptr<PluginInfo>& info, const PluginDefBase& def);

        Status InitOutputSinkInfo(std::shared_ptr<PluginRegInfo>& reg, const PluginDefBase& def);

        bool Verification(const PluginDefBase& definition);

        bool VersionMatched(const PluginDefBase& definition);

        bool MoreAcceptable(std::shared_ptr<PluginRegInfo>& regInfo, const PluginDefBase& definition);

        std::shared_ptr<PluginLoader> pluginLoader;
        std::shared_ptr<RegisterData> registerData;
        std::shared_ptr<PackageDef> packageDef {nullptr};
    };

    std::shared_ptr<RegisterData> registerData = std::make_shared<RegisterData>();
    std::vector<std::shared_ptr<PluginLoader>> registeredLoaders;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_REGISTER_H
