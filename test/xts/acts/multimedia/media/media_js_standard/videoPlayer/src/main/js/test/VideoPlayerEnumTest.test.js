/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import media from '@ohos.multimedia.media'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('VideoPlayerEnumTest', function () {
    beforeAll(function() {
        console.info('beforeAll case');
    })

    beforeEach(function() {
        console.info('beforeEach case');
    })

    afterEach(function() {
        console.info('afterEach case');
    })

    afterAll(function() {
        console.info('afterAll case');
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_SEEK_MODE_00_0100
        * @tc.name      : SeekMode
        * @tc.desc      : SeekMode Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_ENUM_SEEK_MODE_00_0100', 0, async function (done) {
        expect(media.SeekMode.SEEK_NEXT_SYNC).assertEqual(0);
        expect(media.SeekMode.SEEK_PREV_SYNC).assertEqual(1);
        expect(media.SeekMode.SEEK_CLOSEST_SYNC).assertEqual(2);
        expect(media.SeekMode.SEEK_CLOSEST).assertEqual(3);
        done();
    })

     /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_MEDIA_ERROR_CODE_00_0100
        * @tc.name      : MediaErrorCode
        * @tc.desc      : MediaErrorCode Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
     it('SUB_MEDIA_VIDEO_PLAYER_ENUM_MEDIA_ERROR_CODE_00_0100', 0, async function (done) {
        expect(media.MediaErrorCode.MSERR_OK).assertEqual(0);
        expect(media.MediaErrorCode.MSERR_NO_MEMORY).assertEqual(1);
        expect(media.MediaErrorCode.MSERR_OPERATION_NOT_PERMIT).assertEqual(2);
        expect(media.MediaErrorCode.MSERR_INVALID_VAL).assertEqual(3);
        expect(media.MediaErrorCode.MSERR_IO).assertEqual(4);
        expect(media.MediaErrorCode.MSERR_TIMEOUT).assertEqual(5);
        expect(media.MediaErrorCode.MSERR_UNKNOWN).assertEqual(6);
        expect(media.MediaErrorCode.MSERR_SERVICE_DIED).assertEqual(7);
        expect(media.MediaErrorCode.MSERR_INVALID_STATE).assertEqual(8);
        expect(media.MediaErrorCode.MSERR_UNSUPPORTED).assertEqual(9);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_AV_DATA_SOURCE_ERROR_00_0100
        * @tc.name      : AVDataSourceError
        * @tc.desc      : AVDataSourceError Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_ENUM_AV_DATA_SOURCE_ERROR_00_0100', 0, async function (done) {
        expect(media.AVDataSourceError.SOURCE_ERROR_IO).assertEqual(-2);
        expect(media.AVDataSourceError.SOURCE_ERROR_EOF).assertEqual(-1);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_BUFFERING_INFO_TYPE_00_0100
        * @tc.name      : BufferingInfoType
        * @tc.desc      : BufferingInfoType Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_ENUM_BUFFERING_INFO_TYPE_00_0100', 0, async function (done) {
        expect(media.BufferingInfoType.BUFFERING_START).assertEqual(1);
        expect(media.BufferingInfoType.BUFFERING_END).assertEqual(2);
        expect(media.BufferingInfoType.BUFFERING_PERCENT).assertEqual(3);
        expect(media.BufferingInfoType.CACHED_DURATION).assertEqual(4);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_PLAYBACK_SPEED_00_0100
        * @tc.name      : PlaybackSpeed
        * @tc.desc      : PlaybackSpeed Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_ENUM_PLAYBACK_SPEED_00_0100', 0, async function (done) {
        expect(media.PlaybackSpeed.SPEED_FORWARD_0_75_X).assertEqual(0);
        expect(media.PlaybackSpeed.SPEED_FORWARD_1_00_X).assertEqual(1);
        expect(media.PlaybackSpeed.SPEED_FORWARD_1_25_X).assertEqual(2);
        expect(media.PlaybackSpeed.SPEED_FORWARD_1_75_X).assertEqual(3);
        expect(media.PlaybackSpeed.SPEED_FORWARD_2_00_X).assertEqual(4);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_CONTAINER_FORMAT_TYPE_00_0100
        * @tc.name      : ContainerFormatType
        * @tc.desc      : ContainerFormatType Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_ENUM_CONTAINER_FORMAT_TYPE_00_0100', 0, async function (done) {
        expect(media.ContainerFormatType.CFT_MPEG_4).assertEqual("mp4");
        expect(media.ContainerFormatType.CFT_MPEG_4A).assertEqual("m4a");
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_MEDIA_TYPE_00_0100
        * @tc.name      : MediaType
        * @tc.desc      : MediaType Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_ENUM_MEDIA_TYPE_00_0100', 0, async function (done) {
        expect(media.MediaType.MEDIA_TYPE_AUD).assertEqual(0);
        expect(media.MediaType.MEDIA_TYPE_VID).assertEqual(1);
        expect(media.MediaType.MEDIA_TYPE_SUBTITLE).assertEqual(2);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_VIDEO_PLAYER_ENUM_MEDIA_DESCRIPTION_KEY_00_0100
        * @tc.name      : MediaDescriptionKey
        * @tc.desc      : MediaDescriptionKey Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_PLAYER_ENUM_MEDIA_DESCRIPTION_KEY_00_0100', 0, async function (done) {
        expect(media.MediaDescriptionKey.MD_KEY_TRACK_INDEX).assertEqual("track_index");
        expect(media.MediaDescriptionKey.MD_KEY_TRACK_TYPE).assertEqual("track_type");
        expect(media.MediaDescriptionKey.MD_KEY_CODEC_MIME).assertEqual("codec_mime");
        expect(media.MediaDescriptionKey.MD_KEY_DURATION).assertEqual("duration");
        expect(media.MediaDescriptionKey.MD_KEY_BITRATE).assertEqual("bitrate");
        expect(media.MediaDescriptionKey.MD_KEY_WIDTH).assertEqual("width");
        expect(media.MediaDescriptionKey.MD_KEY_HEIGHT).assertEqual("height");
        expect(media.MediaDescriptionKey.MD_KEY_AUD_CHANNEL_COUNT).assertEqual("channel_count");
        expect(media.MediaDescriptionKey.MD_KEY_AUD_SAMPLE_RATE).assertEqual("sample_rate");
        done();
    })
})