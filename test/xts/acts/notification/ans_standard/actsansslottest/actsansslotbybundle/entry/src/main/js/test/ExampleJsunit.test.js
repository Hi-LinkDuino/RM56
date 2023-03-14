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

describe('ActsAnsSlotByBundle', function () {
    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0100
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding slottype to SERVICE_INFORMATION's slot,
     *                 call getsbybundle for information.(promise)
     */
    it('ActsAnsGetSlotsByBundle_0100', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0100 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0100====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(2)
            expect(data[0].level).assertEqual(3)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot SERVICE_INFORMATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0200
     * @tc.name      : Verify getSlot UNKNOWN_TYPE
     * @tc.desc      : After adding slottype to UNKNOWN_TYPE's slot,
     *                 call getsbybundle for information.(promise)
     */
    it('ActsAnsGetSlotsByBundle_0200', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0200 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise UNKNOWN_TYPE enter====>");
        await notification.addSlot(notification.SlotType.UNKNOWN_TYPE);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0200====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(1)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.UNKNOWN_TYPE, (err)=>{
                console.debug("====>removeSlot UNKNOWN_TYPE====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0300
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to SOCIAL_COMMUNICATION's slot,
     *                 call getsbybundle for information.(promise)
     */
    it('ActsAnsGetSlotsByBundle_0300', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0300 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise SOCIAL_COMMUNICATION enter====>");
        await notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0300====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(1)
            expect(data[0].level).assertEqual(4)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                console.debug("====>removeSlot SOCIAL_COMMUNICATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })

    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0400
     * @tc.name      : Verify getSlot CONTENT_INFORMATION
     * @tc.desc      : After adding slottype to CONTENT_INFORMATION's slot,
     *                 call getsbybundle for information.(promise)
     */
    it('ActsAnsGetSlotsByBundle_0400', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0400 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.CONTENT_INFORMATION);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0400====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(3)
            expect(data[0].level).assertEqual(2)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>ActsAnsGetSlotsByBundle_0400 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                console.debug("====>removeSlot CONTENT_INFORMATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })

    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0500
     * @tc.name      : Verify getSlot OTHER_TYPES
     * @tc.desc      : After adding slottype to OTHER_TYPES's slot,
     *                 call getsbybundle for information.(promise)
     */
    it('ActsAnsGetSlotsByBundle_0500', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0500 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise OTHER_TYPES enter====>");
        await notification.addSlot(notification.SlotType.OTHER_TYPES);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0500====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(1)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                console.debug("====>removeSlot OTHER_TYPES====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })

    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0600
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding slottype to SERVICE_INFORMATION's slot,
     *                 call getsbybundle for information.(callback)
     */
    it('ActsAnsGetSlotsByBundle_0600', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0600 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption, (err, data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0600====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(2)
            expect(data[0].level).assertEqual(3)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot SERVICE_INFORMATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0700
     * @tc.name      : Verify getSlot UNKNOWN_TYPE
     * @tc.desc      : After adding slottype to UNKNOWN_TYPE's slot,
     *                 call getsbybundle for information.(callback)
     */
    it('ActsAnsGetSlotsByBundle_0700', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0700 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise UNKNOWN_TYPE enter====>");
        await notification.addSlot(notification.SlotType.UNKNOWN_TYPE);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption, (err, data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0700====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(1)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.UNKNOWN_TYPE, (err)=>{
                console.debug("====>removeSlot UNKNOWN_TYPE====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0800
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to SOCIAL_COMMUNICATION's slot,
     *                 call getsbybundle for information.(callback)
     */
    it('ActsAnsGetSlotsByBundle_0800', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0800 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise SOCIAL_COMMUNICATION enter====>");
        await notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption, (err, data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0800====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(1)
            expect(data[0].level).assertEqual(4)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                console.debug("====>removeSlot SOCIAL_COMMUNICATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_0900
     * @tc.name      : Verify getSlot CONTENT_INFORMATION
     * @tc.desc      : After adding slottype to CONTENT_INFORMATION's slot,
     *                 call getsbybundle for information.(callback)
     */
    it('ActsAnsGetSlotsByBundle_0900', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_0900 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.CONTENT_INFORMATION);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption, (err, data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_0900====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(3)
            expect(data[0].level).assertEqual(2)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                console.debug("====>removeSlot CONTENT_INFORMATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotsByBundle_1000
     * @tc.name      : Verify getSlot OTHER_TYPES
     * @tc.desc      : After adding slottype to OTHER_TYPES's slot,
     *                 call getsbybundle for information.(callback)
     */
    it('ActsAnsGetSlotsByBundle_1000', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotsByBundle_1000 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise OTHER_TYPES enter====>");
        await notification.addSlot(notification.SlotType.OTHER_TYPES);
        console.debug("====>getSlotsByBundle1 start====>");
        notification.getSlotsByBundle(bundleoption, (err, data) => {
            console.debug("====>ActsAnsGetSlotsByBundle_1000====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(1)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                console.debug("====>removeSlot OTHER_TYPES====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0100
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding slottype to SERVICE_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(promise)
     */
    it('ActsAnsSetSlotByBundle_0100', 0, async function (done) {
        console.debug("====>ActsAnsSetSlotByBundle_0100 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.SERVICE_INFORMATION,
            level: 4
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>getSlotsByBundle1 start====>");
        var data = await notification.getSlotsByBundle(bundleoption)
            console.debug("====>ActsAnsSetSlotByBundle_0100====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(2)
            expect(data[0].level).assertEqual(3)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
        console.debug("====>setSlotsByBundle====>")
        await notification.setSlotByBundle(bundleoption, notificationslot);
        console.debug("====>getSlotsByBundle1.1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsSetSlotByBundle_0100.1====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(2)
            expect(data[0].level).assertEqual(4)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1.1 finish====>");
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot SERVICE_INFORMATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0200
     * @tc.name      : Verify getSlot UNKNOWN_TYPE
     * @tc.desc      : After adding slottype to UNKNOWN_TYPE's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(promise)
     */
    it('ActsAnsSetSlotByBundle_0200', 0, async function (done) {
        console.debug("====>ActsAnsSetSlotByBundle_0200 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.UNKNOWN_TYPE,
            level: 4
        }
        console.debug("====>addSlotByTypePromise UNKNOWN_TYPE enter====>");
        await notification.addSlot(notification.SlotType.UNKNOWN_TYPE);
        console.debug("====>getSlotsByBundle1 start====>");
        var data = await notification.getSlotsByBundle(bundleoption)
            console.debug("====>ActsAnsSetSlotByBundle_0200====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(1)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
        console.debug("====>setSlotsByBundle====>")
        await notification.setSlotByBundle(bundleoption, notificationslot);
        console.debug("====>getSlotsByBundle1.1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsSetSlotByBundle_0200.1====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(4)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1.1 finish====>");
            notification.removeSlot(notification.SlotType.UNKNOWN_TYPE, (err)=>{
                console.debug("====>removeSlot UNKNOWN_TYPE====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0300
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to SOCIAL_COMMUNICATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(promise)
     */
    it('ActsAnsSetSlotByBundle_0300', 0, async function (done) {
        console.debug("====>ActsAnsSetSlotByBundle_0300 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.SOCIAL_COMMUNICATION,
            level: 3
        }
        console.debug("====>addSlotByTypePromise SOCIAL_COMMUNICATION enter====>");
        await notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION);
        console.debug("====>getSlotsByBundle1 start====>");
        var data = await notification.getSlotsByBundle(bundleoption)
            console.debug("====>ActsAnsSetSlotByBundle_0300====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(1)
            expect(data[0].level).assertEqual(4)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
        await notification.setSlotByBundle(bundleoption, notificationslot);
        console.debug("====>getSlotsByBundle1.1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsSetSlotByBundle_0300.1====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(1)
            expect(data[0].level).assertEqual(3)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(2)
            expect(data[0].vibrationEnabled).assertEqual(true)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1.1 finish====>");
            notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                console.debug("====>removeSlot SOCIAL_COMMUNICATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0400
     * @tc.name      : Verify getSlot CONTENT_INFORMATION
     * @tc.desc      : After adding slottype to CONTENT_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(promise)
     */
    it('ActsAnsSetSlotByBundle_0400', 0, async function (done) {
        console.debug("====>ActsAnsSetSlotByBundle_0400 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.CONTENT_INFORMATION,
            level: 4
        }
        console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.CONTENT_INFORMATION);
        console.debug("====>getSlotsByBundle1 start====>");
        var data = await notification.getSlotsByBundle(bundleoption)
            console.debug("====>ActsAnsSetSlotByBundle_0400====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(3)
            expect(data[0].level).assertEqual(2)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
        console.debug("====>setSlotsByBundle====>")
        await notification.setSlotByBundle(bundleoption, notificationslot);
        console.debug("====>getSlotsByBundle1.1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsSetSlotByBundle_0400.1====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(3)
            expect(data[0].level).assertEqual(4)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1.1 finish====>");
            notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                console.debug("====>removeSlot CONTENT_INFORMATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0500
     * @tc.name      : Verify getSlot OTHER_TYPES
     * @tc.desc      : After adding slottype to OTHER_TYPES's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(promise)
     */
    it('ActsAnsSetSlotByBundle_0500', 0, async function (done) {
        console.debug("====>ActsAnsSetSlotByBundle_0500 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.OTHER_TYPES,
            level: 4
        }
        console.debug("====>addSlotByTypePromise OTHER_TYPES enter====>");
        await notification.addSlot(notification.SlotType.OTHER_TYPES);
        console.debug("====>getSlotsByBundle1 start====>");
        var data = await notification.getSlotsByBundle(bundleoption)
            console.debug("====>ActsAnsSetSlotByBundle_0500====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(1)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1 finish====>");
            console.debug("====>setSlotByBundle start====>");
        console.debug("====>setSlotsByBundle====>")
        await notification.setSlotByBundle(bundleoption, notificationslot);
        console.debug("====>getSlotsByBundle1.1 start====>");
        notification.getSlotsByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsSetSlotByBundle_0500.1====>" + JSON.stringify(data));
            expect(data[0].type).assertEqual(65535)
            expect(data[0].level).assertEqual(4)
            expect(data[0].badgeFlag).assertEqual(true)
            expect(data[0].bypassDnd).assertEqual(false)
            expect(data[0].lockscreenVisibility).assertEqual(3)
            expect(data[0].vibrationEnabled).assertEqual(false)
            expect(data[0].lightEnabled).assertEqual(false)
            expect(data[0].lightColor).assertEqual(0)
            console.debug("====>getSlotsByBundle1.1 finish====>");
            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                console.debug("====>removeSlot OTHER_TYPES====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0600
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding slottype to SERVICE_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(callback)
     */
    it('ActsAnsSetSlotByBundle_0600', 0, async function (done) {
        console.debug("====>ActsAnsSetSlotByBundle_0600 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.SERVICE_INFORMATION,
            level: 4
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        notification.addSlot(notification.SlotType.SERVICE_INFORMATION,()=>{
            console.debug("====>getSlotsByBundle1 start====>");
            notification.getSlotsByBundle(bundleoption,(err,data) => {
                console.debug("====>ActsAnsSetSlotByBundle_0600====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(2)
                expect(data[0].level).assertEqual(3)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(2)
                expect(data[0].vibrationEnabled).assertEqual(true)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
                notification.setSlotByBundle(bundleoption, notificationslot, () => {
                    console.debug("====>setSlotsByBundle====>")
                    console.debug("====>getSlotsByBundle1.1 start====>");
                    notification.getSlotsByBundle(bundleoption,(err,data) => {
                        console.debug("====>ActsAnsSetSlotByBundle_0600.1====>" + JSON.stringify(data));
                        expect(data[0].type).assertEqual(2)
                        expect(data[0].level).assertEqual(4)
                        expect(data[0].badgeFlag).assertEqual(true)
                        expect(data[0].bypassDnd).assertEqual(false)
                        expect(data[0].lockscreenVisibility).assertEqual(2)
                        expect(data[0].vibrationEnabled).assertEqual(true)
                        expect(data[0].lightEnabled).assertEqual(false)
                        expect(data[0].lightColor).assertEqual(0)
                        console.debug("====>getSlotsByBundle1.1 finish====>");
                        notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                            console.debug("====>removeSlot SERVICE_INFORMATION====>");
                            expect(err.code).assertEqual(0);
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0700
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to UNKNOWN_TYPE's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(callback)
     */
    it('ActsAnsSetSlotByBundle_0700', 0, async function (done) {
        console.debug("====>ActsAnsSlotByBundle_0100 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.UNKNOWN_TYPE,
            level: 4
        }
        console.debug("====>addSlotByTypePromise UNKNOWN_TYPE enter====>");
        notification.addSlot(notification.SlotType.UNKNOWN_TYPE,() => {
            console.debug("====>getSlotsByBundle1 start====>");
            notification.getSlotsByBundle(bundleoption,(err,data) => {
                console.debug("====>ActsAnsSetSlotByBundle_0700====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(65535)
                expect(data[0].level).assertEqual(1)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(3)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
                notification.setSlotByBundle(bundleoption, notificationslot, () => {
                    console.debug("====>setSlotsByBundle====>")
                    console.debug("====>getSlotsByBundle1.1 start====>");
                    notification.getSlotsByBundle(bundleoption,(err,data) => {
                        console.debug("====>ActsAnsSetSlotByBundle_0700.1====>" + JSON.stringify(data));
                        expect(data[0].type).assertEqual(65535)
                        expect(data[0].level).assertEqual(4)
                        expect(data[0].badgeFlag).assertEqual(true)
                        expect(data[0].bypassDnd).assertEqual(false)
                        expect(data[0].lockscreenVisibility).assertEqual(3)
                        expect(data[0].vibrationEnabled).assertEqual(false)
                        expect(data[0].lightEnabled).assertEqual(false)
                        expect(data[0].lightColor).assertEqual(0)
                        console.debug("====>getSlotsByBundle1.1 finish====>");
                        notification.removeSlot(notification.SlotType.UNKNOWN_TYPE, (err)=>{
                            console.debug("====>removeSlot UNKNOWN_TYPE====>");
                            expect(err.code).assertEqual(0);
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0800
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to SOCIAL_COMMUNICATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(callback)
     */
    it('ActsAnsSetSlotByBundle_0800', 0, async function (done) {
        console.debug("====>ActsAnsSlotByBundle_0100 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.SOCIAL_COMMUNICATION,
            level: 3
        }
        console.debug("====>addSlotByTypePromise SOCIAL_COMMUNICATION enter====>");
        notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION,() => {
            console.debug("====>getSlotsByBundle1 start====>");
            notification.getSlotsByBundle(bundleoption,(err,data) => {
                console.debug("====>ActsAnsSetSlotByBundle_0800====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(1)
                expect(data[0].level).assertEqual(4)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(2)
                expect(data[0].vibrationEnabled).assertEqual(true)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
                notification.setSlotByBundle(bundleoption, notificationslot, () => {
                    console.debug("====>setSlotsByBundle====>")
                    console.debug("====>getSlotsByBundle1.1 start====>");
                    notification.getSlotsByBundle(bundleoption,(err,data) => {
                        console.debug("====>ActsAnsSetSlotByBundle_0800.1====>" + JSON.stringify(data));
                        expect(data[0].type).assertEqual(1)
                        expect(data[0].level).assertEqual(3)
                        expect(data[0].badgeFlag).assertEqual(true)
                        expect(data[0].bypassDnd).assertEqual(false)
                        expect(data[0].lockscreenVisibility).assertEqual(2)
                        expect(data[0].vibrationEnabled).assertEqual(true)
                        expect(data[0].lightEnabled).assertEqual(false)
                        expect(data[0].lightColor).assertEqual(0)
                        console.debug("====>getSlotsByBundle1.1 finish====>");
                        notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                            console.debug("====>removeSlot SOCIAL_COMMUNICATION====>");
                            expect(err.code).assertEqual(0);
                            done();
                        })
                    })
                })
            })
        })
    })

    /*
     * @tc.number    : ActsAnsSetSlotByBundle_0900
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to CONTENT_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(callback)
     */
    it('ActsAnsSetSlotByBundle_0900', 0, async function (done) {
        console.debug("====>ActsAnsSetSlotByBundle_0900 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        var notificationslot = {
            type: notification.SlotType.CONTENT_INFORMATION,
            level: 4
        }
        console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
        notification.addSlot(notification.SlotType.CONTENT_INFORMATION,() => {
            console.debug("====>getSlotsByBundle1 start====>");
            notification.getSlotsByBundle(bundleoption,(err,data) => {
                console.debug("====>ActsAnsSetSlotByBundle_0900====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(3)
                expect(data[0].level).assertEqual(2)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(3)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");

                notification.setSlotByBundle(bundleoption, notificationslot, () => {
                    console.debug("====>setSlotsByBundle====>")
                    console.debug("====>getSlotsByBundle1.1 start====>");
                    notification.getSlotsByBundle(bundleoption,(err,data) => {
                        console.debug("====>ActsAnsSetSlotByBundle_0900.1====>" + JSON.stringify(data));
                        expect(data[0].type).assertEqual(3)
                        expect(data[0].level).assertEqual(4)
                        expect(data[0].badgeFlag).assertEqual(true)
                        expect(data[0].bypassDnd).assertEqual(false)
                        expect(data[0].lockscreenVisibility).assertEqual(3)
                        expect(data[0].vibrationEnabled).assertEqual(false)
                        expect(data[0].lightEnabled).assertEqual(false)
                        expect(data[0].lightColor).assertEqual(0)
                        console.debug("====>getSlotsByBundle1.1 finish====>");
                        notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                            console.debug("====>removeSlot CONTENT_INFORMATION====>");
                            expect(err.code).assertEqual(0);
                            done();
                        })
                    })
                })
            })
        })
    })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1000
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to OTHER_TYPES's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 to update the information.(callback)
     */
        it('ActsAnsSetSlotByBundle_1000', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1000 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot = {
                type: notification.SlotType.OTHER_TYPES,
                level: 4
            }
            console.debug("====>addSlotByTypePromise OTHER_TYPES enter====>");
            await notification.addSlot(notification.SlotType.OTHER_TYPES,() => {
                console.debug("====>getSlotsByBundle1 start====>");
                notification.getSlotsByBundle(bundleoption,(err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_1000====>" + JSON.stringify(data));
                    expect(data[0].type).assertEqual(65535)
                    expect(data[0].level).assertEqual(1)
                    expect(data[0].badgeFlag).assertEqual(true)
                    expect(data[0].bypassDnd).assertEqual(false)
                    expect(data[0].lockscreenVisibility).assertEqual(3)
                    expect(data[0].vibrationEnabled).assertEqual(false)
                    expect(data[0].lightEnabled).assertEqual(false)
                    expect(data[0].lightColor).assertEqual(0)
                    console.debug("====>getSlotsByBundle1 finish====>");
                    console.debug("====>setSlotByBundle start====>");
                    notification.setSlotByBundle(bundleoption, notificationslot, () => {
                        console.debug("====>setSlotsByBundle====>")
                        console.debug("====>getSlotsByBundle1.1 start====>");
                        notification.getSlotsByBundle(bundleoption,(err,data) => {
                            console.debug("====>ActsAnsSetSlotByBundle_1000.1====>" + JSON.stringify(data));
                            expect(data[0].type).assertEqual(65535)
                            expect(data[0].level).assertEqual(4)
                            expect(data[0].badgeFlag).assertEqual(true)
                            expect(data[0].bypassDnd).assertEqual(false)
                            expect(data[0].lockscreenVisibility).assertEqual(3)
                            expect(data[0].vibrationEnabled).assertEqual(false)
                            expect(data[0].lightEnabled).assertEqual(false)
                            expect(data[0].lightColor).assertEqual(0)
                            console.debug("====>getSlotsByBundle1.1 finish====>");
                            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                                console.debug("====>removeSlot OTHER_TYPES====>");
                                expect(err.code).assertEqual(0);
                                done();
                            })
                        })
                    })
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1100
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding slottype to SERVICE_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(promise)
     */
        it('ActsAnsSetSlotByBundle_1100', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1100 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.SERVICE_INFORMATION,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.SERVICE_INFORMATION,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
            await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
            console.debug("====>getSlotsByBundle1 start====>");
            var data = await notification.getSlotsByBundle(bundleoption)
                console.debug("====>ActsAnsSetSlotByBundle_1100====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(2)
                expect(data[0].level).assertEqual(3)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(2)
                expect(data[0].vibrationEnabled).assertEqual(true)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
            console.debug("====>setSlotsByBundle1====>")
            await notification.setSlotByBundle(bundleoption, notificationslot1);
            console.debug("====>setSlotsByBundle2====>")
            await notification.setSlotByBundle(bundleoption, notificationslot2);
            console.debug("====>getSlotsByBundle1.1 start====>");
            notification.getSlotsByBundle(bundleoption).then((data) => {
                console.debug("====>ActsAnsSetSlotByBundle_1100.1====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(2)
                expect(data[0].level).assertEqual(3)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(1)
                expect(data[0].vibrationEnabled).assertEqual(true)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1.1 finish====>");
                notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                    console.debug("====>removeSlot SERVICE_INFORMATION====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1200
     * @tc.name      : Verify getSlot UNKNOWN_TYPE
     * @tc.desc      : After adding slottype to UNKNOWN_TYPE's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(promise)
     */
        it('ActsAnsSetSlotByBundle_1200', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1200 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.UNKNOWN_TYPE,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.UNKNOWN_TYPE,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise UNKNOWN_TYPE enter====>");
            await notification.addSlot(notification.SlotType.UNKNOWN_TYPE);
            console.debug("====>getSlotsByBundle1 start====>");
            var data = await notification.getSlotsByBundle(bundleoption)
                console.debug("====>ActsAnsSetSlotByBundle_1200====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(65535)
                expect(data[0].level).assertEqual(1)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(3)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
            console.debug("====>setSlotsByBundle1====>")
            await notification.setSlotByBundle(bundleoption, notificationslot1);
            console.debug("====>setSlotsByBundle2====>")
            await notification.setSlotByBundle(bundleoption, notificationslot2);
            console.debug("====>getSlotsByBundle1.1 start====>");
            notification.getSlotsByBundle(bundleoption).then((data) => {
                console.debug("====>ActsAnsSetSlotByBundle_1200.1====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(65535)
                expect(data[0].level).assertEqual(1)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(1)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1.1 finish====>");
                notification.removeSlot(notification.SlotType.UNKNOWN_TYPE, (err)=>{
                    console.debug("====>removeSlot UNKNOWN_TYPE====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1300
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to SOCIAL_COMMUNICATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(promise)
     */
        it('ActsAnsSetSlotByBundle_1300', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1300 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise SOCIAL_COMMUNICATION enter====>");
            await notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION);
            console.debug("====>getSlotsByBundle1 start====>");
            var data = await notification.getSlotsByBundle(bundleoption)
                console.debug("====>ActsAnsSetSlotByBundle_1300====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(1)
                expect(data[0].level).assertEqual(4)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(2)
                expect(data[0].vibrationEnabled).assertEqual(true)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
            console.debug("====>setSlotsByBundle1====>")
            await notification.setSlotByBundle(bundleoption, notificationslot1);
            console.debug("====>setSlotsByBundle2====>")
            await notification.setSlotByBundle(bundleoption, notificationslot2);
            console.debug("====>getSlotsByBundle1.1 start====>");
            notification.getSlotsByBundle(bundleoption).then((data) => {
                console.debug("====>ActsAnsSetSlotByBundle_1300.1====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(1)
                expect(data[0].level).assertEqual(4)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(1)
                expect(data[0].vibrationEnabled).assertEqual(true)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1.1 finish====>");
                notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                    console.debug("====>removeSlot SOCIAL_COMMUNICATION====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1400
     * @tc.name      : Verify getSlot CONTENT_INFORMATION
     * @tc.desc      : After adding slottype to CONTENT_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(promise)
     */
        it('ActsAnsSetSlotByBundle_1400', 0, async function (done) {
            console.debug("====>ActsAnsSlotByBundle_0100 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.CONTENT_INFORMATION,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.CONTENT_INFORMATION,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
            await notification.addSlot(notification.SlotType.CONTENT_INFORMATION);
            console.debug("====>getSlotsByBundle1 start====>");
            var data = await notification.getSlotsByBundle(bundleoption)
                console.debug("====>ActsAnsSetSlotByBundle_1400====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(3)
                expect(data[0].level).assertEqual(2)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(3)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
            console.debug("====>setSlotsByBundle1====>")
            await notification.setSlotByBundle(bundleoption, notificationslot1);
            console.debug("====>setSlotsByBundle2====>")
            await notification.setSlotByBundle(bundleoption, notificationslot2);
            console.debug("====>getSlotsByBundle1.1 start====>");
            notification.getSlotsByBundle(bundleoption).then((data) => {
                console.debug("====>ActsAnsSetSlotByBundle_1400.1====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(3)
                expect(data[0].level).assertEqual(2)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(1)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1.1 finish====>");
                notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                    console.debug("====>removeSlot CONTENT_INFORMATION====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1500
     * @tc.name      : Verify getSlot OTHER_TYPES
     * @tc.desc      : After adding slottype to OTHER_TYPES's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(promise)
     */
        it('ActsAnsSetSlotByBundle_1500', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1500 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.OTHER_TYPES,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.OTHER_TYPES,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise OTHER_TYPES enter====>");
            await notification.addSlot(notification.SlotType.OTHER_TYPES);
            console.debug("====>getSlotsByBundle1 start====>");
            var data = await notification.getSlotsByBundle(bundleoption)
                console.debug("====>ActsAnsSetSlotByBundle_1500====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(65535)
                expect(data[0].level).assertEqual(1)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(3)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1 finish====>");
                console.debug("====>setSlotByBundle start====>");
            console.debug("====>setSlotsByBundle1====>")
            await notification.setSlotByBundle(bundleoption, notificationslot1);
            console.debug("====>setSlotsByBundle2====>")
            await notification.setSlotByBundle(bundleoption, notificationslot2);
            console.debug("====>getSlotsByBundle1.1 start====>");
            notification.getSlotsByBundle(bundleoption).then((data) => {
                console.debug("====>ActsAnsSetSlotByBundle_1500.1====>" + JSON.stringify(data));
                expect(data[0].type).assertEqual(65535)
                expect(data[0].level).assertEqual(1)
                expect(data[0].badgeFlag).assertEqual(true)
                expect(data[0].bypassDnd).assertEqual(false)
                expect(data[0].lockscreenVisibility).assertEqual(1)
                expect(data[0].vibrationEnabled).assertEqual(false)
                expect(data[0].lightEnabled).assertEqual(false)
                expect(data[0].lightColor).assertEqual(0)
                console.debug("====>getSlotsByBundle1.1 finish====>");
                notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                    console.debug("====>removeSlot OTHER_TYPES====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1600
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding slottype to SERVICE_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(callback)
     */
        it('ActsAnsSetSlotByBundle_1600', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1600 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.SERVICE_INFORMATION,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.SERVICE_INFORMATION,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
            notification.addSlot(notification.SlotType.SERVICE_INFORMATION,() => {
                console.debug("====>getSlotsByBundle1 start====>");
                notification.getSlotsByBundle(bundleoption,(err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_1600====>" + JSON.stringify(data));
                    expect(data[0].type).assertEqual(2)
                    expect(data[0].level).assertEqual(3)
                    expect(data[0].badgeFlag).assertEqual(true)
                    expect(data[0].bypassDnd).assertEqual(false)
                    expect(data[0].lockscreenVisibility).assertEqual(2)
                    expect(data[0].vibrationEnabled).assertEqual(true)
                    expect(data[0].lightEnabled).assertEqual(false)
                    expect(data[0].lightColor).assertEqual(0)
                    console.debug("====>getSlotsByBundle1 finish====>");
                    console.debug("====>setSlotByBundle start====>");
                    notification.setSlotByBundle(bundleoption, notificationslot1, () => {
                        console.debug("====>setSlotsByBundle1====>")
                        notification.setSlotByBundle(bundleoption, notificationslot2, () => {
                            console.debug("====>setSlotsByBundle2====>")
                            console.debug("====>getSlotsByBundle1.1 start====>");
                            notification.getSlotsByBundle(bundleoption,(err,data) => {
                                console.debug("====>ActsAnsSetSlotByBundle_1600.1====>" + JSON.stringify(data));
                                expect(data[0].type).assertEqual(2)
                                expect(data[0].level).assertEqual(3)
                                expect(data[0].badgeFlag).assertEqual(true)
                                expect(data[0].bypassDnd).assertEqual(false)
                                expect(data[0].lockscreenVisibility).assertEqual(1)
                                expect(data[0].vibrationEnabled).assertEqual(true)
                                expect(data[0].lightEnabled).assertEqual(false)
                                expect(data[0].lightColor).assertEqual(0)
                                console.debug("====>getSlotsByBundle1.1 finish====>");
                                notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                                    console.debug("====>removeSlot SERVICE_INFORMATION====>");
                                    expect(err.code).assertEqual(0);
                                    done();
                                })
                            })
                        })
                    })
                })
            })
        })


        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1700
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to UNKNOWN_TYPE's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(callback)
     */
        it('ActsAnsSetSlotByBundle_1700', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1700 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.UNKNOWN_TYPE,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.UNKNOWN_TYPE,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise UNKNOWN_TYPE enter====>");
            notification.addSlot(notification.SlotType.UNKNOWN_TYPE,() => {
                console.debug("====>getSlotsByBundle1 start====>");
                notification.getSlotsByBundle(bundleoption,(err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_1700====>" + JSON.stringify(data));
                    expect(data[0].type).assertEqual(65535)
                    expect(data[0].level).assertEqual(1)
                    expect(data[0].badgeFlag).assertEqual(true)
                    expect(data[0].bypassDnd).assertEqual(false)
                    expect(data[0].lockscreenVisibility).assertEqual(3)
                    expect(data[0].vibrationEnabled).assertEqual(false)
                    expect(data[0].lightEnabled).assertEqual(false)
                    expect(data[0].lightColor).assertEqual(0)
                    console.debug("====>getSlotsByBundle1 finish====>");
                    console.debug("====>setSlotByBundle start====>");
                    notification.setSlotByBundle(bundleoption, notificationslot1, () => {
                        console.debug("====>setSlotsByBundle1====>")
                        notification.setSlotByBundle(bundleoption, notificationslot2, () => {
                            console.debug("====>setSlotsByBundle2====>")
                            console.debug("====>getSlotsByBundle1.1 start====>");
                            notification.getSlotsByBundle(bundleoption,(err,data) => {
                                console.debug("====>ActsAnsSetSlotByBundle_1700.1====>" + JSON.stringify(data));
                                expect(data[0].type).assertEqual(65535)
                                expect(data[0].level).assertEqual(1)
                                expect(data[0].badgeFlag).assertEqual(true)
                                expect(data[0].bypassDnd).assertEqual(false)
                                expect(data[0].lockscreenVisibility).assertEqual(1)
                                expect(data[0].vibrationEnabled).assertEqual(false)
                                expect(data[0].lightEnabled).assertEqual(false)
                                expect(data[0].lightColor).assertEqual(0)
                                console.debug("====>getSlotsByBundle1.1 finish====>");
                                notification.removeSlot(notification.SlotType.UNKNOWN_TYPE, (err)=>{
                                    console.debug("====>removeSlot UNKNOWN_TYPE====>");
                                    expect(err.code).assertEqual(0);
                                    done();
                                })
                            })
                        })
                    })
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1800
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to SOCIAL_COMMUNICATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(callback)
     */
        it('ActsAnsSetSlotByBundle_1800', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1800 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise SOCIAL_COMMUNICATION enter====>");
            notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION,() => {
                console.debug("====>getSlotsByBundle1 start====>");
                notification.getSlotsByBundle(bundleoption,(err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_1800====>" + JSON.stringify(data));
                    expect(data[0].type).assertEqual(1)
                    expect(data[0].level).assertEqual(4)
                    expect(data[0].badgeFlag).assertEqual(true)
                    expect(data[0].bypassDnd).assertEqual(false)
                    expect(data[0].lockscreenVisibility).assertEqual(2)
                    expect(data[0].vibrationEnabled).assertEqual(true)
                    expect(data[0].lightEnabled).assertEqual(false)
                    expect(data[0].lightColor).assertEqual(0)
                    console.debug("====>getSlotsByBundle1 finish====>");
                    console.debug("====>setSlotByBundle start====>");
                    notification.setSlotByBundle(bundleoption, notificationslot1, () => {
                        console.debug("====>setSlotsByBundle1====>")
                        notification.setSlotByBundle(bundleoption, notificationslot2, () => {
                            console.debug("====>setSlotsByBundle2====>")
                            console.debug("====>getSlotsByBundle1.1 start====>");
                            notification.getSlotsByBundle(bundleoption,(err,data) => {
                                console.debug("====>ActsAnsSetSlotByBundle_1800.1====>" + JSON.stringify(data));
                                expect(data[0].type).assertEqual(1)
                                expect(data[0].level).assertEqual(4)
                                expect(data[0].badgeFlag).assertEqual(true)
                                expect(data[0].bypassDnd).assertEqual(false)
                                expect(data[0].lockscreenVisibility).assertEqual(1)
                                expect(data[0].vibrationEnabled).assertEqual(true)
                                expect(data[0].lightEnabled).assertEqual(false)
                                expect(data[0].lightColor).assertEqual(0)
                                console.debug("====>getSlotsByBundle1.1 finish====>");
                                notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                                    console.debug("====>removeSlot SOCIAL_COMMUNICATION====>");
                                    expect(err.code).assertEqual(0);
                                    done();
                                })
                            })
                        })
                    })
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_1900
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to CONTENT_INFORMATION's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(callback)
     */
        it('ActsAnsSetSlotByBundle_1900', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_1900 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.CONTENT_INFORMATION,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.CONTENT_INFORMATION,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
            notification.addSlot(notification.SlotType.CONTENT_INFORMATION,() => {
                console.debug("====>getSlotsByBundle1 start====>");
                notification.getSlotsByBundle(bundleoption,(err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_1900====>" + JSON.stringify(data));
                    expect(data[0].type).assertEqual(3)
                    expect(data[0].level).assertEqual(2)
                    expect(data[0].badgeFlag).assertEqual(true)
                    expect(data[0].bypassDnd).assertEqual(false)
                    expect(data[0].lockscreenVisibility).assertEqual(3)
                    expect(data[0].vibrationEnabled).assertEqual(false)
                    expect(data[0].lightEnabled).assertEqual(false)
                    expect(data[0].lightColor).assertEqual(0)
                    console.debug("====>getSlotsByBundle1 finish====>");
                    console.debug("====>setSlotByBundle start====>");
                    notification.setSlotByBundle(bundleoption, notificationslot1, () => {
                        console.debug("====>setSlotsByBundle1====>")
                        notification.setSlotByBundle(bundleoption, notificationslot2, () => {
                            console.debug("====>setSlotsByBundle2====>")
                            console.debug("====>getSlotsByBundle1.1 start====>");
                            notification.getSlotsByBundle(bundleoption,(err,data) => {
                                console.debug("====>ActsAnsSetSlotByBundle_1900.1====>" + JSON.stringify(data));
                                expect(data[0].type).assertEqual(3)
                                expect(data[0].level).assertEqual(2)
                                expect(data[0].badgeFlag).assertEqual(true)
                                expect(data[0].bypassDnd).assertEqual(false)
                                expect(data[0].lockscreenVisibility).assertEqual(1)
                                expect(data[0].vibrationEnabled).assertEqual(false)
                                expect(data[0].lightEnabled).assertEqual(false)
                                expect(data[0].lightColor).assertEqual(0)
                                console.debug("====>getSlotsByBundle1.1 finish====>");
                                notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                                    console.debug("====>removeSlot CONTENT_INFORMATION====>");
                                    expect(err.code).assertEqual(0);
                                    done();
                                })
                            })
                        })
                    })
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_2000
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slottype to OTHER_TYPES's slot,
     *                 call getsbybundle for information, and then call setSlotByBundle
     *                 twice to update the information.(callback)
     */
        it('ActsAnsSetSlotByBundle_2000', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_2000 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot1 = {
                type: notification.SlotType.OTHER_TYPES,
                level: 4
            }
            var notificationslot2 = {
                type: notification.SlotType.OTHER_TYPES,
                lockscreenVisibility: 1
            }
            console.debug("====>addSlotByTypePromise OTHER_TYPES enter====>");
            notification.addSlot(notification.SlotType.OTHER_TYPES,() => {
                console.debug("====>getSlotsByBundle1 start====>");
                notification.getSlotsByBundle(bundleoption,(err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_2000====>" + JSON.stringify(data));
                    expect(data[0].type).assertEqual(65535)
                    expect(data[0].level).assertEqual(1)
                    expect(data[0].badgeFlag).assertEqual(true)
                    expect(data[0].bypassDnd).assertEqual(false)
                    expect(data[0].lockscreenVisibility).assertEqual(3)
                    expect(data[0].vibrationEnabled).assertEqual(false)
                    expect(data[0].lightEnabled).assertEqual(false)
                    expect(data[0].lightColor).assertEqual(0)
                    console.debug("====>getSlotsByBundle1 finish====>");
                    console.debug("====>setSlotByBundle start====>");
                    notification.setSlotByBundle(bundleoption, notificationslot1, () => {
                        console.debug("====>setSlotsByBundle1====>")
                        notification.setSlotByBundle(bundleoption, notificationslot2, () => {
                            console.debug("====>setSlotsByBundle2====>")
                            console.debug("====>getSlotsByBundle1.1 start====>");
                            notification.getSlotsByBundle(bundleoption,(err,data) => {
                                console.debug("====>ActsAnsSetSlotByBundle_2000.1====>" + JSON.stringify(data));
                                expect(data[0].type).assertEqual(65535)
                                expect(data[0].level).assertEqual(1)
                                expect(data[0].badgeFlag).assertEqual(true)
                                expect(data[0].bypassDnd).assertEqual(false)
                                expect(data[0].lockscreenVisibility).assertEqual(1)
                                expect(data[0].vibrationEnabled).assertEqual(false)
                                expect(data[0].lightEnabled).assertEqual(false)
                                expect(data[0].lightColor).assertEqual(0)
                                console.debug("====>getSlotsByBundle1.1 finish====>");
                                notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                                    console.debug("====>removeSlot OTHER_TYPES====>");
                                    expect(err.code).assertEqual(0);
                                    done();
                                })
                            })
                        })
                    })
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_2100
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : Do not call addSlot, call setSlotByBundle directly,
     *                 and then call getSlotsByBundle query results.(callback)
     */
        it('ActsAnsSetSlotByBundle_2100', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_2100 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot = {
                type: notification.SlotType.OTHER_TYPES,
                level: 4
            }
            notification.setSlotByBundle(bundleoption, notificationslot, (err) => {
                console.debug("====>setSlotsByBundle1====>")
                console.debug("====>getSlotsByBundle1.1 start====>");
                notification.getSlotsByBundle(bundleoption,(err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_2100.1====>" + JSON.stringify(data));
                    expect(data).assertEqual(0)
                    console.debug("====>getSlotsByBundle1.1 finish====>");
                    done();
                })
            })
        })

        /*
     * @tc.number    : ActsAnsSetSlotByBundle_2200
     * @tc.name      : Verify getSlot OTHER_TYPES
     * @tc.desc      : Do not call addSlot, call setSlotByBundle directly,
     *                 and then call getSlotsByBundle query results.(promise)
     */
        it('ActsAnsSetSlotByBundle_2200', 0, async function (done) {
            console.debug("====>ActsAnsSetSlotByBundle_2200 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            var notificationslot = {
                type: notification.SlotType.OTHER_TYPES,
                level: 4
            }
            notification.setSlotByBundle(bundleoption, notificationslot).then(() => {
            }).catch((err)=>{
                console.debug("====>setSlotsByBundle1====>")
                console.debug("====>getSlotsByBundle1.1 start====>");
                notification.getSlotsByBundle(bundleoption, (err,data) => {
                    console.debug("====>ActsAnsSetSlotByBundle_2200.1====>" + JSON.stringify(data));
                    expect(data).assertEqual(0)
                    console.debug("====>getSlotsByBundle1.1 finish====>");
                    done();
                })
            })
        })

        /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0100
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding slot, call getSlotNumByBundle for information.(callback)
     */
        it('ActsAnsGetSlotNumByBundle_0100', 0, async function (done) {
            console.debug("====>ActsAnsGetSlotNumByBundle_0600 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
            notification.addSlot(notification.SlotType.SERVICE_INFORMATION,() => {
                notification.getSlotNumByBundle(bundleoption, (err, data) => {
                    console.debug("====>ActsAnsGetSlotNumByBundle_0100====>" + JSON.stringify(data));
                    expect(data).assertEqual(1)
                    notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                        console.debug("====>removeSlot SERVICE_INFORMATION====>");
                        expect(err.code).assertEqual(0);
                        done();
                    })
                })
            })
        })

        /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0200
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding slot, call getSlotNumByBundle for information.(promise)
     */
        it('ActsAnsGetSlotNumByBundle_0200', 0, async function (done) {
            console.debug("====>ActsAnsGetSlotNumByBundle_0200 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
            await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
            notification.getSlotNumByBundle(bundleoption).then((data) => {
                console.debug("====>ActsAnsGetSlotNumByBundle_0200====>" + JSON.stringify(data));
                expect(data).assertEqual(1)
                notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                    console.debug("====>removeSlot SERVICE_INFORMATION====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })




        /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0300
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding twice different type slot,
     *                 call getSlotNumByBundle for information.(callback)
     */
        it('ActsAnsGetSlotNumByBundle_0300', 0, async function (done) {
            console.debug("====>ActsAnsGetSlotNumByBundle_0300 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
            notification.addSlot(notification.SlotType.SERVICE_INFORMATION, () => {
                console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
                notification.addSlot(notification.SlotType.CONTENT_INFORMATION, () => {
                    notification.getSlotNumByBundle(bundleoption, (err, data) => {
                        console.debug("====>ActsAnsGetSlotNumByBundle_0300====>" + JSON.stringify(data));
                        expect(data).assertEqual(2)
                        notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                            console.debug("====>removeSlot SERVICE_INFORMATION====>");
                            console.debug("====>SERVICE_INFORMATION err.code====>" + JSON.stringify(err.code));
                            expect(err.code).assertEqual(0);
                            notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                                console.debug("====>removeSlot CONTENT_INFORMATION====>");
                                console.debug("====>CONTENT_INFORMATION err.code====>" + JSON.stringify(err.code));
                                expect(err.code).assertEqual(0);
                                done();
                            })
                        })
                    })
                })
            })
        })

        /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0400
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding twice different type slot,
     *                 call getSlotNumByBundle for information.(promise)
     */
        it('ActsAnsGetSlotNumByBundle_0400', 0, async function (done) {
            console.debug("====>ActsAnsGetSlotNumByBundle_0400 start====>");
            var bundleoption = {
                bundle: "com.example.actsansslotbybundle"
            }
            console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
            await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
            console.debug("====>addSlotByTypePromise CONTENT_INFORMATION enter====>");
            await notification.addSlot(notification.SlotType.CONTENT_INFORMATION);
        notification.getSlotNumByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotNumByBundle_0400====>" + JSON.stringify(data));
            expect(data).assertEqual(2)
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot SERVICE_INFORMATION====>");
                expect(err.code).assertEqual(0);
                notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                    console.debug("====>removeSlot CONTENT_INFORMATION====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0500
     * @tc.name      : Verify getSlot null
     * @tc.desc      : Call getSlotNumByBundle for information.(callback)
     */
    it('ActsAnsGetSlotNumByBundle_0500', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotNumByBundle_0700 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        notification.getSlotNumByBundle(bundleoption, (err, data) => {
            console.debug("====>ActsAnsGetSlotNumByBundle_0500====>" + JSON.stringify(data));
            expect(data).assertEqual(0)
            done();
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0600
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : Call getSlotNumByBundle for information.(promise)
     */
    it('ActsAnsGetSlotNumByBundle_0600', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotNumByBundle_0600 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        notification.getSlotNumByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotNumByBundle_0600====>" + JSON.stringify(data));
            expect(data).assertEqual(0)
            done();
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0700
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : After adding twice same type slot,
     *                 call getSlotNumByBundle for information.(callback)
     */
    it('ActsAnsGetSlotNumByBundle_0700', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotNumByBundle_0700 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        notification.addSlot(notification.SlotType.SERVICE_INFORMATION, ()=>{
            console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
            notification.addSlot(notification.SlotType.SERVICE_INFORMATION, ()=>{
                notification.getSlotNumByBundle(bundleoption, (err, data) => {
                    console.debug("====>ActsAnsGetSlotNumByBundle_0700====>" + JSON.stringify(data));
                    expect(data).assertEqual(1)
                    notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                        console.debug("====>removeSlot SERVICE_INFORMATION====>");
                        expect(err.code).assertEqual(0);
                        done();
                    })
                })
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0800
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : After adding twice same type slot,
     *                 call getSlotNumByBundle for information.(promise)
     */
    it('ActsAnsGetSlotNumByBundle_0800', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotNumByBundle_0800 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundle"
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
        notification.getSlotNumByBundle(bundleoption).then((data) => {
            console.debug("====>ActsAnsGetSlotNumByBundle_0800====>" + JSON.stringify(data));
            expect(data).assertEqual(1)
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot SERVICE_INFORMATION====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_0900
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : Bundlename incorrectly adds slot after calling
     *                 getSlotNumByBundle to get information.(callback)
     */
    it('ActsAnsGetSlotNumByBundle_0900', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotNumByBundle_0900 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundleerr"
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        notification.addSlot(notification.SlotType.SERVICE_INFORMATION, ()=>{
            notification.getSlotNumByBundle(bundleoption, (err, data) => {
                console.debug("====>ActsAnsGetSlotNumByBundle_0900====>" + JSON.stringify(data));
                expect(data).assertEqual(0)
                notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                    console.debug("====>removeSlot SERVICE_INFORMATION====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            })
        })
    })

    /*
     * @tc.number    : ActsAnsGetSlotNumByBundle_1000
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : Bundlename incorrectly adds slot after calling
     *                 getSlotNumByBundle to get information.(promise)
     */
    it('ActsAnsGetSlotNumByBundle_1000', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotNumByBundle_1000 start====>");
        var bundleoption = {
            bundle: "com.example.actsansslotbybundleerr"
        }
        console.debug("====>addSlotByTypePromise SERVICE_INFORMATION enter====>");
        await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
        notification.getSlotNumByBundle(bundleoption).then((data) => {
        }).catch((err)=>{
            expect(err!=0).assertEqual(true)
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot SERVICE_INFORMATION err:"  + JSON.stringify(err));
            })
            done();
        })
    })
})

