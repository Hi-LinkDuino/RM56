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

var WantAgenta;
var WantAgentb;
var WantAgentc;
var WantAgentd;
var WantAgente;
var time = 1000
describe('ActsAnsGetWantAgentInfoTest', function () {
    console.info('----ActsGetWantAgentInfoTest----');
    /*
    * @tc.number: ACTS_SetWantInfo_0100
    * @tc.name: getWantAgent(),getBundleName(),getUid(),getWant(),cancel()
    * @tc.desc: verify the function of getWantAgent(),getBundleName(),getUid(),getWant(),cancel()
    */
    it('ACTS_SetWantInfo_0100', 0, async function (done) {
        console.info('----ACTS_SetWantInfo_0100----');
        var agentInfoa = {
            wants: [
                    {
                        bundleName: "com.example.WantAgentTest",
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
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        }
        wantAgent.getWantAgent(agentInfoa,
            (err, data) => {
                if (err.code == 0) {
                    WantAgenta = data;
                    console.info('----getWantAgenta success!----'+data);
                    expect(typeof(data)).assertEqual("object");

                    wantAgent.getBundleName(data,
                        (err1, data1) => {
                            if (err1.code == 0) {
                                console.info('----getBundleNameA success!----'+err.code+data1);
                                expect(typeof(data1)).assertEqual('string')
                            } else {
                                console.info('----getBundleNameA failed!----');
                            }
                        }
                    );
                    wantAgent.getUid(data,
                        (err2, data2) => {
                            if (err2.code == 0) {
                                console.info('----getUidA success!----'+err.code+data2);
                                expect(typeof(data2)).assertEqual('number')
                            } else {
                                console.info('----getUidA failed!----');
                            }
                        }
                    );
                    wantAgent.getWant(data,(err3, data3) => {
                        if (err3.code == 0) {
                            console.info('----getWantA success!----'+err.code+data3);
                            expect(typeof(data3)).assertEqual('object')
                        } else {
                            console.info('----getWantA failed!----');
                        }
                    });
                    wantAgent.cancel(data,(err)=>{
                        console.info("========cancelA========"+err.code)
                    });
                } else {
                    console.info('----getWantAgenta failed!----'+err.code+data);
                    expect(typeof(data)).assertEqual("object");
                }
                done();
            }),
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_0100====>");
        }, time);
            console.info('----getWantAgenta after----');
    })
    /*
    * @tc.number: ACTS_SetWantInfo_0200
    * @tc.name: getWantAgent(),getBundleName(),getUid(),getWant(),cancel()
    * @tc.desc: verify the function of getWantAgent(),getBundleName(),getUid(),getWant(),cancel()  promise
    */
    it('ACTS_SetWantInfo_0200', 0, async function (done) {
        var agentInfod = {
            wants: [
                    {
                        bundleName: "com.example.WantAgentTest",
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
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        }
        wantAgent.getWantAgent(agentInfod).then(
            (data) => {
                console.info('----getWantAgent Promise success!----'+data);
                expect(typeof(data)).assertEqual("object");
                wantAgent.getBundleName(data).then(
                    (data1) => {
                        console.info('----getBcanundleName Promise success!----'+data1);
                        expect(typeof(data1)).assertEqual('string')
                    }
                );
                wantAgent.getUid(data).then(
                    (data2) => {
                        console.info('----getUid Promise success!----'+data2);
                        expect(typeof(data2)).assertEqual('number')
                    }
                );
                wantAgent.getWant(data).then(
                    (data3) => {
                        console.info('----getWant Promise success!----'+data3);
                        console.info('Want = ' + data3);
                        expect(typeof(data3)).assertEqual('object')
                    }
                );
                wantAgent.cancel(data).then(
                    console.info("========cancelPromise========")
                );
            })
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_0200====>");
        }, time)
    })
    /*
    * @tc.number: ACTS_SetWantInfo_0300
    * @tc.name: equal(normal)
    * @tc.desc: verify the function of equal(normal)Callback
    */
    it('ACTS_SetWantInfo_0300', 0, async function (done) {
        var agentInfob = {
            wants: [
                    {
                        bundleName: "bundleName",
                        abilityName: "abilityName",
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
        wantAgent.getWantAgent(agentInfob,(err,data)=>{
            WantAgentb = data
        })

        var agentInfoc = {
            wants: [
                    {
                        bundleName: "com.example.WantAgentTest",
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
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        }
		setTimeout(function(){
			wantAgent.getWantAgent(agentInfoc,
            (err, data) => {
                if (err.code == 0) {
                    WantAgentc = data;
                    console.log("=======WantAgentb======="+JSON.stringify(WantAgentb))
                    console.log("=======WantAgentc======="+JSON.stringify(WantAgentc))
                    expect(typeof(data)).assertEqual("object");
                    wantAgent.equal(WantAgentb,WantAgentc,
                        (error,data) => {
                            if(error.code == 0) {
                                console.info('----equala success!----'+data)
                                expect(typeof(data)).assertEqual("boolean");
                                expect(data).assertEqual(false);
                            }
                            else{
                                console.info('----equala failed!----')
                            }
                        }
                    )
                    wantAgent.equal(WantAgentb,WantAgentb,
                        (error,data) => {
                            if(error.code == 0) {
                                console.info('----equalb success!----'+data)
                                expect(typeof(data)).assertEqual("boolean");
                                expect(data).assertEqual(true);
                            }
                            else{
                                console.info('----equalb failed!----')
                            }
                        }
                    )
                }
                else{
                    console.info('----getWantAgent failed!----');
                    console.info(data);
                    expect(typeof(data)).assertEqual("object");
                }
                setTimeout(function(){
                    console.debug("====>time out ACTS_SetWantInfo_0300====>");
					done();
                }, time);
            });
        }, 1000);
        
      })

    /*
    * @tc.number: ACTS_SetWantInfo_0400
    * @tc.name: equal(normal)
    * @tc.desc: verify the function of equal(normal) promise
    */
    it('ACTS_SetWantInfo_0400', 0, async function (done) {
        var agentInfoe = {
            wants: [
                    {
                        bundleName: "bundleName",
                        abilityName: "abilityName",
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
        wantAgent.getWantAgent(agentInfoe,(err,data)=>{
            console.log("=======agentInfoe======="+err.code+JSON.stringify(data))
            WantAgentd = data
        })

        var agentInfof = {
            wants: [
                    {
                        bundleName: "com.example.WantAgentTest",
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
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        }
		setTimeout(function(){
			wantAgent.getWantAgent(agentInfof,
            (err, data) => {
                console.log("=======agentInfof======="+err.code+JSON.stringify(data))
                if (err.code == 0) {
                    WantAgente = data;
                    console.log("=======WantAgente======="+JSON.stringify(WantAgentd))
                    console.log("=======WantAgentf======="+JSON.stringify(WantAgente))
                    wantAgent.equal(WantAgentd,WantAgente).then(
                        (data) => {
                            console.info('----equalc success!----'+data)
                            expect(typeof(data)).assertEqual("boolean");
                            expect(data).assertEqual(false);
                        }
                    )
                    wantAgent.equal(WantAgentd,WantAgentd).then(
                        (data) => {
                            console.info('----equald success!----'+data)
                            expect(typeof(data)).assertEqual("boolean");
                            expect(data).assertEqual(true);
                        }
                    )
                }
                else{
                    console.info('----getWantAgent failed!----'+err.code+data);
                }
                
                setTimeout(function(){
                    console.debug("====>time out ACTS_SetWantInfo_0400====>");
					done();
                }, time);
            });
		}, 1000);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_0500
    * @tc.name: equal(),cancel()
    * @tc.desc: verify the function of equal(),cancel() promise
    */
    it('ACTS_SetWantInfo_0500', 0, async function (done) {
        var agentInfoe = {
            wants: [
                    {
                        bundleName: "$%^%^%&^%&",
                        abilityName: "$%^&%&*^&*^",
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
        wantAgent.getWantAgent(agentInfoe,(err,data)=>{
            WantAgentd = data
        })

        var agentInfof = {
            wants: [
                    {
                        bundleName: "com.neu.WantAgentTest1",
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
            wantAgentFlags:[WantAgentFlags.UPDATE_PRESENT_FLAG]
        }
		setTimeout(function(){
			wantAgent.getWantAgent(agentInfof,
            (err, data) => {
                if (err.code == 0) {
                    WantAgente = data;
                    console.log("=======WantAgente======="+JSON.stringify(WantAgentd))
                    console.log("=======WantAgentf======="+JSON.stringify(WantAgente))
                    expect(typeof(data)).assertEqual("object");
                    wantAgent.equal(WantAgentd,WantAgente).then(
                        (data) => {
                            console.info('----equale success!----'+data)
                            expect(typeof(data)).assertEqual("boolean");
                        }
                    )
                    wantAgent.cancel(WantAgentd).then(
                        console.info("========cancelPromise========")
                    );
                    wantAgent.equal(WantAgentd,WantAgente).then(
                        (data) => {
                            console.info('----equalf success!----'+data)
                            expect(typeof(data)).assertEqual("boolean");
                        }
                    )
                    wantAgent.equal(WantAgentd,WantAgentd).then(
                        (data) => {
                            console.info('----equalg success!----'+data)
                            expect(typeof(data)).assertEqual("boolean");
                        }
                    )
                }
                else{
                    console.info('----getWantAgent failed!----'+err.code);
                }
                done();
                setTimeout(function(){
                    console.debug("====>time out ACTS_SetWantInfo_0500====>");
                }, time);
            });
		}, 1000);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_0600
    * @tc.name: equal()
    * @tc.desc: verify the function of equal("$%$%^$%^","$%$%^$%^")callback
    */
    it('ACTS_SetWantInfo_0600', 0, async function (done) {
        wantAgent.equal("$%$%^$%^","$%$%^$%^",
            (error,data) => {
                if(error.code == 0) {
                    console.info('----equalh success!----'+data)
                    expect(typeof(data)).assertEqual("boolean");
                }
                else{
                    console.info('----equalh failed!----')
                }
            }
        )
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_0600====>");
        }, time);
    });
    /*
    * @tc.number: ACTS_SetWantInfo_0700
    * @tc.name: equal()
    * @tc.desc: verify the function of equal("$%$%^$%^","$%$%^$%^")promise
    */
    it('ACTS_SetWantInfo_0700', 0, async function (done) {
        var promise = await wantAgent.equal("$%$%^$%^","$%$%^$%^")
        console.info('----equali success!----'+promise)
        expect(promise).assertEqual(undefined);
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_0700====>");
        }, time);
    });
    /*
    * @tc.number: ACTS_SetWantInfo_0800
    * @tc.name: equal()
    * @tc.desc: verify the function of equal({},{})callback
    */
    it('ACTS_SetWantInfo_0800', 0, async function (done) {
        wantAgent.equal({},{},
            (err,data) => {
                console.info('----equalj success!----'+err.code+data)
                expect(typeof(data)).assertEqual("boolean");
            }
        )
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_0800====>");
        }, time);
    });
    /*
    * @tc.number: ACTS_SetWantInfo_0900
    * @tc.name: equal()
    * @tc.desc: verify the function of equal({},{})promise
    */
    it('ACTS_SetWantInfo_0900', 0, async function (done) {
        var promise = wantAgent.equal({},{})
        console.info('----equalk success!----'+promise)
        expect(promise).assertEqual(null);
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_0900====>");
        }, time);
    });
    /*
    * @tc.number: ACTS_SetWantInfo_1000
    * @tc.name: equal()
    * @tc.desc: verify the function of equal(100,100)callback
    */
    it('ACTS_SetWantInfo_1000', 0, async function (done) {
        wantAgent.equal(100,100,
            (err,data) => {
                console.info('----equalm success!----'+err.code+data)
                expect(typeof(data)).assertEqual("boolean");
            }
        )
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1000====>");
        }, time);
    });
    /*
    * @tc.number: ACTS_SetWantInfo_2200
    * @tc.name: equal()
    * @tc.desc: verify the function of equal(100,100)promise
    */
    it('ACTS_SetWantInfo_1100', 0, async function (done) {
        var promise = await wantAgent.equal(100,100)
        console.info('----equaln success!----'+promise)
        expect(promise).assertEqual(undefined);
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1100====>");
        }, time);
    });
    /*
    * @tc.number: ACTS_SetWantInfo_1200
    * @tc.name: getWantAgent()
    * @tc.desc: verify the function of getWantAgent("")callback
    */
    it('ACTS_SetWantInfo_1200', 0, async function (done) {
        console.info('----ACTS_SetWantInfo_0300 begin----');
        wantAgent.getWantAgent("",(err,data) => {
            console.log("===getWantAgenta==="+err.code+data)})
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1200====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_1300
    * @tc.name: getWantAgent()
    * @tc.desc: verify the function of getWantAgent("")promise
    */
    it('ACTS_SetWantInfo_1300', 0, async function (done) {
        console.info('----ACTS_SetWantInfo_0300 begin----');
        var promise = wantAgent.getWantAgent("")
        console.log("===getWantAgentb==="+promise)
        expect(promise).assertEqual(null);
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1300====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_1400
    * @tc.name: getBundleName()
    * @tc.desc: verify the function of getBundleName("")callback
    */
    it('ACTS_SetWantInfo_1400', 0, async function (done) {
        wantAgent.getBundleName("",(err,data) => {
                console.info('----getBundleNamea success!----'+err.code+data);
                expect(typeof(data)).assertEqual('string')});
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1400====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_1500
    * @tc.name: getBundleName()
    * @tc.desc: verify the function of getBundleName("")promise
    */
    it('ACTS_SetWantInfo_1500', 0, async function (done) {
        var promise = await wantAgent.getBundleName("")
        console.info('----getBundleNameb success!----'+promise);
        expect(promise).assertEqual(undefined)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1500====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_1600
    * @tc.name: getUid()
    * @tc.desc: verify the function of getUid("")callback
    */
    it('ACTS_SetWantInfo_1600', 0, async function (done) {
        wantAgent.getUid("",(err,data) => {
                console.info('----getUida success!----'+err.code+data);
                expect(typeof(data)).assertEqual('number')});
                done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1600====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_1700
    * @tc.name: getUid()
    * @tc.desc: verify the function of getUid("")promise
    */
    it('ACTS_SetWantInfo_1700', 0, async function (done) {
        var promise =await wantAgent.getUid("")
        console.info('----getUidb success!----'+promise);
        expect(promise).assertEqual(undefined)
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1700====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_1800
    * @tc.name: getWant()
    * @tc.desc: verify the function of getWant("")callback
    */
    it('ACTS_SetWantInfo_1800', 0, async function (done) {
        wantAgent.getWant("",(err,data) => {
            console.info('----getWanta success!----'+err.code+data);
            expect(typeof(data)).assertEqual('object')
        });
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1800====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_1900
    * @tc.name: getWant()
    * @tc.desc: verify the function of getWant("")promise
    */
    it('ACTS_SetWantInfo_1900', 0, async function (done) {
        var promise = await wantAgent.getWant("")
        console.info('----getWantb success!----'+promise);
        expect(promise).assertEqual(undefined)
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_1900====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2000
    * @tc.name: getWantAgent()
    * @tc.desc: verify the function of getWantAgent(100)callback
    */
    it('ACTS_SetWantInfo_2000', 0, async function (done) {
        console.info('----ACTS_SetWantInfo_1000 begin----');
        wantAgent.getWantAgent(100,(err,data) => {
                console.log('----getWantAgenta success!----'+err.code+data)
            })
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2000====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2100
    * @tc.name: getWantAgent()
    * @tc.desc: verify the function of getWantAgent(100)promises
    */
    it('ACTS_SetWantInfo_2100', 0, async function (done) {
        var promise = wantAgent.getWantAgent(100)
        console.log("----getWantAgentb success!----"+promise)
        expect(promise).assertEqual(null)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2100====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2200
    * @tc.name: getBundleName()
    * @tc.desc: verify the function of getBundleName(100)callback
    */
    it('ACTS_SetWantInfo_2200', 0, async function (done) {
        wantAgent.getBundleName(100,(err,data) => {
                console.info('----getBundleNamec success!----'+err.code+data);
                expect(typeof(data)).assertEqual('string')
            });
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2200====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2300
    * @tc.name: getBundleName()
    * @tc.desc: verify the function of getBundleName(100)promise
    */
    it('ACTS_SetWantInfo_2300', 0, async function (done) {
        var promise =await wantAgent.getBundleName(100)
        console.info('----getBundleNamed success!----'+promise);
        expect(promise).assertEqual(undefined)
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2300====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2400
    * @tc.name: getUid()
    * @tc.desc: verify the function of getUid(100)callback
    */
    it('ACTS_SetWantInfo_2400', 0, async function (done) {
        wantAgent.getUid(100,(err,data) => {
                console.info('----getUidc success!----'+err.code+data);
            });
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2400====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2500
    * @tc.name: getUid()
    * @tc.desc: verify the function of getUid(100)promise
    */
    it('ACTS_SetWantInfo_2500', 0, async function (done) {
        var promise =await wantAgent.getUid(100)
        console.info('----getUidd success!----'+promise);
        expect(promise).assertEqual(undefined)
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2500====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2600
    * @tc.name: getWant()
    * @tc.desc: verify the function of getWant(100)callback
    */
    it('ACTS_SetWantInfo_2600', 0, async function (done) {
        wantAgent.getWant(100,(err,data) => {
            console.info('----getWantc success!----'+err.code+data);
            expect(typeof(data)).assertEqual('object')
        });
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2600====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2700
    * @tc.name: getWant()
    * @tc.desc: verify the function of getWant(100)promise
    */
    it('ACTS_SetWantInfo_2700', 0, async function (done) {
        var promise = await wantAgent.getWant(100)
        console.info('----getWantd success!----'+promise);
        expect(promise).assertEqual(undefined)
        done()
        setTimeout(function(){
            console.debug("====>time out ACTS_SetWantInfo_2700====>");
        }, time);
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2800
    * @tc.name: getWantAgent()
    * @tc.desc: verify the function of getWantAgent({})callback
    */
    it('ACTS_SetWantInfo_2800', 0, async function (done) {
        console.info('----ACTS_SetWantInfo_2800 begin----');
        wantAgent.getWantAgent({},(err,data) => {
                console.log("----getWantAgentc success!----"+err.code+data)
            })
        done();
    })
    /*
    * @tc.number: ACTS_SetWantInfo_2900
    * @tc.name: getWantAgent()
    * @tc.desc: verify the function of getWantAgent({})promise
    */
    it('ACTS_SetWantInfo_2900', 0, async function (done) {
        console.info('----ACTS_SetWantInfo_2900 begin----');
        wantAgent.getWantAgent({}).then((data)=>{
            console.log("----getWantAgentd success!----"+data)
            expect(data).assertEqual("object")
           })
        done();
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3000
    * @tc.name: getBundleName()
    * @tc.desc: verify the function of getBundleName({})callback
    */
    it('ACTS_SetWantInfo_3000', 0, async function (done) {
        wantAgent.getBundleName({},(err,data) => {
                console.info('----getBundleNamee success!----'+err.code+data);
                expect(typeof(data)).assertEqual('string')
            });
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3100
    * @tc.name: getBundleName()
    * @tc.desc: verify the function of getBundleName({})promise
    */
    it('ACTS_SetWantInfo_3100', 0, async function (done) {
        var promise = wantAgent.getBundleName({})
            console.info('----getBundleNamef success!----'+promise);
            expect(promise).assertEqual(null)
            done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3200
    * @tc.name: getUid()
    * @tc.desc: verify the function of getUid({})callback
    */
    it('ACTS_SetWantInfo_3200', 0, async function (done) {
        wantAgent.getUid({},(err,data) => {
                console.info('----getUide success!----'+err.code+data);
            });
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3300
    * @tc.name: getUid()
    * @tc.desc: verify the function of getUid({})promise
    */
    it('ACTS_SetWantInfo_3300', 0, async function (done) {
        var promise = wantAgent.getUid({})
            console.info('----getUidf success!----'+promise);
            expect(promise).assertEqual(null)
            done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3400
    * @tc.name: getWant()
    * @tc.desc: verify the function of getWant({})callback
    */
    it('ACTS_SetWantInfo_3400', 0, async function (done) {
        wantAgent.getWant({},(err,data) => {
            console.info('----getWante success!----'+err.code+data);
        });
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3500
    * @tc.name: getWant()
    * @tc.desc: verify the function of getWant({})promise
    */
    it('ACTS_SetWantInfo_3500', 0, async function (done) {
       var promise = wantAgent.getWant({})
            console.info('----getWantf success!----'+promise);
            expect(promise).assertEqual(null)
            done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3600
    * @tc.name: cancel()
    * @tc.desc: verify the function of cancel({})callback
    */
    it('ACTS_SetWantInfo_3600', 0, async function (done) {
        wantAgent.cancel({},(err,data) => {
            console.info('----cancela success!----'+err.code+data);
        });
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3700
    * @tc.name: cancel()
    * @tc.desc: verify the function of cancel({})promise
    */
    it('ACTS_SetWantInfo_3700', 0, async function (done) {
        var promise = await wantAgent.cancel({})
            console.info('----cancelb success!----'+promise);
            expect(promise).assertEqual(null)
            done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3800
    * @tc.name: cancel()
    * @tc.desc: verify the function of cancel(100)callback
    */
    it('ACTS_SetWantInfo_3800', 0, async function (done) {
        wantAgent.cancel(100,(err) => {
            console.info('----cancelc success!----'+err.code);
        });
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_3900
    * @tc.name: cancel()
    * @tc.desc: verify the function of cancel(100)promise
    */
    it('ACTS_SetWantInfo_3900', 0, async function (done) {
        var promise =await wantAgent.cancel(100)
            console.info('----canceld success!----'+promise);
            expect(promise).assertEqual(undefined)
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_4000
    * @tc.name: getWant()
    * @tc.desc: verify the function of cancel("")callback
    */
    it('ACTS_SetWantInfo_4000', 0, async function (done) {
        wantAgent.cancel("",(err) => {
            console.info('----cancele success!----'+err.code);
        });
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_4100
    * @tc.name: cancel()
    * @tc.desc: verify the function of cancel("")promise
    */
    it('ACTS_SetWantInfo_4100', 0, async function (done) {
        var promise = await wantAgent.cancel("")
            console.info('----cancelf success!----'+promise);
        expect(promise).assertEqual(undefined)
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_4200
    * @tc.name: cancel()
    * @tc.desc: verify the function of cancel("%^%^%^")callback
    */
    it('ACTS_SetWantInfo_4200', 0, async function (done) {
        wantAgent.cancel("%^%^%^",(err) => {
            console.info('----cancelg success!----'+err.code);
        });
        done()
    })
    /*
    * @tc.number: ACTS_SetWantInfo_4300
    * @tc.name: cancel()
    * @tc.desc: verify the function of cancel("%^%^%^")promise
    */
    it('ACTS_SetWantInfo_4300', 0, async function (done) {
        var promise =await wantAgent.cancel("%^%^%^")
            console.info('----cancelh success!----'+promise);
        expect(promise).assertEqual(undefined)
        done()
    })
})


