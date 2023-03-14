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
import image from '@ohos.multimedia.image';
import featureAbility from '@ohos.ability.featureAbility';

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index';

describe('FileAssetGetThumbnailPromise.test.js', function () {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    let fileKeyObj = mediaLibrary.FileKey;
    let imagetype = mediaLibrary.MediaType.IMAGE;
    let videoType = mediaLibrary.MediaType.VIDEO;
    let audioType = mediaLibrary.MediaType.AUDIO;
    let imageFetchOp = {
        selections: fileKeyObj.MEDIA_TYPE + '=?',
        selectionArgs: [imagetype.toString()],
        order: fileKeyObj.DATE_ADDED,
    };
    let videoFetchOp = {
        selections: fileKeyObj.MEDIA_TYPE + '=?',
        selectionArgs: [videoType.toString()],
        order: fileKeyObj.DATE_ADDED,
    };
    let audioFetchOp = {
        selections: fileKeyObj.MEDIA_TYPE + '=?',
        selectionArgs: [audioType.toString()],
        order: fileKeyObj.DATE_ADDED,
    };
    beforeAll(function () {});
    beforeEach(function () {});
    afterEach(function () {});
    afterAll(function () {});

    // ------------------------------ image type start -----------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_01
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(image) by { width: 128, height: 128 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_01', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imageFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 128, height: 128 };
            asset.getThumbnail(size,async (err, pixelmap) => {
                if(pixelmap == undefined){
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 001_01 failed');
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 001_01 pixel image info ' + info);
                    console.info('MediaLibraryTest : 001_01 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 001_01 pixel height ' + info.size.height);
                    expect(info.size.width == size.width).assertTrue();
                    expect(info.size.height == size.height).assertTrue();
                    if (info.size.width == size.width && info.size.height == size.height) {
                        expect(true).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_01 passed');
                        done();
                    } else {
                        expect(false).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_01 failed');
                        done();
                    }
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 001_01 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_02
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(image) by { width: 128, height: 256 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_02', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imageFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 128, height: 256 };
            asset.getThumbnail(size,async (err, pixelmap) => {
                if(pixelmap == undefined) {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 001_02 failed');
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 001_02 pixel image info ' + info);
                    console.info('MediaLibraryTest : 001_02 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 001_02 pixel height ' + info.size.height);
                    if (info.size.width == size.width && info.size.height == size.height) {
                        expect(true).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_02 passed');
                        done();
                    } else {
                        expect(false).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_02 failed');
                        done();
                    }
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 001_02 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_03
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(image) by no arg
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_03', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imageFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 256, height: 256 };
            asset.getThumbnail(async (err,pixelmap) => {
                if(pixelmap == undefined) {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 001_03 failed');
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 001_03 pixel image info ' + info);
                    console.info('MediaLibraryTest : 001_03 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 001_03 pixel height ' + info.size.height);
                    if (info.size.width == size.width && info.size.height == size.height) {
                        expect(true).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_03 passed');
                        done();
                    } else {
                        expect(false).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_03 failed');
                        done();
                    }
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 001_03 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_04
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(image) by { width: 1, height: 1 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_04', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imageFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 1, height: 1 };
            asset.getThumbnail(size,async (err, pixelmap) => {
                if(pixelmap == undefined) {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 001_04 failed');
                    done();
                } else{
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 001_04 pixel image info ' + info);
                    console.info('MediaLibraryTest : 001_04 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 001_04 pixel height ' + info.size.height);
                    if (info.size.width == size.width && info.size.height == size.height) {
                        expect(true).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_04 passed');
                        done();
                    } else {
                        expect(false).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_04 failed');
                        done();
                    }
                }
            });
           
        } catch (error) {
            console.info('MediaLibraryTest : getFileAssets 001_04 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_05
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(image) by { width: 0, height: 0 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_05', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imageFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 0, height: 0 };
            asset.getThumbnail(size,async (err, pixelmap) => {
                if(pixelmap == undefined) {
                    expect(true).assertTrue();
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 001_05 pixel image info ' + info);
                    console.info('MediaLibraryTest : 001_05 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 001_05 pixel height ' + info.size.height);
                    expect(info.size.width == size.width || info.size.height == size.height).assertFalse();
                    console.info('MediaLibraryTest : getFileAssets 001_05 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getFileAssets 001_05 failed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_06
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(image) by { width: -128, height: -128 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_06', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imageFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: -128, height: -128 };
            asset.getThumbnail(size,async (err, pixelmap) => {
                if(pixelmap == undefined) {
                    expect(true).assertTrue();
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 001_06 pixel image info ' + info);
                    console.info('MediaLibraryTest : 001_06 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 001_06 pixel height ' + info.size.height);
                    expect(info.size.width == size.width || info.size.height == size.height).assertFalse();
                    console.info('MediaLibraryTest : getThumbnail 001_06 failed');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : getFileAssets 001_06 failed ');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_07
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: 1024, height: 1024 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_001_07', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(imageFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 1024, height: 1024 };
            asset.getThumbnail(size,async (err, pixelmap) => {
                if(pixelmap == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 003_06 pixel image info ' + info);
                    console.info('MediaLibraryTest : 003_06 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 003_06 pixel height ' + info.size.height);
                    if (info.size.width == size.width && info.size.height == size.height) {
                        expect(true).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_07 passed');
                        done();
                    } else {
                        expect(false).assertTrue();
                        console.info('MediaLibraryTest : getThumbnail 001_07 failed');
                        done();
                    }
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 001_07 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------image type end--------------------------

    // ------------------------------video type start -----------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_01
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(video) by { width: 128, height: 128 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_01', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videoFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 128, height: 128 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 002_01 pixel image info ' + info);
                console.info('MediaLibraryTest : 002_01 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 002_01 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_01 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_01 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 002_01 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_02
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(video) by { width: 128, height: 256 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_02', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videoFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 128, height: 256 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 002_02 pixel image info ' + info);
                console.info('MediaLibraryTest : 002_02 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 002_02 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_02 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_02 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 002_02 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_03
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(video) by no arg
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_03', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videoFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 256, height: 256 };
            asset.getThumbnail(async (err, pixelmap) => {
                console.info('MediaLibraryTest : getThumbnail 002_03 Successfull ' + pixelmap);
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 002_03 pixel image info ' + info);
                console.info('MediaLibraryTest : 002_03 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 002_03 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_03 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_03 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 002_03 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_04
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(video) by { width: 1, height: 1 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_04', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videoFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 1, height: 1 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 002_04 pixel image info ' + info);
                console.info('MediaLibraryTest : 002_04 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 002_04 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 001_05 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 001_05 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 002_04 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_05
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(video) by { width: 0, height: 0 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_05', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videoFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 0, height: 0 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                if(pixelmap == undefined) {
                    console.info('MediaLibraryTest : getThumbnail 002_05 passed');
                    expect(true).assertTrue();
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 002_05 pixel image info ' + info);
                    console.info('MediaLibraryTest : 002_05 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 002_05 pixel height ' + info.size.height);
                    console.info('MediaLibraryTest : getThumbnail 002_05 failed');
                    expect(false).assertTrue();
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 002_05 failed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_06
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(video) by { width: -128, height: -128 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_06', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videoFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: -128, height: -128 };
            const pixelmap = await asset.getThumbnail(size, async (err, pixelmap) => {
                if(pixelmap ==undefined) {
                    console.info('MediaLibraryTest : getThumbnail 002_06 passed');
                    expect(true).assertTrue();
                    done();
                }else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 002_06 pixel image info ' + info);
                    console.info('MediaLibraryTest : 002_06 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 002_06 pixel height ' + info.size.height);
                    console.info('MediaLibraryTest : getThumbnail 003_01 failed');
                    expect(false).assertTrue();
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 002_06 failed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_07
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: 1024, height: 1024 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_002_07', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(videoFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 1024, height: 1024 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 003_06 pixel image info ' + info);
                console.info('MediaLibraryTest : 003_06 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 003_06 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_07 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 002_07 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 002_07 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------video type end--------------------------

    // ------------------------------audio type start -----------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_01
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: 128, height: 128 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_01', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audioFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 128, height: 128 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 003_01 pixel image info ' + info);
                console.info('MediaLibraryTest : 003_01 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 003_01 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_01 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_01 failed');
                    done();
                }
            });
           
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 003_01 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_02
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: 128, height: 256 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_02', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audioFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 128, height: 256 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 003_02 pixel image info ' + info);
                console.info('MediaLibraryTest : 003_02 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 003_02 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_02 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_02 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 003_02 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_03
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by no arg
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_03', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audioFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 256, height: 256 };
            asset.getThumbnail(async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 003_03 pixel image info ' + info);
                console.info('MediaLibraryTest : 003_03 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 003_03 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_03 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_03 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 003_03 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_04
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: 1, height: 1 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_04', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audioFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 1, height: 1 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 003_04 pixel image info ' + info);
                console.info('MediaLibraryTest : 003_04 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 003_04 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_04 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_04 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 003_04 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_05
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: 0, height: 0 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_05', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audioFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 0, height: 0 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                if(pixelmap ==undefined) {
                    console.info('MediaLibraryTest : getThumbnail 003_05 passed');
                    expect(true).assertTrue();
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : getThumbnail 003_06 failed');
                    console.info('MediaLibraryTest : 003_05 pixel image info ' + info);
                    console.info('MediaLibraryTest : 003_05 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 003_05 pixel height ' + info.size.height);
                    expect(false).assertTrue();
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 003_05 failed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_06
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: -128, height: -128 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_06', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audioFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: -128, height: -128 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                if(pixelmap ==undefined) {
                    console.info('MediaLibraryTest : getThumbnail 003_06 passed');
                    expect(true).assertTrue();
                    done();
                } else {
                    const info = await pixelmap.getImageInfo();
                    console.info('MediaLibraryTest : 003_06 pixel image info ' + info);
                    console.info('MediaLibraryTest : 003_06 pixel width ' + info.size.width);
                    console.info('MediaLibraryTest : 003_06 pixel height ' + info.size.height);
                    console.info('MediaLibraryTest : getThumbnail 003_06 failed');
        
                    expect(false).assertTrue();
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 003_06 failed');
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_07
     * @tc.name      : getThumbnail
     * @tc.desc      : getThumbnail(audio) by { width: 1024, height: 1024 }
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_GETTHUMBNAIL_CALLBACK_003_07', 0, async function (done) {
        try {
            const fetchFileResult = await media.getFileAssets(audioFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            const asset = dataList[0];
            let size = { width: 1024, height: 1024 };
            asset.getThumbnail(size, async (err, pixelmap) => {
                const info = await pixelmap.getImageInfo();
                console.info('MediaLibraryTest : 003_06 pixel image info ' + info);
                console.info('MediaLibraryTest : 003_06 pixel width ' + info.size.width);
                console.info('MediaLibraryTest : 003_06 pixel height ' + info.size.height);
                if (info.size.width == size.width && info.size.height == size.height) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_07 passed');
                    done();
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : getThumbnail 003_07 failed');
                    done();
                }
            });
            
        } catch (error) {
            console.info('MediaLibraryTest : getThumbnail 003_07 failed ' + error.message);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------audio type end--------------------------
});
