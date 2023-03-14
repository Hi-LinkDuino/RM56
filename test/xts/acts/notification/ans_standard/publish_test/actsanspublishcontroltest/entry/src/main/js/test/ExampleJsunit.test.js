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
import notification from '@ohos.notification'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

var notificationSubscriberInfo ={
    bundleNames: ["com.example.actsanspublishcontroltest"]
}

var notificationSubscriber = {
    onConsume: consumeCallback,
    onConnect: subscribeOnCallback
}

var idRecord = new Array(20).fill(0);
const publishFrequence = 10;
const TIMEOUT = 3000;

function consumeCallback(data) {
    console.debug("====>consumeCallback data: ====>" + JSON.stringify(data));
    console.debug("====>consumeCallback id: ====>" + data.request.id);
    switch(data.request.id){
        case 1:
            idRecord[0] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 2:
            idRecord[1] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 3:
            idRecord[2] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 4:
            idRecord[3] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 5:
            idRecord[4] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 6:
            idRecord[5] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 7:
            idRecord[6] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 8:
            idRecord[7] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 9:
            idRecord[8] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 10:
            idRecord[9] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 11:
            idRecord[10] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 12:
            idRecord[11] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 13:
            idRecord[12] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 14:
            idRecord[13] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 15:
            idRecord[14] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 16:
            idRecord[15] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 17:
            idRecord[16] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 18:
            idRecord[17] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 19:
            idRecord[18] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;
        case 20:
            idRecord[19] = 1;
            console.debug("====>id is recorded: " + data.request.id);
            break;

        default:
            console.debug("====>fail enter default====>");
            expect().assertFail();
            break;
    }
}

function subscribeOnCallback() {
    console.debug("====>subscribeOnCallback enter====>");
}

function subscribeCallback(err) {
    console.debug("====>subscribeCallback err: ====>" + JSON.stringify(err));
    expect(err.code).assertEqual(0);
}

function publish_the_first_notification(err){
    console.debug("====>publish the first notification 11111 err: ====>" + JSON.stringify(err));
}
function publish_the_second_notification(err){
    console.debug("====>publish the second notification 22222 err: ====>" + JSON.stringify(err));
}
function publish_the_third_notification(err){
    console.debug("====>publish the third notification 33333 err: ====>" + JSON.stringify(err));
}
function publish_the_fourth_notification(err){
    console.debug("====>publish the fourth  notification 44444 err: ====>" + JSON.stringify(err));
}
function publish_the_fifth_notification(err){
    console.debug("====>publish the fifth notification 55555 err: ====>" + JSON.stringify(err));
}
function publish_the_sixth_notification(err){
    console.debug("====>publish the sixth notification 66666 err: ====>" + JSON.stringify(err));
}
function publish_the_seventh_notification(err){
    console.debug("====>publish the seventh notification 77777 err: ====>" + JSON.stringify(err));
}
function publish_the_eighth_notification(err){
    console.debug("====>publish the eighth notification 88888 err: ====>" + JSON.stringify(err));
}
function publish_the_ninth_notification(err){
    console.debug("====>publish the ninth notification 99999 err: ====>" + JSON.stringify(err));
}
function publish_the_tenth_notification(err){
    console.debug("====>publish the tenth notification 10 10 err: ====>" + JSON.stringify(err));
}
function publish_the_eleventh_notification(err){
    console.debug("====>publish the eleventh notification 11 11 err: ====>" + JSON.stringify(err));
}
function publish_the_twelfth_notification(err){
    console.debug("====>publish the twelfth notification 12 12 err: ====>" + JSON.stringify(err));
}
function publish_the_thirteenth_notification(err){
    console.debug("====>publish the thirteenth notification 13 13 err: ====>" + JSON.stringify(err));
}
function publish_the_fourteenth_notification(err){
    console.debug("====>publish the fourteenth notification 14 14 err: ====>" + JSON.stringify(err));
}
function publish_the_fifteenth_notification(err){
    console.debug("====>publish the fifteenth notification 15 15 err: ====>" + JSON.stringify(err));
}
function publish_the_sixteenth_notification(err){
    console.debug("====>publish the sixteenth notification 16 16 err: ====>" + JSON.stringify(err));
}
function publish_the_seventeenth_notification(err){
    console.debug("====>publish the seventeenth notification 17 17 err: ====>" + JSON.stringify(err));
}
function publish_the_eighteenth_notification(err){
    console.debug("====>publish the eighteenth notification 18 18 err: ====>" + JSON.stringify(err));
}
function publish_the_nineteenth_notification(err){
    console.debug("====>publish the nineteenth notification 19 19 err: ====>" + JSON.stringify(err));
}

describe('ActsAnsPublishControlTest', function () {

    /*
     * @tc.number    : ActsAnsPublishControlTest_0100
     * @tc.name      : Verify rejection of publishing notifications that exceed the threshold
     * @tc.desc      : Twenty notifications are published continuously within one second, and the first
     *                 ten notifications can be received, the last ten notifications cannot be received
     */
    it('ActsAnsPublishControlTest_0100', 0, async function (done) {
        console.debug("====>ActsAnsPublishControlTest_0100 start====>");
        try{
            await notification.subscribe(notificationSubscriber, notificationSubscriberInfo, subscribeCallback);
        }catch(err) {
            console.error("====>ActsAnsPublishControlTest_0100 subscribe fail err:" + JSON.stringify(err));
        }

        function timeOut(){
            console.debug("====>time out enter====>");
            var sumOne = 0;
            var sumTwo = 0;
            for (let i = 0; i <= publishFrequence - 1; i++) {
                sumOne += idRecord[i];
            }
            for (let i = publishFrequence; i < 20; i++) {
                sumTwo += idRecord[i];
            }
            console.debug("====>sumOne====>" + sumOne);
            console.debug("====>sumTwo====>" + sumTwo);
            expect(sumOne).assertEqual(10);
            expect(sumTwo).assertEqual(0);
            console.debug("====>ActsAnsPublishControlTest_0100 end====>");
            done();
        }

        function publish_the_twentieth_notification(err){
            console.debug("====>publish the twentieth notification 20 20 err: ====>" + JSON.stringify(err));
            console.debug("====>time out start====>");
            setTimeout(timeOut, TIMEOUT);
        }

        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test1_title",
                    text: "test1_text",
                    additionalText: "test1_additionalText"
                },
            },
            id: 1
        }, publish_the_first_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test2_title",
                    text: "test2_text",
                    additionalText: "test2_additionalText"
                },
            },
            id: 2
        }, publish_the_second_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test3_title",
                    text: "test3_text",
                    additionalText: "test3_additionalText"
                },
            },
            id: 3
        }, publish_the_third_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test4_title",
                    text: "test4_text",
                    additionalText: "test4_additionalText"
                },
            },
            id: 4
        }, publish_the_fourth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test5_title",
                    text: "test5_text",
                    additionalText: "test5_additionalText"
                },
            },
            id: 5
        }, publish_the_fifth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test6_title",
                    text: "test6_text",
                    additionalText: "test6_additionalText"
                },
            },
            id: 6
        }, publish_the_sixth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test7_title",
                    text: "test7_text",
                    additionalText: "test7_additionalText"
                },
            },
            id: 7
        }, publish_the_seventh_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test8_title",
                    text: "test8_text",
                    additionalText: "test8_additionalText"
                },
            },
            id: 8
        }, publish_the_eighth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test9_title",
                    text: "test9_text",
                    additionalText: "test9_additionalText"
                },
            },
            id: 9
        }, publish_the_ninth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test10_title",
                    text: "test10_text",
                    additionalText: "test10_additionalText"
                },
            },
            id: 10
        }, publish_the_tenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test11_title",
                    text: "test11_text",
                    additionalText: "test11_additionalText"
                },
            },
            id: 11
        }, publish_the_eleventh_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test12_title",
                    text: "test12_text",
                    additionalText: "test12_additionalText"
                },
            },
            id: 12
        }, publish_the_twelfth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test13_title",
                    text: "test13_text",
                    additionalText: "test13_additionalText"
                },
            },
            id: 13
        }, publish_the_thirteenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test14_title",
                    text: "test14_text",
                    additionalText: "test14_additionalText"
                },
            },
            id: 14
        }, publish_the_fourteenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test15_title",
                    text: "test15_text",
                    additionalText: "test15_additionalText"
                },
            },
            id: 15
        }, publish_the_fifteenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test16_title",
                    text: "test16_text",
                    additionalText: "test11_additionalText"
                },
            },
            id: 16
        }, publish_the_sixteenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test17_title",
                    text: "test17_text",
                    additionalText: "test17_additionalText"
                },
            },
            id: 17
        }, publish_the_seventeenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test18_title",
                    text: "test18_text",
                    additionalText: "test18_additionalText"
                },
            },
            id: 18
        }, publish_the_eighteenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test19_title",
                    text: "test19_text",
                    additionalText: "test19_additionalText"
                },
            },
            id: 19
        }, publish_the_nineteenth_notification);
        notification.publish({
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test20_title",
                    text: "test20_text",
                    additionalText: "test20_additionalText"
                },
            },
            id: 20
        }, publish_the_twentieth_notification);
    })
})