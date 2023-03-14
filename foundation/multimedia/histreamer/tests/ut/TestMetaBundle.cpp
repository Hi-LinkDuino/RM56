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

#include <gtest/gtest.h>

#define private public
#define protected public
#define UNIT_TEST 1

#include "pipeline/core/pipeline_core.h"
#include "plugin/core/plugin_meta.h"

namespace OHOS {
namespace Media {
namespace Test {
TEST(Test_meta_bundle, GetNull_before_Update)
{
    Pipeline::MetaBundle bundle;
    ASSERT_TRUE(bundle.GetGlobalMeta() == nullptr);
    ASSERT_TRUE(bundle.GeTrackMeta(0) == nullptr);
}

TEST(Test_meta_bundle, GetGlobalMeta_after_Update)
{
    Plugin::Meta meta;
    std::string title("title");
    std::string album("album");
    int32_t channels = 64;
    meta.SetString(Media::Plugin::MetaID::MEDIA_TITLE, title);
    meta.SetString(Media::Plugin::MetaID::MEDIA_ALBUM, album);
    meta.SetInt32(Media::Plugin::MetaID::AUDIO_CHANNELS, channels);

    Pipeline::MetaBundle bundle;
    ASSERT_TRUE(bundle.GetGlobalMeta() == nullptr);
    ASSERT_TRUE(bundle.GeTrackMeta(0) == nullptr);
    bundle.UpdateGlobalMeta(meta);
    auto out = bundle.GetGlobalMeta();
    ASSERT_TRUE(out != nullptr);
    std::string outString;
    ASSERT_TRUE(out->GetString(Media::Plugin::MetaID::MEDIA_TITLE, outString));
    ASSERT_STREQ("title", outString.c_str());
    ASSERT_TRUE(out->GetString(Media::Plugin::MetaID::MEDIA_ALBUM, outString));
    ASSERT_STREQ("album", outString.c_str());
    int32_t oChannel = 0;
    ASSERT_TRUE(out->GetInt32(Media::Plugin::MetaID::AUDIO_CHANNELS, oChannel));
    ASSERT_EQ(channels, oChannel);
}

TEST(Test_meta_bundle, GetStreamMeta_after_Update)
{
    Plugin::Meta meta;
    std::string mime("audio/mpeg");
    std::string language("zh");
    meta.SetString(Media::Plugin::MetaID::MIME, mime);
    meta.SetString(Media::Plugin::MetaID::MEDIA_LANGUAGE, language);

    Pipeline::MetaBundle bundle;
    ASSERT_TRUE(bundle.GetGlobalMeta() == nullptr);
    ASSERT_TRUE(bundle.GeTrackMeta(0) == nullptr);
    bundle.UpdateTrackMeta(meta);
    auto out = bundle.GeTrackMeta(0);
    ASSERT_TRUE(out == nullptr);
    meta.SetUint32(Media::Plugin::MetaID::TRACK_ID, 1);
    bundle.UpdateTrackMeta(meta);
    out = bundle.GeTrackMeta(1);
    std::string outString;
    ASSERT_TRUE(out->GetString(Media::Plugin::MetaID::MIME, outString));
    ASSERT_STREQ("audio/mpeg", outString.c_str());
    ASSERT_TRUE(out->GetString(Media::Plugin::MetaID::MEDIA_LANGUAGE, outString));
    ASSERT_STREQ("zh", outString.c_str());
    int32_t oChannel = 0;
    ASSERT_FALSE(out->GetInt32(Media::Plugin::MetaID::AUDIO_CHANNELS, oChannel));
    uint32_t sIndex = 0;
    ASSERT_TRUE(out->GetUint32(Media::Plugin::MetaID::TRACK_ID, sIndex));
    ASSERT_EQ(sIndex, 1);
}
} // namespace Test
} // namespace Media
} // namespace OHOS