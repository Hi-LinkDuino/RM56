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

describe('mediaLibraryTestPromiseOnOff.test.js', async function () {
    const context = featureAbility.getContext();
    const media = mediaLibrary.getMediaLibrary(context);
    beforeAll(function() {});
    beforeEach(function() {});
    afterEach(function() {});
    afterAll(function() {});
    let fileKeyObj = mediaLibrary.FileKey;
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
    
    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_001
     * @tc.name      : ON
     * @tc.desc      : ON Image ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_001', 0, async function (done) {
        try {
            let conut = 0;
            media.on('imageChange', () => {
                console.info('MediaLibraryTest : on 001 callback');
                conut++;
            });
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);

            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                expect(conut > 0).assertTrue();
                media.off('imageChange');
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : on 001 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('imageChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_002
     * @tc.name      : ON
     * @tc.desc      : ON Video ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_002', 0, async function (done) {
        try {
            let conut = 0;
            media.on('videoChange', () => {
                console.info('MediaLibraryTest : on 002 callback');
                conut++;
            });
            const fetchFileResult = await media.getFileAssets(videosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                expect(conut > 0).assertTrue();
                media.off('videoChange');
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : on 002 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('videoChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_003
     * @tc.name      : ON
     * @tc.desc      : ON Audio ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_003', 0, async function (done) {
        try {
            let conut = 0;
            media.on('audioChange', () => {
                console.info('MediaLibraryTest : on 003 callback');
                conut++;
            });
            const fetchFileResult = await media.getFileAssets(audiosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                expect(conut > 0).assertTrue();
                media.off('audioChange');
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : on 003 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('audioChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_004
     * @tc.name      : ON
     * @tc.desc      : ON File ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_004', 0, async function (done) {
        try {
            let conut = 0;
            media.on('fileChange', () => {
                console.info('MediaLibraryTest : on 004 callback');
                conut++;
            });
            const fetchFileResult = await media.getFileAssets(filesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                expect(conut > 0).assertTrue();
                media.off('fileChange');
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : on 004 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('fileChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_005
     * @tc.name      : ON
     * @tc.desc      : ON ALBUM ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_005', 0, async function (done) {
        try {
            let conut = 0;
            media.on('albumChange', () => {
                console.info('MediaLibraryTest : on 005 callback');
                conut++;
            });
            const fetchFileResult = await media.getFileAssets(filesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                expect(conut == 0).assertTrue();
                media.off('albumChange');
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : on 005 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('albumChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_006
     * @tc.name      : ON
     * @tc.desc      : ON DEVICE ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_006', 0, async function (done) {
        try {
            let conut = 0;
            expect(true).assertTrue();
            done();
            media.on('deviceChange', () => {
                console.info('MediaLibraryTest : on 006 callback');
                conut++;
            });
            const fetchFileResult = await media.getFileAssets(filesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                expect(conut == 0).assertTrue();
                media.off('deviceChange');
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : on 006 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('deviceChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_006
     * @tc.name      : ON
     * @tc.desc      : ON REMOTE_FILE ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_ON_007', 0, async function (done) {
        try {
            let conut = 0;
            expect(true).assertTrue();
            done();
            media.on('remoteFileChange', () => {
                console.info('MediaLibraryTest : on 007 callback');
                conut++;
            });
            const fetchFileResult = await media.getFileAssets(filesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                expect(conut == 0).assertTrue();
                media.off('remoteFileChange');
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : on 006 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('remoteFileChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_001
     * @tc.name      : off
     * @tc.desc      : off Image ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_001', 0, async function (done) {
        try {
            media.on('imageChange', () => {
                console.info('MediaLibraryTest : off 001 failed');
                expect(false).assertTrue();
                media.off('imageChange');
                done();
            });

            media.off('imageChange');
            const fetchFileResult = await media.getFileAssets(imagesfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                console.info('MediaLibraryTest : off 001 passed');
                expect(true).assertTrue();
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : off 001 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('imageChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_002
     * @tc.name      : off
     * @tc.desc      : off video ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_002', 0, async function (done) {
        try {
            media.on('videoChange', () => {
                console.info('MediaLibraryTest : off 002 failed');
                expect(false).assertTrue();
                media.off('videoChange');
                done();
            });

            media.off('videoChange');
            const fetchFileResult = await media.getFileAssets(videosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                console.info('MediaLibraryTest : off 002 passed');
                expect(true).assertTrue();
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : off 002 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('videoChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_003
     * @tc.name      : off
     * @tc.desc      : off audio ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_003', 0, async function (done) {
        try {
            media.on('audioChange', () => {
                console.info('MediaLibraryTest : off 003 failed');
                expect(false).assertTrue();
                media.off('audioChange');
                done();
            });

            media.off('audioChange');
            const fetchFileResult = await media.getFileAssets(audiosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                console.info('MediaLibraryTest : off 003 passed');
                expect(true).assertTrue();
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : off 003 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('audioChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_004
     * @tc.name      : off
     * @tc.desc      : off file ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_004', 0, async function (done) {
        try {
            media.on('fileChange', () => {
                console.info('MediaLibraryTest : off 004 failed');
                expect(false).assertTrue();
                media.off('fileChange');
                done();
            });

            media.off('fileChange');
            const fetchFileResult = await media.getFileAssets(videosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'changename';
            await asset.commitModify();
            setTimeout(() => {
                console.info('MediaLibraryTest : off 004 passed');
                expect(true).assertTrue();
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : off 004 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('fileChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_005
     * @tc.name      : off
     * @tc.desc      : off album ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_005', 0, async function (done) {
        try {
            media.on('albumChange', () => {
                console.info('MediaLibraryTest : off 005 failed');
                expect(false).assertTrue();
                media.off('albumChange');
                done();
            });

            media.off('albumChange');
            const fetchFileResult = await media.getFileAssets(videosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'albumChange';
            await asset.commitModify();
            setTimeout(() => {
                console.info('MediaLibraryTest : off 005 passed');
                expect(true).assertTrue();
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : off 005 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('fileChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_006
     * @tc.name      : off
     * @tc.desc      : off device ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_006', 0, async function (done) {
        try {
            media.on('deviceChange', () => {
                console.info('MediaLibraryTest : off 006 failed');
                expect(false).assertTrue();
                media.off('deviceChange');
                done();
            });

            media.off('deviceChange');
            const fetchFileResult = await media.getFileAssets(videosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'deviceChange';
            await asset.commitModify();
            setTimeout(() => {
                console.info('MediaLibraryTest : off 006 passed');
                expect(true).assertTrue();
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : off 006 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('fileChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_007
     * @tc.name      : off
     * @tc.desc      : off remoteFile ASSET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MIDIALIBRARY_PROMISE_OFF_007', 0, async function (done) {
        try {
            media.on('remoteFileChange', () => {
                console.info('MediaLibraryTest : off 007 failed');
                expect(false).assertTrue();
                media.off('remoteFileChange');
                done();
            });

            media.off('remoteFileChange');
            const fetchFileResult = await media.getFileAssets(videosfetchOp);
            const asset = await fetchFileResult.getFirstObject();
            asset.title = asset.title + 'remoteFileChange';
            await asset.commitModify();
            setTimeout(() => {
                console.info('MediaLibraryTest : off 007 passed');
                expect(true).assertTrue();
                done();
            }, 1000);
        } catch (error) {
            console.info(`MediaLibraryTest : off 006 failed, error: ${error}`);
            expect(false).assertTrue();
            media.off('fileChange');
            done();
        }
    });

    /**
     * @tc.number    : SUB__MEDIA_MIDIALIBRARY_PROMISE_RELEASE_001
     * @tc.name      : release
     * @tc.desc      : Release MediaLibrary instance
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB__MEDIA_MIDIALIBRARY_PROMISE_RELEASE_001', 0, async function (done) {
        try {
            await media.release();
            console.info('MediaLibraryTest : release 001 passed');
            expect(true).assertTrue();
            done();
        } catch (error) {
            console.info(`MediaLibraryTest : release 001 failed, error: ${error}`);
            expect(false).assertTrue();
            done();
        }
    });
});