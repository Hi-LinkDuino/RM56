/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import featureAbility from '@ohos.ability.featureAbility';

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index';

describe('file.promise.test.js', function () {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    beforeAll(function () {
        console.info('File Promise MediaLibraryTest: beforeAll : Prerequisites at the test suite level, which are executed before the test suite is executed.');
    });
    beforeEach(function () {
        console.info('File Promise MediaLibraryTest: beforeEach: Prerequisites at the test case level, which are executed before each test case is executed.');
    });
    afterEach(function () {
        console.info('File Promise MediaLibraryTest: afterEach: Test case-level clearance conditions, which are executed after each test case is executed.');
    });
    afterAll(function () {
        console.info('File Promise MediaLibraryTest: afterAll: Test suite-level cleanup condition, which is executed after the test suite is executed');
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_01
     * @tc.name      : favorite
     * @tc.desc      : favorite by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_01', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image01.jpg', path);
            await asset.favorite(true);
            console.info('MediaLibraryTest : ASSET_PROMISE favorite 007_01 success');
            expect(true).assertTrue();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE close 007_01 fail, message = ' + error);
            expect(false).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_02
     * @tc.name      : favorite
     * @tc.desc      : favorite by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_02', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image02.jpg', path);
            await asset.favorite(false);
            console.info('MediaLibraryTest : ASSET_PROMISE favorite 007_02 success');
            expect(true).assertTrue();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE close 007_02 fail, message = ' + error);
            expect(false).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_03
     * @tc.name      : favorite
     * @tc.desc      : favorite by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_03', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image03.jpg', path);
            await asset.favorite(666);
            console.info('MediaLibraryTest : ASSET_PROMISE favorite 007_03 success');
            expect(false).assertTrue();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE close 007_03 fail, message = ' + error);
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_04
     * @tc.name      : favorite
     * @tc.desc      : favorite by '666'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_04', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image04.jpg', path);
            await asset.favorite('666');
            console.info('MediaLibraryTest : ASSET_PROMISE favorite 007_04 success');
            expect(false).assertTrue();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE close 007_04 fail, message = ' + error);
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_05
     * @tc.name      : favorite
     * @tc.desc      : favorite by 0.666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_05', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image05.jpg', path);
            await asset.favorite(0.666);
            console.info('MediaLibraryTest : ASSET_PROMISE favorite 007_05 success');
            expect(false).assertTrue();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE close 007_05 fail, message = ' + error);
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_06
     * @tc.name      : favorite
     * @tc.desc      : favorite by null
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_PROMISE_007_06', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image06.jpg', path);
            await asset.favorite();
            console.info('MediaLibraryTest : ASSET_PROMISE favorite 007_06 success');
            expect(false).assertTrue();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE close 007_06 fail, message = ' + error);
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_PROMISE_008_01
     * @tc.name      : isFavorite
     * @tc.desc      : Is Favourite
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_PROMISE_008_01', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image07.jpg', path);
            await asset.favorite(true);
            let isFavorite = await asset.isFavorite();
            if (isFavorite == true) {
                console.info('MediaLibraryTest : ASSET_PROMISE isFavorite = ' + isFavorite);
                expect(true).assertTrue();
            } else {
                console.info('MediaLibraryTest : ASSET_PROMISE isFavorite = ' + isFavorite);
                expect(true).assertTrue();
            }
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE isFavorite fail, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_PROMISE_008_02
     * @tc.name      : isFavorite
     * @tc.desc      : Is Favourite
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_PROMISE_008_02', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image0702.jpg', path);
            await asset.favorite(false);
            let isFavorite = await asset.isFavorite();
            if (isFavorite == false) {
                console.info('MediaLibraryTest : ASSET_PROMISE isFavorite = ' + isFavorite);
                expect(true).assertTrue();
            } else {
                console.info('MediaLibraryTest : ASSET_PROMISE isFavorite = ' + isFavorite);
                expect(true).assertTrue();
            }
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE isFavorite fail, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_01
     * @tc.name      : trash
     * @tc.desc      : Trash by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_01', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image08.jpg', path);
            await asset.trash(true);
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_01');
            expect(true).assertTrue();
            done();
        } catch (trashError) {
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_01 fail ,message = ', trashError);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_02
     * @tc.name      : trash
     * @tc.desc      : Trash by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_02', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image09.jpg', path);
            await asset.trash(false);
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_02');
            expect(true).assertTrue();
            done();
        } catch (trashError) {
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_02 fail ,message = ', trashError);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_03
     * @tc.name      : trash
     * @tc.desc      : Trash by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_03', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image10.jpg', path);
            await asset.trash(666);
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_03');
            expect(false).assertTrue();
            done();
        } catch (trashError) {
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_03 fail ,message = ' + trashError);
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_04
     * @tc.name      : trash
     * @tc.desc      : Trash by '666'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_04', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image11.jpg', path);
            await asset.trash('666');
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_04');
            expect(false).assertTrue();
            done();
        } catch (trashError) {
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_04 fail ,message = ' + trashError);
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_05
     * @tc.name      : trash
     * @tc.desc      : Trash by 0.666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_05', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image12.jpg', path);
            await asset.trash(0.666);
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_05');
            expect(false).assertTrue();
            done();
        } catch (trashError) {
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_05 fail ,message = ' + trashError);
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_06
     * @tc.name      : trash
     * @tc.desc      : Trash by null
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_PROMISE_009_06', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image13.jpg', path);
            await asset.trash();
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_06');
            expect(false).assertTrue();
            done();
        } catch (trashError) {
            console.info('MediaLibraryTest : ASSET_PROMISE trash 009_06 fail ,message = ' + trashError);
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_PROMISE_0010_01
     * @tc.name      : isTrash
     * @tc.desc      : isTrash true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_PROMISE_0010_01', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image14.jpg', path);
            await asset.trash(true);
            let isTrash = await asset.isTrash();
            console.info('MediaLibraryTest : ASSET_PROMISE Trash 0010_01 = ' + isTrash);
            if (isTrash) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE isTrash 0010_01 fail, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_PROMISE_0010_02
     * @tc.name      : isTrash
     * @tc.desc      : isTrash false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_PROMISE_0010_02', 0, async function (done) {
        try {
            const asset = await media.createAsset(mediaType, 'image15.jpg', path);
            await asset.trash(false);
            let isTrash = await asset.isTrash();
            console.info('MediaLibraryTest : ASSET_PROMISE Trash 0010_02 = ' + isTrash);
            if (!isTrash) {
                expect(true).assertTrue();
            } else {
                expect(false).assertTrue();
            }
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ASSET_PROMISE isTrash 0010_02 fail, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
});
