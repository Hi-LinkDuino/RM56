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
let fileKeyObj = mediaLibrary.FileKey;
let fileType = mediaLibrary.MediaType.FILE;
let imageType = mediaLibrary.MediaType.IMAGE;
let videoType = mediaLibrary.MediaType.VIDEO;
let audioType = mediaLibrary.MediaType.AUDIO;

let getFileCountOneOp = {
    selections : fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs : [ fileType.toString() ],
    order : fileKeyObj.DATE_ADDED + " DESC LIMIT 0,1",
    extendArgs : "",
};

let getFileCountTwoOp = {
    selections : fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs : [ fileType.toString() ],
    order : fileKeyObj.DATE_ADDED + " DESC LIMIT 0,2",
    extendArgs : "",
};

let getFileCountTenOp = {
    selections : fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs : [ fileType.toString() ],
    order : fileKeyObj.DATE_ADDED + " DESC LIMIT 0,10",
    extendArgs : "",
};

let getFileCountOneHundredOp = {
    selections : fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs : [ fileType.toString() ],
    order : fileKeyObj.DATE_ADDED + " DESC LIMIT 0,100",
    extendArgs : "",
};

let getFirstObjectOp = {
    selections : fileKeyObj.RELATIVE_PATH + '= ?',
    selectionArgs : [ 'Camera/' ],
    order : fileKeyObj.ID + " DESC LIMIT 0,5",
    extendArgs : "",
}

let getAllObjectLimitOneOp = {
    selections : fileKeyObj.RELATIVE_PATH + '= ?',
    selectionArgs : [ 'Camera/' ],
    order : fileKeyObj.ID + " DESC LIMIT 0,1",
    extendArgs : "",
}

let getAllObjectLimitTwoOp = {
    selections : fileKeyObj.RELATIVE_PATH + '= ?',
    selectionArgs : [ 'Camera/' ],
    order : fileKeyObj.ID + " DESC LIMIT 0,1",
    extendArgs : "",
}

let getAllObjectLimitOneHundredOp = {
    selections : fileKeyObj.RELATIVE_PATH + '= ?',
    selectionArgs : [ 'Camera/' ],
    order : fileKeyObj.ID + " DESC LIMIT 0,100",
    extendArgs : "",
}

let getFileCountZeroOp = {
    selections : fileKeyObj.DISPLAY_NAME + '=?',
    selectionArgs : [ 'The world has kissed my soul with its pain, asking for its return in songs.' ],
};

let getFileOp = {
    selections : fileKeyObj.DISPLAY_NAME + '= ? AND ' + fileKeyObj.RELATIVE_PATH + '= ?',
    selectionArgs : [ '01.jpg', 'Camera/' ],
    order : fileKeyObj.ID + " DESC LIMIT 0,100",
    extendArgs : "",
}

describe('fetchFileResultCallback.test.js', async function() {
    var context = featureAbility.getContext();
    var media = mediaLibrary.getMediaLibrary(context);
    beforeAll(function() {});
    beforeEach(function() {});
    afterEach(function() {});
    afterAll(function() {});

    // ------------------------------ 004 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETFIRSTOBJECT_CALLBACK_004
     * @tc.name      : getFirstObject
     * @tc.desc      : Get FetchResult, get first object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETFIRSTOBJECT_CALLBACK_004', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getFirstObject begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            fetchFileResult.getFirstObject(async (error, firstObject) => {
                if (firstObject == undefined) {
                    expect(false).assertTrue();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 004  fail');
                    done();
                } else {
                    expect(firstObject.relativePath == 'Camera/').assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getFirstObject 004 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getFirstObject 004 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    // ------------------------------ 004 test end -------------------------

    // ------------------------------ 005 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETNEXTOBJECT_CALLBACK_005
     * @tc.name      : getNextObject
     * @tc.desc      : Get FetchResult, get first object, get next object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETNEXTOBJECT_CALLBACK_005', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getNextObject 005 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            let firstObject = await fetchFileResult.getFirstObject();
            expect(firstObject.relativePath == 'Camera/').assertTrue();
            fetchFileResult.getNextObject(async (error, nextObject) => {
                if (nextObject == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    done();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 005  fail');
                } else {
                    expect(nextObject.relativePath == 'Camera/').assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 005  end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getNextObject 005 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 005 test end -------------------------

    // ------------------------------ 006 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETLASTOBJECT_CALLBACK_006
     * @tc.name      : getLastObject
     * @tc.desc      : Get FetchResult, get first object, get next object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETLASTOBJECT_CALLBACK_006', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getLastObject begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            fetchFileResult.getLastObject(async (error, lastObject) => {
                if (lastObject == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 006  fail');
                    done();
                } else {
                    expect(lastObject.relativePath == 'Camera/').assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 006  end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getLastObject 006 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 006 test end -------------------------

    // ------------------------------ 007 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_01
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 0 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_01', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_01 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            fetchFileResult.getPositionObject(0, async (error, targetObject) => {
                if (targetObject == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 007_01  fail');
                    done();
                } else {
                    expect(targetObject.relativePath == 'Camera/').assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_01 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_02
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 1 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_02', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_02 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            fetchFileResult.getPositionObject(1, async (error, targetObject) => {
                if (targetObject == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 007_02  fail');
                    done();
                } else {
                    expect(targetObject.relativePath == 'Camera/').assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_02 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_02
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 1 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_03', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_03 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            const count = await fetchFileResult.getCount();
            fetchFileResult.getPositionObject(count - 1, async (error, targetObject) => {
                if (targetObject == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 007_03  fail');
                    done();
                } else {
                    expect(targetObject.relativePath == 'Camera/').assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_03 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_03 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_04
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 1 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_CALLBACK_007_04', 0, async function(done) {
        try {
            console.info('MediaLibraryTest :  FETCHRESULT getPositionObject 007_04 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            const count = await fetchFileResult.getCount();
            try {
                fetchFileResult.getPositionObject(count + 100, async (error, targetObject) => {
                    if (targetObject == undefined) {
                        expect(true).assertTrue();
                        fetchFileResult.close();
                        console.info('MediaLibraryTest : FETCHRESULT getNextObject 007_04  fail');
                        done();
                    } else {
                        expect(false).assertTrue();
                        fetchFileResult.close();
                        console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_04 end');
                        done();
                    }
                });
            } catch (err) {
                expect(true).assertTrue();
                fetchFileResult.close();
                done();
            }
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_04 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 007 test end -------------------------

    // ------------------------------ 008 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_CALLBACK_008_01
     * @tc.name      : getAllObject
     * @tc.desc      : Get FetchResult, get all object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_CALLBACK_008_01', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_01 begin');
            let fetchFileResult = await media.getFileAssets(getAllObjectLimitOneOp);
            fetchFileResult.getAllObject((error, targetObjects) => {
                if (targetObjects == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 008_01  fail');
                    done();
                } else {
                    expect(0 < targetObjects.length <= 1).assertTrue();
                    console.info('MediaLibraryTest : FETCHRESULT getPositionObject 008_01 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_CALLBACK_008_02
     * @tc.name      : getAllObject
     * @tc.desc      : Get FetchResult, get all object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_CALLBACK_008_02', 0, async function(done) {
        try {
            console.info('MediaLibraryTest :  FETCHRESULT getAllObject 008_02 begin');
            let fetchFileResult = await media.getFileAssets(getAllObjectLimitTwoOp);
            fetchFileResult.getAllObject((error, targetObjects) => {
                if (targetObjects == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 008_01  fail');
                    done();
                } else {
                    expect(0 < targetObjects.length <= 2).assertTrue();
                    console.info('MediaLibraryTest :  FETCHRESULT getAllObject 008_02 targetObjects.length:'
                                 + targetObjects.length);
                    console.info('MediaLibraryTest :  FETCHRESULT getAllObject 008_02 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_CALLBACK_008_03
     * @tc.name      : getAllObject
     * @tc.desc      : Get FetchResult, get all object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_CALLBACK_008_03', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_03 begin');
            let fetchFileResult = await media.getFileAssets(getAllObjectLimitOneHundredOp);
            fetchFileResult.getAllObject((error, targetObjects) => {
                if (targetObjects == undefined) {
                    expect(false).assertTrue();
                    fetchFileResult.close();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 008_03  fail');
                    done();
                } else {
                    expect(0 < targetObjects.length <= 100).assertTrue();
                    console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_03 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_03 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 008 test end -------------------------

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_getFirstObject_CALLBACK_009_01
     * @tc.name      : getFirstObject
     * @tc.desc      : Get FetchResult, get first object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_getFirstObject_CALLBACK_009_01', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 009_01 begin');
            let fetchFileResult = await media.getFileAssets(getFileOp);
            fetchFileResult.getFirstObject(async (error, firstObject) => {
                if (firstObject == undefined) {
                    expect(false).assertTrue();
                    done();
                    console.info('MediaLibraryTest : FETCHRESULT getNextObject 004  fail');
                } else {
                    expect(firstObject.id != undefined).assertTrue();
                    if (firstObject.id == undefined) {
                        console.info('MediaLibraryTest :firstObject.id == undefined');
                    }

                    expect(firstObject.uri != undefined).assertTrue();
                    if (firstObject.uri == undefined) {
                        console.info('MediaLibraryTest :firstObject.uri === undefined');
                    }

                    expect(firstObject.mimeType == 'image/*').assertTrue();
                    if (firstObject.mimeType != 'image/*') {
                        console.info('MediaLibraryTest :firstObject.mimeType:' + firstObject.mimeType);
                    }

                    expect(firstObject.mediaType == imageType).assertTrue();
                    if (firstObject.mediaType != imageType) {
                        console.info('MediaLibraryTest :firstObject.mediaType:' + firstObject.mediaType);
                    }

                    expect(firstObject.displayName == '01.jpg').assertTrue();
                    if (firstObject.displayName != '01.jpg') {
                        console.info('MediaLibraryTest :firstObject.mediaType:' + firstObject.mediaType);
                    }

                    expect(firstObject.title == '01').assertTrue();
                    if (firstObject.title != '01') {
                        console.info('MediaLibraryTest :firstObject.title:' + firstObject.title);
                    }

                    expect(firstObject.relativePath == 'Camera/').assertTrue();
                    if (firstObject.relativePath != 'Camera/') {
                        console.info('MediaLibraryTest :firstObject.relativePath:' + firstObject.relativePath);
                    }

                    expect(firstObject.parent != undefined).assertTrue();
                    if (firstObject.parent == undefined) {
                        console.info('MediaLibraryTest :firstObject.parent == undefined');
                    }

                    expect(firstObject.size == 348113).assertTrue();
                    if (firstObject.size != 348113) {
                        console.info('MediaLibraryTest :firstObject.size:' + firstObject.size);
                    }

                    expect(firstObject.dateTaken == 0).assertTrue();
                    if (firstObject.dateTaken != 0) {
                        console.info('MediaLibraryTest :firstObject.dateTaken:' + firstObject.dateTaken);
                    }

                    expect(firstObject.artist == '').assertTrue();
                    if (firstObject.artist != '') {
                        console.info('MediaLibraryTest :firstObject.artist:' + firstObject.artist);
                    }

                    expect(firstObject.audioAlbum == '').assertTrue();
                    if (firstObject.audioAlbum != '') {
                        console.info('MediaLibraryTest :firstObject.audioAlbum:' + firstObject.audioAlbum);
                    }

                    expect(firstObject.width == 1279).assertTrue();
                    if (firstObject.width != 1279) {
                        console.info('MediaLibraryTest :firstObject.width:' + firstObject.width);
                    }

                    expect(firstObject.height == 1706).assertTrue();
                    if (firstObject.height != 1706) {
                        console.info('MediaLibraryTest :firstObject.height:' + firstObject.height);
                    }

                    expect(firstObject.orientation == 0).assertTrue();
                    if (firstObject.orientation != 0) {
                        console.info('MediaLibraryTest :firstObject.orientation:' + firstObject.orientation);
                    }

                    expect(firstObject.duration == 0).assertTrue();
                    if (firstObject.duration != 0) {
                        console.info('MediaLibraryTest :firstObject.duration:' + firstObject.duration);
                    }

                    expect(firstObject.albumId != undefined).assertTrue();
                    if (firstObject.albumId == undefined) {
                        console.info('MediaLibraryTest :firstObject.albumId == undefined');
                    }

                    expect(firstObject.albumUri != undefined).assertTrue();
                    if (firstObject.albumUri == undefined) {
                        console.info('MediaLibraryTest :firstObject.albumUri:' + firstObject.albumUri);
                    }

                    expect(firstObject.albumName == 'Camera').assertTrue();
                    if (firstObject.albumName != 'Camera') {
                        console.info('MediaLibraryTest :firstObject.albumName:' + firstObject.albumName);
                    }

                    expect(firstObject.dateAdded != undefined).assertTrue();
                    if (firstObject.dateAdded == undefined) {
                        console.info('MediaLibraryTest :firstObject.dateAdded:' + firstObject.dateAdded);
                    }

                    expect(firstObject.albumName != undefined).assertTrue();
                    if (firstObject.albumName == undefined) {
                        console.info('MediaLibraryTest :firstObject.albumName:' + firstObject.albumName);
                    }
                    console.info('MediaLibraryTest : FETCHRESULT getAllObject 009_01 end');
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 009_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
});