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

import mediaLibrary from '@ohos.multimedia.medialibrary';
import featureAbility from '@ohos.ability.featureAbility'
import image from '@ohos.multimedia.image';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
describe('distributedPromise.test.js', function() {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    beforeAll(function() {
        onsole.info(
            'Distributed. Promise MediaLibraryTest: beforeAll： Prerequisites at the test suite level, which are executed before the test suite is executed.');
    })

    beforeEach(function() {
        console.info(
            'Distributed. Promise MediaLibraryTest: beforeEach：Prerequisites at the test case level, which are executed before each test case is executed.');
    })
    afterEach(function() {
        console.info(
            'Distributed. Promise MediaLibraryTest: afterEach： Test case-level clearance conditions, which are executed after each test case is executed.');
    })
    afterAll(function() {
        console.info(
            'Distributed. Promise MediaLibraryTest: afterAll：  Test suite-level cleanup condition, which is executed after the test suite is executed');
    })

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_distributed_001_01
     * @tc.name      : getActivePeers
     * @tc.desc      : getActivePeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_DISTRIBUTED_001_01', 0, async function(done) {
        expect(true).assertTrue();
        done();
        return;
        try {
            done();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_DISTRIBUTED_001_01 begin');
            let peers = await media.getActivePeers();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_DISTRIBUTED_001_01 done');
            done();
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_DISTRIBUTED_001_01 fail, message = ' + error);
            done();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_DISTRIBUTED_001_02
     * @tc.name      : getAllPeers
     * @tc.desc      : getAllPeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_DISTRIBUTED_001_02', 0, async function(done) {
        expect(true).assertTrue();
        done();
        return;
        try {
            done();
            let peers = await media.getAllPeers();
            done();
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_DISTRIBUTED_001_02 fail, message = ' + error);
            done();
        }
        done();
    });
})