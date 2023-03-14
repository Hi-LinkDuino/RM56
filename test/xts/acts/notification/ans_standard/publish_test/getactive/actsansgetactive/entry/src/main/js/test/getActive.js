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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var time = 300
var ERR_ANS_NON_SYSTEM_APP = 67108877
describe('ActsAnsActiveTest', function () {
    console.info("===========ActsAnsActiveTest start====================>");
    function getCallback(err, data){
        console.log("Ans_GetActive_0100 getCallback ============>");
        var i;
        console.log("Ans_GetActive_0100 getCallback  data.length============>"+data.length);
        console.log("Ans_GetActive_0100 getCallback  data============>"+JSON.stringify(data));
        for (i = 0; i < data.length; i++) {
            expect(data[i].content.contentType).assertEqual(notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT);
            console.log("Ans_GetActive_0100 getCallback contentType============>"+data[i].content.contentType)
            expect(data[i].content.normal.title).assertEqual("test_title");
            console.log("=========Ans_GetActive_0100 getCallback title============>"+data[i].content.normal.title)
            expect(data[i].content.normal.text).assertEqual("test_text");
            console.log("==========Ans_GetActive_0100 getCallback text============>"+data[i].content.normal.text)
            expect(data[i].content.normal.additionalText).assertEqual("test_additionalText");
            console.log("========Ans_GetActive_0100 getCallback text========>"+data[i].content.normal.additionalText)
            expect(data[i].id).assertEqual(1);
            console.log("============Ans_GetActive_0100 getCallback id============>"+data[i].id)
            expect(data[i].slotType).assertEqual(notify.SlotType.OTHER_TYPES);
            console.log("============Ans_GetActive_0100 getCallback slotType============>"+data[i].slotType)
            expect(data[i].deliveryTime).assertEqual(1624950453);
            console.log("============Ans_GetActive_0100 getCallback deliveryTime========>"+data[i].deliveryTime)
            expect(data[i].autoDeletedTime).assertEqual(1625036817);
            console.log("===========Ans_GetActive_0100 getCallback autoDeletedTime=====>"+data[i].autoDeletedTime)
            expect(data[i].statusBarText).assertEqual("statusBarText");
            console.log("============Ans_GetActive_0100 getCallback statusBarText=====>"+data[i].statusBarText)
            expect(data[i].label).assertEqual("0100");
            console.log("============Ans_GetActive_0100 getCallback label=====>"+data[i].label)
            expect(data[i].badgeIconStyle).assertEqual(1);
            console.log("============Ans_GetActive_0100 getCallback badgeIconStyle=====>"+data[i].badgeIconStyle)
        }
    }

    /*
    * @tc.number: Ans_GetActive_0100
    * @tc.name: getActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: after publishing a notification, get all active notification info(callback)
    */
    it('Ans_GetActive_0100', 0, async function (done) {
        console.debug("===============Ans_GetActive_0100 start==================>");
        await notify.cancelAll();
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 1,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            statusBarText: "statusBarText",
            label: "0100",
            badgeIconStyle: 1,
        }
        await notify.publish(notificationRequest);
        console.debug("===============Ans_GetActive_0100 publish end==================>");
        notify.getActiveNotifications(getCallback);
        console.debug("===============Ans_GetActive_0100 getActiveNotifications end==================>");
        setTimeout(async function(){
            console.debug("===============Ans_GetActive_0100 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_0200
    * @tc.name: getActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: after publishing a notification, get all active notification info(promise)
    */
    it('Ans_GetActive_0200', 0, async function (done) {
        console.debug("===============Ans_GetActive_0200 start==================>");
        await notify.cancelAll();
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 2,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            color: 2,
            statusBarText: "statusBarText",
            label: "0200",
            badgeIconStyle: 1,
        }
        await notify.publish(notificationRequest);
        console.debug("===============Ans_GetActive_0200 publish end==================>");
        var promiseData = await notify.getActiveNotifications();
        var i;
        for (i = 0; i < promiseData.length; i++) {
            expect(promiseData[i].content.contentType).assertEqual(notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT);
            console.log("Ans_GetActive_0200 getCallback contentType============>"+promiseData[i].content.contentType)
            expect(promiseData[i].content.normal.title).assertEqual("test_title");
            console.log("====Ans_GetActive_0200 getCallback title============>"+promiseData[i].content.normal.title)
            expect(promiseData[i].content.normal.text).assertEqual("test_text");
            console.log("=====Ans_GetActive_0200 getCallback text============>"+promiseData[i].content.normal.text)
            expect(promiseData[i].content.normal.additionalText).assertEqual("test_additionalText");
            console.log("==Ans_GetActive_0200 getCallback text========>"+promiseData[i].content.normal.additionalText)
            expect(promiseData[i].id).assertEqual(2);
            console.log("============Ans_GetActive_0200 getCallback id============>"+promiseData[i].id)
            expect(promiseData[i].slotType).assertEqual(notify.SlotType.OTHER_TYPES);
            console.log("============Ans_GetActive_0200 getCallback slotType============>"+promiseData[i].slotType)
            expect(promiseData[i].deliveryTime).assertEqual(1624950453);
            console.log("=======Ans_GetActive_0200 getCallback deliveryTime========>"+promiseData[i].deliveryTime)
            expect(promiseData[i].autoDeletedTime).assertEqual(1625036817);
            console.log("=======Ans_GetActive_0200 getCallback autoDeletedTime=====>"+promiseData[i].autoDeletedTime)
            //            expect(data[i].color).assertEqual(2);
            console.log("============Ans_GetActive_0200 getCallback color=====>"+promiseData[i].color)
            expect(promiseData[i].statusBarText).assertEqual("statusBarText");
            console.log("=======Ans_GetActive_0200 getCallback statusBarText=====>"+promiseData[i].statusBarText)
            expect(promiseData[i].label).assertEqual("0200");
            console.log("============Ans_GetActive_0200 getCallback label=====>"+promiseData[i].label)
            expect(promiseData[i].badgeIconStyle).assertEqual(1);
            console.log("=======Ans_GetActive_0200 getCallback badgeIconStyle=====>"+promiseData[i].badgeIconStyle)
        }
        console.debug("===============Ans_GetActive_0200 getActiveNotifications end==================>");
        setTimeout(async function(){
            console.debug("===============Ans_GetActive_0200 done==================>");
            done();
        }, time);
    })

    function getCallbackTwo(err, data){
        console.log("Ans_GetActive_0300 getCallback  data.length============>"+data.length);
        expect(data.length).assertEqual(2);
        console.log("Ans_GetActive_0300 getCallback  data============>"+JSON.stringify(data));
        var i;
        for (i = 0; i < data.length; i++) {
            if (i == 0){
                expect(data[i].content.normal.title).assertEqual("test_title_1");
                console.log("==========Ans_GetActive_0300 getCallback title=========>"+data[i].content.normal.title)
                expect(data[i].content.normal.text).assertEqual("test_text_1");
                console.log("==========Ans_GetActive_0300 getCallback text============>"+data[i].content.normal.text)
                expect(data[i].content.normal.additionalText).assertEqual("test_additionalText_1");
                console.log("======Ans_GetActive_0300 getCallback text=======>"+data[i].content.normal.additionalText)
                expect(data[i].id).assertEqual(1);
                console.log("============Ans_GetActive_0300 getCallback id============>"+data[i].id)
                expect(data[i].label).assertEqual("0300_1");
                console.log("============Ans_GetActive_0300 getCallback label=====>"+data[i].label)
            }else if(i == 1){
                expect(data[i].content.normal.title).assertEqual("test_title_2");
                console.log("==========Ans_GetActive_0300 getCallback title=========>"+data[i].content.normal.title)
                expect(data[i].content.normal.text).assertEqual("test_text_2");
                console.log("==========Ans_GetActive_0300 getCallback text============>"+data[i].content.normal.text)
                expect(data[i].content.normal.additionalText).assertEqual("test_additionalText_2");
                console.log("======Ans_GetActive_0300 getCallback text=======>"+data[i].content.normal.additionalText)
                expect(data[i].id).assertEqual(2);
                console.log("============Ans_GetActive_0300 getCallback id============>"+data[i].id)
                expect(data[i].label).assertEqual("0300_2");
                console.log("============Ans_GetActive_0300 getCallback label=====>"+data[i].label)
            }
        }
    }

    /*
    * @tc.number: Ans_GetActive_0300
    * @tc.name: getActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: after publishing two notifications, get all active notifications info(callback)
    */
    it('Ans_GetActive_0300', 0, async function (done) {
        console.debug("===============Ans_GetActive_0300 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "0300_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "0300_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_0300 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_0300 publish2 end==================>");
        notify.getActiveNotifications(getCallbackTwo);
        console.debug("===============Ans_GetActive_0300 getActiveNotifications end==================>");
        setTimeout(async function(){
            console.debug("===============Ans_GetActive_0300 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_0400
    * @tc.name: getActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: after publishing two notifications, get all active notifications info(promise)
    */
    it('Ans_GetActive_0400', 0, async function (done) {
        console.debug("===============Ans_GetActive_0400 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "0400_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "0400_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_0400 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_0400 publish2 end==================>");
        var promiseData = await notify.getActiveNotifications();
        expect(promiseData.length).assertEqual(2);
        var i;
        for (i = 0; i < promiseData.length; i++) {
            if (i == 0){
                expect(promiseData[i].content.normal.title).assertEqual("test_title_1");
                console.log("====Ans_GetActive_0400 getCallback title=====>"+promiseData[i].content.normal.title)
                expect(promiseData[i].content.normal.text).assertEqual("test_text_1");
                console.log("===Ans_GetActive_0400 getCallback text========>"+promiseData[i].content.normal.text)
                expect(promiseData[i].content.normal.additionalText).assertEqual("test_additionalText_1");
                console.log("Ans_GetActive_0400 getCallback text=====>"+promiseData[i].content.normal.additionalText)
                expect(promiseData[i].id).assertEqual(1);
                console.log("============Ans_GetActive_0400 getCallback id============>"+promiseData[i].id)
                expect(promiseData[i].label).assertEqual("0400_1");
                console.log("============Ans_GetActive_0400 getCallback label=====>"+promiseData[i].label)
            }else if(i == 1){
                expect(promiseData[i].content.normal.title).assertEqual("test_title_2");
                console.log("===Ans_GetActive_0400 getCallback title=========>"+promiseData[i].content.normal.title)
                expect(promiseData[i].content.normal.text).assertEqual("test_text_2");
                console.log("====Ans_GetActive_0400 getCallback text============>"+promiseData[i].content.normal.text)
                expect(promiseData[i].content.normal.additionalText).assertEqual("test_additionalText_2");
                console.log("Ans_GetActive_0400 getCallback text=====>"+promiseData[i].content.normal.additionalText)
                expect(promiseData[i].id).assertEqual(2);
                console.log("============Ans_GetActive_0400 getCallback id============>"+promiseData[i].id)
                expect(promiseData[i].label).assertEqual("0400_2");
                console.log("============Ans_GetActive_0400 getCallback label=====>"+promiseData[i].label)
            }
        }
        console.debug("===============Ans_GetActive_0400 getActiveNotifications end==================>");
        setTimeout(async function(){
            console.debug("===============Ans_GetActive_0400 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_0500
    * @tc.name: getActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: before publishing any notifications, no active notifications can be get(callback)
    */
    it('Ans_GetActive_0500', 0, async function (done) {
        console.debug("===============Ans_GetActive_0500 start==================>");
        await notify.cancelAll();
        notify.getActiveNotifications((err, data)=>{
            console.log("Ans_GetActive_0500 getCallback data.length============>" + data.length);
            console.log("Ans_GetActive_0500 getCallback JSON.stringify(data)============>" + JSON.stringify(data));
            expect(data.length).assertEqual(0);
            setTimeout(function(){
                console.debug("===============Ans_GetActive_0500 done==================>");
                done();
            }, time);
        });
    })

    /*
    * @tc.number: Ans_GetActive_0600
    * @tc.name: getActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: before publishing any notifications, no active notifications can be get(promise)
    */
    it('Ans_GetActive_0600', 0, async function (done) {
        console.debug("===============Ans_GetActive_0600 start==================>");
        await notify.cancelAll();
        var promiseData = await notify.getActiveNotifications();
        console.debug("===============Ans_GetActive_0600 getActiveNotifications end==================>");
        expect(promiseData.length).assertEqual(0);
        console.debug("=========Ans_GetActive_0600 promiseData.length=============>"+promiseData.length);
        console.debug("==Ans_GetActivcae_0600 JSON.stringify(promiseData)========>"+JSON.stringify(promiseData));
        setTimeout(function(){
            console.debug("===============Ans_GetActive_0600 done==================>");
            done();
        }, time);
    })

    function getCallbackFour(err, data){
        console.log("Ans_GetActive_0700 getCallback ============>");
        console.log("Ans_GetActive_0700 getCallback  data.length============>"+data.length);
        console.log("Ans_GetActive_0700 getCallback  data============>"+JSON.stringify(data));
        expect(data.length).assertEqual(1);
        var i;
        for (i = 0; i < data.length; i++) {
            expect(data[i].content.normal.title).assertEqual("test_title_2");
            console.log("==========Ans_GetActive_0700 getCallback title=========>"+data[i].content.normal.title)
            expect(data[i].content.normal.text).assertEqual("test_text_2");
            console.log("==========Ans_GetActive_0700 getCallback text============>"+data[i].content.normal.text)
            expect(data[i].content.normal.additionalText).assertEqual("test_additionalText_2");
            console.log("======Ans_GetActive_0700 getCallback text=======>"+data[i].content.normal.additionalText)
            expect(data[i].id).assertEqual(2);
            console.log("============Ans_GetActive_0700 getCallback id============>"+data[i].id)
            expect(data[i].label).assertEqual("0700_2");
            console.log("============Ans_GetActive_0700 getCallback label=====>"+data[i].label)
        }
    }

    /*
    * @tc.number: Ans_GetActive_0700
    * @tc.name: getActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: after publishing two notifications,
                cancel one of the notifications, get all active notifications info(callback)
    */
    it('Ans_GetActive_0700', 0, async function (done) {
        console.debug("===============Ans_GetActive_0700 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "0700_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "0700_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_0700 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_0700 publish2 end==================>");
        await notify.cancel(1, "0700_1");
        console.debug("===============Ans_GetActive_0700 cancel end==================>");
        notify.getActiveNotifications(getCallbackFour);
        console.debug("===============Ans_GetActive_0700 getActiveNotifications end==================>");
        setTimeout(async function(){
            console.debug("===============Ans_GetActive_0700 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_0800
    * @tc.name: getActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: after publishing two notifications,
                cancel one of the notifications, get all active notifications info(promise)
    */
    it('Ans_GetActive_0800', 0, async function (done) {
        console.debug("===============Ans_GetActive_0800 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "0800_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "0800_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_0800 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_0800 publish2 end==================>");
        await notify.cancel(1, "0800_1");
        console.debug("===============Ans_GetActive_0700 cancel end==================>");
        var promiseData = await notify.getActiveNotifications();
        var i;
        for (i = 0; i < promiseData.length; i++) {
            expect(promiseData[i].content.normal.title).assertEqual("test_title_2");
            console.log("======Ans_GetActive_0800 getCallback title=========>"+promiseData[i].content.normal.title)
            expect(promiseData[i].content.normal.text).assertEqual("test_text_2");
            console.log("======Ans_GetActive_0800 getCallback text============>"+promiseData[i].content.normal.text)
            expect(promiseData[i].content.normal.additionalText).assertEqual("test_additionalText_2");
            console.log("==Ans_GetActive_0800 getCallback text=======>"+promiseData[i].content.normal.additionalText)
            expect(promiseData[i].id).assertEqual(2);
            console.log("============Ans_GetActive_0800 getCallback id============>"+promiseData[i].id)
            expect(promiseData[i].label).assertEqual("0800_2");
            console.log("============Ans_GetActive_0800 getCallback label=====>"+promiseData[i].label)
        }
        console.debug("===============Ans_GetActive_0800 getActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_0800 done==================>");
            done();
        }, time);
    })

    function getCallbackFive(err, data){
        console.log("Ans_GetActive_0900 getCallback ============>");
        console.log("Ans_GetActive_0900 getCallback  data.length============>"+data.length);
        console.log("Ans_GetActive_0900 getCallback  data============>"+JSON.stringify(data));
        expect(data.length).assertEqual(0);
    }

    /*
    * @tc.number: Ans_GetActive_0900
    * @tc.name: getActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: after publishing two notifications,
                cancel all the notifications, get all active notifications info(callback)
    */
    it('Ans_GetActive_0900', 0, async function (done) {
        console.debug("===============Ans_GetActive_0900 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "0900_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "0900_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_0900 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_0900 publish2 end==================>");
        await notify.cancelAll();
        console.debug("===============Ans_GetActive_0900 cancelAll end==================>");
        notify.getActiveNotifications(getCallbackFive);
        console.debug("===============Ans_GetActive_0900 getActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_0900 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_1000
    * @tc.name: getActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: after publishing two notifications,
                cancel all the notifications, get all active notifications info(promise)
    */
    it('Ans_GetActive_1000', 0, async function (done) {
        console.debug("===============Ans_GetActive_1000 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "1000_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "1000_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_1000 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_1000 publish2 end==================>");
        await notify.cancelAll();
        console.debug("===============Ans_GetActive_1000 cancelAll end==================>");
        var promiseData = await notify.getActiveNotifications();
        console.log("Ans_GetActive_1000 getCallback  data.length============>"+promiseData.length);
        console.log("Ans_GetActive_1000 getCallback  data============>"+JSON.stringify(promiseData));
        expect(promiseData.length).assertEqual(0);
        console.debug("===============Ans_GetActive_1000 getActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1000 done==================>");
            done();
        }, time);
    })

    function getCountCallback(err, count){
        console.debug("===============Ans_GetActive_1100 getNumCallback count==================>"+count);
        expect(count).assertEqual(1);
    }

    /*
    * @tc.number: Ans_GetActive_1100
    * @tc.name: getActiveNotificationCount(callback: AsyncCallback<number>): void;
    * @tc.desc: Verify: after publishing a notification, get active notification count(callback)
    */
    it('Ans_GetActive_1100', 0, async function (done) {
        console.debug("===============Ans_GetActive_1100 start==================>");
        await notify.cancelAll();
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 1,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            color: 2,
            statusBarText: "statusBarText",
            label: "1100",
            badgeIconStyle: 1,
        }
        await notify.publish(notificationRequest);
        console.debug("===============Ans_GetActive_1100 publish end==================>");
        notify.getActiveNotificationCount(getCountCallback);
        console.debug("===============Ans_GetActive_1100 getActiveNotificationCount end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1100 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_1200
    * @tc.name: getActiveNotificationCount(): Promise<number>;
    * @tc.desc: Verify: after publishing a notification, get active notification number(promise)
    */
    it('Ans_GetActive_1200', 0, async function (done) {
        console.debug("===============Ans_GetActive_1200 start==================>");
        await notify.cancelAll();
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 1,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            color: 2,
            statusBarText: "statusBarText",
            label: "1200",
            badgeIconStyle: 1,
        }
        await notify.publish(notificationRequest);
        console.debug("===============Ans_GetActive_1200 publish end==================>");
        var promiseCount = await notify.getActiveNotificationCount();
        expect(promiseCount).assertEqual(1);
        console.debug("===============Ans_GetActive_1200 promiseCount==================>"+promiseCount);
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1200 done==================>");
            done();
        }, time);
    })


    function getCountCallbackTwo(err, count){
        console.debug("===============Ans_GetActive_1300 getCountCallbackTwo count==================>"+count);
        expect(count).assertEqual(2);
    }

    /*
    * @tc.number: Ans_GetActive_1300
    * @tc.name: getActiveNotificationCount(callback: AsyncCallback<number>): void;
    * @tc.desc: Verify: after publishing two notifications, get active notification count(callback)
    */
    it('Ans_GetActive_1300', 0, async function (done) {
        console.debug("===============Ans_GetActive_1300 start==================>");
        await notify.cancelAll();
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 1,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            color: 2,
            statusBarText: "statusBarText",
            label: "1300_1",
            badgeIconStyle: 1,
        }
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 2,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            color: 2,
            statusBarText: "statusBarText",
            label: "1300_2",
            badgeIconStyle: 1,
        }
        await notify.publish(notificationRequest);
        console.debug("===============Ans_GetActive_1300 publish end==================>");
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_1300 publish end==================>");
        notify.getActiveNotificationCount(getCountCallbackTwo);
        console.debug("===============Ans_GetActive_1300 getActiveNotificationCount end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1300 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_1400
    * @tc.name: getActiveNotificationCount(): Promise<number>;
    * @tc.desc: Verify: after publishing two notifications, get active notification number(promise)
    */
    it('Ans_GetActive_1400', 0, async function (done) {
        console.debug("===============Ans_GetActive_1400 start==================>");
        await notify.cancelAll();
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 1,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            color: 2,
            statusBarText: "statusBarText",
            label: "1400_1",
            badgeIconStyle: 1,
        }
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 2,
            slotType : notify.SlotType.OTHER_TYPES,
            deliveryTime : 1624950453,
            autoDeletedTime: 1625036817,
            color: 2,
            statusBarText: "statusBarText",
            label: "1400_2",
            badgeIconStyle: 1,
        }
        await notify.publish(notificationRequest);
        console.debug("===============Ans_GetActive_1400 publish1 end==================>");
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_1400 publish2 end==================>");
        var promiseCount = await notify.getActiveNotificationCount();
        expect(promiseCount).assertEqual(2);
        console.debug("===============Ans_GetActive_1400 promiseCount==================>"+promiseCount);
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1400 done==================>");
            done();
        }, time);
    })

    function getCountCallbackThree(err, count){
        console.debug("===============Ans_GetActive_1500 getCountCallbackThree count==================>"+count);
        expect(count).assertEqual(0);
    }

    /*
    * @tc.number: Ans_GetActive_1500
    * @tc.name: getActiveNotificationCount(callback: AsyncCallback<number>): void;
    * @tc.desc: Verify: before publishing any notifications, no active notifications count can be get(callback)
    */
    it('Ans_GetActive_1500', 0, async function (done) {
        console.debug("===============Ans_GetActive_1500 start==================>");
        await notify.cancelAll();
        notify.getActiveNotificationCount(getCountCallbackThree);
        console.debug("===============Ans_GetActive_1500 getActiveNotificationCount end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1500 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_1600
    * @tc.name: getActiveNotificationCount(): Promise<number>;
    * @tc.desc: Verify: before publishing any notifications, no active notifications count can be get(promise)
    */
    it('Ans_GetActive_1600', 0, async function (done) {
        console.debug("===============Ans_GetActive_1600 start==================>");
        await notify.cancelAll();
        var promiseCount = await notify.getActiveNotificationCount();
        expect(promiseCount).assertEqual(0);
        console.debug("===============Ans_GetActive_1600 promiseCount==================>"+promiseCount);
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1600 done==================>");
            done();
        }, time);
    })


    function getCountCallbackFour(err, count){
        console.debug("===============Ans_GetActive_1700 getCountCallbackFour count==================>"+count);
        expect(count).assertEqual(1);
    }

    /*
    * @tc.number: Ans_GetActive_1700
    * @tc.name: getActiveNotificationCount(callback: AsyncCallback<number>): void;
    * @tc.desc: Verify: after publishing two notifications,
                cancel one of the notifications, get all active notifications count(callback)
    */
    it('Ans_GetActive_1700', 0, async function (done) {
        console.debug("===============Ans_GetActive_1700 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "1700_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "1700_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_1700 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_1700 publish2 end==================>");
        await notify.cancel(1, "1700_1");
        console.debug("===============Ans_GetActive_1700 cancel end==================>");
        notify.getActiveNotificationCount(getCountCallbackFour);
        console.debug("===============Ans_GetActive_1700 getActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1700 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_1800
    * @tc.name: getActiveNotificationCount(): Promise<number>;
    * @tc.desc: Verify: after publishing two notifications,
                cancel one of the notifications, get all active notifications count(promise)
    */
    it('Ans_GetActive_1800', 0, async function (done) {
        console.debug("===============Ans_GetActive_1800 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "1800_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "1800_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_1800 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_1800 publish2 end==================>");
        await notify.cancel(1, "1800_1");
        console.debug("===============Ans_GetActive_1800 cancel end==================>");
        var promiseCount = await notify.getActiveNotificationCount();
        expect(promiseCount).assertEqual(1);
        console.debug("===============Ans_GetActive_1800 promiseCount===========>"+promiseCount);
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1800 done==================>");
            done();
        }, time);
    })

    function getCountCallbackFive(err, count){
        console.debug("===============Ans_GetActive_1900 getCountCallbackFour count==================>"+count);
        expect(count).assertEqual(0);
    }

    /*
    * @tc.number: Ans_GetActive_1900
    * @tc.name: getActiveNotificationCount(callback: AsyncCallback<number>): void;
    * @tc.desc: Verify: after publishing two notifications,
                cancel one of the notifications, get all active notifications count(callback)
    */
    it('Ans_GetActive_1900', 0, async function (done) {
        console.debug("===============Ans_GetActive_1900 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "1900_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "1900_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_1900 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_1900 publish2 end==================>");
        await notify.cancelAll();
        console.debug("===============Ans_GetActive_1900 cancel end==================>");
        notify.getActiveNotificationCount(getCountCallbackFive);
        console.debug("===============Ans_GetActive_1900 getActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetActive_1900 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetActive_2000
    * @tc.name: getActiveNotificationCount(): Promise<number>;
    * @tc.desc: Verify: after publishing two notifications,
                cancel one of the notifications, get all active notifications count(promise)
    */
    it('Ans_GetActive_2000', 0, async function (done) {
        console.debug("===============Ans_GetActive_2000 start==================>");
        await notify.cancelAll();
        var notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_1",
                    text: "test_text_1",
                    additionalText: "test_additionalText_1"
                },
            },
            id: 1,
            label: "2000_1",
        }
        var notificationRequest2 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_2",
                    text: "test_text_2",
                    additionalText: "test_additionalText_2"
                },
            },
            id: 2,
            label: "2000_2",
        }
        await notify.publish(notificationRequest1);
        console.debug("===============Ans_GetActive_2000 publish1 end==================>");
        await notify.publish(notificationRequest2);
        console.debug("===============Ans_GetActive_2000 publish2 end==================>");
        await notify.cancelAll();
        console.debug("===============Ans_GetActive_2000 cancelAll end==================>");
        var promiseCount = await notify.getActiveNotificationCount();
        expect(promiseCount).assertEqual(0);
        console.debug("===============Ans_GetActive_2000 promiseCount===========>"+promiseCount);
        setTimeout(function(){
            console.debug("===============Ans_GetActive_2000 done==================>");
            done();
        }, time);
    })

    function getAllCallbackNine(err,data){
        console.debug("===========Ans_GetAllActive_0900 getAllCallbackNine data.length============>"+data.length);
        console.debug("===========Ans_GetAllActive_0900 getAllCallbackNine err.code============>"+err.code);
        expect(err.code).assertEqual(0);
    }

    /*
    * @tc.number: Ans_GetAllActive_0900
    * @tc.name: getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: Under non system permission, after the current app publish a notification,
                get all active notifications in the system(callback)
    */
    it('Ans_GetAllActive_0900', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0900 start==================>");
        await notify.cancelAll();
        var notificationRequestOfCurrentApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_currentApp",
                    text: "test_text_currentApp",
                    additionalText: "test_additionalText_currentApp"
                },
            },
            id: 1,
            label: "currentApp_0900",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("===============Ans_GetAllActive_0900 publish CurrentApp notify end==================>");
        notify.getAllActiveNotifications(getAllCallbackNine);
        console.debug("===============Ans_GetAllActive_0900 getAllActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0900 done==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetAllActive_1000
    * @tc.name: getAllActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: Under non system permission, after the current app publish a notification,
                get all active notifications in the system(promise)
    */
    it('Ans_GetAllActive_1000', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_1000 start==================>");
        await notify.cancelAll();
        var notificationRequestOfCurrentApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_currentApp",
                    text: "test_text_currentApp",
                    additionalText: "test_additionalText_currentApp"
                },
            },
            id: 1,
            label: "currentApp_1000",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("===============Ans_GetAllActive_1000 publish CurrentApp notify end==================>");
        await notify.getAllActiveNotifications().then(()=>{
            console.debug("=======Ans_GetAllActive_1000 then========>");
			expect(err.code).assertEqual(0);
        }).catch((err)=>{
            console.debug("=======Ans_GetAllActive_1000 err==========>"+err.code);
        });
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_1000 done==================>");
            done();
        }, time);
    })
})

