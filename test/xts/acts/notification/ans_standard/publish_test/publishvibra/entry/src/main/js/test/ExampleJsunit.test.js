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
describe('ActsAnsPublishVibraTest', function () {
    console.info("===========ActsAnsPublishVibraTest start====================>");
    var timeout = 200
    var bundleoption = {
        bundle: "com.example.actsanspublishvibratest"
    }
    var timesOfOnConsume
    function onConsume0100(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0100 onConsume start============>");
        console.info("=============Ans_PublishVibra_0100 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0100 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0200 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([200]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0100 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([1,0,1,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0100 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0100 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0100
     * @tc.name: addSlot(type: SlotType): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a SOCIAL_COMMUNICATION type slot, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0100', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0100================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume0100
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0100 subscribe======>");
        await notify.addSlot(notify.SlotType.SOCIAL_COMMUNICATION);
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
            slotType : notify.SlotType.SOCIAL_COMMUNICATION,
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0100 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.SOCIAL_COMMUNICATION,
                    vibrationValues:[1,0,1,0],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0100 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0100 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0100 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0100 publish1 then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0100 setTimeout========>");
            await notify.removeSlot(notify.SlotType.SOCIAL_COMMUNICATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0100 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0200(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0200 onConsume start============>");
        console.info("=============Ans_PublishVibra_0200 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0200 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0200 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([200]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0200 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([2,0,2,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0200 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0200 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0200
     * @tc.name: addSlot(type: SlotType): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a SERVICE_INFORMATION  type slot, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0200', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0200================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume0200
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0200 subscribe======>");
        await notify.addSlot(notify.SlotType.SERVICE_INFORMATION);
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
            slotType : notify.SlotType.SERVICE_INFORMATION ,
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0200 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.SERVICE_INFORMATION ,
                    vibrationValues:[2,0,2,0],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0200 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0200 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0200 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0200 publish1 then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0200 setTimeout========>");
            await notify.removeSlot(notify.SlotType.SERVICE_INFORMATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0200 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0300(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0300 onConsume start============>");
        console.info("=============Ans_PublishVibra_0300 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0300 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0300 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([]));
            expect(vibrationEnabled).assertEqual(false);
            console.info("=============Ans_PublishVibra_0300 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([3,0,3,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0300 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0300 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0300
     * @tc.name: addSlot(type: SlotType): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a CONTENT_INFORMATION type slot, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0300', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0300================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume0300
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0300 subscribe======>");
        await notify.addSlot(notify.SlotType.CONTENT_INFORMATION);
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 3,
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
            label: "0300",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0300 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.CONTENT_INFORMATION,
                    vibrationValues:[3,0,3,0],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0300 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0300 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0300 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0300 publish1 then catch err======>"+err);
        })

        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0300 setTimeout========>");
            await notify.removeSlot(notify.SlotType.CONTENT_INFORMATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0300 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0400(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0400 onConsume start============>");
        console.info("=============Ans_PublishVibra_0400 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0400 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0400 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([]));
            expect(vibrationEnabled).assertEqual(false);
            console.info("=============Ans_PublishVibra_0400 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([4,0,4,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0400 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0400 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0400
     * @tc.name: addSlot(type: SlotType): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a OTHER_TYPES type slot, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0400', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0400================>");
        timesOfOnConsume = 0;
        var subscriber = {
            onConsume:onConsume0400
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0400 subscribe======>");
        await notify.addSlot(notify.SlotType.OTHER_TYPES);
        var notificationRequest = {
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0400 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.OTHER_TYPES,
                    vibrationValues:[4,0,4,0],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0400 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0400 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0400 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0400 publish1 then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0400 setTimeout========>");
            await notify.removeSlot(notify.SlotType.OTHER_TYPES);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0400 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0500(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0500 onConsume start============>");
        console.info("=============Ans_PublishVibra_0500 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0500 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0500 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([]));
            expect(vibrationEnabled).assertEqual(false);
            console.info("=============Ans_PublishVibra_0500 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([5,0,5,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0500 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0500 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0500
     * @tc.name: addSlot(type: SlotType): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a UNKNOWN_TYPE type slot, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0500', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0500================>");
        timesOfOnConsume = 0;
        var subscriber = {
            onConsume:onConsume0500
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0500 subscribe======>");
        await notify.addSlot(notify.SlotType.UNKNOWN_TYPE);
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 5,
            slotType : notify.SlotType.UNKNOWN_TYPE,
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0500 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.OTHER_TYPES,
                    vibrationValues:[5,0,5,0],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0500 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0500 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0500 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0500 publish1 then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0500 setTimeout========>");
            await notify.removeSlot(notify.SlotType.UNKNOWN_TYPE);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0500 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0600(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0600 onConsume start============>");
        console.info("=============Ans_PublishVibra_0600 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0600 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0600 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([6,0,6,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0600 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([6,6,6,6]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0600 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0600 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0600
     * @tc.name: addSlot(slot: NotificationSlot): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a SOCIAL_COMMUNICATION type slot, set vibra information, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0600', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0600================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume0600
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0600 subscribe======>");
        await notify.addSlot({
            type:notify.SlotType.SOCIAL_COMMUNICATION,
            vibrationValues:[6,0,6,0],
        });
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 6,
            slotType : notify.SlotType.SOCIAL_COMMUNICATION,
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0600 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.SOCIAL_COMMUNICATION,
                    vibrationValues:[6,6,6,6],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0600 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0600 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0600 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0600 publish1 then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0600 setTimeout========>");
            await notify.removeSlot(notify.SlotType.SOCIAL_COMMUNICATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0600 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0700(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0700 onConsume start============>");
        console.info("=============Ans_PublishVibra_0700 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0700 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0700 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([7,0,7,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0700 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([7,7,7,7]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0700 onConsume timesOfOnConsume2============>");
        }

        console.info("=============Ans_PublishVibra_0700 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0700
     * @tc.name: addSlot(slot: NotificationSlot): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a SERVICE_INFORMATION  type slot, set vibra information, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0700', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0700================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume0700
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0700 subscribe======>");
        await notify.addSlot({
            type:notify.SlotType.SERVICE_INFORMATION,
            vibrationValues:[7,0,7,0],
        });
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 7,
            slotType : notify.SlotType.SERVICE_INFORMATION ,
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0700 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.SERVICE_INFORMATION ,
                    vibrationValues:[7,7,7,7],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0700 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0700 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0700 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0700 publish1 then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0700 setTimeout========>");
            await notify.removeSlot(notify.SlotType.SERVICE_INFORMATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0700 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0800(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0800 onConsume start============>");
        console.info("=============Ans_PublishVibra_0800 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0800 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0800 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([8,0,8,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0800 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([8,8,8,8]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0800 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0800 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0800
     * @tc.name: addSlot(slot: NotificationSlot): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a CONTENT_INFORMATION type slot, set vibra information, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0800', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0800================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume0800
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0800 subscribe======>");
        await notify.addSlot({
            type:notify.SlotType.CONTENT_INFORMATION,
            vibrationValues:[8,0,8,0],
        });
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 8,
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
            label: "0800",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0800 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.CONTENT_INFORMATION,
                    vibrationValues:[8,8,8,8],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0800 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0800 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0800 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0800 publish1 then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0800 setTimeout========>");
            await notify.removeSlot(notify.SlotType.CONTENT_INFORMATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0800 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume0900(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_0900 onConsume start============>");
        console.info("=============Ans_PublishVibra_0900 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_0900 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_0900 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([9,0,9,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0900 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([9,9,9,9]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_0900 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_0900 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_0900
     * @tc.name: addSlot(slot: NotificationSlot): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a OTHER_TYPES type slot, set vibra information, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_0900', 0,async function (done) {
        console.info("===============Ans_PublishVibra_0900================>");
        timesOfOnConsume = 0;
        var subscriber = {
            onConsume:onConsume0900
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_0900 subscribe======>");
        await notify.addSlot({
            type:notify.SlotType.OTHER_TYPES,
            vibrationValues:[9,0,9,0],
        });
        var notificationRequest = {
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_0900 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.OTHER_TYPES,
                    vibrationValues:[9,9,9,9],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_0900 setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_0900 setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_0900 setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_0900 publish1 then catch err======>"+err);
        })

        setTimeout((async function(){
            console.info("======Ans_PublishVibra_0900 setTimeout========>");
            await notify.removeSlot(notify.SlotType.OTHER_TYPES);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_0900 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume1000(data){
        timesOfOnConsume ++;
        console.info("=============Ans_PublishVibra_1000 onConsume start============>");
        console.info("=============Ans_PublishVibra_1000 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_1000 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_1000 onConsume vibrationEnabled============>" + vibrationEnabled);
        if (timesOfOnConsume == 1){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([10,0,10,0]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_1000 onConsume timesOfOnConsume1============>");
        } else if (timesOfOnConsume == 2){
            expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([10,10,10,10]));
            expect(vibrationEnabled).assertEqual(true);
            console.info("=============Ans_PublishVibra_1000 onConsume timesOfOnConsume2============>");
        }
        console.info("=============Ans_PublishVibra_1000 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_1000
     * @tc.name: addSlot(slot: NotificationSlot): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a UNKNOWN_TYPE type slot, set vibra information, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_1000', 0,async function (done) {
        console.info("===============Ans_PublishVibra_1000================>");
        timesOfOnConsume = 0;
        var subscriber = {
            onConsume:onConsume1000
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_1000 subscribe======>");
        await notify.addSlot({
            type:notify.SlotType.UNKNOWN_TYPE,
            vibrationValues:[10,0,10,0],
        });
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 10,
            slotType : notify.SlotType.UNKNOWN_TYPE,
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_1000 publish1 then======>");
            notify.setSlotByBundle(bundleoption,
                {
                    type:notify.SlotType.UNKNOWN_TYPE,
                    vibrationValues:[10,10,10,10],
                }).then(()=>{
                console.info("=======Ans_PublishVibra_1000 publish1 then setSlotByBundle then======>");
                notify.publish(notificationRequest);
                console.info("=======Ans_PublishVibra_1000 publish1 then setSlotByBundle then publish2======>");
            }).catch((err)=>{
                console.info("=======Ans_PublishVibra_1000 publish1 then setSlotByBundle catch err======>"+err);
            })
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_1000 publish1 then catch err======>"+err);
        })

        setTimeout((async function(){
            console.info("======Ans_PublishVibra_1000 setTimeout========>");
            await notify.removeSlot(notify.SlotType.UNKNOWN_TYPE);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_1000 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume1100(data){
        console.info("=============Ans_PublishVibra_1100 onConsume start============>");
        console.info("=============Ans_PublishVibra_1100 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_1100 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_1100 onConsume vibrationEnabled============>" + vibrationEnabled);
        expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([200]));
        expect(vibrationEnabled).assertEqual(true);
        console.info("=============Ans_PublishVibra_1100 onConsume timesOfOnConsume1============>");
        console.info("=============Ans_PublishVibra_1100 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_1100
     * @tc.name: addSlot(slot: NotificationSlot): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a SOCIAL_COMMUNICATION type slot, set vibra flag, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_1100', 0,async function (done) {
        console.info("===============Ans_PublishVibra_1100================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume1100
        }
        notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_1100 subscribe======>");
        notify.addSlot({
            type:notify.SlotType.SOCIAL_COMMUNICATION,
            vibrationEnabled:true,
        }).then(()=>{
            console.info("===============Ans_PublishVibra_1100 addSlot then======>");
        }).catch((err)=>{
            console.info("===============Ans_PublishVibra_1100 addSlot catch err======>"+err);
        });
        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 11,
            slotType : notify.SlotType.SOCIAL_COMMUNICATION,
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
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_1100 publish then======>");
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_1100 publish then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_1100 setTimeout========>");
            await notify.removeSlot(notify.SlotType.SOCIAL_COMMUNICATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_1100 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });

    function onConsume1200(data){
        console.info("=============Ans_PublishVibra_1200 onConsume start============>");
        console.info("=============Ans_PublishVibra_1200 onConsume data:============>" + JSON.stringify(data));
        var hashCode = data.request.hashCode
        var vibrationValues = data.sortingMap.sortings[hashCode].slot.vibrationValues
        var vibrationEnabled = data.sortingMap.sortings[hashCode].slot.vibrationEnabled
        console.info("=============Ans_PublishVibra_1200 onConsume vibrationValues============>" + vibrationValues);
        console.info("=============Ans_PublishVibra_1200 onConsume vibrationEnabled============>" + vibrationEnabled);
        expect(JSON.stringify(vibrationValues)).assertEqual(JSON.stringify([]));
        expect(vibrationEnabled).assertEqual(true);
        console.info("=============Ans_PublishVibra_1200 onConsume end==============>");
    }

    /*
     * @tc.number: Ans_PublishVibra_1200
     * @tc.name: addSlot(slot: NotificationSlot): Promise<void>;
                 setSlotByBundle(bundle: BundleOption, slot: NotificationSlot): Promise<void>;
     * @tc.desc: Create a CONTENT_INFORMATION type slot, set vibra flag, publish a notification and verify.
                 After changing the slot information, publish a notification again and verify.
     */
    it('Ans_PublishVibra_1200', 0,async function (done) {
        console.info("===============Ans_PublishVibra_1200================>");
        timesOfOnConsume = 0;
        var subscriber ={
            onConsume:onConsume1200
        }
        await notify.subscribe(subscriber);
        console.info("===============Ans_PublishVibra_1200 subscribe======>");
        await notify.addSlot({
            type:notify.SlotType.CONTENT_INFORMATION,
            vibrationEnabled:true,
        })

        var notificationRequest = {
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 12,
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
            label: "1200",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        }
        notify.publish(notificationRequest).then(()=>{
            console.info("=======Ans_PublishVibra_1200 publish then======>");
        }).catch((err)=>{
            console.info("=======Ans_PublishVibra_1200 publish then catch err======>"+err);
        })
        setTimeout((async function(){
            console.info("======Ans_PublishVibra_1200 setTimeout========>");
            await notify.removeSlot(notify.SlotType.CONTENT_INFORMATION);
            await notify.unsubscribe(subscriber);
            console.info("======Ans_PublishVibra_1200 setTimeout unsubscribe===>");
            done();
        }),timeout);
    });
}) 