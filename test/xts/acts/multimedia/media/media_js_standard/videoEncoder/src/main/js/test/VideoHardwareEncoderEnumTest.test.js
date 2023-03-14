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

describe('VideoHardwareEncoderEnum', function () {

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
        * @tc.number    : SUB_MEDIA_VIDEO_HARDWARE_ENCODER_ENUM_MediaDescriptionKey_0100
        * @tc.name      : 001.MediaDescriptionKey
        * @tc.desc      : Test Enumerate MediaDescriptionKey
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_HARDWARE_ENCODER_ENUM_MediaDescriptionKey_0100', 0, async function (done) {
        expect(media.MediaDescriptionKey.MD_KEY_SUSPEND_INPUT_SURFACE).assertEqual("suspend_input_surface");
        expect(media.MediaDescriptionKey.MD_KEY_VIDEO_ENCODE_BITRATE_MODE).assertEqual("video_encode_bitrate_mode");
        expect(media.MediaDescriptionKey.MD_KEY_PROFILE).assertEqual("codec_profile");        
        expect(media.MediaDescriptionKey.MD_KEY_QUALITY).assertEqual("codec_quality");
        expect(media.MediaDescriptionKey.MD_KEY_CUSTOM).assertEqual("vendor.custom");
        expect(media.MediaDescriptionKey.MD_KEY_MAX_INPUT_SIZE).assertEqual("max_input_size");        
        expect(media.MediaDescriptionKey.MD_KEY_MAX_ENCODER_FPS).assertEqual("max_encoder_fps");
        expect(media.MediaDescriptionKey.MD_KEY_WIDTH).assertEqual("width");
        expect(media.MediaDescriptionKey.MD_KEY_HEIGHT).assertEqual("height");        
        expect(media.MediaDescriptionKey.MD_KEY_PIXEL_FORMAT).assertEqual("pixel_format");
        expect(media.MediaDescriptionKey.MD_KEY_FRAME_RATE).assertEqual("frame_rate");
        expect(media.MediaDescriptionKey.MD_KEY_CAPTURE_RATE).assertEqual("capture_rate");        
        expect(media.MediaDescriptionKey.MD_KEY_I_FRAME_INTERVAL).assertEqual("i_frame_interval");
        expect(media.MediaDescriptionKey.MD_KEY_REQUEST_I_FRAME).assertEqual("req_i_frame");
        expect(media.MediaDescriptionKey.MD_KEY_REPEAT_FRAME_AFTER).assertEqual("repeat_frame_after");
        done();
    })
})