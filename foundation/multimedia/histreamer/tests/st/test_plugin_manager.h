/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include "testngpp/testngpp.hpp"
#include "plugin/core/plugin_manager.h"

using namespace OHOS::Media::Plugin;

// @fixture(tags=audio_play_fast)
FIXTURE(PluginsEnableAndDisable)
{
    /**
     *  Test Scenario:   Check package exists, package count.
     *  1. [Done] Can disable one package.
     *  2. [Done] Can enable one package.
     *  3. [Done] Can disable two packages.
     *  4. [Done] Can enable two packages.
     *  5. [Done] Can not disable a package not exists.
     *  6. [To Fix] Can not enable a package not exists.
     *  7. [To Fix] Can not enable a package that also enabled.
     *  8. [To Fix] The plugin manager seems only registered these plugins:
     *  1 (wav_demuxer_plugin.cpp, 248) : Func(RegisterPlugin) RegisterPlugin called.
        1 (audio_ffmpeg_decoder_plugin.cpp, 60) : Func(RegisterAudioDecoderPlugins) registering audio decoders
        1 (audio_ffmpeg_encoder_plugin.cpp, 45) : Func(RegisterAudioEncoderPlugins) registering audio encoders
        1 (minimp3_decoder_plugin.cpp, 242) : Func(RegisterDecoderPlugin) RegisterPlugins called.
        1 (video_ffmpeg_encoder_plugin.cpp, 47) : Func(RegisterVideoEncoderPlugins) registering video encoders
        1 (video_ffmpeg_decoder_plugin.cpp, 50) : Func(RegisterVideoDecoderPlugins) registering video decoders
     * */
    PluginManager& pluginManager = PluginManager::Instance();
    int pluginCounts = pluginManager.GetAllRegisteredPluginCount();

    DATA_PROVIDER(plugins, 2,
    DATA_GROUP(OHOS::Media::Plugin::PluginType::SOURCE, std::string("FileSource")),
    DATA_GROUP(OHOS::Media::Plugin::PluginType::SOURCE, std::string("HttpSource")),
    DATA_GROUP(OHOS::Media::Plugin::PluginType::DEMUXER, std::string("FFmpegDemuxer")),
    DATA_GROUP(OHOS::Media::Plugin::PluginType::CODEC, std::string("Minimp3Decoder")));

    // @test(data="plugins",tags=fast)
    PTEST((OHOS::Media::Plugin::PluginType type, std::string name), Can disable one package and enable it again)
    {
        pluginManager.DisablePackage(type, name);
        ASSERT_FALSE(pluginManager.IsPackageExist(type, name));
        ASSERT_TRUE(pluginManager.GetAllRegisteredPluginCount() < pluginCounts);
        pluginManager.EnablePackage(type, name);
        ASSERT_TRUE(pluginManager.IsPackageExist(type, name));
        ASSERT_TRUE(pluginManager.GetAllRegisteredPluginCount() == pluginCounts);
    }

    // @test(tags=fast)
    TEST(Can disable two packages and enable them again)
    {
        pluginManager.DisablePackage(OHOS::Media::Plugin::PluginType::SOURCE, "FileSource");
        pluginManager.DisablePackage(OHOS::Media::Plugin::PluginType::DEMUXER, "FFmpegDemuxer");
        ASSERT_FALSE(pluginManager.IsPackageExist(OHOS::Media::Plugin::PluginType::SOURCE, "FileSource"));
        ASSERT_FALSE(pluginManager.IsPackageExist(OHOS::Media::Plugin::PluginType::DEMUXER, "FFmpegDemuxer"));
        ASSERT_TRUE(pluginManager.GetAllRegisteredPluginCount() < pluginCounts);
        pluginManager.EnablePackage(OHOS::Media::Plugin::PluginType::SOURCE, "FileSource");
        pluginManager.EnablePackage(OHOS::Media::Plugin::PluginType::DEMUXER, "FFmpegDemuxer");
        ASSERT_TRUE(pluginManager.IsPackageExist(OHOS::Media::Plugin::PluginType::SOURCE, "FileSource"));
        ASSERT_TRUE(pluginManager.IsPackageExist(OHOS::Media::Plugin::PluginType::DEMUXER, "FFmpegDemuxer"));
        ASSERT_TRUE(pluginManager.GetAllRegisteredPluginCount() == pluginCounts);
    }

    // @test(tags=fast)
    TEST(Plugin counts should be decreased by the plugin count of package when the package disabled : FileSource)
    {
        int pluginCountInPkg = pluginManager.GetRegisteredPluginCountByPackageName("FileSource");
        pluginManager.DisablePackage(OHOS::Media::Plugin::PluginType::SOURCE, "FileSource");
        ASSERT_EQ(pluginManager.GetAllRegisteredPluginCount() + pluginCountInPkg, pluginCounts);
        pluginManager.EnablePackage(OHOS::Media::Plugin::PluginType::SOURCE, "FileSource");
    }

    // @test(tags=fast)
    TEST(Plugin counts should be decreased by the plugin count of package when the package disabled : FFmpegDemuxer)
    {
        int pluginCountInPkg = pluginManager.GetRegisteredPluginCountByPackageName("FFmpegDemuxer");
        pluginManager.DisablePackage(OHOS::Media::Plugin::PluginType::DEMUXER, "FFmpegDemuxer");
        ASSERT_EQ(pluginManager.GetAllRegisteredPluginCount() + pluginCountInPkg, pluginCounts);
        pluginManager.EnablePackage(OHOS::Media::Plugin::PluginType::DEMUXER, "FFmpegDemuxer");
    }
};
