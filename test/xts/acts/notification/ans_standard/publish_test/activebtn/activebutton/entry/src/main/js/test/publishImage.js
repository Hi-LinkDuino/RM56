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
import image from '@ohos.multimedia.image'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var BufferA
var BufferB
var BufferC
var BufferD
var optsA
var optsB
var optsC
var optsD
var pubimageA
var pubimageB
var pubimageC
var pubimageD
describe('ActsAnsPublishImageTest', function () {
    console.info("===ActsAnsPublishImageTest start===>");

    //consume
    function consumeCallbackA(data) {
        console.debug("===>consumeDoNotCallback1 data : ===>" + JSON.stringify(data));
        expect(data.request.id).assertEqual(1)
    }
    function consumeCallbackB(data) {
        console.debug("===>consumeDoNotCallback2 data : ===>" +JSON.stringify(data));
        expect(data.request.id).assertEqual(2)
    }
    //subscribe
    function subscribeCallbackA(err) {
        console.debug("===>subscribeCallbackA===>"+err.code);
        expect(err.code).assertEqual(0)
    }
    function subscribeCallbackB(err) {
        console.debug("===>subscribeCallbackB===>"+err.code);
        expect(err.code).assertEqual(0)
    }
    function publishCallbackA(err){
        console.log('ActsAnsPublishImageTest publishCallbackA asyncCallback'+err.code)
        expect(err.code).assertEqual(0)
    }
    function publishCallbackB(err){
        console.log('ActsAnsPublishImageTest publishCallbackB asyncCallback'+err.code)
        expect(err.code != 0).assertEqual(true);
    }

    /*
     * @tc.number: ActsPublishImage_test_0100
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsPublishImage_test_0100', 0, async function (done) {
        console.debug("===ActsDoNotSubscriber_test_0100===begin===>");

        var subInfo ={
            onConsume:consumeCallbackA,
        }
        await notify.subscribe(subInfo, subscribeCallbackA);

        BufferA = new ArrayBuffer(32);
        optsA = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 2, width: 3}}

        const promise_Large = image.createPixelMap(BufferA, optsA);
        promise_Large.then((data) => {
            pubimageA = data;
            console.debug("===createPixelMapA image===>"+pubimageA);

            notify.publish({
                id: 1,
                content: {
                    contentType: notify.ContentType.NOTIFICATION_CONTENT_PICTURE,
                    picture: {
                        title: "image_titleA",
                        text: "image_textA",
                        additionalText: "image_additionalTextA",
                        briefText:"image_briefA",
                        expandedTitle:"expandedTitleA",
                        picture:pubimageA
                    },
                },
                slotType:notify.SlotType.SOCIAL_COMMUNICATION,
                classification:"classificationA",
                sortingKey:"sortingKeyA",
            },publishCallbackA);
                setTimeout((async function(){
                    notify.unsubscribe(subInfo);
                    console.info("======ActsPublishImage_test_0100 setTimeout unsubscribe===>");
                    done();
                }),300);
            })
        })

    /*
     * @tc.number: ActsPublishImage_test_0200
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsPublishImage_test_0200', 0, async function (done) {
        console.debug("===ActsPublishImage_test_0200===begin===>");

        var subInfo ={
            onConsume:consumeCallbackB,
        }
        await notify.subscribe(subInfo, subscribeCallbackB);

        BufferB = new ArrayBuffer(32);
        optsB = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 2, width: 3}}

        const promise_Large = image.createPixelMap(BufferB, optsB);
        promise_Large.then((data) => {
            pubimageB = data;
            console.debug("===createPixelMapB image===>"+pubimageB);

            notify.publish({
                id: 2,
                content: {
                    contentType: notify.ContentType.NOTIFICATION_CONTENT_PICTURE,
                    picture: {
                        title: "image_titleB",
                        text: "image_textB",
                        additionalText: "image_additionalTextB",
                        briefText:"image_briefB",
                        expandedTitle:"expandedTitleB",
                        picture:pubimageB
                    },
                },
                slotType:notify.SlotType.SOCIAL_COMMUNICATION,
                classification:"classificationB",
                sortingKey:"sortingKeyB",
            }).then(()=> {
                console.debug("===ActsPublishImage_test_0200 promise===>")
            })
            setTimeout((async function(){
                notify.unsubscribe(subInfo);
                console.info("===ActsPublishImage_test_0200 setTimeout unsubscribe===>");
                done();
            }),300);
        })
    })

    /*
     * @tc.number: ActsPublishImage_test_0300
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsPublishImage_test_0300', 0, async function (done) {
        console.debug("===ActsPublishImage_test_0300===begin===>");
        BufferC = new ArrayBuffer( 4*1024* 2048);
        optsC = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 1024, width: 2048}}

        const promise_Large = image.createPixelMap(BufferC, optsC);
        promise_Large.then((data) => {
            pubimageC = data;
            console.debug("====createPixelMapC image===>"+pubimageC);

            notify.publish({
                id: 3,
                content: {
                    contentType: notify.ContentType.NOTIFICATION_CONTENT_PICTURE,
                    picture: {
                        title: "image_titleC",
                        text: "image_textC",
                        additionalText: "image_additionalTextC",
                        briefText:"image_briefC",
                        expandedTitle:"expandedTitleC",
                        picture:pubimageC
                    },
                },
                slotType:notify.SlotType.SOCIAL_COMMUNICATION,
                classification:"classificationC",
                sortingKey:"sortingKeyC",
            },publishCallbackB);
            done();
        })
     })

    /*
     * @tc.number: ActsPublishImage_test_0400
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsPublishImage_test_0400', 0, async function (done) {
        console.debug("===ActsPublishImage_test_0400===begin===>");
        BufferD = new ArrayBuffer( 4*1024* 2048);
        optsD = {alphaType: 0, editable: true, pixelFormat: 4, scaleMode: 1, size: {height: 1024, width: 2048}}

        const promise_Large = image.createPixelMap(BufferD, optsD);
        promise_Large.then((data) => {
            pubimageD = data;
            console.debug("===createPixelMapD image===>"+pubimageD);

            notify.publish({
                id: 4,
                content: {
                    contentType: notify.ContentType.NOTIFICATION_CONTENT_PICTURE,
                    picture: {
                        title: "image_titleD",
                        text: "image_textD",
                        additionalText: "image_additionalTextD",
                        briefText:"image_briefD",
                        expandedTitle:"expandedTitleD",
                        picture:pubimageD
                    },
                },
                slotType:notify.SlotType.SOCIAL_COMMUNICATION,
                classification:"classificationD",
                sortingKey:"sortingKeyD",
            }).then().catch((err)=>{
                console.debug("===ActsPublishImage_test_0400 err===>"+err.code);
                expect(err.code != 0).assertEqual(true);
                done();
            });
        })
     })
   })


