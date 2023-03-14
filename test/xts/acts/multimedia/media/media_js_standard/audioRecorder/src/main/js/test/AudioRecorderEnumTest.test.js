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

describe('AudioRecorderEnumTest', function () {
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
        * @tc.number    : SUB_MEDIA_AUDIO_RECORDER_ENUM_AUDIO_OUTPUT_FORMAT_00_0100
        * @tc.name      : AudioOutputFormat
        * @tc.desc      : AudioOutputFormat Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_RECORDER_ENUM_AUDIO_OUTPUT_FORMAT_00_0100', 0, async function (done) {
        expect(media.AudioOutputFormat.MPEG_4).assertEqual(2);
        expect(media.AudioOutputFormat.AAC_ADTS).assertEqual(6);
        done();
    })

    /* *
        * @tc.number    : SUB_MEDIA_AUDIO_RECORDER_ENUM_AUDIO_ENCODER_00_0100
        * @tc.name      : AudioOutputFormat
        * @tc.desc      : AudioOutputFormat Enumeration test
        * @tc.size      : MediumTest
        * @tc.type      : Function test
        * @tc.level     : Level0
    */
    it('SUB_MEDIA_AUDIO_RECORDER_ENUM_AUDIO_ENCODER_00_0100', 0, async function (done) {
        expect(media.AudioEncoder.AAC_LC).assertEqual(3);
        done();
    })
})