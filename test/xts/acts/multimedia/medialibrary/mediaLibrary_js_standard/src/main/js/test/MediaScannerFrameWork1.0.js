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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'


describe('GET_MEDIA_AUDIO_VIDEO_IMAGE_ALBUM.test.js', function () {
console.info("MediaLibraryTest : mediaLibrary Instance before");

const context = featureAbility.getContext();
const media = mediaLibrary.getMediaLibrary(context);

//const media = mediaLibrary.getMediaLibrary();
let scannerObj = mediaLibrary.getScannerInstance();
console.info("MediaLibraryTest :mediaLibrary Instance after");
let path = "/storage/media/local/files/"

beforeAll(function () {
//onsole.info('beforeAll： Prerequisites at the test suite level, which are executed before the test suite is executed.');

})

beforeEach(function () {
//console.info('MediaLibraryTest: beforeEach：Prerequisites at the test case level, which are executed before each test case is executed.');

})
afterEach(function () {
//console.info('MediaLibraryTest: afterEach： Test case-level clearance conditions, which are executed after each test case is executed.');

})
afterAll(function () {
//console.info('MediaLibraryTest: afterAll：  Test suite-level cleanup condition, which is executed after the test suite is executed');

})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_DIR_PUBLIC_DIRECTORY_EMPTY_001
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */

it('SUB_SCANNER_CB_SCAN_DIR_PUBLIC_DIRECTORY_EMPTY_001', 0, async function (done) {
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest :MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(path,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN PUBLIC DIRECOTY IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : 001_SUB_MEDIA_MEDIALIBRARY_SCAN_DIR_DATA_MEDIA : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : 001_SUB_MEDIA_MEDIALIBRARY_SCAN_DIR_DATA_MEDIA : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_IMAGE_ALBUM_002
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_IMAGE_ALBUM_002', 0, async function (done) {
    console.info('MediaLibraryTest : create Image album (jpg)');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
    let mediaType = mediaLibrary.MediaType.IMAGE;
    await media.createAsset(mediaType, "image.jpg", path1).then (function (asset) {
        console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
    }).catch(function(err){
        console.info("MediaLibraryTest :createAsset failed with error:"+ err);
    });
    await sleep(1000);
    let scanImageAlbumPath = "/storage/media/100/local/files/"+path1
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanImageAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN IMAGE ALBUM IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_IMAGE_ALBUM_002 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_IMAGE_ALBUM_002 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_ALBUM_003
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_AUDIO_ALBUM_003', 0, async function (done) {
    console.info('MediaLibraryTest : create Audio album (MP3)');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
    let mediaType = mediaLibrary.MediaType.AUDIO;
    await media.createAsset(mediaType, "audio.mp3", path1).then (function (asset) {
        console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
    }).catch(function(err){
        console.info("MediaLibraryTest :createAsset failed with error:"+ err);
    });
    await sleep(1000);
    let scanAudioAlbumPath = "/storage/media/100/local/files/"+path1
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN AUDIO ALBUM IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_ALBUM_003 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_ALBUM_003 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_VIDEO_ALBUM_004
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_VIDEO_ALBUM_004', 0, async function (done) {
    console.info('MediaLibraryTest : create Video album (MP4)');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);
    let mediaType = mediaLibrary.MediaType.VIDEO;
    await media.createAsset(mediaType, "video.mp4", path1).then (function (asset) {
        console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
    }).catch(function(err){
        console.info("MediaLibraryTest :createAsset failed with error:"+ err);
    });
    await sleep(1000);
    let scanVideoAlbumPath = "/storage/media/100/local/files/"+path1
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanVideoAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN VIDEO ALBUM IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_VIDEO_ALBUM_004 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_VIDEO_ALBUM_004 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

  /* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_DOC_ALBUM_005
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_DOC_ALBUM_005', 0, async function (done) {
        console.info('MediaLibraryTest : create doc album (txt)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_DOCUMENTS);
        let mediaType = mediaLibrary.MediaType.FILE;
        await media.createAsset(mediaType, "Doc.txt", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanDocAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanDocAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN DOC ALBUM IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_DOC_ALBUM_005 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_DOC_ALBUM_005 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
    })



	    /* *
        * @tc.number    : SUB_SCANNER_CB_NONEXISTING_ALBUM_009
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_NONEXISTING_ALBUM_009', 0, async function (done) {
    let modifiedAlbumPath = "Pictures/NONEXISTING/";
    await sleep(1000);
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(modifiedAlbumPath,async (err, data) => {
            if (data == undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN OF NON EXISTING ALBUM IS UNSUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_NONEXISTING_ALBUM_009 : PASS');
            } else {
                console.info('MediaLibraryTest : status is '+ data.status);
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_NONEXISTING_ALBUM_009 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
    })

	    /* *
        * @tc.number    : SUB_SCANNER_CB_UNICODE_ALBUM_010
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */

    it('SUB_SCANNER_CB_UNICODE_ALBUM_010', 0, async function (done) {
        console.info('MediaLibraryTest : create Audio album (MP3)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
        let mediaType = mediaLibrary.MediaType.IMAGE;
        await media.createAsset(mediaType, "image_UNICODE.jpg", path1+"来自于华为公司/").then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanImageAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanImageAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN Audio album (MP3) IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_UNICODE_ALBUM_010 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_UNICODE_ALBUM_010 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();

    })

	    /* *
        * @tc.number    : SUB_SCANNER_CB_CANONICAL_ALBUM_011
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_CANONICAL_ALBUM_011', 0, async function (done) {
    console.info('MediaLibraryTest : create Image album (jpg)');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
    let mediaType = mediaLibrary.MediaType.IMAGE;
    await media.createAsset(mediaType, "image_UNICODE.jpg", path1+"1/2/3/4/").then (function (asset) {
        console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
    }).catch(function(err){
        console.info("MediaLibraryTest :createAsset failed with error:"+ err);
    });
    await sleep(1000);
    let scanImageAlbumPath = "/storage/media/100/local/files/"+path1+"../../../../"
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanImageAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN Image album (jpg) SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_CANONICAL_ALBUM_011 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_CANONICAL_ALBUM_011 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
    })

/* *
        * @tc.number    : SUB_SCANNER_CB_NON_PUBLIC_DIRECTORY_012
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_NON_PUBLIC_DIRECTORY_012', 0, async function (done) {
    let nonPublicDirectoryPath = "/data/media"
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest :MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(nonPublicDirectoryPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                if (data.status != 0){
                    console.info('MediaLibraryTest : SCAN NON PUBLIC DIRECOTY IS UNSUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_NON_PUBLIC_DIRECTORY_012 : PASS');
                }else{
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_NON_PUBLIC_DIRECTORY_012 : FAIL');
                }
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_NON_PUBLIC_DIRECTORY_012 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_IMAGE_JPG_FILE_013
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */

it('SUB_SCANNER_CB_SCAN_IMAGE_JPG_FILE_013', 0, async function (done) {
    console.info('MediaLibraryTest : create Image album (jpg)');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
    let mediaType = mediaLibrary.MediaType.IMAGE;
    await media.createAsset(mediaType, "Scan_image_01.jpg", path1).then (function (asset) {
        console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
    }).catch(function(err){
        console.info("MediaLibraryTest :createAsset failed with error:"+ err);
    });
    await sleep(1000);
    let scanImageAlbumPath = "/storage/media/100/local/files/"+path1
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanImageAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN Image album (jpg) IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_IMAGE_JPG_FILE_013 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_IMAGE_JPG_FILE_013 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})


  /* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_IMAGE_PNG_FILE_014
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */

    it('SUB_SCANNER_CB_SCAN_IMAGE_PNG_FILE_014', 0, async function (done) {
        console.info('MediaLibraryTest : create Image album (png)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
        let mediaType = mediaLibrary.MediaType.IMAGE;
        await media.createAsset(mediaType, "Scan_image_02.png", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanImageAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanImageAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN Image album (png) IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_IMAGE_PNG_FILE_014 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_IMAGE_PNG_FILE_014 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
    })

	/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_AAC_FILE_015
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_AUDIO_AAC_FILE_015', 0, async function (done) {
        console.info('MediaLibraryTest : create Audio album (AAC)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
        let mediaType = mediaLibrary.MediaType.AUDIO;
        await media.createAsset(mediaType, "audio.aac", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanAudioAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN AUDIO ALBUM IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_AAC_FILE_015 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_AAC_FILE_015 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_FLAC_FILE_016
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_AUDIO_FLAC_FILE_016', 0, async function (done) {
        console.info('MediaLibraryTest : create Audio album (FLAC)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
        let mediaType = mediaLibrary.MediaType.AUDIO;
        await media.createAsset(mediaType, "audio.flac", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanAudioAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN Audio album (FLAC) IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_FLAC_FILE_016 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_FLAC_FILE_016 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_WAV_FILE_017
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_AUDIO_WAV_FILE_017', 0, async function (done) {
        console.info('MediaLibraryTest : create Audio album (WAV)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
        let mediaType = mediaLibrary.MediaType.AUDIO;
        await media.createAsset(mediaType, "audio.wav", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanAudioAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN Audio album (WAV) IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_WAV_FILE_017 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_WAV_FILE_017 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_MP3_FILE_018
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_AUDIO_MP3_FILE_018', 0, async function (done) {
        console.info('MediaLibraryTest : create Audio album (MP3)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
        let mediaType = mediaLibrary.MediaType.AUDIO;
        await media.createAsset(mediaType, "scan_audio.MP3", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanAudioAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN Audio album (MP3) IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_MP3_FILE_018 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_MP3_FILE_018 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_OGG_FILE_019
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_AUDIO_OGG_FILE_019', 0, async function (done) {
    console.info('MediaLibraryTest : create Audio album (OGG)');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
    let mediaType = mediaLibrary.MediaType.AUDIO;
    await media.createAsset(mediaType, "audio.0GG", path1).then (function (asset) {
        console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
    }).catch(function(err){
        console.info("MediaLibraryTest :createAsset failed with error:"+ err);
    });
    await sleep(1000);
    let scanAudioAlbumPath = "/storage/media/100/local/files/"+path1
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN Audio album (OGG) IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_OGG_FILE_019 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_OGG_FILE_019 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_VIDEO_MP4_FILE_020
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_VIDEO_MP4_FILE_020', 0, async function (done) {
        console.info('MediaLibraryTest : create Video album (MP4)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_VIDEO);
        let mediaType = mediaLibrary.MediaType.VIDEO;
        await media.createAsset(mediaType, "scan_video.mp4", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanVideoAlbumPath = "/storage/media/100/local/files/"+path1
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanVideoAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN Video album (MP4) IS SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_VIDEO_MP4_FILE_020 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_VIDEO_MP4_FILE_020 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_DOC_FILE_021
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_DOC_FILE_021', 0, async function (done) {
    console.info('MediaLibraryTest : create doc album (txt)');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_DOCUMENTS);
    let mediaType = mediaLibrary.MediaType.FILE;
    await media.createAsset(mediaType, "Scan_Doc.txt", path1).then (function (asset) {
        console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
    }).catch(function(err){
        console.info("MediaLibraryTest :createAsset failed with error:"+ err);
    });
    await sleep(1000);
    let scanDocAlbumPath = "/storage/media/100/local/files/Documents/Scan_Doc.txt"
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanDocAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN doc album (txt) IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_DOC_FILE_021 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_DOC_FILE_021 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})


	    /* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_OPEN_CLOSE_024
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_OPEN_CLOSE_024', 0, async function (done) {
        console.info('MediaLibraryTest : create Audio album (MP3)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
        let mediaType = mediaLibrary.MediaType.AUDIO;
        await media.createAsset(mediaType, "open_close_scan_audio.mp3", path1).then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
            asset.open('rw', (openError, fd) => {
                console.info("MediaLibraryTest :openAsset successfully:");
                if(fd > 0){
                    asset.close(fd);
                    console.info("MediaLibraryTest :closeAsset successfully:");
                }else{
                    console.info('MediaLibraryTest :File Open Failed!' + openError);
                }
            });
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);

        let scanAudioAlbumPath = "/storage/media/100/local/files/Music/open_close_scan_audio.mp3"
        if (scannerObj == null || scannerObj == undefined) {
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanFile(scanAudioAlbumPath,async (err, data) => {
                if (data != undefined) {
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : status is ' + data.status);
                    console.info('MediaLibraryTest : status is ' + data.fileUri);
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_OPEN_CLOSE_024 : PASS');
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : scandfile has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_OPEN_CLOSE_024 : FAIL');
                }
                await sleep (10000);
                done();
            });
            await sleep (10000);
            done();
        }
        await sleep (10000);
        done();
    })

	    /* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_EMPTY_PATH_025
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_EMPTY_PATH_025', 0, async function (done) {
        console.info('MediaLibraryTest : starting of scan file');
        let audioAlbumPath = "";
        if (scannerObj == null || scannerObj == undefined) {
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanFile(audioAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is ' + data.status);
                    if (data.status == 1){
                        expect(true).assertTrue();
                        console.info('MediaLibraryTest : status is ' + data.fileUri);
                        console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_EMPTY_PATH_025 : PASS');
                    }else {
                        expect(false).assertTrue();
                        console.info('MediaLibraryTest : scandfile has an error');
                        console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_EMPTY_PATH_025 : FAIL');
                    }
                } else {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : scandfile has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_EMPTY_PATH_025 : FAIL');
                }
                await sleep (10000);
                done();
            });
            await sleep (10000);
            done();
        }
        await sleep (10000);
        done();
    })

	/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_NONEXISTING_PATH_026
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_NONEXISTING_PATH_026', 0, async function (done) {
    console.info('MediaLibraryTest : starting of scan file');
    const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
    let audioAlbumPath = path1+"/NONEXISTING.aac";
    if (scannerObj == null || scannerObj == undefined) {
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanFile(audioAlbumPath,async (err, data) => {
            if (data != undefined) {
                expect(true).assertTrue();
                console.info('MediaLibraryTest : status is ' + data.status);
                console.info('MediaLibraryTest : status is ' + data.fileUri);
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_NONEXISTING_PATH_026 : PASS');
            } else {
                expect(false).assertTrue();
                console.info('MediaLibraryTest : scandfile has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_NONEXISTING_PATH_026 : FAIL');
            }
            await sleep (10000);
            done();
        });
        await sleep (10000);
        done();
    }
    await sleep (10000);
    done();
})

    /* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_UNICODE_PATH_AND_FILE_027
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */

    it('SUB_SCANNER_CB_SCAN_UNICODE_PATH_AND_FILE_027', 0, async function (done) {
        console.info('MediaLibraryTest : create Audio album (MP3 unicode)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_AUDIO);
        let mediaType = mediaLibrary.MediaType.AUDIO;
        await media.createAsset(mediaType, "来自于华为公司_1.MP3", path1+"/来自于华为公司_1").then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanAudioAlbumPath = "/storage/media/100/local/files/Music/来自于华为公司_1"+"/来自于华为公司_1.MP3"
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
                if (data != undefined) {
                    if (data.status == -1) {
                        console.info('MediaLibraryTest : status is '+ data.status);
                        console.info('MediaLibraryTest : SCAN Audio album (MP3 unicode) IS SUCCESSFULL');
                        expect(true).assertTrue();
                        console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_UNICODE_PATH_AND_FILE_027 : PASS');
                    } else {
                        expect(true).assertfalse();
                        console.info('MediaLibraryTest : scandir has an error');
                        console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_UNICODE_PATH_AND_FILE_027 : FAIL');
                    }

                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_UNICODE_PATH_AND_FILE_027 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
    })

	/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_SAME_FILE_AGAIN_028
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_SAME_FILE_AGAIN_028', 0, async function (done) {
    console.info('MediaLibraryTest : create Audio album (MP3)');
    let scanAudioAlbumPath = "/storage/media/100/local/files/Music/audio.mp3"
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(scanAudioAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN Audio album (MP3) IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_SAME_FILE_AGAIN_028 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_SAME_FILE_AGAIN_028 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

    /* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_CANONICAL_PATH_FILE_AGAIN_029
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


    it('SUB_SCANNER_CB_SCAN_CANONICAL_PATH_FILE_AGAIN_029', 0, async function (done) {
        console.info('MediaLibraryTest : create Image album (JPG)');
        const path1 = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
        let mediaType = mediaLibrary.MediaType.IMAGE;
        await media.createAsset(mediaType, "image_UNICODE.jpg", path1+"1/2/3/4/").then (function (asset) {
            console.info("MediaLibraryTest :createAsset successfully:"+ JSON.stringify(asset));
        }).catch(function(err){
            console.info("MediaLibraryTest :createAsset failed with error:"+ err);
        });
        await sleep(1000);
        let scanImageAlbumPath = "/storage/media/100/local/files/Pictures/1/2/3/4/../../../../image.jpg"
        console.info('MediaLibraryTest : starting of scan dir');
        if (scannerObj == null || scannerObj == undefined){
            console.info('MediaLibraryTest : Error sannerObj is null');
        } else {
            scannerObj.scanFile(scanImageAlbumPath,async (err, data) => {
                if (data != undefined) {
                    console.info('MediaLibraryTest : status is '+ data.status);
                    console.info('MediaLibraryTest : SCAN Image album (JPG) SUCCESSFULL');
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_CANONICAL_PATH_FILE_AGAIN_029 : PASS');
                } else {
                    expect(true).assertfalse();
                    console.info('MediaLibraryTest : scandir has an error');
                    console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_CANONICAL_PATH_FILE_AGAIN_029 : FAIL');
                }
                await sleep (10000);
                done();
            });
        }
        await sleep(1000);
        done();
    })

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_MODIFIED_ALBUM_006
        * @tc.name      : CreateAlbum-->ModifyAlbum
        * @tc.desc      : CreateAlbum-->ModifyAlbum
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_MODIFIED_ALBUM_006', 0, async function (done) {
    let allTypefetchOp = {
        selections: '',
        selectionArgs: [],
    };
    try {
        const albumList = await media.getAlbums(allTypefetchOp);
        const album = albumList[0];
        const albumId = album.albumId;
        console.info('ALBUM_PROMISE Modify 003_01 album.albumName(old) = ' + album.albumName);
        const newName = 'newhello';
        console.info('#############MediaLibraryTest : original album name is  '+ album.albumName);
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
        done();
    } catch (error) {
        console.info('ALBUM_PROMISE Modify 003_01 failed, message = ' + error);
    }
    done();
    let modifiedAlbumPath = "/storage/media/100/local/files/"+"newhello";
    await sleep(1000);
    console.info('MediaLibraryTest : starting of scan dir');
    if (scannerObj == null || scannerObj == undefined){
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanDir(modifiedAlbumPath,async (err, data) => {
            if (data != undefined) {
                console.info('MediaLibraryTest : status is '+ data.status);
                console.info('MediaLibraryTest : SCAN MODIFED ALBUM IS SUCCESSFULL');
                expect(true).assertTrue();
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_MODIFIED_ALBUM_006 : PASS');
            } else {
                expect(true).assertfalse();
                console.info('MediaLibraryTest : scandir has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_MODIFIED_ALBUM_006 : FAIL');
            }
            await sleep (10000);
            done();
        });
    }
    await sleep(1000);
    done();
})

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_MODIFY_022
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */


it('SUB_SCANNER_CB_SCAN_AUDIO_MP4_AFTER_MODIFY_022', 0, async function (done) {
    let fileKeyObj = mediaLibrary.FileKey;
    let imageType = mediaLibrary.MediaType.VIDEO;
    let imagesfetchOp = {
        selections: fileKeyObj.MEDIA_TYPE + '= ?',
        selectionArgs: [imageType.toString()],
    };
    try {
        const fetchFileResult = await media.getFileAssets(imagesfetchOp);
        const asset = await fetchFileResult.getFirstObject();
        console.info('##################MediaLibraryTest : ASSET is ' + asset);
        console.info('##################MediaLibraryTest : ASSET Title is ' + asset.title);
        console.info('##################MediaLibraryTest : ASSET Title is ' + asset.id);
        const newTitle = 'newTitle';
        asset.title = newTitle;
        const id = asset.id;
        await asset.commitModify();

        //const fetchFileResult2 = await media.getFileAssets(imagesfetchOp);
        const dataList = await fetchFileResult.getAllObject();
        let passed = false;
        for (let i = 0; i < dataList.length; i++) {
            const asset = dataList[i];
            if (asset.id == id && asset.title == newTitle) {
                passed = true;
                break;
            }
        }
        done();
    } catch (error) {
        console.info('FileAsset commitModify 002 failed, message = ' + error);
        expect(false).assertTrue();
        done();
    }
    await sleep(1000);
    let scanAudioAlbumPath = "/storage/media/100/local/files/Movies/newTitle.mp4"
    if (scannerObj == null || scannerObj == undefined) {
        console.info('MediaLibraryTest : Error sannerObj is null');
    } else {
        scannerObj.scanFile(scanAudioAlbumPath,async (err, data) => {
            if (data != undefined) {
                expect(true).assertTrue();
                console.info('MediaLibraryTest : status is ' + data.status);
                console.info('MediaLibraryTest : status is ' + data.fileUri);
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_MODIFY_022 : PASS');
            } else {
                expect(false).assertTrue();
                console.info('MediaLibraryTest : scandfile has an error');
                console.info('MediaLibraryTest : SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_MODIFY_022 : FAIL');
            }
            await sleep (10000);
            done();
        });
        await sleep (10000);
        done();
    }
    await sleep (10000);
    done();

});

function sleep (ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_DELETE_ALBUM_007[MANUAL TEST]
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */

/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_MODIFY_DELETE_ALBUM_008[MANUAL TEST]
        * @tc.name      : CreateAlbum-->ModifyAlbum-->DeleteAlbum-->Scan
        * @tc.desc      : CreateAlbum-->ModifyAlbum-->DeleteAlbum-->Scan
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
	
/* *
        * @tc.number    : SUB_SCANNER_CB_SCAN_AUDIO_MP3_AFTER_DELETE_023 [Manual test]
        * @tc.name      :
        * @tc.desc      :
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */



})

