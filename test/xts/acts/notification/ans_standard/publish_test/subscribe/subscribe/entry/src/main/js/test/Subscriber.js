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
describe('ActsAnsSubscriberTest', function () {
    console.info("==ActsAnsSubscriberTest start==>");
    //ActsSubscriber_test_0100
    var subInfoa ={
        onConsume:consumeCallbackc,
        onConnect:connectCallbacka,
        onDisconnect:disconnectCallbacka,
    }
    function consumeCallbackc(data) {
        console.debug("==>consumeCallbackc data : ==>" + JSON.stringify(data));
        checkConsumeData(data)
        notify.unsubscribe(subInfoa, unSubscribeCallbacka);
    }
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
    function disconnectCallbacka(){
        console.debug("==>disconnectCallbacka code==>");
    }
    //ActsSubscriber_test_0200
    function connectCallbackb() {
        console.debug("==>connectCallbackb code==>");
    }
    function subscribeCallbackb(err) {
        console.debug("==>subscribeCallbackb code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function subscribeCallbackc(err) {
        console.debug("==>subscribeCallbackc code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackb(err){
        console.debug("==>unSubscribeCallbackb code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbackb(){
        console.debug("==>disconnectCallbackb code==>");
    }
    //ActsSubscriber_test_0300
    function connectCallbackc() {
        console.debug("==>connectCallbackc code==>");
    }
    function connectCallbackd() {
        console.debug("==>connectCallbackd code==>");
    }
    function subscribeCallbackd(err) {
        console.debug("==>subscribeCallbackd code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function subscribeCallbacke(err) {
        console.debug("==>subscribeCallbacke code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackc(err){
        console.debug("==>unSubscribeCallbackc code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackd(err){
        console.debug("==>unSubscribeCallbackd code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbackc(){
        console.debug("==>disconnectCallbackc code==>");
    }
    function disconnectCallbackd(){
        console.debug("==>disconnectCallbackd code==>");
    }
    //ActsSubscriber_test_0500
    function connectCallbackf() {
        console.debug("==>connectCallbackf code==>");
    }
    function connectCallbackg() {
        console.debug("==>connectCallbackg code==>");
    }
    function subscribeCallbackg(err) {
        console.debug("==>subscribeCallbackg code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function subscribeCallbackh(err) {
        console.debug("==>subscribeCallbackh code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function subscribeCallbacki(err) {
        console.debug("==>subscribeCallbacki code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackf(err){
        console.debug("==>unSubscribeCallbackf code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackg(err){
        console.debug("==>unSubscribeCallbackg code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbackf(){
        console.debug("==>disconnectCallbackf code==>");
    }
    function disconnectCallbackg(){
        console.debug("==>disconnectCallbackg code==>");
    }
    //ActsSubscriber_test_0600
    var subInfob = {
        onConsume:consumeCallbacka,
        onConnect:connectCallbacki,
        onDisconnect:disconnectCallbacki,
    }
    function consumeCallbacka(data) {
        console.debug("==>consumeCallbacka data : ==>" + JSON.stringify(data));
        checkConsumeData(data)
        notify.unsubscribe(subInfob, unSubscribeCallbacki);
    }
    function connectCallbacki() {
        console.debug("==>connectCallbacki code==>");
    }
    function subscribeCallbackl(err) {
        console.debug("==>subscribeCallbackl code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbacki(err){
        console.debug("==>unSubscribeCallbacki code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbacki(){
        console.debug("==>disconnectCallbacki code==>");
    }
    //ActsSubscriber_test_0700
    var subInfoc ={
        onConsume:consumeCallbackb,
        onConnecte:connectCallbackj,
        onDisconnect:disconnectCallbackj,
    }
    function consumeCallbackb(data) {
        console.debug("==>consumeCallbackb data : ==>" + JSON.stringify(data));
        checkConsumeData(data)
        notify.unsubscribe(subInfoc, unSubscribeCallbackj);
    }
    function connectCallbackj() {
        console.debug("==>connectCallbackj code==>");
    }
    function unSubscribeCallbackj(err){
        console.debug("==>unSubscribeCallbackj code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbackj(){
        console.debug("==>disconnectCallbackj code==>");
    }
    //ActsSubscriber_test_0800
    var subInfod ={
        onConsume:consumeCallbackd,
        onConnect:connectCallbackm,
        onDisconnect:disconnectCallbackl,
    }
    function consumeCallbackd(data) {
        console.debug("==>consumeCallbackd data : ==>" + JSON.stringify(data));
        checkConsumeData(data)
        notify.unsubscribe(subInfod, unSubscribeCallbackl);
    }
    function connectCallbackm() {
        console.debug("==>connectCallbackm code==>");
    }
    function subscribeCallbackn(err) {
        console.debug("==>subscribeCallbackn code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackl(err){
        console.debug("==>unSubscribeCallbackl code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbackl(){
        console.debug("==>disconnectCallbackl code==>");
    }
    //ActsSubscriber_test_0900
    var subInfoe ={
        onConsume:consumeCallbacke,
        onConnect:connectCallbackn,
        onDisconnect:disconnectCallbackm,
    }
    function consumeCallbacke(data) {
        console.debug("==>consumeCallbacke data : ==>" + JSON.stringify(data));
        checkConsumeData(data)
        notify.unsubscribe(subInfoe, unSubscribeCallbackm);
    }
    function connectCallbackn() {
        console.debug("==>connectCallbackn code==>");
    }
    function unSubscribeCallbackm(err){
        console.debug("==>unSubscribeCallbackm code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbackm(){
        console.debug("==>disconnectCallbackm code==>");
    }
    //ActsSubscriber_test_1400
    function connectCallbackl(){
        console.debug("==>connectCallbackl code==>");
    }
    function subscribeCallbacko(err){
        console.debug("==>subscribeCallbacko code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbackn(err){
        console.debug("==>unSubscribeCallbackn code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbackn(){
        console.debug("==>disconnectCallbackn code==>");
    }
    //ActsSubscriber_test_1500
    function connectCallbacko(){
        console.debug("==>connectCallbacko  code==>");
    }
    function subscribeCallbackp(err){
        console.debug("==>subscribeCallbackp code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function unSubscribeCallbacko(err){
        console.debug("==>unSubscribeCallbacko code==>" +err.code);
        expect(err.code).assertEqual(0);
    }
    function disconnectCallbacko(){
        console.debug("==>disconnectCallbacko code==>");
    }
    function checkConsumeData(data){
        expect(data.request.id).assertEqual(1);
        expect(data.request.content.contentType).assertEqual(0);
        expect(data.request.content.normal.title).assertEqual("test1_title");
        expect(data.request.content.normal.text).assertEqual("test1_text");
        expect(data.request.content.normal.additionalText).assertEqual("test1_additionalText");
        expect(data.request.slotType).assertEqual(65535);
        expect(data.request.isOngoing).assertEqual(true);
        expect(data.request.isUnremovable).assertEqual(false);
        expect(data.request.deliveryTime).assertEqual(1624950453);
        expect(data.request.tapDismissed).assertEqual(true);
        expect(data.request.autoDeletedTime).assertEqual(1625036817);
        expect(data.request.color).assertEqual(2);
        expect(data.request.colorEnabled).assertEqual(true);
        expect(data.request.isAlertOnce).assertEqual(true);
        expect(data.request.isStopwatch).assertEqual(true);
        expect(data.request.isCountDown).assertEqual(true);
        expect(data.request.progressValue).assertEqual(12);
        expect(data.request.progressMaxValue).assertEqual(100);
        expect(data.request.isIndeterminate).assertEqual(true);
        expect(data.request.statusBarText).assertEqual("statusBarText");
        expect(data.request.isFloatingIcon).assertEqual(true);
        expect(data.request.label).assertEqual("0100_1");
        expect(data.request.badgeIconStyle).assertEqual(1);
        expect(data.request.showDeliveryTime).assertEqual(true);
    }

    /*
     * @tc.number: ActsSubscriber_test_0100
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0100', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0100==begin==>");
        await notify.subscribe(subInfoa, subscribeCallbacka);
        console.debug("==ActsSubscriber_test_0100==end==>");
        done();
    })

    /*
     * @tc.number: ActsSubscriber_test_0200
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0200', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0200==begin==>");
        var subInfo ={
            onConnect:connectCallbackb,
            onDisconnect:disconnectCallbackb,
        }
        await notify.subscribe(subInfo,subscribeCallbackb);
        await notify.subscribe(subInfo,subscribeCallbackc);
        await notify.unsubscribe(subInfo, unSubscribeCallbackb);
        console.debug("==ActsSubscriber_test_0200==end==>");
        done();
    })

    /*
     * @tc.number: ActsSubscriber_test_0300
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0300', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0300==begin==>");

        var subInfo ={
            onConnect:connectCallbackc,
            onDisconnect:disconnectCallbackc,
        }
        var subInfo2 ={
            onConnect:connectCallbackd,
            onDisconnect:disconnectCallbackd,
        }
        await notify.subscribe(subInfo,subscribeCallbackd);
        await notify.subscribe(subInfo2,subscribeCallbacke);
        await notify.unsubscribe(subInfo, unSubscribeCallbackc);
        await notify.unsubscribe(subInfo2, unSubscribeCallbackd);
        console.debug("==ActsSubscriber_test_0300==end==>");
        done();
    })
    /*
     * @tc.number: ActsSubscriber_test_0400
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0400', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0500==begin==>");

        var subInfo ={
            onConnect:connectCallbackf,
            onDisconnect:disconnectCallbackf,
        }
        var subInfo2 ={
            onConnect:connectCallbackg,
            onDisconnect:disconnectCallbackg,
        }
        await notify.subscribe(subInfo,subscribeCallbackg);
        await notify.subscribe(subInfo,subscribeCallbackh);
        await notify.subscribe(subInfo2,subscribeCallbacki);

        await notify.unsubscribe(subInfo, unSubscribeCallbackf);
        await notify.unsubscribe(subInfo2, unSubscribeCallbackg);
        console.debug("==ActsSubscriber_test_0500==end==>");
        done();
    })

    /*
     * @tc.number: ActsSubscriber_test_0500
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0500', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0500==begin==>");
        await notify.subscribe(subInfob, {bundleNames:["com.example.actsanspublishtest"]},subscribeCallbackl);
        console.debug("==ActsSubscriber_test_0500==end3==>");
        done();
    })

    /*
     * @tc.number: ActsSubscriber_test_0700
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0600', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0600==begin==>");
        await notify.subscribe(subInfoc, {bundleNames:["com.example.actsanspublishtest"]}).then(
                console.log("ActsSubscriber_test_0600=======promise")
            );
        console.debug("==ActsSubscriber_test_0600==end==>");
        done();
    })

    /*
     * @tc.number: ActsSubscriber_test_0700
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0700', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0700==begin==>");
        await notify.subscribe(subInfod, {bundleNames:["com.example.actsanspublishtest","com.example.actsanspublishtest"]},subscribeCallbackn);
        console.debug("==ActsSubscriber_test_0700==end==>");
        done();
    })

    /*
     * @tc.number: ActsSubscriber_test_0800
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0800', 0, async function (done) {
        console.debug("==ActsSubscriber_test_0900==begin==>");
        await notify.subscribe(subInfoe, {bundleNames:["com.example.actsanspublishtest","com.example.actsanspublishtest"]}).then(
                console.log("ActsSubscriber_test_0900=======promise")
            );
        console.debug("==ActsSubscriber_test_0900==end==>");
        done();
    })
    /*
     * @tc.number: ActsSubscriber_test_0900
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_0900', 0, async function (done) {
        console.debug("==ActsSubscriber_test_1000==begin==>");
        var promise = notify.subscribe(100,{bundleNames:["com.example.actsanspublishtest"]});
        expect(promise).assertEqual(undefined);
        console.debug("==ActsSubscriber_test_1000==end==>");
        done();
    })
    /*
     * @tc.number: ActsSubscriber_test_1000
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_1000', 0, async function (done) {
        console.debug("==ActsSubscriber_test_1000==begin==>");
        var subInfo = null
        var promise = await notify.subscribe(subInfo,{bundleNames:["com.example.actsanspublishtest"]});
        expect(promise).assertEqual(undefined);
        console.debug("==ActsSubscriber_test_1000==end==>");
        done();
    })
    /*
     * @tc.number: ActsSubscriber_test_1200
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_1100', 0, async function (done) {
        console.debug("==ActsSubscriber_test_1200==begin==>");
        var subInfo = "#$#%$%$^&%^%"
        var promise = notify.subscribe(subInfo,{bundleNames:["com.example.actsanspublishtest"]});
        expect(promise).assertEqual(undefined);
        console.debug("==ActsSubscriber_test_1200==end==>");
        done();
    })
    /*
     * @tc.number: ActsSubscriber_test_1200
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_1200', 0, async function (done) {
        console.debug("==ActsSubscriber_test_1200==begin==>");
        var subInfo = ""
        var promise = await notify.subscribe(subInfo,{bundleNames:["com.example.actsanspublishtest"]});
        expect(promise).assertEqual(undefined);
        console.debug("==ActsSubscriber_test_1200==end==>");
        done();
    })
    /*
     * @tc.number: ActsSubscriber_test_1300
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_1300', 0, async function (done) {
        console.debug("==ActsSubscriber_test_1300==begin==>");
        var subInfo ={
            onConnect:connectCallbackl,
            onDisconnect:disconnectCallbackn,
        }
        await notify.subscribe(subInfo,{bundleNames:["wrongBudleName"]},subscribeCallbacko);
        await notify.unsubscribe(subInfo, unSubscribeCallbackn);
        console.debug("==ActsSubscriber_test_1300==end==>");
        done();
    })
    /*
     * @tc.number: ActsSubscriber_test_1500
     * @tc.name: subscribe()
     * @tc.desc: verify the function of subscribe
     */
    it('ActsSubscriber_test_1400', 0, async function (done) {
        console.debug("==ActsSubscriber_test_1400==begin==>");
        var subInfo ={
            onConnect:connectCallbacko,
            onDisconnect:disconnectCallbacko,
        }
        await notify.subscribe(subInfo,{bundleNames:[]},subscribeCallbackp);
        await notify.unsubscribe(subInfo, unSubscribeCallbacko);
        console.debug("==ActsSubscriber_test_1400==end==>");
        done();
    })
})

