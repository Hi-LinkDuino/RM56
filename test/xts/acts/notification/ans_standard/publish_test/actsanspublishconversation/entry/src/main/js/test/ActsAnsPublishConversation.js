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
import image from '@ohos.multimedia.image'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var timeout = 500;
describe('ActsAnsConversationTest', function () {
    console.info("===========ActsAnsConversationTest start====================>");

    function onConsumeOne(data) {
        console.info("=========Ans_Conversation_0100 onConsume start==============>");
        console.info("=========Ans_Conversation_0100 onConsume data:==============>" + JSON.stringify(data));

        expect(data.request.content.contentType).assertEqual(notify.ContentType.NOTIFICATION_CONTENT_CONVERSATION);
        expect(data.request.content.conversation.title).assertEqual("test_title");
        expect(data.request.content.conversation.text).assertEqual("test_text");
        expect(data.request.content.conversation.additionalText).assertEqual("test_additionalText");
        expect(data.request.content.conversation.conversationTitle).assertEqual("conversationTitle_0100");
        expect(data.request.content.conversation.conversationGroup).assertEqual(true);

        expect(data.request.content.conversation.messages[0].text).assertEqual("conversation_text_1");
        expect(data.request.content.conversation.messages[0].timestamp).assertEqual(1);
        expect(data.request.content.conversation.messages[0].sender.name).assertEqual("sender_name_1");
        expect(data.request.content.conversation.messages[0].sender.key).assertEqual("sender_key_1");
        expect(data.request.content.conversation.messages[0].sender.uri).assertEqual("sender_uri_1");
        expect(data.request.content.conversation.messages[0].sender.isMachine).assertEqual(true);
        expect(data.request.content.conversation.messages[0].sender.isUserImportant).assertEqual(true);
        expect(data.request.content.conversation.messages[0].mimeType).assertEqual("conversation_mimeType_1");
        expect(data.request.content.conversation.messages[0].uri).assertEqual("conversation_uri_1");

        expect(data.request.content.conversation.messages[1].text).assertEqual("conversation_text_2");
        expect(data.request.content.conversation.messages[1].timestamp).assertEqual(1);
        expect(data.request.content.conversation.messages[1].sender.name).assertEqual("sender_name_2");
        expect(data.request.content.conversation.messages[1].sender.key).assertEqual("sender_key_2");
        expect(data.request.content.conversation.messages[1].sender.uri).assertEqual("sender_uri_2");
        expect(data.request.content.conversation.messages[1].sender.isMachine).assertEqual(true);
        expect(data.request.content.conversation.messages[1].sender.isUserImportant).assertEqual(true);
        expect(data.request.content.conversation.messages[1].mimeType).assertEqual("conversation_mimeType_2");
        expect(data.request.content.conversation.messages[1].uri).assertEqual("conversation_uri_2");

        console.info("=========Ans_Conversation_0100 onConsume end================>");
    }

    /*
     * @tc.number: Ans_Conversation_0100
     * @tc.name: publish(request: NotificationRequest): Promise<void>;
     * @tc.desc: Verify that the conversation information can be received in the received notification.
     */
    it('Ans_Conversation_0100', 0, async function (done) {
        console.info("==================Ans_Conversation_0100 start==================>");
        var bufferA = new ArrayBuffer(4*2*25);
        var optionA = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 25, width: 2}}
        var conversationIconA = await image.createPixelMap(bufferA, optionA);
        console.info("===========Ans_Conversation_0100 createPixelMap A promise======>");
        var bufferB = new ArrayBuffer(4*2*24);
        var optionB = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 24, width: 2}}
        var conversationIconB = await image.createPixelMap(bufferB, optionB);
        console.info("===========Ans_Conversation_0100 createPixelMap B promise======>");
        var bufferC = new ArrayBuffer(4*2*23);
        var optionC = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 23, width: 2}}
        var conversationIconC = await image.createPixelMap(bufferC, optionC);
        console.info("===========Ans_Conversation_0100 createPixelMap C promise======>");
        var subscriber = {
            onConsume:onConsumeOne,
        }
        await notify.subscribe(subscriber);
        console.info("==================Ans_Conversation_0100 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_CONVERSATION,
                conversation: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    user:{
                        name:"user_name",
                        key: "user_key",
                        uri: "user_uri",
                        isMachine:true,
                        isUserImportant:true,
                        icon:conversationIconA
                    },
                    messages:[
                            {
                                  text:"conversation_text_1",
                                  timestamp:1,
                                  sender:{
                                      name:"sender_name_1",
                                      key: "sender_key_1",
                                      uri: "sender_uri_1",
                                      isMachine:true,
                                      isUserImportant:true,
                                      icon:conversationIconB
                                  },
                                  mimeType:"conversation_mimeType_1",
                                  uri:"conversation_uri_1",
                            },
                            {
                                text:"conversation_text_2",
                                timestamp:1,
                                sender:{
                                    name:"sender_name_2",
                                    key: "sender_key_2",
                                    uri: "sender_uri_2",
                                    isMachine:true,
                                    isUserImportant:true,
                                    icon:conversationIconC
                                },
                                mimeType:"conversation_mimeType_2",
                                uri:"conversation_uri_2",
                            },],
                    conversationGroup:true,
                    conversationTitle:"conversationTitle_0100",
                },
            },
        }
        await notify.publish(notificationRequest);
        console.info("===========Ans_Conversation_0100 publish promise========>");
        setTimeout((async function(){
            console.info("======Ans_Conversation_0100 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======Ans_Conversation_0100 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })
})