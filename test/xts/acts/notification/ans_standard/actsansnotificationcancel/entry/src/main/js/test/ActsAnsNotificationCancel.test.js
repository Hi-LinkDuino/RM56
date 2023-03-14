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
let timeout = 500;
describe('ActsAnsNotificationCancel', function () {
    console.info('===========ActsAnsNotificationCancel start====================>');
    let timesOfOnConsume
    function onConsumeCancelAll(data) {
        console.info('================ANS_Cancel_0100 onConsume start=======================>');
        console.info('================ANS_Cancel_0100 onConsume data:=================>' + JSON.stringify(data));
        timesOfOnConsume = timesOfOnConsume + 1
        if (timesOfOnConsume == 2){
            notify.cancelAll(cancelAllCallBack);
            console.info('================ANS_Cancel_0100 onConsume cancelAll=======================>');
        }
        console.info('================ANS_Cancel_0100 onConsume end=======================>');
    }
    let timesOfOnCancel
    function onCancelCancelAll(data) {
        console.info('================ANS_Cancel_0100 onCancel start===============>');
        console.info('================ANS_Cancel_0100 onCancel data:===============>' + JSON.stringify(data));
        timesOfOnCancel = timesOfOnCancel + 1
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(1);
        } else if (timesOfOnCancel == 2){
            expect(data.request.id).assertEqual(2);
        }
        console.info('================ANS_Cancel_0100 onCancel end=======================>');
    }
    function cancelAllCallBack(err, data){
        console.info('========ANS_Cancel_0100 cancelAllCallBack start==============>');
        console.info('========ANS_Cancel_0100 cancelAllCallBack data===============>' + JSON.stringify(data));
        console.info('========ANS_Cancel_0100 cancelAllCallBack err================>' + JSON.stringify(err));
        console.info('========ANS_Cancel_0100 cancelAllCallBack end================>');
    }

    /*
     * @tc.number: ANS_Cancel_0100
     * @tc.name: cancelAll(callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the application successfully cancels all its published notifications by calling the
                 cancelAll(callback: AsyncCallback<void>) interface
     */
    it('ANS_Cancel_0100', 0, async function (done) {
        console.info('==================ANS_Cancel_0100 start==================>');
        timesOfOnConsume = 0
        timesOfOnCancel = 0
        let subscriber ={
            onConsume:onConsumeCancelAll,
            onCancel:onCancelCancelAll,
        }
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0100_1',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        let notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0100_2',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.subscribe(subscriber);
        console.info('===========ANS_Cancel_0100 subscribe promise=======>');
        await notify.publish(notificationRequest);
        console.info('===========ANS_Cancel_0100 publish1 promise=======>');
        await notify.publish(notificationRequest1);
        console.info('===========ANS_Cancel_0100 publish2 promise=======>');
        setTimeout((async function(){
            console.info('======ANS_Cancel_0100 setTimeout==================>');
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0100 setTimeout unsubscribe==================>');
            done();
        }),timeout);
    })


    function onConsumeCancelAllPromise(data) {
        timesOfOnConsume = timesOfOnConsume + 1
        console.info('================ANS_Cancel_0200 onConsume start===========>');
        console.info('================ANS_Cancel_0200 onConsume data:============>' + JSON.stringify(data));
        if (timesOfOnConsume == 2){
            notify.cancelAll();
            console.info('==========ANS_Cancel_0200 onConsume cancelAll promise==========>');
        }
        console.info('================ANS_Cancel_0200 onConsume end===============>');
    }

    function onCancelCancelAllPromise(data) {
        timesOfOnCancel = timesOfOnCancel + 1
        console.info('=========ANS_Cancel_0200 onCancel start==========>');
        console.info('=========ANS_Cancel_0200 onCancel data:==========>' + JSON.stringify(data));
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(1);
        } else if (timesOfOnCancel == 2){
            expect(data.request.id).assertEqual(2);
        }
        console.info('==========ANS_Cancel_0200 onCancel end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_0200
     * @tc.name: cancelAll(): Promise<void>;
     * @tc.desc: Verify that the application successfully cancels all its published notifications by
                 calling the  cancelAll(): Promise<void> interface
     */
    it('ANS_Cancel_0200', 0, async function (done) {
        console.info('===============ANS_Cancel_0200 start==========================>');
        timesOfOnConsume = 0
        timesOfOnCancel = 0
        let subscriber ={
            onConsume:onConsumeCancelAllPromise,
            onCancel:onCancelCancelAllPromise,
        }
        await notify.subscribe(subscriber);
        console.info('==================ANS_Cancel_0200 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0200',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        let notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0200',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==========ANS_Cancel_0200 publish1 promise==================>');
        await notify.publish(notificationRequest1);
        console.info('==========ANS_Cancel_0200 publish2 promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0200 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelAllNoNotify(data) {
        console.info('================ANS_Cancel_0300 onConsume start========>');
        console.info('================ANS_Cancel_0300 onConsume data:========>' + JSON.stringify(data));
        expect().assertFail();
        console.info('================ANS_Cancel_0300 onConsume end=======================>');
    }
    function onCancelCancelAllNoNotify(data) {
        console.info('=================ANS_Cancel_0300 onCancel start=============>');
        expect().assertFail();
        console.info('================ANS_Cancel_0300 onCancel data:==============>' + JSON.stringify(data));
        console.info('================ANS_Cancel_0300 onCancel end================>');
    }
    function cancelAllCallBackNoNotify(err, data){
        console.info('==========ANS_Cancel_0300 cancelAllCallBack start=============>');
        console.info('==========ANS_Cancel_0300 cancelAllCallBack data:=============>' + JSON.stringify(data));
        console.info('==========ANS_Cancel_0300 cancelAllCallBack  err:=============>' + JSON.stringify(err));
        console.info('==========ANS_Cancel_0300 cancelAllCallBack end===============>');
    }

    /*
     * @tc.number: ANS_Cancel_0300
     * @tc.name: cancelAll(callback: AsyncCallback<void>): void
     * @tc.desc: Verify that when there is no notification in the notification list,
                 call the cancelAll(callback: AsyncCallback<void>): void interface,
                 and the application cancels all its published notifications.
                 At this time, no notification information is cancelled,
                 and there is no OnCancel notification（Callback mode)
     */
    it('ANS_Cancel_0300', 0, async function (done) {
        console.info('===============ANS_Cancel_0300 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelAllNoNotify,
            onCancel:onCancelCancelAllNoNotify,
        }
        await notify.subscribe(subscriber);
        console.info('===============ANS_Cancel_0300 subscribe promise===============>');
        await notify.cancelAll(cancelAllCallBackNoNotify);
        console.info('===============ANS_Cancel_0300 cancelAll promise===============>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0300 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelAllNoNotifyPromise(data) {
        console.info('================ANS_Cancel_0400 onConsume===============>');
        console.info('================ANS_Cancel_0400 onConsume data:===========>' + JSON.stringify(data));
        expect().assertFail();
        console.info('================ANS_Cancel_0400 onConsume end===========>');
    }
    function onCancelCancelAllNoNotifyPromise(data) {
        console.info('===============ANS_Cancel_0400 onCancel ==================>');
        console.info('===============ANS_Cancel_0400 onCancel data==============>' + JSON.stringify(data));
        expect().assertFail();
        console.info('===============ANS_Cancel_0400 onCancel end===============>');
    }

    /*
     * @tc.number: ANS_Cancel_0400
     * @tc.name: cancelAll(): Promise<void>;
     * @tc.desc: Verify that when there is no notification in the notification list,
                 call the cancelAll(callback: AsyncCallback<void>): void interface,
                 and the application cancels all its published notifications.
                 At this time, no notification information is cancelled,
                 and there is no OnCancel notification（ Callback mode)
     */
    it('ANS_Cancel_0400', 0, async function (done) {
        console.info('===============ANS_Cancel_0400 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelAllNoNotifyPromise,
            onCancel:onCancelCancelAllNoNotifyPromise,
        }
        await notify.subscribe(subscriber);
        console.info('================ANS_Cancel_0400 subscribe promise=============>');
        await notify.cancelAll();
        console.info('================ANS_Cancel_0400 cancelAll promise=============>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0400 setTimeout unsubscribe end==================>');
            done();
        }),timeout);

    })

    function onConsumeCancelAll2Times(data) {
        console.info('==========ANS_Cancel_0500 onConsume start==============>');
        console.info('==========ANS_Cancel_0500 onConsume data:==============>' + JSON.stringify(data));
        notify.cancelAll(cancelAllCallBack2Times1);
        console.info('==========ANS_Cancel_0500 onConsume cancelAll=================>');
        console.info('==========ANS_Cancel_0500 onConsume end=======================>');
    }

    function onCancelCancelAll2Times(data) {
        console.info('=========ANS_Cancel_0500 onCancel start===============>');
        console.info('=========ANS_Cancel_0500 onCancel data:===============>' + JSON.stringify(data));
        timesOfOnCancel = timesOfOnCancel + 1
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(5);
        } else if (timesOfOnCancel){
            expect(data.request.id).assertEqual(0);
        }
        console.info('==========ANS_Cancel_0500 onCancel end===============>');
    }
    function cancelAllCallBack2Times1(err){
        console.info('==========ANS_Cancel_0500 cancelAllCallBack1 start==========>');
        console.info('==========ANS_Cancel_0500 cancelAllCallBack1 err:===========>' + JSON.stringify(err));
        notify.cancelAll(cancelAllCallBack2Times2);
        console.info('==========ANS_Cancel_0500 cancelAllCallBack1 cancelAll======>');
        console.info('==========ANS_Cancel_0500 cancelAllCallBack1  end===========>');
    }
    function cancelAllCallBack2Times2(err){
        console.info('==========ANS_Cancel_0500 cancelAllCallBack2 start=============>');
        console.info('==========ANS_Cancel_0500 cancelAllCallBack2 err:==============>' + JSON.stringify(err));
        console.info('==========ANS_Cancel_0500 cancelAllCallBack2 end===============>');
    }

    /*
     * @tc.number: ANS_Cancel_0500
     * @tc.name: cancelAll(callback: AsyncCallback<void>): void
     * @tc.desc: Verify that all notifications are cancelled successfully
                 by calling the cancelAll(callback: AsyncCallback<void>): void interface,
                 and then cancel the notification again
     */
    it('ANS_Cancel_0500', 0, async function (done) {
        console.info('============ANS_Cancel_0500 start==================>');
        timesOfOnCancel = 0
        let subscriber ={
            onConsume:onConsumeCancelAll2Times,
            onCancel:onCancelCancelAll2Times,
        }
        await notify.subscribe(subscriber);
        console.info('============ANS_Cancel_0500 subscribe promise======>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0500',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==============ANS_Cancel_0500 publish promise end==============>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0500 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelAll2TimesPromise(data) {
        console.info('==========ANS_Cancel_0600 onConsume start============>');
        console.info('==========ANS_Cancel_0600 onConsume data=============>' + JSON.stringify(data));
        notify.cancelAll();
        console.info('==========ANS_Cancel_0600 onConsume cancelAll 2times1 end======>');
        notify.cancelAll();
        console.info('==========ANS_Cancel_0600 onConsume cancelAll 2times2 end======>');
        console.info('==========ANS_Cancel_0600 onConsume end========>');
    }
    function onCancelCancelAll2TimesPromise(data) {
        timesOfOnCancel = timesOfOnCancel + 1
        console.info('==========ANS_Cancel_0600 onCancel===================>');
        console.info('==========ANS_Cancel_0600 onCancel data:============>' + JSON.stringify(data));
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(6);
        } else if (timesOfOnCancel == 2){
            expect().assertFail();
        }
        console.info('==========ANS_Cancel_0600 onCancel end=============>');
    }

    /*
     * @tc.number: ANS_Cancel_0600
     * @tc.name: cancelAll(): Promise<void>;
     * @tc.desc: Verify that all notifications are cancelled successfully by calling the
                 cancelAll(): Promise<void> interface, and then cancel the notification again
     */
    it('ANS_Cancel_0600', 0, async function (done) {
        console.info('===============ANS_Cancel_0600 start==========================>');
        timesOfOnCancel = 0
        let subscriber ={
            onConsume:onConsumeCancelAll2TimesPromise,
            onCancel:onCancelCancelAll2TimesPromise,
        }
        await notify.subscribe(subscriber);
        console.info('==================ANS_Cancel_0600 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0600',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==================ANS_Cancel_0600 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0600 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelAllIsUnremovable(data) {
        console.info('========ANS_Cancel_0700 onConsume start===========>');
        console.info('========ANS_Cancel_0700 onConsume data: ===========>' + JSON.stringify(data));
        notify.cancelAll(cancelAllCallBackIsUnremovable);
        console.info('========ANS_Cancel_0700 onConsume cancelAll===========>');
        console.info('========ANS_Cancel_0700 onConsume end============>');
    }
    function onCancelCancelAllIsUnremovable(data) {
        console.info('================ANS_Cancel_0700 onCancel start=======================>');
        console.info('================ANS_Cancel_0700 onCancel data:====================>' + JSON.stringify(data));
        expect(data.request.id).assertEqual(7);
        console.info('================ANS_Cancel_0700 onCancel end=======================>');
    }
    function cancelAllCallBackIsUnremovable(err, data){
        console.info('===========ANS_Cancel_0700 cancelAllCallBack start==========>');
        console.info('===========ANS_Cancel_0700 cancelAllCallBack err:===========>' + JSON.stringify(err));
        console.info('===========ANS_Cancel_0700 cancelAllCallBack data:==========>' + JSON.stringify(data));
        console.info('===========ANS_Cancel_0700 cancelAllCallBack end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_0700
     * @tc.name: cancelAll(callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that by calling the cancelAll(callback: AsyncCallback<void>): void;
                 interface, the application successfully cancels all the notifications
                 that isUnremovable is true that it has published
     */
    it('ANS_Cancel_0700', 0, async function (done) {
        console.info('===============ANS_Cancel_0700 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelAllIsUnremovable,
            onCancel:onCancelCancelAllIsUnremovable,
        }
        await notify.subscribe(subscriber);
        console.info('========ANS_Cancel_0700 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0700',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('===========ANS_Cancel_0700 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0700 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelAllIsUnremovablPromise(data) {
        console.info('================ANS_Cancel_0800 onConsume start=============>');
        console.info('================ANS_Cancel_0800 onConsume data:=============>' + JSON.stringify(data));
        notify.cancelAll();
        console.info('================ANS_Cancel_0800 cancelAll==========>');
        console.info('================ANS_Cancel_0800 onConsume end===============>');
    }
    function onCancelCancelAllIsUnremovablePromise(data) {
        console.info('================ANS_Cancel_0800 onCancel start================>');
        console.info('================ANS_Cancel_0800 onCancel data :===============>' + JSON.stringify(data));
        expect(data.request.id).assertEqual(8);
        console.info('================ANS_Cancel_0800 onCancel end================>');
    }

    /*
     * @tc.number: ANS_Cancel_0800
     * @tc.name: cancelAll(): Promise<void>;
     * @tc.desc: Verify that by calling the cancelAll(): Promise<void> interface,
                 the application successfully cancels all the notifications
                 that isUnremovable is true that it has published
     */
    it('ANS_Cancel_0800', 0, async function (done) {
        console.info('===============ANS_Cancel_0800 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelAllIsUnremovablPromise,
            onCancel:onCancelCancelAllIsUnremovablePromise,
        }
        await notify.subscribe(subscriber);
        console.info('==================ANS_Cancel_0800 subscribe promsie==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '0800',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('=========ANS_Cancel_0800 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0800 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancel(data) {
        console.info('=========ANS_Cancel_0900 onConsume start================>');
        console.info('=========ANS_Cancel_0900 onConsume data:================>' + JSON.stringify(data));
        notify.cancel(data.request.id,cancelCallBackCancel);
        console.info('=========ANS_Cancel_0900 onConsume cancel=============>');
        console.info('=========ANS_Cancel_0900 onConsume end================>');
    }
    function onCancelCancel(data) {
        console.info('=========ANS_Cancel_0900 onCancel start================>');
        console.info('=========ANS_Cancel_0900 onCancel data:================>' + JSON.stringify(data));
        expect(data.request.id).assertEqual(9);
        console.info('=========ANS_Cancel_0900 onCancel end================>');
    }
    function cancelCallBackCancel(err){
        console.info('===========ANS_Cancel_0900 cancelCallBack start================>');
        console.info('===========ANS_Cancel_0900 cancelCallBack err:=================>' + JSON.stringify(err));
        console.info('===========ANS_Cancel_0900 cancelCallBack end==================>');
    }

    /*
     * @tc.number: ANS_Cancel_0900
     * @tc.name: cancel(id: number, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the specific ID notification is cancelled successfully by calling
                 the cancel(id: number, callback: AsyncCallback<void>): void; interface
     */
    it('ANS_Cancel_0900', 0, async function (done) {
        console.info('===============ANS_Cancel_0900 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancel,
            onCancel:onCancelCancel,
        }
        await notify.subscribe(subscriber);
        console.info('==================ANS_Cancel_0900 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==================ANS_Cancel_0900 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_0900 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelWrongId(data) {
        console.info('========ANS_Cancel_1000 onConsume start==============>');
        console.info('========ANS_Cancel_1000 onConsume data:==============>' + JSON.stringify(data));
        console.info('========ANS_Cancel_1000 onConsume label:=============>' + data.request.label);
        console.info('========ANS_Cancel_1000 onConsume id:================>' + data.request.id);
        notify.cancel(9999,cancelCallBackCancelWrongId);
        console.info('========ANS_Cancel_1000 onConsume cancel==========>');
        console.info('========ANS_Cancel_1000 onConsume end============>');
    }
    function onCancelCancelWrongId(data) {
        console.info('================ANS_Cancel_1000 onCancel start=============>');
        console.info('================ANS_Cancel_1000 onCancel data:=============>' + JSON.stringify(data));
        expect().assertFail();
        console.info('================ANS_Cancel_1000 onCancel end===============>');
    }
    function cancelCallBackCancelWrongId(err){
        console.info('==============ANS_Cancel_1000 cancelCallBack start===============>');
        console.info('==============ANS_Cancel_1000 cancelCallBack err:================>' + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info('==============ANS_Cancel_1000 cancelCallBack end=================>');
    }

    /*
     * @tc.number: ANS_Cancel_1000
     * @tc.name: cancel(id: number, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that when the cancel(id: number, callback: AsyncCallback<void>): void
                 interface is called, when the id is wrong, no notification information is cancelled at this time
     */
    it('ANS_Cancel_1000', 0, async function (done) {
        console.info('===============ANS_Cancel_1000 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelWrongId,
            onCancel:onCancelCancelWrongId,
        }
        await notify.subscribe(subscriber);
        console.info('================ANS_Cancel_1000 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1000',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('===========ANS_Cancel_1000 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1000 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelIsUnremovable(data) {
        console.info('============ANS_Cancel_1100 onConsume start===============>');
        console.info('============ANS_Cancel_1100 onConsume data:===============>' + JSON.stringify(data));
        console.info('============ANS_Cancel_1100 onConsume label:==============>' + data.request.label);
        console.info('============ANS_Cancel_1100 onConsume id: =======================>' + data.request.id);
        notify.cancel(data.request.id,cancelCallBackCancelIsUnremovable);
        console.info('============ANS_Cancel_1100 onConsume cancel=======================>');
        console.info('============ANS_Cancel_1100 onConsume end=======================>');
    }
    function onCancelCancelIsUnremovable(data) {
        console.info('============ANS_Cancel_1100 onCancel start================>');
        console.info('============ANS_Cancel_1100 onCancel data:================>' + JSON.stringify(data));
        console.info('============ANS_Cancel_1100 onCancel id:================>' + data.request.id);
        expect(data.request.id).assertEqual(11);
        console.info('============ANS_Cancel_1100 onCancel end=======================>');
    }
    function cancelCallBackCancelIsUnremovable(err){
        console.info('============ANS_Cancel_1100 cancelCallBack start=================>');
        console.info('============ANS_Cancel_1100 cancelCallBack err:==================>' + JSON.stringify(err));
        console.info('============ANS_Cancel_1100 cancelCallBack end===================>');
    }

    /*
     * @tc.number: ANS_Cancel_1100
     * @tc.name: cancel(id: number, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify the success of canceling the notification with the notification attribute isUnremovable
                 being true by calling the cancel(id: number, callback: AsyncCallback<void>): void interface
     */
    it('ANS_Cancel_1100', 0, async function (done) {
        console.info('===============ANS_Cancel_1100 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelIsUnremovable,
            onCancel:onCancelCancelIsUnremovable,
        }
        await notify.subscribe(subscriber);
        console.info('===============ANS_Cancel_1100 subscribe promise=============>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('============ANS_Cancel_1100 publish promise===========>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1100 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    let id1200
    function onConsumeCancel2Times(data) {
        console.info('=========ANS_Cancel_1200 onConsume start=====================>');
        console.info('=========ANS_Cancel_1200 onConsume data:=====================>' + JSON.stringify(data));
        console.info('=========ANS_Cancel_1200 onConsume label:====================>' + data.request.label);
        console.info('=========ANS_Cancel_1200 onConsume id:=======================>' + data.request.id);
        id1200 = data.request.id
        notify.cancel(id1200, cancelCallBackCancel2Times1);
        console.info('=========ANS_Cancel_1200 onConsume cancel====================>');
        console.info('=========ANS_Cancel_1200 onConsume end=======================>');
    }
    function onCancelCancel2Times(data) {
        timesOfOnCancel = timesOfOnCancel + 1
        console.info('=========ANS_Cancel_1200 onCancel start======================>');
        console.info('=========ANS_Cancel_1200 onCancel data:===========>' + JSON.stringify(data));
        console.info('=========ANS_Cancel_1200 onCancel timesOfOnCancel========>'+timesOfOnCancel);
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(12);
            console.info('=========ANS_Cancel_1200 onCancel id:======================>' + data.request.id);
            notify.cancel(id1200, cancelCallBackCancel2Times2);
            console.info('=========ANS_Cancel_1200 onCancel cancelCallBack_cancel 2Times2===========>');
        }else if(timesOfOnCancel == 2) {
            expect().assertFail();
        }
        console.info('=========ANS_Cancel_1200 onCancel end=======================>');
    }
    function cancelCallBackCancel2Times1(err){
        console.info('===========ANS_Cancel_1200 cancelCallBack1 start================>');
        console.info('===========ANS_Cancel_1200 cancelCallBack1 err:=================>' + JSON.stringify(err));
        console.info('===========ANS_Cancel_1200 cancelCallBack1 end=======================>');
    }

    function cancelCallBackCancel2Times2(err){
        console.info('===========ANS_Cancel_1200 cancelCallBack2 start===========>');
        console.info('===========ANS_Cancel_1200 cancelCallBack2 err:============>' + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info('===========ANS_Cancel_1200 cancelCallBack2 end============>');
    }

    /*
     * @tc.number: ANS_Cancel_1200
     * @tc.name: cancel(id: number, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, callback: AsyncCallback<void>): void
                 interface is called twice in a row to cancel the notification
     */
    it('ANS_Cancel_1200', 0, async function (done) {
        console.info('=============ANS_Cancel_1200 start==========================>');
        timesOfOnCancel = 0
        let subscriber ={
            onConsume:onConsumeCancel2Times,
            onCancel:onCancelCancel2Times,
        }
        await notify.subscribe(subscriber);
        console.info('=============ANS_Cancel_1200 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 12,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('========ANS_Cancel_1200 publish promise=================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1200 setTimeout unsubscribe end==================>');
            done();
        }),1500);
    })

    function onConsumeCancelLabel(data) {
        console.info('============ANS_Cancel_1300 onConsume start=============>');
        console.info('============ANS_Cancel_1300 onConsume data:=============>' + JSON.stringify(data));
        console.info('============ANS_Cancel_1300 onConsume label: ===========>' + data.request.label);
        console.info('============ANS_Cancel_1300 onConsume id: ==============>' + data.request.id);
        notify.cancel(data.request.id, data.request.label, cancelCallBackCancelLabel);
        console.info('============ANS_Cancel_1300 onConsume cance=============>');
        console.info('============ANS_Cancel_1300 onConsume end===============>');
    }
    function onCancelCancelLabel(data) {
        console.info('=========ANS_Cancel_1300 onCancel start============>');
        console.info('=========ANS_Cancel_1300 onCancel data:============>' + JSON.stringify(data));
        expect(data.request.id).assertEqual(13);
        console.info('=========ANS_Cancel_1300 onCancel id:============>' + data.request.id);
        expect(data.request.label).assertEqual('1300');
        console.info('=========ANS_Cancel_1300 onCancel label:============>' + data.request.label);
        console.info('=========ANS_Cancel_1300 onCancel end==============>');
    }
    function cancelCallBackCancelLabel(err){
        console.info('=========ANS_Cancel_1300 cancelCallBack start====================>');
        console.info('=========ANS_Cancel_1300 cancelCallBack err:=====================>' + JSON.stringify(err));
        console.info('=========ANS_Cancel_1300 cancelCallBack end=====================>');
    }

    /*
     * @tc.number: ANS_Cancel_1300
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel notification is successful by calling the
                 cancel(id: number, label: string, callback: AsyncCallback<void>): void; interface
     */
    let subscriber
    it('ANS_Cancel_1300', 0, async function (done) {
        console.info('===============ANS_Cancel_1300 start==========================>');
        subscriber ={
            onConsume:onConsumeCancelLabel,
            onCancel:onCancelCancelLabel,
        }
        await notify.subscribe(subscriber);
        console.info('================ANS_Cancel_1300 subscribe promise==============>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1300',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('===========ANS_Cancel_1300 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1300 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelLabelPromise(data) {
        console.info('==========ANS_Cancel_1400 onConsume start=================>');
        console.info('==========ANS_Cancel_1400 onConsume data:=================>' + JSON.stringify(data));
        console.info('==========ANS_Cancel_1400 onConsume label:================>' + data.request.label);
        console.info('==========ANS_Cancel_1400 onConsume id:===================>' + data.request.id);
        notify.cancel(data.request.id, data.request.label);
        console.info('==========ANS_Cancel_1400 onConsume cancel==========>');
        console.info('==========ANS_Cancel_1400 onConsume end=============>');
    }
    function onCancelCancelLabelPromise(data) {
        console.info('==========ANS_Cancel_1400 onCancel start============>');
        console.info('==========ANS_Cancel_1400 onCancel data:============>' + JSON.stringify(data));
        expect(data.request.id).assertEqual(14);
        console.info('==========ANS_Cancel_1400 onCancel id:================>' + data.request.id);
        expect(data.request.label).assertEqual('1400');
        console.info('==========ANS_Cancel_1400 onCancel label:================>' + data.request.label);
        console.info('==========ANS_Cancel_1400 onCancel end==============>');
    }

    /*
     * @tc.number: ANS_Cancel_1400
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel notification is successful by calling the
                 cancel(id: number, label?: string): Promise<void> interface
     */
    it('ANS_Cancel_1400', 0, async function (done) {
        console.info('===============ANS_Cancel_1400 start==========================>');
        subscriber ={
            onConsume:onConsumeCancelLabelPromise,
            onCancel:onCancelCancelLabelPromise,
        }
        await notify.subscribe(subscriber);
        console.info('==========ANS_Cancel_1400 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1400',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('===============ANS_Cancel_1400 publish promise==============>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1400 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelLabelIsUnremoveable(data) {
        console.info('========ANS_Cancel_1500 onConsume start=====================>');
        console.info('========ANS_Cancel_1500 onConsume data:=====================>' + JSON.stringify(data));
        console.info('========ANS_Cancel_1500 onConsume label:====================>' + data.request.label);
        console.info('========ANS_Cancel_1500 onConsume id:=======================>' + data.request.id);
        notify.cancel(data.request.id, data.request.label, cancelCallBackCancelLabelIsUnremoveable);
        console.info('========ANS_Cancel_1500 onConsume cancel====================>');
        console.info('========ANS_Cancel_1500 onConsume end=======================>');
    }
    function onCancelCancelLabelIsUnremoveable(data) {
        console.info('========ANS_Cancel_1500 onCancel start=======================>');
        console.info('========ANS_Cancel_1500 onCancel data : =======================>' + JSON.stringify(data));
        expect(data.request.id).assertEqual(15);
        console.info('========ANS_Cancel_1500 onCancel id : =======================>' + data.request.id);
        expect(data.request.label).assertEqual('1500');
        console.info('========ANS_Cancel_1500 onCancel label : =======================>' + data.request.label);
        console.info('========ANS_Cancel_1500 onCancel end=======================>');
    }
    function cancelCallBackCancelLabelIsUnremoveable(err){
        console.info('========ANS_Cancel_1500 cancelCallBack start=========>');
        console.info('========ANS_Cancel_1500 cancelCallBack err:==========>' + JSON.stringify(err));
        console.info('========ANS_Cancel_1500 cancelCallBack end===========>');
    }

    /*
     * @tc.number: ANS_Cancel_1500
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void
     * @tc.desc: Verify that the notification whose notification property isUnremovable is true is canceled
     successfully by calling the cancel(id: number, label: string, callback: AsyncCallback<void>): void interface
     */
    it('ANS_Cancel_1500', 0, async function (done) {
        console.info('===============ANS_Cancel_1500 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelLabelIsUnremoveable,
            onCancel:onCancelCancelLabelIsUnremoveable,
        }
        await notify.subscribe(subscriber);
        console.info('=======ANS_Cancel_1500 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 15,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1500',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('===========ANS_Cancel_1500 publish promise=============>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1500 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelLabelIsUnremoveablePromise(data) {
        console.info('============ANS_Cancel_1600 onConsume start==================>');
        console.info('============ANS_Cancel_1600 onConsume data:==================>' + JSON.stringify(data));
        console.info('============ANS_Cancel_1600 onConsume label:=================>' + data.request.label);
        console.info('============ANS_Cancel_1600 onConsume id:====================>' + data.request.id);
        notify.cancel(data.request.id, data.request.label)
        console.info('============ANS_Cancel_1600 onConsume cancel end==========>');
        console.info('============ANS_Cancel_1600 onConsume end====================>');
    }
    function onCancelCancelLabelIsUnremoveablePromise(data) {
        console.info('============ANS_Cancel_1600 onCancel start=================>');
        console.info('============ANS_Cancel_1600 onCancel data:=================>' + JSON.stringify(data));
        expect(data.request.id).assertEqual(16);
        console.info('============ANS_Cancel_1600 onCancel id : =======================>' + data.request.id);
        expect(data.request.label).assertEqual('1600');
        console.info('============ANS_Cancel_1600 onCancel label : =======================>' + data.request.label);
        console.info('============ANS_Cancel_1600 onCancel end===================>');
    }

    /*
     * @tc.number: ANS_Cancel_1600
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the notification whose notification property isUnremovable is true
                 is canceled successfully by calling the cancel(id: number, label?: string): Promise<void> interface
     */
    it('ANS_Cancel_1600', 0, async function (done) {
        console.info('===============ANS_Cancel_1600 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelLabelIsUnremoveablePromise,
            onCancel:onCancelCancelLabelIsUnremoveablePromise,
        }
        await notify.subscribe(subscriber);
        console.info('==============ANS_Cancel_1600 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 16,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1600',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==============ANS_Cancel_1600 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1600 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })
    function onConsumeCancelWrongLabel(data) {
        console.info('================ANS_Cancel_1700 onConsume start===============>');
        console.info('================ANS_Cancel_1700 onConsume data:===============>' + JSON.stringify(data));
        console.info('================ANS_Cancel_1700 onConsume label:==============>' + data.request.label);
        console.info('================ANS_Cancel_1700 onConsume id:=================>' + data.request.id);
        notify.cancel(data.request.id, '9999', cancelCallBackCancelWrongLabel);
        console.info('================ANS_Cancel_1700 onConsume cancel=======================>');
        console.info('================ANS_Cancel_1700 onConsume end=======================>');
    }
    function onCancelCancelWrongLabel(data) {
        console.info('================ANS_Cancel_1700 onCancel start===============>');
        console.info('================ANS_Cancel_1700 onCancel data:===============>' + JSON.stringify(data));
        expect().assertFail(0);
        console.info('================ANS_Cancel_1700 onCancel end=================>');
    }
    function cancelCallBackCancelWrongLabel(err){
        console.info('================ANS_Cancel_1700 cancelCallBack start=================>');
        console.info('================ANS_Cancel_1700 cancelCallBack err:==================>' + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info('================ANS_Cancel_1700 cancelCallBack end===================>');
    }

    /*
     * @tc.number: ANS_Cancel_1700
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called, the label is wrong and the ID is correct.
     */
    it('ANS_Cancel_1700', 0, async function (done) {
        console.info('===============ANS_Cancel_1700 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelWrongLabel,
            onCancel:onCancelCancelWrongLabel,
        }
        await notify.subscribe(subscriber);
        console.info('==============ANS_Cancel_1700 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 17,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1700',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==============ANS_Cancel_1700 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1700 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelWrongLabelPromise(data) {
        console.info('=========ANS_Cancel_1800 onConsume start================>');
        console.info('=========ANS_Cancel_1800 onConsume data:================>' + JSON.stringify(data));
        console.info('=========ANS_Cancel_1800 onConsume label:===============>' + data.request.label);
        console.info('=========ANS_Cancel_1800 onConsume id:==================>' + data.request.id);
        notify.cancel(data.request.id, '9999').then(()=>{
            console.info('=========ANS_Cancel_1800 onConsume cancel then======>');
        }).catch((err)=>{
            console.info('=========ANS_Cancel_1800 onConsume cancel catch err======>'+JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
        });
        console.info('=========ANS_Cancel_1800 onConsume end=======================>');
    }
    function onCancelCancelWrongLabelPromise(data) {
        console.info('=========ANS_Cancel_1800 onCancel start=======================>');
        console.info('=========ANS_Cancel_1800 onCancel data : =======================>' + JSON.stringify(data));
        expect().assertFail(0);
        console.info('=========ANS_Cancel_1800 onCancel end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_1800
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void> interface is called,
                 the label is wrong and the ID is correct.
     */
    it('ANS_Cancel_1800', 0, async function (done) {
        console.info('===============ANS_Cancel_1800 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelWrongLabelPromise,
            onCancel:onCancelCancelWrongLabelPromise,
        }
        await notify.subscribe(subscriber);
        console.info('==============ANS_Cancel_1800 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 18,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1800',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==============ANS_Cancel_1800 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1800 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelLabelNullCharacter(data) {
        console.info('===========ANS_Cancel_1900 onConsume start==================>');
        console.info('===========ANS_Cancel_1900 onConsume data:==================>' + JSON.stringify(data));
        console.info('===========ANS_Cancel_1900 onConsume label:=================>' + data.request.label);
        console.info('===========ANS_Cancel_1900 onConsume id:====================>' + data.request.id);
        notify.cancel(data.request.id, '', cancelCallBackCancelNullCharacter);
        console.info('===========ANS_Cancel_1900 onConsume cancel=======================>');
        console.info('===========ANS_Cancel_1900 onConsume end=======================>');
    }
    function onCancelCancelLabelNullCharacter(data) {
        console.info('===========ANS_Cancel_1900 onCancel start================>');
        console.info('===========ANS_Cancel_1900 onCancel data:================>' + JSON.stringify(data));
        expect().assertFail(0);
        console.info('===========ANS_Cancel_1900 onCancel end=======================>');
    }
    function cancelCallBackCancelNullCharacter(err){
        console.info('===========ANS_Cancel_1900 cancelCallBack start=================>');
        console.info('===========ANS_Cancel_1900 cancelCallBack err:==================>' + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info('===========ANS_Cancel_1900 cancelCallBack end===================>');
    }

    /*
     * @tc.number: ANS_Cancel_1900
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called, and the label uses empty characters
     */
    it('ANS_Cancel_1900', 0, async function (done) {
        console.info('===============ANS_Cancel_1900 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelLabelNullCharacter,
            onCancel:onCancelCancelLabelNullCharacter,
        }
        await notify.subscribe(subscriber);
        console.info('=============ANS_Cancel_1900 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 19,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '1900',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==========ANS_Cancel_1900 publish promise==============>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_1900 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelNullCharacter(data) {
        console.info('==========ANS_Cancel_2000 onConsume start=================>');
        console.info('==========ANS_Cancel_2000 onConsume data:=================>' + JSON.stringify(data));
        console.info('==========ANS_Cancel_2000 onConsume label:================>' + data.request.label);
        console.info('==========ANS_Cancel_2000 onConsume id:===================>' + data.request.id);
        notify.cancel(data.request.id, '').then(()=>{
            console.info('=========ANS_Cancel_2000 onConsume cancel then======>');
        }).catch((err)=>{
            console.info('=========ANS_Cancel_2000 onConsume cancel catch err======>'+JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
        });
        console.info('==========ANS_Cancel_2000 onConsume cancel end==========>');
        console.info('==========ANS_Cancel_2000 onConsume end=================>');
    }
    function onCancelCancelNullCharacter(data) {
        console.info('==========ANS_Cancel_2000 onCancel start====================>');
        console.info('==========ANS_Cancel_2000 onCancel data:======================>' + JSON.stringify(data));
        expect().assertFail(0);
        console.info('==========ANS_Cancel_2000 onCancel end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_2000
     * @tc.name: cancel(id: number, label?: string): Promise<void>;
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void> interface is called,
                 and the label uses empty characters
     */
    it('ANS_Cancel_2000', 0, async function (done) {
        console.info('===============ANS_Cancel_2000 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelNullCharacter,
            onCancel:onCancelCancelNullCharacter,
        }
        await notify.subscribe(subscriber);
        console.info('=========ANS_Cancel_2000 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 20,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '2000',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('============ANS_Cancel_2000 publish promise===============>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_2000 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    let id2100
    let label2100
    function onConsumeCancelLabel2Times(data) {
        console.info('=========ANS_Cancel_2100 onConsume start==================>');
        console.info('=========ANS_Cancel_2100 onConsume data:==================>' + JSON.stringify(data));
        console.info('=========ANS_Cancel_2100 onConsume label:=================>' + data.request.label);
        console.info('=========ANS_Cancel_2100 onConsume id:====================>' + data.request.id);
        id2100 = data.request.id
        label2100 = data.request.label
        notify.cancel(id2100, label2100, cancelCallBackCancelLabel2Times1);
        console.info('=========ANS_Cancel_2100 onConsume cancel=======================>');
        console.info('=========ANS_Cancel_2100 onConsume end=======================>');
    }
    function onCancelCancelLabel2Times(data) {
        timesOfOnCancel = timesOfOnCancel + 1
        console.info('=========ANS_Cancel_2100 onCancel start==========>');
        console.info('=========ANS_Cancel_2100 onCancel data===========>' + JSON.stringify(data));
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(21);
            expect(data.request.label).assertEqual('2100');
        }else if(timesOfOnCancel == 2){
            expect().assertFail();
        }
        console.info('=========ANS_Cancel_2100 onCancel end==========>');
    }
    function cancelCallBackCancelLabel2Times1(err){
        console.info('=========ANS_Cancel_2100 cancelCallBack 2Times1 start============>');
        console.info('=========ANS_Cancel_2100 cancelCallBack 2Times1 err==============>' + JSON.stringify(err));
        expect(err.code).assertEqual(0);
        notify.cancel(id2100, label2100, cancelCallBackCancelLabel2Times2);
        console.info('=========ANS_Cancel_2100 cancelCallBack 2Times1 cancel =======================>');
        console.info('=========ANS_Cancel_2100 cancelCallBack 2Times1 end=======================>');
    }
    function cancelCallBackCancelLabel2Times2(err){
        console.info('=========ANS_Cancel_2100 cancelCallBack 2Times2 start================>');
        console.info('=========ANS_Cancel_2100 cancelCallBack 2Times2 err:=================>' + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info('=========ANS_Cancel_2100 cancelCallBack 2Times2 end==================>');
    }

    /*
     * @tc.number: ANS_Cancel_2100
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called twice in a row
     */
    it('ANS_Cancel_2100', 0, async function (done) {
        console.info('===============ANS_Cancel_2100 start==========================>');
        timesOfOnCancel = 0
        let subscriber ={
            onConsume:onConsumeCancelLabel2Times,
            onCancel:onCancelCancelLabel2Times,
        }
        await notify.subscribe(subscriber);
        console.info('=============ANS_Cancel_2100 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 21,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '2100',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('=============ANS_Cancel_2100 publish promise================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_2100 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    let id2200
    let label2200
    function onConsumeCancelLabelPromise2Times(data) {
        console.info('===========ANS_Cancel_2200 onConsume start=======================>');
        console.info('===========ANS_Cancel_2200 onConsume data:=========>' + JSON.stringify(data));
        console.info('===========ANS_Cancel_2200 onConsume label:========>' + data.request.label);
        console.info('===========ANS_Cancel_2200 onConsume id:===========>' + data.request.id);
        id2200 = data.request.id
        label2200 = data.request.label
        notify.cancel(id2200, label2200);
        console.info('===========ANS_Cancel_2200 onConsume cancel1==========>');
        notify.cancel(id2200, label2200).then(()=>{
            console.info('=========ANS_Cancel_2200 onConsume cancel2 then======>');
        }).catch((err)=>{
            console.info('=========ANS_Cancel_2200 onConsume cancel2 catch err======>'+JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
        });
        console.info('===========ANS_Cancel_2200 onConsume cancel2==========>');
        console.info('===========ANS_Cancel_2200 onConsume end==============>');
    }
    function onCancelCancelLabelPromise2Times(data) {
        console.info('===========ANS_Cancel_2200 onCancel start===================>');
        console.info('===========ANS_Cancel_2200 onCancel data:===================>' + JSON.stringify(data));
        timesOfOnCancel = timesOfOnCancel + 1
        if (timesOfOnCancel == 1){
            expect(data.request.id).assertEqual(22);
            expect(data.request.label).assertEqual('2200');
        }else if (timesOfOnCancel == 2){
            expect().assertFail();
        }
        console.info('===========ANS_Cancel_2200 onCancel end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_2200
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void>
                 interface is called twice in a row
     */
    it('ANS_Cancel_2200', 0, async function (done) {
        console.info('===============ANS_Cancel_2200 start==========================>');
        timesOfOnCancel = 0
        let subscriber ={
            onConsume:onConsumeCancelLabelPromise2Times,
            onCancel:onCancelCancelLabelPromise2Times,
        }
        await notify.subscribe(subscriber);
        console.info('================ANS_Cancel_2200 subscribe_2200_promise=============>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 22,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '2200',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('================ANS_Cancel_2200 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_2200 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelRightLabelWrongId(data) {
        console.info('============ANS_Cancel_2300 onConsume start=======================>');
        console.info('============ANS_Cancel_2300 onConsume data: =======================>' + JSON.stringify(data));
        console.info('============ANS_Cancel_2300 onConsume label: =======================>' + data.request.label);
        console.info('============ANS_Cancel_2300 onConsume id: =======================>' + data.request.id);
        notify.cancel(11111, data.request.label, cancelCallBackCancelRightLabelWrongId);
        console.info('============ANS_Cancel_2300 onConsume cancel=======================>');
        console.info('============ANS_Cancel_2300 onConsume end=======================>');
    }
    function onCancelCancelRightLabelWrongId(data) {
        console.info('============ANS_Cancel_2300 onCancel start=======================>');
        console.info('============ANS_Cancel_2300 onCancel data:=======================>' + JSON.stringify(data));
        expect().assertFail();
        console.info('============ANS_Cancel_2300 onCancel end=======================>');
    }
    function cancelCallBackCancelRightLabelWrongId(err){
        console.info('============ANS_Cancel_2300 cancelCallBack start=======================>');
        console.info('============ANS_Cancel_2300 cancelCallBack err:===============>' + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info('============ANS_Cancel_2300 cancelCallBack end===================>');
    }

    /*
     * @tc.number: ANS_Cancel_2300
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>):
     void interface is called, the label is correct and the ID is wrong.
     */
    it('ANS_Cancel_2300', 0, async function (done) {
        console.info('===============ANS_Cancel_2300 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelRightLabelWrongId,
            onCancel:onCancelCancelRightLabelWrongId,
        }
        await notify.subscribe(subscriber);
        console.info('================ANS_Cancel_2300 promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 23,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '2300',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('============ANS_Cancel_2300 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_2300 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelRightLabelWrongIdPromise(data) {
        console.info('============ANS_Cancel_2400 onConsume start==============>');
        console.info('============ANS_Cancel_2400 onConsume data:==============>' + JSON.stringify(data));
        console.info('============ANS_Cancel_2400 onConsume label:=============>' + data.request.label);
        console.info('============ANS_Cancel_2400 onConsume id:================>' + data.request.id);
        notify.cancel(11111, data.request.label).then(()=>{
            console.info('=========ANS_Cancel_2400 onConsume cancel then======>');
        }).catch((err)=>{
            console.info('=========ANS_Cancel_2400 onConsume cancel catch err======>'+JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
        });
        console.info('============ANS_Cancel_2400 onConsume cancel==========>');
        console.info('============ANS_Cancel_2400 onConsume end=============>');
    }
    function onCancelCancelRightLabelWrongIdPromise(data) {
        console.info('============ANS_Cancel_2400 onCancel start:==============>');
        console.info('============ANS_Cancel_2400 onCancel data:===============>' + JSON.stringify(data));
        expect().assertFail();
        console.info('============ANS_Cancel_2400 onCancel end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_2400
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                 interface is called, the label is correct and the ID is correct wrong.
     */
    it('ANS_Cancel_2400', 0, async function (done) {
        console.info('===============ANS_Cancel_2400 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelRightLabelWrongIdPromise,
            onCancel:onCancelCancelRightLabelWrongIdPromise,
        }
        await notify.subscribe(subscriber);
        console.info('============ANS_Cancel_2400 subscribe promise========>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 24,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '2400',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('============ANS_Cancel_2400 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_2400 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelWrongLabelWrongId(data) {
        console.info('==========ANS_Cancel_2500 onConsume start=======================>');
        console.info('==========ANS_Cancel_2500 onConsume data:=======================>' + JSON.stringify(data));
        console.info('==========ANS_Cancel_2500 onConsume label:======================>' + data.request.label);
        console.info('==========ANS_Cancel_2500 onConsume id==========================>' + data.request.id);
        notify.cancel(6666, '6666', cancelCallBackCancelWrongLabelWrongId);
        console.info('==========ANS_Cancel_2500 onConsume cancel====================>');
        console.info('==========ANS_Cancel_2500 onConsume end=======================>');
    }
    function onCancelCancelWrongLabelWrongId(data) {
        console.info('==========ANS_Cancel_2500 onCancel start====================>');
        console.info('==========ANS_Cancel_2500 onCancel data:====================>' + JSON.stringify(data));
        expect().assertFail();
        console.info('==========ANS_Cancel_2500 onCancel end======================>');
    }
    function cancelCallBackCancelWrongLabelWrongId(err){
        console.info('==========ANS_Cancel_2500 cancelCallBack start=======================>');
        console.info('==========ANS_Cancel_2500 cancelCallBack err:================>' + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info('==========ANS_Cancel_2500 cancelCallBack end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_2500
     * @tc.name: cancel(id: number, label: string, callback: AsyncCallback<void>): void
     * @tc.desc: Verify that the cancel(id: number, label: string, callback: AsyncCallback<void>): void
                　interface is called, the label is correct and the ID is correct wrong.
     */
    it('ANS_Cancel_2500', 0, async function (done) {
        console.info('===============ANS_Cancel_2500 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelWrongLabelWrongId,
            onCancel:onCancelCancelWrongLabelWrongId,
        }
        await notify.subscribe(subscriber);
        console.info('================ANS_Cancel_2500 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 25,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '2500',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('==========ANS_Cancel_2500 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_2500 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })

    function onConsumeCancelWrongLabelWrongIdPromise(data) {
        console.info('========ANS_Cancel_2600 onConsume start:=============>');
        console.info('========ANS_Cancel_2600 onConsume data:==============>' + JSON.stringify(data));
        console.info('========ANS_Cancel_2600 onConsume label:=============>' + data.request.label);
        console.info('========ANS_Cancel_2600 onConsume id:================>' + data.request.id);
        notify.cancel(6666, '6666').then(()=>{
            console.info('=========ANS_Cancel_2600 onConsume cancel then======>');
        }).catch((err)=>{
            console.info('=========ANS_Cancel_2600 onConsume cancel catch err======>'+JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
        });
        console.info('========ANS_Cancel_2600 onConsume end================>');
        console.info('========ANS_Cancel_2600 onConsume end================>');
    }
    function onCancelCancelWrongLabelWrongIdPromise(data) {
        console.info('========ANS_Cancel_2600 onCancel start=======================>');
        console.info('========ANS_Cancel_2600 onCancel data:=======================>' + JSON.stringify(data));
        expect().assertFail();
        console.info('========ANS_Cancel_2600 onCancel end=======================>');
    }

    /*
     * @tc.number: ANS_Cancel_2600
     * @tc.name: cancel(id: number, label?: string): Promise<void>
     * @tc.desc: Verify that the cancel(id: number, label?: string): Promise<void> interface is called,
                　the label is wrong and the ID is wrong.
     */
    it('ANS_Cancel_2600', 0, async function (done) {
        console.info('===============ANS_Cancel_2600 start==========================>');
        let subscriber ={
            onConsume:onConsumeCancelWrongLabelWrongIdPromise,
            onCancel:onCancelCancelWrongLabelWrongIdPromise,
        }
        await notify.subscribe(subscriber);
        console.info('===============ANS_Cancel_2600 subscribe promise==================>');
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: 'test_title',
                    text: 'test_text',
                    additionalText: 'test_additionalText'
                },
            },
            id: 26,
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
            statusBarText: 'statusBarText',
            isFloatingIcon : true,
            label: '2600',
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info('===============ANS_Cancel_2600 publish promise==================>');
        setTimeout((async function(){
            await notify.unsubscribe(subscriber);
            console.info('======ANS_Cancel_2600 setTimeout unsubscribe end==================>');
            done();
        }),timeout);
    })
}) 