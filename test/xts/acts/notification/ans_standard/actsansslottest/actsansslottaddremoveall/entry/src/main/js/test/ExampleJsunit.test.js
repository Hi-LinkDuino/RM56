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
describe('ActsAnsSlotAddRemoveAll', function () {

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0100
     * @tc.name      : removeAllSlots after addSlots callback
     * @tc.desc      : Verify that removeAllSlots after addSlots
     */
    it('ActsAnsSlotAddRemoveAll_0100', 0, async function (done) {
        console.debug("====>ActsAnsSlotAddRemoveAll_0100 start====>");
        console.debug("====>addSlots start ActsAnsSlotAddRemoveAll_0100====>");
        notification.addSlots([
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
            }
        ], (err)=>{
            console.debug("====>addSlots callback ActsAnsSlotAddRemoveAll_0100====>");
            expect(err.code).assertEqual(0);
            console.debug("====>getSlots start ActsAnsSlotAddRemoveAll_0100====>");
            notification.getSlots((err, data)=>{
                console.debug("====>getSlots callback ActsAnsSlotAddRemoveAll_0100====>");
                console.debug("====>getSlots err ActsAnsSlotAddRemoveAll_0100====>" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>getSlots data ActsAnsSlotAddRemoveAll_0100====>" + JSON.stringify(data));
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
                    console.debug("====>getSlots end ActsAnsSlotAddRemoveAll_0100====>");
                    console.debug("====>removeAllSlots start ActsAnsSlotAddRemoveAll_0100====>");
                    notification.removeAllSlots((err)=>{
                        console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0100 callback====>");
                        expect(err.code).assertEqual(0);
                        console.debug("====>getSlots start second ActsAnsSlotAddRemoveAll_0100====>");
                        notification.getSlots((err, data)=>{
                            console.debug("====>getSlots second 0100 err====>" + JSON.stringify(err));
                            console.debug("====>getSlots second 0100 data====>" + JSON.stringify(data));
                            console.debug("====>getSlots second 0100 data.length====>"+ data.length);
                            expect(data.length).assertEqual(0);
                            console.debug("====>ActsAnsSlotAddRemoveAll_0100 end====>");
                            done();
                        })
                    })
                }catch(err){
                    console.error("====>getSlots catch err ActsAnsSlotAddRemoveAll_0100====>" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
            })

        })
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0100====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0200
     * @tc.name      : removeAllSlots after addSlots promise
     * @tc.desc      : Verify that removeAllSlots after addSlots
     */
    it('ActsAnsSlotAddRemoveAll_0200', 0, async function (done) {
        console.debug("====>ActsAnsSlotAddRemoveAll_0200 start====>");
        console.debug("====>addSlots start ActsAnsSlotAddRemoveAll_0200====>");
        await notification.addSlots([
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: notification.SlotLevel.LEVEL_NONE,
                desc: "slot_SOCIAL_COMMUNICATION_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_SOCIAL_COMMUNICATION_sound_second",
                lightEnabled: false,
                lightColor: 1
            },
            {
                type: notification.SlotType.SERVICE_INFORMATION,
                level: notification.SlotLevel.LEVEL_MIN,
                desc: "slot_SERVICE_INFORMATION_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_SERVICE_INFORMATION_sound_second",
                lightEnabled: false,
                lightColor: 2
            },
            {
                type: notification.SlotType.CONTENT_INFORMATION,
                level: notification.SlotLevel.LEVEL_LOW,
                desc: "slot_CONTENT_INFORMATION_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_CONTENT_INFORMATION_sound_second",
                lightEnabled: false,
                lightColor: 3
            },
            {
                type: notification.SlotType.OTHER_TYPES,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_OTHER_TYPES_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_OTHER_TYPES_sound_second",
                lightColor: 4
            }
        ]);
        console.debug("====>getSlots start ActsAnsSlotAddRemoveAll_0200====>");
        var data = await notification.getSlots();
        console.debug("====>getSlots ActsAnsSlotAddRemoveAll_0200 data:" + JSON.stringify(data));
        try{
            expect(data[0].type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
            expect(data[0].level).assertEqual(notification.SlotLevel.LEVEL_NONE);
            expect(data[0].desc).assertEqual("slot_SOCIAL_COMMUNICATION_desc_second");
            expect(data[0].badgeFlag).assertEqual(true);
            expect(data[0].bypassDnd).assertEqual(false);
            expect(data[0].vibrationEnabled).assertEqual(false);
            expect(data[0].sound).assertEqual("slot_SOCIAL_COMMUNICATION_sound_second");
            expect(data[0].lightEnabled).assertEqual(false);
            expect(data[0].lightColor).assertEqual(1);

            expect(data[1].type).assertEqual(notification.SlotType.SERVICE_INFORMATION);
            expect(data[1].level).assertEqual(notification.SlotLevel.LEVEL_MIN);
            expect(data[1].desc).assertEqual("slot_SERVICE_INFORMATION_desc_second");
            expect(data[1].badgeFlag).assertEqual(true);
            expect(data[1].bypassDnd).assertEqual(false);
            expect(data[1].vibrationEnabled).assertEqual(false);
            expect(data[1].sound).assertEqual("slot_SERVICE_INFORMATION_sound_second");
            expect(data[1].lightEnabled).assertEqual(false);
            expect(data[1].lightColor).assertEqual(2);

            expect(data[2].type).assertEqual(notification.SlotType.CONTENT_INFORMATION);
            expect(data[2].level).assertEqual(notification.SlotLevel.LEVEL_LOW);
            expect(data[2].desc).assertEqual("slot_CONTENT_INFORMATION_desc_second");
            expect(data[2].badgeFlag).assertEqual(true);
            expect(data[2].bypassDnd).assertEqual(false);
            expect(data[2].vibrationEnabled).assertEqual(false);
            expect(data[2].sound).assertEqual("slot_CONTENT_INFORMATION_sound_second");
            expect(data[2].lightEnabled).assertEqual(false);
            expect(data[2].lightColor).assertEqual(3);

            expect(data[3].type).assertEqual(notification.SlotType.OTHER_TYPES);
            expect(data[3].level).assertEqual(notification.SlotLevel.LEVEL_DEFAULT);
            expect(data[3].desc).assertEqual("slot_OTHER_TYPES_desc_second");
            expect(data[3].badgeFlag).assertEqual(true);
            expect(data[3].bypassDnd).assertEqual(false);
            expect(data[3].vibrationEnabled).assertEqual(false);
            expect(data[3].sound).assertEqual("slot_OTHER_TYPES_sound_second");
            expect(data[3].lightColor).assertEqual(4);
            console.debug("====>getSlots end ActsAnsSlotAddRemoveAll_0200====>");
        }catch(err){
            console.error("====>getSlots catch err ActsAnsSlotAddRemoveAll_0200====>" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>removeAllSlots start ActsAnsSlotAddRemoveAll_0200====>");
        await notification.removeAllSlots();
        console.debug("====>ActsAnsSlotAddRemoveAll_0200 end====>");
        done();
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0200====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0300
     * @tc.name      : removeAllSlots after addSlots repeatedly callback
     * @tc.desc      : Verify that removeAllSlots after addSlots repeatedly
     */
    it('ActsAnsSlotAddRemoveAll_0300', 0, async function (done) {
        console.debug("====>ActsAnsSlotAddRemoveAll_0300 start====>");
        console.debug("====>addSlots start ActsAnsSlotAddRemoveAll_0300====>");
        notification.addSlots([
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
            }
        ], (err)=>{
            console.debug("====>addSlots callback ActsAnsSlotAddRemoveAll_0300====>");
            expect(err.code).assertEqual(0);
            console.debug("====>addSlots second time ActsAnsSlotAddRemoveAll_0300====>");
            notification.addSlots([
                {
                    type: notification.SlotType.SOCIAL_COMMUNICATION,
                    level: notification.SlotLevel.LEVEL_NONE,
                    desc: "slot_SOCIAL_COMMUNICATION_desc_twice",
                    badgeFlag: true,
                    bypassDnd: true,
                    vibrationEnabled: false,
                    sound: "slot_SOCIAL_COMMUNICATION_sound_twice",
                    lightEnabled: true,
                    lightColor: 5
                },
                {
                    type: notification.SlotType.SERVICE_INFORMATION,
                    level: notification.SlotLevel.LEVEL_MIN,
                    desc: "slot_SERVICE_INFORMATION_desc",
                    badgeFlag: false,
                    bypassDnd: false,
                    vibrationEnabled: true,
                    sound: "slot_SERVICE_INFORMATION_sound_twice",
                    lightEnabled: true,
                    lightColor: 6
                },
                {
                    type: notification.SlotType.CONTENT_INFORMATION,
                    level: notification.SlotLevel.LEVEL_LOW,
                    desc: "slot_CONTENT_INFORMATION_desc_twice",
                    badgeFlag: false,
                    bypassDnd: true,
                    vibrationEnabled: true,
                    sound: "slot_CONTENT_INFORMATION_sound_twice",
                    lightEnabled: false,
                    lightColor: 7
                },
                {
                    type: notification.SlotType.OTHER_TYPES,
                    level: notification.SlotLevel.LEVEL_DEFAULT,
                    desc: "slot_OTHER_TYPES_desc_twice",
                    badgeFlag: true,
                    bypassDnd: true,
                    vibrationEnabled: true,
                    sound: "slot_OTHER_TYPES_sound_twice",
                    lightEnabled: true,
                    lightColor: 8
                }
            ], (err)=>{
                console.debug("====>addSlots twice callback ActsAnsSlotAddRemoveAll_0300====>");
                expect(err.code).assertEqual(0);
                console.debug("====>getSlots start ActsAnsSlotAddRemoveAll_0300====>");
                notification.getSlots((err, data)=>{
                    console.debug("====>getSlots callback ActsAnsSlotAddRemoveAll_0300====>");
                    console.debug("====>getSlots err ActsAnsSlotAddRemoveAll_0300====>" + JSON.stringify(err));
                    expect(err.code).assertEqual(0);
                    console.debug("====>getSlots data ActsAnsSlotAddRemoveAll_0300====>" + JSON.stringify(data));
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
                        console.debug("====>getSlots end ActsAnsSlotAddRemoveAll_0300====>");
                        console.debug("====>removeAllSlots start ActsAnsSlotAddRemoveAll_0300====>");
                        notification.removeAllSlots((err)=>{
                            console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0300 callback====>");
                            expect(err.code).assertEqual(0);
                            console.debug("====>getSlots start second ActsAnsSlotAddRemoveAll_0300====>");
                            notification.getSlots((err, data)=>{
                                console.debug("====>getSlots second 0300 err====>" + JSON.stringify(err));
                                console.debug("====>getSlots second 0300 data====>" + JSON.stringify(data));
                                console.debug("====>getSlots second 0300 data.length====>"+ data.length);
                                expect(data.length).assertEqual(0);
                                console.debug("====>ActsAnsSlotAddRemoveAll_0300 end====>");
                                done();
                            })
                        })
                    }catch(err){
                        console.error("====>getSlots catch err 0300====>" + JSON.stringify(err));
                        expect().assertFail();
                        done();
                    }
                })
            })

        })
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0300====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0400
     * @tc.name      : removeAllSlots after addSlots repeatedly promise
     * @tc.desc      : Verify that removeAllSlots after addSlots repeatedly
     */
    it('ActsAnsSlotAddRemoveAll_0400', 0, async function (done) {
        console.debug("====>ActsAnsSlotAddRemoveAll_0400 start====>");
        console.debug("====>addSlots start ActsAnsSlotAddRemoveAll_0400====>");
        await notification.addSlots([
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: notification.SlotLevel.LEVEL_NONE,
                desc: "slot_SOCIAL_COMMUNICATION_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_SOCIAL_COMMUNICATION_sound_second",
                lightEnabled: false,
                lightColor: 1
            },
            {
                type: notification.SlotType.SERVICE_INFORMATION,
                level: notification.SlotLevel.LEVEL_MIN,
                desc: "slot_SERVICE_INFORMATION_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_SERVICE_INFORMATION_sound_second",
                lightEnabled: false,
                lightColor: 2
            },
            {
                type: notification.SlotType.CONTENT_INFORMATION,
                level: notification.SlotLevel.LEVEL_LOW,
                desc: "slot_CONTENT_INFORMATION_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_CONTENT_INFORMATION_sound_second",
                lightEnabled: false,
                lightColor: 3
            },
            {
                type: notification.SlotType.OTHER_TYPES,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_OTHER_TYPES_desc_second",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_OTHER_TYPES_sound_second",
                lightColor: 4
            }
        ]);
        console.debug("====>addSlots twice start ActsAnsSlotAddRemoveAll_0400====>");
        await notification.addSlots([
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                level: notification.SlotLevel.LEVEL_NONE,
                desc: "slot_SOCIAL_COMMUNICATION_desc_second_twice",
                badgeFlag: false,
                bypassDnd: false,
                vibrationEnabled: false,
                sound: "slot_SOCIAL_COMMUNICATION_sound_second_twice",
                lightEnabled: true,
                lightColor: 1
            },
            {
                type: notification.SlotType.SERVICE_INFORMATION,
                level: notification.SlotLevel.LEVEL_MIN,
                desc: "slot_SERVICE_INFORMATION_desc_second_twice",
                badgeFlag: true,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_SERVICE_INFORMATION_sound_second_twice",
                lightEnabled: true,
                lightColor: 2
            },
            {
                type: notification.SlotType.CONTENT_INFORMATION,
                level: notification.SlotLevel.LEVEL_LOW,
                desc: "slot_CONTENT_INFORMATION_desc_second_twice",
                badgeFlag: true,
                bypassDnd: false,
                vibrationEnabled: true,
                sound: "slot_CONTENT_INFORMATION_sound_second_twice",
                lightEnabled: false,
                lightColor: 3
            },
            {
                type: notification.SlotType.OTHER_TYPES,
                level: notification.SlotLevel.LEVEL_DEFAULT,
                desc: "slot_OTHER_TYPES_desc_second_twice",
                badgeFlag: false,
                bypassDnd: true,
                vibrationEnabled: true,
                sound: "slot_OTHER_TYPES_sound_second_twice",
                lightColor: 4
            }
        ]);
        console.debug("====>getSlots start ActsAnsSlotAddRemoveAll_0400====>");
        var data = await notification.getSlots();
        console.debug("====>getSlots enter ActsAnsSlotAddRemoveAll_0400====>");
        console.debug("====>getSlots data ActsAnsSlotAddRemoveAll_0400====>" + JSON.stringify(data));
        try{
            expect(data[0].type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
            expect(data[0].level).assertEqual(notification.SlotLevel.LEVEL_NONE);
            expect(data[0].desc).assertEqual("slot_SOCIAL_COMMUNICATION_desc_second");
            expect(data[0].badgeFlag).assertEqual(true);
            expect(data[0].bypassDnd).assertEqual(false);
            expect(data[0].vibrationEnabled).assertEqual(false);
            expect(data[0].sound).assertEqual("slot_SOCIAL_COMMUNICATION_sound_second");
            expect(data[0].lightEnabled).assertEqual(false);
            expect(data[0].lightColor).assertEqual(1);

            expect(data[1].type).assertEqual(notification.SlotType.SERVICE_INFORMATION);
            expect(data[1].level).assertEqual(notification.SlotLevel.LEVEL_MIN);
            expect(data[1].desc).assertEqual("slot_SERVICE_INFORMATION_desc_second");
            expect(data[1].badgeFlag).assertEqual(true);
            expect(data[1].bypassDnd).assertEqual(false);
            expect(data[1].vibrationEnabled).assertEqual(false);
            expect(data[1].sound).assertEqual("slot_SERVICE_INFORMATION_sound_second");
            expect(data[1].lightEnabled).assertEqual(false);
            expect(data[1].lightColor).assertEqual(2);

            expect(data[2].type).assertEqual(notification.SlotType.CONTENT_INFORMATION);
            expect(data[2].level).assertEqual(notification.SlotLevel.LEVEL_LOW);
            expect(data[2].desc).assertEqual("slot_CONTENT_INFORMATION_desc_second");
            expect(data[2].badgeFlag).assertEqual(true);
            expect(data[2].bypassDnd).assertEqual(false);
            expect(data[2].vibrationEnabled).assertEqual(false);
            expect(data[2].sound).assertEqual("slot_CONTENT_INFORMATION_sound_second");
            expect(data[2].lightEnabled).assertEqual(false);
            expect(data[2].lightColor).assertEqual(3);

            expect(data[3].type).assertEqual(notification.SlotType.OTHER_TYPES);
            expect(data[3].level).assertEqual(notification.SlotLevel.LEVEL_DEFAULT);
            expect(data[3].desc).assertEqual("slot_OTHER_TYPES_desc_second");
            expect(data[3].badgeFlag).assertEqual(true);
            expect(data[3].bypassDnd).assertEqual(false);
            expect(data[3].vibrationEnabled).assertEqual(false);
            expect(data[3].sound).assertEqual("slot_OTHER_TYPES_sound_second");
            expect(data[3].lightColor).assertEqual(4);
            console.debug("====>getSlots end ActsAnsSlotAddRemoveAll_0400====>");
        }catch(err){
            console.error("====>getSlots catch err ActsAnsSlotAddRemoveAll_0400====>" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        console.debug("====>removeAllSlots start ActsAnsSlotAddRemoveAll_0400====>");
        await notification.removeAllSlots();
        console.debug("====>ActsAnsSlotAddRemoveAll_0400 end====>");
        done();
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0400====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0500
     * @tc.name      : removeAllSlots callback
     * @tc.desc      : Verify that removeAllSlots without adding slots
     */
    it('ActsAnsSlotAddRemoveAll_0500', 0, async function (done) {
        console.debug("====>removeAllSlots start ActsAnsSlotAddRemoveAll_0500====>");
        notification.removeAllSlots((err)=>{
            console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0500 err:" + JSON.stringify(err));
            expect(err.code).assertEqual(0);
            done();
        })
        console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0500====>");
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0500====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0600
     * @tc.name      : removeAllSlots promise
     * @tc.desc      : Verify that removeAllSlots without adding slots
     */
    it('ActsAnsSlotAddRemoveAll_0600', 0, async function (done) {
        console.debug("====>removeAllSlots start ActsAnsSlotAddRemoveAll_0600====>");
        notification.removeAllSlots().then(()=>{
            console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0600====>");
            done();
        }).catch((err)=>{
            console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0600 err:" + JSON.stringify(err));
            expect().assertFail();
            done();
        })
        console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0600====>");
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0600====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0700
     * @tc.name      : addSlots callback
     * @tc.desc      : Verify that addSlots with same type of slot
     */
    it('ActsAnsSlotAddRemoveAll_0700', 0, async function (done) {
        console.debug("====>start ActsAnsSlotAddRemoveAll_0700====>");
        await notification.addSlots([
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                desc: "slot_SOCIAL_COMMUNICATION_desc_first"
            },
            {
                type: notification.SlotType.SOCIAL_COMMUNICATION,
                desc: "slot_SERVICE_INFORMATION_desc_second"
            }
        ], (err)=>{
            console.debug("====>addSlots callback ActsAnsSlotAddRemoveAll_0700====>");
            expect(err.code).assertEqual(0);
            console.debug("====>getSlots ActsAnsSlotAddRemoveAll_0700====>");
            notification.getSlots((err, data)=>{
                console.debug("====>getSlots enter ActsAnsSlotAddRemoveAll_0700====>");
                console.debug("====>getSlots err ActsAnsSlotAddRemoveAll_0700====>" + JSON.stringify(err));
                expect(err.code).assertEqual(0);
                console.debug("====>getSlots data ActsAnsSlotAddRemoveAll_0700====>" + JSON.stringify(data));
                try{
                    console.debug("====>getSlots ActsAnsSlotAddRemoveAll_0700 data.length:====>" + data.length);
                    expect(data.length).assertEqual(1);
                    expect(data[0].type).assertEqual(notification.SlotType.SOCIAL_COMMUNICATION);
                    expect(data[0].desc).assertEqual("slot_SERVICE_INFORMATION_desc_second");
                    console.debug("====>getSlots end ActsAnsSlotAddRemoveAll_0700====>");
                    notification.removeAllSlots((err)=>{
                        console.debug("====>removeAllSlots ActsAnsSlotAddRemoveAll_0700 err:" + JSON.stringify(err));
                        console.debug("====>ActsAnsSlotAddRemoveAll_0700 end====>");
                        expect(err.code).assertEqual(0);
                        done();
                    })
                }catch(err){
                    console.error("====>getSlots catch err ActsAnsSlotAddRemoveAll_0700====>" + JSON.stringify(err));
                    expect().assertFail();
                    done();
                }
            })
        })
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0700====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsSlotAddRemoveAll_0800
     * @tc.name      : addSlots promise
     * @tc.desc      : Verify that addSlots with same type of slot
     */
    it('ActsAnsSlotAddRemoveAll_0800', 0, async function (done) {
        console.debug("====>start ActsAnsSlotAddRemoveAll_0800====>");
        await notification.addSlots([
            {
                type: notification.SlotType.OTHER_TYPES,
                desc: "slot_SOCIAL_COMMUNICATION_desc_first"
            },
            {
                type: notification.SlotType.OTHER_TYPES,
                desc: "slot_SERVICE_INFORMATION_desc_second"
            }
        ]);
        console.debug("====>getSlots ActsAnsSlotAddRemoveAll_0800====>");
        var data = await notification.getSlots();
        console.debug("====>getSlots ActsAnsSlotAddRemoveAll_0800 data:" + JSON.stringify(data));
        try{
            console.debug("====>getSlots ActsAnsSlotAddRemoveAll_0800 data.length:====>" + data.length);
            expect(data.length).assertEqual(1);
            expect(data[0].type).assertEqual(notification.SlotType.OTHER_TYPES);
            expect(data[0].desc).assertEqual("slot_SERVICE_INFORMATION_desc_second");
            console.debug("====>getSlots end ActsAnsSlotAddRemoveAll_0800====>");
            await notification.removeAllSlots();
            console.debug("====>ActsAnsSlotAddRemoveAll_0800 end====>");
            done();
        }catch(err){
            console.error("====>getSlots err ActsAnsSlotAddRemoveAll_0800====>" + JSON.stringify(err));
            expect().assertFail();
            done();
        }
        setTimeout(function (){
            console.debug("====>time out ActsAnsSlotAddRemoveAll_0800====>");
        }, TIMEOUT);
    })
})