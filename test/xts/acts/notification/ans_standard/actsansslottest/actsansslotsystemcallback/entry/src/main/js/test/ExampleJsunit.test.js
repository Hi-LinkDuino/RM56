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
describe('ActsAnsSlotSystemCallback', function () {

    /*
     * @tc.number    : ActsAnsSlotSystemCallback_0100
     * @tc.name      : Verify getSlots after adding slots and removeAllSlots
     * @tc.desc      : getSlots after adding all type slots and remove all slots
     */
    it('ActsAnsSlotSystemCallback_0100', 0, async function (done) {
        console.debug("====>ActsAnsSlotSystemCallback_0100 start====>");
        function timeOut(){
            console.debug("====>time out enter ActsAnsSlotSystemCallback_0100====>");
        }
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
        console.debug("====>addSlot OTHER_TYPES====>");
        await notification.addSlot(
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
            });
        console.debug("====>addSlot UNKNOWN_TYPE====>");
        await notification.addSlot(
            {
                type: notification.SlotType.UNKNOWN_TYPE,
                level: notification.SlotLevel.LEVEL_HIGH,
                desc: "slot_UNKNOWN_TYPE_desc",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_UNKNOWN_TYPE_sound",
                lightEnabled: true,
                lightColor: 5
            });
        notification.getSlots((err, data)=>{
            console.debug("====>getSlots enter====>");
            console.debug("====>getSlots data====>" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlots data====>" + JSON.stringify(data));
            try{
                expect(data[0].type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
                expect(data[0].level).assertEqual(notification.SlotLevel.LEVEL_NONE);
                expect(data[0].desc).assertEqual("slot_SOCIAL_COMMUNICATION_desc");
                expect(data[0].badgeFlag).assertEqual(false);
                expect(data[0].bypassDnd).assertEqual(true);
                expect(data[0].vibrationEnabled).assertEqual(true);
                expect(data[0].sound).assertEqual("slot_SOCIAL_COMMUNICATION_sound");
                expect(data[0].lightEnabled).assertEqual(true);
                expect(data[0].lightColor).assertEqual(1);

                expect(data[1].type).assertEqual(notification.SlotType.SERVICE_INFORMATION);
                expect(data[1].level).assertEqual(notification.SlotLevel.LEVEL_MIN);
                expect(data[1].desc).assertEqual("slot_SERVICE_INFORMATION_desc");
                expect(data[1].badgeFlag).assertEqual(false);
                expect(data[1].bypassDnd).assertEqual(true);
                expect(data[1].vibrationEnabled).assertEqual(true);
                expect(data[1].sound).assertEqual("slot_SERVICE_INFORMATION_sound");
                expect(data[1].lightEnabled).assertEqual(true);
                expect(data[1].lightColor).assertEqual(2);

                expect(data[2].type).assertEqual(notification.SlotType.CONTENT_INFORMATION);
                expect(data[2].level).assertEqual(notification.SlotLevel.LEVEL_LOW);
                expect(data[2].desc).assertEqual("slot_CONTENT_INFORMATION_desc");
                expect(data[2].badgeFlag).assertEqual(false);
                expect(data[2].bypassDnd).assertEqual(true);
                expect(data[2].vibrationEnabled).assertEqual(true);
                expect(data[2].sound).assertEqual("slot_CONTENT_INFORMATION_sound");
                expect(data[2].lightEnabled).assertEqual(true);
                expect(data[2].lightColor).assertEqual(3);

                expect(data[3].type).assertEqual(notification.SlotType.OTHER_TYPES);
                expect(data[3].level).assertEqual(notification.SlotLevel.LEVEL_DEFAULT);
                expect(data[3].desc).assertEqual("slot_OTHER_TYPES_desc");
                expect(data[3].badgeFlag).assertEqual(false);
                expect(data[3].bypassDnd).assertEqual(true);
                expect(data[3].vibrationEnabled).assertEqual(true);
                expect(data[3].sound).assertEqual("slot_OTHER_TYPES_sound");
                expect(data[3].lightEnabled).assertEqual(true);
                expect(data[3].lightColor).assertEqual(4);
                console.debug("====>getSlots end====>");
                notification.removeAllSlots((err)=>{
                    console.debug("====>removeAllSlots ActsAnsSlotSystemCallback_0100 err====>" + JSON.stringify(err));
                    console.debug("====>ActsAnsSlotSystemCallback_0100 end====>");
                    expect(err.code).assertEqual(0);
                    done();
                })
            }catch(err){
                console.error("====>getSlots catch err====>" + JSON.stringify(err));
                expect().assertFail();
                done();
            }
        })
        setTimeout(timeOut, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotSystemPromise_0200
     * @tc.name      : Verify that the same type of slot is added repeatedly
     * @tc.desc      : the same type of slot is added repeatedly, and the obtained slot is added for the first time
     */
    it('ActsAnsSlotSystemCallback_0200', 0, async function (done) {
        console.debug("====>ActsAnsSlotSystemCallback_0200 start====>");
        async function timeOutTwo(){
            console.debug("====>time out enter ActsAnsSlotSystemCallback_0200====>");
        }
        console.debug("====>addSlot SOCIAL_COMMUNICATION====>");
        await notification.addSlot(
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: notification.SlotLevel.LEVEL_NONE,
                desc: "slot_SOCIAL_COMMUNICATION_Desc_First",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_SOCIAL_COMMUNICATION_Sound_First",
                lightEnabled: true,
                lightColor: 1
            });
        await notification.addSlot(
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_SOCIAL_COMMUNICATION_Desc_Second",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_SOCIAL_COMMUNICATION_Sound_Second",
                lightEnabled: true,
                lightColor: 1
            });
        console.debug("====>getSlot SlotType.SOCIAL_COMMUNICATION: ====>");
        notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err, data) => {
            console.debug("====>getSlotActsAnsSlotSystemCallback_0200 enter====>");
            console.debug("====>getSlotActsAnsSlotSystemCallback_0200 err====>" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            console.debug("====>getSlotActsAnsSlotSystemCallback_0200 data====>" + JSON.stringify(data));
            expect(data.type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
            expect(data.level).assertEqual(notification.SlotLevel.LEVEL_NONE);
            expect(data.desc).assertEqual("slot_SOCIAL_COMMUNICATION_Desc_First");
            expect(data.badgeFlag).assertEqual(false);
            expect(data.bypassDnd).assertEqual(true);
            expect(data.vibrationEnabled).assertEqual(true);
            expect(data.sound).assertEqual("slot_SOCIAL_COMMUNICATION_Sound_First");
            expect(data.lightEnabled).assertEqual(true);
            expect(data.lightColor).assertEqual(1);
            console.debug("====>getSlotActsAnsSlotSystemCallback_0200 finish====>");
            notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, (err)=>{
                console.debug("====>removeSlot SOCIAL_COMMUNICATION err====>" + JSON.stringify(err));
                console.debug("====>ActsAnsSlotSystemCallback_0200 end====>");
                expect(err.code).assertEqual(0);
                done();
            })
        })
        setTimeout(timeOutTwo, TIMEOUT);
    })
}) 