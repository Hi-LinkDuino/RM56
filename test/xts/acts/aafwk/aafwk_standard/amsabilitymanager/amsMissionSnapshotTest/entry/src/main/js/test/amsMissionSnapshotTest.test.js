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
import featureAbility from '@ohos.ability.featureAbility'
import missionManager from '@ohos.application.missionManager'
import commonEvent from '@ohos.commonEvent'
import image from '@ohos.multimedia.image'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe('ActsMissionSnapshotTest', function () {
    beforeAll(async (done) => {
        console.debug('= ACTS_beforeAll ====<begin');
        done();
    })
    afterAll(async (done) => {
        console.debug('= ACTS_afterAll ====<begin');
        done();
    })

    /*
    * @tc.number: ACTS_MissionSnapshot_0100
    * @tc.name: getMissionSnapshot
    * @tc.desc: Recent task stack provides current screenshot.(AsyncCallback)
    */
    it('ACTS_MissionSnapshot_0100', 0, async function (done) {
        console.debug('ACTS_MissionSnapshot_0100====<begin');
        try {
            await missionManager.getMissionSnapShot("", -1, (err, data) => {
                expect(err.code != 0).assertTrue();
                console.debug("=ACTS_MissionSnapshot_0100 err.code,data=======>"
                + err.code + ("json data【") + JSON.stringify(data) + (" 】") + " , " + data);
                console.debug('ACTS_MissionSnapshot_0100====<end')
                done();
            });
        } catch (err) {
            console.debug('ACTS_MissionSnapshot_0100====<end err' + err);
            console.debug('ACTS_MissionSnapshot_0100====<end err');
            done();
        }
    })

    /*
     * @tc.number: ACTS_MissionSnapshot_0200
     * @tc.name: getMissionSnapshot
     * @tc.desc: Recent task stack provides current screenshot.(AsyncCallback)
     */
    it('ACTS_MissionSnapshot_0200', 0, async function (done) {
        console.debug('ACTS_MissionSnapshot_0200====<begin');
        var m_missionId = -1;
        var m_bundleName = "";
        var m_abilityName = "";
        try {
            var upperLimit = 10;
            await missionManager.getMissionInfos("", upperLimit, async (err, data) => {
                console.debug("=ACTS_MissionSnapshot_0200 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】")
                    + ("json data【") + JSON.stringify(data) + (" 】")
                    + " ,err=" + err + " ,data=" + data);
                console.debug('ACTS_MissionSnapshot_0200 MissionInfo data.length ====>: ' + data.length);
                if (!data.length) {
                    m_missionId = -99;
                    m_bundleName = "";
                    m_abilityName = "";
                    expect("if (!data.length) ").assertEqual("0");
                    console.debug('ACTS_MissionSnapshot_0200====<end 0')
                    done();
                } else {
                    m_missionId = data[0].missionId;
                    m_bundleName = data[0].want.bundleName;
                    m_abilityName = data[0].want.abilityName;
                }
                console.debug("=ACTS_MissionSnapshot_0200 m_missionId====>" + m_missionId);
                await missionManager.getMissionSnapShot("", m_missionId, async (err, data) => {
                    expect(err.code).assertEqual(0);
                    console.debug("=ACTS_MissionSnapshot_0200 err,data=======>"
                        + ("json data【") + JSON.stringify(data) + (" 】") + " , " + data);
                    expect(data.ability.bundleName).assertEqual(m_bundleName);
                    expect(data.ability.abilityName).assertEqual(m_abilityName);
                    console.debug("=ACTS_MissionSnapshot_0200 ====> JSON.stringify(data.snapshot)="
                        + JSON.stringify(data.snapshot) + " , " + data.snapshot);
                    done();
                });
            });
        } catch (err) {
            expect(err).assertEqual("0");
            console.debug('ACTS_MissionSnapshot_0200====<end err' + err)
            done();
        }
    })

 /*
    * @tc.number: ACTS_MissionSnapshot_0300
    * @tc.name: getMissionSnapshot 
    * @tc.desc: Recent task stack provides current screenshot.(AsyncCallback)
    */
 it('ACTS_MissionSnapshot_0300', 0, async function (done) {
    console.debug('ACTS_MissionSnapshot_0300====<begin');
    var subscriber;
    var setTimeout = 5000;
    var currentAlertTimeout;
    var CommonEventSubscribeInfo = {
        events: ["ACTS_MissionSnapshot_OnActive",
            "ACTS_MissionSnapshot_onShow"
        ],
    };
    try {
        commonEvent.createSubscriber(CommonEventSubscribeInfo).then(async (data) => {
            console.debug("=ACTS_MissionSnapshot_0300 createSubscriber .then(data)=======>"
                + ("json data【") + JSON.stringify(data) + (" 】")
                + " ,data=" + data);
            subscriber = data;
            await commonEvent.subscribe(subscriber, async (err, data) => {
                clearTimeout(currentAlertTimeout);
                console.debug("=ACTS_MissionSnapshot_0300 subscribe (err,data)=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】")
                    + ("json data【") + JSON.stringify(data) + (" 】")
                    + " ,err=" + err + " ,data=" + data);
                unsubscribe("subscribe", subscriber);

                var m_missionId = -1;
                var m_bundleName = "";
                var m_abilityName = "";
                var upperLimit = 10;
                await missionManager.getMissionInfos("", upperLimit, async (err, data) => {
                    console.debug("=ACTS_MissionSnapshot_0300 err,data=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】")
                        + ("json data【") + JSON.stringify(data) + (" 】")
                        + " ,err=" + err + " ,data=" + data);
                    console.debug('ACTS_MissionSnapshot_0300 MissionInfo data.length ====>'
                        + data.length);
                    if (!data.length) {
                        m_missionId = -99;
                        m_bundleName = "";
                        m_abilityName = "";
                        expect("if (!data.length) ").assertEqual(0);
                        unsubscribe("if (!data.length) ", subscriber);
                        console.debug('ACTS_MissionSnapshot_0300====<end 0')
                        done();
                    } else {
                        m_missionId = data[0].missionId;
                        m_bundleName = data[0].want.bundleName;
                        m_abilityName = data[0].want.abilityName;
                    }
                    console.debug("=ACTS_MissionSnapshot_0300 m_missionId====>" + m_missionId);
                    await missionManager.getMissionSnapShot("", m_missionId, async (err, data) => {
                        expect(err.code).assertEqual(0);
                        console.debug("=ACTS_MissionSnapshot_0300 then data====>"
                            + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data);
                        try {
                            expect(data.ability.bundleName).assertEqual(m_bundleName);
                            expect(data.ability.abilityName).assertEqual(m_abilityName);
                            done();
                        } catch (err) {
                            expect(err).assertEqual("0");
                            unsubscribe("catch (err) get ", subscriber);
                            console.debug('ACTS_MissionSnapshot_0300====<end catch (err) get')
                            done();
                        }                      
                    })
                });
            });
        });

        await featureAbility.startAbility(
            {
                want:
                {
                    bundleName: "com.amsst.amsMissionSnapshotTestSingleton",
                    abilityName: "com.amsst.amsMissionSnapshotTestSingleton.MainAbility",
                },
            },
        );
    } catch (err) {
        expect(err).assertEqual("0");
        console.debug('ACTS_MissionSnapshot_0300====<end catch(err)')
        done();
    }
    console.debug('ACTS_MissionSnapshot_0300====< ')
    currentAlertTimeout = setTimeout(() => {
        console.debug('ACTS_MissionSnapshot_0300====<end setTimeout');
        unsubscribe("setTimeout ", subscriber);
        expect("Event False").assertEqual("Event timeout");
        done();
    }, setTimeout);
})

    /*
    * @tc.number: ACTS_MissionSnapshot_0400
    * @tc.name: getMissionSnapshot 
    * @tc.desc: Recent task stack provides current screenshot.(Promise)
    */
    it('ACTS_MissionSnapshot_0400', 0, async function (done) {
        console.debug('ACTS_MissionSnapshot_0400====<begin');
        try {
            missionManager.getMissionSnapShot("", -1)
                .then(function (data) {
                    console.debug("=ACTS_MissionSnapshot_0400 then data====>"
                        + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data);
                    console.debug('ACTS_MissionSnapshot_0400====<end');
                    done();
                }).catch(function (err) {
                    console.debug("=ACTS_MissionSnapshot_0400 catch err ====>"
                        + ("json err 【") + JSON.stringify(err) + (" 】 "));
                    expect(err.code != 0).assertTrue();
                    console.debug('ACTS_MissionSnapshot_0400====<end .catch(err)');
                    done();
                });
        } catch (err) {
            expect(err).assertEqual("0");
            console.debug('ACTS_MissionSnapshot_0400====<end catch(err)');
            done();
        }
    })

    /*
    * @tc.number: ACTS_MissionSnapshot_0500
    * @tc.name: getMissionSnapshot 
    * @tc.desc: Recent task stack provides current screenshot.(Promise)
    */
    it('ACTS_MissionSnapshot_0500', 0, async function (done) {
        console.debug('ACTS_MissionSnapshot_0500====<begin');
        var m_missionId = -1;
        var m_bundleName = "";
        var m_abilityName = "";
        try {
            var upperLimit = 10;
            await missionManager.getMissionInfos("", upperLimit, async (err, data) => {
                console.debug("=ACTS_MissionSnapshot_0500 err,data=======>"
                    + ("json err【") + JSON.stringify(err) + (" 】")
                    + ("json data【") + JSON.stringify(data) + (" 】")
                    + " ,err=" + err + " ,data=" + data);
                console.debug('ACTS_MissionSnapshot_0500 MissionInfo data.length ====>: ' + data.length);
                if (!data.length) {
                    m_missionId = -99;
                    m_bundleName = "";
                    m_abilityName = "";
                    expect("if (!data.length) ").assertEqual(0);
                    console.debug('ACTS_MissionSnapshot_0500====<end 0')
                    done();
                } else {
                    m_missionId = data[0].missionId;
                    m_bundleName = data[0].want.bundleName;
                    m_abilityName = data[0].want.abilityName;
                }
                console.debug("=ACTS_MissionSnapshot_0500 m_missionId====>" + m_missionId);
                missionManager.getMissionSnapShot("", m_missionId)
                    .then(function (data) {
                        console.debug("=ACTS_MissionSnapshot_0500 then data====>"
                            + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data);
                        expect(data.ability.bundleName).assertEqual(m_bundleName);
                        expect(data.ability.abilityName).assertEqual(m_abilityName);
                        console.debug("=ACTS_MissionSnapshot_0500 err,data=======> JSON.stringify(data.snapshot)="
                            + JSON.stringify(data.snapshot) + " , " + data.snapshot);
                        done();
                    }).catch(function (err) {
                        console.debug("=ACTS_MissionSnapshot_0500 catch err ====>"
                            + ("json err 【") + JSON.stringify(err) + (" 】 "));
                        expect(err).assertEqual("false");
                        console.debug('ACTS_MissionSnapshot_0500====<end .catch(err)')
                        done();
                    });
            });
        } catch (err) {
            expect(err).assertEqual("0");
            console.debug('ACTS_MissionSnapshot_0500====<end catch(err)');
            done();
        }
    })

    function unsubscribe(caller, subscriber) {
        commonEvent.unsubscribe(subscriber, (err, data) => {
            console.debug("=ACTS_MissionSnapshot_unsubscribe (err,data)=======>"
                + (caller)
                + (" , json err【") + JSON.stringify(err) + (" 】")
                + ("json data【") + JSON.stringify(data) + (" 】")
                + " ,err=" + err + " ,data=" + data);
        });
    }
    /*
    * @tc.number: ACTS_MissionSnapshot_0600
    * @tc.name: getMissionSnapshot 
    * @tc.desc: Recent task stack provides current screenshot.(Promise)
    */
    it('ACTS_MissionSnapshot_0600', 0, async function (done) {
        console.debug('ACTS_MissionSnapshot_0600====<begin');
        var subscriber;
        var setTimeout = 5000;
        var currentAlertTimeout;
        var CommonEventSubscribeInfo = {
            events: ["ACTS_MissionSnapshot_OnActive",
                "ACTS_MissionSnapshot_onShow"
            ],
        };
        try {
            commonEvent.createSubscriber(CommonEventSubscribeInfo).then(async (data) => {
                console.debug("=ACTS_MissionSnapshot_0600 createSubscriber .then(data)=======>"
                    + ("json data【") + JSON.stringify(data) + (" 】")
                    + " ,data=" + data);
                subscriber = data;
                await commonEvent.subscribe(subscriber, async (err, data) => {
                    clearTimeout(currentAlertTimeout);
                    console.debug("=ACTS_MissionSnapshot_0600 subscribe (err,data)=======>"
                        + ("json err【") + JSON.stringify(err) + (" 】")
                        + ("json data【") + JSON.stringify(data) + (" 】")
                        + " ,err=" + err + " ,data=" + data);
                    unsubscribe("subscribe", subscriber);

                    var m_missionId = -1;
                    var m_bundleName = "";
                    var m_abilityName = "";
                    var upperLimit = 10;
                    await missionManager.getMissionInfos("", upperLimit, (err, data) => {
                        console.debug("=ACTS_MissionSnapshot_0600 err,data=======>"
                            + ("json err【") + JSON.stringify(err) + (" 】")
                            + ("json data【") + JSON.stringify(data) + (" 】")
                            + " ,err=" + err + " ,data=" + data);
                        console.debug('ACTS_MissionSnapshot_0600 MissionInfo data.length ====>'
                            + data.length);
                        if (!data.length) {
                            m_missionId = -99;
                            m_bundleName = "";
                            m_abilityName = "";
                            expect("if (!data.length) ").assertEqual(0);
                            unsubscribe("if (!data.length) ", subscriber);
                            console.debug('ACTS_MissionSnapshot_0600====<end 0')
                            done();
                        } else {
                            m_missionId = data[0].missionId;
                            m_bundleName = data[0].want.bundleName;
                            m_abilityName = data[0].want.abilityName;
                        }
                        console.debug("=ACTS_MissionSnapshot_0600 m_missionId====>" + m_missionId);
                        missionManager.getMissionSnapShot("", m_missionId).then(function (data) {
                            console.debug("=ACTS_MissionSnapshot_0600 then data====>"
                                + ("json data 【") + JSON.stringify(data) + (" 】") + " , " + data);
                            try {
                                expect(data.ability.bundleName).assertEqual(m_bundleName);
                                expect(data.ability.abilityName).assertEqual(m_abilityName);
                                console.debug("=ACTS_MissionSnapshot_0600=====> JSON.stringify(data.snapshot)="
                                         + JSON.stringify(data.snapshot) + " , " + data.snapshot);
                               done();
                            } catch (err) {
                                expect(err).assertEqual("0");
                                unsubscribe("catch (err) get ", subscriber);
                                console.debug('ACTS_MissionSnapshot_0600====<end catch (err) get')
                                done();
                            }
                        }).catch(function (err) {
                            console.debug("=ACTS_MissionSnapshot_0600 catch err ====>"
                                + ("json err 【") + JSON.stringify(err) + (" 】 "));
                            expect(err).assertEqual("false");
                            console.debug('ACTS_MissionSnapshot_0600====<end .catch(err)')
                            unsubscribe(" .catch(err)", subscriber);
                            done();
                        });
                    });
                });
            });
            await featureAbility.startAbility(
                {
                    want:
                    {
                        bundleName: "com.amsst.amsMissionSnapshotTestSingletonS",
                        abilityName: "com.amsst.amsMissionSnapshotTestSingletonS.MainAbility",
                    },
                },
            );

        } catch (err) {
            expect("0").assertEqual(err);
            console.debug('ACTS_MissionSnapshot_0600====<end catch(err)')
            done();
        }
        console.debug('ACTS_MissionSnapshot_0600====< 333')
        currentAlertTimeout = setTimeout(() => {
            console.debug('ACTS_MissionSnapshot_0600====<end setTimeout');
            unsubscribe("setTimeout ", subscriber);
            expect("Event False").assertEqual("Event timeout");
            done();
        }, setTimeout);
    })
})