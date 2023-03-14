/**
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

import {describe, it, expect} from 'deccjsunit/index.ets';
import Utils from './Utils';
import bundleManager from '@ohos.bundle';

export default function GetabilityInfo() {
  describe('GetabilityInfo', function () {
    let bundleName = "com.open.harmony.packagemag";
    let abilityName = "com.open.harmony.packagemag.MainAbility";

    let bundleName_other = "com.ohos.acecollaboration";
    let abilityName_other = "com.ohos.acecollaboration.MainAbility";

    let bundleName1 = "com.harmony.packagemag";
    let abilityName1 = "com.harmony.packagemag1.MainAbility";

    /*
     * @tc.number: bundle_GetabilityInfo_test_0100
     * @tc.name: getAbilityInfo : The basic ability is enhanced to obtain the specified Ability information
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('bundle_GetabilityInfo_test_0100', 0, async function (done) {
      let mData;
      var timeOldStamp = await Utils.getNowTime();
      await bundleManager.getAbilityInfo(bundleName, abilityName)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('bundle_GetabilityInfo_test_0100', timeOldStamp, timeNewStamp)
          console.info('[bundle_GetabilityInfo_test_0100] getApplicationInfo promise data is: ' + JSON.stringify(data));
          expect(typeof (data)).assertEqual("object");
          mData = data;
        }).catch((error) => {
        console.error('[bundle_GetabilityInfo_test_0100]Operation failed. Cause: ' + JSON.stringify(error));
        expect(error).assertFail();
      })
      checkAbilityInfo(mData);
      done();
    });

    /*
     * @tc.number: bundle_GetabilityInfo_test_0200
     * @tc.name: getAbilityInfo : The basic ability is enhanced to obtain the specified Ability information
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
	it('bundle_GetabilityInfo_test_0200', 0, async function (done) {
      let mData;
      var timeOldStamp = await Utils.getNowTime();
      await bundleManager.getAbilityInfo(bundleName, abilityName, (err, data) => {
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('bundle_GetabilityInfo_test_0200', timeOldStamp, timeNewStamp)
        if (err) {
          console.error('[bundle_GetabilityInfo_test_0200]Operation failed. Cause: ' + JSON.stringify(err));
          expect(err).assertFail();
        }
        console.info('[bundle_GetabilityInfo_test_0200] getApplicationInfo callback data is: ' + JSON.stringify(data));
        mData = data;
      })
      await Utils.sleep(2000);
      checkAbilityInfo(mData);
      done();
    });

    /*
     * @tc.number: bundle_GetabilityInfo_test_0300
     * @tc.name: getAbilityInfo : The basic ability is enhanced to obtain the specified Ability information
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('bundle_GetabilityInfo_test_0300', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      await bundleManager.getAbilityInfo(bundleName_other, abilityName_other)
        .then((data) => {
          var timeNewStamp =  Utils.getNowTime();
          Utils.getDurationTime('bundle_GetabilityInfo_test_0300',timeOldStamp,timeNewStamp)
          expect(data).assertFail();
        }).catch((error) => {
          console.error('[bundle_GetabilityInfo_test_0300]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertEqual(1);
        });
      done();
    });

    /*
     * @tc.number: bundle_GetabilityInfo_test_0400
     * @tc.name: getAbilityInfo : The basic ability is enhanced to obtain the specified Ability information
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('bundle_GetabilityInfo_test_0400', 0, async function (done) {
      let error1;
      var timeOldStamp = await Utils.getNowTime();
      bundleManager.getAbilityInfo(bundleName1, abilityName)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('bundle_GetabilityInfo_test_0400', timeOldStamp, timeNewStamp)
          console.info('[bundle_GetabilityInfo_test_0400] getApplicationInfo data is: ' + JSON.stringify(data));
          expect(typeof (data)).assertEqual("object");
        }).catch((error) => {
        console.error('[bundle_GetabilityInfo_test_0400]Operation failed. Cause: ' + JSON.stringify(error));
        error1 = error;
      })
      await Utils.sleep(1000);
      await expect(error1).assertEqual(1);
      done();
    });

    /*
     * @tc.number: bundle_GetabilityInfo_test_0500
     * @tc.name: getAbilityInfo : The basic ability is enhanced to obtain the specified Ability information
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('bundle_GetabilityInfo_test_0500', 0, async function (done) {
      let error
      await bundleManager.getAbilityInfo(bundleName, abilityName1).then((data) => {
        console.info('[bundle_GetabilityInfo_test_0500] START' + JSON.stringify(data));
      }).catch((err) => {
        console.info('[bundle_GetabilityInfo_test_0500] err = ' + JSON.stringify(err));
        error = err
      })
      expect(error).assertEqual(1);
      done();
      console.info('[bundle_GetabilityInfo_test_0500] END');
    });

    /*
     * @tc.number: bundle_GetabilityInfo_test_0600
     * @tc.name: getAbilityInfo : The basic ability is enhanced to obtain the specified Ability information
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    let bundleName2 = "";
    it('bundle_GetabilityInfo_test_0600', 0, async function (done) {
      let error1;
      var timeOldStamp = await Utils.getNowTime();
      bundleManager.getAbilityInfo(bundleName2, abilityName)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('bundle_GetabilityInfo_test_0600', timeOldStamp, timeNewStamp)
          console.info('[bundle_GetabilityInfo_test_0600] getApplicationInfo data promise is: ' + JSON.stringify(data));
          expect(typeof (data)).assertEqual("object");
        }).catch((error) => {
        console.error('[bundle_GetabilityInfo_test_0600]Operation failed. Cause: ' + JSON.stringify(error));
        error1 = error;
      })
      await Utils.sleep(1000);
      await expect(error1).assertEqual(1);
      done();
    });

    /**
     * 打印AbilityInfo属性信息
     * @param data
     */
    function checkAbilityInfo(data) {
      expect(typeof (data.bundleName)).assertEqual("string");
      expect(data.bundleName).assertEqual("com.open.harmony.packagemag")
      expect(typeof (data.name)).assertEqual("string");
      expect(data.name).assertEqual("com.open.harmony.packagemag.MainAbility");
      expect(data.label).assertEqual("$string:entry_MainAbility");
      expect(typeof (data.label)).assertEqual("string");
      expect(data.description).assertEqual("$string:mainability_description");
      expect(typeof (data.description)).assertEqual("string");
      expect(data.icon).assertEqual("$media:icon");
      expect(typeof (data.icon)).assertEqual("string");
      expect(data.srcPath).assertEqual("MainAbility");
      expect(typeof (data.srcPath)).assertEqual("string");
      expect(data.srcLanguage).assertEqual("ets");
      expect(typeof (data.srcLanguage)).assertEqual("string");
      expect(data.isVisible).assertEqual(true);
      expect(Array.isArray(data.permissions)).assertEqual(true);
      expect(Array.isArray(data.deviceCapabilities)).assertEqual(true);
      expect(data.deviceTypes[0]).assertEqual("phone");
      expect(typeof (data.process)).assertEqual("string");
      expect(data.process).assertEqual("");
      expect(typeof (data.uri)).assertEqual("string");
      expect(data.uri).assertEqual("");
      expect(data.moduleName).assertEqual("entry");
      expect(typeof (data.moduleName)).assertEqual("string");
      expect(typeof (data.applicationInfo)).assertEqual("object");
      checkApplicationInfo(data.applicationInfo);
      console.log("---checkAbilityInfo End---  ");
    }

    /**
     * 打印ApplicationInfo属性信息
     * @param data
     */
    function checkApplicationInfo(info) {
      expect(typeof (info)).assertEqual("object")
      expect(typeof (info.name)).assertEqual("string")
      expect(info.name).assertEqual("com.open.harmony.packagemag");
      expect(typeof (info.codePath)).assertEqual("string")
      expect(info.codePath).assertEqual("/data/app/el1/bundle/public/com.open.harmony.packagemag");
      expect(typeof (info.accessTokenId)).assertEqual("number")
      expect(info.accessTokenId > 0).assertTrue()
      expect(typeof (info.description)).assertEqual("string")
      expect(info.description).assertEqual("$string:mainability_description")
      expect(typeof (info.descriptionId)).assertEqual("number")
      expect(info.descriptionId > 0).assertTrue()
      expect(typeof (info.icon)).assertEqual("string")
      expect(info.icon).assertEqual("$media:icon")
      expect(typeof (info.iconId)).assertEqual("number")
      expect(info.iconId > 0).assertTrue()
      expect(typeof (info.label)).assertEqual("string")
      expect(info.label).assertEqual("$string:entry_MainAbility")
      expect(typeof (info.labelId)).assertEqual("number")
      expect(info.labelId > 0).assertTrue()
      expect(info.systemApp).assertEqual(true)
      expect(typeof (info.entryDir)).assertEqual("string")
      expect(info.entryDir).assertEqual("/data/app/el1/bundle/public/com.open.harmony.packagemag/com.open.harmony.packagemag");
      expect(typeof (info.supportedModes)).assertEqual("number")
      expect(info.supportedModes).assertEqual(0)
      expect(typeof (info.process)).assertEqual("string")
      expect(info.process).assertEqual("")
      expect(Array.isArray(info.moduleSourceDirs)).assertEqual(true);
      expect(info.moduleSourceDirs[0]).assertEqual("/data/app/el1/bundle/public/com.open.harmony.packagemag/com.open.harmony.packagemag");
      expect(Array.isArray(info.permissions)).assertEqual(true);
    }

    /**
     * 打印ApplicationInfo属性信息
     * @param data
     */
    function checkAbilityInfo_other(data) {
      expect(typeof (data.bundleName)).assertEqual("string");
      expect(data.bundleName).assertEqual("com.ohos.acecollaboration");
      expect(typeof (data.name)).assertEqual("string");
      expect(data.name).assertEqual("com.ohos.acecollaboration.MainAbility");
      expect(data.label).assertEqual("$string:entry_MainAbility");
      expect(typeof (data.label)).assertEqual("string");
      expect(data.description).assertEqual("$string:mainability_description");
      expect(typeof (data.description)).assertEqual("string");
      expect(data.icon).assertEqual("$media:icon");
      expect(typeof (data.icon)).assertEqual("string");
      expect(data.srcPath).assertEqual("MainAbility");
      expect(typeof (data.srcPath)).assertEqual("string");
      expect(data.srcLanguage).assertEqual("ets");
      expect(typeof (data.srcLanguage)).assertEqual("string");
      expect(data.isVisible).assertEqual(true);
      expect(Array.isArray(data.permissions)).assertEqual(true);
      expect(Array.isArray(data.deviceCapabilities)).assertEqual(true);
      expect(data.deviceTypes[0]).assertEqual("phone");
      expect(typeof (data.process)).assertEqual("string");
      expect(data.process).assertEqual("");
      expect(typeof (data.uri)).assertEqual("string");
      expect(data.uri).assertEqual("");
      expect(data.moduleName).assertEqual("entry");
      expect(typeof (data.moduleName)).assertEqual("string");
      expect(typeof (data.applicationInfo)).assertEqual("object");
      checkApplicationInfo_other(data.applicationInfo);
      console.log("---checkAbilityInfo_other End---  ");
    }

    /**
     * 打印ApplicationInfo属性信息
     * @param data
     */
    function checkApplicationInfo_other(info) {
      expect(typeof (info)).assertEqual("object")
      expect(typeof (info.name)).assertEqual("string")
      expect(info.name).assertEqual("com.ohos.acecollaboration");
      expect(typeof (info.codePath)).assertEqual("string")
      expect(info.codePath).assertEqual("/data/app/el1/bundle/public/com.ohos.acecollaboration");
      expect(typeof (info.accessTokenId)).assertEqual("number")
      expect(info.accessTokenId > 0).assertTrue()
      expect(typeof (info.description)).assertEqual("string")
      expect(info.description).assertEqual("$string:mainability_description")
      expect(typeof (info.descriptionId)).assertEqual("number")
      expect(info.descriptionId > 0).assertTrue()
      expect(typeof (info.icon)).assertEqual("string")
      expect(info.icon).assertEqual("$media:icon")
      expect(typeof (info.iconId)).assertEqual("number")
      expect(info.iconId > 0).assertTrue()
      expect(typeof (info.label)).assertEqual("string")
      expect(info.label).assertEqual("$string:entry_MainAbility")
      expect(typeof (info.labelId)).assertEqual("number")
      expect(info.labelId > 0).assertTrue()
      expect(info.systemApp).assertEqual(true)
      expect(typeof (info.entryDir)).assertEqual("string")
      expect(info.entryDir).assertEqual("/data/app/el1/bundle/public/com.ohos.acecollaboration/com.ohos.acecollaboration");
      expect(typeof (info.supportedModes)).assertEqual("number")
      expect(info.supportedModes).assertEqual(0)
      expect(typeof (info.process)).assertEqual("string")
      expect(info.process).assertEqual("")
      expect(Array.isArray(info.moduleSourceDirs)).assertEqual(true);
      expect(info.moduleSourceDirs[0]).assertEqual("/data/app/el1/bundle/public/com.ohos.acecollaboration/com.ohos.acecollaboration");
      expect(Array.isArray(info.permissions)).assertEqual(true);
    }
  });
}