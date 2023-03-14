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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
var time = 1000
describe('ActsAnsEnableNotificationTest', function () {
    console.info("===========ActsAnsEnableNotificationTest start====================>");
    /*
     * @tc.number: ActsGetEnable_test_0100
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0100', 0, async function (done) {
        await notify.isNotificationEnabled({
            bundle:"com.example.actsanslocalnotificationtest",
        },(err,data) => {
            console.log("===>ActsGetEnable_test_0100 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
            expect(data).assertEqual(true)
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsGetEnable_test_0100====>");
        }, time);
    })
    /*
     * @tc.number: ActsGetEnable_test_0200
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0200', 0, async function (done) {
        var promise = await notify.isNotificationEnabled({
            bundle:"com.example.actsanslocalnotificationtest",
        })
        console.log("===>ActsGetEnable_test_0200 success===>"+promise)
        expect(typeof(promise)).assertEqual('boolean')
        expect(promise).assertEqual(true)
        done();
        setTimeout(function(){
            console.debug("====>time out ActsGetEnable_test_0200====>");
        }, time);
    })
    /*
     * @tc.number: ActsGetEnable_test_0300
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0300', 0, async function (done) {
        await notify.isNotificationEnabled({
            bundle:"wrong BundleName",
        },(err,data) => {
            console.log("===>ActsGetEnable_test_0300 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
            expect(data).assertEqual(false)
            done();
        })
        setTimeout(function(){
            console.debug("====>time out ActsGetEnable_test_0300====>");
        }, time);
    })
    /*
     * @tc.number: ActsGetEnable_test_0400
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0400', 0, async function (done) {
         notify.isNotificationEnabled({
            bundle:"wrong BundleName",
        }).then().catch((err)=>{
             console.debug("====>ActsGetEnable_test_0400 promsie====>"+err.code);
             expect(err.code != 0).assertEqual(true);
             done();
         })
    })
    /*
     * @tc.number: ActsGetEnable_test_0500
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0500', 0, async function (done) {
        await notify.isNotificationEnabled("#$#$%$%^",(err,data) => {
            console.log("===>ActsGetEnable_test_0500 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
        })
        done();
    })
    /*
     * @tc.number: ActsGetEnable_test_0600
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0600', 0, async function (done) {
        var promise =  await notify.isNotificationEnabled("#$#$%$%^")
        expect(promise).assertEqual(undefined)
        done();
    })
    /*
     * @tc.number: ActsGetDisplay_test_0700
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0700', 0, async function (done) {
        await notify.isNotificationEnabled({},(err,data) => {
            console.log("===>ActsGetEnable_test_0700 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
        })
        done();
    })
    /*
     * @tc.number: ActsGetEnable_test_0800
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0800', 0, async function (done) {
        var promise =  await notify.isNotificationEnabled({})
        expect(promise).assertEqual(undefined)
        done();
    })
    /*
     * @tc.number: ActsGetEnable_test_0900
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_0900', 0, async function (done) {
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },true,async(err) => {
            console.log("===>ActsGetEnable_test_0900 success===>"+err)
            await notify.isNotificationEnabled({
                bundle:"com.example.actsanslocalnotificationtest",
            },(err,data) => {
                console.log("===>ActsGetEnable_test_0900 success===>"+err+data)
                expect(typeof(data)).assertEqual('boolean')
                expect(data).assertEqual(true)
                done();
            })
        })
    })
    /*
     * @tc.number: ActsGetEnable_test_1000
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_1000', 0, async function (done) {
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },true,async(err) => {
            console.log("===>ActsGetEnable_test_1000 success===>"+err)
            var promise = await notify.isNotificationEnabled({
                bundle:"com.example.actsanslocalnotificationtest",
            })
            console.log("===>ActsGetEnable_test_1000 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(true)
            done();
        })
    })
    /*
     * @tc.number: ActsGetEnable_test_1100
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_1100', 0, async function (done) {
        await notify.isNotificationEnabled((err,data) => {
            console.log("==========================>ActsGetEnable_test_1100 success=======================>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
        })
        done();
    })
    /*
     * @tc.number: ActsGetEnable_test_1200
     * @tc.name: isNotificationEnabled()
     * @tc.desc: verify the function of isNotificationEnabled
     */
    it('ActsGetEnable_test_1200', 0, async function (done) {
        var promise = await notify.isNotificationEnabled()
        console.log("==========================>ActsGetEnable_test_1200 success=======================>"+promise)
        expect(typeof(promise)).assertEqual('boolean')
        done();
    })
    /*
     * @tc.number: ActsSetEnable_test_0100
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0100', 0, async function (done) {
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },100,(err) => {
            console.log("===>ActsSetEnable_test_0100 success===>"+err)
        })
        done();
    })
    /*
     * @tc.number: ActsSetEnable_test_0200
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0200', 0, async function (done) {
        var promise =  await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },100)
        expect(promise).assertEqual(undefined)
        done();
    })
    /*
     * @tc.number: ActsSetEnable_test_0300
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0300', 0, async function (done) {
        await notify.enableNotification({
            bundle:"Wrong BundleName"
        },true,(err) => {
            console.log("===>ActsSetEnable_test_0300 success===>"+err.code)
            expect(err.code).assertEqual(ERR_ANS_INVALID_BUNDLE)
        })
        done();
    })
    /*
     * @tc.number: ActsSetEnable_test_0400
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0400', 0, async function (done) {
       notify.enableNotification({
            bundle:"Wrong BundleName"
        },true).then().catch((err)=>{
           console.log("===>ActsSetEnable_test_0400 err===>"+err.code)
           expect(err.code != 0).assertEqual(true);
           done();
       })
    })
    /*
     * @tc.number: ActsSetEnable_test_0500
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0500', 0, async function (done) {
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },true,async(err) => {
            await notify.isNotificationEnabled({
                bundle:"com.example.actsanslocalnotificationtest",
            },(err,data) => {
                console.log("===>ActsSetEnable_test_0500 success===>"+err+data)
                expect(typeof(data)).assertEqual('boolean')
                expect(data).assertEqual(true)
                done();
            })
        })
        setTimeout(function(){
            console.debug("====>time out ActsGetDisplay_test_0900====>");
        }, time);
    })
    /*
     * @tc.number: ActsSetEnable_test_0600
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0600', 0, async function (done) {
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },true).then(async(err) => {
            var promise = await notify.isNotificationEnabled({
                bundle:"com.example.actsanslocalnotificationtest",
            })
            console.log("===>ActsSetEnable_test_0600 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(true)
            done();
        })
        setTimeout(function(){
            console.debug("===>time out ActsSetEnable_test_0600===>");
        }, time);
    })
    /*
     * @tc.number: ActsSetEnable_test_0700
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0700', 0, async function (done) {
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },false,async(err) => {
            await notify.isNotificationEnabled({
                bundle:"com.example.actsanslocalnotificationtest",
            },(err,data) => {
                console.log("===>ActsSetEnable_test_0700 success===>"+err+data)
                expect(typeof(data)).assertEqual('boolean')
                expect(data).assertEqual(false)
                done();
            })
        })
        setTimeout(function(){
            console.debug("====>time out ActsSetEnable_test_0700====>");
        }, time);
    })
    /*
     * @tc.number: ActsSetEnable_test_0800
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0800', 0, async function (done) {
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },false).then(async(err) => {
            var promise = await notify.isNotificationEnabled({
                bundle:"com.example.actsanslocalnotificationtest",
            })
            console.log("===>ActsSetEnable_test_0800 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(false)
            done();
        })
        setTimeout(function(){
            console.debug("===>time out ActsSetEnable_test_0800===>");
        }, time);
    })
    /*
     * @tc.number: ActsSetEnable_test_0900
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetEnable_test_0900', 0, async function (done) {
        await notify.isNotificationEnabled({
            bundle:"com.example.actsanslocalnotificationtest",
        },(err,data) => {
            console.log("===>ActsSetEnable_test_0900 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
            expect(data).assertEqual(false)
        })
        await notify.enableNotification({
            bundle:"com.example.actsanslocalnotificationtest"
        },true).then(async(err) => {
            var promise = await notify.isNotificationEnabled({
                bundle:"com.example.actsanslocalnotificationtest",
            })
            console.log("===>ActsSetEnable_test_0900 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(true)
            done();
        })
        setTimeout(function(){
            console.debug("===>time out ActsSetEnable_test_0900===>");
        }, time);
    })
})

