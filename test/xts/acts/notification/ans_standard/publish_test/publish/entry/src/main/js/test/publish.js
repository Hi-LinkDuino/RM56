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
var timeout = 1000;
describe('ActsAnsNotificationPublishXts', function () {
    console.info("===========ActsAnsNotificationPublish start====================>");
    function onConsumeA(data) {
        console.info("===ACTS_PublishMULTILINEContent_0100 onConsume start===>");
        console.info("===ACTS_PublishMULTILINEContent_0100 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishMULTILINEContent_0100");
        expect(data.request.creatorUserId).assertEqual(100);
        console.info("===ACTS_PublishMULTILINEContent_0100 onConsume end===>");
    }
    
    /*
     * @tc.number: ACTS_PublishMULTILINEContent_xts_0100
     * @tc.name: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_MULTILINE)
     */
    it('ACTS_PublishMULTILINEContent_xts_0100', 0, async function (done) {
        console.info("===ACTS_PublishMULTILINEContent_0100 start===>");
        var subscriber ={
            onConsume:onConsumeA
        }
        await notify.subscribe(subscriber);
        console.info("===========ACTS_PublishMULTILINEContent_0100 subscribe promise=======>");
        var notificationRequest = {
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                multiLine: {
                title: "test1_title",
                text: "test1_text",
                additionalText: "test1_additionalText",
                briefText: "briefText1",
                longTitle: "longTitle1",
                lines: ["thrive", "democracy", "civilization", "harmonious"]
              },
            },
            id: 1,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishMULTILINEContent_0100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===ACTS_PublishMULTILINEContent_0100 err===>"+err.code);
        });
        setTimeout((async function(){
            console.info("===ACTS_PublishMULTILINEContent_0100 setTimeout===>");
            await notify.unsubscribe(subscriber);
            console.info("===ACTS_PublishMULTILINEContent_0100 setTimeout unsubscribe===>");
            done();
        }),timeout);
    })

    function onConsumeB(data) {
        console.info("===ACTS_PublishMULTILINEContent_0200 onConsume start===>");
        console.info("===ACTS_PublishMULTILINEContent_0200 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishMULTILINEContent_0200 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_xts_0200
     * @tc.name: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     * @tc.desc: verify the function of publish()
     */
    it('ACTS_PublishMULTILINEContent_xts_0200', 0, async function (done) {
        console.info("===ACTS_PublishMULTILINEContent_0200 start===>");
        var subscriber ={
            onConsume:onConsumeB
        }
        await notify.subscribe(subscriber);
        console.info("===ACTS_PublishMULTILINEContent_0200 subscribe promise===>");
        var notificationRequest = {
            content:{
//                contentType: notify.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                multiLine: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    briefText: "briefText",
                    longTitle: "longTitle",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            },
            id: 2,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishMULTILINEContent_0200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest, (err) => {
                console.info("===ACTS_PublishMULTILINEContent_0200 err===>" + err.code);
            });
        }catch(err){
            console.info("===ACTS_PublishMULTILINEContent_0200 err===>" + err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("===ACTS_PublishMULTILINEContent_0200 setTimeout unsubscribe end===>");
            done();
        }),timeout);
    })

    function onConsumeC(data) {
        console.info("===ANS_Publish_0300 onConsume start===>");
        console.info("===ANS_Publish_0300 onConsume data:===>" + JSON.stringify(data));
        console.info("===ANS_Publish_0300 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_xts_0300
     * @tc.name: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     * @tc.desc: verify the function of publish()
     */
    it('ACTS_PublishMULTILINEContent_xts_0300', 0, async function (done) {
        console.info("===============ACTS_PublishMULTILINEContent_0300 start==========================>");
        var subscriber ={
            onConsume:onConsumeC
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishMULTILINEContent_0300 subscribe promise===============>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_PICTURE,
                multiLine: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    briefText: "briefText",
                    longTitle: "longTitle",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            },
            id: 3,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishMULTILINEContent_0300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest, (err) => {
                console.info("==========ACTS_PublishMULTILINEContent_0300 err==================>" + err.code);
            });
        }catch(err){
            console.info("==========ACTS_PublishMULTILINEContent_0300 err==================>" + err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishMULTILINEContent_0300 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeD(data) {
        console.info("===ACTS_PublishMULTILINEContent_0400 onConsume===>");
        console.info("===ACTS_PublishMULTILINEContent_0400 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishMULTILINEContent_0400");
        console.info("===ACTS_PublishMULTILINEContent_0400 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_xts_0400
     * @tc.name: function publish(request: NotificationRequest): Promise<void>;
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_MULTILINE) promise
     */
    it('ACTS_PublishMULTILINEContent_xts_0400', 0, async function (done) {
        console.info("===============ACTS_PublishMULTILINEContent_0400 start==========================>");
        var subscriber ={
            onConsume:onConsumeD
        }
        await notify.subscribe(subscriber);
        console.info("================ACTS_PublishMULTILINEContent_0400 subscribe promise=============>");
        var notificationRequest = {
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                multiLine: {
                    title: "test4_title",
                    text: "test4_text",
                    additionalText: "test4_additionalText",
                    briefText: "briefText4",
                    longTitle: "longTitle4",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            },
            id: 4,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishMULTILINEContent_0400",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.publish(notificationRequest).then(console.info("======ACTS_PublishMULTILINEContent_0400 promise==================>"));
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishMULTILINEContent_0400 setTimeout unsubscribe end==================>");
            done();
        }),timeout);

    })

    function onConsumeE(data) {
        console.info("===ACTS_PublishMULTILINEContent_0500 onConsume start===>");
        console.info("===ACTS_PublishMULTILINEContent_0500 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishMULTILINEContent_0500 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_xts_0500
     * @tc.name: function publish(request: NotificationRequest): Promise<void>;
     * @tc.desc: function publish(request: NotificationRequest): Promise<void>;
     */
    it('ACTS_PublishMULTILINEContent_xts_0500', 0, async function (done) {
        console.info("============ACTS_PublishMULTILINEContent_0500 start==================>");
        var subscriber ={
            onConsume:onConsumeE
        }
        await notify.subscribe(subscriber);
        console.info("============ACTS_PublishMULTILINEContent_0500 subscribe promise======>");
        var notificationRequest = {
            content: {
//                contentType: notify.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                multiLine: {
                    title: "test5_title",
                    text: "test5_text",
                    additionalText: "test5_additionalText",
                    briefText: "briefText5",
                    longTitle: "longTitle5",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            },
            id: 5,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishMULTILINEContent_0500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try{
          await notify.publish(notificationRequest)
        }catch(err){
            console.info("======ACTS_PublishMULTILINEContent_0500 err==================>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishMULTILINEContent_0500 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeF(data) {
        console.info("===ACTS_PublishMULTILINEContent_0600 onConsume start===>");
        console.info("===ACTS_PublishMULTILINEContent_0600 onConsume data===>" + JSON.stringify(data));
        console.info("===ACTS_PublishMULTILINEContent_0600 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_xts_0600
     * @tc.name: cancelAll(): Promise<void>;
     * @tc.desc: Verify that all notifications are cancelled successfully by calling the
                 cancelAll(): Promise<void> interface, and then cancel the notification again
     */
    it('ACTS_PublishMULTILINEContent_xts_0600', 0, async function (done) {
        console.info("===============ACTS_PublishMULTILINEContent_0600 start==========================>");
        var subscriber ={
            onConsume:onConsumeF
        }
        await notify.subscribe(subscriber);
        console.info("==================ACTS_PublishMULTILINEContent_0600 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_PICTURE,
                multiLine: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    briefText: "briefText",
                    longTitle: "longTitle",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            },
            id: 6,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishMULTILINEContent_0600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest);
        }catch(err){
            console.info("==================ACTS_PublishMULTILINEContent_0600 err==================>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishMULTILINEContent_0600 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeG(data) {
        console.info("===ACTS_PublishLONGContent_0100 onConsume start===>");
        console.info("===ACTS_PublishLONGContent_0100 onConsume data: ===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishLONGContent_0100");
        console.info("===ACTS_PublishLONGContent_0100 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishLONGContent_xts_0100
     * @tc.name: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     * @tc.desc: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     */
    it('ACTS_PublishLONGContent_xts_0100', 0, async function (done) {
        console.info("===============ACTS_PublishLONGContent_0100 start==========================>");
        var subscriber ={
            onConsume:onConsumeG
        }
        await notify.subscribe(subscriber);
        console.info("========ACTS_PublishLONGContent_0100 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test7_title",
                    text: "test7_text",
                    additionalText: "test7_additionalText",
                    longText:"longText7",
                    briefText:"briefText7",
                    expandedTitle:"expandedTitle7"
                },
            },
            id: 7,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishLONGContent_0100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===========ACTS_PublishLONGContent_0100 err==================>"+err.code);
        });
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishLONGContent_0100 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeH(data) {
        console.info("===ACTS_PublishLONGContent_0200 onConsume start===>");
        console.info("===ACTS_PublishLONGContent_0200 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishLONGContent_0200 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishLONGContent_xts_0200
     * @tc.name: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     * @tc.desc: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     */
    it('ACTS_PublishLONGContent_xts_0200', 0, async function (done) {
        console.info("===============ACTS_PublishLONGContent_0200 start==========================>");
        var subscriber ={
            onConsume:onConsumeH
        }
        await notify.subscribe(subscriber);
        console.info("==================ACTS_PublishLONGContent_0200 subscribe promise==================>");
        var notificationRequest = {
            content:{
//                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test8_title",
                    text: "test8_text",
                    additionalText: "test8_additionalText",
                    longText:"longText8",
                    briefText:"briefText8",
                    expandedTitle:"expandedTitle8"
                },
            },
            id: 8,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishLONGContent_0200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try{
          await notify.publish(notificationRequest,(err)=>{
            console.info("=========ACTS_PublishLONGContent_0200 err==================>"+err.code);
          });
        }catch(err){
            console.info("=========ACTS_PublishLONGContent_0200 err==================>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishLONGContent_0200 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeI(data) {
        console.info("===ACTS_PublishLONGContent_0300 onConsume start===>");
        console.info("===ACTS_PublishLONGContent_0300 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishLONGContent_0300 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishLONGContent_xts_0300
     * @tc.name: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     * @tc.desc: function publish(request: NotificationRequest, callback: AsyncCallback<void>): void;
     */
    it('ACTS_PublishLONGContent_xts_0300', 0, async function (done) {
        console.info("===============ACTS_PublishLONGContent_0300 start==========================>");
        var subscriber ={
            onConsume:onConsumeI
        }
        await notify.subscribe(subscriber);
        console.info("==================ACTS_PublishLONGContent_0300 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_MEDIA,
                longText: {
                    title: "test9_title",
                    text: "test9_text",
                    additionalText: "test9_additionalText",
                    longText:"longText9",
                    briefText:"briefText9",
                    expandedTitle:"expandedTitle9"
                },
            },
            id: 9,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishLONGContent_0300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest,(err)=>{
                console.info("==================ACTS_PublishLONGContent_0300 err==================>"+err.code);
            });
        }catch(err){
            console.info("==================ACTS_PublishLONGContent_0300 err==================>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishLONGContent_0300 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeJ(data) {
        console.info("===ACTS_PublishLONGContent_0400 onConsume start===>");
        console.info("===ACTS_PublishLONGContent_0400 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishLONGContent_0400");
        console.info("===ACTS_PublishLONGContent_0400 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishLONGContent_xts_0400
     * @tc.name: cancel(id: number, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that when the cancel(id: number, callback: AsyncCallback<void>): void
                 interface is called, when the id is wrong, no notification information is cancelled at this time
     */
    it('ACTS_PublishLONGContent_xts_0400', 0, async function (done) {
        console.info("===============ACTS_PublishLONGContent_0400 start==========================>");
        var subscriber ={
            onConsume:onConsumeJ
        }
        await notify.subscribe(subscriber);
        console.info("================ACTS_PublishLONGContent_0400 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test10_title",
                    text: "test10_text",
                    additionalText: "test10_additionalText",
                    longText:"longText10",
                    briefText:"briefText10",
                    expandedTitle:"expandedTitle10"
                },
            },
            id: 10,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishLONGContent_0400",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("===========ACTS_PublishLONGContent_0400 publish promise==================>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishLONGContent_0400 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeK(data) {
        console.info("===ACTS_PublishLONGContent_0500 onConsume start===>");
        console.info("===ACTS_PublishLONGContent_0500 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishLONGContent_0500 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishLONGContent_xts_0500
     * @tc.name: cancel(id: number, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify the success of canceling the notification with the notification attribute isUnremovable
                 being true by calling the cancel(id: number, callback: AsyncCallback<void>): void interface
     */
    it('ACTS_PublishLONGContent_xts_0500', 0, async function (done) {
        console.info("===============ACTS_PublishLONGContent_0500 start==========================>");
        var subscriber ={
            onConsume:onConsumeK
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishLONGContent_0500 subscribe promise=============>");
        var notificationRequest = {
            content:{
//                contentType: notify.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test11_title",
                    text: "test11_text",
                    additionalText: "test11_additionalText",
                    longText:"longText11",
                    briefText:"briefText11",
                    expandedTitle:"expandedTitle11"
                },
            },
            id: 11,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishLONGContent_0500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest);
        }catch(err){
            console.info("============ACTS_PublishLONGContent_0500 err===========>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishLONGContent_0500 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeL(data) {
        console.info("===ACTS_PublishLONGContent_0600 onConsume start===>");
        console.info("===ACTS_PublishLONGContent_0600 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishLONGContent_0600 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishLONGContent_xts_0600
     * @tc.name: cancel(id: number, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, callback: AsyncCallback<void>): void
                 interface is called twice in a row to cancel the notification
     */
    it('ACTS_PublishLONGContent_xts_0600', 0, async function (done) {
        console.info("=============ACTS_PublishLONGContent_0600 start==========================>");
        var subscriber ={
            onConsume:onConsumeL
        }
        await notify.subscribe(subscriber);
        console.info("=============ACTS_PublishLONGContent_0600 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_MEDIA,
                longText: {
                    title: "test12_title",
                    text: "test12_text",
                    additionalText: "test12_additionalText",
                    longText:"longText12",
                    briefText:"briefText12",
                    expandedTitle:"expandedTitle12"
                },
            },
            id: 12,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishLONGContent_0600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest);
        }catch(err){
            console.info("========ACTS_PublishLONGContent_0600 err=================>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishLONGContent_0600 setTimeout unsubscribe end==================>");
            done();
        }),1500);
    })

    function onConsumeM(data) {
        console.info("===ANS_Cancel_1300 onConsume start===>");
        console.info("===ANS_Cancel_1300 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_Publish_SlotTypeContent_0100");
        console.info("===ANS_Cancel_1300 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0100
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel notification is successful by calling the
                 cancel(id: number, label: string, callback: AsyncCallback<void>): void; interface
     */
    it('ACTS_Publish_SlotTypeContent_xts_0100', 0, async function (done) {
        console.info("===ACTS_Publish_SlotTypeContent_0100 start===>");
        var subscriber ={
            onConsume:onConsumeM
        }
        await notify.subscribe(subscriber);
        console.info("===ACTS_Publish_SlotTypeContent_0100 subscribe promise===>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 13,
            slotType : notify.SlotType.CONTENT_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===ACTS_Publish_SlotTypeContent_0100 err===>"+err.code);
        });
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("===ACTS_Publish_SlotTypeContent_0100 setTimeout unsubscribe end===>");
            done();
        }),timeout);
    })

    function onConsumeN(data) {
        console.info("===ACTS_Publish_SlotTypeContent_0200 onConsume start===>");
        console.info("===ACTS_Publish_SlotTypeContent_0200 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_Publish_SlotTypeContent_0200 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0200
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel notification is successful by calling the
                 cancel(id: number, label?: string): Promise<void> interface
     */
    it('ACTS_Publish_SlotTypeContent_xts_0200', 0, async function (done) {
        console.info("===============ACTS_Publish_SlotTypeContent_0200 start==========================>");
        var subscriber ={
            onConsume:onConsumeN
        }
        await notify.subscribe(subscriber);
        console.info("==========ACTS_Publish_SlotTypeContent_0200 subscribe promise==================>");
        var notificationRequest = {
            content:{
//                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 14,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest,(err)=>{
                console.info("===============ACTS_Publish_SlotTypeContent_0200 err==============>"+err.code);
            });
        }catch(err){
            console.info("===============ACTS_Publish_SlotTypeContent_0200 err==============>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_Publish_SlotTypeContent_0200 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeO(data) {
        console.info("===ACTS_Publish_SlotTypeContent_0300 onConsume start===>");
        console.info("===ACTS_Publish_SlotTypeContent_0300 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_Publish_SlotTypeContent_0300 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0300
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void
     * @tc.desc: Verify that the notification whose notification property isUnremovable is true is canceled
     successfully by calling the cancel(id: number, label: string, callback: AsyncCallback<void>): void interface
     */
    it('ACTS_Publish_SlotTypeContent_xts_0300', 0, async function (done) {
        console.info("===============ACTS_Publish_SlotTypeContent_0300 start==========================>");
        var subscriber ={
            onConsume:onConsumeO
        }
        await notify.subscribe(subscriber);
        console.info("=======ACTS_Publish_SlotTypeContent_0300 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 15,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===========ACTS_Publish_SlotTypeContent_0300 publish promise=============>"+err.code);
        });
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_Publish_SlotTypeContent_0300 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeP(data) {
        console.info("===ACTS_Publish_SlotTypeContent_0400 onConsume start===>");
        console.info("===ACTS_Publish_SlotTypeContent_0400 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_Publish_SlotTypeContent_0400");
        console.info("===ACTS_Publish_SlotTypeContent_0400 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0400
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the notification whose notification property isUnremovable is true
                 is canceled successfully by calling the cancel(id: number, label?: string): Promise<void> interface
     */
    it('ACTS_Publish_SlotTypeContent_xts_0400', 0, async function (done) {
        console.info("===ACTS_Publish_SlotTypeContent_0400 start===>");
        var subscriber ={
            onConsume:onConsumeP
        }
        await notify.subscribe(subscriber);
        console.info("===ACTS_Publish_SlotTypeContent_0400 subscribe promise===>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 16,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0400",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===ACTS_Publish_SlotTypeContent_0400 err===>"+err.code);
        });
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("===ACTS_Publish_SlotTypeContent_0400 setTimeout unsubscribe end===>");
            done();
        }),timeout);
    })

    function onConsumeQ(data) {
        console.info("===ACTS_Publish_SlotTypeContent_0500 onConsume start===>");
        console.info("===ACTS_Publish_SlotTypeContent_0500 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_Publish_SlotTypeContent_0500");
        console.info("===ACTS_Publish_SlotTypeContent_0500 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0500
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called, the label is wrong and the ID is correct.
     */
    it('ACTS_Publish_SlotTypeContent_xts_0500', 0, async function (done) {
        console.info("===============ACTS_Publish_SlotTypeContent_0500 start==========================>");
        var subscriber ={
            onConsume:onConsumeQ
        }
        await notify.subscribe(subscriber);
        console.info("==============ACTS_Publish_SlotTypeContent_0500 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 17,
            slotType : notify.SlotType.CONTENT_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==============ACTS_Publish_SlotTypeContent_0500 publish promise==================>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_Publish_SlotTypeContent_0500 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeR(data) {
        console.info("===ACTS_Publish_SlotTypeContent_0600 onConsume start===>");
        console.info("===ACTS_Publish_SlotTypeContent_0600 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_Publish_SlotTypeContent_0600 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0600
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void> interface is called,
                 the label is wrong and the ID is correct.
     */
    it('ACTS_Publish_SlotTypeContent_xts_0600', 0, async function (done) {
        console.info("===============ACTS_Publish_SlotTypeContent_0600 start==========================>");
        var subscriber ={
            onConsume:onConsumeR
        }
        await notify.subscribe(subscriber);
        console.info("==============ACTS_Publish_SlotTypeContent_0600 subscribe promise==================>");
        var notificationRequest = {
            content:{
//                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 18,
            slotType : notify.SlotType.CONTENT_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest);
        }catch(err){
            console.info("===ACTS_Publish_SlotTypeContent_0600 publish promise===>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_Publish_SlotTypeContent_0600 setTimeout unsubscribe end===>");
            done();
        }),timeout);
    })

    function onConsumeS(data) {
        console.info("===ACTS_Publish_SlotTypeContent_0700 onConsume start===>");
        console.info("===ACTS_Publish_SlotTypeContent_0700 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_Publish_SlotTypeContent_0700");
        console.info("===ACTS_Publish_SlotTypeContent_0700 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0700
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called, and the label uses empty characters
     */
    it('ACTS_Publish_SlotTypeContent_xts_0700', 0, async function (done) {
        console.info("===============ACTS_Publish_SlotTypeContent_0700 start==========================>");
        var subscriber ={
            onConsume:onConsumeS
        }
        await notify.subscribe(subscriber);
        console.info("=============ACTS_Publish_SlotTypeContent_0700 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 19,
//            slotType : notify.SlotType.CONTENT_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0700",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==========ACTS_Publish_SlotTypeContent_0700 publish promise==============>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_Publish_SlotTypeContent_0700 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeT(data) {
        console.info("===ACTS_Publish_SlotTypeContent_0800 onConsume start===>");
        console.info("===ACTS_Publish_SlotTypeContent_0800 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_Publish_SlotTypeContent_0800");
        console.info("===ACTS_Publish_SlotTypeContent_0800 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_xts_0800
     * @tc.name: cancel(id: number, label?: string): Promise<void>;
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void> interface is called,
                 and the label uses empty characters
     */
    it('ACTS_Publish_SlotTypeContent_xts_0800', 0, async function (done) {
        console.info("===============ACTS_Publish_SlotTypeContent_0800 start==========================>");
        var subscriber ={
            onConsume:onConsumeT
        }
        await notify.subscribe(subscriber);
        console.info("=========ACTS_Publish_SlotTypeContent_0800 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 20,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_Publish_SlotTypeContent_0800",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ACTS_Publish_SlotTypeContent_0800 publish promise===============>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_Publish_SlotTypeContent_0800 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeU(data) {
        console.info("===ACTS_PublishSlotTypeOther_0100 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0100 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishSlotTypeOther_0100");
        console.info("===ACTS_PublishSlotTypeOther_0100 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0100
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel notification is successful by calling the
                 cancel(id: number, label: string, callback: AsyncCallback<void>): void; interface
     */
    it('ACTS_PublishSlotTypeOther_xts_0100', 0, async function (done) {
        console.info("===ACTS_PublishSlotTypeOther_0100 start===>");
        var subscriber ={
            onConsume:onConsumeU
        }
        await notify.subscribe(subscriber);
        console.info("===ACTS_PublishSlotTypeOther_0100 subscribe promise===>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 13,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===ACTS_PublishSlotTypeOther_0100 err===>"+err.code);
        });
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("===ACTS_PublishSlotTypeOther_0100 setTimeout unsubscribe end===>");
            done();
        }),timeout);
    })

    function onConsumeV(data) {
        console.info("===ACTS_PublishSlotTypeOther_0200 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0200 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishSlotTypeOther_0200 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0200
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel notification is successful by calling the
                 cancel(id: number, label?: string): Promise<void> interface
     */
    it('ACTS_PublishSlotTypeOther_xts_0200', 0, async function (done) {
        console.info("===============ACTS_PublishSlotTypeOther_0200 start==========================>");
        var subscriber ={
            onConsume:onConsumeV
        }
        await notify.subscribe(subscriber);
        console.info("==========ACTS_PublishSlotTypeOther_0200 subscribe promise==================>");
        var notificationRequest = {
            content:{
            //                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 14,
            slotType : notify.SlotType.OTHER_TYPES,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest,(err)=>{
                console.info("===============ACTS_PublishSlotTypeOther_0200 err==============>"+err.code);
            });
        }catch(err){
            console.info("===============ACTS_PublishSlotTypeOther_0200 err==============>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeOther_0200 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeW(data) {
        console.info("===ACTS_PublishSlotTypeOther_0300 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0300 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishSlotTypeOther_0300 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0300
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void
     * @tc.desc: Verify that the notification whose notification property isUnremovable is true is canceled
     successfully by calling the cancel(id: number, label: string, callback: AsyncCallback<void>): void interface
     */
    it('ACTS_PublishSlotTypeOther_xts_0300', 0, async function (done) {
        console.info("===============ACTS_PublishSlotTypeOther_0300 start==========================>");
        var subscriber ={
            onConsume:onConsumeW
        }
        await notify.subscribe(subscriber);
        console.info("=======ACTS_PublishSlotTypeOther_0300 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 15,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===========ACTS_PublishSlotTypeOther_0300 err=============>"+err.code);
        });
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeOther_0300 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeX(data) {
        console.info("===ACTS_PublishSlotTypeOther_0400 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0400 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishSlotTypeOther_0400");
        console.info("===ACTS_PublishSlotTypeOther_0400 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0400
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the notification whose notification property isUnremovable is true
                 is canceled successfully by calling the cancel(id: number, label?: string): Promise<void> interface
     */
    it('ACTS_PublishSlotTypeOther_xts_0400', 0, async function (done) {
        console.info("===ACTS_PublishSlotTypeOther_0400 start===>");
        var subscriber ={
            onConsume:onConsumeX
        }
        await notify.subscribe(subscriber);
        console.info("===ACTS_PublishSlotTypeOther_0400 subscribe promise===>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 16,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            isOngoing : true,
            isUnremovable : true,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0400",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest,(err)=>{
            console.info("===ACTS_PublishSlotTypeOther_0400 err===>"+err.code);
        });
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("===ACTS_PublishSlotTypeOther_0400 setTimeout unsubscribe end===>");
            done();
        }),timeout);
    })

    function onConsumeY(data) {
        console.info("===ACTS_PublishSlotTypeOther_0500 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0500 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishSlotTypeOther_0500");
        console.info("===ACTS_PublishSlotTypeOther_0500 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0500
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called, the label is wrong and the ID is correct.
     */
    it('ACTS_PublishSlotTypeOther_xts_0500', 0, async function (done) {
        console.info("===============ACTS_PublishSlotTypeOther_0500 start==========================>");
        var subscriber ={
            onConsume:onConsumeY
        }
        await notify.subscribe(subscriber);
        console.info("==============ACTS_PublishSlotTypeOther_0500 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 17,
            slotType : notify.SlotType.CONTENT_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==============ACTS_PublishSlotTypeOther_0500 publish promise==================>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeOther_0500 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeZ(data) {
        console.info("===ACTS_PublishSlotTypeOther_0600 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0600 onConsume data:===>" + JSON.stringify(data));
        console.info("===ACTS_PublishSlotTypeOther_0600 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0600
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void> interface is called,
                 the label is wrong and the ID is correct.
     */
    it('ACTS_PublishSlotTypeOther_xts_0600', 0, async function (done) {
        console.info("===============ACTS_PublishSlotTypeOther_0600 start==========================>");
        var subscriber ={
            onConsume:onConsumeZ
        }
        await notify.subscribe(subscriber);
        console.info("==============ACTS_PublishSlotTypeOther_0600 subscribe promise==================>");
        var notificationRequest = {
            content:{
            //                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 18,
            slotType : notify.SlotType.CONTENT_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        try {
            await notify.publish(notificationRequest);
        }catch(err){
            console.info("===ACTS_PublishSlotTypeOther_0600 publish promise===>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeOther_0600 setTimeout unsubscribe end===>");
            done();
        }),timeout);
    })

    function onConsumea(data) {
        console.info("===ACTS_PublishSlotTypeOther_0700 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0700 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishSlotTypeOther_0700");
        console.info("===ACTS_PublishSlotTypeOther_0700 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0700
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called, and the label uses empty characters
     */
    it('ACTS_PublishSlotTypeOther_xts_0700', 0, async function (done) {
        console.info("===============ACTS_PublishSlotTypeOther_0700 start==========================>");
        var subscriber ={
            onConsume:onConsumea
        }
        await notify.subscribe(subscriber);
        console.info("=============ACTS_PublishSlotTypeOther_0700 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 19,
        //            slotType : notify.SlotType.CONTENT_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0700",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==========ACTS_PublishSlotTypeOther_0700 publish promise==============>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeOther_0700 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })

    function onConsumeb(data) {
        console.info("===ACTS_PublishSlotTypeOther_0800 onConsume start===>");
        console.info("===ACTS_PublishSlotTypeOther_0800 onConsume data:===>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("ACTS_PublishSlotTypeOther_0800");
        console.info("===ACTS_PublishSlotTypeOther_0800 onConsume end===>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_xts_0800
     * @tc.name: cancel(id: number, label?: string): Promise<void>;
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void> interface is called,
                 and the label uses empty characters
     */
    it('ACTS_PublishSlotTypeOther_xts_0800', 0, async function (done) {
        console.info("===============ACTS_PublishSlotTypeOther_0800 start==========================>");
        var subscriber ={
            onConsume:onConsumeb
        }
        await notify.subscribe(subscriber);
        console.info("=========ACTS_PublishSlotTypeOther_0800 subscribe promise==================>");
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 20,
            slotType : notify.SlotType.SERVICE_INFORMATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "ACTS_PublishSlotTypeOther_0800",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ACTS_PublishSlotTypeOther_0800 publish promise===============>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeOther_0800 setTimeout unsubscribe end==================>");
            done();
        }),timeout);
    })
    function publishSlotSocialCallback001(error){
        console.log('=========ACTS_PublishSlotTypeSocial_0100 publish callback==========>'+JSON.stringify(error.code));
    }
    function onConsume0100(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0100 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0100 onConsume data:===========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test37_title");
        console.info("===========ACTS_PublishSlotTypeSocial_0100 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0100
     * @tc.name: publish()
     * @tc.desc: verify publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SOCIAL_COMMUNICATION)
     */
    it('ACTS_PublishSlotTypeSocial_xts_0100', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0100================>");
        var subscriber ={
            onConsume:onConsume0100
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeSocial_0100 subscribe======>");
        await notify.publish({
            id: 37,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test37_title",
                    text: "test37_text",
                    additionalText: "test37_additionalText"
                },
                slotType:notify.SlotType.SOCIAL_COMMUNICATION
            }
        },publishSlotSocialCallback001);
        console.info("==========ACTS_PublishSlotTypeSocial_0100 publish======>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeSocial_0100 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function publishSlotSocialCallback002(error){
        console.log('=========ACTS_PublishSlotTypeSocial_0200 publish callback==========>'+JSON.stringify(error.code));
    }

    function onConsume0200(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0200 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0200 onConsume data:===========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("===========ACTS_PublishSlotTypeSocial_0200 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SOCIAL_COMMUNICATION)
     */
    it('ACTS_PublishSlotTypeSocial_xts_0200', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0200================>");
        var subscriber ={
            onConsume:onConsume0200
        }
 	await notify.subscribe(subscriber);
        try {
            await notify.publish({
                id: 38,
                content: {
                // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                    normal: {
                        title: "test_title",
                        text: "test_text",
                        additionalText: "test_additionalText"
                    },
                    slotType: notify.SlotType.SOCIAL_COMMUNICATION
                }
            }, publishSlotSocialCallback002);
        }catch(err){
          console.info("==========ACTS_PublishSlotTypeSocial_0200 publish======>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeSocial_0200 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function publishSlotSocialCallback003(error){
        console.log('=========ACTS_PublishSlotTypeSocial_0300 publish callback==========>'+JSON.stringify(error.code));
    }

    function onConsume0300(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0300 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0300 onConsume data:===========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test39_title");
        console.info("===========ACTS_PublishSlotTypeSocial_0300 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)
     */
    it('ACTS_PublishSlotTypeSocial_xts_0300', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0300================>");
        var subscriber ={
            onConsume:onConsume0300
        }
 	await notify.subscribe(subscriber);
        await notify.publish({
            id: 39,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test39_title",
                    text: "test39_text",
                    additionalText: "test39_additionalText"
                },
            // slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        },publishSlotSocialCallback003);
        console.info("==========ACTS_PublishSlotTypeSocial_0300 publish======>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeSocial_0300 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function publishSlotSocialCallback004(error){
        console.log('=========ACTS_PublishSlotTypeSocial_0400 publish callback==========>'+JSON.stringify(error.code));
    }

    function onConsume0400(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0400 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0400 onConsume data:===========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test40_title");
        console.info("===========ACTS_PublishSlotTypeSocial_0400 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0400
     * @tc.name: publish()
     * @tc.desc: verify the publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:CONTENT_INFORMATION)
     */
    it('ACTS_PublishSlotTypeSocial_xts_0400', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0400================>");
        var subscriber ={
            onConsume:onConsume0400
        }
 	await notify.subscribe(subscriber);
        await notify.publish({
            id: 40,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test40_title",
                    text: "test40_text",
                    additionalText: "test40_additionalText"
                },
                slotType:notify.SlotType.CONTENT_INFORMATION
            }
        },publishSlotSocialCallback004);
        console.info("==========ACTS_PublishSlotTypeSocial_0400 publish======>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeSocial_0400 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0500(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0500 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0500 onConsume data:===========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test41_title");
        console.info("===========ACTS_PublishSlotTypeSocial_0500 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0500
     * @tc.name: publish()
     * @tc.desc: verify publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SOCIAL_COMMUNICATION) promise
     */
    it('ACTS_PublishSlotTypeSocial_xts_0500', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0500================>");
        var subscriber ={
            onConsume:onConsume0500
        }
 	await notify.subscribe(subscriber);
        notify.publish({
            id: 41,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test41_title",
                    text: "test41_text",
                    additionalText: "test41_additionalText"
                },
                slotType:notify.SlotType.SOCIAL_COMMUNICATION
            }
        }).then(console.info("==========ACTS_PublishSlotTypeSocial_0500 publish then======>"));
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeSocial_0500 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0600(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0600 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0600 onConsume data:===========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("===========ACTS_PublishSlotTypeSocial_0600 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SOCIAL_COMMUNICATION) promise
     */
    it('ACTS_PublishSlotTypeSocial_xts_0600', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0600================>");
        var subscriber ={
            onConsume:onConsume0600
        }
 	await notify.subscribe(subscriber);
        try {
            var promise = notify.publish({
                id: 42,
                content: {
                // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                    normal: {
                        title: "test_title",
                        text: "test_text",
                        additionalText: "test_additionalText"
                    },
                    slotType: notify.SlotType.SOCIAL_COMMUNICATION
                }
            })
        }catch(err){
            console.info("======ACTS_PublishSlotTypeSocial_0600 err===>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeSocial_0600 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0700(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0700 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0700 onConsume data:===========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test43_title");
        console.info("===========ACTS_PublishSlotTypeSocial_0700 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0700
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT) promise
     */
    it('ACTS_PublishSlotTypeSocial_xts_0700', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0700================>");
        var subscriber ={
            onConsume:onConsume0700
        }
 	await notify.subscribe(subscriber);
       var notificationRequest = {
            id: 43,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test43_title",
                    text: "test43_text",
                    additionalText: "test43_additionalText"
                },
            //slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        }
        await notify.publish(notificationRequest);
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeSocial_0700 setTimeout unsubscribe===>");
            done();
        }),timeout)
    });

    function onConsume0800(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0800 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0800 onConsume data:===========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test44_title");
        console.info("===========ACTS_PublishSlotTypeSocial_0800 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_xts_0800
     * @tc.name: publish()
     * @tc.desc: verify the publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:CONTENT_INFORMATION) promise
     */
    it('ACTS_PublishSlotTypeSocial_xts_0800', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeSocial_0800================>");
        var subscriber ={
            onConsume:onConsume0800
        }
 	await notify.subscribe(subscriber);
        await notify.publish({
            id: 44,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test44_title",
                    text: "test44_text",
                    additionalText: "test44_additionalText"
                },
                slotType:notify.SlotType.CONTENT_INFORMATION
            }
        })
         setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("===============ACTS_PublishSlotTypeSocial_0800 setTimeout unsubscribe================>");
            done();
        }),timeout)
    });

    function publishSlotServiceCallback001(error){
        console.log('=========ACTS_PublishSlotTypeSocial_0100 publish callback==========>'+JSON.stringify(error.code));
    }
    function onConsume0900(data){
        console.info("===========ACTS_PublishSlotTypeSocial_0100 onConsume start===========>");
        console.info("===========ACTS_PublishSlotTypeSocial_0100 onConsume data:===========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test29_title");
        console.info("===========ACTS_PublishSlotTypeSocial_0100 onConsume end=============>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0100
     * @tc.name: publish()
     * @tc.desc: verify the publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SERVICE_INFORMATION)
     */
    it('ACTS_PublishSlotTypeService_xts_0100', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0100================>");
        var subscriber ={
            onConsume:onConsume0900
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0100 subscribe======>");
        await notify.publish({
            id:29,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test29_title",
                    text: "test29_text",
                    additionalText: "test29_additionalText"
                },
            },
            slotType:notify.SlotType.SERVICE_INFORMATION
        },publishSlotServiceCallback001);
        console.info("==========ACTS_PublishSlotTypeService_0100 publish======>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0100 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function publishSlotServiceCallback002(error){
        console.log('========ACTS_PublishSlotTypeService_0200 publish callback========>'+JSON.stringify(error.code));
    }
    function onConsume1000(data){
        console.info("=======ACTS_PublishSlotTypeService_0200 onConsume start=========>");
        console.info("===========ACTS_PublishSlotTypeService_0200 onConsume data:=========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("===========ACTS_PublishSlotTypeService_0200 onConsume end===========>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SERVICE_INFORMATION)
     */
    it('ACTS_PublishSlotTypeService_xts_0200', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0200================>");
        var subscriber ={
            onConsume:onConsume1000
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0200 subscribe======>");
        try {
            await notify.publish({
                id: 30,
                content: {
                // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                    normal: {
                        title: "test_title",
                        text: "test_text",
                        additionalText: "test_additionalText"
                    },
                },
                slotType: notify.SlotType.SERVICE_INFORMATION
            }, publishSlotServiceCallback002);
        }catch(err){
            console.info("==========ACTS_PublishSlotTypeService_0200 err======>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0200 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function publishSlotServiceCallback003(error){
        console.log('========ACTS_PublishSlotTypeService_0300 publish callback========>'+JSON.stringify(error.code));
    }
    function onConsume1100(data){
        console.info("=======ACTS_PublishSlotTypeService_0300 onConsume start=========>");
        console.info("=======ACTS_PublishSlotTypeService_0300 onConsume data:=========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test31_title");
        console.info("=======ACTS_PublishSlotTypeService_0300 onConsume end===========>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)
     */
    it('ACTS_PublishSlotTypeService_xts_0300', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0300================>");
        var subscriber ={
            onConsume:onConsume1100
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0300 subscribe======>");
        await notify.publish({
            id: 31,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test31_title",
                    text: "test31_text",
                    additionalText: "test31_additionalText"
                },
            },
        // slotType:notification.SlotType.SERVICE_INFORMATION
        },publishSlotServiceCallback003);
        console.info("==========ACTS_PublishSlotTypeService_0300 publish======>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0300 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function publishSlotServiceCallback004(error){
        console.log('========ACTS_PublishSlotTypeService_0400 publish callback========>'+JSON.stringify(error.code));
    }
    function onConsume1200(data){
        console.info("=======ACTS_PublishSlotTypeService_0400 onConsume start=========>");
        console.info("=======ACTS_PublishSlotTypeService_0400 onConsume data:=========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test32_title");
        console.info("=======ACTS_PublishSlotTypeService_0400 onConsume end===========>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0400
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:UNKNOWN_TYPE)
     */
    it('ACTS_PublishSlotTypeService_xts_0400', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0400================>");
        var subscriber ={
            onConsume:onConsume1200
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0400 subscribe======>");
        await notify.publish({
            id: 32,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test32_title",
                    text: "test32_text",
                    additionalText: "test32_additionalText"
                },
            },
            slotType:notify.SlotType.UNKNOWN_TYPE
        },publishSlotServiceCallback004);
        console.info("==========ACTS_PublishSlotTypeService_0400 publish======>");
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0400 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume1300(data){
        console.info("=======ACTS_PublishSlotTypeService_0500 onConsume start=========>");
        console.info("=======ACTS_PublishSlotTypeService_0500 onConsume data:=========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test33_title");
        console.info("=======ACTS_PublishSlotTypeService_0500 onConsume end===========>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0500
     * @tc.name: publish()
     * @tc.desc: verify the publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SERVICE_INFORMATION) promise
     */
    it('ACTS_PublishSlotTypeService_xts_0500', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0500================>");
        var subscriber ={
            onConsume:onConsume1300
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0500 subscribe======>");
        notify.publish({
            id: 33,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test33_title",
                    text: "test33_text",
                    additionalText: "test33_additionalText"
                },
            },
            slotType:notify.SlotType.SERVICE_INFORMATION
        }).then(console.log("==========ACTS_PublishSlotTypeService_0500 publish then======>"))
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0500 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume1400(data){
        console.info("=======ACTS_PublishSlotTypeService_0600 onConsume start=========>");
        console.info("=======ACTS_PublishSlotTypeService_0600 onConsume data:=========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("=======ACTS_PublishSlotTypeService_0600 onConsume end===========>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SERVICE_INFORMATION) promise
     */
    it('ACTS_PublishSlotTypeService_xts_0600', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0600================>");
        var subscriber ={
            onConsume:onConsume1400
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0600 subscribe======>");
        try {
            var promise = notify.publish({
                id: 34,
                content: {
                // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                    normal: {
                        title: "test_title",
                        text: "test_text",
                        additionalText: "test_additionalText"
                    },
                },
                slotType: notify.SlotType.SERVICE_INFORMATION
            })
        }catch(err){
            console.info("======ACTS_PublishSlotTypeService_0600 err===>"+err.code);
        }
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0600 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume1500(data){
        console.info("=======ACTS_PublishSlotTypeService_0700 onConsume start=========>");
        console.info("=======ACTS_PublishSlotTypeService_0700 onConsume data:=========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test35_title");
        console.info("=======ACTS_PublishSlotTypeService_0700 onConsume end===========>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0700
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT) promise
     */
    it('ACTS_PublishSlotTypeService_xts_0700', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0700================>");
        var subscriber ={
            onConsume:onConsume1500
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0700 subscribe======>");
        notify.publish({
            id: 35,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test35_title",
                    text: "test35_text",
                    additionalText: "test35_additionalText"
                },
            },
        // slotType:notification.SlotType.SERVICE_INFORMATION
        }).then(console.log("===ACTS_PublishSlotTypeService_0700 finished==="));
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0700 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume1600(data){
        console.info("=======ACTS_PublishSlotTypeService_0800 onConsume start=========>");
        console.info("=======ACTS_PublishSlotTypeService_0800 onConsume data:=========>" + JSON.stringify(data));
        expect(data.request.content.normal.title).assertEqual("test36_title");
        console.info("=======ACTS_PublishSlotTypeService_0800 onConsume end===========>");
    }

    /*
     * @tc.number: ACTS_PublishSlotTypeService_xts_0800
     * @tc.name: publish()
     * @tc.desc: verify publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:UNKNOWN_TYPE) promise
     */
    it('ACTS_PublishSlotTypeService_xts_0800', 0,async function (done) {
        console.info("===============ACTS_PublishSlotTypeService_0800================>");
        var subscriber ={
            onConsume:onConsume1600
        }
        await notify.subscribe(subscriber);
        console.info("===============ACTS_PublishSlotTypeService_0800 subscribe======>");
        notify.publish({
            id: 36,
            content: {
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test36_title",
                    text: "test36_text",
                    additionalText: "test36_additionalText"
                },
            },
            slotType:notify.SlotType.UNKNOWN_TYPE
        }).then(console.log("===ACTS_PublishSlotTypeService_0800 finished==="))
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info("======ACTS_PublishSlotTypeService_0800 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });
}) 
