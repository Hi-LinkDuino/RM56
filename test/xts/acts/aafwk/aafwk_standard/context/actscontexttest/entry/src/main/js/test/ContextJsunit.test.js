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
import featureAbility from '@ohos.ability.featureAbility'
import bundle from '@ohos.bundle'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('ActsContextTest', function () {

    //  @tc.number: ACTS_GetBundleName_0100
    //  @tc.name: getBundleName : Query return value type
    //  @tc.desc: The class of the test return value is made Promise
    it('ACTS_GetBundleName_0100', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var result = await context.getBundleName();
        expect(typeof(context)).assertEqual("object");
        ret = true
        done();
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_GetBundleName_0200
    //  @tc.name: getBundleName : Get the bundlename of the hap package
    //  @tc.desc: Get the bundlename of the hap package(by promise)
    it('ACTS_GetBundleName_0200', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var result = await context.getBundleName();
        expect(result).assertEqual('com.example.actscontext');
        ret = true
        done();
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_GetBundleName_0300
    //  @tc.name: getBundleName : Get the bundlename of the hap package
    //  @tc.desc: Get the value of return is void (by callback)
    it('ACTS_GetBundleName_0300', 0, async function (done) {
        var ret = false
        var context = featureAbility.getContext();
                var info = context.getBundleName(
                    (err, data) => {
                        expect(err.code).assertEqual(0);
                        expect(data).assertEqual('com.example.actscontext');
                        ret = true
                        done();
                    })


        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_GetBundleName_0400
    //  @tc.name: getBundleName : Get the bundlename of the hap package
    //  @tc.desc: Get the bundlename of the hap package(by callback)
    it('ACTS_GetBundleName_0400', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var info = context.getBundleName(
            (err, data) => {
                expect(data).assertEqual('com.example.actscontext');
                ret = true
                done();
            }
        );
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_GetBundleName_0500
    //  @tc.name: getBundleName : Get the bundlename of the hap package
    //  @tc.desc: Wrong parameters are provided, and the test return type is void (by callback)
    it('ACTS_GetBundleName_0500', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var info = context.getBundleName("error_param",
            (err, data) => {
                expect(data).assertEqual('com.example.actscontext');
            }
        );
        expect(info).assertEqual(null);
        ret = true
        done();
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_VerifyPermission_0100
    //  @tc.name: verifySelfPermission : Query whether the application of the specified PID and
    //  UID has been granted a certain permission
    //  @tc.desc: Query whether the application of the specified PID and UID has been granted
    //  a certain permission (by callback)
    it('ACTS_VerifyPermission_0100', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var datainfo = await bundle.getBundleInfo('com.example.actscontext',1);
        var options = {
            pid :0,
            uid :datainfo.uid
        }
        context.verifyPermission("ohos.permission.INSTALL_BUNDLE",options,
            (err, data) => {
                console.info("ACTS_VerifyPermission_0100 in verifyPermission")
                expect(data).assertEqual(0);
                ret = true
                done();
            });
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_VerifyPermission_0200
    //  @tc.name: verifySelfPermission : Query whether the application of the specified PID and
    //  UID has been granted a certain permission
    //  @tc.desc: Query whether the application of the specified PID and UID has been granted
    //  a certain permission (by Promise)
    it('ACTS_VerifyPermission_0200', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var datainfo = await bundle.getBundleInfo('com.example.actscontext',1);
        var options = {
            pid :0,
            uid :datainfo.uid
        }
        var promise = await context.verifyPermission("ohos.permission.INSTALL_BUNDLE",options );
        expect(promise).assertEqual(0);
        ret = true
        done();
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_VerifyPermission_0300
    //  @tc.name: verifySelfPermission : Query whether the application of the specified PID and
    //  UID has been granted a certain permission
    //  @tc.desc: Query whether the application of the specified PID and UID has been granted
    //  a certain permission (by Promise)
    it('ACTS_VerifyPermission_0300', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var datainfo = await bundle.getBundleInfo('com.example.actscontext',1);
        var options = {
            pid :0,
            uid :datainfo.uid
        }
        var result = context.verifyPermission("com.example.permission.NOT",options,
            (err, data) => {
                expect(data).assertEqual(-1);
                ret = true
                done();
            });
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_VerifyPermission_0400
    //  @tc.name: verifySelfPermission : Query whether the application of the specified PID and
    //  UID has been granted a certain permission
    //  @tc.desc: Query whether the application of the specified PID and UID has been granted
    //  a certain permission (by Promise)
    it('ACTS_VerifyPermission_0400', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var datainfo = await bundle.getBundleInfo('com.example.actscontext',1);
        var options = {
            pid :0,
            uid :datainfo.uid
        }
        var promise = await context.verifyPermission("ohos.permission.CAMERA.NOT",options );
        expect(promise).assertEqual(-1);
        ret = true
        done();
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    //  @tc.number: ACTS_VerifyPermission_0500
    //  @tc.name: verifySelfPermission : Query whether the application of the specified PID and
    //  UID has been granted a certain permission
    //  @tc.desc: Query whether the application of the specified PID and UID has been granted
    //  a certain permission (by Promise)
    it('ACTS_VerifyPermission_0500', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        var datainfo = await bundle.getBundleInfo('com.example.actscontext',1);
        var options = {
            pid :0,
            uid :datainfo.uid
        }
        var promise = await context.verifyPermission(2000,options );
        expect(promise).assertEqual(null);
        ret = true
        done();
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

  
    //  @tc.number: ACTS_RequestPermissionForUser_0100
    //  @tc.name: requestPermissionsFromUser : Requests certain permissions from the system.
    //  permission: The list of permissions to be requested.
    //  @tc.desc: Requests certain permissions from the system.
    //  process is the current process. (by callback)
    it('ACTS_RequestPermissionForUser_0100', 0, async function (done) {
        var ret = false
        var context = await featureAbility.getContext();
        console.log("RequestPermissionForUser ----------1");

        context.requestPermissionsFromUser([], 1,
            (err, data)=>{
                console.log("====>requestdata====>" + JSON.stringify(data));
                console.log("====>requesterrcode====>" + JSON.stringify(err.code));
                expect(err.code).assertEqual(-104)
            });
        console.log("RequestPermissionForUser ----------2");
        ret = true
        done();
        setTimeout(function(){
            expect(ret).assertEqual(true)
        },1000)
    })

    /*
    * @tc.number: ACTS_GetDisplayOrientation_0100
    * @tc.name: context.getDisplayOrientation : Get an ability display orientation.
    * @tc.desc: Check the return display orientation of the interface (by AsyncCallback)
    */
    it('ACTS_GetDisplayOrientation_0100', 0, async function (done) {
        console.log('ACTS_GetDisplayOrientation_0100====<begin');

        try {
            context.getDisplayOrientation((err,data)=>{
                console.log('getDisplayOrientation call back');
                done();
            });
            done();
        } catch (err) {
            console.log('ACTS_GetDisplayOrientation_0100====<end err=' + err)
            done();
        }
        console.log('ACTS_GetDisplayOrientation_0100====<end');
    })

    /*
    * @tc.number: ACTS_GetDisplayOrientation_0200
    * @tc.name: context.getDisplayOrientation : Get an ability display orientation.
    * @tc.desc: Check the return display orientation of the interface
    */
    it('ACTS_GetDisplayOrientation_0200', 0, async function (done) {
        console.log('ACTS_GetDisplayOrientation_0200====<begin');
        try {
            var displayOrientation = context.getDisplayOrientation();
            done();
        } catch (err) {
            console.log('ACTS_GetDisplayOrientation_0200====<end err=' + err)
            done();
        }
        console.log('ACTS_GetDisplayOrientation_0200====<end');
    })

    /*
    * @tc.number: ACTS_SetDisplayOrientation_0100
    * @tc.name: context.setDisplayOrientation : Set an ability display orientation.
    * @tc.desc: Check the return display orientation of the interface (by AsyncCallback)
    */
    it('ACTS_SetDisplayOrientation_0100', 0, async function (done) {
        console.log('ACTS_SetDisplayOrientation_0100====<begin');

        try {
            context.setDisplayOrientation(true, (err,data)=>{
                console.log('SetDisplayOrientation call back');
                done();
            });
            done();
        } catch (err) {
            console.log('ACTS_SetDisplayOrientation_0100====<end err=' + err)
            done();
        }
        console.log('ACTS_SetDisplayOrientation_0100====<end');
    })

    /*
    * @tc.number: ACTS_SetDisplayOrientation_0200
    * @tc.name: context.setDisplayOrientation : Set an ability display orientation.
    * @tc.desc: Check the return display orientation of the interface
    */
    it('ACTS_SetDisplayOrientation_0200', 0, async function (done) {
        console.log('ACTS_SetDisplayOrientation_0200====<begin');
        try {
            context.setDisplayOrientation(true);
            done();
        } catch (err) {
            console.log('ACTS_SetDisplayOrientation_0200====<end err=' + err)
            done();
        }
        console.log('ACTS_SetDisplayOrientation_0200====<end');
    })

    /*
    * @tc.number: ACTS_SetShowOnLockScreen_0100
    * @tc.name: context.setShowOnLockScreen : Set an ability show on lock screen.
    * @tc.desc: Check the return show on lock screen status of the interface (by AsyncCallback)
    */
    it('ACTS_SetShowOnLockScreen_0100', 0, async function (done) {
        console.log('ACTS_SetShowOnLockScreen_0100====<begin');

        try {
            context.setShowOnLockScreen(true, (err,data)=>{
                console.log('SetShowOnLockScreen call back');
                done();
            });
            done();
        } catch (err) {
            console.log('ACTS_SetShowOnLockScreen_0100====<end err=' + err)
            done();
        }
        console.log('ACTS_SetShowOnLockScreen_0100====<end');
    })

    /*
    * @tc.number: ACTS_SetShowOnLockScreen_0200
    * @tc.name: context.setShowOnLockScreen : Set an ability show on lock screen.
    * @tc.desc: Check the return show on lock screen status of the interface
    */
    it('ACTS_SetShowOnLockScreen_0200', 0, async function (done) {
        console.log('ACTS_SetShowOnLockScreen_0200====<begin');
        try {
            context.setShowOnLockScreen(true);
            done();
        } catch (err) {
            console.log('ACTS_SetShowOnLockScreen_0200====<end err=' + err)
            done();
        }
        console.log('ACTS_SetShowOnLockScreen_0200====<end');
    })

    /*
    * @tc.number: ACTS_SetWakeUpScreen_0100
    * @tc.name: context.setWakeUpScreen : Set an ability wake up screen.
    * @tc.desc: Check the return wake up screen status of the interface (by AsyncCallback)
    */
    it('ACTS_SetWakeUpScreen_0100', 0, async function (done) {
        console.log('ACTS_SetWakeUpScreen_0100====<begin');

        try {
            context.setWakeUpScreen(true, (err,data)=>{
                console.log('setWakeUpScreen call back');
                done();
            });
            done();
        } catch (err) {
            console.log('ACTS_SetWakeUpScreen_0100====<end err=' + err)
            done();
        }
        console.log('ACTS_SetWakeUpScreen_0100====<end');
    })

    /*
    * @tc.number: ACTS_SetWakeUpScreen_0200
    * @tc.name: context.setWakeUpScreen : Set an ability wake up screen.
    * @tc.desc: Check the return wake up screen status of the interface
    */
    it('ACTS_SetWakeUpScreen_0200', 0, async function (done) {
        console.log('ACTS_SetWakeUpScreen_0200====<begin');
        try {
            context.setWakeUpScreen(true);
            done();
        } catch (err) {
            console.log('ACTS_SetWakeUpScreen_0200====<end err=' + err)
            done();
        }
        console.log('ACTS_SetWakeUpScreen_0200====<end');
    })

})
