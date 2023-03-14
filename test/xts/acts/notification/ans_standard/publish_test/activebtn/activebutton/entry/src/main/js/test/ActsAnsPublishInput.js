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

import notify from '@ohos.notification';
import { OperationType, WantAgentFlags} from '@ohos.wantagent';
import wantagent from '@ohos.wantAgent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var timeout = 300;
describe('ActsAnsPublishInput', function () {
    console.info("===========ActsAnsPublishInput start====================>");

    function onConsumeOne(data) {
        console.info("=========Ans_PublishInput_0100 onConsume start==============>");
        console.info("=========Ans_PublishInput_0100 onConsume data:==============>" + JSON.stringify(data));
        var inputKey = data.request.actionButtons[0].userInput.inputKey
        var tag = data.request.actionButtons[0].userInput.tag
        var options = data.request.actionButtons[0].userInput.options
        var permitMimeTypes = data.request.actionButtons[0].userInput.permitMimeTypes
        var editType = data.request.actionButtons[0].userInput.editType
        var additionalData = data.request.actionButtons[0].userInput.additionalData
        console.info("=========Ans_PublishInput_0100 onConsume inputKey================>"+inputKey);
        console.info("=========Ans_PublishInput_0100 onConsume tag================>"+tag);
        console.info("=========Ans_PublishInput_0100 onConsume options================>"+options);
        console.info("=========Ans_PublishInput_0100 onConsume permitMimeTypes================>"+permitMimeTypes);
        console.info("=========Ans_PublishInput_0100 onConsume editType================>"+editType);
        expect(inputKey).assertEqual("inputKey_0100");
        expect(tag).assertEqual("tag_0100");
        expect(JSON.stringify(options)).assertEqual(JSON.stringify(["options_0100"]));
        expect(JSON.stringify(permitMimeTypes)).assertEqual(JSON.stringify(["permitMimeTypes_0100"]));
        expect(editType).assertEqual(1);
        expect(additionalData.key0100).assertEqual("0100");
        console.info("=========Ans_PublishInput_0100 onConsume end================>");
    }


    /*
     * @tc.number: Ans_PublishInput_0100
     * @tc.name: publish(request: NotificationRequest): Promise<void>;
     * @tc.desc: Verify that the inputKey information can be received in the received notification.
     */
    it('Ans_PublishInput_0100', 0, async function (done) {
        console.info("==================Ans_PublishInput_0100 start==================>");
        var subscriber = {
            onConsume:onConsumeOne,
        }
        var agentInfo = {
            wants: [
                    {
                        deviceId: "deviceId",
                        bundleName: "com.example.actsanspublishinput",
                        abilityName: "com.example.actsanspublishinput.MainAbility",
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
        var wantAgentData = await wantagent.getWantAgent(agentInfo);
        console.info("===========Ans_PublishInput_0100 getWantAgent promise======>");
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
            wantAgent: wantAgentData,
            actionButtons: [
                    {
                        title:"button1",
                        wantAgent:wantAgentData,
                        userInput : {
                            inputKey: "inputKey_0100",
                            tag: "tag_0100",
                            options: ["options_0100"],
                            permitMimeTypes:["permitMimeTypes_0100"],
                            editType:1,
                            additionalData:{"key0100":"0100"}
                        }
                    }]
        }
        await notify.subscribe(subscriber);
        console.info("===========Ans_PublishInput_0100 subscribe promise======>");
        await notify.publish(notificationRequest);
        console.info("===========Ans_PublishInput_0100 publish promise========>");
        setTimeout((async function(){
            console.info("======Ans_PublishInput_0100 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======Ans_PublishInput_0100 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })
})