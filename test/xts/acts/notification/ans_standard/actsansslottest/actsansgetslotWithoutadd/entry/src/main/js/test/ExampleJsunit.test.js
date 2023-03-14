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
describe('ActsAnsGetSlotWithoutAdd', function () {

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0100
     * @tc.name      : getSlot by type callback
     * @tc.desc      : Get the SOCIAL_COMMUNICATION type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0100', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0100 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0100 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0100 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0100 data:" + JSON.stringify(data));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>getSlot callback SlotType.SOCIAL_COMMUNICATION====>");
        await notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION, getSlotCallback);
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0100====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0200
     * @tc.name      : getSlot by type callback
     * @tc.desc      : Get the SERVICE_INFORMATION type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0200', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0200 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsGetSlotWithoutAdd_0200 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0200 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0200 data:" + JSON.stringify(data));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>getSlot callback SlotType.SERVICE_INFORMATION====>");
        await notification.getSlot(notification.SlotType.SERVICE_INFORMATION, getSlotCallback);
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0200====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0300
     * @tc.name      : getSlot by type callback
     * @tc.desc      : Get the CONTENT_INFORMATION type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0300', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0300 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsGetSlotWithoutAdd_0300 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0300 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0300 data:" + JSON.stringify(data));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>getSlot callback SlotType.CONTENT_INFORMATION====>");
        await notification.getSlot(notification.SlotType.CONTENT_INFORMATION, getSlotCallback);
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0300====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0400
     * @tc.name      : getSlot by type callback
     * @tc.desc      : Get the OTHER_TYPES type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0400', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0400 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsGetSlotWithoutAdd_0400 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0400 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0400 data:" + JSON.stringify(data));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>getSlot callback SlotType.OTHER_TYPES====>");
        await notification.getSlot(notification.SlotType.OTHER_TYPES, getSlotCallback);
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0400====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0500
     * @tc.name      : getSlot by type callback
     * @tc.desc      : Get the UNKNOWN_TYPE type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0500', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0500 start====>");
        function getSlotCallback(err, data) {
            console.debug("====>ActsAnsGetSlotWithoutAdd_0500 enter====>");
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0500 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0500 data:" + JSON.stringify(data));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>getSlot callback SlotType.UNKNOWN_TYPE====>");
        await notification.getSlot(notification.SlotType.UNKNOWN_TYPE, getSlotCallback);
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0500====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0600
     * @tc.name      : getSlot by type promise
     * @tc.desc      : Get the SOCIAL_COMMUNICATION type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0600', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0600 start====>");
        console.debug("====>getSlot SlotType.SOCIAL_COMMUNICATION====>");
        notification.getSlot(notification.SlotType.SOCIAL_COMMUNICATION).then((data)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0600 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0600 err:" + JSON.stringify(err));
            console.debug("====>ActsAnsGetSlotWithoutAdd_0600 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0600====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0700
     * @tc.name      : getSlot by type promise
     * @tc.desc      : Get the SERVICE_INFORMATION type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0700', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0700 start====>");
        console.debug("====>getSlot SlotType.SERVICE_INFORMATION====>");
        notification.getSlot(notification.SlotType.SERVICE_INFORMATION).then((data)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0700 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0700 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0700 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0700====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0800
     * @tc.name      : getSlot by type promise
     * @tc.desc      : Get the UNKNOWN_TYPE type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0800', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0800 start====>");
        console.debug("====>getSlot SlotType.UNKNOWN_TYPE====>");
        notification.getSlot(notification.SlotType.UNKNOWN_TYPE).then((data)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0800 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0800 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0800 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0800====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_0900
     * @tc.name      : getSlot by type promise
     * @tc.desc      : Get the OTHER_TYPES type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_0900', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_0900 start====>");
        console.debug("====>getSlot SlotType.OTHER_TYPES====>");
        notification.getSlot(notification.SlotType.OTHER_TYPES).then((data)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0900 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0900 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_0900 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_0900====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsGetSlotWithoutAdd_1000
     * @tc.name      : getSlot by type promise
     * @tc.desc      : Get the CONTENT_INFORMATION type slot without adding
     */
    it('ActsAnsGetSlotWithoutAdd_1000', 0, async function (done) {
        console.debug("====>ActsAnsGetSlotWithoutAdd_1000 start====>");
        console.debug("====>getSlot SlotType.CONTENT_INFORMATION====>");
        notification.getSlot(notification.SlotType.CONTENT_INFORMATION).then((data)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_1000 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_1000 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsGetSlotWithoutAdd_1000 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsGetSlotWithoutAdd_1000====>");
        }, TIMEOUT);
    })
}) 