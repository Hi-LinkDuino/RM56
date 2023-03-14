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
function printAttr(asset) {
    for (const key in asset) {
        console.info(`${key}: asset[key]`);
    }
}
function checkAttrs(done, asset, tNum) {
    let passed = true;
    for (const key in asset) {
        if (asset[key] == undefined) {
            passed = false;
            break;
        }
    }
    if (passed) {
        console.info(`FileAsset checkAttrs ${tNum} passed`);
        expect(true).assertTrue();
        done();
    } else {
        console.info(`FileAsset checkAttrs ${tNum} failed`);
        expect(false).assertTrue();
        done();
    }
}
describe('fileAssetCallBack2.test.js', async function () {
    let fileKeyObj = mediaLibrary.FileKey;

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

    let allTypefetchOp = {
        selections: '',
        selectionArgs: [],
    };
    const context = featureAbility.getContext();
    const media = mediaLibrary.getMediaLibrary(context);
    beforeAll(function () {});
    beforeEach(function () {});
    afterEach(function () {});
    afterAll(function () {});

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_commitModify_callback_001
     * @tc.name      : commitModify
     * @tc.desc      : Modify displayName
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_commitModify_callback_001', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            const newName = 'newName';
            asset.displayName = newName;
            const id = asset.id;
            asset.commitModify(async ()=>{
                const fetchFileResult2 = await media.getFileAssets(imagesfetchOp);
                const dataList = await fetchFileResult2.getAllObject();
                let passed = false;
                for (let i = 0; i < dataList.length; i++) {
                    const asset = dataList[i];
                    if (asset.id == id && asset.displayName == newName) {
                        passed = true;
                        break;
                    }
                }
                expect(passed).assertTrue();
                done();
            });

            
        } catch (error) {
            console.info('FileAsset commitModify 001 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_commitModify_callback_002
     * @tc.name      : commitModify
     * @tc.desc      : Modify title
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_commitModify_callback_002', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            const newTitle = 'newTitle';
            asset.title = newTitle;
            const id = asset.id;
            asset.commitModify(async () => {
                const fetchFileResult2 = await media.getFileAssets(imagesfetchOp);
                const dataList = await fetchFileResult2.getAllObject();
                let passed = false;
                for (let i = 0; i < dataList.length; i++) {
                    const asset = dataList[i];
                    if (asset.id == id && asset.title == newTitle) {
                        passed = true;
                        break;
                    }
                }
                expect(passed).assertTrue();
                done();
            });
        } catch (error) {
            console.info('FileAsset commitModify 002 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_commitModify_callback_003
     * @tc.name      : commitModify
     * @tc.desc      : Modify relativePath
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_commitModify_callback_003', 0, async function (done) {
        try {
            const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);

            const path2 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);

            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            let relativePath = asset.relativePath;
            let newrelativePath = path1;
            if (relativePath == path1) {
                newrelativePath = path2;
            }

            asset.relativePath = newrelativePath;
            const id = asset.id;
            asset.commitModify(async () => {
                const fetchFileResult2 = await media.getFileAssets(imagesfetchOp);
                const dataList = await fetchFileResult2.getAllObject();
                let passed = false;
                for (let i = 0; i < dataList.length; i++) {
                    const asset = dataList[i];
                    if (asset.id == id && asset.relativePath == newrelativePath) {
                        passed = true;
                        break;
                    }
                }
                expect(passed).assertTrue();
                done();
            });
        } catch (error) {
            console.info('FileAsset commitModify 003 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_commitModify_callback_004
     * @tc.name      : commitModify
     * @tc.desc      : Modify orientation
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_commitModify_callback_004', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            let neworientation = 1;
            if (asset.orientation == 1) {
                neworientation = 0;
            }
            asset.orientation = neworientation;
            const id = asset.id;
            asset.commitModify(async () => {
                const fetchFileResult2 = await media.getFileAssets(imagesfetchOp);
                const dataList = await fetchFileResult2.getAllObject();
                let passed = false;
                for (let i = 0; i < dataList.length; i++) {
                    const asset = dataList[i];
                    if (asset.id == id && asset.orientation == neworientation) {
                        passed = true;
                        break;
                    }
                }
                expect(passed).assertTrue();
                done();
            });
        } catch (error) {
            console.info('FileAsset commitModify 004 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_commitModify_callback_005
     * @tc.name      : commitModify
     * @tc.desc      : Modify uri
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_commitModify_callback_005', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            const id = asset.id;
            const newUri = 'newUri';

            asset.uri = newUri;

            asset.commitModify(async (err) => {
                if(err) {
                    expect(true).assertTrue();
                    done(); 
                } else {
                    console.info('FileAsset commitModify 005 failed');
                    expect(false).assertTrue();
                    done();
                }
            });

            
        } catch (error) {
            console.info('FileAsset commitModify 005 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_commitModify_callback_006
     * @tc.name      : commitModify
     * @tc.desc      : Modify mediaType
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_commitModify_callback_006', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            const id = asset.id;
            const newMediaType = 'newMediaType';

            asset.mediaType = newMediaType;

            asset.commitModify(async (err) => {
                if(err) {
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('FileAsset commitModify 006 failed');
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('FileAsset commitModify 006 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_isDirectory_callback_001
     * @tc.name      : isDirectory
     * @tc.desc      : isDirectory asset
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_isDirectory_callback_001', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.isDirectory((err, isDir) => {
                if(isDir == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    expect(!isDir).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('FileAsset isDirectory 001 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_checkAttr_callback_001
     * @tc.name      : attrs
     * @tc.desc      : imagesfetchOp attrs print and check
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_checkAttr_callback_001', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            fetchFileResult.getFirstObject((err, asset) => {
                if(asset == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    printAttr(asset);
                    checkAttrs(done, asset, '001');
                }
            });
        } catch (error) {
            console.info('FileAsset checkAttr 001 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_checkAttr_callback_002
     * @tc.name      : attrs
     * @tc.desc      : videosfetchOp attrs print and check
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_checkAttr_callback_002', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videosfetchOp);
            fetchFileResult.getFirstObject((err, asset) => {
                if(asset == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    printAttr(asset);
                    checkAttrs(done, asset, '002');
                }
            });
        } catch (error) {
            console.info('FileAsset checkAttr 002 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_checkAttr_callback_003
     * @tc.name      : attrs
     * @tc.desc      : audiosfetchOp attrs print and check
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_checkAttr_callback_003', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audiosfetchOp);
            fetchFileResult.getFirstObject((err, asset)=>{
                if(asset == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    printAttr(asset);
                    checkAttrs(done, asset, '003');
                }
            });
        } catch (error) {
            console.info('FileAsset checkAttr 003 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_FILEASSET_checkAttr_callback_004
     * @tc.name      : attrs
     * @tc.desc      : album attrs print and check
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_FILEASSET_checkAttr_callback_004', 0, async function (done) {
        try {
            const albumList = await media.getAlbums(allTypefetchOp);
            const album = albumList[0];
            const fetchFileResult = await album.getFileAssets(allTypefetchOp);
             fetchFileResult.getFirstObject((err, asset) => {
                if(asset == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    printAttr(asset);
                    checkAttrs(done, asset, '004');
                }
            });
            
        } catch (error) {
            console.info('FileAsset checkAttr 003 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
});
