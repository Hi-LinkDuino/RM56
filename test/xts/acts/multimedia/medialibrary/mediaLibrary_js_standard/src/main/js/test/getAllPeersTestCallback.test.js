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

describe('getAllPeersTestCallback.test.js', function() {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    beforeAll(function() {});
    beforeEach(function() {});
    afterEach(function() {});
    afterAll(function() {});

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_01
     * @tc.name      : getAllPeers
     * @tc.desc      : getAllPeers device
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_01', 0, async function(done) {
        try {
            done();
            media.getAllPeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_01 peer.deviceName = '
                                 + peer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_01 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_02
     * @tc.name      : getAllPeers
     * @tc.desc      : get the first peers by getAllPeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_02', 0, async function(done) {
        try {
            done();
            media.getAllPeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    peer = allPeerList[0];
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_02 peer.deviceName = '
                                 + peer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_02 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_03
     * @tc.name      : getAllPeers
     * @tc.desc      : get the second peers by getAllPeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_03', 0, async function(done) {
        try {
            done();
            media.getAllPeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    peer = allPeerList[1];
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_03 peer.deviceName = '
                                 + peer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_03 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_04
     * @tc.name      : getAllPeers
     * @tc.desc      : get the third peers by getAllPeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_04', 0, async function(done) {
        try {
            done();
            media.getAllPeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    peer = allPeerList[2];
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_04 peer.deviceName = '
                                 + peer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_04 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_04
     * @tc.name      : getAllPeers
     * @tc.desc      : get the forth peers by getAllPeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_05', 0, async function(done) {
        try {
            done();
            media.getAllPeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    peer = allPeerList[3];
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_05 peer.deviceName = '
                                 + peer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_05 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_04
     * @tc.name      : getAllPeers
     * @tc.desc      : get the fifth peers by getAllPeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_CALLBACK_001_06', 0, async function(done) {
        try {
            done();
            media.getAllPeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    peer = allPeerList[4];
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_06 peer.deviceName = '
                                 + peer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_06 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_01
     * @tc.name      : getActivePeers
     * @tc.desc      : get the first peers by getActivePeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_01', 0, async function(done) {
        try {
            done();
            media.getActivePeers((error, activePeerList) => {
                if (activePeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    activePeer = activePeerList[0];
                    console.info(
                        'MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_01 activePeer.deviceName = '
                        + activePeer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_01 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_02
     * @tc.name      : getActivePeers
     * @tc.desc      : get the second peers by getActivePeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_02', 0, async function(done) {
        try {
            done();
            media.getActivePeers((error, activePeerList) => {
                if (activePeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    activePeer = activePeerList[1];
                    console.info(
                        'MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_02 activePeer.deviceName = '
                        + activePeer.deviceName);
                    done();
                }
            });
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_02 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_03
     * @tc.name      : getActivePeers
     * @tc.desc      : get the third peers by getActivePeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_03', 0, async function(done) {
        try {
            done();
            let activePeerList = await media.getActivePeers();
            activePeer = activePeerList[2];
            console.info('MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_03 activePeer.deviceName = '
                         + activePeer.deviceName);
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_03 fail, message = ' + error);
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_04
     * @tc.name      : getActivePeers
     * @tc.desc      : get the forth peers by getActivePeers
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_CALLBACK_001_04', 0, async function(done) {
        try {
            done();
            let activePeerList = await media.getActivePeers();
            activePeer = activePeerList[3];
            console.info('MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_04 activePeer.deviceName = '
                         + activePeer.deviceName);
            done();
        } catch (error) {
            console.info('MediaLibraryTest : ACTIVE_PEER_CALLBACK getActivePeers 001_04 fail, message = ' + error);
            done();
        }
    });
});