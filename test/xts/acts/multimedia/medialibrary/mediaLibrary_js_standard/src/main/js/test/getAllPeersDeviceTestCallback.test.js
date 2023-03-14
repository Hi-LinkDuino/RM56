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

describe('getAllPeersDeviceTestCallback.test.js', function() {
    var context = featureAbility.getContext();
    console.info('MediaLibraryTest : getMediaLibrary IN');
    var media = mediaLibrary.getMediaLibrary(context);
    console.info('MediaLibraryTest : getMediaLibrary OUT');
    beforeAll(function() {});
    beforeEach(function() {});
    afterEach(function() {});
    afterAll(function() {});

    console.info('MediaLibraryTest : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02 begin');

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer device information TYPE_UNKNOWN
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02', 0, async function(done) {
        done();
        media.getAllPeers((error, allPeerList) => {
            if (allPeerList == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                expect(allPeerList[1].DeviceType.TYPE_UNKNOWN).assertEqual(0);
                done();
            }
        });
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer device information TYPE_LAPTOP
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_01', 0, async function(done) {
        done();
        media.getAllPeers((error, allPeerList) => {
            if (allPeerList == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                expect(allPeerList[1].DeviceType.TYPE_LAPTOP).assertEqual(1);
                done();
            }
        });
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_02
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer device information TYPE_PHONE
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_02', 0, async function(done) {
        done();
        media.getAllPeers((error, allPeerList) => {
            if (allPeerList == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                expect(allPeerList[1].DeviceType.TYPE_PHONE).assertEqual(2);
                done();
            }
        });
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_03
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer device information TYPE_TABLET
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_03', 0, async function(done) {
        done();
        media.getAllPeers((error, allPeerList) => {
            if (allPeerList == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                expect(allPeerList[1].DeviceType.TYPE_TABLET).assertEqual(3);
                done();
            }
        });
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_04
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer device information TYPE_WATCH
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_04', 0, async function(done) {
        done();
        media.getAllPeers((error, allPeerList) => {
            if (allPeerList == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                expect(allPeerList[1].DeviceType.TYPE_WATCH).assertEqual(4);
                done();
            }
        });
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_05
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer device information TYPE_CAR
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_05', 0, async function(done) {
        done();
        media.getAllPeers((error, allPeerList) => {
            if (allPeerList == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                expect(allPeerList[1].DeviceType.TYPE_CAR).assertEqual(5);
                done();
            }
        });
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_06
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer device information TYPE_TV
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_CALLBACK_001_02_06', 0, async function(done) {
        done();
        media.getAllPeers((error, allPeerList) => {
            if (allPeerList == undefined) {
                expect(false).assertTrue();
                done();
            } else {
                expect(allPeerList[1].DeviceType.TYPE_TV).assertEqual(6);
                done();
            }
        });
        done();
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_PROMISE_001_02_06
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer networkId
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_NETWORKID_CALLBACK_001_02', 0, async function(done) {
        try {
            done();
            media.getActivePeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    networkId = allPeerList[0].networkId;
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_01 peer.networkId = '
                                 + networkId);
                    done();
                }
            });
            done();
        } catch (error) {
            done();
        }
    });

    /**
     * @tc.number    : SUB_MEDIA_MEDIALIBRARY_ALL_PEER_DEVICETYPE_PROMISE_001_02_06
     * @tc.name      : getActivePeers
     * @tc.desc      : Get Active Peer isOnline
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_MEDIA_MEDIALIBRARY_ACTIVE_PEER_NETWORKID_ISONLINE_CALLBACK_001_02', 0, async function(done) {
        try {
            done();
            media.getActivePeers((error, allPeerList) => {
                if (allPeerList == undefined) {
                    expect(false).assertTrue();
                    done();
                } else {
                    isOnline = allPeerList[0].isOnline;
                    console.info('MediaLibraryTest : ALL_PEER_CALLBACK getAllPeers 001_01 peer.isOnline = ' + isOnline);
                    done();
                }
            });
            done();
        } catch (error) {
            done();
        }
    });
});