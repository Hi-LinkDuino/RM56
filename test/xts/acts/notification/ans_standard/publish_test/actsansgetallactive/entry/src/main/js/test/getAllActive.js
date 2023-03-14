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
var time = 500
describe('ActsAnsGetAllActiveTestXts', function () {
    console.info("===========ActsAnsGetAllActiveTest  start====================>");
    function getAllCallback(err, data){
        console.log("Ans_GetAllActive_0100 getAllCallback ============>");
        var i;
        console.log("Ans_GetAllActive_0100 getAllCallback  data.length============>"+data.length);
        expect(data.length).assertEqual(2);
        console.log("Ans_GetAllActive_0100 getAllCallback  data============>"+JSON.stringify(data));
        for (i = 0; i < data.length; i++) {
            if (i == 0){
                expect(data[i].content.normal.title).assertEqual("test_title_otherApp");
                console.log("=======Ans_GetAllActive_0100 getCallback title=====>"+data[i].content.normal.title)
                expect(data[i].content.normal.text).assertEqual("test_text_otherApp");
                console.log("=======Ans_GetAllActive_0100 getCallback text========>"+data[i].content.normal.text)
                expect(data[i].content.normal.additionalText).assertEqual("test_additionalText_otherApp");
                console.log("===Ans_GetAllActive_0100 getCallback text====>"+data[i].content.normal.additionalText)
                expect(data[i].id).assertEqual(2);
                console.log("============Ans_GetAllActive_0100 getCallback id============>"+data[i].id)
                expect(data[i].label).assertEqual("otherApp");
                console.log("============Ans_GetAllActive_0100 getCallback label=====>"+data[i].label)
            }else if(i == 1){
                expect(data[i].content.normal.title).assertEqual("test_title_currentApp");
                console.log("======Ans_GetAllActive_0100 getCallback title=========>"+data[i].content.normal.title)
                expect(data[i].content.normal.text).assertEqual("test_text_currentApp");
                console.log("==========Ans_GetAllActive_0100 getCallback text=======>"+data[i].content.normal.text)
                expect(data[i].content.normal.additionalText).assertEqual("test_additionalText_currentApp");
                console.log("===Ans_GetAllActive_0100 getCallback text=====>"+data[i].content.normal.additionalText)
                expect(data[i].id).assertEqual(1);
                console.log("============Ans_GetAllActive_0100 getCallback id============>"+data[i].id)
                expect(data[i].label).assertEqual("currentApp_0100");
                console.log("============Ans_GetAllActive_0100 getCallback label=====>"+data[i].label)
            }
        }
    }

    /*
    * @tc.number: Ans_GetAllActive_xts_0100
    * @tc.name: getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: After the current app and other apps publish two notifications,
                get all active notifications in the system(callback)
    */
    it('Ans_GetAllActive_xts_0100', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0100 start==================>");
        await notify.cancelAll();
        var notificationRequestOfOtherApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_otherApp",
                    text: "test_text_otherApp",
                    additionalText: "test_additionalText_otherApp"
                },
            },
            id: 2,
            label: "otherApp",
        }
        await notify.publish(notificationRequestOfOtherApp);
        console.debug("===============Ans_GetAllActive_0100 publish OtherApp notify end==================>");
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
            label: "currentApp_0100",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("===============Ans_GetAllActive_0100 publish CurrentApp notify end==================>");
        notify.getAllActiveNotifications(getAllCallback);
        console.debug("===============Ans_GetAllActive_0100 getAllActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0100 setTimeout==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetAllActive_xts_0200
    * @tc.name: getAllActiveNotifications(): Promise<Array<NotificationRequest>>
    * @tc.desc: Verify: After the current app and other apps publish two notifications,
                get all active notifications in the system(promise)
    */
    it('Ans_GetAllActive_xts_0200', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0200 start==================>");
        await notify.cancelAll();
        var notificationRequestOfOtherApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_otherApp",
                    text: "test_text_otherApp",
                    additionalText: "test_additionalText_otherApp"
                },
            },
            id: 2,
            label: "otherApp",
        }
        await notify.publish(notificationRequestOfOtherApp);
        console.debug("===============Ans_GetAllActive_0200 publish OtherApp notify end==================>");
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
            label: "currentApp_0200",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("===============Ans_GetAllActive_0200 publish CurrentApp notify end==================>");
        var promiseData = await notify.getAllActiveNotifications();
        console.debug("===============Ans_GetAllActive_0200 getActiveNotifications end==================>");
        expect(promiseData.length).assertEqual(2);
        var i;
        for (i = 0; i < promiseData.length; i++) {
            if (i == 0){
                expect(promiseData[i].content.normal.title).assertEqual("test_title_otherApp");
                console.log("=======Ans_GetAllActive_0200 title=====>"+promiseData[i].content.normal.title)
                expect(promiseData[i].content.normal.text).assertEqual("test_text_otherApp");
                console.log("=======Ans_GetAllActive_0200 text========>"+promiseData[i].content.normal.text)
                expect(promiseData[i].content.normal.additionalText).assertEqual("test_additionalText_otherApp");
                console.log("===Ans_GetAllActive_0200 text====>"+promiseData[i].content.normal.additionalText)
                expect(promiseData[i].id).assertEqual(2);
                console.log("============Ans_GetAllActive_0200 id============>"+promiseData[i].id)
                expect(promiseData[i].label).assertEqual("otherApp");
                console.log("============Ans_GetAllActive_0200 label=====>"+promiseData[i].label)
            }else if(i == 1){
                expect(promiseData[i].content.normal.title).assertEqual("test_title_currentApp");
                console.log("====Ans_GetAllActive_0200 title=====>"+promiseData[i].content.normal.title)
                expect(promiseData[i].content.normal.text).assertEqual("test_text_currentApp");
                console.log("======Ans_GetAllActive_0200 text=====>"+promiseData[i].content.normal.text)
                expect(promiseData[i].content.normal.additionalText).assertEqual("test_additionalText_currentApp");
                console.log("Ans_GetAllActive_0200 text===>"+promiseData[i].content.normal.additionalText)
                expect(promiseData[i].id).assertEqual(1);
                console.log("============Ans_GetAllActive_0200 id============>"+promiseData[i].id)
                expect(promiseData[i].label).assertEqual("currentApp_0200");
                console.log("============Ans_GetAllActive_0200 label=====>"+promiseData[i].label)
            }
        }
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0200 setTimeout==================>");
            done();
        }, time);
    })

    function getAllCallbackThree(err, data){
        console.log("Ans_GetAllActive_0300 getAllCallbackThree ============>");
        console.log("Ans_GetAllActive_0300 getAllCallbackThree  data.length============>"+data.length);
        console.log("Ans_GetAllActive_0300 getAllCallbackThree  data============>"+JSON.stringify(data));
        expect(data.length).assertEqual(1);
        var i;
        for (i = 0; i < data.length; i++) {
            expect(data[i].content.normal.title).assertEqual("test_title_otherApp");
            console.log("==========Ans_GetAllActive_0300 getCallback title=========>"+data[i].content.normal.title)
            expect(data[i].content.normal.text).assertEqual("test_text_otherApp");
            console.log("==========Ans_GetAllActive_0300 getCallback text============>"+data[i].content.normal.text)
            expect(data[i].content.normal.additionalText).assertEqual("test_additionalText_otherApp");
            console.log("======Ans_GetAllActive_0300 getCallback text=======>"+data[i].content.normal.additionalText)
            expect(data[i].id).assertEqual(2);
            console.log("============Ans_GetAllActive_0300 getCallback id============>"+data[i].id)
            expect(data[i].label).assertEqual("otherApp");
            console.log("============Ans_GetAllActive_0300 getCallback label=====>"+data[i].label)
        }
    }

    /*
    * @tc.number: Ans_GetAllActive_xts_0300
    * @tc.name: getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: After the current app and other apps publish two notifications, cancel the notifications
                of the current app, get all active notifications in the system(callback)
    */
    it('Ans_GetAllActive_xts_0300', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0300 start==================>");
        await notify.cancelAll();
        var notificationRequestOfOtherApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_otherApp",
                    text: "test_text_otherApp",
                    additionalText: "test_additionalText_otherApp"
                },
            },
            id: 2,
            label: "otherApp",
        }
        await notify.publish(notificationRequestOfOtherApp);
        console.debug("===============Ans_GetAllActive_0300 publish OtherApp notify end==================>");
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
            label: "currentApp_0300",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("===============Ans_GetAllActive_0300 publish CurrentApp notify end==================>");
        await notify.cancel(1, "currentApp_0300");
        notify.getAllActiveNotifications(getAllCallbackThree);
        console.debug("===============Ans_GetAllActive_0300 getAllActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0300 setTimeout==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetAllActive_xts_0400
    * @tc.name: getAllActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: after publishing two notifications,
                cancel one of the notifications, get all active notifications info(promise)
    */
    it('Ans_GetAllActive_xts_0400', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0400 start==================>");
        await notify.cancelAll();
        var notificationRequestOfOtherApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_otherApp",
                    text: "test_text_otherApp",
                    additionalText: "test_additionalText_otherApp"
                },
            },
            id: 2,
            label: "otherApp",
        }
        await notify.publish(notificationRequestOfOtherApp);
        console.debug("===============Ans_GetAllActive_0400 publish OtherApp notify end==================>");
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
            label: "currentApp_0400",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("===============Ans_GetAllActive_0400 publish CurrentApp notify end==================>");
        await notify.cancel(1, "currentApp_0400");
        console.debug("===============Ans_GetAllActive_0400 cancel end==================>");
        var promiseData = await notify.getAllActiveNotifications();
        var i;
        for (i = 0; i < promiseData.length; i++) {
            expect(promiseData[i].content.normal.title).assertEqual("test_title_otherApp");
            console.log("=======Ans_GetAllActive_0400 title=====>"+promiseData[i].content.normal.title)
            expect(promiseData[i].content.normal.text).assertEqual("test_text_otherApp");
            console.log("=======Ans_GetAllActive_0400 text========>"+promiseData[i].content.normal.text)
            expect(promiseData[i].content.normal.additionalText).assertEqual("test_additionalText_otherApp");
            console.log("===Ans_GetAllActive_0400 text====>"+promiseData[i].content.normal.additionalText)
            expect(promiseData[i].id).assertEqual(2);
            console.log("============Ans_GetAllActive_0400 id============>"+promiseData[i].id)
            expect(promiseData[i].label).assertEqual("otherApp");
            console.log("============Ans_GetAllActive_0400 label=====>"+promiseData[i].label)
        }
        console.debug("===============Ans_GetAllActive_0400 getAllActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0400 setTimeout==================>");
            done();
        }, time);
    })

    function getAllCallbackFive(err, data){
        console.log("Ans_GetAllActive_0500 getAllCallbackFive data.length============>"+data.length);
        console.log("Ans_GetAllActive_0500 getAllCallbackFive data============>"+JSON.stringify(data));
        expect(data.length).assertEqual(0);
    }

    /*
    * @tc.number: Ans_GetAllActive_xts_0500
    * @tc.name: getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify: After the current app and other apps publish two notifications, remove all the notifications
                of the system, get all active notifications in the system(callback)
    */
    it('Ans_GetAllActive_xts_0500', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0500 start==================>");
        await notify.cancelAll();
        var notificationRequestOfOtherApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_otherApp",
                    text: "test_text_otherApp",
                    additionalText: "test_additionalText_otherApp"
                },
            },
            id: 2,
            label: "otherApp",
        }
        await notify.publish(notificationRequestOfOtherApp);
        console.debug("===============Ans_GetAllActive_0500 publish OtherApp notify end==================>");
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
            label: "currentApp_0500",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("===============Ans_GetAllActive_0500 publish CurrentApp notify end==================>");
        await notify.removeAll();
        notify.getAllActiveNotifications(getAllCallbackFive);
        console.debug("===============Ans_GetAllActive_0500 getAllActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0500 setTimeout==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetAllActive_xts_0600
    * @tc.name: getAllActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: After the current app and other apps publish two notifications, remove all the notifications
                of the system, get all active notifications in the system(promise)
    */
    it('Ans_GetAllActive_xts_0600', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0600 start==================>");
        await notify.cancelAll();
        var notificationRequestOfOtherApp = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title_otherApp",
                    text: "test_text_otherApp",
                    additionalText: "test_additionalText_otherApp"
                },
            },
            id: 2,
            label: "otherApp",
        }
        await notify.publish(notificationRequestOfOtherApp);
        console.debug("===============Ans_GetAllActive_0600 publish OtherApp notify end==================>");
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
            label: "currentApp_0600",
        }
        await notify.publish(notificationRequestOfCurrentApp);
        console.debug("==========Ans_GetAllActive_0600 publish CurrentApp notify end==================>");
        await notify.removeAll();
        var promiseData = await notify.getAllActiveNotifications();
        expect(promiseData.length).assertEqual(0);
        console.debug("=======Ans_GetAllActive_0600 promiseData.length==========>"+promiseData.length);
        console.debug("=======Ans_GetAllActive_0600 promiseData==========>"+JSON.stringify(promiseData));
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0600 setTimeout==================>");
            done();
        }, time);
    })

    function getAllCallbackSeven(err, data){
        console.log("Ans_GetAllActive_0700 getAllCallbackSeven  data.length============>"+data.length);
        console.log("Ans_GetAllActive_0700 getAllCallbackSeven  data============>"+JSON.stringify(data));
        expect(data.length).assertEqual(0);
    }

    /*
    * @tc.number: Ans_GetAllActive_xts_0700
    * @tc.name: getAllActiveNotifications(callback: AsyncCallback<Array<NotificationRequest>>): void;
    * @tc.desc: Verify:No active notifications in the system, get all active notifications in the system(callback)
    */
    it('Ans_GetAllActive_xts_0700', 0, async function (done) {
        console.debug("===============Ans_GetAllActive_0700 start==================>");
        await notify.removeAll();
        notify.getAllActiveNotifications(getAllCallbackSeven);
        console.debug("===============Ans_GetAllActive_0700 getAllActiveNotifications end==================>");
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0700 setTimeout==================>");
            done();
        }, time);
    })

    /*
    * @tc.number: Ans_GetAllActive_xts_0800
    * @tc.name: getAllActiveNotifications(): Promise<Array<NotificationRequest>>;
    * @tc.desc: Verify: No active notifications in the system, get all active notifications in the system(promise)
    */
    it('Ans_GetAllActive_xts_0800', 0, async function (done) {
        console.debug("==========Ans_GetAllActive_0800 start==================>");
        await notify.removeAll();
        var promiseData = await notify.getAllActiveNotifications();
        console.debug("=========Ans_GetAllActive_0800 promiseData.length=============>"+promiseData.length);
        expect(promiseData.length).assertEqual(0);
        setTimeout(function(){
            console.debug("===============Ans_GetAllActive_0800 setTimeout==================>");
            done();
        }, time);
    })
})

