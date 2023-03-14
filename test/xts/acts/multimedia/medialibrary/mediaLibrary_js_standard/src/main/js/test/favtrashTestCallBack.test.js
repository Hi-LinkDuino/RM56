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

describe('file.callback.test.js', function () {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    beforeAll(function () {
        console.info('File Callback MediaLibraryTest: beforeAll: Prerequisites at the test suite level, which are executed before the test suite is executed.');
    });
    beforeEach(function () {
        console.info('File Callback MediaLibraryTest: beforeEach: Prerequisites at the test case level, which are executed before each test case is executed.');
    });
    afterEach(function () {
        console.info('File Callback MediaLibraryTest: afterEach: Test case-level clearance conditions, which are executed after each test case is executed.');
    });
    afterAll(function () {
        console.info('File Callback MediaLibraryTest: afterAll: Test suite-level cleanup condition, which is executed after the test suite is executed');
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_01
     * @tc.name      : favorite
     * @tc.desc      : favorite by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_01', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image01.jpg', path);
        asset.favorite(true, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_01 success');
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_01 fail, message = ' + err);
                expect(false).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_02
     * @tc.name      : favorite
     * @tc.desc      : favorite by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_02', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image02.jpg', path);
        asset.favorite(false, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_02 success');
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_02 fail, message = ' + err);
                expect(false).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_03
     * @tc.name      : favorite
     * @tc.desc      : favorite by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_03', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image03.jpg', path);
        asset.favorite(666, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_03 success');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_03 fail, message = ' + err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_04
     * @tc.name      : favorite
     * @tc.desc      : favorite by '666'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_04', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image04.jpg', path);
        asset.favorite('666', (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_04 success');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_04 fail, message = ' + err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_05
     * @tc.name      : favorite
     * @tc.desc      : favorite by 0.666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_05', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image05.jpg', path);
        asset.favorite(0.666, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_05 success');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_05 fail, message = ' + err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_06
     * @tc.name      : favorite
     * @tc.desc      : favorite by null
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_007_06', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image06.jpg', path);
        asset.favorite(null, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_06 success');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK favorite 007_06 fail, message = ' + err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_CALLBACK_008_01
     * @tc.name      : isFavorite
     * @tc.desc      : Is Favourite true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_CALLBACK_008_01', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image07.jpg', path);
        await asset.favorite(true);
        asset.isFavorite((err, isFavorite) => {
            if (err == undefined && isFavorite == true) {
                console.info('MediaLibraryTest : ASSET_CALLBACK isFavorite 008_01 isFavorite = ' + isFavorite);
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK isFavorite 008_01 fail, message = ' + err);
                expect(false).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_CALLBACK_008_02
     * @tc.name      : isFavorite
     * @tc.desc      : Is Favourite true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISFAV_ASSET_CALLBACK_008_02', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image0702.jpg', path);
        await asset.favorite(false);
        asset.isFavorite((err, isFavorite) => {
            if (err == undefined && isFavorite == false) {
                console.info('MediaLibraryTest : ASSET_CALLBACK isFavorite 008_02 isFavorite = ' + isFavorite);
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK isFavorite 008_02 fail, message = ' + err);
                expect(false).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_01
     * @tc.name      : trash
     * @tc.desc      : Trash by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_01', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image08.jpg', path);
        asset.trash(true, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_01');
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_01 fail ,message = ', err);
                expect(false).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_02
     * @tc.name      : trash
     * @tc.desc      : Trash by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_02', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image09.jpg', path);
        asset.trash(false, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_02');
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_02 fail ,message = ', err);
                expect(false).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_03
     * @tc.name      : trash
     * @tc.desc      : Trash by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_03', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image10.jpg', path);
        asset.trash(666, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_03');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_03 fail ,message = ', err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_04
     * @tc.name      : trash
     * @tc.desc      : Trash by '666'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_04', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image11.jpg', path);
        asset.trash('666', (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_04');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_04 fail ,message = ', err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_05
     * @tc.name      : trash
     * @tc.desc      : Trash by 0.666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_05', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image12.jpg', path);
        asset.trash(0.666, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_05');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_05 fail ,message = ', err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_06
     * @tc.name      : trash
     * @tc.desc      : Trash by null
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_TRA_ASSET_CALLBACK_009_06', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image13.jpg', path);
        asset.trash(null, (err) => {
            if (err == undefined) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_06');
                expect(false).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 009_06 fail ,message = ', err);
                expect(true).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_CALLBACK_0010_01
     * @tc.name      : isTrash
     * @tc.desc      : isTrash true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_CALLBACK_0010_01', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image14.jpg', path);
        await asset.trash(true);
        asset.isTrash((err, isTrash) => {
            if (err == undefined && isTrash == true) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 0010_01');
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 0010_01 fail ,message = ', err);
                expect(false).assertTrue();
                done();
            }
        });
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_CALLBACK_0010_02
     * @tc.name      : isTrash
     * @tc.desc      : isTrash false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_ISTRASH_ASSET_CALLBACK_0010_02', 0, async function (done) {
        const asset = await media.createAsset(mediaType, 'image15.jpg', path);
        await asset.trash(false);
        asset.isTrash((err, isTrash) => {
            if (err == undefined && isTrash == false) {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 0010_02');
                expect(true).assertTrue();
                done();
            } else {
                console.info('MediaLibraryTest : ASSET_CALLBACK trash 0010_02 fail ,message = ', err);
                expect(false).assertTrue();
                done();
            }
        });
    });
});
