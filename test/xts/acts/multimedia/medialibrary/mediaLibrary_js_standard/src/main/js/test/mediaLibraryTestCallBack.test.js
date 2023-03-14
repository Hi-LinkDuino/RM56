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
import fileio from '@ohos.fileio';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index';
let fileKeyObj = mediaLibrary.FileKey;
let fetchOp = {
    selections: fileKeyObj.PATH + ' LIKE ? ',
    selectionArgs: ['/data/media/%'],
    order: fileKeyObj.PATH,
};
// let directoryTypeObj = mediaLibrary.DirectoryType;

let DIR_CAMERA = mediaLibrary.DirectoryType.DIR_CAMERA;
let DIR_VIDEO = mediaLibrary.DirectoryType.DIR_VIDEO;
let DIR_IMAGE = mediaLibrary.DirectoryType.DIR_IMAGE;
let DIR_AUDIO = mediaLibrary.DirectoryType.DIR_AUDIO;
let DIR_DOCUMENTS = mediaLibrary.DirectoryType.DIR_DOCUMENTS;

let imageType = mediaLibrary.MediaType.IMAGE;
let videoType = mediaLibrary.MediaType.VIDEO;
let audioType = mediaLibrary.MediaType.AUDIO;
let fileType = mediaLibrary.MediaType.FILE;

let imagesfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [imageType.toString()],
};
let videosfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [videoType.toString()],
};
let audiosfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [audioType.toString()],
};
let filesfetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [fileType.toString()],
};

let imageAndVideofetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [imageType.toString(), videoType.toString()],
};
let imageAndVideoAndfilefetchOp = {
    selections:
        fileKeyObj.MEDIA_TYPE +
        '= ? or ' +
        fileKeyObj.MEDIA_TYPE +
        '= ? or ' +
        fileKeyObj.MEDIA_TYPE +
        '= ?',
    selectionArgs: [imageType.toString(), videoType.toString(), fileType.toString()],
};
let imageAndVideoAndfileAndAudiofetchOp = {
    selections:
        fileKeyObj.MEDIA_TYPE +
        '= ? or ' +
        fileKeyObj.MEDIA_TYPE +
        '= ? or ' +
        fileKeyObj.MEDIA_TYPE +
        '= ? or ' +
        fileKeyObj.MEDIA_TYPE +
        '= ?',
    selectionArgs: [
        imageType.toString(),
        videoType.toString(),
        fileType.toString(),
        audioType.toString(),
    ],
};

let allTypefetchOp = {
    selections: '',
    selectionArgs: [],
};

async function copyFile(fd1, fd2) {
    let stat = await fileio.fstat(fd1);
    let buf = new ArrayBuffer(stat.size);
    await fileio.read(fd1, buf);
    await fileio.write(fd2, buf);
}

describe('mediaLibraryTestCallBack.test.js', function () {
    const context = featureAbility.getContext();
    const media = mediaLibrary.getMediaLibrary(context);

    beforeAll(function () { });
    beforeEach(function () { });
    afterEach(function () { });
    afterAll(function () { });

    var timestamp = new Date().getTime();
    var fileName = new Date().getTime() + '.bat';



    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_001
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_001', 0, async function (done) {
        try {
            media.getFileAssets(imagesfetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult != undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 001 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_002
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_002', 0, async function (done) {
        try {
            media.getFileAssets(videosfetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult != undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 002 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_003
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_003', 0, async function (done) {
        try {
            media.getFileAssets(audiosfetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult != undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 003 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_004
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_004', 0, async function (done) {
        try {
            media.getFileAssets(filesfetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult != undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 004 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_005
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_005', 0, async function (done) {
        try {
            media.getFileAssets(imageAndVideofetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult != undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 005 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_006
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_006', 0, async function (done) {
        try {
            media.getFileAssets(imageAndVideoAndfilefetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult != undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 006 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_007
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_007', 0, async function (done) {
        try {
            media.getFileAssets(imageAndVideoAndfileAndAudiofetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult != undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 007 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_008
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_008', 0, async function (done) {
        let fetchOp = {
            selections: fileKeyObj.MEDIA_TYPE + 'abc= ?',
            selectionArgs: ['abc'],
        };
        try {
            media.getFileAssets(fetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult == undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 008 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_009
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_009', 0, async function (done) {
        let fetchOp = {
            selections: fileKeyObj.MEDIA_TYPE + 'abc= ? or ' + fileKeyObj.MEDIA_TYPE + '= ?',
            selectionArgs: ['abc', audioType.toString()],
        };
        try {
            media.getFileAssets(fetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult == undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 009 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_009
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_010', 0, async function (done) {
        let fetchOp = {
            selections:
                fileKeyObj.MEDIA_TYPE +
                'abc= ? or ' +
                fileKeyObj.MEDIA_TYPE +
                '= ? or ' +
                fileKeyObj.MEDIA_TYPE +
                '= ?',
            selectionArgs: ['abc', videoType.toString(), fileType.toString()],
        };
        try {
            media.getFileAssets(fetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult == undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 010 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_011
     * @tc.name      : getFileAssets
     * @tc.desc      : query all assets
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETFILEASSETS_011', 0, async function (done) {
        let fetchOp = {
            selections:
                fileKeyObj.MEDIA_TYPE +
                'abc= ? or ' +
                fileKeyObj.MEDIA_TYPE +
                '= ? or ' +
                fileKeyObj.MEDIA_TYPE +
                '= ? or ' +
                fileKeyObj.MEDIA_TYPE +
                '= ?',
            selectionArgs: ['abc', videoType.toString(), fileType.toString(), audioType.toString()],
        };
        try {
            media.getFileAssets(fetchOp, (err, fetchFileResult) => {
                expect(fetchFileResult == undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getFileAssets 011 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_001
     * @tc.name      : getPublicDirectory
     * @tc.desc      : getPublicDirectory DIR_CAMERA
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_001', 0, async function (done) {
        try {

            let DIR_CAMERA = mediaLibrary.DirectoryType.DIR_CAMERA;

            media.getPublicDirectory(DIR_CAMERA, async (err, dicResult) => {
                expect(dicResult == 'Camera/').assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getPublicDirectory 001 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_002
     * @tc.name      : getPublicDirectory
     * @tc.desc      : getPublicDirectory DIR_VIDEO
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_002', 0, async function (done) {
        try {
            let DIR_VIDEO = mediaLibrary.DirectoryType.DIR_VIDEO;

            media.getPublicDirectory(DIR_VIDEO, async (err, dicResult) => {
                expect(dicResult == 'Videos/').assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getPublicDirectory 002 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_003
     * @tc.name      : getPublicDirectory
     * @tc.desc      : getPublicDirectory DIR_IMAGE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_003', 0, async function (done) {
        try {
            let DIR_IMAGE = mediaLibrary.DirectoryType.DIR_IMAGE;

            media.getPublicDirectory(DIR_IMAGE, async (err, dicResult) => {
                expect(dicResult == 'Pictures/').assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getPublicDirectory 003 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_004
     * @tc.name      : getPublicDirectory
     * @tc.desc      : getPublicDirectory DIR_IMAGE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_004', 0, async function (done) {
        try {
            let DIR_AUDIO = mediaLibrary.DirectoryType.DIR_AUDIO;

            media.getPublicDirectory(DIR_AUDIO, async (err, dicResult) => {
                expect(dicResult == 'Audios/').assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getPublicDirectory 004 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_005
     * @tc.name      : getPublicDirectory
     * @tc.desc      : getPublicDirectory DIR_IMAGE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_005', 0, async function (done) {
        try {
            let DIR_DOCUMENTS = mediaLibrary.DirectoryType.DIR_DOCUMENTS;

            media.getPublicDirectory(DIR_DOCUMENTS, async (err, dicResult) => {
                expect(dicResult == 'Documents/').assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getPublicDirectory 005 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_006
     * @tc.name      : getPublicDirectory
     * @tc.desc      : getPublicDirectory 110
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_GETPUBLICDIRECTORY_006', 0, async function (done) {
        try {
            media.getPublicDirectory(110, async (err, dicResult) => {
                expect(dicResult == undefined).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : getPublicDirectory 006 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_DELETEASSET_001
     * @tc.name      : deleteAsset
     * @tc.desc      : Delete File by Asset uri
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_DELETEASSET_001', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset1 = dataList[0];
            const delUri = asset1.uri;

            media.deleteAsset(asset1.uri, async () => {
                const fetchFileResult1 = await media.getFileAssets(imagesfetchOp);
                const dataList1 = await fetchFileResult1.getAllObject();

                let passed = true;
                for (let i = 0; i < dataList1.length; i++) {
                    const asset = dataList1[i];
                    if (asset.uri == delUri) {
                        passed = false;
                    }
                }
                expect(passed).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : deleteAsset 001 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });



    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_DELETEASSET_002
     * @tc.name      : deleteAsset
     * @tc.desc      : Delete File Asset by aaaa + uri + aaaaa
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_DELETEASSET_002', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset1 = dataList[0];
            const delUri = asset1.uri;

            await media.deleteAsset('aaaa' + asset1.uri + 'aaaa', async () => {
                const fetchFileResult1 = await media.getFileAssets(imagesfetchOp);
                const dataList1 = await fetchFileResult1.getAllObject();

                let passed = false;
                for (let i = 0; i < dataList1.length; i++) {
                    const asset = dataList1[i];
                    if (asset.uri == delUri) {
                        passed = true;
                    }
                }
                expect(passed).assertTrue();
                done();
            });
        } catch (error) {
            console.info(`MediaLibraryTest : deleteAsset 002 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
         * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_009
         * @tc.name      : createAsset
         * @tc.desc      : Create File Asset image (does not exist)
         * @tc.size      : MEDIUM
         * @tc.type      : Function
         * @tc.level     : Level 0
         */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_009', 0, async function (done) {
        try {
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
            const filePath = path + "image/";
            const fileAssets = await media.getFileAssets(imagesfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            const jpgName = new Date().getTime() + '.jpg';
            media.createAsset(imageType, jpgName, filePath, async (err, creatAsset1) => {
                if (creatAsset1 == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);
                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);
                    console.info('MediaLibraryTest : createAsset 009 passed');
                    expect(true).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 009 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_001
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset image (does not exist)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_001', 0, async function (done) {
        try {
            const jpgName = new Date().getTime() + '.jpg';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);

            const fileAssets = await media.getFileAssets(imagesfetchOp);

            const asset1 = await fileAssets.getFirstObject();

            media.createAsset(imageType, jpgName, path, async (err, creatAsset1) => {
                if (creatAsset1 == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);

                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);

                    console.info('MediaLibraryTest : createAsset 001 passed');
                    expect(true).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 001 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_002
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset image (existed)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_002', 0, async function (done) {
        try {
            const jpgName = new Date().getTime() + '.jpg';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
            const fileAssets = await media.getFileAssets(imagesfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            media.createAsset(imageType, jpgName, path, async (err, creatAsset1) => {
                if (creatAsset1 == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);

                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);
                    media.createAsset(imageType, jpgName, path, async (err, creatAsset2) => {
                        expect(creatAsset2 == undefined).assertTrue();
                        done();
                    });
                }
            });

        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 002 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_003
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset video (does not exist)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_003', 0, async function (done) {
        try {
            const videoName = new Date().getTime() + '.mp4';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);
            const fileAssets = await media.getFileAssets(videosfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            media.createAsset(videoType, videoName, path, async (err, creatAsset1) => {
                if (creatAsset1 == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);
                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);
                    console.info('MediaLibraryTest : createAsset 003 passed');
                    expect(true).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 003 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_004
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset video (existed)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_004', 0, async function (done) {
        try {
            const videoName = new Date().getTime() + '.mp4';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);
            const fileAssets = await media.getFileAssets(videosfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            media.createAsset(videoType, videoName, path, async (err, creatAsset1) => {
                if (creatAsset1 == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);
                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);
                    media.createAsset(videoType, videoName, path, async (err, creatAsset2) => {
                        expect(creatAsset2 == undefined).assertTrue();
                        done();
                    });
                }
            });

        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 004 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_005
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset audio (does not exist)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_005', 0, async function (done) {
        try {
            const audioName = new Date().getTime() + '.mp3';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
            const fileAssets = await media.getFileAssets(audiosfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            media.createAsset(audioType, audioName, path, async (err, creatAsset1) => {
                if (creatAsset1 == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);
                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);

                    console.info('MediaLibraryTest : createAsset 005 passed');
                    expect(true).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 005 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_006
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset audio (existed)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_006', 0, async function (done) {
        try {
            const audioName = new Date().getTime() + '.mp3';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
            const fileAssets = await media.getFileAssets(audiosfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            media.createAsset(audioType, audioName, path, async (err, creatAsset1) => {
                if (creatAsset1 == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);
                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);
                    media.createAsset(audioType, audioName, path, async (err, creatAsset2) => {
                        expect(creatAsset2 == undefined).assertTrue();
                        done();
                    });
                }
            });

        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 006 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_007
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset file (does not exist)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_007', 0, async function (done) {
        try {
            const fileName = new Date().getTime() + '.bat';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_DOWNLOAD);
            const fileAssets = await media.getFileAssets(filesfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            media.createAsset(fileType, fileName, path, async (err, creatAsset1) => {
                try {
                    if (creatAsset1 == undefined) {
                        expect(false).assertTrue();
                        done();
                    } else {
                        const fd1 = await asset1.open('r');
                        const creatAssetFd1 = await creatAsset1.open('rw');
                        await copyFile(fd1, creatAssetFd1);
                        await creatAsset1.close(creatAssetFd1);
                        await asset1.close(fd1);

                        console.info('MediaLibraryTest : createAsset 007 passed');
                        expect(true).assertTrue();
                        done();
                    }
                } catch (error) {
                    console.info(`MediaLibraryTest : createAsset 007 failed, error: ${error}`);
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 007 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_008
     * @tc.name      : createAsset
     * @tc.desc      : Create File Asset file (existed)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_CALLBACK_CREATEASSET_008', 0, async function (done) {
        try {
            const fileName = new Date().getTime() + '.bat';
            const path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_DOWNLOAD);
            const fileAssets = await media.getFileAssets(filesfetchOp);
            const asset1 = await fileAssets.getFirstObject();
            media.createAsset(fileType, fileName, path, async (err, creatAsset1) => {
                try {
                    if (creatAsset1 == undefined) {
                        expect(false).assertTrue();
                        done();
                    } else {
                        const fd1 = await asset1.open('r');
                        const creatAssetFd1 = await creatAsset1.open('rw');
                        await copyFile(fd1, creatAssetFd1);
                        await creatAsset1.close(creatAssetFd1);
                        await asset1.close(fd1);
                        media.createAsset(fileType, fileName, path, async (err, creatAsset2) => {
                            expect(creatAsset2 == undefined).assertTrue();
                            done();
                        });
                    }
                } catch (error) {
                    console.info(`MediaLibraryTest : createAsset 008 failed, error: ${error}`);
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info(`MediaLibraryTest : createAsset 008 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });


});
