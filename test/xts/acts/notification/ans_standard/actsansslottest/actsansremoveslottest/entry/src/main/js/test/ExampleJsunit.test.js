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

const TIMEOUT = 3000;
const SOUNDURL = "file://system/etc/Light.ogg";
describe('ActsAnsRemoveSlotTest', function () {

    /*
     * @tc.number    : ActsAnsRemoveSlotTest_0100
     * @tc.name      : Verify that the slot is removed twice callback
     * @tc.desc      : Remove the added SOCIAL_COMMUNICATION slot and then remove it again
     */
    it('ActsAnsRemoveSlotTest_0100', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotTest_0100 start====>");
        console.debug("====>addSlot ActsAnsRemoveSlotTest_0100 start====>");
        await notification.addSlot(notification.SlotType.SOCIAL_COMMUNICATION);
        console.debug("====>getSlot SlotType.SOCIAL_COMMUNICATION start====>");
        var data = await notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION);
        console.debug("====>getSlot ActsAnsRemoveSlotTest_0100 enter====>");
        console.debug("====>getSlot ActsAnsRemoveSlotTest_0100 data====>" + JSON.stringify(data));
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
        console.debug("====>getSlot ActsAnsRemoveSlotTest_0100 finish====>");
        notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
            console.debug("====>removeSlot first time callback err====>" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err, data)=>{
                console.debug("====>getSlot second time enter====>");
                console.debug("====>getSlot second time err:" + JSON.stringify(err));
                expect(err.code != 0).assertEqual(true);
                console.debug("====>getSlot second data:" + JSON.stringify(data));
                notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                    console.debug("====>removeSlot second time callback err:" + JSON.stringify(err));
                    console.debug("====>ActsAnsRemoveSlotTest_0100 end====>");
                    expect(err.code != 0).assertEqual(true);
                    done();
                })
            })
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotTest_0100====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotTest_0200
     * @tc.name      : Verify that the slot is removed twice 
     * @tc.desc      : Remove the added SERVICE_INFORMATION slot and then remove it again
     */
    it('ActsAnsRemoveSlotTest_0200', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotTest_0200 start====>");
        console.debug("====>addSlot ActsAnsRemoveSlotTest_0200 start====>");
        await notification.addSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>getSlot SlotType.SERVICE_INFORMATION start====>");
        var data = await notification.getSlot(notification.SlotType.SERVICE_INFORMATION);
        console.debug("====>getSlot ActsAnsRemoveSlotTest_0200 enter====>");
        console.debug("====>getSlot ActsAnsRemoveSlotTest_0200 data:" + JSON.stringify(data));
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
        console.debug("====>getSlot ActsAnsRemoveSlotTest_0200 finish====>");
        console.debug("====>removeSlot first time promise start====>");
        await notification.removeSlot(notification.SlotType.SERVICE_INFORMATION);
        notification.getSlot(notification.SlotType.SERVICE_INFORMATION, (err, data)=>{
            console.debug("====>getSlot second time enter====>");
            console.debug("====>getSlot second time err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            console.debug("====>second getSlot data====>" + JSON.stringify(data));
            notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, (err)=>{
                console.debug("====>removeSlot second time err:" + JSON.stringify(err));
                console.debug("====>ActsAnsRemoveSlotTest_0200 end====>");
                expect(err.code != 0).assertEqual(true);
                done();
            })
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotTest_0200====>");
        }, TIMEOUT);
    })
})