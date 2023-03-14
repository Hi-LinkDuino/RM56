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
describe('ActsAnsRemoveSlotWithoutAdd', function () {

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0100
     * @tc.name      : removeSlot callback
     * @tc.desc      : remove the SOCIAL_COMMUNICATION type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0100', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0100 start====>");
        function removeSlotCallbck(err) {
            console.debug("====>removeSlotActsAnsGetSlotWithoutAdd_0100 enter====>");
            console.debug("====>removeSlot ActsAnsRemoveSlotWithoutAdd_0100 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>removeSlot callback SlotType.SOCIAL_COMMUNICATION====>");
        await notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION, removeSlotCallbck);
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0100====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0200
     * @tc.name      : removeSlot callback
     * @tc.desc      : remove the SERVICE_INFORMATION type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0200', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0200 start====>");
        function removeSlotCallbck(err) {
            console.debug("====>ActsAnsRemoveSlotWithoutAdd_0200 enter====>");
            console.debug("====>removeSlot ActsAnsRemoveSlotWithoutAdd_0200 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>removeSlot callback SlotType.SERVICE_INFORMATION====>");
        await notification.removeSlot(notification.SlotType.SERVICE_INFORMATION, removeSlotCallbck);
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0200====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0300
     * @tc.name      : removeSlot callback
     * @tc.desc      : remove the CONTENT_INFORMATION type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0300', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0300 start====>");
        function removeSlotCallbck(err) {
            console.debug("====>ActsAnsRemoveSlotWithoutAdd_0300 enter====>");
            console.debug("====>removeSlot ActsAnsRemoveSlotWithoutAdd_0300 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>removeSlot callback SlotType.CONTENT_INFORMATION====>");
        await notification.removeSlot(notification.SlotType.CONTENT_INFORMATION, removeSlotCallbck);
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0300====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0400
     * @tc.name      : removeSlot callback
     * @tc.desc      : remove the OTHER_TYPES type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0400', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0400 start====>");
        function removeSlotCallbck(err) {
            console.debug("====>ActsAnsRemoveSlotWithoutAdd_0400 enter====>");
            console.debug("====>removeSlot ActsAnsRemoveSlotWithoutAdd_0400 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>removeSlot callback SlotType.OTHER_TYPES====>");
        await notification.removeSlot(notification.SlotType.OTHER_TYPES, removeSlotCallbck);
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0400====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0500
     * @tc.name      : removeSlot callback
     * @tc.desc      : remove the UNKNOWN_TYPE type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0500', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0500 start====>");
        function removeSlotCallbck(err) {
            console.debug("====>ActsAnsRemoveSlotWithoutAdd_0500 enter====>");
            console.debug("====>removeSlot ActsAnsRemoveSlotWithoutAdd_0500 err:" + JSON.stringify(err));
            expect(err.code != 0).assertEqual(true);
            done();
        }
        console.debug("====>removeSlot callback SlotType.UNKNOWN_TYPE====>");
        await notification.removeSlot(notification.SlotType.UNKNOWN_TYPE, removeSlotCallbck);
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0500====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0600
     * @tc.name      : removeSlot promise
     * @tc.desc      : remove the SOCIAL_COMMUNICATION type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0600', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0600 start====>");
        console.debug("====>removeSlot SlotType.SOCIAL_COMMUNICATION====>");
        notification.removeSlot(notification.SlotType.SOCIAL_COMMUNICATION).then(()=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0600 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0600 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0600 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0600====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0700
     * @tc.name      : removeSlot promise
     * @tc.desc      : remove the SERVICE_INFORMATION type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0700', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0700 start====>");
        console.debug("====>removeSlot SlotType.SERVICE_INFORMATION====>");
        notification.removeSlot(notification.SlotType.SERVICE_INFORMATION).then(()=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0700 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0700 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0700 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0700====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0800
     * @tc.name      : removeSlot promise
     * @tc.desc      : remove the CONTENT_INFORMATION type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0800', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0800 start====>");
        console.debug("====>removeSlot SlotType.CONTENT_INFORMATION====>");
        notification.removeSlot(notification.SlotType.CONTENT_INFORMATION).then(()=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0800 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0800 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0800 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0800====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_0900
     * @tc.name      : removeSlot promise
     * @tc.desc      : remove the OTHER_TYPES type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_0900', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_0900 start====>");
        console.debug("====>removeSlot SlotType.OTHER_TYPES====>");
        notification.removeSlot(notification.SlotType.OTHER_TYPES).then(()=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0900 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0900 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_0900 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_0900====>");
        }, TIMEOUT);
    })

    /*
     * @tc.number    : ActsAnsRemoveSlotWithoutAdd_1000
     * @tc.name      : removeSlot promise
     * @tc.desc      : remove the UNKNOWN_TYPE type slot without adding
     */
    it('ActsAnsRemoveSlotWithoutAdd_1000', 0, async function (done) {
        console.debug("====>ActsAnsRemoveSlotWithoutAdd_1000 start====>");
        console.debug("====>removeSlot SlotType.UNKNOWN_TYPE====>");
        notification.removeSlot(notification.SlotType.UNKNOWN_TYPE).then(()=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_1000 enter====>");
            expect().assertFail();
            done();
        }).catch((err)=>{
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_1000 err:" + JSON.stringify(err));
            console.debug("====>getSlot ActsAnsRemoveSlotWithoutAdd_1000 end====>");
            expect(err.code != 0).assertEqual(true);
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsAnsRemoveSlotWithoutAdd_1000====>");
        }, TIMEOUT);
    })
}) 