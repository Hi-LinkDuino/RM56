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
import featureAbility from '@ohos.ability.featureAbility'
import wantConstant from '@ohos.ability.wantConstant'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe('ActsGetWantTest', function () {
    beforeAll(async (done) => {
        console.info('ActsGetWantTest begin')
        done();
    })

    var TIMEOUT_NUMBER = 2500;

    //  @tc.number: ACTS_GetWant_0100
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_0100', 0, async function (done) {
        featureAbility.startAbilityForResult({
            want: {
                deviceId: "",
                bundleName: "com.example.actsgetwantalltesthap",
                abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                action: "action1",
                parameters:
                {
                    mykey0: 1,
                    mykey1: [1, 2, 3],
                    mykey2: "[1, 2, 3]",
                    mykey3: "str",
                    mykey4: [false, true, false],
                    mykey5: ["str", "STR", "helloopenharmony"],
                },
            }
        },
            (err, data) => {
                console.info('====> ACTS_StartAbilityForResult_0100 start ability=====>' + JSON.stringify(data))
                expect(data.want.deviceId).assertEqual("");
                expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                expect(data.want.action).assertEqual("action1");
                expect(data.want.parameters.mykey0).assertEqual(1);
                expect(data.want.parameters.mykey1[0]).assertEqual(1);
                expect(data.want.parameters.mykey1[1]).assertEqual(2);
                expect(data.want.parameters.mykey1[2]).assertEqual(3);
                expect(data.want.parameters.mykey2).assertEqual("[1, 2, 3]");
                expect(data.want.parameters.mykey3).assertEqual("str");
                expect(data.want.parameters.mykey4[0]).assertEqual(false);
                expect(data.want.parameters.mykey4[1]).assertEqual(true);
                expect(data.want.parameters.mykey4[2]).assertEqual(false);
                expect(data.want.parameters.mykey5[0]).assertEqual("str");
                expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmony");
                setTimeout(()=>{
                    done();
                }, TIMEOUT_NUMBER);
            })
    })

    //  @tc.number: ACTS_GetWant_0400
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_0400', 0, async function (done) {
        featureAbility.startAbilityForResult({
            want:
            {
                deviceId: "",
                bundleName: "com.example.actsgetwantalltesthap",
                abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                action: "action2",
                entities: ["entity1", "entity2"],
                type: "MIMETYPE",
                uri: "key={true,true,false}",
                flags: wantConstant.Flags.FLAG_ABILITY_FORWARD_RESULT,
                parameters:
                {
                    mykey0: 0.1,
                    mykey1: [0.1, 0.2, 0.3],
                    mykey2: "[1, 2, 3]",
                    mykey3: "str",
                    mykey4: [false, true, false],
                    mykey5: ["str", "!@#$%", "helloopenharmony"],
                },
            },
        },
            (err, data) => {
                console.info('====> ACTS_StartAbilityForResult_0400 start ability=====>' + JSON.stringify(data))
                expect(data.want.deviceId).assertEqual("");
                expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                expect(data.want.action).assertEqual("action2");
                expect(data.want.entities[0]).assertEqual("entity1");
                expect(data.want.entities[1]).assertEqual("entity2");
                expect(data.want.type).assertEqual("MIMETYPE");
                expect(data.want.uri).assertEqual("key={true,true,false}");
                expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITY_FORWARD_RESULT);
                expect(data.want.parameters.mykey0).assertEqual(0.1);
                expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                expect(data.want.parameters.mykey1[2]).assertEqual(0.3);
                expect(data.want.parameters.mykey2).assertEqual("[1, 2, 3]");
                expect(data.want.parameters.mykey3).assertEqual("str");
                expect(data.want.parameters.mykey4[0]).assertEqual(false);
                expect(data.want.parameters.mykey4[1]).assertEqual(true);
                expect(data.want.parameters.mykey4[2]).assertEqual(false);
                expect(data.want.parameters.mykey5[0]).assertEqual("str");
                expect(data.want.parameters.mykey5[1]).assertEqual("!@#$%");
                expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmony");
                console.info('====> before done=====>')
                setTimeout(()=>{
                    done();
                }, TIMEOUT_NUMBER);
            })
    })

    //  @tc.number: ACTS_GetWant_0500
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_0500', 0, async function (done) {
        featureAbility.startAbilityForResult({
            want:
            {
                deviceId: "",
                bundleName: "com.example.actsgetwantalltesthap",
                abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                action: "action1",
                entities: ["entity1"],
                type: "MIMETYPE",
                uri: "key={true,true,false}",
                flags: wantConstant.Flags.FLAG_ABILITY_FORM_ENABLED,
                parameters:
                {
                    mykey0: 0.1,
                    mykey1: [0.1, 0.2, 0.0000000003],
                    mykey2: "[a, b, c]",
                    mykey3: "str",
                    mykey4: [false, true, false],
                    mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                },
            },
        },
            (err, data) => {
                console.info('====> ACTS_StartAbilityForResult_0500 start ability=====>' + JSON.stringify(data))
                expect(data.want.deviceId).assertEqual("");
                expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                expect(data.want.action).assertEqual("action1");
                expect(data.want.entities[0]).assertEqual("entity1");
                expect(data.want.type).assertEqual("MIMETYPE");
                expect(data.want.uri).assertEqual("key={true,true,false}");
                expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITY_FORM_ENABLED);
                expect(data.want.parameters.mykey0).assertEqual(0.1);
                expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                expect(data.want.parameters.mykey3).assertEqual("str");
                expect(data.want.parameters.mykey4[0]).assertEqual(false);
                expect(data.want.parameters.mykey4[1]).assertEqual(true);
                expect(data.want.parameters.mykey4[2]).assertEqual(false);
                expect(data.want.parameters.mykey5[0]).assertEqual("str");
                expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                console.info('====> before done=====>')
                setTimeout(()=>{
                    done();
                }, TIMEOUT_NUMBER);
            })
    })
    //  @tc.number: ACTS_GetWant_0600
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_0600', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_AUTH_READ_URI_PERMISSION ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_0600 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_AUTH_READ_URI_PERMISSION);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> before done=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_0700
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_0700', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_AUTH_WRITE_URI_PERMISSION ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_0700 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_AUTH_WRITE_URI_PERMISSION);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> before done=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_0800
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_0800', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_0800 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> before done=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_0900
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_0900', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_NOT_OHOS_COMPONENT ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_0900 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_NOT_OHOS_COMPONENT);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> before done=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1000
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1000', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_AUTH_PERSISTABLE_URI_PERMISSION  ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1000 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_AUTH_PERSISTABLE_URI_PERMISSION );
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> before done=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1100
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1100', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_AUTH_PREFIX_URI_PERMISSION  ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1100 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_AUTH_PREFIX_URI_PERMISSION );
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1200
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1200', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_ABILITYSLICE_MULTI_DEVICE   ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1200 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITYSLICE_MULTI_DEVICE  );
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1300
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1300', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_START_FOREGROUND_ABILITY   ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1300 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_START_FOREGROUND_ABILITY );
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1400
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1400', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_ABILITY_CONTINUATION_REVERSIBLE,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1400 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITY_CONTINUATION_REVERSIBLE);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1500
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1500', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_INSTALL_ON_DEMAND,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1500 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_INSTALL_ON_DEMAND);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1600
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1600', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_INSTALL_WITH_BACKGROUND_MODE,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1600 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_INSTALL_WITH_BACKGROUND_MODE);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1700
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1700', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_ABILITY_CLEAR_MISSION,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1700 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITY_CLEAR_MISSION);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1800
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1800', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_ABILITY_NEW_MISSION,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1800 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITY_NEW_MISSION);
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
    //  @tc.number: ACTS_GetWant_1900
    //  @tc.name: getWant : get want in current ability
    //  @tc.desc:Start the ability through startabilityforresult,
    //           and then use terminateselfwithresult to return the data
    it('ACTS_GetWant_1900', 0, async function (done) {
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action1",
                    entities: ["entity1"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_ABILITY_MISSION_TOP ,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.0000000003],
                        mykey2: "[a, b, c]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "STR", "helloopenharmonyhelloopenharmonyhelloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    console.info('====> ACTS_StartAbilityForResult_1900 start ability=====>' + JSON.stringify(data))
                    expect(data.want.deviceId).assertEqual("");
                    expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
                    expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
                    expect(data.want.action).assertEqual("action1");
                    expect(data.want.entities[0]).assertEqual("entity1");
                    expect(data.want.type).assertEqual("MIMETYPE");
                    expect(data.want.uri).assertEqual("key={true,true,false}");
                    expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITY_MISSION_TOP );
                    expect(data.want.parameters.mykey0).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
                    expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
                    expect(data.want.parameters.mykey1[2]).assertEqual(0.0000000003);
                    expect(data.want.parameters.mykey2).assertEqual("[a, b, c]");
                    expect(data.want.parameters.mykey3).assertEqual("str");
                    expect(data.want.parameters.mykey4[0]).assertEqual(false);
                    expect(data.want.parameters.mykey4[1]).assertEqual(true);
                    expect(data.want.parameters.mykey4[2]).assertEqual(false);
                    expect(data.want.parameters.mykey5[0]).assertEqual("str");
                    expect(data.want.parameters.mykey5[1]).assertEqual("STR");
                    expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmonyhelloopenharmonyhelloopenharmony");
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        })
        
     /*
     * @tc.number  ACTS_GetWant_2000
     * @tc.name    Misconfigured bundlename validation
     * @tc.desc    Function test
     * @tc.level   0
     */
    it("ACTS_GetWant_2000",0, async function(done){
        console.info("------------------start ACTS_GetWant_2000-------------------");
        try{
            featureAbility.startAbilityForResult({
                want:
                {
                    deviceId: "",
                    bundleName: "com.example.actsgetwantalltesthap",
                    abilityName: "com.example.actsgetwantalltesthap.MainAbility",
                    action: "action2",
                    entities: ["entity1", "entity2"],
                    type: "MIMETYPE",
                    uri: "key={true,true,false}",
                    flags: wantConstant.Flags.FLAG_ABILITY_FORWARD_RESULT,
                    parameters:
                    {
                        mykey0: 0.1,
                        mykey1: [0.1, 0.2, 0.3],
                        mykey2: "[1, 2, 3]",
                        mykey3: "str",
                        mykey4: [false, true, false],
                        mykey5: ["str", "test123", "helloopenharmony"],
                    },
                },
            },
                (err, data) => {
                    checkOnAbilityResult(data);
                    console.info('====> setTimeout before done.=====>')
                    setTimeout(()=>{
                        done();
                    }, TIMEOUT_NUMBER);
                })
        }catch(error){
            console.log("ACTS_GetWant_2000 : error = " + error);
        }
        console.log('ACTS_GetWant_2000 end');
    });

    /**
     * @tc.number: SUB_AA_OpenHarmony_wantConstantEnumeration_0100
     * @tc.name: Check the enumerated value ACTION_IMAGE_CAPTURE of wantConstant.
     * @tc.desc: Check the enumerated value ACTION_IMAGE_CAPTURE of wantConstant.
     */
     it('SUB_AA_OpenHarmony_wantConstantEnumeration_0100',0, async function (done) {
        console.log("SUB_AA_OpenHarmony_wantConstantEnumeration_0100 --- start")
        let image = wantConstant.Action.ACTION_IMAGE_CAPTURE
        expect(image).assertEqual("ohos.want.action.imageCapture")
        console.log("SUB_AA_OpenHarmony_wantConstantEnumeration_0100 --- end")
        done()
        })

        /**
         * @tc.number: SUB_AA_OpenHarmony_wantConstantEnumeration_0200
         * @tc.name: Check the enumerated value ACTION_VIDEO_CAPTURE of wantConstant.
         * @tc.desc: Check the enumerated value ACTION_VIDEO_CAPTURE of wantCconstant.
         */
        it('SUB_AA_OpenHarmony_wantConstantEnumeration_0200',0, async function (done) {
            console.log("SUB_AA_OpenHarmony_wantConstantEnumeration_0200 --- start")
            let video = wantConstant.Action.ACTION_VIDEO_CAPTURE
            expect(video).assertEqual("ohos.want.action.videoCapture")
            console.log("SUB_AA_OpenHarmony_wantConstantEnumeration_0200 --- end")
            done()
        })

        /**
         * @tc.number: SUB_AA_OpenHarmony_wantConstantEnumeration_0300
         * @tc.name: Check the enumerated value ACTION_APP_ACCOUNT_OAUTH of wantConstant.
         * @tc.desc: Check the enumerated value ACTION_APP_ACCOUNT_OAUTH of wantConstant.
         */
        it('SUB_AA_OpenHarmony_wantConstantEnumeration_0300',0, async function (done) {
        console.log("SUB_AA_OpenHarmony_wantConstantEnumeration_0300 --- start")
        let app = wantConstant.Action.ACTION_APP_ACCOUNT_OAUTH
        expect(app).assertEqual("ohos.account.appAccount.action.oauth")
        console.log("SUB_AA_OpenHarmony_wantConstantEnumeration_0300 --- end")
        done()
        })
    
    function checkOnAbilityResult(data) {
        console.info('====> ACTS_StartAbilityForResult_2000 start ability=====>' + JSON.stringify(data))
        expect(data.want.deviceId).assertEqual("");
        expect(data.want.bundleName).assertEqual("com.example.actsgetwantalltesthap");
        expect(data.want.abilityName).assertEqual("com.example.actsgetwantalltesthap.MainAbility");
        expect(data.want.action).assertEqual("action2");
        expect(data.want.entities[0]).assertEqual("entity1");
        expect(data.want.type).assertEqual("MIMETYPE");
        expect(data.want.uri).assertEqual("key={true,true,false}");
        expect(data.want.flags).assertEqual(wantConstant.Flags.FLAG_ABILITY_FORWARD_RESULT );
        expect(data.want.parameters.mykey0).assertEqual(0.1);
        expect(data.want.parameters.mykey1[0]).assertEqual(0.1);
        expect(data.want.parameters.mykey1[1]).assertEqual(0.2);
        expect(data.want.parameters.mykey1[2]).assertEqual(0.3);
        expect(data.want.parameters.mykey2).assertEqual("[1, 2, 3]");
        expect(data.want.parameters.mykey3).assertEqual("str");
        expect(data.want.parameters.mykey4[0]).assertEqual(false);
        expect(data.want.parameters.mykey4[1]).assertEqual(true);
        expect(data.want.parameters.mykey4[2]).assertEqual(false);
        expect(data.want.parameters.mykey5[0]).assertEqual("str");
        expect(data.want.parameters.mykey5[1]).assertEqual("test123");
        expect(data.want.parameters.mykey5[2]).assertEqual("helloopenharmony");
        console.info('====> before done=====>')
    }
})
