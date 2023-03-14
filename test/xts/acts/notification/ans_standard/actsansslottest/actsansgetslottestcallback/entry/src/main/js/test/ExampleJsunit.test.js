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
const SOUNDURL = "file://system/etc/Light.ogg";
describe('ActsAnsGetSlotTestCallback', function () {

    /*
     * @tc.number    : ActsAnsGetSlotTestCallback_0100
     * @tc.name      : Verify getSlot SOCIAL_COMMUNICATION
     * @tc.desc      : Get the SOCIAL_COMMUNICATION type slot after adding
     */
    it('ActsAnsGetSlotTestCallback_0100', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotTestCallback_0100 start====>");
        console.debug("====>addSlot SlotType.SOCIAL_COMMUNICATION start====>");
        notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
            console.debug("====>addSlot 0100 enter====>");
            console.debug("====>addSlot 0100 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot SlotType.SOCIAL_COMMUNICATION start====>");
            notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err, data)=>{
                console.debug("====>getSlot 0100 enter====>");
                console.debug("====>getSlot 0100 err:" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>getSlot ActsAnsGetSlotTestCallback_0100 data:" + JSON.stringify(data));
                expect(data.type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
                expect(data.level).assertEqual(4);
                expect(data.desc).assertEqual("");
                expect(data.badgeFlag).assertEqual(true);
                expect(data.bypassDnd).assertEqual(false);
                expect(data.lockscreenVisibility).assertEqual(2);
                expect(data.vibrationEnabled).assertEqual(true);
                expect(data.sound).assertEqual(SOUNDURL);
                expect(data.lightEnabled).assertEqual(false);
                expect(data.lightColor).assertEqual(0);
                console.debug("====>getSlot ActsAnsGetSlotTestCallback_0100 finish====>");
                notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                    console.debug("====>removeSlot SOCIAL_COMMUNICATION enter====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            });
        });
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotTestCallback_0100====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotTestCallback_0200
     * @tc.name      : Verify getSlot SERVICE_INFORMATION
     * @tc.desc      : Get the SERVICE_INFORMATION type slot after adding
     */
    it('ActsAnsGetSlotTestCallback_0200', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotTestCallback_0200 start====>");
        function getSlotCallbackSecond(err, data) {
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0200 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0200 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0200 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.SERVICE_INFORMATION);
            expect(data.level).assertEqual(3);
            expect(data.desc).assertEqual("");
            expect(data.badgeFlag).assertEqual(true);
            expect(data.bypassDnd).assertEqual(false);
            expect(data.lockscreenVisibility).assertEqual(2);
            expect(data.vibrationEnabled).assertEqual(true);
            expect(data.sound).assertEqual(SOUNDURL);
            expect(data.lightEnabled).assertEqual(false);
            expect(data.lightColor).assertEqual(0);
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0200 finish====>");
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot SERVICE_INFORMATION enter====>");
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot SlotType.SERVICE_INFORMATION start====>");
        notification.addSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
            console.debug("====>addSlot ActsAnsGetSlotTestCallback_0200 enter====>");
            console.debug("====>addSlot ActsAnsGetSlotTestCallback_0200 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot SlotType.SERVICE_INFORMATION start====>");
            notification.getSlot(notification.SlotType.SERVICE_INFORMATION, getSlotCallbackSecond);
        });
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotTestCallback_0200====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotTestCallback_0300
     * @tc.name      : Verify getSlot CONTENT_INFORMATION
     * @tc.desc      : Get the CONTENT_INFORMATION type slot after adding
     */
    it('ActsAnsGetSlotTestCallback_0300', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotTestCallback_0300 start====>");
        function getSlotCallbackThird(err, data) {
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0300 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0300 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.CONTENT_INFORMATION);
            expect(data.level).assertEqual(2);
            expect(data.desc).assertEqual("");
            expect(data.badgeFlag).assertEqual(true);
            expect(data.bypassDnd).assertEqual(false);
            expect(data.lockscreenVisibility).assertEqual(3);
            expect(data.vibrationEnabled).assertEqual(false);
            expect(data.sound).assertEqual("");
            expect(data.lightEnabled).assertEqual(false);
            expect(data.lightColor).assertEqual(0);
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0300 finish====>");
            notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
                console.debug("====>removeSlot CONTENT_INFORMATION enter====>");
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot SlotType.CONTENT_INFORMATION start====>");
        notification.addSlot(notification.SlotType.CONTENT_INFORMATION, (err)=>{
            console.debug("====>addSlotActsAnsGetSlotTestCallback_0300 enter====>");
            console.debug("====>addSlotActsAnsGetSlotTestCallback_0300 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot SlotType.CONTENT_INFORMATION start====>");
            notification.getSlot(notification.SlotType.CONTENT_INFORMATION, getSlotCallbackThird);
        });
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotTestCallback_0300====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotTestCallback_0400
     * @tc.name      : Verify getSlot OTHER_TYPES
     * @tc.desc      : Get the OTHER_TYPES type slot after adding
     */
    it('ActsAnsGetSlotTestCallback_0400', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotTestCallback_0400 start====>");
        function getSlotCallbackFourth(err, data) {
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0400 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0400 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0400 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.OTHER_TYPES);
            expect(data.level).assertEqual(1);
            expect(data.desc).assertEqual("");
            expect(data.badgeFlag).assertEqual(true);
            expect(data.bypassDnd).assertEqual(false);
            expect(data.lockscreenVisibility).assertEqual(3);
            expect(data.vibrationEnabled).assertEqual(false);
            expect(data.sound).assertEqual("");
            expect(data.lightEnabled).assertEqual(false);
            expect(data.lightColor).assertEqual(0);
            console.debug("====>getSlotActsAnsGetSlotTestCallback_0400 finish====>");
            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                console.debug("====>removeSlot OTHER_TYPES enter====>");
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot SlotType.OTHER_TYPES start====>");
        notification.addSlot(notification.SlotType.OTHER_TYPES, (err)=>{
            console.debug("====>addSlotActsAnsGetSlotTestCallback_0400 enter====>");
            console.debug("====>addSlotActsAnsGetSlotTestCallback_0400 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot SlotType.OTHER_TYPES start====>");
            notification.getSlot(notification.SlotType.OTHER_TYPES, getSlotCallbackFourth);
        });
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotTestCallback_0400====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotTestCallback_0500
     * @tc.name      : Verify getSlot UNKNOWN_TYPE
     * @tc.desc      : Get the UNKNOWN_TYPE type slot after adding
     */
    it('ActsAnsGetSlotTestCallback_0500', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotTestCallback_0500 start====>");
        function getSlotCallbackFifth(err, data) {
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0500 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0500 data:" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.OTHER_TYPES);
            expect(data.level).assertEqual(1);
            expect(data.desc).assertEqual("");
            expect(data.badgeFlag).assertEqual(true);
            expect(data.bypassDnd).assertEqual(false);
            expect(data.lockscreenVisibility).assertEqual(3);
            expect(data.vibrationEnabled).assertEqual(false);
            expect(data.sound).assertEqual("");
            expect(data.lightEnabled).assertEqual(false);
            expect(data.lightColor).assertEqual(0);
            console.debug("====>getSlot ActsAnsGetSlotTestCallback_0500 finish====>");
            notification.removeSlot(notification.SlotType.OTHER_TYPES, (err)=>{
                console.debug("====>removeSlot OTHER_TYPES enter====>");
                expect(err.code).assertEqual(0);
                done();
            })
        }
        console.debug("====>addSlot SlotType.UNKNOWN_TYPE start====>");
        notification.addSlot(notification.SlotType.UNKNOWN_TYPE, (err)=>{
            console.debug("====>addSlotActsAnsGetSlotTestCallback_0500 enter====>");
            console.debug("====>addSlotActsAnsGetSlotTestCallback_0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlot SlotType.UNKNOWN_TYPE start====>");
            notification.getSlot(notification.SlotType.UNKNOWN_TYPE, getSlotCallbackFifth);
        });
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotTestCallback_0500====>");
        }, TIMEOUT);
    })
})