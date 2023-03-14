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
let timeout = 300;
describe('ActsAnsNotificationRemove', function () {
    console.info("===========ActsAnsNotificationRemove start====================>");
    let subscriber;
    function publishCallback(err) {
        console.info("==========================publishCallback=======================>");
    }
    let hashCode;
    function onConsumeRemove(data) {
        console.info("=============ANS_Remove_0100 onConsume start=======================>");
        console.info("=============ANS_Remove_0100 onConsume data:==================>" + JSON.stringify(data));
        console.info("=============ANS_Remove_0100 onConsume hascode:===============>" + data.request.hashCode);
        hashCode = data.request.hashCode
        notify.remove(hashCode,removeCallBack);
        console.info("=============ANS_Remove_0100 onConsume remove=======================>");
        console.info("=============ANS_Remove_0100 onConsume end=======================>");
    }

    function onCancelRemove(data) {
        console.info("==========ANS_Remove_0100 onCancel start==================>");
        console.info("==========ANS_Remove_0100 onCancel data : ================>" + JSON.stringify(data));
        console.info("==========ANS_Remove_0100 onCancel hashCode===============>" + hashCode);
        console.info("==========ANS_Remove_0100 onCancel data.request.hashCode==>" + data.request.hashCode);
        expect(hashCode).assertEqual(data.request.hashCode);
        console.info("==========ANS_Remove_0100 onCancel end=======================>");
    }

    function removeCallBack(err, data) {
        console.info("==========ANS_Remove_0100 removeCallBack err=========>" + JSON.stringify(err));
        console.info("==========ANS_Remove_0100 removeCallBack data : =======================>" + JSON.stringify(data));
    }
    function subscriberCallBack(err, data) {
        console.info("================subscriberCallBack err : =======================>" + JSON.stringify(err));
        console.info("================subscriberCallBack data : =======================>" + JSON.stringify(data));
    }
    function publishCallback(err, data) {
        console.info("================publishCallback err : =======================>" + JSON.stringify(err));
        console.info("================publishCallback data : =======================>" + JSON.stringify(data));
    }

    /*
     * @tc.number: ANS_Remove_0100
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the call interface remove(hashCode: string, callback: AsyncCallback<void>): void
                 deletes the notification information through hashcode
     */
    it('ANS_Remove_0100', 0, async function (done) {
        console.info("===============ANS_Remove_0100==========================>");
        hashCode = 0
        let subscriber ={
            onConsume:onConsumeRemove,
            onCancel:onCancelRemove,
        }
        await notify.subscribe(subscriber);
        console.info("==================ANS_Remove_0100 subscribe promise==================>");
        let notificationRequest = {
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
        await notify.publish(notificationRequest);
        console.info("=============ANS_Remove_0100 publish promise===========>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0100 setTimeout==================>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0100 setTimeout unsubscribe==================>");
            await notify.cancelAll();
            done();
        }),timeout);

    })

    function onConsumeRemovePromise(data) {
        console.info("================ANS_Remove_0200 onConsume start===============>");
        console.info("================ANS_Remove_0200 onConsume data================>" + JSON.stringify(data));
        hashCode = data.request.hashCode
        console.info("================ANS_Remove_0200 onConsume hascode:========>" + data.request.hashCode);
        notify.remove(hashCode);
        console.info("================ANS_Remove_0200 onConsume remove============>");
        console.info("================ANS_Remove_0200 onConsume end===============>");
    }

    function onCancelRemovePromise(data) {
        console.info("===========ANS_Remove_0200 onCancel start================>");
        console.info("===========ANS_Remove_0200 onCancel data:=================>" + JSON.stringify(data));
        console.info("===========ANS_Remove_0200 onCancel hashCode====================>" + hashCode);
        console.info("===========ANS_Remove_0200 onCancel data.request.hashCode=======>" + data.request.hashCode);
        expect(hashCode).assertEqual(data.request.hashCode);
        console.info("===========ANS_Remove_0200 onCancel end===================>");
    }

    /*
     * @tc.number: ANS_Remove_0200
     * @tc.name: remove(hashCode: string): Promise<void>;
     * @tc.desc: Verify that the call interface remove(hashCode: string): Promise<void>
                 deletes the notification information through hashcode
     */
    it('ANS_Remove_0200', 0, async function (done) {
        console.info("===============ANS_Remove_0200==========================>");
        hashCode = 0
        let subscriber ={
            onConsume:onConsumeRemovePromise,
            onCancel:onCancelRemovePromise,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_0200 subscribe promise==================>");
        let notificationRequest = {
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
        await notify.publish(notificationRequest);
        console.info("======ANS_Remove_0200 publish==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0200 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0200 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveErrHashCode(data) {
        console.info("================ANS_Remove_0300 onConsume start=======================>");
        console.info("================ANS_Remove_0300 onConsume data:=============>" + JSON.stringify(data));
        notify.remove("errorHashCode",removeErrHashCodeCallBack);
        console.info("================ANS_Remove_0300 onConsume remove=======================>");
        console.info("================ANS_Remove_0300 onConsume end=======================>");
    }
    function onCancelRemoveErrHashCode() {
        console.info("================ANS_Remove_0300 onCancel start=======================>");
        expect().assertFail();
        console.info("================ANS_Remove_0300 onCancel end=======================>");
    }
    function removeErrHashCodeCallBack(err) {
        console.info("==========ANS_Remove_0300 removeCallBack start==============>");
        console.info("==========ANS_Remove_0300 removeCallBack err====================>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("==========ANS_Remove_0300 removeCallBack end=======================>");
    }

    /*
     * @tc.number: ANS_Remove_0300
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void
     * @tc.desc: Verify that the error hashcode is used to call the interface
                 remove(hashCode: string, callback: AsyncCallback<void>) to delete the notification information
     */
    it('ANS_Remove_0300', 0, async function (done) {
        console.info("===============ANS_Remove_0300==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveErrHashCode,
            onCancel:onCancelRemoveErrHashCode,
        }
        await notify.subscribe(subscriber);
        console.info("==================ANS_Remove_0300 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            label: "0300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==================ANS_Remove_0300 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0300 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0300 setTimeout unsubscribe============>");
            await notify.cancelAll()
            done();
        }),timeout);
    })

    function onConsumeRemoveErrHashCodePromise(data) {
        console.info("===========ANS_Remove_0400 onConsume start:===========>");
        console.info("===========ANS_Remove_0400 onConsume data:===========>" + JSON.stringify(data));
        notify.remove("errorHashCode").then((data)=>{
            console.info("===========ANS_Remove_0400 onConsume remove data:===========>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("===========ANS_Remove_0400 onConsume remove err:============>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.info("===========ANS_Remove_0400 onConsume end:===================>");
        });
    }

    function onCancelRemoveErrHashCodePromise() {
        console.info("===========ANS_Remove_0400 onCancel start:===========>");
        expect().assertFail();
        console.info("===========ANS_Remove_0400 onCancel end:===========>");
    }

    /*
     * @tc.number: ANS_Remove_0400
     * @tc.name: remove(hashCode: string): Promise<void>
     * @tc.desc: Verify that the error hashcode is used to call the interface
                 remove(hashCode: string): Promise<void> to delete the notification information
     */
    it('ANS_Remove_0400', 0, async function (done) {
        console.info("===============ANS_Remove_0400==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveErrHashCodePromise,
            onCancel:onCancelRemoveErrHashCodePromise,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_0400 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            label: "0400",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==================ANS_Remove_0400 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0400 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0400 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveUseEmptyCharacter(data) {
        console.info("================ANS_Remove_0500 onConsume start==============>");
        console.info("================ANS_Remove_0500 onConsume data:==============>" + JSON.stringify(data));
        notify.remove('',removeCallBackUseEmptyCharacter);
        console.info("================ANS_Remove_0500 onConsume remove=============>");
        console.info("================ANS_Remove_0500 onConsume end================>");
    }

    function onCancelRemoveUseEmptyCharacter(data) {
        console.info("=============ANS_Remove_0500 onCancel start===============>");
        console.info("=============ANS_Remove_0500 onCancel data:===============>" + JSON.stringify(data));
        expect().assertFail();
        console.info("=============ANS_Remove_0500 onCancel end=================>");
    }

    function removeCallBackUseEmptyCharacter(err) {
        console.info("=============ANS_Remove_0500 removeCallBack start===============>");
        console.info("=============ANS_Remove_0500 removeCallBack err=================>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("=============ANS_Remove_0500 removeCallBack end=================>");
    }

    /*
     * @tc.number: ANS_Remove_0500
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the Empty Character hashcode is used to call the interface
                 remove(hashCode: string, callback: AsyncCallback<void>): void
                 to delete the notification information
     */
    it('ANS_Remove_0500', 0, async function (done) {
        console.info("===============ANS_Remove_0500==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveUseEmptyCharacter,
            onCancel:onCancelRemoveUseEmptyCharacter,
        }
        await notify.subscribe(subscriber);
        console.info("==================ANS_Remove_0500 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            label: "0500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==================ANS_Remove_0500 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0500 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0500 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function OnConsumeRemoveUseEmptyCharacterPromise(data) {
        console.info("===============ANS_Remove_0600 onConsume start==================>");
        console.info("===============ANS_Remove_0600 onConsume data:==================>" + JSON.stringify(data));
        notify.remove('').then((data)=>{
            console.info("===========ANS_Remove_0600 onConsume remove data:============>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("===========ANS_Remove_0600 onConsume remove err:=============>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.info("===========ANS_Remove_0600 onConsume end=====================>");
        });

    }

    function OnCancelRemoveUseEmptyCharacterPromise(data) {
        console.info("==============ANS_Remove_0600 onCancel start=============>");
        console.info("==============ANS_Remove_0600 onCancel data:=============>" + JSON.stringify(data));
        expect().assertFail();
        console.info("==============ANS_Remove_0600 onCancel end===============>");
    }

    /*
     * @tc.number: ANS_Remove_0600
     * @tc.name: remove(hashCode: string): Promise<void>;
     * @tc.desc: Verify that the Empty Character hashcode is used to call the interface
                 remove(hashCode: string): Promise<void> to delete the notification information
     */
    it('ANS_Remove_0600', 0, async function (done) {
        console.info("===============ANS_Remove_0600 start==========================>");
        let subscriber ={
            onConsume:OnConsumeRemoveUseEmptyCharacterPromise,
            onCancel:OnCancelRemoveUseEmptyCharacterPromise,
        }
        await notify.subscribe(subscriber);
        console.info("==================ANS_Remove_0600 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            label: "0600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==================ANS_Remove_0600 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0600 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0600 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveNotExistHashCode(data) {
        console.info("==============ANS_Remove_0700 onConsume start===================>");
        console.info("==============ANS_Remove_0700 onConsume data:===================>" + JSON.stringify(data));
        notify.remove("9999_9999_9",removeNotExistHashCodeCallBack);
        console.info("==============ANS_Remove_0700 onConsume remove===================>");
        console.info("==============ANS_Remove_0700 onConsume end===================>");
    }

    function onCancelRemoveNotExistHashCode(data) {
        console.info("==============ANS_Remove_0700 onCancel start=======================>");
        console.info("==============ANS_Remove_0700 onCancel data:============>" + JSON.stringify(data));
        expect().assertFail();
        console.info("==============ANS_Remove_0700 onCancel end=======================>");
    }

    function removeNotExistHashCodeCallBack(err) {
        console.info("==========ANS_Remove_0700 removeCallBack start==========>");
        console.info("==========ANS_Remove_0700 removeCallBack err=================>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("==========ANS_Remove_0700 removeCallBack end=======================>");
    }

    /*
     * @tc.number: ANS_Remove_0700
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the not exist hashCode is used to call the interface
     *           remove(hashCode: string, callback: AsyncCallback<void>): void
     *           to delete the notification information
     */
    it('ANS_Remove_0700', 0, async function (done) {
        console.info("===============ANS_Remove_0700==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveNotExistHashCode,
            onCancel:onCancelRemoveNotExistHashCode,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_0700 subscribe promise=======>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 7,
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
            label: "0700",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ANS_Remove_0700 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0700 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0700 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveNotExistHashCodePromise(data) {
        console.info("================ANS_Remove_0800 onConsume start===============>");
        console.info("================ANS_Remove_0800 onConsume data:===============>" + JSON.stringify(data));
        notify.remove("9999_9999_9").then((data)=>{
            console.info("===========ANS_Remove_0800 onConsume remove data:=========>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("===========ANS_Remove_0800 onConsume remove err:==========>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.info("===========ANS_Remove_0800 onConsume end==================>");
        });

    }

    function onCancelRemoveNotExistHashCodePromise(data) {
        console.info("============ANS_Remove_0800 onCancel start===============>");
        console.info("============ANS_Remove_0800 onCancel data:===============>" + JSON.stringify(data));
        expect().assertFail();
        console.info("============ANS_Remove_0800 onCancel end=================>");
    }

    /*
     * @tc.number: ANS_Remove_0800
     * @tc.name: remove(hashCode: string): Promise<void>;
     * @tc.desc: Verify that the not exist hashCode is used to call the interface remove(hashCode: string,
     *           callback: AsyncCallback<void>): void to delete the notification information
     */
    it('ANS_Remove_0800', 0, async function (done) {
        console.info("===============ANS_Remove_0800==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveNotExistHashCodePromise,
            onCancel:onCancelRemoveNotExistHashCodePromise,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_0800 subscribe promise=======>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 8,
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
            label: "0800",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==========ANS_Remove_0800 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0800 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0800 setTimeout unsubscribe============>");
            await notify.cancelAll()
            done();
        }),timeout);
    })

    function onConsumeRemoveNonComplianceHashCode(data) {
        console.info("================ANS_Remove_0900 onConsume start===================>");
        console.info("================ANS_Remove_0900 onConsume data: ==================>" + JSON.stringify(data));
        notify.remove("哈希码",removeNonComplianceHashCallBack);
        console.info("================ANS_Remove_0900 onConsume remove==================>");
        console.info("================ANS_Remove_0900 onConsume end=====================>");
    }

    function onCancelRemoveNonComplianceHashCode(data) {
        console.info("================ANS_Remove_0900 onCancel start====================>");
        console.info("================ANS_Remove_0900 onCancel data:====================>" + JSON.stringify(data));
        expect().assertFail();
        console.info("================ANS_Remove_0900 onCancel end======================>");
    }

    function removeNonComplianceHashCallBack(err, data) {
        console.info("================ANS_Remove_0900 removeCallBack start=======================>");
        console.info("================ANS_Remove_0900 removeCallBack err=================>" + JSON.stringify(err));
        console.info("================ANS_Remove_0900 removeCallBack data=================>" + JSON.stringify(data));
        console.info("================ANS_Remove_0900 removeCallBack end=======================>");
        expect(err.code != 0).assertEqual(true);
    }

    /*
     * @tc.number: ANS_Remove_0900
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the non compliance hashCode is used to call the interface remove(hashCode: string,
     *           callback: AsyncCallback<void>): void to delete the notification information
     */
    it('ANS_Remove_0900', 0, async function (done) {
        console.info("===============ANS_Remove_0900==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveNonComplianceHashCode,
            onCancel:onCancelRemoveNonComplianceHashCode,
        }
        await notify.subscribe(subscriber);
        console.info("============ANS_Remove_0900 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "0900",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==================ANS_Remove_0900 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_0900 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_0900 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveNonComplianceHashCodePromise(data) {
        console.info("================ANS_Remove_1000 onConsume start===========>");
        console.info("================ANS_Remove_1000 onConsume data:===========>" + JSON.stringify(data));
        notify.remove("哈希码").then((data)=>{
            console.info("===========ANS_Remove_1000 onConsume remove data:===========>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("===========ANS_Remove_1000 onConsume remove err:===========>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.info("===========ANS_Remove_1000 onConsume end===================>");
        });
    }

    function onCancelRemoveNonComplianceHashCodePromise(data) {
        console.info("================ANS_Remove_1000 onCancel start==================>");
        console.info("================ANS_Remove_1000 onCancel data:==================>" + JSON.stringify(data));
        expect().assertFail();
        console.info("================ANS_Remove_1000 onCancel end====================>");
    }

    /*
     * @tc.number: ANS_Remove_1000
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the non compliance hashCode is used to call the interface
                 remove(hashCode: string, callback: AsyncCallback<void>): void to delete the notification information
     */
    it('ANS_Remove_1000', 0, async function (done) {
        console.info("===============ANS_Remove_1000==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveNonComplianceHashCodePromise,
            onCancel:onCancelRemoveNonComplianceHashCodePromise,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_1000 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "1000",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("================ANS_Remove_1000 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1000 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1000 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function removeCallBack2TimesOf2(err) {
        console.info("=====ANS_Remove_1100 removeCallBack2TimesOf2 start==========>");
        console.info("=====ANS_Remove_1100 removeCallBack2TimesOf2 err============>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("=====ANS_Remove_1100 removeCallBack2TimesOf2 end==========>");
    }
    function removeCallBack2TimesOf1(err) {
        console.info("=====ANS_Remove_1100 removeCallBack2TimesOf1 start============>");
        console.info("=====ANS_Remove_1100 removeCallBack2TimesOf1 err========>" + JSON.stringify(err));
        expect(err.code).assertEqual(0);
        notify.remove(hashCode1100,removeCallBack2TimesOf2);
        console.info("=====ANS_Remove_1100 removeCallBack2TimesOf1 end============>");
    }
    let hashCode1100;
    function onConsumeCallbackRemove2Times(data) {
        console.info("=====ANS_Remove_1100 onConsume start=================>");
        console.info("=====ANS_Remove_1100 onConsume data: ================>" + JSON.stringify(data));
        hashCode1100 = data.request.hashCode;
        notify.remove(data.request.hashCode,removeCallBack2TimesOf1);
        console.info("=====ANS_Remove_1100 onConsume remove================>");
        console.info("=====ANS_Remove_1100 onConsume end===================>");
    }
    let timesOfOnCancelCallbackRemove2Times
    function onCancelCallbackRemove2Times(data) {
        timesOfOnCancelCallbackRemove2Times = timesOfOnCancelCallbackRemove2Times + 1
        console.info("=====ANS_Remove_1100 onCancel start=======================>");
        console.info("=====ANS_Remove_1100 onCancel data:=======================>" + JSON.stringify(data));
        if (timesOfOnCancelCallbackRemove2Times == 1){
            expect(data.request.id).assertEqual(11);
        } else if  (timesOfOnCancelCallbackRemove2Times == 2){
            expect().assertFail();
        }
        console.info("=====ANS_Remove_1100 onCancel end=======================>");
    }

    /*
     * @tc.number: ANS_Remove_1100
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the interface remove(hashCode: string, callback: AsyncCallback<void>): void;
                 is called twice in a row to delete the notification information
     */
    it('ANS_Remove_1100', 0, async function (done) {
        console.info("===============ANS_Remove_1100==========================>");
        hashCode1100 = 0;
        timesOfOnCancelCallbackRemove2Times = 0
        let subscriber ={
            onConsume:onConsumeCallbackRemove2Times,
            onCancel:onCancelCallbackRemove2Times,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_1100 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 11,
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
            label: "1100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==========ANS_Remove_1100 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1100 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1100 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeCallbackRemove2TimesPromise(data) {
        console.info("=======ANS_Remove_1200 onConsume start=============>");
        console.info("=======ANS_Remove_1200 onConsume data:=============>" + JSON.stringify(data));
        notify.remove(data.request.hashCode).then(()=>{
            console.info("=======ANS_Remove_1200 onConsume remove_2times1:=======>");
        }).catch((err)=>{
            console.info("=======ANS_Remove_1200 onConsume remove_2times1 err:========>" + JSON.stringify(err));
            expect(err.code == 0).assertEqual(true);
        });
        notify.remove(data.request.hashCode).then((data)=>{
            console.info("=======ANS_Remove_1200 onConsume remove_2times2 data:=======>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("=======ANS_Remove_1200 onConsume remove_2times2 err:========>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.info("================ANS_Remove_1200 onConsume end=================>");
        });
    }
    let timesOfOnCancelCallbackRemove2TimesPromise
    function onCancelCallbackRemove2TimesPromise(data) {
        timesOfOnCancelCallbackRemove2TimesPromise = timesOfOnCancelCallbackRemove2TimesPromise + 1
        console.info("================ANS_Remove_1200 onCancel start=========>");
        console.info("================ANS_Remove_1200 onCancel data:=========>" + JSON.stringify(data));
        if (timesOfOnCancelCallbackRemove2TimesPromise == 1){
            expect(data.request.id).assertEqual(12);
            console.info("============ANS_Remove_1200 onCancel id:===========>" + JSON.stringify(data.request.id));
        } else if  (timesOfOnCancelCallbackRemove2TimesPromise == 2){
            expect().assertFail();
        }
        console.info("================ANS_Remove_1200 onCancel end==============>");
    }

    /*
     * @tc.number: ANS_Remove_1200
     * @tc.name: remove(hashCode: string): Promise<void>;
     * @tc.desc: Verify that the interface remove(hashCode: string): Promise<void> is called twice in a row to
     *           delete the notification information
     */
    it('ANS_Remove_1200', 0, async function (done) {
        console.info("===============ANS_Remove_1200 start========================>");
        timesOfOnCancelCallbackRemove2TimesPromise = 0
        let subscriber ={
            onConsume:onConsumeCallbackRemove2TimesPromise,
            onCancel:onCancelCallbackRemove2TimesPromise,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_1200 subscribe promise=============>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "1200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ANS_Remove_1200 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1200 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1200 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveIsUnremovable(data) {
        console.info("==========ANS_Remove_1300 onConsume start=================>");
        console.info("==========ANS_Remove_1300 onConsume data:=================>" + JSON.stringify(data));
        notify.remove(data.request.hashCode,removeIsUnremovableCallBack);
        console.info("==========ANS_Remove_1300 onConsume remove==============>");
        console.info("==========ANS_Remove_1300 onConsume end=================>");
    }

    function onCancelRemoveIsUnremovable(data) {
        console.info("==========ANS_Remove_1300 onCancel start================>");
        console.info("==========ANS_Remove_1300 onCancel data:================>" + JSON.stringify(data));
        console.info("==========ANS_Remove_1300 onCancel end================>");
    }

    function removeIsUnremovableCallBack(err) {
        console.info("==========ANS_Remove_1300 removeCallBack start===================>");
        console.info("==========ANS_Remove_1300 removeCallBack err=====================>" + JSON.stringify(err));
        expect(err.code).assertEqual(0);
        console.info("==========ANS_Remove_1300 removeCallBack end=====================>");
    }

    /*
     * @tc.number: ANS_Remove_1300
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the calling interface remove(hashCode: string, callback: AsyncCallback<void>): void;
     *           deletes the notification information that the property isunremovable is true
     */
    it('ANS_Remove_1300', 0, async function (done) {
        console.info("===============ANS_Remove_1300==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveIsUnremovable,
            onCancel:onCancelRemoveIsUnremovable,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_1300 subscribe promise==================>");
        let notificationRequest = {
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
            label: "1300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("================ANS_Remove_1300 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1300 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1300 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function OnConsumeRemoveIsUnremovablePromise(data) {
        console.info("==============ANS_Remove_1400 onConsume start==============>");
        console.info("==============ANS_Remove_1400 onConsume data:==============>" + JSON.stringify(data));
        notify.remove(data.request.hashCode).then((data)=>{
            console.info("=======ANS_Remove_1400 onConsume remove data:=======>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("=======ANS_Remove_1400 onConsume remove err:========>" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.info("=======ANS_Remove_1400 onConsume end================>");
        });
    }

    function OnCancelRemoveIsUnremovablePromise(data) {
        console.info("==============ANS_Remove_1400 onCancel start==============>");
        console.info("==============ANS_Remove_1400 onCancel data:==============>" + JSON.stringify(data));
        console.info("==============ANS_Remove_1400 onCancel end================>");
    }

    /*
     * @tc.number: ANS_Remove_1400
     * @tc.name: remove(hashCode: string): Promise<void>;
     * @tc.desc: Verify that the calling interface remove(hashCode: string): Promise<void>;
                 deletes the notification information that the property isunremovable is true
     */
    it('ANS_Remove_1400', 0, async function (done) {
        console.info("===============ANS_Remove_1400 start==========================>");
        let subscriber ={
            onConsume:OnConsumeRemoveIsUnremovablePromise,
            onCancel:OnCancelRemoveIsUnremovablePromise,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_1400 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 14,
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
            label: "1400",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==============ANS_Remove_1400 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1400 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1400 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    let timesOfOnConsumeRemoveAllByBundleOption
    function onConsumeRemoveAllByBundleOption(data) {
        timesOfOnConsumeRemoveAllByBundleOption = timesOfOnConsumeRemoveAllByBundleOption + 1
        console.info("==========ANS_Remove_1500 onConsume start=======>");
        console.info("==========ANS_Remove_1500 onConsume data:=======>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        if (timesOfOnConsumeRemoveAllByBundleOption == 2){
            notify.removeAll(bundleOption,removeAllByBundleOptionCallBack);
            console.info("=======ANS_Remove_1500 onConsume remove=============>");
        }
        console.info("===========ANS_Remove_1500 onConsume end===========>");
    }
    let timesOfOnCancelRemoveAllByBundleOption
    function onCancelRemoveAllByBundleOption(data) {
        timesOfOnCancelRemoveAllByBundleOption = timesOfOnCancelRemoveAllByBundleOption + 1
        console.info("===========ANS_Remove_1500 onCancel start====================>");
        console.info("===========ANS_Remove_1500 onCancel data:====================>" + JSON.stringify(data));
        if (timesOfOnCancelRemoveAllByBundleOption == 1){
            expect(data.request.label).assertEqual("1500_1");
        }else if (timesOfOnCancelRemoveAllByBundleOption == 2){
            expect(data.request.label).assertEqual("1500_2");
        }
        console.info("===========ANS_Remove_1500 onCancel end=======================>");
    }
    function removeAllByBundleOptionCallBack(data) {
        console.info("===========ANS_Remove_1500 removeAllCallBack start=================>");
        console.info("===========ANS_Remove_1500 removeAllCallBack data==================>" + JSON.stringify(data));
        console.info("===========ANS_Remove_1500 removeAllCallBack end===================>");
    }

    /*
     * @tc.number: ANS_Remove_1500
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the calling interface removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     *           deletes all notification information through BundleOption
     */
    it('ANS_Remove_1500', 0, async function (done) {
        console.info("===============ANS_Remove_1500 start==========================>");
        timesOfOnCancelRemoveAllByBundleOption = 0
        timesOfOnConsumeRemoveAllByBundleOption = 0
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOption,
            onCancel:onCancelRemoveAllByBundleOption,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_1500 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 15,
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
            label: "1500_1",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        let notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 15,
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
            label: "1500_2",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("=============ANS_Remove_1500 publish1 promise==================>");
        await notify.publish(notificationRequest1);
        console.info("=============ANS_Remove_1500 publish2 promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1500 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1500 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    let timesOfOnConsumeRemoveAllByBundleOptionNullUid
    function onConsumeRemoveAllByBundleOptionNullUid(data) {
        console.info("===============ANS_Remove_1600 onConsume start================>");
        console.info("===============ANS_Remove_1600 onConsume data:========>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:0,
        }
        timesOfOnConsumeRemoveAllByBundleOptionNullUid = timesOfOnConsumeRemoveAllByBundleOptionNullUid + 1
        if (timesOfOnConsumeRemoveAllByBundleOptionNullUid == 2){
            notify.removeAll(bundleOption,removeAllByBundleOptionCallBackNullUid);
            console.info("===============ANS_Remove_1600 onConsume remove==================>");
        }
        console.info("===============ANS_Remove_1600 onConsume end================>");
    }
    function onCancelRemoveAllByBundleOptionNullUid(data) {
        console.info("===============ANS_Remove_1600 onCancel start===================>");
        console.info("===============ANS_Remove_1600 onCancel data:===================>" + JSON.stringify(data));
        expect().assertFail();
        console.info("===============ANS_Remove_1600 onCancel end====================>");
    }
    function removeAllByBundleOptionCallBackNullUid(err) {
        console.info("=========ANS_Remove_1600 removeAllCallBack start=============>");
        console.info("=========ANS_Remove_1600 removeAllCallBack err===============>" + JSON.stringify(err));
        console.info("=========ANS_Remove_1600 removeAllCallBack end===============>");
    }

    /*
     * @tc.number: ANS_Remove_1600
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the calling interface removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     *           deletes all notification information through BundleOption Correct bundle, 0 uid.
     */
    it('ANS_Remove_1600', 0, async function (done) {
        console.info("===============ANS_Remove_1600==========================>");
        timesOfOnConsumeRemoveAllByBundleOptionNullUid = 0
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOptionNullUid,
            onCancel:onCancelRemoveAllByBundleOptionNullUid,
        }
        await notify.subscribe(subscriber);
        console.info("==================ANS_Remove_1600 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 16,
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
            label: "1600_1",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        let notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 16,
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
            label: "1600_2",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==================ANS_Remove_1600 publish1 promise==================>");
        await notify.publish(notificationRequest1);
        console.info("==================ANS_Remove_1600 publish2 promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1600 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1600 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveAllByBundleOptionWrongUid(data) {
        console.info("========ANS_Remove_1700 onConsume start==============>");
        console.info("========ANS_Remove_1700 onConsume data:==============>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:123456789
        }
        notify.removeAll(bundleOption,removeAllByBundleOptionCallBackWrongUid);
        console.info("========ANS_Remove_1700 onConsume remove============>");
        console.info("========ANS_Remove_1700 onConsume end===============>");
    }
    function onCancelRemoveAllByBundleOptionWrongUid(data) {
        console.info("==========ANS_Remove_1700 onCancel start====================>");
        console.info("==========ANS_Remove_1700 onCancel data:====================>" + JSON.stringify(data));
        expect().assertFail();
        console.info("==========ANS_Remove_1700 onCancel end======================>");
    }
    function removeAllByBundleOptionCallBackWrongUid(err) {
        console.info("==========ANS_Remove_1700 removeAllCallBack start=============>");
        console.info("==========ANS_Remove_1700 removeAllCallBack err===============>" + JSON.stringify(err));
        console.info("==========ANS_Remove_1700 removeAllCallBack end=======================>");
    }

    /*
     * @tc.number: ANS_Remove_1700
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the calling interface removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     *           deletes all notification information through BundleOption Correct bundle, wrong uid.
     */
    it('ANS_Remove_1700', 0, async function (done) {
        console.info("===============ANS_Remove_1700==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOptionWrongUid,
            onCancel:onCancelRemoveAllByBundleOptionWrongUid,
        }
        await notify.subscribe(subscriber);
        console.info("=============ANS_Remove_1700 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "1700",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("===============ANS_Remove_1700 publish promise===============>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1700 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1700 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);

    })

    function onConsumeRemoveAllByBundleOptionWrongBundleCorrectUid(data) {
        console.info("==============ANS_Remove_1800 onConsume start=============>");
        console.info("==============ANS_Remove_1800 onConsume data:=============>" + JSON.stringify(data));
        let bundleOption = {
            bundle:"wrongBundleName",
            uid:data.request.creatorUid,
        }
        notify.removeAll(bundleOption,removeAllByBundleOptionCallBackWrongBundleCorrectUid);
        console.info("==============ANS_Remove_1800 onConsume remove=============>");
        console.info("==============ANS_Remove_1800 onConsume end================>");
    }
    function onCancelremoveAllByBundleOptionwrongBundleCorrectUid(data) {
        console.info("==============ANS_Remove_1800 onCancel start===============>");
        console.info("==============ANS_Remove_1800 onCancel data:===============>" + JSON.stringify(data));
        expect().assertFail();
        console.info("==============ANS_Remove_1800 onCancel end===============>");
    }
    function removeAllByBundleOptionCallBackWrongBundleCorrectUid(err, data) {
        console.info("=========ANS_Remove_1800 removeAllCallBack start==========>");
        console.info("=========ANS_Remove_1800 removeAllCallBack err============>" + JSON.stringify(err));
        console.info("=========ANS_Remove_1800 removeAllCallBack end==========>");
    }

    /*
     * @tc.number: ANS_Remove_1800
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the calling interface removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     *           deletes all notification information through BundleOption wrong bundle, correct uid.
     */
    it('ANS_Remove_1800', 0, async function (done) {
        console.info("===============ANS_Remove_1800 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOptionWrongBundleCorrectUid,
            onCancel:onCancelremoveAllByBundleOptionwrongBundleCorrectUid,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_1800 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "1800",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ANS_Remove_1800 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1800 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1800 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveAllByBundleOptionNullCharacterBundleCorrectUid(data) {
        console.info("===========ANS_Remove_1900 onConsume start===============>");
        console.info("===========ANS_Remove_1900 onConsume data:===============>" + JSON.stringify(data));
        let bundleOption = {
            bundle:"",
            uid:data.request.creatorUid,
        }
        notify.removeAll(bundleOption,removeAllByBundleOptionCallBackNullCharacterBundleCorrectUid);
        console.info("===========ANS_Remove_1900 onConsume removeAll===========>");
        console.info("===========ANS_Remove_1900 onConsume end=================>");
    }
    function onCancelRemoveAllByBundleOptionNullCharacterBundleCorrectUid(data) {
        console.info("===========ANS_Remove_1900 onCancel start===========>");
        console.info("===========ANS_Remove_1900 onCancel data:==========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("===========ANS_Remove_1900 onCancel end===========>");
    }
    function removeAllByBundleOptionCallBackNullCharacterBundleCorrectUid(err) {
        console.info("========ANS_Remove_1900 removeAllCallback start===========>");
        console.info("========ANS_Remove_1900 removeAllCallback err=============>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("========ANS_Remove_1900 removeAllCallback end=============>");
    }

    /*
     * @tc.number: ANS_Remove_1900
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the calling interface removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     *           deletes all notification information through BundleOption null character bundle, correct uid.
     */
    it('ANS_Remove_1900', 0, async function (done) {
        console.info("===============ANS_Remove_1900 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOptionNullCharacterBundleCorrectUid,
            onCancel:onCancelRemoveAllByBundleOptionNullCharacterBundleCorrectUid,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_1900 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "1900",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("===============ANS_Remove_1900 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_1900 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_1900 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveAllByBundleOptionWrongBundleWrongUid(data) {
        console.info("============ANS_Remove_2000 onConsume start================>");
        console.info("============ANS_Remove_2000 onConsume data:================>" + JSON.stringify(data));
        let bundleOption = {
            bundle:"wrongBundleName",
            uid:123456789,
        }
        notify.removeAll(bundleOption,removeAllByBundleOptionCallBackWrongBundleWrongUid);
        console.info("============ANS_Remove_2000 onConsume remove================>");
        console.info("============ANS_Remove_2000 onConsume end===================>");
    }
    function onCancelRemoveAllByBundleOptionWrongBundleWrongUid(data) {
        console.info("============ANS_Remove_2000 onCancel start=================>");
        console.info("============ANS_Remove_2000 onCancel data:=================>" + JSON.stringify(data));
        expect().assertFail();
        console.info("============ANS_Remove_2000 onCancel end==============>");
    }
    function removeAllByBundleOptionCallBackWrongBundleWrongUid(err) {
        console.info("============ANS_Remove_2000 removeAllCallBack start============>");
        console.info("============ANS_Remove_2000 removeAllCallBack err==============>" + JSON.stringify(err));
        console.info("============ANS_Remove_2000 removeAllCallBack end=============>");
    }

    /*
     * @tc.number: ANS_Remove_2000
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the calling interface removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     *           deletes all notification information through BundleOption wrong bundle, wrong uid.
     */
    it('ANS_Remove_2000', 0, async function (done) {
        console.info("===============ANS_Remove_2000 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOptionWrongBundleWrongUid,
            onCancel:onCancelRemoveAllByBundleOptionWrongBundleWrongUid,
        }
        await notify.subscribe(subscriber);
        console.info("==============ANS_Remove_2000 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "2000",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==============ANS_Remove_2000 publish promise==============>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2000 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2000 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveAllByBundleOptionNullCharacterBundleWrongUid(data) {
        console.info("=========ANS_Remove_2100 onConsume start===================>");
        console.info("=========ANS_Remove_2100 onConsume data:======>" + JSON.stringify(data));
        let bundleOption = {
            bundle:"",
            uid:123456789,
        }
        notify.removeAll(bundleOption,removeAllByBundleOptionCallBackNullCharacterBundleWrongUid);
        console.info("=========ANS_Remove_2100 onConsume remove===================>");
        console.info("=========ANS_Remove_2100 onConsume end===================>");
    }
    function onCancelRemoveAllByBundleOptionNullCharacterBundleWrongUid(data) {
        console.info("=========ANS_Remove_2100 onCancel start==========>");
        console.info("=========ANS_Remove_2100 onCancel data:==========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("=========ANS_Remove_2100 onCancel end============>");
    }
    function removeAllByBundleOptionCallBackNullCharacterBundleWrongUid(err) {
        console.info("=========ANS_Remove_2100 removeAllCallBack start===============>");
        console.info("=========ANS_Remove_2100 err=========>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("=========ANS_Remove_2100 removeAllCallBack end===============>");
    }

    /*
     * @tc.number: ANS_Remove_2100
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the calling interface removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     *           deletes all notification information through BundleOption null character bundle, wrong uid.
     */
    it('ANS_Remove_2100', 0, async function (done) {
        console.info("===============ANS_Remove_2100 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOptionNullCharacterBundleWrongUid,
            onCancel:onCancelRemoveAllByBundleOptionNullCharacterBundleWrongUid,
        }
        await notify.subscribe(subscriber);
        console.info("============ANS_Remove_2100 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "2100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("=========ANS_Remove_2100 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2100 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2100 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveAllByBundleOptionIsUnremovable(data) {
        console.info("==========ANS_Remove_2200 onConsume start==================>");
        console.info("==========ANS_Remove_2200 onConsume data:=================>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        notify.removeAll(bundleOption,removeAllByBundleOptionCallBackIsUnremovable);
        console.info("==========ANS_Remove_2200 onConsume removeAll==================>");
        console.info("==========ANS_Remove_2200 onConsume end==================>");
    }
    function onCancelRemoveAllByBundleOptionIsUnremovable(data) {
        console.info("==========ANS_Remove_2200 onCancel start=================>");
        console.info("==========ANS_Remove_2200 onCancel data:=================>" + JSON.stringify(data));
        expect().assertFail();
        console.info("==========ANS_Remove_2200 onCancel end===================>");
    }
    function removeAllByBundleOptionCallBackIsUnremovable(err) {
        console.info("==========ANS_Remove_2200 removeAllCallBack start==========>");
        console.info("==========ANS_Remove_2200 removeAllCallBack err============>" + JSON.stringify(err));
        console.info("==========ANS_Remove_2200 removeAllCallBack end============>");
    }

    /*
     * @tc.number: ANS_Remove_2200
     * @tc.name: removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void;
     * @tc.desc: Verify that the removeAll(bundle: BundleOption, callback: AsyncCallback<void>):void
                 interface is called to delete the notification information whose attribute isUnremovable is true
     */
    it('ANS_Remove_2200', 0, async function (done) {
        console.info("===============ANS_Remove_2200 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveAllByBundleOptionIsUnremovable,
            onCancel:onCancelRemoveAllByBundleOptionIsUnremovable,
        }
        await notify.subscribe(subscriber);
        console.info("==============ANS_Remove_2200 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 22,
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
            label: "2200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("=============ANS_Remove_2200 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2200 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2200 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function removeAllCallBack(err,data) {
        console.info("================ANS_Remove_2300 removeAllCallBack start:========>");
        console.info("================ANS_Remove_2300 removeAllCallBack err:==========>" + JSON.stringify(err));
        console.info("================ANS_Remove_2300 removeAllCallBack data:=========>" + JSON.stringify(data));
        console.info("================ANS_Remove_2300 removeAllCallBack end:==========>")
    }
    let timesOfOnConsumeRemoveAll
    function onConsumeRemoveAll(data) {
        timesOfOnConsumeRemoveAll = timesOfOnConsumeRemoveAll + 1
        console.info("================ANS_Remove_2300 onConsume start==================>");
        console.info("================ANS_Remove_2300 onConsume data: =================>" + JSON.stringify(data));
        if (timesOfOnConsumeRemoveAll == 2)
        {
            notify.removeAll(removeAllCallBack);
            console.info("============ANS_Remove_2300 onConsume removeAll==========>");
        }
        console.info("================ANS_Remove_2300 onConsume end====================>");
    }
    let timesOfOnCancelRemoveAll
    function onCancelRemoveAll(data) {
        timesOfOnCancelRemoveAll = timesOfOnCancelRemoveAll + 1
        console.info("==================ANS_Remove_2300 onCancel start===========>");
        console.info("==================ANS_Remove_2300 onCancel data============>" + JSON.stringify(data));
        if (timesOfOnCancelRemoveAll == 1)
        {
            expect(data.request.label).assertEqual("2300_1");
        }
        if (timesOfOnCancelRemoveAll == 2)
        {
            expect(data.request.label).assertEqual("2300_2");
        }
        console.info("==================ANS_Remove_2300 onCancel end=============>");
    }

    /*
     * @tc.number: ANS_Remove_2300
     * @tc.name: removeAll(callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the removeAll(callback: AsyncCallback<void>): void interface is called to delete the
     *           notification information
     */
    it('ANS_Remove_2300', 0, async function (done) {
        console.info("===============ANS_Remove_2300 start==========================>");
        timesOfOnConsumeRemoveAll = 0;
        timesOfOnCancelRemoveAll = 0;
        let subscriber ={
            onConsume:onConsumeRemoveAll,
            onCancel:onCancelRemoveAll,
        }
        await notify.subscribe(subscriber);
        console.info("=============ANS_Remove_2300 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "2300_1",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        let notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "2300_2",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("===========ANS_Remove_2300 publish1 promise================>");
        await notify.publish(notificationRequest1);
        console.info("============ANS_Remove_2300 publish2 promise===============>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2300 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2300 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveAllPromise(data) {
        timesOfOnConsumeRemoveAll = timesOfOnConsumeRemoveAll + 1
        console.info("===========ANS_Remove_2400 onConsume start==============>");
        console.info("===========ANS_Remove_2400 onConsume data:==============>" + JSON.stringify(data));
        if (timesOfOnConsumeRemoveAll == 2)
        {
            notify.removeAll();
            console.info("===========ANS_Remove_2400 onConsume removeAll==========>");
        }
        console.info("===========ANS_Remove_2400 onConsume end==============>");
    }

    function onCancelRemoveAllPromise(data) {
        timesOfOnCancelRemoveAll = timesOfOnCancelRemoveAll + 1
        console.info("===========ANS_Remove_2400 onCancel start=======================>");
        console.info("===========ANS_Remove_2400 onCancel data : =============>" + JSON.stringify(data));
        if (timesOfOnCancelRemoveAll == 1)
        {
            expect(data.request.label).assertEqual("2400_1");
        }
        if (timesOfOnCancelRemoveAll == 2)
        {
            expect(data.request.label).assertEqual("2400_2");
        }
        console.info("===========ANS_Remove_2400 onCancel end=======================>");
    }

    /*
     * @tc.number: ANS_Remove_2400
     * @tc.name: removeAll(bundle?: BundleOption): Promise<void>
     * @tc.desc: Verify that the removeAll(bundle?: BundleOption): Promise<void> interface is called to delete the
     *           notification information
     */
    it('ANS_Remove_2400', 0, async function (done) {
        console.info("===============ANS_Remove_2400 start==========================>");
        timesOfOnConsumeRemoveAll = 0
        timesOfOnCancelRemoveAll = 0
        let subscriber ={
            onConsume:onConsumeRemoveAllPromise,
            onCancel:onCancelRemoveAllPromise,
        }
        await notify.subscribe(subscriber);
        console.info("==============ANS_Remove_2400 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "2400_1",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        let notificationRequest1 = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
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
            statusBarText: "statusBarText",
            isFloatingIcon : true,
            label: "2400_2",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("===ANS_Remove_2400 publish1 promise============>");
        await notify.publish(notificationRequest1);
        console.info("===ANS_Remove_2400 publish2 promise============>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2400 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2400 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function removeAllCallBackIsUnremovable(err,data) {
        console.info("=========ANS_Remove_2500 removeAllCallBack start=========>");
        console.info("=========ANS_Remove_2500 removeAllCallBack err:==========>" + JSON.stringify(err));
        console.info("=========ANS_Remove_2500 removeAllCallBack data:=========>" + JSON.stringify(data));
        console.info("=========ANS_Remove_2500 removeAllCallBack end===========>");
    }
    function onConsumeRemoveAllIsUnremovable(data) {
        console.info("=========ANS_Remove_2500 onConsume start===========>");
        console.info("=========ANS_Remove_2500 onConsumedata:============>" + JSON.stringify(data));
        notify.removeAll(removeAllCallBackIsUnremovable);
        console.info("=========ANS_Remove_2500 onConsume removeAll=======>");
        console.info("=========ANS_Remove_2500 onConsume end=============>");
    }
    function onCancelCallbackRemoveAllIsUnremovable(data) {
        expect().assertFail();
        console.info("=========ANS_Remove_2500 onCancel start==============>");
        console.info("=========ANS_Remove_2500 onCancel data:==============>" + JSON.stringify(data));
        console.info("=========ANS_Remove_2500 onCancel end================>");
    }

    /*
     * @tc.number: ANS_Remove_2500
     * @tc.name: removeAll(callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the removeAll(callback: AsyncCallback<void>): void interface is called to delete the
     *           notification information whose attribute isUnremovable is true
     */
    it('ANS_Remove_2500', 0, async function (done) {
        console.info("===============ANS_Remove_2500 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveAllIsUnremovable,
            onCancel:onCancelCallbackRemoveAllIsUnremovable,
        }
        await notify.subscribe(subscriber);
        console.info("======ANS_Remove_2500 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 25,
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
            label: "2500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ANS_Remove_2500 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2500 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2500 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function OnConsumeRemoveAllIsUnremovablePromise(data) {
        console.info("===========ANS_Remove_2600 onConsume start===========>");
        console.info("===========ANS_Remove_2600 onConsume data============>" + JSON.stringify(data));
        notify.removeAll()
        console.info("===========ANS_Remove_2600 onConsume removeAll=======>");
        console.info("===========ANS_Remove_2600 onConsume end=============>");
    }
    function OnCancelCallbackRemoveAllIsUnremovablePromise(data) {
        console.info("===========ANS_Remove_2600 onCancel start================>");
        console.info("===========ANS_Remove_2600 onCancel data : ==============>" + JSON.stringify(data));
        console.info("===========ANS_Remove_2600 onCancel end==================>");
    }

    /*
     * @tc.number: ANS_Remove_2600
     * @tc.name: removeAll(bundle?: BundleOption): Promise<void>;
     * @tc.desc: Verify that the removeAll(bundle?: BundleOption): Promise<void> interface is called to delete the
     *           notification information whose attribute isUnremovable is true
     */
    it('ANS_Remove_2600', 0, async function (done) {
        console.info("===============ANS_Remove_2600 start==========================>");
        let subscriber ={
            onConsume:OnConsumeRemoveAllIsUnremovablePromise,
            onCancel:OnCancelCallbackRemoveAllIsUnremovablePromise,
        }
        await notify.subscribe(subscriber);
        console.info("============ANS_Remove_2600 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 26,
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
            label: "2600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==========ANS_Remove_2600 publish promise==============>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2600 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2600 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function removeAllCallBack2TimesOf2(err) {
        console.info("=======ANS_Remove_2700 removeAllCallBack2TimesOf2 start===========>");
        console.info("=======ANS_Remove_2700 removeAllCallBack2TimesOf2 err=============>" + JSON.stringify(err));
        console.info("=======ANS_Remove_2700 removeAllCallBack2TimesOf2 end=============>");
    }
    function removeAllCallBack2TimesOf1(err) {
        console.info("=======ANS_Remove_2700 removeAllCallBack2TimesOf1 start==========>");
        console.info("=======ANS_Remove_2700 removeAllCallBack2TimesOf1 err============>" + JSON.stringify(err));
        expect(err.code).assertEqual(0);
        notify.removeAll(removeAllCallBack2TimesOf2);
        console.info("=======ANS_Remove_2700 removeAllCallBack2TimesOf1 removeAll=====>");
        console.info("=======ANS_Remove_2700 removeAllCallBack2TimesOf1 end==========>");
    }
    function onConsumeRemoveAll2Times(data) {
        console.info("=======ANS_Remove_2700 onConsume start=========>");
        console.info("=======ANS_Remove_2700 onConsume data:=========>" + JSON.stringify(data));
        notify.removeAll(removeAllCallBack2TimesOf1);
        console.info("=======ANS_Remove_2700 onConsume remove========>");
        console.info("=======ANS_Remove_2700 onConsume end===========>");
    }
    let timesOfOnCancelCallbackRemoveAll2Times
    function onCancelRemoveAll2Times(data) {
        timesOfOnCancelCallbackRemoveAll2Times = timesOfOnCancelCallbackRemoveAll2Times + 1
        console.info("=======ANS_Remove_2700 onCancel start============>");
        console.info("=======ANS_Remove_2700 onCancel data:============>" + JSON.stringify(data));
        if (timesOfOnCancelCallbackRemoveAll2Times == 1){
            expect(data.request.id).assertEqual(27);
            console.info("=======ANS_Remove_2700 onCancel id============>");
        } else if  (timesOfOnCancelCallbackRemoveAll2Times == 2){
            expect().assertFail();
        }
        console.info("=======ANS_Remove_2700 onCancel end============>");
    }

    /*
     * @tc.number: ANS_Remove_2700
     * @tc.name: remove(hashCode: string, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the interface  remove(hashCode: string, callback: AsyncCallback<void>): void; is called
     *           twice in a row to delete the notification information
     */
    it('ANS_Remove_2700', 0, async function (done) {
        console.info("===============ANS_Remove_2700 start==========================>");
        timesOfOnCancelCallbackRemoveAll2Times = 0
        let subscriber ={
            onConsume:onConsumeRemoveAll2Times,
            onCancel:onCancelRemoveAll2Times,
        }
        await notify.subscribe(subscriber);
        console.info("============ANS_Remove_2700 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 27,
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
            label: "2700",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("=========ANS_Remove_2700 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2700 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2700 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeCallbackRemoveAll2TimesPromise(data) {
        console.info("=============ANS_Remove_2800 onConsume start=============>");
        console.info("=============ANS_Remove_2800 onConsume data: =======================>" + JSON.stringify(data));
        notify.removeAll();
        console.info("=============ANS_Remove_2800 onConsume removeAll1=======================>");
        notify.removeAll();
        console.info("=============ANS_Remove_2800 onConsume removeAll2=======================>");
        console.info("=============ANS_Remove_2800 onConsume end=============>");
    }
    let timesOfOnCancelCallbackRemoveAll2TimesPromise
    function onCancelCallbackRemoveAll2TimesPromise(data) {
        timesOfOnCancelCallbackRemoveAll2TimesPromise = timesOfOnCancelCallbackRemoveAll2TimesPromise + 1
        console.info("===================ANS_Remove_2800 onCancel start=======================>");
        console.info("===================ANS_Remove_2800 onCancel data : ========>" + JSON.stringify(data));
        if (timesOfOnCancelCallbackRemoveAll2TimesPromise == 1){
            expect(data.request.id).assertEqual(28);
        } else if  (timesOfOnCancelCallbackRemoveAll2TimesPromise == 2){
            expect().assertFail();
        }
        console.info("===================ANS_Remove_2800 onCancel end=======================>");
    }

    /*
     * @tc.number: ANS_Remove_2800
     * @tc.name: removeAll(bundle?: BundleOption): Promise<void>;
     * @tc.desc: Verify that the interface removeAll(bundle?: BundleOption): Promise<void>; is called twice in a row
     *           to delete the notification information
     */
    it('ANS_Remove_2800', 0, async function (done) {
        console.info("===============ANS_Remove_2800 start==========================>");
        timesOfOnCancelCallbackRemoveAll2TimesPromise = 0
        let subscriber ={
            onConsume:onConsumeCallbackRemoveAll2TimesPromise,
            onCancel:onCancelCallbackRemoveAll2TimesPromise,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_2800 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 28,
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
            label: "2800",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("================ANS_Remove_2800 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2800 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2800 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveByNotificationKey(data) {
        console.info("=======ANS_Remove_2900 onConsume start=======================>");
        console.info("=======ANS_Remove_2900 onConsume data: =======================>" + JSON.stringify(data));
        console.info("=======ANS_Remove_2900 onConsume creatorBundleName:====>" + data.request.creatorBundleName);
        console.info("=======ANS_Remove_2900 onConsume creatorUid:==============>" + data.request.creatorUid);
        console.info("=======ANS_Remove_2900 onConsume id: =======================>" + data.request.id);
        console.info("=======ANS_Remove_2900 onConsume label: =======================>" + data.request.label);
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        let notificationKey = {
            id:data.request.id,
            label:data.request.label
        }
        notify.remove(bundleOption,notificationKey,removeByNotificationKeyCB);
        console.info("=============ANS_Remove_2900 onConsume remove=======================>");
        console.info("=============ANS_Remove_2900 onConsume end=======================>");
    }
    function onCancelRemoveByNotificationKey(data) {
        console.info("=============ANS_Remove_2900 onCancel start================>");
        console.info("=============ANS_Remove_2900 onCancel data:==========>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("2900");
        console.info("=============ANS_Remove_2900 onCancel label:==========>" + JSON.stringify(data.request.label));
        expect(data.request.id).assertEqual(29);
        console.info("=============ANS_Remove_2900 onCancel id:==========>" + JSON.stringify(data.request.id));
        console.info("=============ANS_Remove_2900 onCancel end================>");
    }
    function removeByNotificationKeyCB(err, data) {
        console.info("==========ANS_Remove_2900 removeCalback start============>");
        console.info("==========ANS_Remove_2900 removeCalback err==============>" + JSON.stringify(err));
        console.info("==========ANS_Remove_2900 removeCalback data=============>" + JSON.stringify(data));
        console.info("==========ANS_Remove_2900 removeCalback end============>");
    }

    /*
     * @tc.number: ANS_Remove_2900
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the calling interface remove(bundle: BundleOption, notificationKey: NotificationKey,
     *           callback: AsyncCallback<void>): void; deletes notification information through BundleOption and
     *           NotificationKey
     */
    it('ANS_Remove_2900', 0, async function (done) {
        console.info("===============ANS_Remove_2900 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveByNotificationKey,
            onCancel:onCancelRemoveByNotificationKey,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_2900 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 29,
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
            label: "2900",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("==============ANS_Remove_2900 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_2900 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_2900 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveByNotificationKeyPromise(data) {
        console.info("==========ANS_Remove_3000 onConsume start==============>");
        console.info("==========ANS_Remove_3000 data: =======================>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        let notificationKey = {
            id:30,
            label:"3000"
        }
        notify.remove(bundleOption,notificationKey)
        console.info("==========ANS_Remove_3000 onConsume remove==============>");
        console.info("==========ANS_Remove_3000 onConsume end=================>");
    }
    function onCancelRemoveByNotificationKeyPromise(data) {
        console.info("========ANS_Remove_3000 onCancel start========>");
        console.info("========ANS_Remove_3000 onCancel data:========>" + JSON.stringify(data));
        expect(data.request.label).assertEqual("3000");
        expect(data.request.id).assertEqual(30);
        console.info("========ANS_Remove_3000 onCancel end==========>");
    }

    /*
     * @tc.number: ANS_Remove_3000
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey): Promise<void>;
     * @tc.desc: Verify that the calling interface
                 remove(bundle: BundleOption, notificationKey: NotificationKey): Promise<void>
                 deletes notification information through BundleOption and NotificationKey
     */
    it('ANS_Remove_3000', 0, async function (done) {
        console.info("===============ANS_Remove_3000 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveByNotificationKeyPromise,
            onCancel:onCancelRemoveByNotificationKeyPromise,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_3000 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 30,
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
            label: "3000",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("=============ANS_Remove_3000 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_3000 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_3000 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveByNotificationKeyWrongKey(data) {
        console.info("==========ANS_Remove_3100 onConsume start=============>");
        console.info("==========ANS_Remove_3100 onConsume data:=============>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        let notificationKey = {
            id:0,
            label:"wrongLabel"
        }
        notify.remove(bundleOption,notificationKey,removeByNotificationKeyCBWrongKey);
        console.info("==========ANS_Remove_3100 onConsume remove==========>");
        console.info("==========ANS_Remove_3100 onConsume end=============>");
    }
    function onCancelRemoveByNotificationKeyWrongKey(data) {
        console.info("==========ANS_Remove_3100 onCancel start===========>");
        console.info("==========ANS_Remove_3100 onCancel data:===========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("==========ANS_Remove_3100 onCancel end===========>");
}
    function removeByNotificationKeyCBWrongKey(err, data) {
        console.info("==========ANS_Remove_3100 removeCallback start===========>");
        console.info("==========ANS_Remove_3100 removeCallback err=============>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("==========ANS_Remove_3100 removeCallback data============>" + JSON.stringify(data));
        console.info("==========ANS_Remove_3100 removeCallback end===========>");
    }

    /*
     * @tc.number: ANS_Remove_3100
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the calling interface remove(bundle: BundleOption, notificationKey: NotificationKey,
     *           callback: AsyncCallback<void>): void; deletes notification information through BundleOption and
     *           NotificationKey correct bundleOption,wrong notificationKey.
     */
    it('ANS_Remove_3100', 0, async function (done) {
        console.info("===============ANS_Remove_3100 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveByNotificationKeyWrongKey,
            onCancel:onCancelRemoveByNotificationKeyWrongKey,
        }
        await notify.subscribe(subscriber);
        console.info("==============ANS_Remove_3100 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 31,
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
            label: "3100",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("=============ANS_Remove_3100 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_3100 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_3100 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveByNotificationKeyWrongKeyPromise(data) {
        console.info("===========ANS_Remove_3200 onConsume start===============>");
        console.info("===========ANS_Remove_3200 onConsume data:===============>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        let notificationKey = {
            id:0,
            label:"wrongLabel"
        }
        notify.remove(bundleOption, notificationKey).then((data)=>{
            console.info("=======ANS_Remove_3200 onConsume remove data:=======>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("=======ANS_Remove_3200 onConsume remove err:========>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
        });
        console.info("===========ANS_Remove_3200 onConsume remove============>");
        console.info("===========ANS_Remove_3200 onConsume end===============>");
    }
    function onCancelRemoveByNotificationKeyWrongKeyPromise(data) {
        console.info("===========ANS_Remove_3200 onCancel start=================>");
        console.info("===========ANS_Remove_3200 onCanceldata:==================>" + JSON.stringify(data));
        expect().assertFail();
        console.info("===========ANS_Remove_3200 onCancel end=================>");
    }

    /*
     * @tc.number: ANS_Remove_3200
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the calling interface remove(bundle: BundleOption, notificationKey: NotificationKey):
     *           Promise<void> deletes notification information through BundleOption and NotificationKey correct
     *           bundleOption,wrong notificationKey.
     */
    it('ANS_Remove_3200', 0, async function (done) {
        console.info("===============ANS_Remove_3200 start=================>");
        let subscriber ={
            onConsume:onConsumeRemoveByNotificationKeyWrongKeyPromise,
            onCancel:onCancelRemoveByNotificationKeyWrongKeyPromise,
        }
        await notify.subscribe(subscriber);
        console.info("===============ANS_Remove_3200 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 32,
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
            label: "3200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("===========ANS_Remove_3200 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_3200 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_3200 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function OnConsumeRemoveByNotificationKeyWrongBundle(data) {
        console.info("==============ANS_Remove_3300 onConsume start===============>");
        console.info("==============ANS_Remove_3300 onConsume data:===============>" + JSON.stringify(data));
        let bundleOption = {
            bundle:"wrongBundleName",
            uid:0,
        }
        let notificationKey = {
            id:33,
            label:"3300"
        }
        notify.remove(bundleOption,notificationKey,removeByNotificationKeyCBWrongBundle);
        console.info("==============ANS_Remove_3300 onConsume remove===============>");
        console.info("==============ANS_Remove_3300 onConsume end==================>");
    }
    function OnCancelRemoveByNotificationKeyWrongBundle(data) {
        console.info("==============ANS_Remove_3300 onCancel start=============>");
        console.info("==============ANS_Remove_3300 onCancel data:===========>" + JSON.stringify(data));
        expect().assertFail();
        console.info("==============ANS_Remove_3300 onCancel end=============>");
    }
    function removeByNotificationKeyCBWrongBundle(err, data) {
        console.info("===========ANS_Remove_3300 removeCallback start==========>");
        console.info("===========ANS_Remove_3300 removeCallback err============>" + JSON.stringify(err));
        expect(err.code != 0).assertEqual(true);
        console.info("===========ANS_Remove_3300 removeCallback data===========>" + JSON.stringify(data));
        console.info("===========ANS_Remove_3300 removeCallback end============>");
    }

    /*
     * @tc.number: ANS_Remove_3300
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the calling interface remove(bundle: BundleOption, notificationKey: NotificationKey,
     *           callback: AsyncCallback<void>): void; deletes notification information through BundleOption and
     *           NotificationKey wrong bundleOption,correct notificationKey.
     */
    it('ANS_Remove_3300', 0, async function (done) {
        console.info("===============ANS_Remove_3300 start==========================>");
        let subscriber ={
            onConsume:OnConsumeRemoveByNotificationKeyWrongBundle,
            onCancel:OnCancelRemoveByNotificationKeyWrongBundle,
        }
        await notify.subscribe(subscriber);
        console.info("================ANS_Remove_3300 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 33,
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
            label: "3300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("================ANS_Remove_3300 publish promise=================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_3300 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_3300 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveByNotificationKeyWrongBundlePromise(data) {
        console.info("===========ANS_Remove_3400 onConsume start============>");
        console.info("===========ANS_Remove_3400 onConsume data:============>" + JSON.stringify(data));
        let bundleOption = {
            bundle:"wrongBundleName",
            uid:0,
        }
        let notificationKey = {
            id:34,
            label:"3400"
        }
        notify.remove(bundleOption, notificationKey).then((data)=>{
            console.info("=======ANS_Remove_3400 onConsume remove data:=======>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("=======ANS_Remove_3400 onConsume remove err:========>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
        });
        console.info("===========ANS_Remove_3400 onConsume remove=========>");
        console.info("===========ANS_Remove_3400 onConsume end============>");
    }
    function onCancelRemoveByNotificationKeyWrongBundlePromise(data) {
        console.info("===========ANS_Remove_3400 onCancel start=============>");
        console.info("===========ANS_Remove_3400 onCancel data: ============>" + JSON.stringify(data));
        expect().assertFail();
        console.info("===========ANS_Remove_3400 onCancel end===============>");
    }

    /*
     * @tc.number: ANS_Remove_3400
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the calling interface remove(bundle: BundleOption, notificationKey: NotificationKey):
     *           Promise<void> deletes notification information through BundleOption and NotificationKey correct
     *           bundleOption,wrong notificationKey.
     */
    it('ANS_Remove_3400', 0, async function (done) {
        console.info("===============ANS_Remove_3400 start==========================>");
        let subscriber ={
            onConsume:onConsumeRemoveByNotificationKeyWrongBundlePromise,
            onCancel:onCancelRemoveByNotificationKeyWrongBundlePromise,
        }
        await notify.subscribe(subscriber);
        console.info("============ANS_Remove_3400 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 34,
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
            label: "3400",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ANS_Remove_3400 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_3400 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_3400 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function removeByNotificationKey2Times2CB(err,data) {
        console.info("==========ANS_Remove_3500 removeCallback2Times2 start============>");
        console.info("==========ANS_Remove_3500 removeCallback2Times2 err==============>" + JSON.stringify(err));
        console.info("==========ANS_Remove_3500 removeCallback2Times2 data============>" + JSON.stringify(data));
        expect(err.code != 0).assertEqual(true);
        console.info("==========ANS_Remove_3500 removeCallback2Times2 end================>");
    }
    function removeByNotificationKey2Times1CB(err,data) {
        console.info("==========ANS_Remove_3500 removeCallback2Times1 start===========>");
        console.info("==========ANS_Remove_3500 removeCallback2Times1 err=============>" + JSON.stringify(err));
        console.info("==========ANS_Remove_3500 removeCallback2Times1 data============>" + JSON.stringify(data));
        expect(err.code).assertEqual(0);
        console.info("==========ANS_Remove_3500 removeCallback2Times1 end==============>");
    }
    function onConsumeRemoveByNotificationKey2Times(data) {
        console.info("=========ANS_Remove_3500 onConsume start=======================>");
        console.info("=========ANS_Remove_3500 onConsume data:=========>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        let notificationKey = {
            id:35,
            label:"3500"
        }
        notify.remove(bundleOption,notificationKey,removeByNotificationKey2Times1CB);
        notify.remove(bundleOption,notificationKey,removeByNotificationKey2Times2CB);
        console.info("=========ANS_Remove_3500 onConsume remove==============>");
        console.info("=========ANS_Remove_3500 onConsume end=================>");
    }
    let timesOfOnCancelRemoveByNotificationKey2Times
    function onCancelRemoveByNotificationKey2Times(data) {
        timesOfOnCancelRemoveByNotificationKey2Times = timesOfOnCancelRemoveByNotificationKey2Times + 1
        console.info("=========ANS_Remove_3500 onCancel start===============>");
        console.info("=========ANS_Remove_3500 onCancel data : ========>" + JSON.stringify(data));
        if (timesOfOnCancelRemoveByNotificationKey2Times == 1){
            expect(data.request.id).assertEqual(35);
            expect(data.request.label).assertEqual("3500");
        } else if  (timesOfOnCancelRemoveByNotificationKey2Times == 2){
            expect().assertFail();
        }
        console.info("=========ANS_Remove_3500 onCancel end=======================>");
    }

    /*
     * @tc.number: ANS_Remove_3500
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey, callback: AsyncCallback<void>): void;
     * @tc.desc: Verify that the interface  remove(bundle: BundleOption, notificationKey: NotificationKey, callback:
     *           AsyncCallback<void>): void; void; is called twice in a row to delete the notification information
     */
    it('ANS_Remove_3500', 0, async function (done) {
        console.info("===============ANS_Remove_3500 start==========================>");
        timesOfOnCancelRemoveByNotificationKey2Times = 0
        let subscriber ={
            onConsume:onConsumeRemoveByNotificationKey2Times,
            onCancel:onCancelRemoveByNotificationKey2Times,
        }
        await notify.subscribe(subscriber);
        console.info("===========ANS_Remove_3500 subscribe promise==================>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 35,
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
            label: "3500",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ANS_Remove_3500 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_3500 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_3500 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })

    function onConsumeRemoveByNotificationKey2TimesPromise(data) {
        console.info("=========ANS_Remove_3600 onConsume start===========>");
        console.info("=========ANS_Remove_3600 onConsume data: =======================>" + JSON.stringify(data));
        let bundleOption = {
            bundle:data.request.creatorBundleName,
            uid:data.request.creatorUid,
        }
        let notificationKey = {
            id:36,
            label:"3600"
        }
        notify.remove(bundleOption, notificationKey)
        console.info("=========ANS_Remove_3600 onConsume remove1===========>");
        notify.remove(bundleOption, notificationKey).then((data)=>{
            console.info("=======ANS_Remove_3600 onConsume remove2 data:=======>" + JSON.stringify(data));
        }).catch((err)=>{
            console.info("=======ANS_Remove_3600 onConsume remove2 err:========>" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.info("=========ANS_Remove_3600 onConsume end===========>");
        });
    }
    let timesOnCancelcbRemoveByKey2TimesPromise
    function onCancelRemoveByNotificationKey2TimesPromise(data) {
        timesOnCancelcbRemoveByKey2TimesPromise = timesOnCancelcbRemoveByKey2TimesPromise + 1
        console.info("=========ANS_Remove_3600 onCancel start=============>");
        console.info("=========ANS_Remove_3600 onCancel data:=============>" + JSON.stringify(data));
        if (timesOnCancelcbRemoveByKey2TimesPromise == 1){
            expect(data.request.id).assertEqual(36);
            expect(data.request.label).assertEqual("3600");
        } else if  (timesOnCancelcbRemoveByKey2TimesPromise == 2){
            expect().assertFail();
        }
        console.info("=========ANS_Remove_3600 onCancel end=============>");
    }

    /*
     * @tc.number: ANS_Remove_3600
     * @tc.name: remove(bundle: BundleOption, notificationKey: NotificationKey): Promise<void>;
     * @tc.desc: Verify that the interface remove(bundle: BundleOption, notificationKey: NotificationKey):
     *           Promise<void> is called twice in a row to delete the notification information
     */
    it('ANS_Remove_3600', 0, async function (done) {
        console.info("===============ANS_Remove_3600 start==========================>");
        timesOnCancelcbRemoveByKey2TimesPromise = 0
        timesOnCancelcbRemoveByKey2TimesPromise = 0
        let subscriber ={
            onConsume:onConsumeRemoveByNotificationKey2TimesPromise,
            onCancel:onCancelRemoveByNotificationKey2TimesPromise,
        }
        await notify.subscribe(subscriber);
        console.info("========ANS_Remove_3600 subscribe promise=============>");
        let notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 36,
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
            label: "3600",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        await notify.publish(notificationRequest);
        console.info("============ANS_Remove_3600 publish promise==================>");
        setTimeout((async function(){
            console.info("======ANS_Remove_3600 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======ANS_Remove_3600 setTimeout unsubscribe============>");
            await notify.cancelAll();
            done();
        }),timeout);
    })
}) 