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
import notify from '@ohos.notification'
import WantAgent from '@ohos.wantAgent'
import { OperationType, WantAgentFlags } from '@ohos.wantagent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('ActsAnsActionButtonTest', function () {
    console.info("===ActsDoNotSubscriberTest start===>");
    function publishCallbacka(err){
        console.debug("===>publishCallbacka===>"+err.code);
        expect(err.code).assertEqual(0)
    }
    function publishCallbackb(err){
        console.debug("===>publishCallbackb===>"+err.code);
        expect(err.code).assertEqual(0)
    }
    function publishCallbackc(err){
        console.debug("===>publishCallbackc===>"+err.code);
        expect(err.code).assertEqual(0)
    }

    function consumeCallbackA(data) {
        console.debug("===>consumeCallbackA data : ===>" +JSON.stringify(data));
        var triggerInfo = {
            code:0
        }
        expect(data.request.actionButtons[0].title).assertEqual("buttonA")
        var wantAgenta = data.request.actionButtons[0].wantAgent
        console.debug("===>titleA: ===>" + JSON.stringify(data.request.actionButtons[0].title))
        console.debug("===>wantAgentA: ===>" + JSON.stringify(wantAgenta))
        WantAgent.trigger(wantAgenta, triggerInfo,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerA success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerA failed!----'+err.code);
                }
            }
        );
    }

    /*
     * @tc.number: ActsActiveButton_test_0100
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsActiveButton_test_0100', 0, async function (done) {
        console.debug("===ActsActiveButton_test_0100===begin===>");
        var subInfo ={
            onConsume:consumeCallbackA
        }
        notify.subscribe(subInfo);

        var agentInfoA = {
            wants: [
                    {
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var wantAgentData = await WantAgent.getWantAgent(agentInfoA);

        var notificationRequest = {
                    content:{
                        contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                        longText : {
                            title: "test_title",
                            text: "test_text",
                            additionalText: "test_additionalText",
                            longText: "long_text",
                            briefText: "long_briefText",
                            expandedTitle: "long_expandedTitle"
                        },
                    },
                    id: 1,
                    slotType : notify.SlotType.SERVICE_INFORMATION,
                    actionButtons: [{title:"buttonA", wantAgent:wantAgentData}]
                }
            await notify.publish(notificationRequest, publishCallbacka);
        console.info("===ActsActiveButton_test_0100===end===>");
        setTimeout((async function(){
            notify.unsubscribe(subInfo);
            console.info("======ActsActiveButton_test_0100 setTimeout unsubscribe===>");
            done();
        }),300);
      })

    //consume
    function consumeCallbackB(data) {
        console.debug("===>consumeCallbackB data : ===>" +JSON.stringify(data));
        var triggerInfo = {
            code:1
        }
        expect(data.request.actionButtons[0].title).assertEqual("buttonB")
        var wantAgentB = data.request.actionButtons[0].wantAgent
        console.debug("===>titleB: ===>" + JSON.stringify(data.request.actionButtons[0].title))
        console.debug("===>wantAgentB: ===>" + JSON.stringify(wantAgentB))
        WantAgent.trigger(wantAgentB, triggerInfo,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerB success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerB failed!----'+err.code);
                }
            }
        );
    }

    /*
     * @tc.number: ActsActiveButton_test_0200
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsActiveButton_test_0200', 0, async function (done) {
        console.debug("===ActsActiveButton_test_0200===begin===>");
        var subInfo ={
            onConsume:consumeCallbackB
        }
        notify.subscribe(subInfo);

        var agentInfoB = {
            wants: [
                    {
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var wantAgentDataB = await WantAgent.getWantAgent(agentInfoB);

        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText : {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText: "long_text",
                    briefText: "long_briefText",
                    expandedTitle: "long_expandedTitle"
                },
            },
            id: 2,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            actionButtons: [{title:"buttonB", wantAgent:wantAgentDataB}]
        }
        await notify.publish(notificationRequest).then(()=>{
            console.info("===ActsActiveButton_test_0200===promise===>");
        });
        setTimeout((async function(){
            notify.unsubscribe(subInfo);
            console.info("======ActsActiveButton_test_0200 setTimeout unsubscribe===>");
            done();
        }),300);
    })


    function consumeCallbackC(data) {
        console.debug("===>consumeCallbackC data : ===>" +JSON.stringify(data));
        var triggerInfoC = {
            code:2
        }
        var triggerInfoD = {
            code:3
        }
        expect(data.request.actionButtons[0].title).assertEqual("buttonC")
        expect(data.request.actionButtons[1].title).assertEqual("buttonD")
        var wantAgentC = data.request.actionButtons[0].wantAgent
        var wantAgentD = data.request.actionButtons[1].wantAgent
        console.debug("===>wantAgentC: ===>" + JSON.stringify(wantAgentC))
        console.debug("===>wantAgentD: ===>" + JSON.stringify(wantAgentD))
        WantAgent.trigger(wantAgentC, triggerInfoC,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerC success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerC failed!----'+err.code);
                }
            });
        WantAgent.trigger(wantAgentD, triggerInfoD,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggered success' + err.code + JSON.stringify(data) );
                } else {
                    console.info('----triggered failed!----'+ err.code);
                }
            });
    }

    /*
     * @tc.number: ActsActiveButton_test_0300
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsActiveButton_test_0300', 0, async function (done) {
        console.debug("===ActsActiveButton_test_0300===begin===>");

        var subInfo ={
            onConsume:consumeCallbackC
        }
        await notify.subscribe(subInfo);

        var agentInfoC = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoD = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var wantAgentDataC = await WantAgent.getWantAgent(agentInfoC);
        var wantAgentDataD = await WantAgent.getWantAgent(agentInfoD);
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText : {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText: "long_text",
                    briefText: "long_briefText",
                    expandedTitle: "long_expandedTitle"
                },
            },
            id: 3,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            actionButtons: [{title:"buttonC", wantAgent:wantAgentDataC},{title:"buttonD", wantAgent:wantAgentDataD}]
        }
        notify.publish(notificationRequest, publishCallbackb);
        setTimeout((async function(){
            notify.unsubscribe(subInfo);
            console.info("======ActsActiveButton_test_0300 setTimeout unsubscribe===>");
            done();
        }),300);
    })

    function consumeCallbackD(data) {
        console.debug("===>consumeCallbackD data : ===>" +JSON.stringify(data));
        var triggerInfoE = {
            code:4
        }
        var triggerInfoF = {
            code:5
        }
        expect(data.request.actionButtons[0].title).assertEqual("buttonE")
        expect(data.request.actionButtons[1].title).assertEqual("buttonF")
        var wantAgentE = data.request.actionButtons[0].wantAgent
        var wantAgentF = data.request.actionButtons[1].wantAgent
        console.debug("===>wantAgentC: ===>" + JSON.stringify(wantAgentE))
        console.debug("===>wantAgentD: ===>" + JSON.stringify(wantAgentF))
        WantAgent.trigger(wantAgentE, triggerInfoE,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerInfoE success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerInfoE failed!----'+err.code);
                }
            });
        WantAgent.trigger(wantAgentF, triggerInfoF,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerInfoF success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerInfoF failed!----'+err.code);
                }
            });
    }

    /*
     * @tc.number: ActsActiveButton_test_0400
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsActiveButton_test_0400', 0, async function (done) {
        console.debug("===ActsActiveButton_test_0400===begin===>");

        var subInfo ={
            onConsume:consumeCallbackD
        }
        await notify.subscribe(subInfo);

        var agentInfoE = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoF = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var wantAgentDataE = await WantAgent.getWantAgent(agentInfoE);
        var wantAgentDataF = await WantAgent.getWantAgent(agentInfoF);
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText : {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText: "long_text",
                    briefText: "long_briefText",
                    expandedTitle: "long_expandedTitle"
                },
            },
            id: 3,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            actionButtons: [{title:"buttonE", wantAgent:wantAgentDataE},{title:"buttonF", wantAgent:wantAgentDataF}]
        }
        notify.publish(notificationRequest).then(()=>{
            console.info("======ActsActiveButton_test_0400 promise===>");
        });
        setTimeout((async function(){
            notify.unsubscribe(subInfo);
            console.info("======ActsActiveButton_test_0400 setTimeout unsubscribe===>");
            done();
        }),300);
    })

    function consumeCallbackE(data) {
        console.debug("===>consumeCallbackE data : ===>" +JSON.stringify(data));
        var triggerInfoG = {
            code:1
        }
        var triggerInfoH = {
            code:2
        }
        var triggerInfoI = {
            code:3
        }
        expect(data.request.actionButtons[0].title).assertEqual("buttonG")
        expect(data.request.actionButtons[1].title).assertEqual("buttonH")
        expect(data.request.actionButtons[2].title).assertEqual("buttonI")
        var wantAgentG = data.request.actionButtons[0].wantAgent
        var wantAgentH = data.request.actionButtons[1].wantAgent
        var wantAgentI = data.request.actionButtons[2].wantAgent
        console.debug("===>wantAgentG: ===>" + JSON.stringify(wantAgentG))
        console.debug("===>wantAgentH: ===>" + JSON.stringify(wantAgentH))
        console.debug("===>wantAgentI: ===>" + JSON.stringify(wantAgentI))
        WantAgent.trigger(wantAgentG, triggerInfoG,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerG success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerG failed!----'+err.code);
                }
            });
        WantAgent.trigger(wantAgentH, triggerInfoH,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerH success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerH failed!----'+err.code);
                }
            });
        WantAgent.trigger(wantAgentI, triggerInfoI,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerI success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerI failed!----'+err.code);
                }
            });
    }

    /*
     * @tc.number: ActsActiveButton_test_0500
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsActiveButton_test_0500', 0, async function (done) {
        console.debug("===ActsActiveButton_test_0500===begin===>");

        var subInfo ={
            onConsume:consumeCallbackE
        }
        await notify.subscribe(subInfo);

        var agentInfoG = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoH = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoI = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var wantAgentDataG = await WantAgent.getWantAgent(agentInfoG);
        var wantAgentDataH = await WantAgent.getWantAgent(agentInfoH);
        var wantAgentDataI = await WantAgent.getWantAgent(agentInfoI);
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText : {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText: "long_text",
                    briefText: "long_briefText",
                    expandedTitle: "long_expandedTitle"
                },
            },
            id: 1,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            actionButtons: [{title:"buttonG", wantAgent:wantAgentDataG},{title:"buttonH", wantAgent:wantAgentDataH},{title:"buttonI", wantAgent:wantAgentDataI}]
        }
        notify.publish(notificationRequest, publishCallbackc);
        setTimeout((async function(){
            notify.unsubscribe(subInfo);
            console.info("======ActsActiveButton_test_0500 setTimeout unsubscribe===>");
            done();
        }),300);
    })

    function consumeCallbackF(data) {
        console.debug("===>consumeCallbackF data : ===>" +JSON.stringify(data));
        var triggerInfoJ = {
            code:4
        }
        var triggerInfoK = {
            code:5
        }
        var triggerInfoL = {
            code:6
        }
        expect(data.request.actionButtons[0].title).assertEqual("buttonJ")
        expect(data.request.actionButtons[1].title).assertEqual("buttonK")
        expect(data.request.actionButtons[2].title).assertEqual("buttonL")
        var wantAgentJ = data.request.actionButtons[0].wantAgent
        var wantAgentK = data.request.actionButtons[1].wantAgent
        var wantAgentL = data.request.actionButtons[2].wantAgent
        console.debug("===>wantAgentJ: ===>" + JSON.stringify(wantAgentJ))
        console.debug("===>wantAgentK: ===>" + JSON.stringify(wantAgentK))
        console.debug("===>wantAgentL: ===>" + JSON.stringify(wantAgentL))
        WantAgent.trigger(wantAgentJ, triggerInfoJ,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerJ success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerJ failed!----'+err.code);
                }
            });
        WantAgent.trigger(wantAgentK, triggerInfoK,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerK success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerK failed!----'+err.code);
                }
            });
        WantAgent.trigger(wantAgentL, triggerInfoL,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerI success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerI failed!----'+err.code);
                }
            });
    }

    /*
     * @tc.number: ActsActiveButton_test_0600
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsActiveButton_test_0600', 0, async function (done) {
        console.debug("===ActsActiveButton_test_0600===begin===>");

        var subInfo ={
            onConsume:consumeCallbackF
        }
        await notify.subscribe(subInfo);

        var agentInfoJ = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoK = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoL = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.wantAgentTest",
                        abilityName: "com.example.wantAgentTest.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var wantAgentDataJ = await WantAgent.getWantAgent(agentInfoJ);
        var wantAgentDataK = await WantAgent.getWantAgent(agentInfoK);
        var wantAgentDataL = await WantAgent.getWantAgent(agentInfoL);
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText : {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText: "long_text",
                    briefText: "long_briefText",
                    expandedTitle: "long_expandedTitle"
                },
            },
            id: 1,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            actionButtons: [{title:"buttonJ", wantAgent:wantAgentDataJ},{title:"buttonK", wantAgent:wantAgentDataK},{title:"buttonL", wantAgent:wantAgentDataL}]
        }
        notify.publish(notificationRequest).then(()=>{
            console.info("======ActsActiveButton_test_0600 promise===>");
        });
        setTimeout((async function(){
            notify.unsubscribe(subInfo);
            console.info("======ActsActiveButton_test_0600 setTimeout unsubscribe===>");
            done();
        }),300);
    })
    function consumeCallbackG(data) {
        console.debug("===>consumeCallbackG data : ===>" +JSON.stringify(data));
        var triggerInfoM = {
            code:7
        }
        var triggerInfoN = {
            code:8
        }
        var triggerInfoO = {
            code:9
        }
        //        expect(data.request.actionButtons.length).assertEqual(3)
        expect(data.request.actionButtons[0].title).assertEqual("buttonM")
        expect(data.request.actionButtons[1].title).assertEqual("buttonN")
        expect(data.request.actionButtons[2].title).assertEqual("buttonO")
        var wantAgentM = data.request.actionButtons[0].wantAgent
        var wantAgentN = data.request.actionButtons[1].wantAgent
        var wantAgentO = data.request.actionButtons[2].wantAgent
        console.debug("===>wantAgentM: ===>" + JSON.stringify(wantAgentM))
        console.debug("===>wantAgentN: ===>" + JSON.stringify(wantAgentN))
        console.debug("===>wantAgentO: ===>" + JSON.stringify(wantAgentO))
        WantAgent.trigger(wantAgentM, triggerInfoM,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerM success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerM failed!----'+err.code);
                }
            });
        WantAgent.trigger(wantAgentN, triggerInfoN,
            (err, data) => {
                if (err.code == 0) {
                    console.info('==== triggerN success' + err.code+JSON.stringify(data) );
                } else {
                    console.info('----triggerN failed!----'+err.code);
                }
            });
        setTimeout((function(){
            WantAgent.trigger(wantAgentO, triggerInfoO,
                (err, data) => {
                    if (err.code == 0) {
                        console.info('==== triggerO success' + err.code+JSON.stringify(data) );
                    } else {
                        console.info('----triggerO failed!----'+err.code);
                    }
                });
        }),300);
    }

    /*
     * @tc.number: ActsActiveButton_test_0700
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsActiveButton_test_0700', 0, async function (done) {
        console.debug("===ActsActiveButton_test_0700===begin===>");

        var subInfo ={
            onConsume:consumeCallbackG
        }
        await notify.subscribe(subInfo);

        var agentInfoM = {
            wants: [
                    {
                        bundleName: "com.example.wantAgentTestA",
                        abilityName: "com.example.wantAgentTestA.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoN = {
            wants: [
                    {
                        bundleName: "com.example.wantAgentTestB",
                        abilityName: "com.example.wantAgentTestB.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoO = {
            wants: [
                    {
                        bundleName: "com.example.wantAgentTestC",
                        abilityName: "com.example.wantAgentTestC.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var agentInfoP = {
            wants: [
                    {
                        bundleName: "com.example.wantAgentTestA",
                        abilityName: "com.example.wantAgentTestA.MainAbility",
                        action: "action1",
                        entities: ["entity1"],
                        type: "MIMETYPE",
                        uri: "key={true,true,false}",
                        parameters:
                        {
                            mykey0: 2222,
                            mykey1: [1, 2, 3],
                            mykey2: "[1, 2, 3]",
                            mykey3: "ssssssssssssssssssssssssss",
                            mykey4: [false, true, false],
                            mykey5: ["qqqqq", "wwwwww", "aaaaaaaaaaaaaaaaa"],
                            mykey6: true,
                        }
                    }
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        };
        var wantAgentDataM = await WantAgent.getWantAgent(agentInfoM);
        var wantAgentDataN = await WantAgent.getWantAgent(agentInfoN);
        var wantAgentDataO = await WantAgent.getWantAgent(agentInfoO);
        var wantAgentDataP = await WantAgent.getWantAgent(agentInfoP);
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText : {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText: "long_text",
                    briefText: "long_briefText",
                    expandedTitle: "long_expandedTitle"
                },
            },
            id: 1,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            actionButtons: [{title:"buttonM", wantAgent:wantAgentDataM},{title:"buttonN", wantAgent:wantAgentDataN},{title:"buttonO", wantAgent:wantAgentDataO},{title:"buttonP", wantAgent:wantAgentDataP}]
        }
        notify.publish(notificationRequest).then(()=>{
            console.info("======ActsActiveButton_test_0700 promise===>");
        });
        setTimeout((async function(){
            notify.unsubscribe(subInfo);
            console.info("======ActsActiveButton_test_0700 setTimeout unsubscribe===>");
            done();
        }),300);
    })
})

