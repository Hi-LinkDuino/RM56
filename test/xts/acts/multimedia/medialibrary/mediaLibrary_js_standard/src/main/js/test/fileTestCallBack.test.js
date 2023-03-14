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
let fileType = mediaLibrary.MediaType.FILE;
let imageType = mediaLibrary.MediaType.IMAGE;
let videoType = mediaLibrary.MediaType.VIDEO;
let audioType = mediaLibrary.MediaType.AUDIO;

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

function checkAssetAttr(done, attr, testNum, asset, checkType) {
    if (checkType && asset[attr] != checkType) {
        console.info(`ASSET_CALLBACK getFileAssets ${testNum} failed`);
        expect(false).assertTrue();
        done();
    } else if (asset[attr] == undefined) {
        console.info(`ASSET_CALLBACK getFileAssets ${testNum} failed`);
        expect(false).assertTrue();
        done();
    }
}

let path;
let presetAsset;
let displayName;
let id;
let mediaType;
let orientation = 0;
describe('fileTestCallBack.test.js', function () {
    var context = featureAbility.getContext();
    console.info('getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('getMediaLibrary OUT');
    beforeAll(function () {});
    beforeEach(function () {});
    afterEach(function () {});
    afterAll(function () {});

    async function copyFile(fd1, fd2) {
        let stat = await fileio.fstat(fd1);
        let buf = new ArrayBuffer(stat.size);
        await fileio.read(fd1, buf);
        await fileio.write(fd2, buf);
    }

    // ------------------------------- image type start ----------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_01
     * @tc.name      : createAsset
     * @tc.desc      : Insert two database records, read a unique identifier, expectations are not equal
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_01', 0, async function (done) {
        try {
            path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
            const fileAssets = await media.getFileAssets(imagesfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset1 = dataList[0];
            presetAsset = asset1;
            media.createAsset(
                imageType,
                `${new Date().getTime()}.jpg`,
                path,
                async (err, creatAsset1) => {
                    if(creatAsset1 == undefined) {
                        expect(false).assertTrue();
                        done();
                    } else {
                        const fd1 = await asset1.open('r');
                        const creatAssetFd1 = await creatAsset1.open('rw');
                        await copyFile(fd1, creatAssetFd1);
                        await creatAsset1.close(creatAssetFd1);
                        await asset1.close(fd1);
                        displayName = `${new Date().getTime()}.jpg`;
                        const asset2 = dataList[1];
                        const creatAsset2 = await media.createAsset(imageType, displayName, path);
                        const fd2 = await asset2.open('r');
                        const creatAssetFd2 = await creatAsset2.open('rw');
                        await copyFile(fd2, creatAssetFd2);
                        await creatAsset2.close(creatAssetFd2);
                        await asset2.close(fd2);
                        id = creatAsset2.id;
                        mediaType = imageType;
                        expect(creatAsset1.id != creatAsset2.id).assertTrue();
                        done();
                    }
                }
            );
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 001_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_02
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file displayName and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_02', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();

            expect(asset.displayName == displayName).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 001_02 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_03
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file relativePath  and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_03', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.relativePath == path).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 001_03 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_07
     * @tc.name      : commitModify
     * @tc.desc      : Access to the file dateModified and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_07', 0, async function (done) {
        try {
            const fileAssets = await media.getFileAssets(imagesfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset = dataList[0];
            asset.title = `title_${new Date().getTime()}`;
            asset.commitModify(async () => {
                const id = asset.id;
                const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
                const newAssets = await media.getFileAssets(idOP);
                const newdataList = await newAssets.getAllObject();
                const newAsset = newdataList[0];
    
                if (asset.dateModified != undefined) {
                    if (newAsset.dateModified != asset.dateModified) {
                        console.info('ASSET_CALLBACK getFileAssets 001_07 passed');
                        expect(true).assertTrue();
                        done();
                    } else {
                        console.info('ASSET_CALLBACK getFileAssets 001_07 failed');
                        expect(false).assertTrue();
                        done();
                    }
                } else if (newAsset.dateModified != undefined) {
                    console.info('ASSET_CALLBACK getFileAssets 001_07 passed');
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('ASSET_CALLBACK getFileAssets 001_07 failed');
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 001_07 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_08
     * @tc.name      : createAsset
     * @tc.desc      : Insert a picture record, the retrieve attributes for images
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_08', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.mediaType == mediaType).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 001_08 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_11
     * @tc.name      : createAsset
     * @tc.desc      : Get the orientaion attribute
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_001_11', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.orientation == orientation).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 001_11 failed, message = ' + error);
        }
    });

    // -------------------------------  image type end -----------------------------

    // ------------------------------- video type start ----------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_01
     * @tc.name      : createAsset
     * @tc.desc      : Insert two database records, read a unique identifier, expectations are not equal
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_01', 0, async function (done) {
        try {
            path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);
            const fileAssets = await media.getFileAssets(videosfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset1 = dataList[0];
            media.createAsset(
                videoType,
                `${new Date().getTime()}.mp4`,
                path,
                async (err, creatAsset1) => {
                    if(creatAsset1 == undefined) {
                        expect(false).assertTrue();
                        done();
                    } else {
                        const fd1 = await asset1.open('r');
                        const creatAssetFd1 = await creatAsset1.open('rw');
                        await copyFile(fd1, creatAssetFd1);
                        await creatAsset1.close(creatAssetFd1);
                        await asset1.close(fd1);
                        displayName = `${new Date().getTime()}.mp4`;
                        const asset2 = dataList[0];
                        const creatAsset2 = await media.createAsset(videoType, displayName, path);
                        const fd2 = await asset2.open('r');
                        const creatAssetFd2 = await creatAsset2.open('rw');
                        await copyFile(fd2, creatAssetFd2);
                        await creatAsset2.close(creatAssetFd2);
                        await asset2.close(fd2);
                        id = creatAsset2.id;
                        mediaType = videoType;

                        expect(creatAsset1.id != creatAsset2.id).assertTrue();
                        done();
                    }
                }
            );
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 002_01 failed' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_02
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file displayName and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_02', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();

            expect(asset.displayName == displayName).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 002_02 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_03
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file relativePath  and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_03', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();

            expect(asset.relativePath == path).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 002_03 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_07
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file dateModified and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_07', 0, async function (done) {
        try {
            const fileAssets = await media.getFileAssets(videosfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset = dataList[0];
            asset.title = `title_${new Date().getTime()}`;
            asset.commitModify(async () => {
                const id = asset.id;
                const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
                const newAssets = await media.getFileAssets(idOP);
                const newdataList = await newAssets.getAllObject();
                const newAsset = newdataList[0];
    
                if (asset.dateModified != undefined) {
                    if (newAsset.dateModified != asset.dateModified) {
                        console.info('ASSET_CALLBACK getFileAssets 002_07 passed');
                        expect(true).assertTrue();
                        done();
                    } else {
                        console.info('ASSET_CALLBACK getFileAssets 002_07 failed');
                        expect(false).assertTrue();
                        done();
                    }
                } else if (newAsset.dateModified != undefined) {
                    console.info('ASSET_CALLBACK getFileAssets 002_07 passed');
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('ASSET_CALLBACK getFileAssets 002_07 failed');
                    expect(false).assertTrue();
                    done();
                }
            });
         } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 002_07 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_08
     * @tc.name      : createAsset
     * @tc.desc      : Insert a picture record, the retrieve attributes for images
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_08', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.mediaType == mediaType).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 002_08 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_11
     * @tc.name      : createAsset
     * @tc.desc      : Get the orientaion attribute
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_002_11', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.orientation == orientation).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 002_11 failed, message = ' + error);
        }
    });
    // -------------------------------  video type end -----------------------------

    // ------------------------------- audio type start ----------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_01
     * @tc.name      : createAsset
     * @tc.desc      : Insert two database records, read a unique identifier, expectations are not equal
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_01', 0, async function (done) {
        try {
            path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
            const fileAssets = await media.getFileAssets(audiosfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset1 = dataList[0];
            media.createAsset(
                audioType,
                `${new Date().getTime()}.mp3`,
                path,
                async (err, creatAsset1) => {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);
                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);
                    displayName = `${new Date().getTime()}.mp3`;
                    const asset2 = dataList[0];
                    const creatAsset2 = await media.createAsset(audioType, displayName, path);
                    const fd2 = await asset2.open('r');
                    const creatAssetFd2 = await creatAsset2.open('rw');
                    await copyFile(fd2, creatAssetFd2);
                    await creatAsset2.close(creatAssetFd2);
                    await asset2.close(fd2);
                    id = creatAsset2.id;
                    mediaType = audioType;
                    expect(creatAsset1.id != creatAsset2.id).assertTrue();
                    done();
                }
            );
            
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 003_01 failed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_02
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file name and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_02', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.displayName == displayName).assertTrue();

            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 003_02 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_03
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file relativePath  and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_03', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.relativePath == path).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 003_03 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_07
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file dateModified and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_07', 0, async function (done) {
        try {
            const fileAssets = await media.getFileAssets(audiosfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset = dataList[0];
            asset.title = `title_${new Date().getTime()}`;
            asset.commitModify(async () => {
                const id = asset.id;
                const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
                const newAssets = await media.getFileAssets(idOP);
                const newdataList = await newAssets.getAllObject();
                const newAsset = newdataList[0];
    
                if (asset.dateModified != undefined) {
                    if (newAsset.dateModified != asset.dateModified) {
                        console.info('ASSET_CALLBACK getFileAssets 003_07 passed');
                        expect(true).assertTrue();
                        done();
                    } else {
                        console.info('ASSET_CALLBACK getFileAssets 003_07 failed');
                        expect(false).assertTrue();
                        done();
                    }
                } else if (newAsset.dateModified != undefined) {
                    console.info('ASSET_CALLBACK getFileAssets 003_07 passed');
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('ASSET_CALLBACK getFileAssets 003_07 failed');
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 003_07 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_08
     * @tc.name      : createAsset
     * @tc.desc      : Insert a picture record, the retrieve attributes for images
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_003_08', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.mediaType == mediaType).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 003_08 failed, message = ' + error);
        }
    });

    // -------------------------------  audio type end -----------------------------

    // ------------------------------ file type start ----------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_01
     * @tc.name      : createAsset
     * @tc.desc      : Insert two database records, read a unique identifier, expectations are not equal
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_01', 0, async function (done) {
        try {
            path = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_DOWNLOAD);
            const fileAssets = await media.getFileAssets(filesfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset1 = dataList[0];
            media.createAsset(
                fileType,
                `${new Date().getTime()}.bat`,
                path,
                async (err, creatAsset1) => {
                    const fd1 = await asset1.open('r');
                    const creatAssetFd1 = await creatAsset1.open('rw');
                    await copyFile(fd1, creatAssetFd1);
                    await creatAsset1.close(creatAssetFd1);
                    await asset1.close(fd1);
                    displayName = `${new Date().getTime()}.bat`;
                    const asset2 = dataList[0];
                    const creatAsset2 = await media.createAsset(fileType, displayName, path);
                    const fd2 = await asset2.open('r');
                    const creatAssetFd2 = await creatAsset2.open('rw');
                    await copyFile(fd2, creatAssetFd2);
                    await creatAsset2.close(creatAssetFd2);
                    await asset2.close(fd2);
                    id = creatAsset2.id;
                    mediaType = fileType;
                    expect(creatAsset1.id != creatAsset2.id).assertTrue();
                    done();
                }
            );
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 004_01 failed' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_02
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file name and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_02', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.displayName == displayName).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 004_02 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_03
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file relativePath  and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_03', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.relativePath == path).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 004_03 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_07
     * @tc.name      : getFileAssets
     * @tc.desc      : Access to the file dateModified and validation is not undefined
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_07', 0, async function (done) {
        try {
            const fileAssets = await media.getFileAssets(filesfetchOp);
            const dataList = await fileAssets.getAllObject();
            const asset = dataList[0];
            asset.title = `title_${new Date().getTime()}`;
            asset.commitModify(async ()=> {
                const id = asset.id;
                const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
                const newAssets = await media.getFileAssets(idOP);
                const newdataList = await newAssets.getAllObject();
                const newAsset = newdataList[0];
    
                if (asset.dateModified != undefined) {
                    if (newAsset.dateModified != asset.dateModified) {
                        console.info('ASSET_CALLBACK getFileAssets 004_07 passed');
                        expect(true).assertTrue();
                        done();
                    } else {
                        console.info('ASSET_CALLBACK getFileAssets 004_07 failed');
                        expect(false).assertTrue();
                        done();
                    }
                } else if (newAsset.dateModified != undefined) {
                    console.info('ASSET_CALLBACK getFileAssets 004_07 passed');
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('ASSET_CALLBACK getFileAssets 004_07 failed');
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('ASSET_CALLBACK getFileAssets 004_07 failed, message = ' + error);
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_08
     * @tc.name      : createAsset
     * @tc.desc      : Insert a picture record, the retrieve attributes for images
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_CREATEASSET_CALLBACK_004_08', 0, async function (done) {
        try {
            const idOP = { selections: fileKeyObj.ID + '= ?', selectionArgs: ['' + id] };
            const fileAssets = await media.getFileAssets(idOP);
            const asset = await fileAssets.getFirstObject();
            expect(asset.mediaType == mediaType).assertTrue();
            done();
        } catch (error) {
            console.info('ASSET_CALLBACK createAsset 004_08 failed, message = ' + error);
        }
    });

    // -------------------------------  file type end -----------------------------
});
