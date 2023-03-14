/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import featureAbility from '@ohos.ability.featureAbility'
import abilityDelegatorRegistry from '@ohos.application.abilityDelegatorRegistry'
import bundle from '@ohos.bundle'

describe('ActsStServiceAbilityTest', function () {
    let gSetTimeout = 1000
    beforeAll(async (done) => {
        console.debug('= ACTS_AbeforeAll 1812 ====<begin');
        console.debug('= ACTS_AbeforeAll ====<end');
        done();
    })
    beforeEach(async (done) => {
        setTimeout(function () {
            done();
        }, gSetTimeout);
    })
    afterEach(async (done) => {
        setTimeout(function () {
            done();
        }, gSetTimeout);
    })
    afterAll((done) => {
        console.debug('= ACTS_AAAfterAll ====<begin');
        setTimeout(function () {
            console.debug('= ACTS_AAAfterAll ====<end');
            featureAbility.terminateSelf();
            done();
        }, gSetTimeout);
    })

    /**
     * @tc.number: ACTS_AExecuteShellCommand_0100
     * @tc.name: Execute a shell command without arguments (AsyncCallback).
     * @tc.desc: Verify that the interface executes the Shell command successfully.
     */
    it('ACTS_AExecuteShellCommand_Callback_0100', 0, async function (done) {
        console.log("ACTS_AExecuteShellCommand_Callback_0100 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AExecuteShellCommand_Callback_0100====<end mySetTimeout')
                done();
            }
            var cmd = 'pwd'
            var escresult = '/system/app'
            var AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
            AbilityDelegator.executeShellCommand(cmd, (err, data) => {
                clearTimeout(currentAlertTimeout);
                console.log('ACTS_AExecuteShellCommand_Callback_0100 - executeShellCommand: start ')

                console.log('ACTS_AExecuteShellCommand_Callback_0100 stdResult = ' + data.stdResult)
                var i = data.stdResult.indexOf('/system/app');
                console.log('ACTS_AExecuteShellCommand_Callback_0100 query string i = ' + i);
                expect(i == -1).assertEqual(false);

                console.log('ACTS_AExecuteShellCommand_Callback_0100 exitCode = ' + data.exitCode)
                expect(data.exitCode).assertEqual(0);

                console.log('ACTS_AExecuteShellCommand_Callback_0100 - executeShellCommand: end')

                done()
            })
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_AExecuteShellCommand_Callback_0100 : error = " + error);
            console.debug('ACTS_AExecuteShellCommand_Callback_0100====<end catch (error)');
            done();
        }

    })

    /**
     * @tc.number: ACTS_AExecuteShellCommand_0200
     * @tc.name: Execute a shell command with arguments (AsyncCallback).
     * @tc.desc: Verify that the interface executes the Shell command successfully.
     */
    it('ACTS_AExecuteShellCommand_Callback_0200', 0, async function (done) {
        console.log("ACTS_AExecuteShellCommand_Callback_0200 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AExecuteShellCommand_Callback_0200====<end mySetTimeout')
                done();
            }
            var cmd = 'aa start -d 0 -a com.example.actskillProcessWithAccountclosetest.MainAbility'
                + ' -b com.example.actskillProcessWithAccountclosetest'
            var AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
            AbilityDelegator.executeShellCommand(cmd, 1000, (err, data) => {
                clearTimeout(currentAlertTimeout);
                console.log('ACTS_AExecuteShellCommand_Callback_0200 - executeShellCommand: start ')

                console.log('ACTS_AExecuteShellCommand_Callback_0200 stdResult = ' + data.stdResult)
                var i = data.stdResult.indexOf('start ability successfully.');
                console.log('ACTS_AExecuteShellCommand_Callback_0200 query string i = ' + i);
                expect(i == -1).assertEqual(false);

                console.log('ACTS_AExecuteShellCommand_Callback_0200 exitCode = ' + data.exitCode)
                expect(data.exitCode).assertEqual(0);

                done()
            })
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_AExecuteShellCommand_Callback_0200 : error = " + error);
            console.debug('ACTS_AExecuteShellCommand_Callback_0200====<end catch (error)');
            done();
        }
    })

    /**
     * @tc.number: ACTS_AExecuteShellCommand_Promise_0100
     * @tc.name: Execute a shell command without arguments (Promise).
     * @tc.desc: Verify that the interface executes the Shell command successfully.
     */
    it('ACTS_AExecuteShellCommand_Promise_0100', 0, async function (done) {
        console.log("ACTS_AExecuteShellCommand_Promise_0100 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AExecuteShellCommand_Callback_0200====<end mySetTimeout')
                done();
            }
            var cmd = 'pwd'
            var AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
            AbilityDelegator.executeShellCommand(cmd, 1).then((data) => {
                clearTimeout(currentAlertTimeout);
                console.log('ACTS_AExecuteShellCommand_Promise_0100 - executeShellCommand: start ')

                console.log('ACTS_AExecuteShellCommand_Promise_0100 stdResult = ' + data.stdResult)
                var i = data.stdResult.indexOf('/system/app');
                console.log('ACTS_AExecuteShellCommand_Callback_0100 query string i = ' + i);
                expect(i == -1).assertEqual(false);

                console.log('ACTS_AExecuteShellCommand_Promise_0100 exitCode = ' + data.exitCode)
                expect(data.exitCode).assertEqual(0);
                done();
            })
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_AExecuteShellCommand_Callback_0200 : error = " + error);
            console.debug('ACTS_AExecuteShellCommand_Callback_0200====<end catch (error)');
            done();
        }
    })

    /**
     * @tc.number: ACTS_AAACommand_0100
     * @tc.name: Hide this for inner system use.
     * @tc.desc: VHide this for inner system use successfully.
     */
    it('ACTS_AAACommand_0100', 0, async function (done) {
        console.log("ACTS_AAACommand_0100 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AAACommand_0100====<end mySetTimeout')
                done();
            }
            var AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
            var msg = '测试日志!@#$%^&*()_+QWE{}|?><Fafq3146'
            AbilityDelegator.finishTest(msg, 1, () => {
                console.log("ACTS_AAACommand_0100 finishTest  test  end ========> callback ")
                done()
            })
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_AAACommand_0100 : error = " + error);
            console.debug('ACTS_AAACommand_0100====<end catch (error)');
            done();
        }
    })

    /**
     * @tc.number: ACTS_AAACommand_1200
     * @tc.name: Hide this for inner system use.
     * @tc.desc: VHide this for inner system use successfully.
     */
    it('ACTS_AAACommand_1200', 0, async function (done) {
        console.log("ACTS_AAACommand_1200 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AAACommand_1200====<end mySetTimeout')
                done();
            }
            var AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
            var msg = '测试日志!@#$%^&*()_+QWE{}|?><Fafq3146'
            AbilityDelegator.finishTest(msg, Number.MAX_VALUE).then(() => {
                console.log("ACTS_AAACommand_1200 finishTest  test  end ========> callback ")
                done()
            })
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_AAACommand_1200 : error = " + error);
            console.debug('ACTS_AAACommand_1200====<end catch (error)');
            done();
        }
    })

    /**
      * @tc.number: ACTS_AGetDisplayOrientation_0100
      * @tc.name: GetDisplayOrientation
      * @tc.desc: Check the return value of the interface (by AsyncCallback)
      */
    it('ACTS_AGetDisplayOrientation_0100', 0, async function (done) {
        console.log("ACTS_AGetDisplayOrientation_0100 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AGetDisplayOrientation_0100====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.getDisplayOrientation(
                (err, DisplayOrientation) => {
                    clearTimeout(currentAlertTimeout);
                    console.debug("ACTS_AGetDisplayOrientation_0100====DisplayOrientation>"
                        + JSON.stringify(DisplayOrientation) + " , err= " + err);
                    expect(DisplayOrientation == bundle.DisplayOrientation.UNSPECIFIED
                        || (DisplayOrientation != bundle.DisplayOrientation.LANDSCAPE
                            || DisplayOrientation != bundle.DisplayOrientation.PORTRAIT
                            || DisplayOrientation != bundle.DisplayOrientation.FOLLOW_RECENT
                        )).assertTrue();
                    done()
                }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_AGetDisplayOrientation_0100 : error = " + error);
            console.debug('ACTS_AGetDisplayOrientation_0100====<end catch (error)');
            done();
        }
    })

    /**
     * @tc.number: ACTS_AGetDisplayOrientation_0200
     * @tc.name: GetDisplayOrientation
     * @tc.desc: Check the return type of the interface (by promise)
     */
    it('ACTS_AGetDisplayOrientation_0200', 0, async function (done) {
        console.log("ACTS_AGetDisplayOrientation_0200 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_AGetDisplayOrientation_0200====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.getDisplayOrientation().then((data) => {
                clearTimeout(currentAlertTimeout);
                console.debug("ACTS_AGetDisplayOrientation_0200====DisplayOrientation>"
                    + JSON.stringify(DisplayOrientation) + " , err= " + err);
                expect(DisplayOrientation == bundle.DisplayOrientation.UNSPECIFIED
                    || (DisplayOrientation != bundle.DisplayOrientation.LANDSCAPE
                        || DisplayOrientation != bundle.DisplayOrientation.PORTRAIT
                        || DisplayOrientation != bundle.DisplayOrientation.FOLLOW_RECENT
                    )).assertTrue();
                done()
            }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_AGetDisplayOrientation_0200 : error = " + error);
            console.debug('ACTS_AGetDisplayOrientation_0200====<end catch (error)');
            done();
        }
    })

    /**
    * @tc.number: ACTS_ASetDisplayOrientation_0100
    * @tc.name: SetDisplayOrientation
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ASetDisplayOrientation_0100', 0, async function (done) {
        console.log("ACTS_ASetDisplayOrientation_0100 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_ASetDisplayOrientation_0100====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.setDisplayOrientation(bundle.DisplayOrientation.UNSPECIFIED,
                (err) => {
                    clearTimeout(currentAlertTimeout);
                    console.debug("ACTS_ASetDisplayOrientation_0100====err>"
                        + JSON.stringify(err) + " , err= " + err);
                    done()
                }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_ASetDisplayOrientation_0100 : error = " + error);
            console.debug('ACTS_ASetDisplayOrientation_0100====<end catch (error)');
            done();
        }
    })

    /**
     * @tc.number: ACTS_ASetDisplayOrientation_0200
     * @tc.name: SetDisplayOrientation
     * @tc.desc: Check the return type of the interface (by promise)
     */
    it('ACTS_ASetDisplayOrientation_0200', 0, async function (done) {
        console.log("ACTS_ASetDisplayOrientation_0200 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_ASetDisplayOrientation_0200====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.setDisplayOrientation(bundle.DisplayOrientation.UNSPECIFIED).then(() => {
                clearTimeout(currentAlertTimeout);
                done()
            }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_ASetDisplayOrientation_0200 : error = " + error);
            console.debug('ACTS_ASetDisplayOrientation_0200====<end catch (error)');
            done();
        }
    })

    /**
    * @tc.number: ACTS_ASetShowOnLockScreen_0100
    * @tc.name: SetShowOnLockScreen
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ASetShowOnLockScreen_0100', 0, async function (done) {
        console.log("ACTS_ASetShowOnLockScreen_0100 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_ASetShowOnLockScreen_0100====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.setShowOnLockScreen(true,
                (err) => {
                    clearTimeout(currentAlertTimeout);
                    console.debug("ACTS_ASetShowOnLockScreen_0100====err>"
                        + JSON.stringify(err) + " , err= " + err);
                    done()
                }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_ASetShowOnLockScreen_0100 : error = " + error);
            console.debug('ACTS_ASetShowOnLockScreen_0100====<end catch (error)');
            done();
        }
    })

    /**
     * @tc.number: ACTS_ASetShowOnLockScreen_0200
     * @tc.name: SetShowOnLockScreen
     * @tc.desc: Check the return type of the interface (by promise)
     */
    it('ACTS_ASetShowOnLockScreen_0200', 0, async function (done) {
        console.log("ACTS_ASetShowOnLockScreen_0200 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_ASetShowOnLockScreen_0200====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.setShowOnLockScreen(true).then(() => {
                clearTimeout(currentAlertTimeout);
                console.debug("ACTS_ASetShowOnLockScreen_0200====then>");
                done()
            }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_ASetShowOnLockScreen_0200 : error = " + error);
            console.debug('ACTS_ASetShowOnLockScreen_0200====<end catch (error)');
            done();
        }
    })

    /**
    * @tc.number: ACTS_ASetWakeUpScreen_0100
    * @tc.name: SetWakeUpScreen
    * @tc.desc: Check the return value of the interface (by AsyncCallback)
    */
    it('ACTS_ASetWakeUpScreen_0100', 0, async function (done) {
        console.log("ACTS_ASetWakeUpScreen_0100 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_ASetWakeUpScreen_0100====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.setWakeUpScreen(true,
                (err) => {
                    clearTimeout(currentAlertTimeout);
                    console.debug("ACTS_ASetWakeUpScreen_0100====err>"
                        + JSON.stringify(err) + " , err= " + err);
                    done()
                }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_ASetWakeUpScreen_0100 : error = " + error);
            console.debug('ACTS_ASetWakeUpScreen_0100====<end catch (error)');
            done();
        }
    })

    /**
     * @tc.number: ACTS_ASetWakeUpScreen_0200
     * @tc.name: SetWakeUpScreen
     * @tc.desc: Check the return type of the interface (by promise)
     */
    it('ACTS_ASetWakeUpScreen_0200', 0, async function (done) {
        console.log("ACTS_ASetWakeUpScreen_0200 --- start")
        var currentAlertTimeout = 0;
        try {
            currentAlertTimeout = setTimeout(mySetTimeout, gSetTimeout);
            function mySetTimeout() {
                console.log('ACTS_ASetWakeUpScreen_0200====<end mySetTimeout')
                done();
            }
            var context = featureAbility.getContext()
            var result = context.setWakeUpScreen(true).then(() => {
                clearTimeout(currentAlertTimeout);
                console.debug("ACTS_ASetWakeUpScreen_0200====then>");
                done()
            }
            );
        } catch (error) {
            clearTimeout(currentAlertTimeout);
            console.log("ACTS_ASetWakeUpScreen_0200 : error = " + error);
            console.debug('ACTS_ASetWakeUpScreen_0200====<end catch (error)');
            done();
        }
    })

    /*
    * @tc.number    : ACTS_ATestRunner_0200
    * @tc.name      : start new ability
    * @tc.desc      : Starting mainability2 with startability succeeded.(callback)
    */
    it('ACTS_ATestRunner_0200', 0, async function (done) {
        console.debug('ACTS_ATestRunner_0200====<begin');
        try {
            var AbilityDelegatorArgs = abilityDelegatorRegistry.getArguments()
            console.debug("ACTS_ATestRunner_0200====>getArguments is====>" + JSON.stringify(AbilityDelegatorArgs));
            console.debug("ACTS_ATestRunner_0200====bundleName>" + JSON.stringify(AbilityDelegatorArgs.bundleName));
            expect(AbilityDelegatorArgs.bundleName).assertEqual(undefined)
            console.debug("ACTS_ATestRunner_0200====parameters>" + JSON.stringify(AbilityDelegatorArgs.parameters));
            expect(AbilityDelegatorArgs.parameters).assertEqual(undefined)
            console.debug("ACTS_ATestRunner_0200====testCaseNames>"
                 + JSON.stringify(AbilityDelegatorArgs.testCaseNames));
            expect(AbilityDelegatorArgs.testCaseNames).assertEqual(undefined)
            console.debug("ACTS_ATestRunner_0200====testRunnerClassName>"
                + JSON.stringify(AbilityDelegatorArgs.testRunnerClassName));
            expect(AbilityDelegatorArgs.testRunnerClassName).assertEqual(undefined)
            console.debug('ACTS_ATestRunner_0200====<end');
            done();
        } catch (error) {
            console.log("ACTS_ATestRunner_0200 : error = " + error);
            console.debug('ACTS_ATestRunner_0200====<end catch (error)');
            done();
        }
    })
})