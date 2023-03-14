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
var timeout = 500;
describe('ActsAnsCancelGroup', function () {
    console.info("===========ActsAnsCancelGroup start====================>");

    var bundleOption = {
        bundle:"com.example.actsanscancelgroup",
    };

    function onConsumeCancelGroup(data) {
        console.info("=========ANS_CancelGroup_0100 onConsume start==============>");
        console.info("=========ANS_CancelGroup_0100 onConsume data:==============>" + JSON.stringify(data));
        notify.cancelGroup("group_0100", CancelGroupCallbackOne);
        console.info("=========ANS_CancelGroup_0100 onConsume cancelGroup======>");
        console.info("=========ANS_CancelGroup_0100 onConsume end================>");
    }

    function onCancelCancelGroup(data) {
        console.info("========ANS_CancelGroup_0100 onCancel start===========>");
        console.info("========ANS_CancelGroup_0100 onCancel data:===========>" + JSON.stringify(data));
        expect(data.request.id).assertEqual(1);
        console.info("========ANS_CancelGroup_0100 onCancel id:=============>" + data.request.id);
        expect(data.request.label).assertEqual("0100");
        console.info("========ANS_CancelGroup_0100 onCancel label:==========>" + data.request.label);
        expect(data.request.groupName).assertEqual("group_0100");
        console.info("========ANS_CancelGroup_0100 onCancel groupName:======>" + data.request.groupName);
        console.info("========ANS_CancelGroup_0100 onCancel end=============>");
    }

    function CancelGroupCallbackOne(err){
        console.info("========ANS_CancelGroup_0100 CancelGroupCallbackOne start==>");
        console.info("========ANS_CancelGroup_0100 CancelGroupCallbackOne err====>"+JSON.stringify(err));
        console.info("========ANS_CancelGroup_0100 CancelGroupCallbackOne end====>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0100
     * @tc.name: cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify : one notification of the specified group can be successfully canceled(callback)
     */
    it('ANS_CancelGroup_0100', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0100 start==================>");
        var subscriber = {
            onConsume:onConsumeCancelGroup,
            onCancel:onCancelCancelGroup
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_0100",
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
            label: "0100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_0100 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_0100 publish promise========>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0100 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_0100 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })

    function onConsumeCancelGroupPromise(data) {
        console.info("=========ANS_CancelGroup_0200 onConsume start==============>");
        console.info("=========ANS_CancelGroup_0200 onConsume data:==============>" + JSON.stringify(data));
        notify.cancelGroup("group_0200");
        console.info("=========ANS_CancelGroup_0200 onConsume cancelGroup======>");
        console.info("=========ANS_CancelGroup_0200 onConsume end================>");
    }

    function onCancelCancelGroupPromise(data) {
        console.info("========ANS_CancelGroup_0200 onCancel start===========>");
        console.info("========ANS_CancelGroup_0200 onCancel data:===========>" + JSON.stringify(data));
        expect(data.request.id).assertEqual(2);
        console.info("========ANS_CancelGroup_0200 onCancel id:=============>" + data.request.id);
        expect(data.request.label).assertEqual("0200");
        console.info("========ANS_CancelGroup_0200 onCancel label:==========>" + data.request.label);
        expect(data.request.groupName).assertEqual("group_0200");
        console.info("========ANS_CancelGroup_0200 onCancel groupName:======>" + data.request.groupName);
        console.info("========ANS_CancelGroup_0200 onCancel end=============>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0200
     * @tc.name: cancelGroup(groupName: string): Promise<void>;
     * @tc.desc: Verify : one notification of the specified group can be successfully cancel(promise)
     */
    it('ANS_CancelGroup_0200', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0200 start==================>");
        var subscriber ={
            onConsume:onConsumeCancelGroupPromise,
            onCancel:onCancelCancelGroupPromise
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_0200",
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
            label: "0200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_0200 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_0200 publish promise========>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0200 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_0200 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })

    var timesOfOnConsume

    function onConsumeCancelGroupTwo(data) {
        console.info("=========ANS_CancelGroup_0300 onConsume start==============>");
        console.info("=========ANS_CancelGroup_0300 onConsume data:==============>" + JSON.stringify(data));
        timesOfOnConsume = timesOfOnConsume + 1
        if (timesOfOnConsume == 2){
            notify.cancelGroup("group_0300", cancelGroupCallbackTwo);
            console.info("=========ANS_CancelGroup_0300 onConsume cancelGroup======>");
        }
        console.info("=========ANS_CancelGroup_0300 onConsume end================>");
    }

    var timesOfOnCancel

    function onCancelCancelGroupTwo(data) {
        console.info("========ANS_CancelGroup_0300 onCancel start===========>");
        console.info("========ANS_CancelGroup_0300 onCancel data:===========>" + JSON.stringify(data));
        timesOfOnCancel = timesOfOnCancel + 1
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(1);
        } else if (timesOfOnCancel == 2){
            expect(data.request.id).assertEqual(2);
        }
        console.info("========ANS_CancelGroup_0300 onCancel end===========>");
    }

    function cancelGroupCallbackTwo(err){
        console.info("========ANS_CancelGroup_0300 cancelGroupCallbackTwo start====>");
        console.info("========ANS_CancelGroup_0300 cancelGroupCallbackTwo err======>" + JSON.stringify(err));
        console.info("========ANS_CancelGroup_0300 cancelGroupCallbackTwo end======>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0300
     * @tc.name: cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify : 2 notifications of the specified group can be successfully canceled(callback)
     */
    it('ANS_CancelGroup_0300', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0300 start==================>");
        timesOfOnConsume = 0
        timesOfOnCancel = 0
        var subscriber ={
            onConsume:onConsumeCancelGroupTwo,
            onCancel:onCancelCancelGroupTwo,
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_0300",
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
            label: "0300_1",
            badgeIconStyle: 1,
            showDeliveryTime: true,
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
            groupName:"group_0300",
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
            label: "0300_2",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_0300 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_0300 publish1 promise=======>");
        notify.publish(notificationRequest1);
        console.info("===========ANS_CancelGroup_0300 publish2 promise=======>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0300 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_0300 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })

    function onConsumeCancelGroupPromiseTwo(data) {
        console.info("=========ANS_CancelGroup_0400 onConsume start==============>");
        console.info("=========ANS_CancelGroup_0400 onConsume data:==============>" + JSON.stringify(data));
        timesOfOnConsume = timesOfOnConsume + 1
        if (timesOfOnConsume == 2){
            notify.cancelGroup("group_0400");
            console.info("=========ANS_CancelGroup_0400 onConsume cancelGroup Promise======>");
        }
        console.info("=========ANS_CancelGroup_0400 onConsume end================>");
    }

    function onCancelCancelGroupPromiseTwo(data) {
        console.info("========ANS_CancelGroup_0400 onCancel start===========>");
        console.info("========ANS_CancelGroup_0400 onCancel data:===========>" + JSON.stringify(data));
        timesOfOnCancel = timesOfOnCancel + 1
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(1);
        } else if (timesOfOnCancel == 2){
            expect(data.request.id).assertEqual(2);
        }
        console.info("========ANS_CancelGroup_0400 onCancel end===========>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0400
     * @tc.name: cancelGroup(groupName: string): Promise<void>;
     * @tc.desc: Verify : 2 notifications of the specified group can be successfully canceled(promise)
     */
    it('ANS_CancelGroup_0400', 0, async function (done) {
        console.info("===============ANS_CancelGroup_0400 start==================>");
        timesOfOnConsume = 0
        timesOfOnCancel = 0
        var subscriber ={
            onConsume:onConsumeCancelGroupPromiseTwo,
            onCancel:onCancelCancelGroupPromiseTwo,
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_0400",
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
            label: "0400_1",
            badgeIconStyle: 1,
            showDeliveryTime: true,
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
            groupName:"group_0400",
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
            label: "0400_2",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_0400 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_0400 publish1 promise=======>");
        notify.publish(notificationRequest1);
        console.info("===========ANS_CancelGroup_0400 publish2 promise=======>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0400 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_0400 setTimeout unsubscribe==>");
            done();
        }),timeout);
    });

    function onConsumeCancelGroupThree(data) {
        console.info("=========ANS_CancelGroup_0500 onConsume start==============>");
        console.info("=========ANS_CancelGroup_0500 onConsume data:==============>" + JSON.stringify(data));
        notify.cancelGroup("group_0500", cancelGroupByBundleCallbackThree);
        console.info("=========ANS_CancelGroup_0500 onConsume cancelGroup======>");
        console.info("=========ANS_CancelGroup_0500 onConsume end================>");
    }

    function onCancelCancelGroupThree(data) {
        timesOfOnCancel = timesOfOnCancel + 1;
        console.info("========ANS_CancelGroup_0500 onCancel start===========>");
        console.info("========ANS_CancelGroup_0500 onCancel data:===========>" + JSON.stringify(data));
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(5);
            console.info("========ANS_CancelGroup_0500 onCancel id:=============>" + data.request.id);
            expect(data.request.label).assertEqual("0500");
            console.info("========ANS_CancelGroup_0500 onCancel label:==========>" + data.request.label);
            expect(data.request.groupName).assertEqual("group_0500");
            console.info("========ANS_CancelGroup_0500 onCancel groupName:======>" + data.request.groupName);
            notify.cancelGroup("group_0500", cancelGroupByBundleCallbackThree2);
            console.info("========ANS_CancelGroup_0500 onCancel cancelGroup:======>");
        } else if (timesOfOnCancel == 2){
            expect.assertFail();
        }
        console.info("========ANS_CancelGroup_0500 onCancel end=============>");
    }

    function cancelGroupByBundleCallbackThree(err){
        console.info("========ANS_CancelGroup_0500 cancelGroupByBundleCallbackThree start==>");
        console.info("========ANS_CancelGroup_0500 cancelGroupByBundleCallbackThree err====>"+JSON.stringify(err));
        console.info("========ANS_CancelGroup_0500 cancelGroupByBundleCallbackThree end====>");
    }

    function cancelGroupByBundleCallbackThree2(err){
        console.info("========ANS_CancelGroup_0500 cancelGroupByBundleCallbackThree2 start==>");
        console.info("========ANS_CancelGroup_0500 cancelGroupByBundleCallbackThree2 err====>"+JSON.stringify(err));
        console.info("========ANS_CancelGroup_0500 cancelGroupByBundleCallbackThree2 end====>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0500
     * @tc.name: cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify : After the notification of a specific group is successfully canceled,
                          cancel the notification of this group again(callback)
     */
    it('ANS_CancelGroup_0500', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0500 start==================>");
        timesOfOnCancel = 0;
        var subscriber ={
            onConsume:onConsumeCancelGroupThree,
            onCancel:onCancelCancelGroupThree
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_0500",
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
            label: "0500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_0500 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_0500 publish promise========>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0500 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_0500 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })

    function onConsumeCancelGroupPromiseThree(data) {
        console.info("=========ANS_CancelGroup_0600 onConsume start==============>");
        console.info("=========ANS_CancelGroup_0600 onConsume data:==============>" + JSON.stringify(data));
        notify.cancelGroup("group_0600");
        console.info("=========ANS_CancelGroup_0600 onConsume cancelGroup======>");
        console.info("=========ANS_CancelGroup_0600 onConsume end================>");
    }

    function onCancelCancelGroupPromiseThree(data) {
        timesOfOnCancel = timesOfOnCancel + 1;
        console.info("========ANS_CancelGroup_0600 onCancel start===========>");
        console.info("========ANS_CancelGroup_0600 onCancel data:===========>" + JSON.stringify(data));
        if (timesOfOnCancel == 1) {
            expect(data.request.id).assertEqual(6);
            console.info("========ANS_CancelGroup_0600 onCancel id:=============>" + data.request.id);
            expect(data.request.label).assertEqual("0600");
            console.info("========ANS_CancelGroup_0600 onCancel label:==========>" + data.request.label);
            expect(data.request.groupName).assertEqual("group_0600");
            console.info("========ANS_CancelGroup_0600 onCancel groupName:======>" + data.request.groupName);
            notify.cancelGroup("group_0600");
            console.info("========ANS_CancelGroup_0600 onCancel cancelGroup:==========>");
        }else if(timesOfOnCancel == 2){
            expect.assertFail();
        }
        console.info("========ANS_CancelGroup_0600 onCancel end=============>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0600
     * @tc.name: cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify : After the notification of a specific group is successfully canceled,
                          cancel the notification of this group again(promise)
     */
    it('ANS_CancelGroup_0600', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0700 start==================>");
        timesOfOnCancel = 0;
        var subscriber ={
            onConsume:onConsumeCancelGroupPromiseThree,
            onCancel:onCancelCancelGroupPromiseThree
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_0600",
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
            label: "0600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_0600 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_0600 publish promise========>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0600 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_0600 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })

    function cancelGroupCallbackFour(err){
        console.info("========ANS_CancelGroup_0700 cancelGroupCallbackFour start==>");
        console.info("========ANS_CancelGroup_0700 cancelGroupCallbackFour err====>"+JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("========ANS_CancelGroup_0700 cancelGroupCallbackFour end====>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0700
     * @tc.name: cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify : Call the interface with an empty groupName parameter(callback)
     */
    it('ANS_CancelGroup_0700', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0700 start==================>");
        notify.cancelGroup("", cancelGroupCallbackFour);
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0700 setTimeout==============>");
            done();
        }),timeout);
    })

    /*
     * @tc.number: ANS_CancelGroup_0800
     * @tc.name: cancelGroup(groupName: string): Promise<void>;
     * @tc.desc: Verify : Call the interface with an empty groupName parameter(promise)
     */
    it('ANS_CancelGroup_0800', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0800 start==================>");
        notify.cancelGroup("").then(()=>{
            console.info("==================ANS_CancelGroup_0800 cancelGroup then==================>");
        }).catch((err)=>{
            console.info("==================ANS_CancelGroup_0800 cancelGroup catch==================>");
            console.info("==================ANS_CancelGroup_0800 cancelGroup err==================>"+err.code);
            expect(err.code != 0).assertEqual(true);
        });
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0800 setTimeout==============>");
            done();
        }),timeout);
    })

    function onConsumCancelGroupFour(data) {
        console.info("=========ANS_CancelGroup_0900 onConsume start==============>");
        console.info("=========ANS_CancelGroup_0900 onConsume data:==============>" + JSON.stringify(data));
        notify.cancelGroup("group_wrong", cancelGroupCallbackFive);
        console.info("=========ANS_CancelGroup_0900 onConsume cancelGroup======>");
        console.info("=========ANS_CancelGroup_0900 onConsume end================>");
    }

    function onCancelCancelGroupFour(data) {
        expect().assertFail();
        console.info("========ANS_CancelGroup_0900 onCancel start===========>");
        console.info("========ANS_CancelGroup_0900 onCancel data:===========>" + JSON.stringify(data));
        console.info("========ANS_CancelGroup_0900 onCancel end=============>");
    }

    function cancelGroupCallbackFive(err){
        console.info("========ANS_CancelGroup_0900 cancelGroupCallbackFive start==>");
        console.info("========ANS_CancelGroup_0900 cancelGroupCallbackFive err====>"+JSON.stringify(err));
        console.info("========ANS_CancelGroup_0900 cancelGroupCallbackFive end====>");
    }

    /*
     * @tc.number: ANS_CancelGroup_0900
     * @tc.name: cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify : Call the interface with an wrong groupName parameter(callback)
     */
    it('ANS_CancelGroup_0900', 0, async function (done) {
        console.info("==================ANS_CancelGroup_0900 start==================>");
        var subscriber ={
            onConsume:onConsumCancelGroupFour,
            onCancel:onCancelCancelGroupFour
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_0900",
            id: 9,
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
            label: "0900",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_0900 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_0900 publish promise========>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_0900 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_0900 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })

    function onConsumeCancelGroupPromiseFour(data) {
        console.info("=========ANS_CancelGroup_1000 onConsume start==============>");
        console.info("=========ANS_CancelGroup_1000 onConsume data:==============>" + JSON.stringify(data));
        notify.cancelGroup("group_wrong");
        console.info("=========ANS_CancelGroup_1000 onConsume cancelGroup======>");
        console.info("=========ANS_CancelGroup_1000 onConsume end================>");
    }

    function onCancelCancelGroupPromiseFour(data) {
        expect().assertFail();
        console.info("========ANS_CancelGroup_1000 onCancel start===========>");
        console.info("========ANS_CancelGroup_1000 onCancel data:===========>" + JSON.stringify(data));
        console.info("========ANS_CancelGroup_1000 onCancel end=============>");
    }

    /*
     * @tc.number: ANS_CancelGroup_1000
     * @tc.name: cancelGroup(groupName: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify : Verify : Call the interface with an wrong groupName parameter(promise)
     */
    it('ANS_CancelGroup_1000', 0, async function (done) {
        console.info("==================ANS_CancelGroup_1000 start==================>");
        var subscriber ={
            onConsume:onConsumeCancelGroupPromiseFour,
            onCancel:onCancelCancelGroupPromiseFour
        }
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            groupName:"group_1000",
            id: 10,
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
            label: "1000",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.subscribe(subscriber);
        console.info("===========ANS_CancelGroup_1000 subscribe promise======>");
        notify.publish(notificationRequest);
        console.info("===========ANS_CancelGroup_1000 publish promise========>");
        setTimeout((async function(){
            console.info("======ANS_CancelGroup_1000 setTimeout==============>");
            notify.unsubscribe(subscriber);
            console.info("======ANS_CancelGroup_1000 setTimeout unsubscribe==>");
            done();
        }),timeout);
    })
})