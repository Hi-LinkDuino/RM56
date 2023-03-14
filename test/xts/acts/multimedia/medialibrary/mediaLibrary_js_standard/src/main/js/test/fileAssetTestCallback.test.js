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
import fileio from '@ohos.fileio';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('fileAssetTestCallback.test.js', async function() {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    const fileKeyObj = mediaLibrary.FileKey;

    //======================== FILE BEGIN ==================================
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01
     * @tc.name      : open('rw')
     * @tc.desc      : open -rw the type of FILE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let fileType = mediaLibrary.MediaType.FILE;
            let fileFetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ fileType.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fileFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset.open('rw', async (error, fd) => {
                if (fd == undefined) {
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01  fd == undefined');
                    expect(false).assertTrue();
                    done();
                } else {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01  fd:'
                                 + fd);
                    expect(fd > 0).assertTrue();
                    let buf = new ArrayBuffer(4096);
                    let res = await fileio.read(fd, buf);
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01  res.bytesRead:'
                        + res.bytesRead);
                    expect(res.bytesRead >= 0).assertTrue();
                    asset1 = dataList[1];
                    fd1 = await asset1.open('r');
                    let buf2 = new ArrayBuffer(4096);
                    let res2 = await fileio.read(fd1, buf2);
                    let write = await fileio.write(fd, buf2);
                    expect(write > 0).assertTrue();
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01  write:'
                                 + write);
                    await asset.close(fd);
                    await asset1.close(fd1);
                    if (fd > 0 && res.bytesRead >= 0 && write > 0) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01  success');
                        done();
                    } else {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01  failed');
                        done();
                    }
                }
            });

        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_01  error' + error);
            await asset.close(fd);
            await asset1.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_02
     * @tc.name      : open('r')
     * @tc.desc      : open -r the type of FILE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_02', 0, async function(done) {
        let asset;
        let fd;
        try {
            let type = mediaLibrary.MediaType.FILE;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset.open('r', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_02  fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        expect(fd > 0).assertTrue();
                        console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_02 fd:'
                                     + fd);
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd, buf);
                        expect(res.bytesRead > 0).assertTrue();
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_02 res.bytesRead:'
                            + res.bytesRead);
                        let write = await fileio.write(fd, buf);
                        await asset.close(fd);
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_02  error:'
                                 + error);
                    expect(true).assertTrue();
                    if (fd > 0) {
                        await asset.close(fd);
                    }
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_02  error:'
                         + error);
            expect(false).assertTrue();
            if (fd > 0) {
                await asset.close(fd);
            }
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03
     * @tc.name      : open('w')
     * @tc.desc      : open -w the type of FILE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.FILE;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset1 = dataList[1];
            asset.open('w', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03  fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        expect(fd > 0).assertTrue();
                        console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03  fd:'
                                     + fd);
                        fd1 = await asset1.open('r');
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd1, buf);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03  read  fd1 success');
                        let write = await fileio.write(fd, buf);
                        expect(write > 0).assertTrue();
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03  write:'
                            + write);
                        let buf1 = new ArrayBuffer(4096);
                        let res1 = await fileio.read(fd, buf1);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03  read fd');
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03  error:'
                                 + error);
                    expect(true).assertTrue();
                    await asset.close(fd);
                    await asset1.close(fd1);
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_03  error:'
                         + error);
            expect(false).assertTrue();
            await asset.close(fd);
            await asset1.close(fd1);
            done();
        }
    });

    //======================== FILE END ==================================

    //======================== ALBUM BEGIN ==================================
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_04
     * @tc.name      : open('rw')
     * @tc.desc      : open -rw the type of ALBUM
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_04', 0, async function(done) {
        let asset;
        let fd;
        try {
            let type = mediaLibrary.MediaType.ALBUM;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset.open('rw', async (error, fd) => {
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_04  error:'
                         + error);
            expect(true).assertTrue();
            if (fd > 0) {
                asset.close(fd);
            }
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_05
     * @tc.name      : open('r')
     * @tc.desc      : open -r the type of ALBUM
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_05', 0, async function(done) {
        let asset;
        let fd;
        try {
            let type = mediaLibrary.MediaType.ALBUM;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset.open('r', async (error, fd) => {
                expect(false).assertTrue();
                done();
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_05  error:'
                         + error);
            expect(true).assertTrue();
            if (fd > 0) {
                asset.close(fd);
            }
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_06
     * @tc.name      : open('w')
     * @tc.desc      : open -w the type of ALBUM
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_06', 0, async function(done) {
        let asset;
        let fd;
        try {
            let type = mediaLibrary.MediaType.ALBUM;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset.open('w', (error, fd) => {
                expect(false).assertTrue();
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_06  error:'
                         + error);
            expect(true).assertTrue();
            if (fd > 0) {
                asset.close(fd);
            }
            done();
        }
    });

    //======================== ALBUM END ==================================

    //======================== IMAGE BEGIN ==================================
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_07
     * @tc.name      : open('rw')
     * @tc.desc      : open -rw the type of IMAGE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_07', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let fileType = mediaLibrary.MediaType.IMAGE;
            let fileFetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ fileType.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fileFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];

            asset.open('rw', async (error, fd) => {
                if (fd == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    expect(fd > 0).assertTrue();
                    let buf = new ArrayBuffer(4096);
                    let res = await fileio.read(fd, buf);
                    expect(res.bytesRead > 0).assertTrue();
                    asset1 = dataList[1];
                    fd1 = await asset1.open('r');
                    let buf2 = new ArrayBuffer(4096);
                    let res2 = await fileio.read(fd1, buf2);
                    let write = await fileio.write(fd, buf2);
                    expect(write > 0).assertTrue();

                    await asset.close(fd);
                    await asset1.close(fd1);

                    if (fd > 0 && res.bytesRead > 0 && write > 0) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_07  success');
                        done();
                    } else {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_07  failed');
                        done();
                    }
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_07  error' + error);
            await asset.close(fd);
            await asset1.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_08
     * @tc.name      : open('r')
     * @tc.desc      : open -r the type of IMAGE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_08', 0, async function(done) {
        let asset;
        let fd;
        try {
            let type = mediaLibrary.MediaType.IMAGE;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset.open('r', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_08  fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        expect(fd > 0).assertTrue();
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd, buf);
                        expect(res.bytesRead > 0).assertTrue();
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_08  res.bytesRead:'
                            + res.bytesRead);
                        let write = await fileio.write(fd, buf);
                        await asset.close(fd);
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_08  error:'
                                 + error);
                    if (fd > 0) {
                        expect(true).assertTrue();
                    }
                    await asset.close(fd);
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_08  error:'
                         + error);
            expect(false).assertTrue();
            await asset.close(fd);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09
     * @tc.name      : open('w')
     * @tc.desc      : open -w the type of IMAGE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.IMAGE;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09 dataList.length:'
                         + dataList.length);
            asset = dataList[0];
            asset1 = dataList[1];

            asset.open('w', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09 fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        expect(fd > 0).assertTrue();
                        console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09  fd:'
                                     + fd);
                        fd1 = await asset1.open('r');
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd1, buf);
                        let write = await fileio.write(fd, buf);
                        expect(write > 0).assertTrue();
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09  write:'
                            + write);
                        let buf1 = new ArrayBuffer(4096);
                        let res1 = await fileio.read(fd, buf1);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09 read');
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09  error:'
                                 + error);
                    expect(true).assertTrue();
                    await asset.close(fd);
                    await asset1.close(fd1);
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_09  error:'
                         + error);
            expect(false).assertTrue();
            await asset.close(fd);
            await asset1.close(fd1);
            done();
        }
    });

    //======================== IMAGE END ==================================

    //======================== AUDIO BEGIN ==================================
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_10
     * @tc.name      : open('rw')
     * @tc.desc      : open -rw the type of AUDIO
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_10', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let fileType = mediaLibrary.MediaType.AUDIO;
            let fileFetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ fileType.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fileFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_10 dataList.length:'
                         + dataList.length);
            asset = dataList[0];
            asset.open('rw', async (error, fd) => {
                if (fd == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    expect(fd > 0).assertTrue();
                    let buf = new ArrayBuffer(4096);
                    let res = await fileio.read(fd, buf);
                    expect(res.bytesRead > 0).assertTrue();
                    asset1 = dataList[1];
                    fd1 = await asset1.open('r');
                    let buf2 = new ArrayBuffer(4096);
                    let res2 = await fileio.read(fd1, buf2);
                    let write = await fileio.write(fd, buf2);
                    expect(write > 0).assertTrue();

                    await asset.close(fd);
                    await asset1.close(fd1);

                    if (fd > 0 && res.bytesRead > 0 && write > 0) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_10  success');
                        done();
                    } else {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_10  failed');
                        done();
                    }
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_10  error' + error);
            await asset.close(fd);
            await asset1.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11
     * @tc.name      : open('r')
     * @tc.desc      : open -r the type of AUDIO
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11', 0, async function(done) {
        let asset;
        let fd;
        try {
            let type = mediaLibrary.MediaType.AUDIO;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11 dataList.length:'
                         + dataList.length);
            asset = dataList[0];
            asset.open('r', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11 fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        expect(fd > 0).assertTrue();
                        console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11 fd:'
                                     + fd);
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd, buf);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11 res.bytesRead:'
                            + res.bytesRead);
                        expect(res.bytesRead > 0).assertTrue();
                        let write = await fileio.write(fd, buf);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11 write');
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11  error:'
                                 + error);
                    expect(true).assertTrue();
                    if (fd > 0) {
                        await asset.close(fd);
                    }
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_11  error:'
                         + error);
            expect(false).assertTrue();
            if (fd > 0) {
                await asset.close(fd);
            }
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12
     * @tc.name      : open('w')
     * @tc.desc      : open -w the type of AUDIO
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.AUDIO;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12 dataList.length:'
                         + dataList.length);
            asset = dataList[0];
            asset1 = dataList[1];

            asset.open('w', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12 fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12 fd:'
                                     + fd);
                        expect(fd > 0).assertTrue();
                        fd1 = await asset1.open('r');
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd1, buf);
                        let write = await fileio.write(fd, buf);
                        expect(write > 0).assertTrue();
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12 write:'
                            + write);
                        let buf1 = new ArrayBuffer(4096);
                        let res1 = await fileio.read(fd, buf1);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12 read');
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12  error:'
                                 + error);
                    expect(true).assertTrue();
                    await asset.close(fd);
                    await asset1.close(fd1);
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_12  error:'
                         + error);
            expect(false).assertTrue();
            await asset.close(fd);
            await asset1.close(fd1);
            done();
        }
    });

    //======================== AUDIO END ==================================

    //======================== VIDEO BEGIN ==================================
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13
     * @tc.name      : open('rw')
     * @tc.desc      : open -rw the type of VIDEO
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let fileType = mediaLibrary.MediaType.VIDEO;
            let fileFetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ fileType.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fileFetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            asset.open('rw', async (error, fd) => {
                if (fd == undefined) {
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13 fd == undefined');
                    expect(false).assertTrue();
                    done();
                } else {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13 fd:'
                                 + fd);
                    expect(fd > 0).assertTrue();
                    let buf = new ArrayBuffer(4096);
                    let res = await fileio.read(fd, buf);
                    expect(res.bytesRead > 0).assertTrue();
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13 res.bytesRead :'
                        + res.bytesRead);
                    asset1 = dataList[1];
                    fd1 = await asset1.open('r');
                    let buf2 = new ArrayBuffer(4096);
                    let res2 = await fileio.read(fd1, buf2);
                    let write = await fileio.write(fd, buf2);
                    expect(write > 0).assertTrue();
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13 write :'
                                 + write);

                    if (fd > 0 && res.bytesRead > 0 && write > 0) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13  success');
                        done();
                    } else {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13  failed');
                        done();
                    }
                }
            });

            await asset.close(fd);
            await asset1.close(fd1);
            done();
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_13  error' + error);
            await asset.close(fd);
            await asset1.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14
     * @tc.name      : open('r')
     * @tc.desc      : open -r the type of VIDEO
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14', 0, async function(done) {
        let asset;
        let fd;
        try {
            let type = mediaLibrary.MediaType.VIDEO;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];

            asset.open('r', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14 fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        expect(fd > 0).assertTrue();
                        console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14 fd:'
                                     + fd);
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd, buf);
                        expect(res.bytesRead > 0).assertTrue();
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14 res.bytesRead:'
                            + res.bytesRead);
                        let write = await fileio.write(fd, buf);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14 write:'
                            + write);
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14  error:'
                                 + error);
                    expect(true).assertTrue();
                    await asset.close(fd);
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_14  error:'
                         + error);
            expect(false).assertTrue();
            if (fd > 0) {
                await asset.close(fd);
            }
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15
     * @tc.name      : open('w')
     * @tc.desc      : open -w the type of VIDEO
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15', 0, async function(done) {
        let asset;
        let asset1;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.VIDEO;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15 dataList.length:'
                         + dataList.length);
            asset = dataList[0];
            asset1 = dataList[1];
            asset.open('w', async (error, fd) => {
                try {
                    if (fd == undefined) {
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15 fd == undefined');
                        expect(false).assertTrue();
                        done();
                    } else {
                        console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15 fd :'
                                     + fd);
                        expect(fd > 0).assertTrue();
                        fd1 = await asset1.open('r');
                        let buf = new ArrayBuffer(4096);
                        let res = await fileio.read(fd1, buf);
                        let write = await fileio.write(fd, buf);
                        expect(write > 0).assertTrue();
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15 wreite:'
                            + write);
                        let buf1 = new ArrayBuffer(4096);
                        let res1 = await fileio.read(fd, buf1);
                        console.info(
                            'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15 read');
                        done();
                    }
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15  error:'
                                 + error);
                    expect(true).assertTrue();
                    await asset.close(fd);
                    await asset1.close(fd1);
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_15  error:'
                         + error);
            expect(false).assertTrue();
            await asset.close(fd);
            await asset1.close(fd1);
            done();
        }
    });

    //======================== VIDEO END ==================================

    //======================== CLOSE BEGIN ================================

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_31
     * @tc.name      : close
     * @tc.desc      : asset close the type of file
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_31', 0, async function(done) {
        let asset;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.FILE;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            fd = await asset.open('r');

            asset.close(fd, async (error) => {
                fd1 = await asset.open('r');
                await asset.close(fd1);
                if (fd > 0 && fd1 > 0) {
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_31  success');
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_31  false');
                    await asset.close(fd);
                    await asset.close(fd1);
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_31  error' + error);
            await asset.close(fd);
            await asset.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_33
     * @tc.name      : close
     * @tc.desc      : asset close the type of image
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_33', 0, async function(done) {
        let asset;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.IMAGE;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            fd = await asset.open('r');

            console.info('MediaLibraryTest : ==1 fd:' + fd);
            asset.close(fd, async (error) => {
                fd1 = await asset.open('r');
                console.info('MediaLibraryTest : ==2 fd2:' + fd1);
                await asset.close(fd1);
                if (fd > 0 && fd1 > 0) {
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_33  success');
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_33  false');
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_33  error' + error);
            await asset.close(fd);
            await asset.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_34
     * @tc.name      : close
     * @tc.desc      : asset close the type of audio
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_34', 0, async function(done) {
        let asset;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.AUDIO;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            asset = dataList[0];
            fd = await asset.open('r');

            asset.close(fd, async (error) => {
                fd1 = await asset.open('r');
                await asset.close(fd1);
                if (fd > 0 && fd1 > 0) {
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_34  success');
                    expect(true).assertTrue();
                    done();
                } else {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_34  false');
                    await asset.close(fd);
                    await asset.close(fd1);
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_34  error' + error);
            await asset.close(fd);
            await asset.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35
     * @tc.name      : close
     * @tc.desc      : asset close the type of video
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35', 0, async function(done) {
        let asset;
        let fd;
        let fd1;
        try {
            let type = mediaLibrary.MediaType.VIDEO;
            let fetchOp = {
                selections : fileKeyObj.MEDIA_TYPE + '= ?',
                selectionArgs : [ type.toString() ],
            };
            let fetchFileResult = await media.getFileAssets(fetchOp);
            const dataList = await fetchFileResult.getAllObject();
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35 dataList.length:'
                         + dataList.length);
            asset = dataList[0];
            fd = await asset.open('r');
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35 fd' + fd);
            expect(fd > 0).assertTrue();
            asset.close(fd, async (error) => {
                try {
                    fd1 = await asset.open('r');
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35 fd1'
                                 + fd1);
                    expect(fd1 > 0).assertTrue();
                    expect(error == undefined).assertTrue();
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35 error == undefined');
                    await asset.close(fd1);
                    console.info(
                        'MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35 close fd1:'
                        + fd1);
                    done();
                } catch (error) {
                    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35  error'
                                 + error);
                    await asset.close(fd);
                    await asset.close(fd1);
                    expect(false).assertTrue();
                    done();
                }
            });
        } catch (error) {
            console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_OPENANDCLOSE_ASSET_CALLBACK_005_35  error' + error);
            await asset.close(fd);
            await asset.close(fd1);
            expect(false).assertTrue();
            done();
        }
    });

    //======================== CLOSE BEGIN ================================
});