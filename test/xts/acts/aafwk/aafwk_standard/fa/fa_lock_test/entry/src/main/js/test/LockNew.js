/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index";
import missionManager  from '@ohos.application.missionManager';
import abilityFeatureAbility from '@ohos.ability.featureAbility';
import commonEvent from '@ohos.commonEvent';

var START_ABILITY_TIMEOUT = 5000;

describe('LockTest', function () {
    console.log("LockTest --- start");
    var missionArray = [];
    function sleep(numberMillis) {
        var now = new Date();
        var exitTime = now.getTime() + numberMillis;
        while (true) {
            now = new Date();
            if (now.getTime() > exitTime)
            return;
        }
    }

    /**
     * @tc.number: SUB_AA_OpenHarmony_Lock_0400
     * @tc.name: wantConstant : Check specific enum
     * @tc.desc: Check the return type of the interface (by Promise)
     */
    it('SUB_AA_OpenHarmony_Lock_0700', 0, async function (done) {
        console.info('=======SUB_AA_OpenHarmony_Lock_0700 step1 start=====startActivity=====');
        abilityFeatureAbility.startAbility({ want: {
            bundleName: "com.example.lock",
            abilityName: "com.example.lock.Ability1"
        } })
            .then((data) => {
                console.info('SUB_AA_OpenHarmony_Lock_0300 startAbility1: ' +
                JSON.stringify(data));
                console.info('SUB_AA_OpenHarmony_Lock_0300 - startAbility2 start: ')
                abilityFeatureAbility.startAbility({ "want": {
                    bundleName: "com.example.lock",
                    abilityName: "com.example.lock.Ability2"
                } }, (error, data) => {
                    console.log('SUB_AA_OpenHarmony_Lock_0300 - startAbility2: ' +
                    JSON.stringify(error) + ", " + JSON.stringify(data))
                    console.info('SUB_AA_OpenHarmony_Lock_0300 - startAbility3 start');
                    abilityFeatureAbility.startAbility({ "want": {
                        bundleName: "com.example.lock",
                        abilityName: "com.example.lock.Ability3"
                    } }, (error, data) => {
                        console.log('SUB_AA_OpenHarmony_Lock_0300 - startAbility3: ' +
                        JSON.stringify(error) + ", " + JSON.stringify(data))
                        console.info('SUB_AA_OpenHarmony_Lock_0300 - startAbility4 start');
                        abilityFeatureAbility.startAbility({ "want": {
                            bundleName: "com.example.lock",
                            abilityName: "com.example.lock.Ability4"
                        } }, (error, data) => {
                            console.log('SUB_AA_OpenHarmony_Lock_0300 - startAbility4: ' +
                            JSON.stringify(error) + ", " + JSON.stringify(data))
                            getMissionInfosOne();
                        });
                    });
                });
            }).catch((error) => {
            console.error('SUB_AA_OpenHarmony_Lock_0700 startAbility1 error ' +
            JSON.stringify(error));
        })
        setTimeout(() => {
            done();
        }, 5000);
    })

    function getMissionInfosOne() {
        console.log('=======SUB_AA_OpenHarmony_Lock_0700 step2 start==========getMissionInfos=====');
        missionArray = [];
        missionManager.getMissionInfos('', 500, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_0700 getMissionInfo err: '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            var i;
            for (i = 0; i < data.length; i++) {
                console.log("getMissionInfos return1 " + JSON.stringify(data[i]));
                if (data[i].want.abilityName == 'com.example.lock.Ability1' &&
                data[i].runningState == 0) {
                    missionArray[0] = new Object();
                    missionArray[0].id = data[i].missionId;
                    missionArray[0].name = data[i].want.abilityName;
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability2' &&
                data[i].runningState == 0) {
                    missionArray[1] = new Object();
                    missionArray[1].id = data[i].missionId;
                    missionArray[1].name = data[i].want.abilityName;
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability3' &&
                data[i].runningState == 0) {
                    missionArray[2] = new Object();
                    missionArray[2].id = data[i].missionId;
                    missionArray[2].name = data[i].want.abilityName;
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability4' &&
                data[i].runningState == 0) {
                    missionArray[3] = new Object();
                    missionArray[3].id = data[i].missionId;
                    missionArray[3].name = data[i].want.abilityName;
                }
                if (data[i].want.abilityName == 'com.example.lock.MainAbility' &&
                data[i].runningState == 0) {
                    missionArray[4] = new Object();
                    missionArray[4].id = data[i].missionId;
                    missionArray[4].name = data[i].want.abilityName;
                }
            }
            console.log("###" + JSON.stringify(missionArray));
            expect(missionArray.length == 5).assertTrue();
            lockMissionMy();
        })
    }

    function lockMissionMy() {
        console.log('===============SUB_AA_OpenHarmony_Lock_0700 step3 start=====lockMission======');
        console.log('SUB_AA_OpenHarmony_Lock_0700 missionArray' + JSON.stringify(missionArray));
        var i;
        var j = 0;
        for (i = 0; i < missionArray.length; i++) {
            if (missionArray[i].name == 'com.example.lock.Ability1' || missionArray[i].name ==
            'com.example.lock.Ability4'|| missionArray[i].name == 'com.example.lock.MainAbility') {
                console.log('SUB_AA_OpenHarmony_Lock_0700 j===' + j);
                console.log('SUB_AA_OpenHarmony_Lock_0700 ' + JSON.stringify(missionArray[i].id));
                missionManager.lockMission(missionArray[i].id, (err, data) => {
                    console.log('SUB_AA_OpenHarmony_Lock_0700 lockMission err:'
                    + JSON.stringify(err) + " data: " + JSON.stringify(data));
                    expect(data == undefined).assertTrue();
                    j++;
                    if (j == 3) {
                        getMissionInfosTest();
                        //clearAllMission();
                    }
                })
            }
        }
    }

    function unlockMissionTest() {
        console.log('===============SUB_AA_OpenHarmony_Lock_0700 step6 start=====unLockMission======');
        console.log("unlockMissionMy start " + JSON.stringify(missionArray));
        missionManager.unlockMission(missionArray[1].id, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_0700 unlockMission err:'
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(data == undefined).assertTrue();
        });
    }

    function getMissionInfosTest(){
        console.log('==========SUB_AA_OpenHarmony_Lock_0700 step5 start=====getMissionInfosTest======');
        missionArray = []
        missionManager.getMissionInfos('', 500, (err, data) => {
            console.log('getMissionInfosTwo getMissionInfos4 err: '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            var i;
            for (i = 0; i < data.length; i++) {
                console.log("getMissionInfos return1 " + JSON.stringify(data[i]));
                if (data[i].want.abilityName == 'com.example.lock.Ability1' &&
                data[i].runningState == 0) {
                    expect(data[i].lockedState == true).assertTrue();
                    expect(data[i].timestamp != 0).assertTrue();
                    expect(data[i].label).assertEquals("$string:entry_MainAbility");
                    missionArray[0] = new Object();
                    missionArray[0].id = data[i].missionId;
                    missionArray[0].name = data[i].want.abilityName;
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability4' &&
                data[i].runningState == 0) {
                    expect(data[i].lockedState == true).assertTrue();
                    expect(data[i].timestamp != 0).assertTrue();
                    expect(data[i].label).assertEquals("$string:entry_MainAbility");
                    missionArray[1] = new Object();
                    missionArray[1].id = data[i].missionId;
                    missionArray[1].name = data[i].want.abilityName;
                }
            }
            console.log("###" + JSON.stringify(missionArray));
            console.log("###" + JSON.stringify(missionArray.length));
            expect(missionArray.length == 2).assertTrue();
            unlockMissionTest();
        })
    }

    function clearAllMission(done) {
        console.log('==========SUB_AA_OpenHarmony_Lock_0700 step4 start=====clearAllMissions======');
        missionManager.clearAllMissions((err, data) => {
            console.log('getMissionInfosTwo clearAllMission err: '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            getMissionInfosTwo();
        });
    }

    function getMissionInfosTwo(done) {
        console.log('==========SUB_AA_OpenHarmony_Lock_0700 step5 start=====getMissionInfos======');
        missionArray = []
        missionManager.getMissionInfos('', 500, (err, data) => {
            console.log('getMissionInfosTwo getMissionInfo err: '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            var i;
            for (i = 0; i < data.length; i++) {
                console.log("getMissionInfos return1 " + JSON.stringify(data[i]));
                if (data[i].want.abilityName == 'com.example.lock.Ability1' &&
                data[i].runningState == 0) {
                    expect(data[i].lockedState == true).assertTrue();
                    missionArray[0] = new Object();
                    missionArray[0].id = data[i].missionId;
                    missionArray[0].name = data[i].want.abilityName;
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability2') {
                    expect().assertFail();
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability3' &&
                data[i].runningState == 0) {
                    expect().assertFail();
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability4' &&
                data[i].runningState == 0) {
                    expect(data[i].lockedState == true).assertTrue();
                    missionArray[1] = new Object();
                    missionArray[1].id = data[i].missionId;
                    missionArray[1].name = data[i].want.abilityName;
                }
            }
            console.log("###" + JSON.stringify(missionArray));
            console.log("###" + JSON.stringify(missionArray.length));
            done();
            expect(missionArray.length == 2).assertTrue();
            //unlockMissionMy();
        })
    }

    function unlockMissionMy() {
        console.log('===========SUB_AA_OpenHarmony_Lock_0700 step6 start=====unLockMission======');
        console.log("unlockMissionMy start " + JSON.stringify(missionArray));
        missionManager.unlockMission(missionArray[1].id, (err, data) => {
            console.log('SUB_AA_OpenHarmony_Lock_0700 unlockMission err:'
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(data == undefined).assertTrue();
            clearAllMission2();
        });
    }

    function clearAllMission2(done) {
        console.log('===========SUB_AA_OpenHarmony_Lock_0700 step7 start=====clearAllMissions======');
        var myEvents = {
            events:
            [
                "Ability1_onShow",
                "Ability4_onDestroy"
            ]
        };
        var Subscriber;
        let id;
        var data = [];

        function SubscribeCallBack(err, data) {
            console.debug("beforeAll====>Subscribe CallBack data:====>" + JSON.stringify(data));
            switch (data.event) {
                case "Ability1_onShow":
                    console.log("Ability1_onShow");
                    data[0] = "Ability1_onCreate";
                    break;
                case "Ability4_onDestroy":
                    console.log("Ability4_onDestroy");
                    data[1] = "Ability2_onCreate";
                    break;
                default:
            }
            if (data.length == 2) {
                clearTimeout(id);
                commonEvent.unsubscribe(Subscriber, UnSubscribeCallback);

            }
        };
        commonEvent.createSubscriber(myEvents).then(async (data) => {
            console.debug("SUB_AA_OpenHarmony_Lock_0400====>Create Subscriber====>");
            Subscriber = data;
            console.log("SUB_AA_OpenHarmony_Lock_0700 Subscriber " + JSON.stringify(Subscriber));
            await commonEvent.subscribe(Subscriber, SubscribeCallBack);
        }).catch((error) => {
            console.error('Operation failed. Cause: ' + JSON.stringify(error));
            done();
        });

        function UnSubscribeCallback() {
            console.debug("====>UnSubscribe CallBack====>");
        }

        function timeout() {
            expect().assertFail();
            console.debug('SUB_AA_OpenHarmony_Lock_0700=====timeout======');
            commonEvent.unsubscribe(Subscriber, UnSubscribeCallback);
            done();
        }

        id = setTimeout(timeout, START_ABILITY_TIMEOUT);

        missionManager.clearAllMissions();
        sleep(500);
        getMissionInfosThree();
    }

    function getMissionInfosThree(done) {
        console.log('===========SUB_AA_OpenHarmony_Lock_0700 step8 start=====getMissonInfos======');
        missionArray = [];
        missionManager.getMissionInfos('', 500, (err, data) => {
            console.log('getMissionInfosThree getMissionInfo err: '
            + JSON.stringify(err) + " data: " + JSON.stringify(data));
            expect(err.code == 0).assertTrue();
            var i;
            for (i = 0; i < data.length; i++) {
                console.log("getMissionInfos return1 " + JSON.stringify(data[i]));
                if (data[i].want.abilityName == 'com.example.lock.Ability1' &&
                data[i].runningState == 0) {
                    expect(data[i].lockedState == true).assertTrue();
                    missionArray[0] = new Object();
                    missionArray[0].id = data[i].missionId;
                    missionArray[0].name = data[i].want.abilityName;
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability2') {
                    expect().assertFail();
                    done();
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability3' &&
                data[i].runningState == 0) {
                    expect().assertFail();
                    done();
                }
                if (data[i].want.abilityName == 'com.example.lock.Ability4' &&
                data[i].runningState == 0) {
                    expect().assertFail();
                    done();
                }
            }
            console.log("###" + JSON.stringify(missionArray));
            expect(missionArray.length == 1).assertTrue();
            console.log('===============SUB_AA_OpenHarmony_Lock_0700 step9 start===========');
            console.info('LockTest TearDown: ' + JSON.stringify(missionArray));
            var i;
            for (i = 0; i < missionArray.length; i++) {
                missionManager.unlockMission(missionArray[i].id, (err, data) => {
                    console.log('SUB_AA_OpenHarmony_Lock_0700 unlockMission err:'
                    + JSON.stringify(err) + " data: " + JSON.stringify(data));
                    expect(data == undefined).assertTrue();
                    missionManager.clearAllMissions();
                    if (i == missionArray.length) {
                        console.info('LockTest - myTearDown: done');
                        missionManager.clearAllMissions();
                        done();
                    }
                    console.info('LockTest - myTearDown%%: i' + i);
                    console.info('LockTest - myTearDown%%: length' + missionArray.length);
                });
            }
        })
    }
}
)
