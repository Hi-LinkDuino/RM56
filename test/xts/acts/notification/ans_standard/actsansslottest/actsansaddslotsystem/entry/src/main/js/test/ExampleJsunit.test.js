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

const TIMEOUT = 2000;
describe('ActsAnsAddSlotSystem', function () {

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0100
     * @tc.name      : addSlot notification
     * @tc.desc      : Get the added SOCIAL_COMMUNICATION type slot
     */
    it('ActsAnsAddSlotSystem_0100', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0100 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsAddSlotSystem_0100 enter====>");
            console.debug("====>getSlot 0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot 0100 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
            expect(data.level).assertEqual(notification.SlotLevel.LEVEL_NONE);
            expect(data.desc).assertEqual("slot_SOCIAL_COMMUNICATION_desc");
            expect(data.badgeFlag).assertEqual(false);
            expect(data.bypassDnd).assertEqual(true);
            expect(data.vibrationEnabled).assertEqual(true);
            expect(data.sound).assertEqual("slot_SOCIAL_COMMUNICATION_sound");
            expect(data.lightEnabled).assertEqual(true);
            expect(data.lightColor).assertEqual(1);
            notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                console.debug("====>removeSlot ActsAnsAddSlotSystem_0100 err====>" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot SOCIAL_COMMUNICATION====>");
        notification.addSlot(
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: notification.SlotLevel.LEVEL_NONE,
                desc: "slot_SOCIAL_COMMUNICATION_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_SOCIAL_COMMUNICATION_sound",
                lightEnabled: true,
                lightColor: 1
            },
            (err)=>{
                console.debug("====>addSlot SOCIAL_COMMUNICATION callback====>");
                expect(err.code).assertEqual(0);
                console.debug("====>getSlot SlotType.SOCIAL_COMMUNICATION====>");
                notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION, getSlotCallback);
            })
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0100====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0200
     * @tc.name      : addSlot notification
     * @tc.desc      : Get the added SERVICE_INFORMATION type slot
     */
    it('ActsAnsAddSlotSystem_0200', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0200 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsAddSlotSystem_0200 enter====>");
            console.debug("====>getSlot 0200 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot 0200 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.SERVICE_INFORMATION);
            expect(data.level).assertEqual(notification.SlotLevel.LEVEL_MIN);
            expect(data.desc).assertEqual("slot_SERVICE_INFORMATION_desc");
            expect(data.badgeFlag).assertEqual(false);
            expect(data.bypassDnd).assertEqual(true);
            expect(data.vibrationEnabled).assertEqual(true);
            expect(data.sound).assertEqual("slot_SERVICE_INFORMATION_sound");
            expect(data.lightEnabled).assertEqual(true);
            expect(data.lightColor).assertEqual(2);
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot ActsAnsAddSlotSystem_0200 err====>" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot SERVICE_INFORMATION====>");
        notification.addSlot(
            {
                type: notification.SlotType.SERVICE_INFORMATION,
                level: notification.SlotLevel.LEVEL_MIN,
                desc: "slot_SERVICE_INFORMATION_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_SERVICE_INFORMATION_sound",
                lightEnabled: true,
                lightColor: 2
            },
            (err)=>{
                console.debug("====>addSlot SERVICE_INFORMATION callback====>");
                expect(err.code).assertEqual(0);
                console.debug("====>getSlot SlotType.SERVICE_INFORMATION====>");
                notification.getSlot(notification.SlotType.SERVICE_INFORMATION, getSlotCallback);
            })
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0200====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0300
     * @tc.name      : addSlot notification
     * @tc.desc      : Get the added CONTENT_INFORMATION type slot
     */
    it('ActsAnsAddSlotSystem_0300', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0300 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsAddSlotSystem_0300 enter====>");
            console.debug("====>getSlot 0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot 0300 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.CONTENT_INFORMATION);
            expect(data.level).assertEqual(notification.SlotLevel.LEVEL_LOW);
            expect(data.desc).assertEqual("slot_CONTENT_INFORMATION_desc");
            expect(data.badgeFlag).assertEqual(false);
            expect(data.bypassDnd).assertEqual(true);
            expect(data.vibrationEnabled).assertEqual(true);
            expect(data.sound).assertEqual("slot_CONTENT_INFORMATION_sound");
            expect(data.lightEnabled).assertEqual(true);
            expect(data.lightColor).assertEqual(3);
            notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                console.debug("====>removeSlot ActsAnsAddSlotSystem_0300 err====>" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot CONTENT_INFORMATION====>");
        notification.addSlot(
            {
                type: notification.SlotType.CONTENT_INFORMATION,
                level: notification.SlotLevel.LEVEL_LOW,
                desc: "slot_CONTENT_INFORMATION_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_CONTENT_INFORMATION_sound",
                lightEnabled: true,
                lightColor: 3
            },
            (err)=>{
                console.debug("====>addSlot CONTENT_INFORMATION callback====>");
                expect(err.code).assertEqual(0);
                console.debug("====>getSlot SlotType.CONTENT_INFORMATION====>");
                notification.getSlot(notification.SlotType.CONTENT_INFORMATION, getSlotCallback);
            })
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0300====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0400
     * @tc.name      : addSlot notification
     * @tc.desc      : Get the added OTHER_TYPES type slot
     */
    it('ActsAnsAddSlotSystem_0400', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0400 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsAddSlotSystem_0400 enter====>");
            console.debug("====>getSlot 0400 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot 0400 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.OTHER_TYPES);
            expect(data.level).assertEqual(notification.SlotLevel.LEVEL_DEFAULT);
            expect(data.desc).assertEqual("slot_OTHER_TYPES_desc");
            expect(data.badgeFlag).assertEqual(false);
            expect(data.bypassDnd).assertEqual(true);
            expect(data.vibrationEnabled).assertEqual(true);
            expect(data.sound).assertEqual("slot_OTHER_TYPES_sound");
            expect(data.lightEnabled).assertEqual(true);
            expect(data.lightColor).assertEqual(4);
            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                console.debug("====>removeSlot ActsAnsAddSlotSystem_0400 err====>" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot OTHER_TYPES====>");
        notification.addSlot(
            {
                type: notification.SlotType.OTHER_TYPES,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_OTHER_TYPES_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_OTHER_TYPES_sound",
                lightEnabled: true,
                lightColor: 4
            },
            (err)=>{
                console.debug("====>addSlot OTHER_TYPES callback====>");
                expect(err.code).assertEqual(0);
            })
        console.debug("====>getSlot SlotType.OTHER_TYPES====>");
        notification.getSlot(notification.SlotType.OTHER_TYPES, getSlotCallback);
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0400====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0500
     * @tc.name      : addSlot notification
     * @tc.desc      : Get the added UNKNOWN_TYPE type slot
     */
    it('ActsAnsAddSlotSystem_0500', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0500 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsAddSlotSystem_0500 enter====>");
            console.debug("====>getSlot 0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot 0500 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.OTHER_TYPES);
            expect(data.level).assertEqual(notification.SlotLevel.LEVEL_DEFAULT);
            expect(data.desc).assertEqual("slot_OTHER_TYPES_desc");
            expect(data.badgeFlag).assertEqual(false);
            expect(data.bypassDnd).assertEqual(true);
            expect(data.vibrationEnabled).assertEqual(true);
            expect(data.sound).assertEqual("slot_OTHER_TYPES_sound");
            expect(data.lightEnabled).assertEqual(true);
            expect(data.lightColor).assertEqual(4);
            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                console.debug("====>removeSlot ActsAnsAddSlotSystem_0500 err====>" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot UNKNOWN_TYPE====>");
        await notification.addSlot(
            {
                type: notification.SlotType.UNKNOWN_TYPE,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_OTHER_TYPES_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_OTHER_TYPES_sound",
                lightEnabled: true,
                lightColor: 4
            },
            (err)=>{
                console.debug("====>addSlot UNKNOWN_TYPE callback====>");
                expect(err.code).assertEqual(0);
                console.debug("====>getSlot SlotType.UNKNOWN_TYPE====>");
                notification.getSlot(notification.SlotType.UNKNOWN_TYPE, getSlotCallback);
            })
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0500====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0600
     * @tc.name      : addSlot notification promise
     * @tc.desc      : Get the added SOCIAL_COMMUNICATION type slot
     */
    it('ActsAnsAddSlotSystem_0600', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0600 start====>");
        console.debug("====>addSlot SOCIAL_COMMUNICATION====>");
        await notification.addSlot(
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: notification.SlotLevel.LEVEL_NONE,
                desc: "slot_SOCIAL_COMMUNICATION_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_SOCIAL_COMMUNICATION_sound",
                lightEnabled: true,
                lightColor: 1
            });
        console.debug("====>getSlot SlotType.SOCIAL_COMMUNICATION====>");
        var data = await notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION);
        console.debug("====>getSlot 0600 data:" + JSON.stringify(data));
        expect(data.type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
        expect(data.level).assertEqual(notification.SlotLevel.LEVEL_NONE);
        expect(data.desc).assertEqual("slot_SOCIAL_COMMUNICATION_desc");
        expect(data.badgeFlag).assertEqual(false);
        expect(data.bypassDnd).assertEqual(true);
        expect(data.vibrationEnabled).assertEqual(true);
        expect(data.sound).assertEqual("slot_SOCIAL_COMMUNICATION_sound");
        expect(data.lightEnabled).assertEqual(true);
        expect(data.lightColor).assertEqual(1);
        console.debug("====>removeSlot ActsAnsAddSlotSystem_0600 start====>");
        await notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION);
        console.debug("====>ActsAnsAddSlotSystem_0600 end====>");
        done();
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0600====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0700
     * @tc.name      : addSlot notification promise
     * @tc.desc      : Get the added SERVICE_INFORMATION type slot
     */
    it('ActsAnsAddSlotSystem_0700', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0700 start====>");
        console.debug("====>addSlot SERVICE_INFORMATION====>");
        await notification.addSlot(
            {
                type: notification.SlotType.SERVICE_INFORMATION,
                level: notification.SlotLevel.LEVEL_MIN,
                desc: "slot_SERVICE_INFORMATION_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_SERVICE_INFORMATION_sound",
                lightEnabled: true,
                lightColor: 2
            });
        console.debug("====>getSlot SlotType.SERVICE_INFORMATION====>");
        var data = await notification.getSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>getSlot 0700 data:" + JSON.stringify(data));
        expect(data.type).assertEqual(notification.SlotType.SERVICE_INFORMATION);
        expect(data.level).assertEqual(notification.SlotLevel.LEVEL_MIN);
        expect(data.desc).assertEqual("slot_SERVICE_INFORMATION_desc");
        expect(data.badgeFlag).assertEqual(false);
        expect(data.bypassDnd).assertEqual(true);
        expect(data.vibrationEnabled).assertEqual(true);
        expect(data.sound).assertEqual("slot_SERVICE_INFORMATION_sound");
        expect(data.lightEnabled).assertEqual(true);
        expect(data.lightColor).assertEqual(2);
        console.debug("====>removeSlot ActsAnsAddSlotSystem_0700 start====>");
        await notification.removeSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>ActsAnsAddSlotSystem_0700 end====>");
        done();
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0700====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0800
     * @tc.name      : addSlot notification promise
     * @tc.desc      : Get the added CONTENT_INFORMATION type slot
     */
    it('ActsAnsAddSlotSystem_0800', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0800 start====>");
        console.debug("====>addSlot CONTENT_INFORMATION====>");
        await notification.addSlot(
            {
                type: notification.SlotType.CONTENT_INFORMATION,
                level: notification.SlotLevel.LEVEL_LOW,
                desc: "slot_CONTENT_INFORMATION_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_CONTENT_INFORMATION_sound",
                lightEnabled: true,
                lightColor: 3
            });
        console.debug("====>getSlot SlotType.CONTENT_INFORMATION====>");
        var data = await notification.getSlot(notification.SlotType.CONTENT_INFORMATION);
        console.debug("====>getSlot 0800 data:" + JSON.stringify(data));
        expect(data.type).assertEqual(notification.SlotType.CONTENT_INFORMATION);
        expect(data.level).assertEqual(notification.SlotLevel.LEVEL_LOW);
        expect(data.desc).assertEqual("slot_CONTENT_INFORMATION_desc");
        expect(data.badgeFlag).assertEqual(false);
        expect(data.bypassDnd).assertEqual(true);
        expect(data.vibrationEnabled).assertEqual(true);
        expect(data.sound).assertEqual("slot_CONTENT_INFORMATION_sound");
        expect(data.lightEnabled).assertEqual(true);
        expect(data.lightColor).assertEqual(3);
        console.debug("====>removeSlot ActsAnsAddSlotSystem_0800 start====>");
        await notification.removeSlot(notification.SlotType.CONTENT_INFORMATION);
        console.debug("====>ActsAnsAddSlotSystem_0800 end====>");
        done();
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0800====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_0900
     * @tc.name      : addSlot notification promise
     * @tc.desc      : Get the added OTHER_TYPES type slot
     */
    it('ActsAnsAddSlotSystem_0900', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_0900 start====>");
        console.debug("====>addSlot OTHER_TYPES====>");
        notification.addSlot(
            {
                type: notification.SlotType.OTHER_TYPES,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_OTHER_TYPES_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_OTHER_TYPES_sound",
                lightEnabled: true,
                lightColor: 4
            })
        console.debug("====>getSlot SlotType.OTHER_TYPES====>");
        var data = await notification.getSlot(notification.SlotType.OTHER_TYPES);
        console.debug("====>getSlot 0900 data:" + JSON.stringify(data));
        expect(data.type).assertEqual(notification.SlotType.OTHER_TYPES);
        expect(data.level).assertEqual(notification.SlotLevel.LEVEL_DEFAULT);
        expect(data.desc).assertEqual("slot_OTHER_TYPES_desc");
        expect(data.badgeFlag).assertEqual(false);
        expect(data.bypassDnd).assertEqual(true);
        expect(data.vibrationEnabled).assertEqual(true);
        expect(data.sound).assertEqual("slot_OTHER_TYPES_sound");
        expect(data.lightEnabled).assertEqual(true);
        expect(data.lightColor).assertEqual(4);
        console.debug("====>removeSlot ActsAnsAddSlotSystem_0900 start====>");
        await notification.removeSlot(notification.SlotType.OTHER_TYPES);
        console.debug("====>ActsAnsAddSlotSystem_0900 end====>");
        done();
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_0900====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsAddSlotSystem_1000
     * @tc.name      : addSlot notification promise
     * @tc.desc      : Get the added UNKNOWN_TYPE type slot
     */
    it('ActsAnsAddSlotSystem_1000', 0, async function (done) {
        console.debug("====>ActsAnsAddSlotSystem_1000 start====>");
        console.debug("====>addSlot UNKNOWN_TYPE====>");
        await notification.addSlot(
            {
                type: notification.SlotType.UNKNOWN_TYPE,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_OTHER_TYPES_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_OTHER_TYPES_sound",
                lightEnabled: true,
                lightColor: 4
            });
        console.debug("====>getSlot SlotType.UNKNOWN_TYPE====>");
        var data = await notification.getSlot(notification.SlotType.UNKNOWN_TYPE);
        console.debug("====>getSlot enter====>");
        console.debug("====>getSlot data:" + JSON.stringify(data));
        expect(data.type).assertEqual(notification.SlotType.OTHER_TYPES);
        expect(data.level).assertEqual(notification.SlotLevel.LEVEL_DEFAULT);
        expect(data.desc).assertEqual("slot_OTHER_TYPES_desc");
        expect(data.badgeFlag).assertEqual(false);
        expect(data.bypassDnd).assertEqual(true);
        expect(data.vibrationEnabled).assertEqual(true);
        expect(data.sound).assertEqual("slot_OTHER_TYPES_sound");
        expect(data.lightEnabled).assertEqual(true);
        expect(data.lightColor).assertEqual(4);
        console.debug("====>removeSlot ActsAnsAddSlotSystem_1000 start====>");
        await notification.removeSlot(notification.SlotType.OTHER_TYPES);
        console.debug("====>ActsAnsAddSlotSystem_1000 end====>");
        done();
        setTimeout(function(){
            console.debug("====>time out ActsAnsAddSlotSystem_1000====>");
        }, TIMEOUT);
    })
}) 