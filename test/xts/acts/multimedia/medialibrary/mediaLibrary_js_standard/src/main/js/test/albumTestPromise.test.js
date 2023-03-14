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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index';
let fileKeyObj = mediaLibrary.FileKey;
let imageType = mediaLibrary.MediaType.IMAGE;
let videoType = mediaLibrary.MediaType.VIDEO;
let audioType = mediaLibrary.MediaType.AUDIO;

let allTypefetchOp = {
    selections: '',
    selectionArgs: [],
};
let albumDeletefetchOp = {
    selections: fileKeyObj.RELATIVE_PATH + '= ? AND ' + fileKeyObj.ALBUM_NAME + '= ?',
    selectionArgs: ['Pictures/','DeleteAlbumPro'],
};
let albumCoverUrifetchOp = {
    selections: fileKeyObj.RELATIVE_PATH + '= ? AND ' + fileKeyObj.ALBUM_NAME + '= ?',
    selectionArgs: ['Pictures/','weixin'],
};
let imageAlbumfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [imageType.toString()],
};
let videoAlbumfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [videoType.toString()],
};
let audioAlbumfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [audioType.toString()],
};
let imageAndVideoAlbumfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [imageType.toString(), videoType.toString()],
};
let imageAndAudioAlbumfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [imageType.toString(), audioType.toString()],
};
let videoAndAudioAlbumfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [videoType.toString(), audioType.toString()],
};
function printAlbumMessage(testNum, album) {
    console.info(`ALBUM_CALLBACK getAlbum ${testNum} album.albumId: ${album.albumId}`);
    console.info(`ALBUM_CALLBACK getAlbum ${testNum} album.albumName: ${album.albumName}`);
    console.info(`ALBUM_CALLBACK getAlbum ${testNum} album.albumUri: ${album.albumUri}`);
    console.info(`ALBUM_CALLBACK getAlbum ${testNum} album.dateModified: ${album.dateModified}`);
    console.info(`ALBUM_CALLBACK getAlbum ${testNum} album.count: ${album.count}`);
    console.info(`ALBUM_CALLBACK getAlbum ${testNum} album.relativePath: ${album.relativePath}`);
    console.info(`ALBUM_CALLBACK getAlbum ${testNum} album.coverUri: ${album.coverUri}`);
}
function checkAlbumAttr(done, album) {
    if (
        album.albumId == undefined ||
        album.albumName == undefined ||
        album.albumUri == undefined ||
        album.count == undefined ||
        album.relativePath == undefined ||
        album.coverUri == undefined
    ) {
        console.info('ALBUM_PROMISE getAlbum 001_01 failed');
        expect(false).assertTrue();
        done();
    }
}

describe('albumTestPromise.test.js', async function () {
    var context = featureAbility.getContext();
    var media = mediaLibrary.getMediaLibrary(context);
    beforeAll(function () {});
    beforeEach(function () {});
    afterEach(function () {});
    afterAll(function () {});

    // ------------------------------ 001 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_01
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by AllTypefetchOp, print all album info,
     *                 print all asset info, check asset info (mediaType, albumId, albumUri, albumName)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_01', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            printAlbumMessage('001_01', album);
            checkAlbumAttr(done, album);

            console.info('ALBUM_PROMISE getAlbum 001_01 success');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 001_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_02
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by imageAlbumfetchOp, print all album info,
     *                 print all asset info, check asset info (mediaType, albumId, albumUri, albumName)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_02', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(imageAlbumfetchOp);
            const album = albumList[0];
            printAlbumMessage('001_02', album);
            checkAlbumAttr(done, album);

            console.info('ALBUM_PROMISE getAlbum 001_02 success');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 001_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_03
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by videoAlbumfetchOp, print all album info,
     *                 print all asset info, check asset info (mediaType, albumId, albumUri, albumName)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_03', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(videoAlbumfetchOp);
            const album = albumList[0];
            printAlbumMessage('001_03', album);
            checkAlbumAttr(done, album);

            console.info('ALBUM_PROMISE getAlbum 001_03 passed');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 001_03 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_04
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by audioAlbumfetchOp, print all album info,
     *                 print all asset info, check asset info (mediaType, albumId, albumUri, albumName)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_04', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(audioAlbumfetchOp);
            const album = albumList[0];
            printAlbumMessage('001_04', album);
            checkAlbumAttr(done, album);

            console.info('ALBUM_PROMISE getAlbum 001_04 passed');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 001_04 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_05
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by imageAndVideoAlbumfetchOp, print all album info,
     *                 print all asset info, check asset info (mediaType, albumId, albumUri, albumName),
     *                 check media types (imageType, audioType)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_05', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(imageAndVideoAlbumfetchOp);
            const album = albumList[0];

            printAlbumMessage('001_05', album);
            checkAlbumAttr(done, album);

            console.info('ALBUM_PROMISE getAlbum 001_05 passed');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 001_05 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_06
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by imageAndAudioAlbumfetchOp, print all album info,
     *                 print all asset info, check asset info (mediaType, albumId, albumUri, albumName),
     *                 check media types (imageType, audioType)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_06', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(imageAndAudioAlbumfetchOp);
            const album = albumList[0];
            printAlbumMessage('001_06', album);
            checkAlbumAttr(done, album);

            console.info('ALBUM_PROMISE getAlbum 001_06 passed');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 001_06 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_07
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by videoAndAudioAlbumfetchOp, print all album info,
     *                 print all asset info, check asset info (mediaType, albumId, albumUri, albumName),
     *                 check media types (imageType, audioType)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_001_07', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(videoAndAudioAlbumfetchOp);
            const album = albumList[0];
            printAlbumMessage('001_07', album);
            checkAlbumAttr(done, album);

            console.info('ALBUM_PROMISE getAlbum 001_07 passed');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 001_07 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 001 test end -------------------------

    // ------------------------------ 002 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_01
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_01', 0, async function (done) {
        try {
            await media.getAlbums(666);
            expect(false).assertTrue();
            console.info('ALBUM_PROMISE getalbum 002_01 failed');
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getalbum 002_01 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_02
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by '666'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_02', 0, async function (done) {
        try {
            await media.getAlbums('666');
            console.info('ALBUM_PROMISE getalbum 002_02 failed');
            expect(false).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getalbum 002_02 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_03
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by 0
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_03', 0, async function (done) {
        try {
            await media.getAlbums(0);
            console.info('ALBUM_PROMISE getalbum 002_03 failed');
            expect(false).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getalbum 002_03 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_04
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_04', 0, async function (done) {
        try {
            await media.getAlbums(true);
            console.info('ALBUM_PROMISE getalbum 002_04 failed');
            expect(false).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getalbum 002_04 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_05
     * @tc.name      : getAlbums
     * @tc.desc      : Get Album by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_002_05', 0, async function (done) {
        try {
            await media.getAlbums(false);
            console.info('ALBUM_PROMISE getalbum 002_05 failed');
            expect(false).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getalbum 002_05 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUMASSETS_PROMISE_002_06
     * @tc.name      : album.getFileAssets
     * @tc.desc      : Get Album Assets by fileHasArgsfetchOp3
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUMASSETS_PROMISE_002_06', 0, async function (done) {
        let fileHasArgsfetchOp3 = {
            selections: fileKeyObj.MEDIA_TYPE + ' = ?',
            selectionArgs: ['666'],
        };
        try {
            const albumList = await media.getAlbums(fileHasArgsfetchOp3);
            console.info('GETALBUMASSETS_PROMISE_002_06 length:' + albumList.length);
            expect(albumList.length == 0).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getFileAssets 002_06 passed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUMASSETS_PROMISE_002_07
     * @tc.name      : album.getFileAssets
     * @tc.desc      : Get Album Assets by fileHasArgsfetchOp4
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUMASSETS_PROMISE_002_07', 0, async function (done) {
        let fileHasArgsfetchOp4 = {
            selections: '666' + '= ?',
            selectionArgs: [videoType.toString()],
        };
        try {
            const albumList = await media.getAlbums(fileHasArgsfetchOp4);
            console.info('GETALBUMASSETS_PROMISE_002_07 length:' + albumList.length);
            expect(albumList.length == 0).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getFileAssets 002_07 passed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUMASSETS_PROMISE_002_08
     * @tc.name      : album.getFileAssets
     * @tc.desc      : Get Album Assets by fileHasArgsfetchOp5
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUMASSETS_PROMISE_002_08', 0, async function (done) {
        let fileHasArgsfetchOp5 = {
            selections: '666' + '= ?',
            selectionArgs: ['666'],
        };

        try {
            const albumList = await media.getAlbums(fileHasArgsfetchOp5);
            console.info('GETALBUMASSETS_PROMISE_002_08 length:' + albumList.length);
            expect(albumList.length == 0).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getFileAssets 002_08 passed');
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 002 test end -------------------------

    // ------------------------------ 003 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_01
     * @tc.name      : commitModify
     * @tc.desc      : Modify Album name to 'hello'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_01', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            const albumId = album.albumId;

            console.info('ALBUM_PROMISE Modify 003_01 album.albumName(old) = ' + album.albumName);
            const newName = 'newhello';
            album.albumName = newName;

            await album.commitModify();
            const newAlbumList = await media.getAlbums(allTypefetchOp);
            let passed = false;
            for (let i = 0; i < newAlbumList.length; i++) {
                const album = newAlbumList[i];
                if (album.albumId == albumId && album.albumName == newName) {
                    console.info('ALBUM_PROMISE Modify 003_01 passed');
                    expect(true).assertTrue();
                    done();
                    passed = true;
                }
            }
            expect(passed).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE Modify 003_01 failed, message = ' + error);
            expect(false).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_02
     * @tc.name      : commitModify
     * @tc.desc      : Modify Album name  ''
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_02', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            const albumId = album.albumId;

            console.info('ALBUM_PROMISE Modify 003_02 album.albumName(old) = ' + album.albumName);
            const newName = '';
            album.albumName = newName;

            await album.commitModify();
            const newAlbumList = await media.getAlbums(allTypefetchOp);
            let changed = false;
            for (let i = 0; i < newAlbumList.length; i++) {
                const album = newAlbumList[i];
                if (album.albumId == albumId && album.albumName == newName) {
                    console.info('ALBUM_PROMISE Modify 003_02 failed');
                    expect(false).assertTrue();
                    done();
                    changed = true;
                }
            }
            expect(!changed).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE Modify 003_02 passed');
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_04
     * @tc.name      : commitModify
     * @tc.desc      : Modify Album name  'i123456...119'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_04', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            const albumId = album.albumId;

            console.info('ALBUM_PROMISE Modify 003_04 album.albumName(old) = ' + album.albumName);
            let newName = true;
            for (var i = 0; i < 1200; i++) {
                newName += 'i';
            }
            album.albumName = newName;

            await album.commitModify();
            const newAlbumList = await media.getAlbums(allTypefetchOp);
            let changed = false;
            for (let i = 0; i < newAlbumList.length; i++) {
                const album = newAlbumList[i];
                if (album.albumId == albumId && album.albumName == newName) {
                    console.info('ALBUM_PROMISE Modify 003_04 failed');
                    expect(false).assertTrue();
                    done();
                    changed = true;
                }
            }
            expect(!changed).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE Modify 003_04 passed');
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_05
     * @tc.name      : commitModify
     * @tc.desc      : Modify Album name  true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_05', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            const albumId = album.albumId;

            console.info('ALBUM_PROMISE Modify 003_05 album.albumName(old) = ' + album.albumName);
            const newName = true;
            album.albumName = newName;

            await album.commitModify();
            const newAlbumList = await media.getAlbums(allTypefetchOp);
            let changed = false;
            for (let i = 0; i < newAlbumList.length; i++) {
                const album = newAlbumList[i];
                if (album.albumId == albumId && album.albumName == newName) {
                    console.info('ALBUM_PROMISE Modify 003_05 failed');
                    expect(false).assertTrue();
                    done();
                    changed = true;
                }
            }
            expect(!changed).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE Modify 003_05 passed');
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_06
     * @tc.name      : commitModify
     * @tc.desc      : Modify Album name  false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_003_06', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            const albumId = album.albumId;

            console.info('ALBUM_PROMISE Modify 003_06 album.albumName(old) = ' + album.albumName);
            const newName = false;
            album.albumName = newName;

            await album.commitModify();
            const newAlbumList = await media.getAlbums(allTypefetchOp);
            let changed = false;
            for (let i = 0; i < newAlbumList.length; i++) {
                const album = newAlbumList[i];
                if (album.albumId == albumId && album.albumName == newName) {
                    console.info('ALBUM_PROMISE Modify 003_06 failed');
                    expect(false).assertTrue();
                    done();
                    changed = true;
                }
            }
            expect(!changed).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE Modify 003_06 passed');
            expect(true).assertTrue();
        }
        done();
    });
    // ------------------------------ 003 test end -------------------------

    // ------------------------------ 004 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_004_01
     * @tc.name      : album.coverUri
     * @tc.desc      : check album.coverUri
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_004_01', 0, async function (done) {
        try {
            let coverUrifetchOp = {
                selections: '',
                selectionArgs: [],
                order: 'date_added DESC LIMIT 0,1',
            };
            const albumList = await media.getAlbums(albumCoverUrifetchOp);
            const album = albumList[0];
            console.info('PROMISE getAlbum 004_01 album name = ' + album.albumName);
            console.info('PROMISE getAlbum 004_01 album id = ' + album.albumId);
            const fetchFileResult = await album.getFileAssets(coverUrifetchOp);
            const asset = await fetchFileResult.getFirstObject();
            if (asset == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                console.info('PROMISE getAlbum 004_01 coveruri = ' + album.coverUri);
                console.info('PROMISE getAlbum 004_01 asset.uri = ' + asset.uri);
                expect(asset.uri == album.coverUri).assertTrue();
                done();
            }
        } catch (error) {
            console.info('PROMISE getAlbum 004_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 004 test end -------------------------

    // ------------------------------ 006 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_006_01
     * @tc.name      : commitModify
     * @tc.desc      : Modify Album albumUri
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_006_01', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            album.albumUri = 'testalbumUri';
            await album.commitModify();
            console.info('ALBUM_PROMISE Modify 006_01 failed');
            expect(false).assertTrue();
        } catch (error) {
            console.info('ALBUM_PROMISE Modify 006_01 passed');
            expect(true).assertTrue();
        }
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_006_02
     * @tc.name      : commitModify
     * @tc.desc      : Modify Album name  false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_MODIFYALBUM_PROMISE_006_02', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            album.coverUri = 'testcoverUri';
            await album.commitModify();
            console.info('ALBUM_PROMISE Modify 006_02 failed');
            expect(false).assertTrue();
        } catch (error) {
            console.info('ALBUM_PROMISE Modify 006_02 passed');
            expect(true).assertTrue();
        }
        done();
    });
    // ------------------------------ 006 test end -------------------------

    // ------------------------------ 005 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_005_01
     * @tc.name      : deleteAsset
     * @tc.desc      : delete album
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */

    it('SUB_MEDIA_MEDIALIBRARY_GETALBUM_PROMISE_005_01', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(albumDeletefetchOp);

            const album = albumList[0];
            let fetchFileResult = await album.getFileAssets(allTypefetchOp);

            let datas = await fetchFileResult.getAllObject();

            for (let j = 0; j < datas.length; j++) {
                const asset = datas[j];
                await media.deleteAsset(asset.uri);
            }

            const albumId = album.albumId;
            const newAlbumList = await media.getAlbums(allTypefetchOp);

            for (let i = 0; i < newAlbumList.length; i++) {
                const album = newAlbumList[i];
                if (album.albumId == albumId) {
                    console.info('ALBUM_PROMISE getAlbum 005_01 failed');
                    expect(false).assertTrue();
                    done();
                }
            }
            console.info('ALBUM_PROMISE getAlbum 005_01 passed');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info('ALBUM_PROMISE getAlbum 005_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 005 test end -------------------------
});
