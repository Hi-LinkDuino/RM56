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
import image from '@ohos.multimedia.image'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var largeIconA
var smallIconA
var largeBufferA
var smallBufferA
var largeOptsA
var smallOptsA

var largeIconB
var smallIconB
var largeBufferB
var smallBufferB
var largeOptsB
var smallOptsB

var largeIconC
var smallIconC
var largeBufferC
var smallBufferC
var largeOptsC
var smallOptsC

var largeIconD
var smallIconD
var largeBufferD
var smallBufferD
var largeOptsD
var smallOptsD

describe('ActsAnsPublishIconTest', function () {
    function publishCallbackA(err){
        console.log('ActsAnsPublishIconTest publishCallbackA asyncCallback'+err.code)
        expect(err.code).assertEqual(0)
    }
    function publishCallbackB(err){
        console.log('ActsAnsPublishIconTest publishCallbackB asyncCallback'+err.code)
        expect(err.code != 0).assertEqual(true);
    }
    function consumeCallbackA(data) {
        console.debug("===consumeCallbackA data : ===>" + JSON.stringify(data));
        expect(data.request.id).assertEqual(1)
    }
    function consumeCallbackB(data) {
        console.debug("===consumeCallbackB data : ===>" + JSON.stringify(data));
        expect(data.request.id).assertEqual(2)
    }

    function subscribeCallbackA(err) {
        console.info("===subscribeCallbackA err : ===>" + JSON.stringify(err));
        expect(err.code).assertEqual(0)
    }
    function subscribeCallbackB(err) {
        console.info("===subscribeCallbackB err : ===>" + JSON.stringify(err));
        expect(err.code).assertEqual(0)
    }

    /*
    * @tc.number: ACTS_PublishIconTest_0100
    * @tc.name: publish()
    * @tc.desc: verify the function of publish
    */
    it('ACTS_PublishIconTest_0100', 0,async function (done) {
        var subInfo ={
            onConsume:consumeCallbackA,
        }
        await notification.subscribe(subInfo,subscribeCallbackA);
        largeBufferA = new ArrayBuffer(4*100*100);
        smallBufferA = new ArrayBuffer(4*2*25);
        largeOptsA = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 100, width: 100}}
        smallOptsA = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 25, width: 2}}
        const promise_Large = image.createPixelMap(largeBufferA, largeOptsA);
        promise_Large.then((data) => {
            largeIconA = data;
            console.debug("===createPixelMapA largeIcon===>"+JSON.stringify(largeIconA));
        const promise_Small = image.createPixelMap(smallBufferA, smallOptsA);
        promise_Small.then((data) => {
            smallIconA = data;
            console.debug("===createPixelMapA smallIcon===>"+JSON.stringify(smallIconA));

         notification.publish({
            content:{
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_titleA",
                    text: "test_textA",
                    additionalText: "test_additionalTextA"
                },
            },
                id: 1,
                slotType : notification.SlotType.OTHER_TYPES,
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
                label: "0100_1",
                badgeIconStyle: 1,
                showDeliveryTime: true,
                smallIcon:smallIconA,
                largeIcon:largeIconA,
            },publishCallbackA);
            setTimeout((async function(){
                notification.unsubscribe(subInfo);
                console.info("======ActsPublishImage_test_0100 setTimeout unsubscribe===>");
                done();
            }),300);
         })
       });
    });

    /*
    * @tc.number: ACTS_PublishTest_0200
    * @tc.name: publish()
    * @tc.desc: verify the function of publish
    */
    it('ACTS_PublishIconTest_0200', 0,async function (done) {
        var subInfo ={
            onConsume:consumeCallbackB,
        }
        await notification.subscribe(subInfo,subscribeCallbackB);
        largeBufferB = new ArrayBuffer(4*200*50);
        smallBufferB = new ArrayBuffer(4*2*25);
        largeOptsB = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 50, width: 200}}
        smallOptsB = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 25, width: 2}}
        const promise_Large = image.createPixelMap(largeBufferB, largeOptsB);
        promise_Large.then((data) => {
            largeIconB = data;
            console.debug("===createPixelMapB largeIcon===>"+JSON.stringify(largeIconB));
            const promise_Small = image.createPixelMap(smallBufferB, smallOptsB);
            promise_Small.then((data) => {
                smallIconB = data;
                console.debug("===createPixelMapB smallIcon===>"+JSON.stringify(smallIconB));

                notification.publish({
                    content:{
                        contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                        normal: {
                            title: "test_titleB",
                            text: "test_textB",
                            additionalText: "test_additionalTextB"
                        },
                    },
                    id: 2,
                    slotType : notification.SlotType.OTHER_TYPES,
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
                    label: "0100_1",
                    badgeIconStyle: 1,
                    showDeliveryTime: true,
                    smallIcon:smallIconB,
                    largeIcon:largeIconB,
                }).then(()=>{
                    console.debug("===ACTS_PublishIconTest_0200 promise===>");
                });
                setTimeout((async function(){
                    notification.unsubscribe(subInfo);
                    console.info("======ActsPublishImage_test_0200 setTimeout unsubscribe===>");
                    done();
                }),300);
            })
        });
    });

    /*
    * @tc.number: ACTS_PublishIconTest_0300
    * @tc.name: publish()
    * @tc.desc: verify the function of publish
    */
    it('ACTS_PublishIconTest_0300', 0,async function (done) {
        largeBufferC = new ArrayBuffer(4*1024*50);
        smallBufferC = new ArrayBuffer(4*2*25);
        largeOptsC = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 50, width: 1024}}
        smallOptsC = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 25, width: 2}}
        const promise_Large = image.createPixelMap(largeBufferC, largeOptsC);
        promise_Large.then((data) => {
            largeIconC = data;
            console.debug("===createPixelMapC largeIcon===>"+JSON.stringify(largeIconC));
            const promise_Small = image.createPixelMap(smallBufferC, smallOptsC);
            promise_Small.then((data) => {
                smallIconC = data;
                console.debug("===createPixelMapC smallIcon===>"+JSON.stringify(smallIconC));

                notification.publish({
                    content:{
                        contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                        normal: {
                            title: "test_titleC",
                            text: "test_textC",
                            additionalText: "test_additionalTextC"
                        },
                    },
                    id: 3,
                    slotType : notification.SlotType.OTHER_TYPES,
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
                    label: "0100_1",
                    badgeIconStyle: 1,
                    showDeliveryTime: true,
                    smallIcon:smallIconC,
                    largeIcon:largeIconC,
                },publishCallbackB);
                done();
            })
        });
    });

    /*
    * @tc.number: ACTS_PublishTest_0400
    * @tc.name: publish()
    * @tc.desc: verify the function of publish
    */
    it('ACTS_PublishIconTest_0400', 0,async function (done) {
        largeBufferD = new ArrayBuffer(4*1024*50);
        smallBufferD = new ArrayBuffer(4*2*25);
        largeOptsD = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 50, width: 1024}}
        smallOptsD = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 25, width: 2}}
        const promise_Large = image.createPixelMap(largeBufferD, largeOptsD);
        promise_Large.then((data) => {
            largeIconD = data;
            console.debug("===createPixelMapD largeIcon===>"+JSON.stringify(largeIconD));
            const promise_Small = image.createPixelMap(smallBufferD, smallOptsD);
            promise_Small.then((data) => {
                smallIconD = data;
                console.debug("===createPixelMapD smallIcon===>"+JSON.stringify(smallIconD));

                notification.publish({
                    content:{
                        contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                        normal: {
                            title: "test1_title",
                            text: "test1_text",
                            additionalText: "test1_additionalText"
                        },
                    },
                    id: 4,
                    slotType : notification.SlotType.OTHER_TYPES,
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
                    label: "0100_1",
                    badgeIconStyle: 1,
                    showDeliveryTime: true,
                    smallIcon:smallIconD,
                    largeIcon:largeIconD,
                }).then().catch((err)=>{
                    console.debug("===ACTS_PublishIconTest_0400 promise===>"+err.code);
                    expect(err.code != 0).assertEqual(true);
                    done()
                });
            })
        });
    });
}) 
