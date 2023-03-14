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

import wantAgent from '@ohos.wantAgent';
import { OperationType, WantAgentFlags } from '@ohos.wantagent';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';
var time = 1000
var WantAgent;
describe('ActsAnsWantAgentOneTest', function () {
    console.info('----ActsWantAgentTest----');

    /*
     * @tc.number: ACTS_SetWant_0100
     * @tc.name: getWantAgent(OperationType.START_ABILITY)
     * @tc.desc: verify the function of getWantAgent(OperationType.START_ABILITY)
     */
    it('ACTS_SetWant_0100', 0, async function (done) {
        console.info('----ACTS_SetWant_0100 start----');
        var agentInfo = {
            wants: [
                    {
                        deviceId: "",
                        bundleName: "com.example.WantAgentTest1",
                        abilityName: "com.example.WantAgentTest1.MainAbility",
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
                    },
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        }
        console.info('----getWantAgent before----');
       await wantAgent.getWantAgent(agentInfo,
            (err, data) => {
                if (err.code == 0) {
                    WantAgent = data;
                    console.info('----getWantAgent success!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                    var triggerInfo = {
                        code:0
                    }
                    wantAgent.trigger(WantAgent, triggerInfo,
                        (err, data) => {
                            if (err.code == 0) {
                                console.info('----trigger success!----');
                                console.info('== trigger data  ' + JSON.stringify(data) );
                                expect(data.finalCode).assertEqual(0);
                                expect(data.finalData).assertEqual("");
                                expect(data.want.deviceId).assertEqual("");
                                expect(data.want.bundleName).assertEqual("com.example.WantAgentTest1");
                                expect(data.want.abilityName).assertEqual("com.example.WantAgentTest1.MainAbility");
                                expect(data.want.uri).assertEqual("key={true,true,false}");
                                expect(JSON.stringify(data.want.entities)).assertEqual(JSON.stringify(["entity1"]));
                                expect(data.want.action).assertEqual("action1");
                            } else {
                                console.info('----trigger failed!----');
                                console.info('== trigger data  ' + JSON.stringify(data) );
                            }
                            done();
                        }
                    );
                } else {
                    console.info('----getWantAgent failed!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                }
                done();
            }
		
        );
	setTimeout(function(){
                    console.debug("====>time out ACTS_SetWant_0100====>");
                }, time);
	
        console.info('----getWantAgent after----');
    })

    /*
     * @tc.number: ACTS_SetWant_0200
     * @tc.name: getWantAgent(OperationType.START_ABILITY)
     * @tc.desc: verify the function of getWantAgent(OperationType.START_ABILITY)[ONE_TIME_FLAG]
     */
    it('ACTS_SetWant_0200', 0, async function (done) {
        console.info('----ACTS_SetWant_0200 start----');
        var agentInfo = {
            wants: [
                    {
                        deviceId: "",
                        bundleName: "com.example.WantAgentTest1",
                        abilityName: "com.example.test.MainAbility",
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
                    },
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.ONE_TIME_FLAG]
        }
        console.info('----getWantAgent before----');
        await wantAgent.getWantAgent(agentInfo,
            (err, data) => {
                if (err.code == 0) {
                    WantAgent = data;
                    console.info('----getWantAgent success!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                } else {
                    console.info('----getWantAgent failed!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                }
                done();
            }

        );
        console.info('----getWantAgent after----');
    })

    /*
     * @tc.number: ACTS_SetWant_0300
     * @tc.name: getWantAgent(OperationType.START_ABILITY)
     * @tc.desc: verify the function of getWantAgent(OperationType.START_ABILITY)[NO_BUILD_FLAG]
     */
    it('ACTS_SetWant_0300', 0, async function (done) {
        console.info('----ACTS_SetWant_0300 start----');
        var agentInfo = {
            wants: [
                    {
                        deviceId: "",
                        bundleName: "com.example.WantAgentTest1",
                        abilityName: "com.example.test.MainAbility",
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
                    },
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.NO_BUILD_FLAG]
        }
        console.info('----getWantAgent before----');
        await wantAgent.getWantAgent(agentInfo,
            (err, data) => {
                if (err.code == 0) {
                    WantAgent = data;
                    console.info('----getWantAgent success!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                } else {
                    console.info('----getWantAgent failed!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                }
                done();
            }

        );
        console.info('----getWantAgent after----');
    })

    /*
     * @tc.number: ACTS_SetWant_0400
     * @tc.name: getWantAgent(OperationType.START_ABILITY)
     * @tc.desc: verify the function of getWantAgent(OperationType.START_ABILITY)[CANCEL_PRESENT_FLAG]
     */
    it('ACTS_SetWant_0400', 0, async function (done) {
        console.info('----ACTS_SetWant_0400 start----');
        var agentInfo = {
            wants: [
                    {
                        deviceId: "",
                        bundleName: "com.example.WantAgentTest1",
                        abilityName: "com.example.test.MainAbility",
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
                    },
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.CANCEL_PRESENT_FLAG]
        }
        console.info('----getWantAgent before----');
        await wantAgent.getWantAgent(agentInfo,
            (err, data) => {
                if (err.code == 0) {
                    WantAgent = data;
                    console.info('----getWantAgent success!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                } else {
                    console.info('----getWantAgent failed!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                }
                done();
            }

        );
        console.info('----getWantAgent after----');
    })

    /*
     * @tc.number: ACTS_SetWant_0500
     * @tc.name: getWantAgent(OperationType.START_ABILITY)
     * @tc.desc: verify the function of getWantAgent(OperationType.START_ABILITY)[CONSTANT_FLAG]
     */
    it('ACTS_SetWant_0500', 0, async function (done) {
        console.info('----ACTS_SetWant_0500 start----');
        var agentInfo = {
            wants: [
                    {
                        deviceId: "",
                        bundleName: "com.example.WantAgentTest1",
                        abilityName: "com.example.test.MainAbility",
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
                    },
            ],
            operationType: OperationType.START_ABILITY,
            requestCode: 0,
            wantAgentFlags:[WantAgentFlags.CONSTANT_FLAG]
        }
        console.info('----getWantAgent before----');
        await wantAgent.getWantAgent(agentInfo,
            (err, data) => {
                if (err.code == 0) {
                    WantAgent = data;
                    console.info('----getWantAgent success!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                } else {
                    console.info('----getWantAgent failed!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                }
                done();
            }

        );
        console.info('----getWantAgent after----');
    })
})

