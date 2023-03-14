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

#include "plugin/core/plugin_meta.h"

namespace OHOS {
namespace Media {
namespace Test {
using namespace OHOS::Media::Plugin;

TEST(TestMeta, set_then_get_uint32)
{
    Meta meta;
    int32_t channels = 64;
    meta.SetInt32(MetaID::AUDIO_CHANNELS, channels);
    int32_t outChannels = 0;
    ASSERT_TRUE(meta.GetInt32(MetaID::AUDIO_CHANNELS, outChannels));
    ASSERT_EQ(channels, outChannels);

    uint32_t canNotGet = 0;
    ASSERT_FALSE(meta.GetUint32(MetaID::AUDIO_CHANNELS, canNotGet));
}

TEST(TestMeta, set_then_get_cstring)
{
    Meta meta;
    std::string artist("abcd");
    meta.SetString(MetaID::MEDIA_TITLE, artist);
    std::string outArtist;
    ASSERT_TRUE(meta.GetString(MetaID::MEDIA_TITLE, outArtist));
    ASSERT_STREQ(artist.c_str(), outArtist.c_str());
}

TEST(TestMeta, set_then_get_float)
{
    Meta meta;
    float in = 9.9999f;
    meta.SetFloat(MetaID::MEDIA_ARTIST, in); // this is only for test, normally MEDIA_ARTIST should be string
    float out = 0;
    ASSERT_TRUE(meta.GetFloat(MetaID::MEDIA_ARTIST, out));
    ASSERT_FLOAT_EQ(in, out);
}

TEST(TestMeta, fail_to_get_unexisted_data)
{
    Meta meta;
    int32_t channels = 64;
    meta.SetInt32(MetaID::AUDIO_CHANNELS, channels);
    int64_t bitRate = 1876411;
    ASSERT_FALSE(meta.GetInt64(MetaID::MEDIA_BITRATE, bitRate));
}

TEST(TestMeta, remove_data)
{
    Meta meta;
    int32_t channels = 64;
    meta.SetInt32(MetaID::AUDIO_CHANNELS, channels);
    ASSERT_TRUE(meta.Remove(MetaID::AUDIO_CHANNELS));
    ASSERT_FALSE(meta.Remove(MetaID::MEDIA_BITRATE));
}

TEST(TestMeta, clear_data)
{
    Meta meta;
    std::string title("title");
    std::string album("album");
    int32_t channels = 64;
    meta.SetString(MetaID::MEDIA_TITLE, title);
    meta.SetString(MetaID::MEDIA_ALBUM, album);
    meta.SetInt32(MetaID::AUDIO_CHANNELS, channels);
    meta.Clear();
    std::string out;
    ASSERT_FALSE(meta.GetString(MetaID::MEDIA_TITLE, out));
    ASSERT_TRUE(out.empty());
    ASSERT_FALSE(meta.GetString(MetaID::MEDIA_ALBUM, out));
    ASSERT_TRUE(out.empty());
    int32_t oChannels = 0;
    ASSERT_FALSE(meta.GetInt32(MetaID::AUDIO_CHANNELS, oChannels));
    ASSERT_EQ(0u, oChannels);
}

TEST(TestMeta, update_meta)
{
    Meta meta;
    std::string title("title");
    std::string album("album");
    int32_t channels = 64;
    meta.SetString(MetaID::MEDIA_TITLE, title);
    meta.SetString(MetaID::MEDIA_ALBUM, album);
    meta.SetUint32(MetaID::AUDIO_CHANNELS, channels);

    Meta meta2;
    int32_t channels2 = 32;
    meta.SetInt32(MetaID::AUDIO_CHANNELS, channels2);

    meta.Update(meta2);
    std::string out;
    ASSERT_TRUE(meta.GetString(MetaID::MEDIA_TITLE, out));
    ASSERT_STREQ("title", out.c_str());
    ASSERT_TRUE(meta.GetString(MetaID::MEDIA_ALBUM, out));
    ASSERT_STREQ("album", out.c_str());
    int32_t oChannel = 0;
    ASSERT_TRUE(meta.GetInt32(MetaID::AUDIO_CHANNELS, oChannel));
    ASSERT_EQ(channels2, oChannel);
}
} // namespace Test
} // namespace Media
} // namespace OHOS
