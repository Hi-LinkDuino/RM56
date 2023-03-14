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

describe('fetchFileResultPromise.test.js', async function() {
    var context = featureAbility.getContext();
    var media = mediaLibrary.getMediaLibrary(context);
    beforeAll(function() {});
    beforeEach(function() {});
    afterEach(function() {});
    afterAll(function() {});

    // ------------------------------ 001 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_01
     * @tc.name      : getCount
     * @tc.desc      : Get FetchResult by getFileCountOneOp, check the return value of the interface (by Promise)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_01', 0, async function(done) {
        try {
            console.info('MediaLibraryTest :  FETCHRESULT getCount 001_01 begin');
            let fetchFileResult = await media.getFileAssets(getFileCountOneOp);
            const fetchCount = await fetchFileResult.getCount();
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_01 count:' + fetchCount);
            expect(fetchCount == 1).assertTrue();
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_01 end');
            await fetchFileResult.close();
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_02
     * @tc.name      : getCount
     * @tc.desc      : Get FetchResult by getFileCountTwoOp, check the return value of the interface (by Promise)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_02', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_02 begin');
            let fetchFileResult = await media.getFileAssets(getFileCountTwoOp);
            const fetchCount = await fetchFileResult.getCount();
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_02 count:' + fetchCount);
            expect(fetchCount == 2).assertTrue();
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_02 end');
            fetchFileResult.close();
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_03
     * @tc.name      : getCount
     * @tc.desc      : Get FetchResult by getFileCountOneHundredOp, check the return value of the interface (by Promise)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_03', 0, async function(done) {
        try {
            console.info('MediaLibraryTest :FETCHRESULT getCount 001_03 begin');
            let fetchFileResult = await media.getFileAssets(getFileCountOneHundredOp);
            const fetchCount = await fetchFileResult.getCount();
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_03:' + fetchCount);
            expect(fetchCount > 0).assertTrue();
            expect(fetchCount <= 100).assertTrue();
            console.info('MediaLibraryTest :FETCHRESULT getCount 001_03 end');
            fetchFileResult.close();
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_03 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_04
     * @tc.name      : getCount
     * @tc.desc      : Get FetchResult by getFileCountZeroOp, check the return value of the interface (by Promise)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETCOUNT_PROMISE_001_04', 0, async function(done) {
        try {
            console.info('MediaLibraryTest :  FETCHRESULT getCount 001_04 begin');
            let fetchFileResult = await media.getFileAssets(getFileCountZeroOp);
            const fetchCount = await fetchFileResult.getCount();
            console.info('MediaLibraryTest :  FETCHRESULT getCount 001_04 count:' + fetchCount);
            expect(fetchCount == 0).assertTrue();
            console.info('MediaLibraryTest :  FETCHRESULT getCount 001_04 end');
            fetchFileResult.close();
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getCount 001_04 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    // ------------------------------ 001 test end -------------------------

    // ------------------------------ 002 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_ISAFTERLAST_PROMISE_002
     * @tc.name      : getCount
     * @tc.desc      : Get FetchResult by getFileCountTenOp, check the return value of the interface (by Promise)
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_ISAFTERLAST_PROMISE_002', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : isAfterLast begin');
            let fetchFileResult = await media.getFileAssets(getFileCountTenOp);
            const fetchCount = await fetchFileResult.getCount();
            console.info('MediaLibraryTest : count:' + fetchCount);
            let fileAsset = await fetchFileResult.getFirstObject();
            for (var i = 1; i < fetchCount; i++) {
                fileAsset = await fetchFileResult.getNextObject();
                if (i == fetchCount - 1) {
                    console.info('MediaLibraryTest : FETCHRESULT isAfterLast 002 isLast');
                    var result = fetchFileResult.isAfterLast();
                    console.info('MediaLibraryTest : FETCHRESULT isAfterLast 002 isAfterLast:' + result);
                    expect(true).assertTrue();
                    console.info('MediaLibraryTest : FETCHRESULT isAfterLast 002 isAfterLast end');
                    fetchFileResult.close();
                    done();
                }
            }
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT isAfterLast 002 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 002 test end -------------------------

    // ------------------------------ 003 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_CLOSE_PROMISE_003
     * @tc.name      : getCount
     * @tc.desc      : Get FetchResult, close it, check if result closed
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_CLOSE_PROMISE_003', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT close 003 begin');
            let fetchFileResult = await media.getFileAssets(getFileCountTenOp);
            fetchFileResult.close();
            try {
                fetchFileResult.getCount();
                console.info('MediaLibraryTest : FETCHRESULT close 003 failed');
                expect(false).assertTrue();
                let fileAsset = await fetchFileResult.getFirstObject();
                expect(false).assertTrue();
                console.info('MediaLibraryTest : FETCHRESULT close getFirstObject 003 failed');
                fileAsset = await fetchFileResult.getNextObject();
                expect(false).assertTrue();
                console.info('MediaLibraryTest : FETCHRESULT close getNextObject 003 failed');
                fileAsset = await fetchFileResult.getLastObject();
                expect(false).assertTrue();
                console.info('MediaLibraryTest : FETCHRESULT close getLastObject 003 failed');
                let targetObject = await fetchFileResult.getPositionObject(0);
                expect(false).assertTrue();
                console.info('MediaLibraryTest : FETCHRESULT close getPositionObject 003 failed');
                s
                done();
            } catch {
                console.info('MediaLibraryTest : FETCHRESULT close 003 closed');
                expect(true).assertTrue();
                done();
            }
            console.info('MediaLibraryTest : FETCHRESULT close 003 end');
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT close 003 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    // ------------------------------ 003 test end -------------------------

    // ------------------------------ 004 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETFIRSTOBJECT_PROMISE_004
     * @tc.name      : getFirstObject
     * @tc.desc      : Get FetchResult, get first object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETFIRSTOBJECT_PROMISE_004', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getFirstObject begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            let firstObject = await fetchFileResult.getFirstObject();
            expect(firstObject.relativePath == 'Camera/').assertTrue();
            fetchFileResult.close();
            console.info('MediaLibraryTest : FETCHRESULT getFirstObject 004 end');
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getFirstObject 004 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    // ------------------------------ 004 test end -------------------------

    // ------------------------------ 005 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETNEXTOBJECT_PROMISE_005
     * @tc.name      : getNextObject
     * @tc.desc      : Get FetchResult, get first object, get next object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETNEXTOBJECT_PROMISE_005', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getNextObject 005 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            let firstObject = await fetchFileResult.getFirstObject();
            expect(firstObject.relativePath == 'Camera/').assertTrue();
            let nextObject = await fetchFileResult.getNextObject();
            expect(nextObject.relativePath == 'Camera/').assertTrue();
            fetchFileResult.close();
            done();
            console.info('MediaLibraryTest : FETCHRESULT getNextObject 005  end');
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getNextObject 005 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 005 test end -------------------------

    // ------------------------------ 006 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETLASTOBJECT_PROMISE_006
     * @tc.name      : getLastObject
     * @tc.desc      : Get FetchResult, get first object, get next object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETLASTOBJECT_PROMISE_006', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getLastObject begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            let lastObject = await fetchFileResult.getLastObject();
            expect(lastObject.relativePath == 'Camera/').assertTrue();
            fetchFileResult.close();
            console.info('MediaLibraryTest : FETCHRESULT getLastObject 006 end');
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getLastObject 006 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 006 test end -------------------------

    // ------------------------------ 007 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_01
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 0 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_01', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_01 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            let targetObject = await fetchFileResult.getPositionObject(0);
            expect(targetObject.relativePath == 'Camera/').assertTrue();
            fetchFileResult.close();
            done();
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_01 end');
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_02
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 1 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_02', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_02 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            let targetObject = await fetchFileResult.getPositionObject(1);
            expect(targetObject.relativePath == 'Camera/').assertTrue();
            fetchFileResult.close();
            done();
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_02 end');
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_02
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 1 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_03', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_03 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            const count = await fetchFileResult.getCount();
            let targetObject = await fetchFileResult.getPositionObject(count - 1);
            expect(targetObject.relativePath == 'Camera/').assertTrue();
            fetchFileResult.close();
            done();
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_03 end');
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_03 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_04
     * @tc.name      : getPositionObject
     * @tc.desc      : Get FetchResult, get position 1 object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETPOSITIONOBJECT_PROMISE_007_04', 0, async function(done) {
        try {
            console.info('MediaLibraryTest :  FETCHRESULT getPositionObject 007_04 begin');
            let fetchFileResult = await media.getFileAssets(getFirstObjectOp);
            const count = await fetchFileResult.getCount();
            try {
                let targetObject = await fetchFileResult.getPositionObject(count + 100);
                if (targetObject == undefined) {
                    expect(true).assertTrue();
                    fetchFileResult.close();
                    done();
                }
                expect(false).assertTrue();
                fetchFileResult.close();
                done();
            } catch (err) {
                expect(true).assertTrue();
                fetchFileResult.close();
                done();
            }
            console.info('MediaLibraryTest :  FETCHRESULT getPositionObject 007_04 end');
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getPositionObject 007_04 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 007 test end -------------------------

    // ------------------------------ 008 test start -------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_PROMISE_008_01
     * @tc.name      : getAllObject
     * @tc.desc      : Get FetchResult, get all object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_PROMISE_008_01', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_01 begin');
            let fetchFileResult = await media.getFileAssets(getAllObjectLimitOneOp);
            var targetObjects = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_01 targetObjects.length:'
                         + targetObjects.length);
            expect(targetObjects.length <= 1).assertTrue();
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_01 end');
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_PROMISE_008_02
     * @tc.name      : getAllObject
     * @tc.desc      : Get FetchResult, get all object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_PROMISE_008_02', 0, async function(done) {
        try {
            console.info('MediaLibraryTest :  FETCHRESULT getAllObject 008_02 begin');
            let fetchFileResult = await media.getFileAssets(getAllObjectLimitTwoOp);
            var targetObjects = await fetchFileResult.getAllObject();
            expect(targetObjects.length <= 2).assertTrue();
            console.info('MediaLibraryTest :  FETCHRESULT getAllObject 008_02 targetObjects.length:'
                         + targetObjects.length);
            console.info('MediaLibraryTest :  FETCHRESULT getAllObject 008_02 end');
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_PROMISE_008_03
     * @tc.name      : getAllObject
     * @tc.desc      : Get FetchResult, get all object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_GETALLOBJECT_PROMISE_008_03', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_03 begin');
            let fetchFileResult = await media.getFileAssets(getAllObjectLimitOneHundredOp);
            var targetObjects = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest :  FETCHRESULT getAllObject 008_03 targetObjects.length:'
                         + targetObjects.length);
            expect(targetObjects.length <= 100).assertTrue();
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_03 end');
            done();
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 008_03 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ 008 test end -------------------------

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_getFirstObject_PROMISE_009_01
     * @tc.name      : getFirstObject
     * @tc.desc      : Get FetchResult, get first object, check result
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FETCHRESULT_getFirstObject_PROMISE_009_01', 0, async function(done) {
        try {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 009_01 begin');
            let fetchFileResult = await media.getFileAssets(getFileOp);
            let firstObject = await fetchFileResult.getFirstObject();
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
        } catch (error) {
            console.info('MediaLibraryTest : FETCHRESULT getAllObject 009_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
});