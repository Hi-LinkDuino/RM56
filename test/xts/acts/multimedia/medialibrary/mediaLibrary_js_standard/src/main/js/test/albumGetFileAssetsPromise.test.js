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
import featureAbility from '@ohos.ability.featureAbility';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('albumGetFileAssetsPromise.test.js', async function() {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    beforeAll(function() {});
    beforeEach(function() {});
    afterEach(function() {});
    afterAll(function() {});

    const fileKeyObj = mediaLibrary.FileKey;

    let imageType = mediaLibrary.MediaType.IMAGE;
    let videoType = mediaLibrary.MediaType.VIDEO;
    let audioType = mediaLibrary.MediaType.AUDIO;
    const count = 3;

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_01
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type all
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_01', 0, async function(done) {
        try {
            let allTypefetchOp = {
                selections : '',
                selectionArgs : [],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(allTypefetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_02
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type image
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_02', 0, async function(done) {
        try {
            let imageAlbumfetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ imageType.toString() ],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(imageAlbumfetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(imageAlbumfetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_03
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_03', 0, async function(done) {
        try {
            let audioAlbumfetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ audioType.toString() ],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(audioAlbumfetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(audioAlbumfetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_04
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type video
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_04', 0, async function(done) {
        try {
            let videoAlbumfetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ videoType.toString() ],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(videoAlbumfetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(videoAlbumfetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_05
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type image and video
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_05', 0, async function(done) {
        try {
            let imageAndVideoAlbumfetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ imageType.toString(), videoType.toString() ],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(imageAndVideoAlbumfetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(imageAndVideoAlbumfetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_06
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type image and audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_06', 0, async function(done) {
        try {
            let imageAndAudioAlbumfetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ imageType.toString(), audioType.toString() ],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(imageAndAudioAlbumfetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(imageAndAudioAlbumfetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**  order: 'date_added DESC LIMIT 0,500'
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_07
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type video and audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_07', 0, async function(done) {
        try {
            let videoAndAudioAlbumfetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ videoType.toString(), audioType.toString() ],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(videoAndAudioAlbumfetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(videoAndAudioAlbumfetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_08
     * @tc.name      : getFileAssets
     * @tc.desc      : more file type image video and audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_08', 0, async function(done) {
        try {
            let imgAndVideoAndAudioAlbumfetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ? or '
                                 + fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ imageType.toString(), videoType.toString(), audioType.toString() ],
                order : 'date_added DESC LIMIT 0,3',
            };
            const albumList = await media.getAlbums(imgAndVideoAndAudioAlbumfetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(imgAndVideoAndAudioAlbumfetchOp);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_09
     * @tc.name      : getFileAssets
     * @tc.desc      : one file tpe image
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_09', 0, async function(done) {
        try {
            let albumfetchOpOne = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ imageType.toString() ],
                order : 'date_added DESC LIMIT 0,1',
            };
            const albumList = await media.getAlbums(albumfetchOpOne);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(albumfetchOpOne);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_10
     * @tc.name      : getFileAssets
     * @tc.desc      : one file type audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_10', 0, async function(done) {
        try {
            let albumfetchOpOne = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ audioType.toString() ],
                order : 'date_added DESC LIMIT 0,1',
            };
            const albumList = await media.getAlbums(albumfetchOpOne);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(albumfetchOpOne);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_11
     * @tc.name      : getFileAssets
     * @tc.desc      : one file type audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_11', 0, async function(done) {
        try {
            let albumfetchOpOne = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ videoType.toString() ],
                order : 'date_added DESC LIMIT 0,1',
            };
            const albumList = await media.getAlbums(albumfetchOpOne);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(albumfetchOpOne);
            expect(fetchFileResult.getCount() <= count).assertTrue();
            done();
        } catch (error) {
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_12
     * @tc.name      : getFileAssets
     * @tc.desc      : no file type image
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_12', 0, async function(done) {
        try {
            let albumfetchOpNone = {
                selections : fileKeyObj.MEDIA_TYPE + '= ? AND date_added < 0',
                selectionArgs : [ imageType.toString() ],
                order : 'date_added DESC LIMIT 0,1',
            };
            const albumList = await media.getAlbums(albumfetchOpNone);
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_12 albumList.length:'
                         + albumList.length);
            expect(albumList.length == 0).assertTrue();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_12 done');
            done();
        } catch (error) {
            expect(false).assertTrue();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_12 error:' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_13
     * @tc.name      : getFileAssets
     * @tc.desc      : no file type audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_13', 0, async function(done) {
        try {
            let albumfetchOpNone = {
                selections : fileKeyObj.MEDIA_TYPE + '= ? AND date_added < 0',
                selectionArgs : [ audioType.toString() ],
                order : 'date_added DESC LIMIT 0,1',
            };
            const albumList = await media.getAlbums(albumfetchOpNone);
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_13 albumList.length:'
                         + albumList.length);
            expect(albumList.length == 0).assertTrue();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_13 done');
            done();
        } catch (error) {
            expect(false).assertTrue();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_13 error:' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_14
     * @tc.name      : getFileAssets
     * @tc.desc      : no file type video
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_14', 0, async function(done) {
        try {
            let albumfetchOpNone = {
                selections : fileKeyObj.MEDIA_TYPE + '= ? AND date_added < 0',
                selectionArgs : [ videoType.toString() ],
                order : 'date_added DESC LIMIT 0,1',
            };
            const albumList = await media.getAlbums(albumfetchOpNone);
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_14 albumList.length:'
                         + albumList.length);
            expect(albumList.length == 0).assertTrue();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_14 done');
            done();
        } catch (error) {
            expect(false).assertTrue();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALBUM_GET_ASSETS_PROMISE_001_13 error:' + error);
            done();
        }
    });
});