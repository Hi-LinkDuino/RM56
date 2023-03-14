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
import Subscriber from '@ohos.commonevent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var time = 5000
var multiLineDataOne
var multiLineDataTwo
var LongContentDataOne
var LongContentDataTwo
var SlotTypeDataOne
var SlotTypeDataTwo
var SlotTypeDataThree
var SlotTypeDataFour
var SlotTypeDataFive
var SlotTypeDataSix
var OtherDataOne
var OtherDataTwo
var OtherDataThree
var OtherDataFour
var OtherDataFive
var OtherDataSix
var ServiceDataOne
var ServiceDataTwo
var ServiceDataThree
var ServiceDataFour
var ServiceDataFive
var ServiceDataSix
var SocialDataOne
var SocialDataTwo
var SocialDataThree
var SocialDataFour
var SocialDataFive
var SocialDataSix
describe('ActsAnsNotificationTest', function () {
    function publishMULTILINEContentCallback001(error){
        console.log('ActsNotificationTest ACTS_PublishMULTILINEContent_0100 asyncCallback'+JSON.stringify(error.code))
    }
    function publishMULTILINEContentCallback002(error){
        console.log('ActsNotificationTest ACTS_PublishMULTILINEContent_0200 asyncCallback'+JSON.stringify(error.code))
    }
    function publishMULTILINEContentCallback003(error){
        console.log('ActsNotificationTest ACTS_PublishMULTILINEContent_0300 asyncCallback'+JSON.stringify(error.code))
    }
    function publishLONGContentCallback001(error){
        console.log('ActsNotificationTest ACTS_PublishLONGContent_0100 asyncCallback'+JSON.stringify(error.code))
    }
    function publishLONGContentCallback002(error){
        console.log('ActsNotificationTest ACTS_PublishLONGContent_0200 asyncCallback'+JSON.stringify(error.code))
    }
    function publishLONGContentCallback003(error){
        console.log('ActsNotificationTest ACTS_PublishLONGContent_0300 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotTypeContentCallback001(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeContent_0100 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotTypeContentCallback002(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeContent_0200 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotTypeContentCallback003(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeContent_0300 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotTypeContentCallback004(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeContent_0400 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotOtherCallback001(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeOther_0100 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotOtherCallback002(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeOther_0200 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotOtherCallback003(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeOther_0300 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotOtherCallback004(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeOther_0400 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotServiceCallback001(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0100 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotServiceCallback002(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0200 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotServiceCallback003(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0300 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotServiceCallback004(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0400 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotSocialCallback001(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0400 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotSocialCallback002(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0400 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotSocialCallback003(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0400 asyncCallback'+JSON.stringify(error.code))
    }
    function publishSlotSocialCallback004(error){
        console.log('ActsNotificationTest ACTS_Publish_SlotTypeService_0400 asyncCallback'+JSON.stringify(error.code))
    }
    function UnSubscribeCallBacka(err){
        console.log('===UnSubscribeCallBacka==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackb(err){
        console.log('===UnSubscribeCallBackb==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackc(err){
        console.log('===UnSubscribeCallBackc==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackd(err){
        console.log('===UnSubscribeCallBackd==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBacke(err){
        console.log('===UnSubscribeCallBacke==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackf(err){
        console.log('===UnSubscribeCallBackf==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackg(err){
        console.log('===UnSubscribeCallBackg==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackh(err){
        console.log('===UnSubscribeCallBackh==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBacki(err){
        console.log('===UnSubscribeCallBacki==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackj(err){
        console.log('===UnSubscribeCallBackj==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackl(err){
        console.log('===UnSubscribeCallBackl==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackm(err){
        console.log('===UnSubscribeCallBackm==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackn(err){
        console.log('===UnSubscribeCallBackn==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBacko(err){
        console.log('===UnSubscribeCallBacko==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackp(err){
        console.log('===UnSubscribeCallBackp==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackq(err){
        console.log('===UnSubscribeCallBackq==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackr(err){
        console.log('===UnSubscribeCallBackr==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBacks(err){
        console.log('===UnSubscribeCallBacks==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackt(err){
        console.log('===UnSubscribeCallBackt==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBacku(err){
        console.log('===UnSubscribeCallBacku==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackv(err){
        console.log('===UnSubscribeCallBackv==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackw(err){
        console.log('===UnSubscribeCallBackw==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackx(err){
        console.log('===UnSubscribeCallBackx==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBacky(err){
        console.log('===UnSubscribeCallBacky==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackz(err){
        console.log('===UnSubscribeCallBackz==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackA(err){
        console.log('===UnSubscribeCallBackA==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackB(err){
        console.log('===UnSubscribeCallBackB==='+err.code)
        expect(err.code).assertEqual(0)
    }
    function UnSubscribeCallBackC(err){
        console.log('===UnSubscribeCallBackC==='+err.code)
        expect(err.code).assertEqual(0)
    }
    /*
     * @tc.number: ACTS_PublishMULTILINEContent_0100
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_MULTILINE)
     */
    it('ACTS_PublishMULTILINEContent_0100', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishMULTILINEContent_0100"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo
            , async (err,data) =>{
            console.info("===ACTS_PublishMULTILINEContent_0100===createSubscriber"+JSON.stringify(data));
            multiLineDataOne = data
            await Subscriber.subscribe(data, subMultilineOneCallBack)
        })
        function subMultilineOneCallBack(err,data){
            console.log('===subMultilineOneCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(multiLineDataOne,UnSubscribeCallBacka)
            if(data.code == 1) {
                expect(data.code).assertEqual(1);
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
           await notification.publish({
               id: 1,
               content: {
                   contentType: notification.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                   multiLine: {
                       title: "test1_title",
                       text: "test1_text",
                       additionalText: "test1_additionalText",
                       briefText: "briefText1",
                       longTitle: "longTitle1",
                       lines: ["thrive", "democracy", "civilization", "harmonious"]
                   },
               }
           }, async (err)=>{
               console.debug("====ACTS_PublishMULTILINEContent_0100===Callback==>")
           });

    })

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish()
     */
    it('ACTS_PublishMULTILINEContent_0200', 0,async function (done) {
           var notificationInfo = {
               id: 2,
               content: {
               //  contentType: notification.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                   multiLine: {
                       title: "test_title",
                       text: "test_text",
                       additionalText: "test_additionalText",
                       briefText: "briefText",
                       longTitle: "longTitle",
                       lines: ["thrive", "democracy", "civilization", "harmonious"]
                   },
               }
           }
           await notification.publish(notificationInfo,publishMULTILINEContentCallback002)
        done();
    })
    /*
     * @tc.number: ACTS_PublishMULTILINEContent_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_PICTURE)
     */
    it('ACTS_PublishMULTILINEContent_0300', 0,async function (done) {
        try {
            await notification.publish({
                id: 3,
                content: {
                    contentType: notification.ContentType.NOTIFICATION_CONTENT_PICTURE,
                    multiLine: {
                        title: "test_title",
                        text: "test_text",
                        additionalText: "test_additionalText",
                        briefText: "briefText",
                        longTitle: "longTitle",
                        lines: ["thrive", "democracy", "civilization", "harmonious"]
                    },
                }
            }, publishMULTILINEContentCallback003);
        }catch(error){
            console.log('ActsNotificationTest ACTS_PublishMULTILINEContent_0300 asyncCallback'+JSON.stringify(error.code))
        }
        done();
    })

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_0400
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_MULTILINE) promise
     */
    it('ACTS_PublishMULTILINEContent_0400', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishMULTILINEContent_0400"]
        };
        await Subscriber.createSubscriber(commonEventSubscribeInfo).then(async(data)=> {
            console.info("===ACTS_PublishMULTILINEContent_0400===createSubscriber" + JSON.stringify(data));
            multiLineDataTwo = data
            await Subscriber.subscribe(data, subMultilineTwoCallBack)
        })
        console.info("===ACTS_PublishMULTILINEContent_0400===subMultilineTwoCallBack");
       async function subMultilineTwoCallBack(err,data){
            console.log('===subMultilineTwoCallBack==='+err.code+JSON.stringify(data))
            await Subscriber.unsubscribe(multiLineDataTwo,UnSubscribeCallBackb)
            if(data.code == 4) {
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 4,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                multiLine: {
                    title: "test4_title",
                    text: "test4_text",
                    additionalText: "test4_additionalText",
                    briefText:"briefText4",
                    longTitle:"longTitle4",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            }
        }).then(console.debug("===ACTS_PublishMULTILINEContent_0400===Promise===>"))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishMULTILINEContent_0400====>");
        }, time);
    })
    /*
     * @tc.number: ACTS_PublishMULTILINEContent_0500
     * @tc.name: publish()
     * @tc.desc: verify the function of publish() promise
     */
    it('ACTS_PublishMULTILINEContent_0500', 0,async function (done) {
        var promise = notification.publish({
            id: 5,
            content: {
            //  contentType: notification.ContentType.NOTIFICATION_CONTENT_MULTILINE,
                multiLine: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    briefText:"briefText",
                    longTitle:"longTitle",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            }
        })
        expect(promise).assertEqual(undefined)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishMULTILINEContent_0500====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishMULTILINEContent_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_PICTURE) promise
     */
    it('ACTS_PublishMULTILINEContent_0600', 0,async function (done) {
        var promise = notification.publish({
            id: 6,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_PICTURE,
                multiLine: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    briefText:"briefText",
                    longTitle:"longTitle",
                    lines: ["thrive", "democracy", "civilization", "harmonious"]
                },
            }
        })
        expect(promise).assertEqual(undefined)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishMULTILINEContent_0600====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishLONGContent_0100
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_LONG_TEXT)
     */
    it('ACTS_PublishLONGContent_0100', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishLONGContent_0100"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_PublishLONGContent_0100===createSubscriber"+JSON.stringify(data));
            LongContentDataOne = data
            await Subscriber.subscribe(data, subLoneOneCallBack)
        })
        function subLoneOneCallBack(err,data){
            console.log('===subLoneOneCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(LongContentDataOne,UnSubscribeCallBackc)
            if(data.code == 7) {
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 7,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test7_title",
                    text: "test7_text",
                    additionalText: "test7_additionalText",
                    longText:"longText7",
                    briefText:"briefText7",
                    expandedTitle:"expandedTitle7"
                }}
        },(err)=>{
            console.debug("===ACTS_PublishLONGContent_0100===Callback===>");
        });
    })

    /*
     * @tc.number: ACTS_PublishLONGContent_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish()
     */
    it('ACTS_PublishLONGContent_0200', 0,async function (done) {
        await notification.publish({
            id: 8,
            content: {
            //  contentType: notification.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText:"longText",
                    briefText:"briefText",
                    expandedTitle:"expandedTitle"
                }}
        },publishLONGContentCallback002);
        console.log("===ACTS_PublishLONGContent_0200 finished===")
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishLONGContent_0200====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishLONGContent_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_MEDIA)
     */
    it('ACTS_PublishLONGContent_0300', 0,async function (done) {
        await notification.publish({
            id: 9,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_MEDIA,
                longText: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText:"longText",
                    briefText:"briefText",
                    expandedTitle:"expandedTitle"
                }}
        },publishLONGContentCallback003);
        console.log("===ACTS_PublishLONGContent_0300 finished===")
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishLONGContent_0300====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishLONGContent_0400
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_LONG_TEXT) promise
     */
    it('ACTS_PublishLONGContent_0400', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishLONGContent_0400"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishLONGContent_0400===createSubscriber"+JSON.stringify(data));
            LongContentDataTwo = data
            await Subscriber.subscribe(data, subLoneTwoCallBack)
        })
        function subLoneTwoCallBack(err,data){
            console.log('===subLoneTwoCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(LongContentDataTwo,UnSubscribeCallBackd)
            if(data.code == 10) {
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 10,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test10_title",
                    text: "test10_text",
                    additionalText: "test10_additionalText",
                    longText:"longText10",
                    briefText:"briefText10",
                    expandedTitle:"expandedTitle10"
                }}
        }).then(console.log("===ACTS_PublishLONGContent_0400 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishLONGContent_0400====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishLONGContent_0500
     * @tc.name: publish()
     * @tc.desc: verify the function of publish() promise
     */
    it('ACTS_PublishLONGContent_0500', 0,async function (done) {
        var promise = await notification.publish({
            id: 11,
            content: {
            //                contentType: notification.ContentType.NOTIFICATION_CONTENT_LONG_TEXT,
                longText: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText:"longText",
                    briefText:"briefText",
                    expandedTitle:"expandedTitle"
                }}
        })
        expect(promise).assertEqual(undefined)
        done();
    })

    /*
     * @tc.number: ACTS_PublishLONGContent_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_MEDIA) promise
     */
    it('ACTS_PublishLONGContent_0600', 0,async function (done) {
        var promise = notification.publish({
            id: 12,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_MEDIA,
                longText: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText",
                    longText:"longText",
                    briefText:"briefText",
                    expandedTitle:"expandedTitle"
                }}
        })
        expect(promise).assertEqual(undefined)
        done();
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0100
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:CONTENT_INFORMATION)
     */
    it('ACTS_Publish_SlotTypeContent_0100', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_Publish_SlotTypeContent_0100"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_Publish_SlotTypeContent_0100===createSubscriber"+JSON.stringify(data));
            SlotTypeDataOne = data
            await Subscriber.subscribe(data, subSlotTypeOneCallBack)
        })
        function subSlotTypeOneCallBack(err,data){
            console.log('===subSlotTypeOneCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SlotTypeDataOne,UnSubscribeCallBacke)
            if(data.code == 13) {
                expect(data.code).assertEqual(13)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 13,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test13_title",
                    text: "test13_text",
                    additionalText: "test13_additionalText"
                }
            },
            slotType:notification.SlotType.CONTENT_INFORMATION
        },publishSlotTypeContentCallback001);
        console.log("===ACTS_Publish_SlotTypeContent_0100 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0100====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:CONTENT_INFORMATION)
     */
    it('ACTS_Publish_SlotTypeContent_0200', 0,async function (done) {
        await notification.publish({
            id: 14,
            content: {
            //  contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                }
            },
            slotType:notification.SlotType.CONTENT_INFORMATION
        },publishSlotTypeContentCallback002);
        console.log("===ACTS_Publish_SlotTypeContent_0200 finished===")
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0200====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)
     */
    it('ACTS_Publish_SlotTypeContent_0300', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_Publish_SlotTypeContent_0300"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_Publish_SlotTypeContent_0300===createSubscriber"+JSON.stringify(data));
            SlotTypeDataTwo = data
            await Subscriber.subscribe(data, subSlotTypeTwoCallBack)
        })
        function subSlotTypeTwoCallBack(err,data){
            console.log('===subSlotTypeTwoCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SlotTypeDataTwo,UnSubscribeCallBackf)
            if(data.code == 15) {
                expect(data.code).assertEqual(15)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 15,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test15_title",
                    text: "test15_text",
                    additionalText: "test_additionalText"
                }
            },
               //slotType:notification.SlotType.CONTENT_INFORMATION
        },publishSlotTypeContentCallback003);
        console.log("===ACTS_Publish_SlotTypeContent_0300 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0300====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0400
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SERVICE_INFORMATION)
     */
    it('ACTS_Publish_SlotTypeContent_0400', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_Publish_SlotTypeContent_0400"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_Publish_SlotTypeContent_0400===createSubscriber"+JSON.stringify(data));
            SlotTypeDataThree = data
            await Subscriber.subscribe(data, subSlotTypeThreeCallBack)
        })
        function subSlotTypeThreeCallBack(err,data){
            console.log('===subSlotTypeThreeCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SlotTypeDataThree,UnSubscribeCallBackg)
            if(data.code == 16) {
                expect(data.code).assertEqual(16)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 16,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test16_title",
                    text: "test16_text",
                    additionalText: "test16_additionalText"
                }
            },
            slotType:notification.SlotType.SERVICE_INFORMATION
        },publishSlotTypeContentCallback004);
        console.log("===ACTS_Publish_SlotTypeContent_0400 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0400====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0500
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:CONTENT_INFORMATION)  promise
     */
    it('ACTS_Publish_SlotTypeContent_0500', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_Publish_SlotTypeContent_0500"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_Publish_SlotTypeContent_0500===createSubscriber"+JSON.stringify(data));
            SlotTypeDataFour = data
            await Subscriber.subscribe(data, subSlotTypeFourCallBack)
        })
        function subSlotTypeFourCallBack(err,data){
            console.log('===subSlotTypeFourCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SlotTypeDataFour,UnSubscribeCallBackh)
            if(data.code == 17) {
                expect(data.code).assertEqual(17)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 17,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test17_title",
                    text: "test17_text",
                    additionalText: "test17_additionalText"
                }
            },
            slotType:notification.SlotType.CONTENT_INFORMATION
        }).then( console.log("===ACTS_Publish_SlotTypeContent_0500 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0500====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:CONTENT_INFORMATION)  promise
     */
    it('ACTS_Publish_SlotTypeContent_0600', 0,async function (done) {
        var promise = notification.publish({
            id: 18,
            content: {
            //  contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                }
            },
            slotType:notification.SlotType.CONTENT_INFORMATION
        })
        expect(promise).assertEqual(undefined)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0600====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0700
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)  promise
     */
    it('ACTS_Publish_SlotTypeContent_0700', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_Publish_SlotTypeContent_0700"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_Publish_SlotTypeContent_0700===createSubscriber"+JSON.stringify(data));
            SlotTypeDataFive = data
            await Subscriber.subscribe(data, subSlotTypeFiveCallBack)
        })
        function subSlotTypeFiveCallBack(err,data){
            console.log('===subSlotTypeFiveCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SlotTypeDataFive,UnSubscribeCallBacki)
            if(data.code == 19) {
                expect(data.code).assertEqual(19)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 19,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test19_title",
                    text: "test19_text",
                    additionalText: "test19_additionalText"
                }
            },
                //slotType:notification.SlotType.CONTENT_INFORMATION
        }).then(console.log("===ACTS_Publish_SlotTypeContent_0700 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0700====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_Publish_SlotTypeContent_0800
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SERVICE_INFORMATION) promise
     */
    it('ACTS_Publish_SlotTypeContent_0800', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_Publish_SlotTypeContent_0800"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_Publish_SlotTypeContent_0800===createSubscriber"+JSON.stringify(data));
            SlotTypeDataSix = data
            await Subscriber.subscribe(data, subSlotTypeSixCallBack)
        })
        function subSlotTypeSixCallBack(err,data){
            console.log('===subSlotTypeSixCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SlotTypeDataSix,UnSubscribeCallBackj)
            if(data.code == 20) {
                expect(data.code).assertEqual(20)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 20,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test20_title",
                    text: "test20_text",
                    additionalText: "test20_additionalText"
                }
            },
            slotType:notification.SlotType.SERVICE_INFORMATION
        }).then(console.log("===ACTS_Publish_SlotTypeContent_0800 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_Publish_SlotTypeContent_0800====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0100
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:OTHER_TYPES)
     */
    it('ACTS_PublishSlotTypeOther_0100', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeOther_0100"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeOther_0100===createSubscriber"+JSON.stringify(data));
            OtherDataOne = data
            await Subscriber.subscribe(data, subOtherOneCallBack)
        })
        function subOtherOneCallBack(err,data){
            console.log('===subOtherOneCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(OtherDataOne,UnSubscribeCallBackl)
            if(data.code == 21) {
                expect(data.code).assertEqual(21)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id:21,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test21_title",
                    text: "test21_text",
                    additionalText: "test21_additionalText"
                },
                slotType:notification.SlotType.OTHER_TYPES
            }
        },publishSlotOtherCallback001);
        console.log("===ACTS_PublishSlotTypeOther_0100 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0100====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:OTHER_TYPES)
     */
    it('ACTS_PublishSlotTypeOther_0200', 0,async function (done) {
        await notification.publish({
            id:22,
            content: {
            //  contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
                slotType:notification.SlotType.OTHER_TYPES
            }
        },publishSlotOtherCallback002);
        console.log("===ACTS_PublishSlotTypeOther_0200 finished===")
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0200====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)
     */
    it('ACTS_PublishSlotTypeOther_0300', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeOther_0300"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeOther_0300===createSubscriber"+JSON.stringify(data));
            OtherDataTwo = data
            await Subscriber.subscribe(data, subOtherTwoCallBack)
        })
        function subOtherTwoCallBack(err,data){
            console.log('===subOtherTwoCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(OtherDataTwo,UnSubscribeCallBackm)
            if(data.code == 23) {
                expect(data.code).assertEqual(23)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id:23,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test23_title",
                    text: "test23_text",
                    additionalText: "test23_additionalText"
                },
            //slotType:notification.SlotType.OTHER_TYPES
            }
        },publishSlotOtherCallback003);
        console.log("===ACTS_PublishSlotTypeOther_0300 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0300====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0400
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SOCIAL_COMMUNICATION)
     */
    it('ACTS_PublishSlotTypeOther_0400', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeOther_0400"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeOther_0400===createSubscriber"+JSON.stringify(data));
            OtherDataThree = data
            await Subscriber.subscribe(data, subOtherThreeCallBack)
        })
        function subOtherThreeCallBack(err,data){
            console.log('===subOtherThreeCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(OtherDataThree,UnSubscribeCallBackn)
            if(data.code == 24) {
                expect(data.code).assertEqual(24)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id:24,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test24_title",
                    text: "test24_text",
                    additionalText: "test24_additionalText"
                },
                slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        },publishSlotOtherCallback004);
        console.log("===ACTS_PublishSlotTypeOther_0400 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0400====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0500
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:OTHER_TYPES)  promise
     */
    it('ACTS_PublishSlotTypeOther_0500', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeOther_0500"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeOther_0500===createSubscriber"+JSON.stringify(data));
            OtherDataFour = data
            await Subscriber.subscribe(data, subOtherFourCallBack)
        })
        function subOtherFourCallBack(err,data){
            console.log('===subOtherFourCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(OtherDataFour,UnSubscribeCallBacko)
            if(data.code == 25) {
                expect(data.code).assertEqual(25)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id:25,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test25_title",
                    text: "test25_text",
                    additionalText: "test25_additionalText"
                },
                slotType:notification.SlotType.OTHER_TYPES
            }
        }).then(console.log("===ACTS_PublishSlotTypeOther_0500 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0500====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:OTHER_TYPES)  promise
     */
    it('ACTS_PublishSlotTypeOther_0600', 0,async function (done) {
        var promise = notification.publish({
            id:26,
            content: {
            // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
                slotType:notification.SlotType.OTHER_TYPES
            }
        })
        expect(promise).assertEqual(undefined)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0600====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0700
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)  promise
     */
    it('ACTS_PublishSlotTypeOther_0700', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeOther_0700"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_PublishSlotTypeOther_0700===createSubscriber"+JSON.stringify(data));
            OtherDataFive = data
            await Subscriber.subscribe(data, subOtherFiveCallBack)
        })
        function subOtherFiveCallBack(err,data){
            console.log('===subOtherFiveCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(OtherDataFive,UnSubscribeCallBackp)
            if(data.code == 27) {
                expect(data.code).assertEqual(27)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id:27,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test27_title",
                    text: "test27_text",
                    additionalText: "test27_additionalText"
                },
            //slotType:notification.SlotType.OTHER_TYPES
            }
        }).then(console.log("===ACTS_PublishSlotTypeOther_0700 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0700====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeOther_0500
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SOCIAL_COMMUNICATION)  promise
     */
    it('ACTS_PublishSlotTypeOther_0800', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeOther_0800"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_PublishSlotTypeOther_0800===createSubscriber"+JSON.stringify(data));
            OtherDataSix = data
            await Subscriber.subscribe(data, subOtherSixCallBack)
        })
        function subOtherSixCallBack(err,data){
            console.log('===subOtherSixCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(OtherDataSix,UnSubscribeCallBackq)
            if(data.code == 28) {
                expect(data.code).assertEqual(28)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id:28,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test28_title",
                    text: "test28_text",
                    additionalText: "test28_additionalText"
                },
                slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        }).then(console.log("===ACTS_PublishSlotTypeOther_0800 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeOther_0800====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0100
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SERVICE_INFORMATION)
     */
    it('ACTS_PublishSlotTypeService_0100', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeService_0100"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeService_0100===createSubscriber"+JSON.stringify(data));
            ServiceDataOne = data
            await Subscriber.subscribe(data, subServiceOneCallBack)
        })
        function subServiceOneCallBack(err,data){
            console.log('===subServiceOneCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(ServiceDataOne,UnSubscribeCallBackr)
            if(data.code == 29) {
                expect(data.code).assertEqual(29)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id:29,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test29_title",
                    text: "test29_text",
                    additionalText: "test29_additionalText"
                },
            },
            slotType:notification.SlotType.SERVICE_INFORMATION
        },publishSlotServiceCallback001);
        console.log("===ACTS_PublishSlotTypeService_0100 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0100====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SERVICE_INFORMATION)
     */
    it('ACTS_PublishSlotTypeService_0200', 0,async function (done) {
        await notification.publish({
            id: 30,
            content: {
            // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            slotType:notification.SlotType.SERVICE_INFORMATION
        },publishSlotServiceCallback002);
        console.log("===ACTS_PublishSlotTypeService_0200 finished===")
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0200====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)
     */
    it('ACTS_PublishSlotTypeService_0300', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeService_0300"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_PublishSlotTypeService_0300===createSubscriber"+JSON.stringify(data));
            ServiceDataTwo = data
            await Subscriber.subscribe(data, subServiceTwoCallBack)
        })
        function subServiceTwoCallBack(err,data){
            console.log('===subServiceTwoCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(ServiceDataTwo,UnSubscribeCallBacks)
            if(data.code == 31) {
                expect(data.code).assertEqual(31)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 31,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test31_title",
                    text: "test31_text",
                    additionalText: "test31_additionalText"
                },
            },
        // slotType:notification.SlotType.SERVICE_INFORMATION
        },publishSlotServiceCallback003);
        console.log("===ACTS_PublishSlotTypeService_0300 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0300====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0400
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:UNKNOWN_TYPE)
     */
    it('ACTS_PublishSlotTypeService_0400', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeService_0400"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async(data) =>{
            console.info("===ACTS_PublishSlotTypeService_0400===createSubscriber"+JSON.stringify(data));
            ServiceDataThree = data
            await Subscriber.subscribe(data, subServiceThreeCallBack)
        })
        function subServiceThreeCallBack(err,data){
            console.log('===subServiceThreeCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(ServiceDataThree,UnSubscribeCallBackt)
            if(data.code == 32) {
                expect(data.code).assertEqual(32)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 32,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test32_title",
                    text: "test32_text",
                    additionalText: "test32_additionalText"
                },
            },
            slotType:notification.SlotType.UNKNOWN_TYPE
        },publishSlotServiceCallback004);
        console.log("===ACTS_PublishSlotTypeService_0400 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0400====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0500
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SERVICE_INFORMATION) promise
     */
    it('ACTS_PublishSlotTypeService_0500', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeService_0500"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeService_0500===createSubscriber"+JSON.stringify(data));
            ServiceDataFour = data
            await Subscriber.subscribe(data, subServiceFourCallBack)
        })
        function subServiceFourCallBack(err,data){
            console.log('===subServiceFourCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(ServiceDataFour,UnSubscribeCallBacku)
            if(data.code == 33) {
                expect(data.code).assertEqual(33)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 33,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test33_title",
                    text: "test33_text",
                    additionalText: "test33_additionalText"
                },
            },
            slotType:notification.SlotType.SERVICE_INFORMATION
        }).then(console.log("===ACTS_PublishSlotTypeService_0500 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0500====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SERVICE_INFORMATION) promise
     */
    it('ACTS_PublishSlotTypeService_0600', 0,async function (done) {
        var promise = notification.publish({
            id: 34,
            content: {
            // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            slotType:notification.SlotType.SERVICE_INFORMATION
        })
        expect(promise).assertEqual(undefined)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0600====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0700
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT) promise
     */
    it('ACTS_PublishSlotTypeService_0700', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeService_0700"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeService_0700===createSubscriber"+JSON.stringify(data));
            ServiceDataFive = data
            await Subscriber.subscribe(data, subServiceFiveCallBack)
        })
        function subServiceFiveCallBack(err,data){
            console.log('===subServiceFourCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(ServiceDataFive,UnSubscribeCallBackv)
            if(data.code == 35) {
                expect(data.code).assertEqual(35)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 35,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test35_title",
                    text: "test35_text",
                    additionalText: "test35_additionalText"
                },
            },
        // slotType:notification.SlotType.SERVICE_INFORMATION
        }).then(console.log("===ACTS_PublishSlotTypeService_0700 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0700====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeService_0800
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:UNKNOWN_TYPE) promise
     */
    it('ACTS_PublishSlotTypeService_0800', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeService_0800"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeService_0800===createSubscriber"+JSON.stringify(data));
            ServiceDataSix = data
            await Subscriber.subscribe(data, subServiceSixCallBack)
        })
        function subServiceSixCallBack(err,data){
            console.log('===subServiceSixCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(ServiceDataSix,UnSubscribeCallBackw)
            if(data.code == 36) {
                expect(data.code).assertEqual(36)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 36,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test36_title",
                    text: "test36_text",
                    additionalText: "test36_additionalText"
                },
            },
            slotType:notification.SlotType.UNKNOWN_TYPE
        }).then(console.log("===ACTS_PublishSlotTypeService_0800 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeService_0800====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0100
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SOCIAL_COMMUNICATION)
     */
    it('ACTS_PublishSlotTypeSocial_0100', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeSocial_0100"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeSocial_0100===createSubscriber"+JSON.stringify(data));
            SocialDataOne = data
            await Subscriber.subscribe(data, subSocialOneCallBack)
        })
        function subSocialOneCallBack(err,data){
            console.log('===subSocialOneCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SocialDataOne,UnSubscribeCallBackx)
            if(data.code == 37) {
                expect(data.code).assertEqual(37)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 37,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test37_title",
                    text: "test37_text",
                    additionalText: "test37_additionalText"
                },
                slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        },publishSlotSocialCallback001);
        console.log("===ACTS_PublishSlotTypeSocial_0600 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeSocial_0100====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0200
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SOCIAL_COMMUNICATION)
     */
    it('ACTS_PublishSlotTypeSocial_0200', 0,async function (done) {
        await notification.publish({
            id: 38,
            content: {
            // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
                slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        },publishSlotSocialCallback002);
        console.log("===ACTS_PublishSlotTypeSocial_0600 finished===")
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeSocial_0200====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0300
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT)
     */
    it('ACTS_PublishSlotTypeSocial_0300', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeSocial_0300"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeSocial_0300===createSubscriber"+JSON.stringify(data));
            SocialDataTwo = data
            await Subscriber.subscribe(data, subSocialTwoCallBack)
        })
        function subSocialTwoCallBack(err,data){
            console.log('===subSocialTwoCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SocialDataTwo,UnSubscribeCallBacky)
            if(data.code == 39) {
                expect(data.code).assertEqual(39)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 39,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test39_title",
                    text: "test39_text",
                    additionalText: "test39_additionalText"
                },
            // slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        },publishSlotSocialCallback003);
        console.log("===ACTS_PublishSlotTypeSocial_0300 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeSocial_0300====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0400
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:CONTENT_INFORMATION)
     */
    it('ACTS_PublishSlotTypeSocial_0400', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeSocial_0400"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeSocial_0400===createSubscriber"+JSON.stringify(data));
            SocialDataThree = data
            await Subscriber.subscribe(data, subSocialThreeCallBack)
        })
        function subSocialThreeCallBack(err,data){
            console.log('===subSocialThreeCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SocialDataThree,UnSubscribeCallBackz)
            if(data.code == 40) {
                expect(data.code).assertEqual(40)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        await notification.publish({
            id: 40,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test40_title",
                    text: "test40_text",
                    additionalText: "test40_additionalText"
                },
                slotType:notification.SlotType.CONTENT_INFORMATION
            }
        },publishSlotSocialCallback004);
        console.log("===ACTS_PublishSlotTypeSocial_0400 finished===")
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeSocial_0400====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0500
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:SOCIAL_COMMUNICATION) promise
     */
    it('ACTS_PublishSlotTypeSocial_0500', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeSocial_0500"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeSocial_0500===createSubscriber"+JSON.stringify(data));
            SocialDataFour = data
            await Subscriber.subscribe(data, subSocialFourCallBack)
        })
        function subSocialFourCallBack(err,data){
            console.log('===subSocialFourCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SocialDataFour,UnSubscribeCallBackA)
            if(data.code == 41) {
                expect(data.code).assertEqual(41)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 41,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test41_title",
                    text: "test41_text",
                    additionalText: "test41_additionalText"
                },
                slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        }).then(console.log("===ACTS_PublishSlotTypeSocial_0500 finished==="))
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeSocial_0500====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0600
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(slotType:SOCIAL_COMMUNICATION) promise
     */
    it('ACTS_PublishSlotTypeSocial_0600', 0,async function (done) {
        var promise = notification.publish({
            id: 42,
            content: {
            // contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
                slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        })
        expect(promise).assertEqual(undefined)
        done();
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeSocial_0600====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0700
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT) promise
     */
    it('ACTS_PublishSlotTypeSocial_0700', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeSocial_0700"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeSocial_0700===createSubscriber"+JSON.stringify(data));
            SocialDataFive = data
            await Subscriber.subscribe(data, subSocialFiveCallBack)
        })
        function subSocialFiveCallBack(err,data){
            console.log('===subSocialFiveCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SocialDataFive,UnSubscribeCallBackB)
            if(data.code == 43) {
                expect(data.code).assertEqual(43)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 43,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test43_title",
                    text: "test43_text",
                    additionalText: "test43_additionalText"
                },
            //slotType:notification.SlotType.SOCIAL_COMMUNICATION
            }
        }).then(console.log("===ACTS_PublishSlotTypeSocial_0700 finished==="),
        )
        setTimeout(function(){
            console.debug("====>time out ACTS_PublishSlotTypeSocial_0700====>");
        }, time);
    })

    /*
     * @tc.number: ACTS_PublishSlotTypeSocial_0800
     * @tc.name: publish()
     * @tc.desc: verify the function of publish(contentType:NOTIFICATION_CONTENT_BASIC_TEXT,slotType:CONTENT_INFORMATION) promise
     */
    it('ACTS_PublishSlotTypeSocial_0800', 0,async function (done) {
        var commonEventSubscribeInfo = {
            events: ["ACTS_PublishSlotTypeSocial_0800"]
        };
        await Subscriber.createSubscriber(
            commonEventSubscribeInfo).then(async (data) =>{
            console.info("===ACTS_PublishSlotTypeSocial_0800===createSubscriber"+JSON.stringify(data));
            SocialDataSix = data
            await Subscriber.subscribe(data, subSocialSixCallBack)
        })
        function subSocialSixCallBack(err,data){
            console.log('===subSocialSixCallBack==='+err.code+JSON.stringify(data))
            Subscriber.unsubscribe(SocialDataSix,UnSubscribeCallBackC)
            if(data.code == 44) {
                expect(data.code).assertEqual(44)
                done();
            }
            else{
                expect().assertFail();
                done();
            }
        }
        notification.publish({
            id: 44,
            content: {
                contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test44_title",
                    text: "test44_text",
                    additionalText: "test44_additionalText"
                },
                slotType:notification.SlotType.CONTENT_INFORMATION
            }
        }).then(console.log("===ACTS_PublishSlotTypeSocial_0800 finished==="))
    })
})