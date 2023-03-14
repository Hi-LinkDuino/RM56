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
describe('ActsAnsBadgeDisplayTest', function () {
    console.info("===ActsAnsBadgeDisplayTest start===>");
    /*
     * @tc.number: ActsGetDisplay_test_0100
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0100', 0, async function (done) {
        await notify.isBadgeDisplayed({
            bundle:"com.example.actsanslocalcandisplaytest",
        },(err,data) => {
            console.log("===>ActsGetDisplay_test_0100 success===>"+JSON.stringify(err)+data)
            expect(typeof(data)).assertEqual('boolean')
            done();
        })
    })
    /*
     * @tc.number: ActsGetDisplay_test_0200
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0200', 0, async function (done) {
       notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            }).then((promise)=>{
                 console.log("===>ActsGetDisplay_test_0200 success===>"+promise)
                 expect(typeof(promise)).assertEqual('boolean')
                 done()}
               ).catch((err)=>{
           console.log("===>ActsGetDisplay_test_0200 err===>"+err.code)
       })
    })
    /*
     * @tc.number: ActsGetDisplay_test_0300
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0300', 0, async function (done) {
        await notify.isBadgeDisplayed({
            bundle:"wrong BundleName",
        },(err,data) => {
            console.log("===>ActsGetDisplay_test_0300 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
            expect(data).assertEqual(false)
            done();
        })
    })
    /*
     * @tc.number: ActsGetDisplay_test_0400
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0400', 0, async function (done) {
      notify.isBadgeDisplayed({
            bundle:"wrong BundleName",
        }).then().catch((err)=>{
          console.log("===>ActsGetDisplay_test_0400 success===>"+err.code)
           expect(err.code != 0).assertEqual(true);
           done();
       })

    })
    /*
     * @tc.number: ActsGetDisplay_test_0500
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsGetDisplay_test_0500', 0, async function (done) {
        await notify.isBadgeDisplayed("#$#$%$%^",(err,data) => {
            console.log("===>ActsGetDisplay_test_0500 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
        })
        done();
    })
    /*
     * @tc.number: ActsGetDisplay_test_0600
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0600', 0, async function (done) {
        var promise =  await notify.isBadgeDisplayed("#$#$%$%^")
        expect(promise).assertEqual(undefined)
        done();
    })
    /*
     * @tc.number: ActsGetDisplay_test_0700
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0700', 0, async function (done) {
        await notify.isBadgeDisplayed({},(err,data) => {
            console.log("===>ActsGetDisplay_test_0700 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
        })
        done();
    })
    /*
     * @tc.number: ActsGetDisplay_test_0800
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0800', 0, async function (done) {
        var promise =  await notify.isBadgeDisplayed({})
        expect(promise).assertEqual(undefined)
        done();
    })
    /*
     * @tc.number: ActsGetDisplay_test_0900
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_0900', 0, async function (done) {
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },true,async(err) => {
            console.log("===>ActsGetDisplay_test_0900 success===>"+err)
            await notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            },(err,data) => {
                console.log("===>ActsGetDisplay_test_0900 success===>"+err+data)
                expect(typeof(data)).assertEqual('boolean')
                expect(data).assertEqual(true)
                done();
            })
        })
    })
    /*
     * @tc.number: ActsGetDisplay_test_1000
     * @tc.name: isBadgeDisplayed()
     * @tc.desc: verify the function of isBadgeDisplayed
     */
    it('ActsGetDisplay_test_1000', 0, async function (done) {
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },true,async(err) => {
            console.log("===>ActsGetDisplay_test_1000 success===>"+err)
            var promise = await notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            })
            console.log("===>ActsGetDisplay_test_1000 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(true)
            done();
            })
    })
    /*
     * @tc.number: ActsSetDisplay_test_0100
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0100', 0, async function (done) {
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },100,(err) => {
            console.log("===>ActsSetDisplay_test_0100 success===>"+err)
        })
        done();
    })
    /*
     * @tc.number: ActsSetDisplay_test_0200
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0200', 0, async function (done) {
        var promise =  await notify.displayBadge({
                bundle:"com.example.actsanslocalcandisplaytest"
            },100)
        expect(promise).assertEqual(undefined)
        done();
    })
    /*
     * @tc.number: ActsSetDisplay_test_0300
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0300', 0, async function (done) {
        await notify.displayBadge({
            bundle:"Wrong BundleName"
        },true,(err) => {
            console.log("===>ActsSetDisplay_test_0300 success===>"+err.code)
            expect(err.code).assertEqual(ERR_ANS_INVALID_BUNDLE)
        })
        done();
    })
    /*
     * @tc.number: ActsSetDisplay_test_0400
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0400', 0, async function (done) {
        notify.displayBadge({
            bundle:"Wrong BundleName"
        },true).then().catch((err)=>{
            console.log("===>ActsSetDisplay_test_0400 err===>"+err.code)
            expect(err.code != 0).assertEqual(true);
            done();
        })
    })
    /*
     * @tc.number: ActsSetDisplay_test_0500
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0500', 0, async function (done) {
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },true,async(err) => {
            await notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            },(err,data) => {
                console.log("===>ActsSetDisplay_test_0500 success===>"+err+data)
                expect(typeof(data)).assertEqual('boolean')
                expect(data).assertEqual(true)
                done();
            })
        })
    })
    /*
     * @tc.number: ActsSetDisplay_test_0600
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0600', 0, async function (done) {
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },true).then(async(err) => {
            var promise = await notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            })
            console.log("===>ActsSetDisplay_test_0600 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(true)
            done();
        })
    })
    /*
     * @tc.number: ActsSetDisplay_test_0700
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0700', 0, async function (done) {
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },false,async(err) => {
            await notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            },(err,data) => {
                console.log("===>ActsSetDisplay_test_0700 success===>"+err+data)
                expect(typeof(data)).assertEqual('boolean')
                expect(data).assertEqual(false)
                done();
            })
        })
    })
    /*
     * @tc.number: ActsSetDisplay_test_0800
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDisplay_test_0800', 0, async function (done) {
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },false).then(async(err) => {
            var promise = await notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            })
            console.log("===>ActsSetDisplay_test_0800 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(false)
            done();
        })
    })
    /*
     * @tc.number: ActsSetDisplay_test_0900
     * @tc.name: enableNotification()
     * @tc.desc: verify the function of enableNotification
     */
    it('ActsSetDisplay_test_0900', 0, async function (done) {
        await notify.isBadgeDisplayed({
            bundle:"com.example.actsanslocalcandisplaytest",
        },(err,data) => {
            console.log("===>ActsSetDisplay_test_0900 success===>"+err+data)
            expect(typeof(data)).assertEqual('boolean')
            expect(data).assertEqual(false)
        })
        await notify.displayBadge({
            bundle:"com.example.actsanslocalcandisplaytest"
        },true).then(async(err) => {
            var promise = await notify.isBadgeDisplayed({
                bundle:"com.example.actsanslocalcandisplaytest",
            })
            console.log("===>ActsSetDisplay_test_0900 success===>"+promise)
            expect(typeof(promise)).assertEqual('boolean')
            expect(promise).assertEqual(true)
            done();
        })
    })
})


