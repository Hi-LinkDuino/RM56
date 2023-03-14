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
const fileKeyObj = mediaLibrary.FileKey;
const fileType = mediaLibrary.MediaType.FILE;
const imageType = mediaLibrary.MediaType.IMAGE;
const videoType = mediaLibrary.MediaType.VIDEO;
const audioType = mediaLibrary.MediaType.AUDIO;
const fileFetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [fileType.toString()],
};
const imageFetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [imageType.toString()],
};
const videoFetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [videoType.toString()],
};
const audioFetchOp = {
    selections: fileKeyObj.MEDIA_TYPE + '= ?',
    selectionArgs: [audioType.toString()],
};

describe('favoriteTestCallBack.test.js', function () {
    var context = featureAbility.getContext();
    console.info('getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('getMediaLibrary OUT');
    beforeAll(function () {});
    beforeEach(function () {});
    afterEach(function () {});
    afterAll(function () {});

    // ------------------------------ file type start ------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_01
     * @tc.name      : favorite
     * @tc.desc      : favorite(file) by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_01', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 001_01 fail0');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(true, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 001_01 success1');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 001_01 fail1');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 001_01 success2');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 001_01 fail2');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_01 fail3, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_02
     * @tc.name      : favorite
     * @tc.desc      : favorite(file) by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_02', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (!isFavorite) {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 001_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(false, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (!isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 001_02 pass');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 001_02 fail');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 001_02 pass');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 001_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_02 fail, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_03
     * @tc.name      : favorite
     * @tc.desc      : favorite(file) by 'true'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_03', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('true', () => {
                            console.info('FAV_ASSET_CALLBACK 001_03 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 001_03 pass');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_03 pass');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_04
     * @tc.name      : favorite
     * @tc.desc      : favorite(file) by 'false'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_04', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('false', () => {
                            console.info('FAV_ASSET_CALLBACK 001_04 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 001_04 pass');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_04 pass');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_05
     * @tc.name      : favorite
     * @tc.desc      : favorite(file) by 'fav'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_05', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('fav', () => {
                            console.info('FAV_ASSET_CALLBACK 001_05 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 001_05 pass');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_05 pass');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_06
     * @tc.name      : favorite
     * @tc.desc      : favorite(file) by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_06', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(666, () => {
                            console.info('FAV_ASSET_CALLBACK 001_06 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 001_06 pass');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_06 pass');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_07
     * @tc.name      : favorite
     * @tc.desc      : favorite(file) by no arg
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_07', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(() => {
                            console.info('FAV_ASSET_CALLBACK 001_07 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 001_07 pass');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_07 pass');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_08
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(file) result false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_08', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            console.info('FAV_ASSET_CALLBACK 001_08 fail');
                            expect(false).assertTrue();
                            done();
                        } else {
                            console.info('FAV_ASSET_CALLBACK 001_08 pass');
                            expect(true).assertTrue();
                            done();
                        }
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_08 fail, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_09
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(file) result true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_001_09', 0, async function (done) {
        try {
            media.getFileAssets(fileFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.favorite(true, () => {
                        asset.isFavorite((err1, isFavorite) => {
                            if (isFavorite) {
                                console.info('FAV_ASSET_CALLBACK 001_09 pass');
                                expect(true).assertTrue();
                                done();
                            } else {
                                console.info('FAV_ASSET_CALLBACK 001_09 fail');
                                expect(false).assertTrue();
                                done();
                            }
                        });
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 001_09 fail, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ file type end ------------------------

    // ------------------------------ image type start ------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_01
     * @tc.name      : favorite
     * @tc.desc      : favorite(image) by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_01', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 002_01 failed');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(true, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 002_01 success');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 002_01 fail');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 002_01 success');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 002_01 fail');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                        
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_02
     * @tc.name      : favorite
     * @tc.desc      : favorite(image) by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_02', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (!isFavorite) {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 002_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(false, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (!isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 002_02 pass');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 002_02 fail');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 002_02 pass');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 002_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_03
     * @tc.name      : favorite
     * @tc.desc      : favorite(image) by 'true'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_03', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('true', () => {
                            console.info('FAV_ASSET_CALLBACK 002_03 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 002_03 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_03 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_04
     * @tc.name      : favorite
     * @tc.desc      : favorite(image) by 'false'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_04', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('false', () => {
                            console.info('FAV_ASSET_CALLBACK 002_04 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 002_04 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_04 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_05
     * @tc.name      : favorite
     * @tc.desc      : favorite(image) by 'fav'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_05', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('fav', () => {
                            console.info('FAV_ASSET_CALLBACK 002_05 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 002_05 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_05 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_06
     * @tc.name      : favorite
     * @tc.desc      : favorite(image) by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_06', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(666, () => {
                            console.info('FAV_ASSET_CALLBACK 002_06 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 002_06 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_06 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_07
     * @tc.name      : favorite
     * @tc.desc      : favorite(image) by no arg
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_07', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(() => {
                            console.info('FAV_ASSET_CALLBACK 002_07 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 002_07 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_07 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_08
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(image) result false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_08', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            console.info('FAV_ASSET_CALLBACK 002_08 fail');
                            expect(false).assertTrue();
                            done();
                        } else {
                            console.info('FAV_ASSET_CALLBACK 002_08 pass');
                            expect(true).assertTrue();
                            done();
                        }
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_08 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_09
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(image) result true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_002_09', 0, async function (done) {
        try {
            media.getFileAssets(imageFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.favorite(true, () => {
                        asset.isFavorite((err1, isFavorite) => {
                            if (isFavorite) {
                                console.info('FAV_ASSET_CALLBACK 002_09 pass');
                                expect(true).assertTrue();
                                done();
                            } else {
                                console.info('FAV_ASSET_CALLBACK 002_09 fail');
                                expect(false).assertTrue();
                                done();
                            }
                        });
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 002_09 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ image type end ------------------------

    // ------------------------------ video type start ------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_01
     * @tc.name      : favorite
     * @tc.desc      : favorite(video) by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_01', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 003_01 fail');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(true, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 003_01 success');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 003_01 fail');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 003_01 success');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 003_01 fail');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                        
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_02
     * @tc.name      : favorite
     * @tc.desc      : favorite(video) by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_02', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (!isFavorite) {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 003_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(false, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (!isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 003_02 pass');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 003_02 fail');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 003_02 pass');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 003_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                        
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_03
     * @tc.name      : favorite
     * @tc.desc      : favorite(video) by 'true'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_03', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('true', () => {
                            console.info('FAV_ASSET_CALLBACK 003_03 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 003_03 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_03 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_04
     * @tc.name      : favorite
     * @tc.desc      : favorite(video) by 'false'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_04', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('false', () => {
                            console.info('FAV_ASSET_CALLBACK 003_04 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 003_04 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_04 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_05
     * @tc.name      : favorite
     * @tc.desc      : favorite(video) by 'fav'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_05', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('fav', () => {
                            console.info('FAV_ASSET_CALLBACK 003_05 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 003_05 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_05 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_06
     * @tc.name      : favorite
     * @tc.desc      : favorite(video) by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_06', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(666, () => {
                            console.info('FAV_ASSET_CALLBACK 003_06 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 003_06 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_06 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_07
     * @tc.name      : favorite
     * @tc.desc      : favorite(video) by no arg
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_07', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(() => {
                            console.info('FAV_ASSET_CALLBACK 003_07 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 003_07 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_07 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_08
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(video) result false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_08', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            console.info('FAV_ASSET_CALLBACK 003_08 fail');
                            expect(false).assertTrue();
                            done();
                        } else {
                            console.info('FAV_ASSET_CALLBACK 003_08 pass');
                            expect(true).assertTrue();
                            done();
                        }
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_08 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_09
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(video) result true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_003_09', 0, async function (done) {
        try {
            media.getFileAssets(videoFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.favorite(true, () => {
                        asset.isFavorite((err1, isFavorite) => {
                            if (isFavorite) {
                                console.info('FAV_ASSET_CALLBACK 003_09 pass');
                                expect(true).assertTrue();
                                done();
                            } else {
                                console.info('FAV_ASSET_CALLBACK 003_09 fail');
                                expect(false).assertTrue();
                                done();
                            }
                        });
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 003_09 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ video type end ------------------------

    // ------------------------------ audio type start ------------------------
    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_01
     * @tc.name      : favorite
     * @tc.desc      : favorite(audio) by true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_01', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 004_01 fail');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(true, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 004_01 success');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 004_01 fail');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 004_01 success');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 004_01 fail');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                        
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_01 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_02
     * @tc.name      : favorite
     * @tc.desc      : favorite(audio) by false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_02', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (!isFavorite) {
                            asset.favorite(true, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 004_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    } else {
                                        asset.favorite(false, () => {
                                            asset.isFavorite((err1, isFavorite) => {
                                                if (!isFavorite) {
                                                    console.info('FAV_ASSET_CALLBACK 004_02 pass');
                                                    expect(true).assertTrue();
                                                    done();
                                                } else {
                                                    console.info('FAV_ASSET_CALLBACK 004_02 fail');
                                                    expect(false).assertTrue();
                                                    done();
                                                }
                                            });
                                        });
                                    }
                                });
                            });
                        } else {
                            asset.favorite(false, () => {
                                asset.isFavorite((err1, isFavorite) => {
                                    if (!isFavorite) {
                                        console.info('FAV_ASSET_CALLBACK 004_02 pass');
                                        expect(true).assertTrue();
                                        done();
                                    } else {
                                        console.info('FAV_ASSET_CALLBACK 004_02 fail');
                                        expect(false).assertTrue();
                                        done();
                                    }
                                });
                            });
                        }
                        
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_02 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_03
     * @tc.name      : favorite
     * @tc.desc      : favorite(audio) by 'true'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_03', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('true', () => {
                            console.info('FAV_ASSET_CALLBACK 004_03 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 004_03 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_03 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_04
     * @tc.name      : favorite
     * @tc.desc      : favorite(audio) by 'false'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_04', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('false', () => {
                            console.info('FAV_ASSET_CALLBACK 004_04 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 004_04 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_04 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_05
     * @tc.name      : favorite
     * @tc.desc      : favorite(audio) by 'fav'
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_05', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite('fav', () => {
                            console.info('FAV_ASSET_CALLBACK 004_05 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 004_05 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_05 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_06
     * @tc.name      : favorite
     * @tc.desc      : favorite(audio) by 666
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_06', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(666, () => {
                            console.info('FAV_ASSET_CALLBACK 004_06 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 004_06 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_06 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_07
     * @tc.name      : favorite
     * @tc.desc      : favorite(audio) by no arg
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_07', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    try {
                        asset.favorite(() => {
                            console.info('FAV_ASSET_CALLBACK 004_07 fail');
                            expect(false).assertTrue();
                            done();
                        });
                    } catch (error) {
                        console.info('FAV_ASSET_CALLBACK 004_07 passed');
                        expect(true).assertTrue();
                        done();
                    }
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_07 passed');
            expect(true).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_08
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(audio) result false
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_08', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.isFavorite((err1, isFavorite) => {
                        if (isFavorite) {
                            console.info('FAV_ASSET_CALLBACK 004_08 fail');
                            expect(false).assertTrue();
                            done();
                        } else {
                            console.info('FAV_ASSET_CALLBACK 004_08 pass');
                            expect(true).assertTrue();
                            done();
                        }
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_08 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_09
     * @tc.name      : isFavorite
     * @tc.desc      : isFavorite(audio) result true
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_FAV_ASSET_CALLBACK_004_09', 0, async function (done) {
        try {
            media.getFileAssets(audioFetchOp, (error, fileAssets) => {
                fileAssets.getAllObject((error, dataList) => {
                    const asset = dataList[0];
                    asset.favorite(true, () => {
                        asset.isFavorite((err1, isFavorite) => {
                            if (isFavorite) {
                                console.info('FAV_ASSET_CALLBACK 004_09 pass');
                                expect(true).assertTrue();
                                done();
                            } else {
                                console.info('FAV_ASSET_CALLBACK 004_09 fail');
                                expect(false).assertTrue();
                                done();
                            }
                        });
                    });
                });
            });
        } catch (error) {
            console.info('FAV_ASSET_CALLBACK 004_09 failed, message = ' + error);
            expect(false).assertTrue();
            done();
        }
    });
    // ------------------------------ audio type end ------------------------
});
