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
var ERR_ANS_INVALID_PARAM = 67108867
describe('ActsAnsDoNotDisturbTest', function () {
    console.info("===ActsAnsDoNotDisturbTest start===>");
    function connectCallbacka() {
        console.debug("==>connectCallbacka code==>");
    }
    function subscribeCallbacka(err) {
        console.debug("==>subscribeCallbacka code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbacka(err){
        console.debug("==>unSubscribeCallbacka code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function connectCallbackb() {
        console.debug("==>connectCallbackb code==>");
    }
    function subscribeCallbackb(err) {
        console.debug("==>subscribeCallbackb code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackb(err){
        console.debug("==>unSubscribeCallbackb code==>" +err.code);
        expect(err.code).assertEqual(0);
    }

    /*
     * @tc.number: ActsGetDoNotDisturbTest_test_0100
     * @tc.name: getDoNotDisturbDate()
     * @tc.desc: verify the function of getDoNotDisturbDate
     */
    it('ActsGetDoNotDisturbTest_test_0100', 0, async function (done) {
        await notify.getDoNotDisturbDate((err,data) => {
            console.log("===>ActsGetDoNotDisturbTest_test_0100 success===>"+JSON.stringify(data))
            expect(data.type).assertEqual(0);
            expect(data.begin.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
            expect(data.end.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
            done();
        })
    })

    /*
     * @tc.number: ActsGetDoNotDisturbTest_test_0200
     * @tc.name: getDoNotDisturbDate()
     * @tc.desc: verify the function of getDoNotDisturbDate
     */
    it('ActsGetDoNotDisturbTest_test_0200', 0, async function (done) {
       notify.getDoNotDisturbDate().then((promise)=>{
                 console.log("===>test_0200 success===>"+JSON.stringify(promise))
                 expect(promise.type).assertEqual(0);
                 expect(promise.begin.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
                 expect(promise.end.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
                 done()}
               ).catch((err)=>{
           console.log("===>ActsGetDoNotDisturbTest_test_0200 err===>"+JSON.stringify(err))
       })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_0300
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_0300', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+8,
                               beginDate.getMinutes())
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_NONE,
            begin:beginDate,
            end:endDate
        },async(err) => {
            console.log("===>test_0300 success===>"+err.code)
            await notify.getDoNotDisturbDate((err,data)=>{
                console.log("===>test_0300 getDoNotDisturbDate===>"+err.code+JSON.stringify(data))
                expect(data.type).assertEqual(0);
                expect(data.begin.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
                expect(data.end.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_0400
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_0400', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+8,
                               beginDate.getMinutes()+20)
        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_NONE,
            begin:beginDate,
            end:endDate
        }).then(()=>{
            notify.getDoNotDisturbDate().then((data)=>{
                console.log("===>test_0400 getDoNotDisturbDate===>"+JSON.stringify(data));
                expect(data.type).assertEqual(0);
                expect(data.begin.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
                expect(data.end.toString()).assertEqual("Thu Jan 01 1970 00:00:00 GMT+0000");
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_0500
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_0500', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+8,
                               beginDate.getMinutes())
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_ONCE,
            begin:beginDate,
            end:endDate
        },async(err) => {
            console.log("===>test_0500 success===>"+err.code);
            await notify.getDoNotDisturbDate(async (err,data)=>{
                console.log("===>test_0500 getDoNotDisturbDate===>"+err.code+JSON.stringify(data))
                expect(data.type).assertEqual(1);
                var DoNotDateE = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                console.log("===>getDoNotDisturbDate DoNotDateE===>"+DoNotDateE)
                console.log("===>getDoNotDisturbDate beginDate===>"+data.begin)
                console.log("===>getDoNotDisturbDate endDate===>"+data.end)
                expect(data.begin.toString()).assertEqual(DoNotDateE.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_0600
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_0600', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+6,
                               beginDate.getMinutes())
        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_ONCE,
            begin:beginDate,
            end:endDate
        }).then(()=>{
            notify.getDoNotDisturbDate().then(async(data)=>{
                console.log("===>test_0600 success===>"+JSON.stringify(data))
                expect(data.type).assertEqual(1);
                var DoNotDateF = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                console.log("===>test_0600 begin===>"+DoNotDateF)
                console.log("===>test_0600 beginDate===>"+data.begin)
                console.log("===>test_0600 endDate===>"+data.end)
                expect(data.begin.toString()).assertEqual(DoNotDateF.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_0700
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_0700', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+7,
                               beginDate.getMinutes()+36)
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        },async(err) => {
            console.log("===>test_0700 success===>"+err.code)
            await notify.getDoNotDisturbDate(async(err,data)=>{
                console.log("===>test_0700 getDoNotDisturbDate===>"+JSON.stringify(data))
                expect(data.type).assertEqual(2);
                var DoNotDateG = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                console.log("===>0700 begin===>"+DoNotDateG)
                console.log("===>0700 beginDate===>"+data.begin)
                console.log("===>0700 endDate===>"+data.end)
                expect(data.begin.toString()).assertEqual(DoNotDateG.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_0800
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDoNotDisturbTest_test_0800', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+13,
                               beginDate.getMinutes()+48)
        console.log("===>ActsSetDoNotDisturbTest_test_0800 beginDate ===>"+beginDate)
        console.log("===>ActsSetDoNotDisturbTest_test_0800 endDate ===>"+endDate)
        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        }).then(()=>{
            notify.getDoNotDisturbDate().then(async(data)=>{
                console.log("===>test_0800 success===>"+JSON.stringify(data))
                expect(data.type).assertEqual(2);
                var DoNotDateH = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                console.log("===>0800 begin===>"+DoNotDateH)
                console.log("===>0800 beginDate===>"+data.begin)
                console.log("===>0800 endDate===>"+data.end)
                expect(data.begin.toString()).assertEqual(DoNotDateH.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_0900
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_0900', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+4,
                               beginDate.getMinutes())
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        },async(err) => {
            console.log("===>ActsSetDoNotDisturbTest_test_0900 success===>"+err.code)
            await notify.getDoNotDisturbDate(async(err,data)=>{
                console.log("===>test_0900 getDoNotDisturbDate===>"+JSON.stringify(data))
                expect(data.type).assertEqual(3);
                var DoNotDateI = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                console.log("===>test_0900 begin===>"+DoNotDateI)
                console.log("===>test_0900 beginDate===>"+data.begin)
                console.log("===>test_0900 endDate===>"+data.end)
                expect(data.begin.toString()).assertEqual(DoNotDateI.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1000
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDoNotDisturbTest_test_1000', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+6,
                               beginDate.getMinutes()+32)
        console.log("===>test_1000 beginDate ===>"+beginDate)
        console.log("===>test_1000 endDate ===>"+endDate)
        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        }).then(()=>{
            notify.getDoNotDisturbDate().then(async(data)=>{
                console.log("===>test_1000 getDoNotDisturbDate===>"+JSON.stringify(data))
                expect(data.type).assertEqual(3);
                var DoNotDateJ = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                console.log("===>test_1000 begin===>"+DoNotDateJ)
                console.log("===>test_1000 beginDate===>"+data.begin)
                console.log("===>test_1000 endDate===>"+data.end)
                expect(data.begin.toString()).assertEqual(DoNotDateJ.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1100
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_1100', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date()
        console.log("===>test_1100 beginDate ===>"+beginDate)
        console.log("===>test_1100 endDate ===>"+endDate)
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        },async(err) => {
            console.log("===>test_1100 success===>"+err.code)
            await notify.getDoNotDisturbDate(async(err,data)=>{
                console.log("===>test_1100 getDoNotDisturbDate===>"+err.code+JSON.stringify(data))
                expect(data.type).assertEqual(2);
                var DoNotDateK = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                expect(data.begin.toString()).assertEqual(DoNotDateK.toString());
                expect(data.end.toString()).assertEqual(DoNotDateK.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1100
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_1200', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date()
        console.log("===>test_1200 beginDate ===>"+beginDate)
        console.log("===>test_1200 endDate ===>"+endDate)
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        }).then(()=>{
            notify.getDoNotDisturbDate(async(err,data)=>{
                console.log("===>test_1200 getDoNotDisturbDate===>"+err.code+JSON.stringify(data));
                expect(data.type).assertEqual(2);
                var DoNotDateL = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                expect(data.begin.toString()).assertEqual(DoNotDateL.toString());
                expect(data.end.toString()).assertEqual(DoNotDateL.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        }).catch(async(err)=>{
            console.log("===>test_1200 getDoNotDisturbDate fail===>"+JSON.stringify(err))
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1300
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_1300', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate()-2,
                               beginDate.getHours()+6,
                               beginDate.getMinutes()+32)
        console.log("===>test_1300 beginDate ===>"+beginDate)
        console.log("===>test_1300 endDate ===>"+endDate)
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        },async(err) => {
            console.log("===>test_1300 success===>"+err.code)
            await notify.getDoNotDisturbDate(async(err,data)=>{
                console.log("===>test_1300 getDoNotDisturbDate success===>"+err.code+JSON.stringify(data))
                expect(data.type).assertEqual(2);
                var DoNotDateM = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                expect(data.begin.toString()).assertEqual(DoNotDateM.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1400
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_1400', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate()-1,
                               beginDate.getHours()+6,
                               beginDate.getMinutes()+32)
        console.log("===>test_1400 beginDate ===>"+beginDate)
        console.log("===>test_1400 endDate ===>"+endDate)
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        }).then(()=>{
            notify.getDoNotDisturbDate(async(err,data)=>{
                console.log("===>test_1400 getDoNotDisturbDate===>"+err.code+JSON.stringify(data))
                expect(data.type).assertEqual(2);
                var DoNotDateN = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                expect(data.begin.toString()).assertEqual(DoNotDateN.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.setDoNotDisturbDate({
                    type:notify.DoNotDisturbType.TYPE_NONE,
                    begin:beginDate,
                    end:endDate
                });
                done();
            })
        }).catch(async(err)=>{
            console.log("===>test_1400 getDoNotDisturbDate fail===>"+JSON.stringify(err))
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1500
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_1500', 0, async function (done) {
        var beginDate = new Date(2021,11,22,12,18);
        var endDate = new Date(2021,11,22,12,18);
        console.log("===>test_1500 beginDate ===>"+beginDate);
        console.log("===>test_1500 endDate ===>"+endDate);
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        },async(err) => {
            console.log("===>ActsSetDoNotDisturbTest_test_1500 success===>"+err.code);
            expect(err.code).assertEqual(ERR_ANS_INVALID_PARAM);
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1600
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_1600', 0, async function (done) {
        var beginDate = new Date(2021,11,22,12,18);
        var endDate = new Date(2021,11,22,12,18);
        console.log("===>test_1600 beginDate ===>"+beginDate);
        console.log("===>test_1600 endDate ===>"+endDate);
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        }).then().catch(async(err)=>{
            console.log("===>test_1600 fail===>"+JSON.stringify(err));
            expect(err.code).assertEqual(ERR_ANS_INVALID_PARAM);
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1700
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDoNotDisturbTest_test_1700', 0, async function (done) {
        var beginDate = new Date(2021,11,22,20,18);
        var endDate = new Date(2021,11,22,2,18);
        console.log("===>test_1700 beginDate ===>"+beginDate);
        console.log("===>test_1700 endDate ===>"+endDate);
        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        },async(err)=>{
            console.log("===>test_1700 setDoNotDisturbDate===>"+JSON.stringify(err));
            expect(err.code).assertEqual(ERR_ANS_INVALID_PARAM);
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
      })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1800
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDoNotDisturbTest_test_1800', 0, async function (done) {
        var beginDate = new Date(2021,11,22,20,18);
        var endDate = new Date(2021,11,22,2,18);
        console.log("===>test_1800 beginDate ===>"+beginDate);
        console.log("===>test_1800 endDate ===>"+endDate);
        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        }).then().catch(async(err)=>{
            console.log("===>test_1800 setDoNotDisturbDate fail===>"+JSON.stringify(err))
            expect(err.code).assertEqual(ERR_ANS_INVALID_PARAM);
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_1900
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_1900', 0, async function (done) {
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:100,
            end:100
        },async(err) => {
            console.log("===>test_1900 success===>"+err.code);
            await notify.getDoNotDisturbDate((err,data)=>{
                console.log("===>test_1900 getDoNotDisturbDate success===>"+err.code+JSON.stringify(data));
            })
        })
        done();
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_2000
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDoNotDisturbTest_test_2000', 0, async function (done) {
        var promise = notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:100,
            end:100
        })
        console.log("===>ActsSetDoNotDisturbTest_test_2000 promise===>"+promise);
        expect(promise).assertEqual(undefined);
        done();
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_2100
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_2100', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+11,
                               beginDate.getMinutes()+26)
        var subInfo ={
            onConnect:connectCallbacka,
            onDoNotDisturbDateChange:async(data)=>{
                console.debug("==>disturbModeCallbacka data==>" +JSON.stringify(data));
                expect(data.type).assertEqual(3);
                var DoNotDateO = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                expect(data.begin.toString()).assertEqual(DoNotDateO.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.getDoNotDisturbDate(async(err,data)=>{
                    console.log("===>test_2100 getDoNotDisturbDate===>"+err.code+JSON.stringify(data))
                    notify.unsubscribe(subInfo, unSubscribeCallbacka);
                    expect(data.type).assertEqual(3);
                    var DoNotDateP = new Date(beginDate.getFullYear(),
                                              beginDate.getMonth(),
                                              beginDate.getDate(),
                                              beginDate.getHours(),
                                              beginDate.getMinutes())
                    expect(data.begin.toString()).assertEqual(DoNotDateP.toString());
                    expect(data.end.toString()).assertEqual(endDate.toString());
                    await notify.setDoNotDisturbDate({
                        type:notify.DoNotDisturbType.TYPE_NONE,
                        begin:beginDate,
                        end:endDate
                    });
                    done();
                })
            },
        }
        await notify.subscribe(subInfo,subscribeCallbacka);
        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        },async (err) => {
            console.log("===>test_2100 success===>"+err.code);
        })
    })

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_2200
     * @tc.name: displayBadge()
     * @tc.desc: verify the function of displayBadge
     */
    it('ActsSetDoNotDisturbTest_test_2200', 0, async function (done) {
        var beginDate = new Date()
        var endDate = new Date(beginDate.getFullYear(),
                               beginDate.getMonth(),
                               beginDate.getDate(),
                               beginDate.getHours()+16,
                               beginDate.getMinutes()+45)
        var subInfo ={
            onConnect:connectCallbackb,
            onDoNotDisturbDateChange:async(data)=>{
                console.debug("==>disturbModeCallbackb data==>" +JSON.stringify(data));
                expect(data.type).assertEqual(3);
                var DoNotDateQ = new Date(beginDate.getFullYear(),
                                          beginDate.getMonth(),
                                          beginDate.getDate(),
                                          beginDate.getHours(),
                                          beginDate.getMinutes())
                expect(data.begin.toString()).assertEqual(DoNotDateQ.toString());
                expect(data.end.toString()).assertEqual(endDate.toString());
                await notify.getDoNotDisturbDate().then(async(data)=>{
                    console.log("===>test_2200 getDoNotDisturbDate===>"+JSON.stringify(data));
                    notify.unsubscribe(subInfo, unSubscribeCallbackb);
                    expect(data.type).assertEqual(3);
                    var DoNotDateR = new Date(beginDate.getFullYear(),
                                              beginDate.getMonth(),
                                              beginDate.getDate(),
                                              beginDate.getHours(),
                                              beginDate.getMinutes())
                    expect(data.begin.toString()).assertEqual(DoNotDateR.toString());
                    expect(data.end.toString()).assertEqual(endDate.toString());
                    await notify.setDoNotDisturbDate({
                        type:notify.DoNotDisturbType.TYPE_NONE,
                        begin:beginDate,
                        end:endDate
                    });
                    done();
                })
            },
        }
        await notify.subscribe(subInfo,subscribeCallbackb);
        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY,
            begin:beginDate,
            end:endDate
        }).then(()=>{
            console.log("===>test_2200 setDoNotDisturbDate===>")
        })
    })

    /*
     * @tc.number: ActsGetDoNotDisturbTest_test_2300
     * @tc.name: supportDoNotDisturbMode()
     * @tc.desc: verify the function of supportDoNotDisturbMode
     */
    it('ActsGetDoNotDisturbTest_test_2300', 0, async function (done) {
        await notify.supportDoNotDisturbMode((err,data) => {
            console.log("===>test_2300 success===>"+JSON.stringify(err)+data);
            expect(data).assertEqual(true);
            done();
        })
    })

    /*
     * @tc.number: ActsGetDoNotDisturbTest_test_2400
     * @tc.name: supportDoNotDisturbMode()
     * @tc.desc: verify the function of supportDoNotDisturbMode
     */
    it('ActsGetDoNotDisturbTest_test_2400', 0, async function (done) {
        notify.supportDoNotDisturbMode().then((promise)=>{
            console.log("===>ActsGetDoNotDisturbTest_test_2400 success===>"+promise);
            expect(promise).assertEqual(true);
            done()}
        ).catch((err)=>{
            console.log("===>test_2400 err===>"+err.code);
        })
    })

    function addSlotCallback(err){
        console.log("addSlotCallback err==>"+JSON.stringify(err));
    }

    function onDoNotDisturbDateChange(mode){
        console.log("===test_2500 onDisturbModeChange mode===>"+JSON.stringify(mode));
        expect(mode.type).assertEqual(2);
    }

    function onConsume(data){
        console.log("===test_2500 onConsume===>"+JSON.stringify(data));
        var hashCode = data.request.hashCode
        var sound = data.sortingMap.sortings[hashCode].slot.sound
        console.log("===test_2500 onConsume sound===>"+sound);
        expect(sound).assertEqual("normalSocialMusic");
        var vibra = data.sortingMap.sortings[hashCode].slot.vibrationValues
        console.log("===test_2500 onConsume vibra===>"+vibra);
        expect(JSON.stringify(vibra)).assertEqual(JSON.stringify([1,0,1,0,1,0]));
        console.log("===test_2500 onConsume end===>");
    }
    function publishCallback(err){
        console.log("===publishCallback===>"+JSON.stringify(err));
    }

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_2500
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_2500', 0, async function (done) {
        notify.addSlot({
            type:notify.SlotType.SOCIAL_COMMUNICATION,
            level:notify.SlotLevel.LEVEL_NONE,
            desc:"SOCIAL_COMMUNICATION_desc",
            badgeFlag:true,
            bypassDnd:false,
            lightEnabled:true,
            lightColor:1,
            vibrationValues:[1,0,1,0,1,0],
            sound:"normalSocialMusic",
        },addSlotCallback);

        var subscriber ={
            onConsume:onConsume,
            onDoNotDisturbDateChange:onDoNotDisturbDateChange,
        }

        await notify.subscribe(subscriber);

        var beginDate = new Date(2021,11,23,0,0);
        var endDate = new Date(2021,11,23,23,59);

        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        },(err) => {
            console.log("===test_2500 setDoNotDisturbDate===>"+err.code)
        })

        await notify.publish({
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 21,
            slotType : notify.SlotType.SOCIAL_COMMUNICATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "normalSocialVibra_statusBarText",
            isFloatingIcon : true,
            label: "normalSocialVibra_label",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        },publishCallback);
        setTimeout((async function(){
            console.info("===test_2500 setTimeout===>");
            await notify.unsubscribe(subscriber);
            console.info("===test_2500 setTimeout unsubscribe===>");
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            console.info("====test_2500 setDoNotDisturbDate removeSlot============>");
            await notify.removeSlot(notify.SlotType.SOCIAL_COMMUNICATION);
            console.info("====test_2500 setTimeout removeSlot============>");
            done();
        }),500);
    })

    function onDoNotDisturbDateChange2600(mode){
        console.log("===test_2600 onDisturbModeChange mode===>"+JSON.stringify(mode));
        expect(mode.type).assertEqual(2);
    }

    function onConsume2600(data){
        console.log("===test_2600 onConsume===>"+JSON.stringify(data));
        var hashCode = data.request.hashCode
        var sound = data.sortingMap.sortings[hashCode].slot.sound
        console.log("===test_2600 onConsume sound===>"+sound);
        expect(sound).assertEqual("normalSocialMusic");
        var vibra = data.sortingMap.sortings[hashCode].slot.vibrationValues
        console.log("===test_2600 onConsume vibra===>"+vibra);
        expect(JSON.stringify(vibra)).assertEqual(JSON.stringify([1,0,1,0,1,0]));
        console.log("===test_2600 onConsume end===>");
    }

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_2600
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_2600', 0, async function (done) {
        notify.addSlot({
            type:notify.SlotType.SOCIAL_COMMUNICATION,
            level:notify.SlotLevel.LEVEL_NONE,
            desc:"SOCIAL_COMMUNICATION_desc",
            badgeFlag:true,
            bypassDnd:false,
            lightEnabled:true,
            lightColor:1,
            vibrationValues:[1,0,1,0,1,0],
            sound:"normalSocialMusic",
        },addSlotCallback);

        var subscriber ={
            onConsume:onConsume2600,
            onDoNotDisturbDateChange:onDoNotDisturbDateChange2600,
        }

        await notify.subscribe(subscriber);

        var beginDate = new Date(2021,11,23,0,0);
        var endDate = new Date(2021,11,23,23,59);

        notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_DAILY,
            begin:beginDate,
            end:endDate
        },(err)=>{
            console.log("===test_2600 setDoNotDisturbDate===>"+err.code)
        })

        await notify.publish({
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 22,
            slotType : notify.SlotType.SOCIAL_COMMUNICATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "normalSocialVibra_statusBarText",
            isFloatingIcon : true,
            label: "normalSocialVibra_label",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        },publishCallback);
        setTimeout((async function(){
            console.info("====test_2600 setTimeout====>");
            await notify.unsubscribe(subscriber);
            console.info("====test_2600 setTimeout unsubscribe====>");
            await notify.removeSlot(notify.SlotType.SOCIAL_COMMUNICATION);
            console.info("====test_2600 setTimeout removeSlot============>");
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            console.info("====test_2600 setTimeout setDoNotDisturbDate============>");
            done();
        }),500);
    })

    function onDoNotDisturbDateChange2700(mode){
        console.log("===test_2700 onDisturbModeChange mode===>"+JSON.stringify(mode));
        expect(mode.type).assertEqual(3);
    }

    function onConsume2700(data){
        console.log("===test_2700 onConsume===>"+JSON.stringify(data));
        var hashCode = data.request.hashCode
        var sound = data.sortingMap.sortings[hashCode].slot.sound
        console.log("===test_2700 onConsume sound===>"+sound);
        expect(sound).assertEqual("normalSocialMusic");
        var vibra = data.sortingMap.sortings[hashCode].slot.vibrationValues
        console.log("===test_2700 onConsume vibra===>"+vibra);
        expect(JSON.stringify(vibra)).assertEqual(JSON.stringify([1,0,1,0,1,0]));
        console.log("===test_2700 onConsume end===>");
    }

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_2700
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_2700', 0, async function (done) {
        notify.addSlot({
            type:notify.SlotType.SOCIAL_COMMUNICATION,
            level:notify.SlotLevel.LEVEL_NONE,
            desc:"SOCIAL_COMMUNICATION_desc",
            badgeFlag:true,
            bypassDnd:false,
            lightEnabled:true,
            lightColor:1,
            vibrationValues:[1,0,1,0,1,0],
            sound:"normalSocialMusic",
        },addSlotCallback);

        var subscriber ={
            onConsume:onConsume2700,
                onDoNotDisturbDateChange:onDoNotDisturbDateChange2700,
        }

        await notify.subscribe(subscriber);

        var beginDate = new Date(2020,1,1,0,0);
        var endDate = new Date(2099,12,31,23,59);

        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY ,
            begin:beginDate,
            end:endDate
        },(err) => {
            console.log("===test_2700 setDoNotDisturbDate===>"+err.code)
        })

        await notify.publish({
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 23,
            slotType : notify.SlotType.SOCIAL_COMMUNICATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "normalSocialVibra_statusBarText",
            isFloatingIcon : true,
            label: "normalSocialVibra_label",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        },publishCallback);
        setTimeout((async function(){
            console.info("====test_2700 setTimeout====>");
            await notify.unsubscribe(subscriber);
            console.info("====test_2700 setTimeout unsubscribe============>");
            await notify.removeSlot(notify.SlotType.SOCIAL_COMMUNICATION);
            console.info("====test_2700 setTimeout removeSlot============>");
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
        }),500);
    })

    function onDoNotDisturbDateChange2800(mode){
        console.log("===test_2800 onDisturbModeChange mode===>"+JSON.stringify(mode));
        expect(mode.type).assertEqual(3);
    }

    function onConsume2800(data){
        console.log("===test_2800 onConsume===>"+JSON.stringify(data));
        var hashCode = data.request.hashCode
        var sound = data.sortingMap.sortings[hashCode].slot.sound
        console.log("===test_2800 onConsume sound===>"+sound);
        expect(sound).assertEqual("normalSocialMusic");
        var vibra = data.sortingMap.sortings[hashCode].slot.vibrationValues
        console.log("===test_2800 onConsume vibra===>"+vibra);
        expect(JSON.stringify(vibra)).assertEqual(JSON.stringify([1,0,1,0,1,0]));
        console.log("===test_2800 onConsume end===>");
    }

    /*
     * @tc.number: ActsSetDoNotDisturbTest_test_2800
     * @tc.name: setDoNotDisturbDate()
     * @tc.desc: verify the function of setDoNotDisturbDate
     */
    it('ActsSetDoNotDisturbTest_test_2800', 0, async function (done) {
        notify.addSlot({
            type:notify.SlotType.SOCIAL_COMMUNICATION,
            level:notify.SlotLevel.LEVEL_NONE,
            desc:"SOCIAL_COMMUNICATION_desc",
            badgeFlag:true,
            bypassDnd:false,
            lightEnabled:true,
            lightColor:1,
            vibrationValues:[1,0,1,0,1,0],
            sound:"normalSocialMusic",
        },addSlotCallback);

        var subscriber ={
            onConsume:onConsume2800,
            onDoNotDisturbDateChange:onDoNotDisturbDateChange2800,
        }

        await notify.subscribe(subscriber);

        var beginDate = new Date(2021,11,23,0,0);
        var endDate = new Date(2021,11,23,23,59);

        await notify.setDoNotDisturbDate({
            type:notify.DoNotDisturbType.TYPE_CLEARLY ,
            begin:beginDate,
            end:endDate
        },(err) => {
            console.log("===test_2800 setDoNotDisturbDate===>"+err.code)
        })

        await notify.publish({
            content:{
                contentType: notify.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
                normal: {
                    title: "test_title",
                    text: "test_text",
                    additionalText: "test_additionalText"
                },
            },
            id: 24,
            slotType : notify.SlotType.SOCIAL_COMMUNICATION,
            isOngoing : true,
            isUnremovable : false,
            deliveryTime : 1624950453,
            tapDismissed : true,
            autoDeletedTime: 1625036817,
            color: 2,
            colorEnabled: true,
            isAlertOnce: true,
            isStopwatch: true,
            isCountDown: true,
            progressValue: 12,
            progressMaxValue: 100,
            isIndeterminate: true,
            statusBarText: "normalSocialVibra_statusBarText",
            isFloatingIcon : true,
            label: "normalSocialVibra_label",
            badgeIconStyle: 1,
            showDeliveryTime: true,
        },publishCallback);
        setTimeout((async function(){
            console.info("======test_2800 setTimeout============>");
            await notify.unsubscribe(subscriber);
            console.info("======test_2800 setTimeout unsubscribe============>");
            await notify.removeSlot(notify.SlotType.SOCIAL_COMMUNICATION);
            await notify.setDoNotDisturbDate({
                type:notify.DoNotDisturbType.TYPE_NONE,
                begin:beginDate,
                end:endDate
            });
            done();
        }),500);
    })
})


