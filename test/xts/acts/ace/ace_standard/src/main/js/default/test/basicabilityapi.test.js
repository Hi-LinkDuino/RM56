/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import configuration from '@system.configuration';
import prompt from '@system.prompt';
import router from '@system.router';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';

describe('basicabilityapi', function () {
    let testResult;
    let testResultFail;
    let test;
    beforeAll(function () {
        testResult = true;
        testResultFail = false;
        test = "success"
    });
    beforeEach(function () {
    });
    afterEach(function () {
    });
    afterAll(function () {
    });

    async function backToIndex(){
        let backToIndexPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.back({
                    uri: 'pages/index/index'
                });
                resolve();
            }, 500);
        });
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear();
                resolve();
            }, 500);
        });
        await backToIndexPromise.then(() => {
            return clearPromise;
        });
    }

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0100
     * @tc.name      testClearInterval
     * @tc.desc      Cancel the repetitive timing tasks previously set by setInterval.
     */
    it('testClearInterval', 0, async function(done) {
        console.info('testClearInterval START');
        let res = 0;
        let intervalID = -1;
        let promise1 = new Promise((resolve, reject) => {
            intervalID = setInterval(function () {
                res++;
                console.info('testClearInterval res = ' + res);
                resolve();
            }, 100);
        });
        let promise2 = new Promise((resolve, reject) => {
            setTimeout(function () {
                console.info('[clearInterval] start');
                clearInterval(intervalID);
                console.info('[clearInterval] end');
                resolve();
            }, 600);
        });
        Promise.all([promise1, promise2]).then(() => {
            console.info('testClearInterval finally');
            expect(5).assertEqual(res);
            console.info('testClearInterval END');
            done();
        });
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0200
     * @tc.name      testConsole
     * @tc.desc      Print a text message.
     */
    it('testConsole', 0, function () {
        console.info('testConsole START');
        const versionCode = 1.1;
        console.info('[console.info] versionCode: ' + versionCode);
        console.debug('[console.debug] versionCode: ' + versionCode);
        console.log('[console.log] versionCode: ' + versionCode);
        console.warn('[console.warn] versionCode: ' + versionCode);
        console.error('[console.error] versionCode: ' + versionCode);
        expect(test).assertEqual('success');
        console.info('testConsole END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0300
     * @tc.name      testRouterPush
     * @tc.desc      Go to the specified page of the application.
     */
    it('testRouterPush', 0, async function (done) {
        console.info('testRouterPush START');
        let options = {
            uri : 'pages/routerPush/index'
        }
        let promise1 = new Promise((resolve, reject) => {
            router.push(options);
            resolve();
        });
        let promise2 = new Promise((resolve, reject) => {
            setTimeout(() => {
                let pages = router.getState();
                console.info("[router.push] getState" + JSON.stringify(pages));
                expect("pages/routerPush/").assertEqual(pages.path);
                console.info("[router.push] getLength:" + router.getLength());
                expect("2").assertEqual(router.getLength());
                console.info('testRouterPush SUCCESS');
                resolve();
            }, 500);
        });
        await promise1.then(() => {
            return promise2;
        });
        await backToIndex();
        console.info('testRouterPush END');
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0310
     * @tc.name      testRouterPushNotExist
     * @tc.desc      Test push not exist page.
     */
    it('testRouterPushNotExist', 0, async function (done) {
        console.info('testRouterPushNotExist START');
        let promise1 = new Promise((resolve, reject) => {
            router.push({
                uri: 'pages/routerNotExist/index'
            });
            resolve();
        });
        let promise2 = new Promise((resolve, reject) => {
            setTimeout(() => {
                let pages = router.getState();
                console.info("testRouterPushNotExist getState" + JSON.stringify(pages));
                expect("pages/index/").assertEqual(pages.path);
                console.info("testRouterPushNotExist getLength:" + router.getLength());
                expect("1").assertEqual(router.getLength());
                console.info('testRouterPushNotExist success');
                resolve();
            }, 500);
        });
        await promise1.then(() => {
            return promise2;
        });
        console.info('testRouterPushNotExist END');
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0400
     * @tc.name      testRouterReplace
     * @tc.desc      Replace the current page with a page in the application, and destroy the replaced page.
     */
    it('testRouterReplace', 0, async function (done) {
        //测试框架不允许index页面直接替换，需要先跳转到其他页面再替换
        console.info('testRouterReplace START');
        let options = {
            uri : 'pages/routerPush/index'
        }
        let promise1 = new Promise((resolve, reject) => {
            router.push(options);
            resolve();
        });
        let repleasePage = {
            uri : 'pages/routerReplace/index'
        }
        let promise2 = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.replace(repleasePage);
                resolve();
            }, 500);
        });
        //替换堆栈数量不会变
        let promise3 = new Promise((resolve, reject) => {
            setTimeout(() => {
                let pages = router.getState();
                console.info("[router.replace] getState" + JSON.stringify(pages));
                expect("pages/routerReplace/").assertEqual(pages.path);
                console.info("[router.replace] getLength:" + router.getLength());
                expect("2").assertEqual(router.getLength());
                console.info('testRouterReplace SUCCESS');
                resolve();
            }, 1000);
        });
        await promise1.then(() => {
            return promise2;
        }).then(() => {
            return promise3;
        });
        await backToIndex();
        console.info('testRouterReplace END');
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0410
     * @tc.name      testRouterReplaceNotExist
     * @tc.desc      Test replace not exist page.
     */
    it('testRouterReplaceNotExist', 0, async function (done) {
        console.info('testRouterReplaceNotExist START');
        await setTimeout(() => {
            router.replace({
                uri: 'pages/routerNotExist/index'
            });
        }, 500);
        await setTimeout(() => {
            let pages = router.getState();
            console.info("testRouterReplaceNotExist getState" + JSON.stringify(pages));
            expect("pages/index/").assertEqual(pages.path);
            console.info("testRouterReplaceNotExist getLength:" + router.getLength());
            expect("1").assertEqual(router.getLength());
            console.info('testRouterReplaceNotExist END');
            done();
        }, 1000);
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0500
     * @tc.name      testRouterBack
     * @tc.desc      Return to the previous page or the specified page.
     */
    it('testRouterBack', 0, async function (done) {
        console.info('testRouterBack START');
        let promise1 = new Promise((resolve, reject) => {
            router.push({
                uri: 'pages/routerPush/index'
            });
            resolve();
        });
        let promise2 = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.back({
                    uri: 'pages/index/index'
                });
                resolve();
            }, 500);
        });
        let promise3 = new Promise((resolve, reject) => {
            setTimeout(() => {
                let pages = router.getState();
                console.info("[router.back] getState" + JSON.stringify(pages));
                expect("pages/index/").assertEqual(pages.path);
                console.info("[router.back] getLength:" + router.getLength());
                expect("1").assertEqual(router.getLength());
                console.info('testRouterBack SUCCESS');
                resolve();
            }, 1000);
        });
        await promise1.then(() => {
            return promise2;
        }).then(() => {
            return promise3;
        });
        console.info('testRouterBack END');
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0600
     * @tc.name      testRouterClear
     * @tc.desc      Clear all historical pages in the page stack, and only keep the current page as the top page.
     */
    it('testRouterClear', 0, function () {
        console.info('testRouterClear START');
        router.clear();
        console.info("[router.clear] router.getLength:" + router.getLength());
        expect("1").assertEqual(router.getLength());
        console.info('testRouterClear END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0700
     * @tc.name      testRouterLength
     * @tc.desc      Get the number of pages currently in the page stack.
     */
    it('testRouterLength', 0, async function (done) {
        console.info('testRouterLength START');
        let size = router.getLength();
        console.info('[router.getLength] pages stack size = ' + size);
        expect(size).assertEqual('1');
        let options = {
            uri : 'pages/routerPush/index'
        }
        let promise1 = new Promise((resolve, reject) => {
            router.push(options);
            resolve();
        });
        let promise2 = new Promise((resolve, reject) => {
            setTimeout(() => {
                console.info("testRouterLength getLength:" + router.getLength());
                expect("2").assertEqual(router.getLength());
                console.info('testRouterLength SUCCESS');
                resolve();
            }, 500);
        });
        await promise1.then(() => {
            return promise2;
        });
        await backToIndex();
        console.info('testRouterLength END');
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0800
     * @tc.name      testRouterGetState
     * @tc.desc      Get the status information of the current page.
     */
    it('testRouterGetState', 0, async function (done) {
        console.info('testRouterGetState START');
        await setTimeout(() => {
            let page = router.getState();
            console.info('[router.getState] index: ' + page.index);
            console.info('[router.getState] name: ' + page.name);
            console.info('[router.getState] path: ' + page.path);
            expect(page.index).assertEqual(1);
            expect(page.name).assertEqual('index');
            expect(page.path).assertEqual('pages/index/');
            console.info('testRouterGetState END');
            done();
        }, 500);
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_0900
     * @tc.name      testPromptShowToast
     * @tc.desc      Show text pop-up window.
     */
    it('testPromptShowToast', 0, function () {
        console.info('testPromptShowToast START');
        const delay = 5000;
        prompt.showToast({
            message: 'message',
            duration: delay,
        });
        expect(test).assertEqual('success');
        console.info('[prompt.showToast] success');
        console.info('testPromptShowToast END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1000
     * @tc.name      testPromptDialog
     * @tc.desc      Display the dialog box in the page.
     */
    it('testPromptDialog', 0, function () {
        console.info('testPromptDialog START')
        prompt.showDialog({
            title: 'dialog showDialog test',
            message: 'message of dialog',
            buttons: [
                {
                    text: 'OK',
                    color: '#0000ff',
                    index: 0
                }
            ],
            success: function (ret) {
                console.info("[prompt.showDialog] ret.index " + ret.index);
                expect(testResult).toBeTrue();
            },
            cancel: function () {
                console.log('[prompt.showDialog] dialog cancel callback');
                expect(testResultFail).toBeTrue();
            },
            complete: function () {
                console.log('[prompt.showDialog] complete');
            }
        });
        console.info('testPromptDialog END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1100
     * @tc.name      testConfigurationGetLocale
     * @tc.desc      Get the current language and region of the app. Synchronize with the language and region.
     */
    it('testConfigurationGetLocale', 0, function () {
        console.info('testConfigurationGetLocale START');
        const localeInfo = configuration.getLocale();
        console.info("[configuration.getLocale] localeInfo: " + JSON.stringify(localeInfo));
        console.info("[configuration.getLocale] language: " + localeInfo.language);
        console.info("[configuration.getLocale] countryOrRegion: " + localeInfo.countryOrRegion);
        console.info("[configuration.getLocale] dir: " + localeInfo.dir);
        expect(localeInfo.language).assertEqual('zh');
        expect(localeInfo.dir).assertEqual('ltr');
        console.info('testConfigurationGetLocale END');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1200
     * @tc.name      testSetTimeout
     * @tc.desc      Set up a timer that executes a function or a specified piece of code after the timer expires.
     */
    it('testSetTimeout', 0, async function (done) {
        console.info('testSetTimeout START');
        let start_time = new Date().getTime();
        const delay = 200;
        await setTimeout(function (v1, v2) {
            let end_time = new Date().getTime();
            console.info("[settimeout] start_time: " + start_time);
            console.info("[settimeout] end_time: " + end_time);
            console.info('[settimeout] delay: ' + (end_time - start_time));
            console.info('[settimeout] v1: ' + v1);
            console.info('[settimeout] v2: ' + v2);
            expect('test').assertEqual(v1);
            expect('message').assertEqual(v2);
            expect(end_time - start_time >= delay).assertTrue();
            console.info('testSetTimeout END');
            done();
        }, delay, 'test', 'message');
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1300
     * @tc.name      testClearTimeout
     * @tc.desc      The timer previously established by calling setTimeout() is cancelled.
     */
    it('testClearTimeout', 0, async function (done) {
        console.info('testClearTimeout START');
        let res = 0;
        let timeoutID = setTimeout(function () {
            res++;
        }, 700);
        await setTimeout(function () {
            console.info('testClearTimeout delay 0.5s')
            clearTimeout(timeoutID);
            console.info("[clearTimeout] success");
        }, 500);
        await setTimeout(function () {
            expect(0).assertEqual(res);
            console.info('testClearTimeout END');
            done();
        }, 1000);
    });

    /**
     * @tc.number    SUB_ACE_BASICABILITY_JS_API_1400
     * @tc.name      testSetInterval
     * @tc.desc      Call a function or execute a code segment repeatedly, with a fixed time delay between each call.
     */
    it('testSetInterval', 0, async function (done) {
        console.info('testSetInterval START');
        let res = 0;
        let intervalID = setInterval(function () {
            res++;
        }, 100);
        await setTimeout(function () {
            expect(9).assertEqual(res);
            console.info('testSetInterval SUCCESS');
            clearInterval(intervalID);
            console.info('testSetInterval END');
            done();
        }, 1000);
    });
});
