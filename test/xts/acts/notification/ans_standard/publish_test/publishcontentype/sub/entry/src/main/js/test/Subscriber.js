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
import Subscriber from '@ohos.commonevent'
import notification from '@ohos.notification'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
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
var time = 1000
function publishCallbacka(err) {
    console.info("===>publishCallbacka"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackb(err) {
    console.info("===>publishCallbackb"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackc(err) {
    console.info("===>publishCallbackc"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackd(err) {
    console.info("===>publishCallbackd"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbacke(err) {
    console.info("===>publishCallbacke"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackf(err) {
    console.info("===>publishCallbackf"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackg(err) {
    console.info("===>publishCallbackg"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackh(err) {
    console.info("===>publishCallbackh"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbacki(err) {
    console.info("===>publishCallbacki"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackj(err) {
    console.info("===>publishCallbackj"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackl(err) {
    console.info("===>publishCallbackl"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackm(err) {
    console.info("===>publishCallbackm"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackn(err) {
    console.info("===>publishCallbackn"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbacko(err) {
    console.info("===>publishCallbacko"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackp(err) {
    console.info("===>publishCallbackp"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackq(err) {
    console.info("===>publishCallbackq"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackr(err) {
    console.info("===>publishCallbackr"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbacks(err) {
    console.info("===>publishCallbacks"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackt(err) {
    console.info("===>publishCallbackt"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbacku(err) {
    console.info("===>publishCallbacku"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackv(err) {
    console.info("===>publishCallbackv"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackw(err) {
    console.info("===>publishCallbackw"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackx(err) {
    console.info("===>publishCallbackx"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbacky(err) {
    console.info("===>publishCallbacky"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackz(err) {
    console.info("===>publishCallbackz"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackA(err) {
    console.info("===>publishCallbackA"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackB(err) {
    console.info("===>publishCallbackB"+err.code);
    expect(err.code).assertEqual(0);
}
function publishCallbackC(err) {
    console.info("===>publishCallbackC"+err.code);
    expect(err.code).assertEqual(0);
}
describe('ActsAnsCommonEventTest', function () {
    console.info("==ActsAnsCommonEventTest start==>");
    //consume
    function consumeCallback(data) {
        console.debug("===>consumeCallback data===>" + JSON.stringify(data));
        if(data.request.id == 1) {
            if(data.request.content.multiLine.title === "test1_title")
            {
                multiLineDataOne = {
                    code: 1
                }
            }
            else{
                multiLineDataOne = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishMULTILINEContent_0100", multiLineDataOne, publishCallbacka);
        }
        else if(data.request.id == 4){
            if(data.request.content.multiLine.title == "test4_title")
            {
                 multiLineDataTwo = {
                    code: 4
                }
            }
            else{
                 multiLineDataTwo = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishMULTILINEContent_0400", multiLineDataTwo, publishCallbackb);
        }else if(data.request.id == 7){
            if(data.request.content.longText.title == "test7_title")
            {
                LongContentDataOne = {
                    code: 7
                }
            }
            else{
                LongContentDataOne = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishLONGContent_0100", LongContentDataOne, publishCallbackc);
        }
        else if(data.request.id == 10){
            if(data.request.content.longText.title == "test10_title")
            {
                LongContentDataTwo = {
                    code: 10
                }
            }
            else{
                LongContentDataTwo = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishLONGContent_0400", LongContentDataTwo, publishCallbackd);
        }
        else if(data.request.id == 13){
            if(data.request.content.normal.title == "test13_title")
            {
                SlotTypeDataOne = {
                    code: 13
                }
            }
            else{
                SlotTypeDataOne = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_Publish_SlotTypeContent_0100", SlotTypeDataOne, publishCallbacke);
        }
        else if(data.request.id == 15){
            if(data.request.content.normal.title == "test15_title")
            {
                SlotTypeDataTwo = {
                    code: 15
                }
            }
            else{
                SlotTypeDataTwo = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_Publish_SlotTypeContent_0300", SlotTypeDataTwo, publishCallbackf);
        }
        else if(data.request.id == 16){
            if(data.request.content.normal.title == "test16_title")
            {
                SlotTypeDataThree = {
                    code: 16
                }
            }
            else{
                SlotTypeDataThree = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_Publish_SlotTypeContent_0400", SlotTypeDataThree, publishCallbackg);
        }
        else if(data.request.id == 17){
            if(data.request.content.normal.title == "test17_title")
            {
                SlotTypeDataFour = {
                    code: 17
                }
            }
            else{
                SlotTypeDataFour = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_Publish_SlotTypeContent_0500", SlotTypeDataFour, publishCallbackh);
        }
        else if(data.request.id == 19){
            if(data.request.content.normal.title == "test19_title")
            {
                SlotTypeDataFive = {
                    code: 19
                }
            }
            else{
                SlotTypeDataFive = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_Publish_SlotTypeContent_0700", SlotTypeDataFive, publishCallbacki);
        }
        else if(data.request.id == 20){
            if(data.request.content.normal.title == "test20_title")
            {
                SlotTypeDataSix = {
                    code: 20
                }
            }
            else{
                SlotTypeDataSix = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_Publish_SlotTypeContent_0800", SlotTypeDataSix, publishCallbackj);
        }
        else if(data.request.id == 21){
            if(data.request.content.normal.title == "test21_title")
            {
                OtherDataOne = {
                    code: 21
                }
            }
            else{
                OtherDataOne = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeOther_0100", OtherDataOne, publishCallbackl);
        }
        else if(data.request.id == 23){
            if(data.request.content.normal.title == "test23_title")
            {
                OtherDataTwo = {
                    code: 23
                }
            }
            else{
                OtherDataTwo = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeOther_0300", OtherDataTwo, publishCallbackm);
        }
        else if(data.request.id == 24){
            if(data.request.content.normal.title == "test24_title")
            {
                OtherDataThree = {
                    code: 24
                }
            }
            else{
                OtherDataThree = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeOther_0400", OtherDataThree, publishCallbackn);
        }
        else if(data.request.id == 25){
            if(data.request.content.normal.title == "test25_title")
            {
                OtherDataFour = {
                    code: 25
                }
            }
            else{
                OtherDataFour = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeOther_0500", OtherDataFour, publishCallbacko);
        }
        else if(data.request.id == 27){
            if(data.request.content.normal.title == "test27_title")
            {
                OtherDataFive = {
                    code: 27
                }
            }
            else{
                OtherDataFive= {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeOther_0700", OtherDataFive, publishCallbackp);
        }
        else if(data.request.id == 28){
            if(data.request.content.normal.title == "test28_title")
            {
                OtherDataSix = {
                    code: 28
                }
            }
            else{
                OtherDataSix = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeOther_0800", OtherDataSix, publishCallbackq);
        }
        else if(data.request.id == 29){
            if(data.request.content.normal.title == "test29_title")
            {
                ServiceDataOne = {
                    code: 29
                }
            }
            else{
                ServiceDataOne = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeService_0100", ServiceDataOne, publishCallbackr);
        }
        else if(data.request.id == 31){
            if(data.request.content.normal.title == "test31_title")
            {
                ServiceDataTwo = {
                    code: 31
                }
            }
            else{
                ServiceDataTwo = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeService_0300", ServiceDataTwo, publishCallbacks);
        }
        else if(data.request.id == 32){
            if(data.request.content.normal.title == "test32_title")
            {
                ServiceDataThree = {
                    code: 32
                }
            }
            else{
                ServiceDataThree = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeService_0400", ServiceDataThree, publishCallbackt);
        }
        else if(data.request.id == 33){
            if(data.request.content.normal.title == "test33_title")
            {
                ServiceDataFour = {
                    code: 33
                }
            }
            else{
                ServiceDataFour = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeService_0500", ServiceDataFour, publishCallbacku);
        }
        else if(data.request.id == 35){
            if(data.request.content.normal.title == "test35_title")
            {
                ServiceDataFive = {
                    code: 35
                }
            }
            else{
                ServiceDataFive = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeService_0700", ServiceDataFive, publishCallbackv);
        }
        else if(data.request.id == 36){
            if(data.request.content.normal.title == "test36_title")
            {
                ServiceDataSix = {
                    code: 36
                }
            }
            else{
                ServiceDataSix = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeService_0800", ServiceDataSix, publishCallbackw);
        }
        else if(data.request.id == 37){
            if(data.request.content.normal.title == "test37_title")
            {
                SocialDataOne = {
                    code: 37
                }
            }
            else{
                SocialDataOne = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeSocial_0100", SocialDataOne, publishCallbackx);
        }
        else if(data.request.id == 39){
            if(data.request.content.normal.title == "test39_title")
            {
                SocialDataTwo= {
                    code: 39
                }
            }
            else{
                SocialDataTwo = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeSocial_0300", SocialDataTwo, publishCallbacky);
        }
        else if(data.request.id == 40){
            if(data.request.content.normal.title == "test40_title")
            {
                SocialDataThree = {
                    code: 40
                }
            }
            else{
                SocialDataThree = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeSocial_0400", SocialDataThree, publishCallbackz);
        }
        else if(data.request.id == 41){
            if(data.request.content.normal.title == "test41_title")
            {
                SocialDataFour = {
                    code: 41
                }
            }
            else{
                SocialDataFour = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeSocial_0500", SocialDataFour, publishCallbackA);
        }
        else if(data.request.id == 43){
            if(data.request.content.normal.title == "test43_title")
            {
                SocialDataFive = {
                    code: 43
                }
            }
            else{
                SocialDataFive = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeSocial_0700", SocialDataFive, publishCallbackB);
        }
        else if(data.request.id == 44){
            if(data.request.content.normal.title == "test44_title")
            {
                SocialDataSix = {
                    code: 44
                }
            }
            else{
                SocialDataSix = {
                    code: 0
                }
            }
            Subscriber.publish("ACTS_PublishSlotTypeSocial_0800", SocialDataSix, publishCallbackC);
        }
    }
    //subscribeOn
    function subscribeOnCallback() {
        console.debug("===>onConnectCallback===>");
    }
    /*
     * @tc.number: ActsDoNotSubscriber_test_0100
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsCommonSubscriber_test_0100', 0, async function (done) {
        console.debug("===ActsCommonSubscriber_test_0100====begin===>");
        var subInfo ={
            onConsume:consumeCallback,
            onConnect:subscribeOnCallback,
        }
            await notification.subscribe(subInfo,(err) => {
                console.debug("===>subscribeCallback===>"+err.code)
                expect(err.code).assertEqual(0);
                done()
        });
    })
})

