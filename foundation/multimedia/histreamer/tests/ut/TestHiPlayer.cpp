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

#ifdef OHOS_LITE
#include <memory>
#include "common/any.h"

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

#include <test_helper.h>
#include "pipeline/filters/codec/audio_decoder/audio_decoder_filter.h"
#include "pipeline/filters/sink/audio_sink/audio_sink_filter.h"
#include "scene/player/lite/hiplayer_impl.h"

using namespace OHOS::Media::Pipeline;

namespace OHOS::Media::Test {
class UtTestHiPlayer : public ::testing::Test {
public:
    MockObject<MediaSourceFilter> audioSource {};
    MockObject<DemuxerFilter> demuxer {};
    MockObject<AudioDecoderFilter> audioDecoder {};
    MockObject<AudioSinkFilter> audioSink {};

    std::shared_ptr<HiPlayerImpl> player = HiPlayerImpl::CreateHiPlayerImpl();
    static OHOS::Media::Source source;
    PInPort emptyInPort = EmptyInPort::GetInstance();
    POutPort emptyOutPort = EmptyOutPort::GetInstance();

    virtual void SetUp() override
    {
        MOCK_METHOD(audioSource, Init).defaults();
        MOCK_METHOD(demuxer, Init).defaults();
        MOCK_METHOD(audioDecoder, Init).defaults();
        MOCK_METHOD(audioSink, Init).defaults();

        MOCK_METHOD(audioSource, Prepare).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(demuxer, Prepare).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(audioDecoder, Prepare).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(audioSink, Prepare).defaults().will(returnValue(ErrorCode::SUCCESS));

        MOCK_METHOD(audioSource, Start).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(demuxer, Start).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(audioDecoder, Start).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(audioSink, Start).defaults().will(returnValue(ErrorCode::SUCCESS));

        MOCK_METHOD(audioSource, Stop).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(demuxer, Stop).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(audioDecoder, Stop).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(audioSink, Stop).defaults().will(returnValue(ErrorCode::SUCCESS));

        MOCK_METHOD(audioSource, GetInPort).defaults().will(returnValue(emptyInPort));
        MOCK_METHOD(demuxer, GetInPort).defaults().will(returnValue(emptyInPort));
        MOCK_METHOD(audioDecoder, GetInPort).defaults().will(returnValue(emptyInPort));
        MOCK_METHOD(audioSink, GetInPort).defaults().will(returnValue(emptyInPort));

        MOCK_METHOD(audioSource, GetOutPort).defaults().will(returnValue(emptyOutPort));
        MOCK_METHOD(demuxer, GetOutPort).defaults().will(returnValue(emptyOutPort));
        MOCK_METHOD(audioDecoder, GetOutPort).defaults().will(returnValue(emptyOutPort));
        MOCK_METHOD(audioSink, GetOutPort).defaults().will(returnValue(emptyOutPort));

        player->audioSource_.reset<MediaSourceFilter>(audioSource);
        player->demuxer_.reset<DemuxerFilter>(demuxer);
        player->audioDecoder_.reset<AudioDecoderFilter>(audioDecoder);
        player->audioSink_.reset<AudioSinkFilter>(audioSink);

        MOCK_METHOD(audioSource, SetSource).defaults().will(returnValue(ErrorCode::SUCCESS));
        MOCK_METHOD(audioSource, SetBufferSize).defaults().will(returnValue(ErrorCode::SUCCESS));

        player->Init();
    }
    virtual void TearDown() override
    {
        audioSource.verify();
        demuxer.verify();
        audioDecoder.verify();
        audioSink.verify();

        audioSource.reset();
        demuxer.reset();
        audioDecoder.reset();
        audioSink.reset();
    }
};

OHOS::Media::Source UtTestHiPlayer::source("./test.mp3");

TEST_F(UtTestHiPlayer, Can_SetSource)
{
    // 因为player内部根据source参数创建shared_ptr，最终调用 audioSource.SetSource. 这里没法检查该指针参数了。
    MOCK_METHOD(audioSource, SetSource)
        .expects(once())
        .with(mockcpp::any())
        .will(returnValue(ErrorCode::SUCCESS));
    ASSERT_EQ(static_cast<int>(ErrorCode::SUCCESS), player->SetSource(source));
    player->fsm_.DoTask();
    ASSERT_EQ("PreparingState", player->fsm_.curState_->GetName());
}

TEST_F(UtTestHiPlayer, Can_SetBufferSize)
{
    size_t size = 100;
    MOCK_METHOD(audioSource, SetBufferSize)
        .expects(once())
        .with(eq(size))
        .will(returnValue(ErrorCode::SUCCESS));
    ASSERT_EQ(ErrorCode::SUCCESS, player->SetBufferSize(size));
}
}
#endif
