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

describe('VideoEncoderEnum', function () {

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
        * @tc.number    : SUB_MEDIA_VIDEO_ENCODER_ENUM_VideoEncodeBitrateMode_0100
        * @tc.name      : 001.VideoEncodeBitrateMode
        * @tc.desc      : Test Enumerate VideoEncodeBitrateMode
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_VIDEO_DECODER_ENUM_VideoEncodeBitrateMode_0100', 0, async function (done) {
        expect(media.VideoEncodeBitrateMode.CBR).assertEqual(0);
        expect(media.VideoEncodeBitrateMode.VBR).assertEqual(1);
        expect(media.VideoEncodeBitrateMode.CQ).assertEqual(2);
        done();
    })
})