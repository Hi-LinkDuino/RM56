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
import Bundle from '@ohos.bundle';

const TAG_TEST_0100_001 = ' bundle_getApplicationInfo_test_0100_001 ';
const TAG_TEST_0100_002 = ' bundle_getApplicationInfo_test_0100_002 ';
const TAG_TEST_0100_003 = ' bundle_getApplicationInfo_test_0100_003 ';
const TAG_TEST_0200_001 = ' bundle_getApplicationInfo_test_0200_004 ';
const TAG_TEST_0200_002 = ' bundle_getApplicationInfo_test_0200_005 ';
const TAG_TEST_0200_003 = ' bundle_getApplicationInfo_test_0200_006 ';
const TAG_TEST_0300_001 = ' bundle_getApplicationInfo_test_0300_007 ';
const TAG_TEST_0400_001 = ' bundle_getApplicationInfo_test_0400_008 ';
const TAG_TEST_0500_001 = ' bundle_getApplicationInfo_test_0500_009 ';
const TAG_TEST_0500_002 = ' bundle_getApplicationInfo_test_0500_0010 ';
const TAG_TEST_0600_001 = ' bundle_getApplicationInfo_test_0600_0011 ';
const TAG_TEST_0600_002 = ' bundle_getApplicationInfo_test_0600_0012 ';
const TAG_TEST_0600_003 = ' bundle_getApplicationInfo_test_0600_0013 ';
const BUNDLE_NAME = 'com.open.harmony.packagemag';
const BUNDLE_NAME_OTHER = 'com.ohos.acepackage';
const USER_ID_100 = 100;


export default function applicationBundleJsunit() {
  describe('appInfoTest', function () {
    /**
     * @tc.number: bundle_getApplicationInfo_test_0100_001
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0100_001, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
        USER_ID_100)
        .catch((error) => {
          console.info(TAG_TEST_0100_001 + 'UserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0100_001, startTime, endTime);
      console.info(TAG_TEST_0100_001 + ' UserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0100_001, data);
      getApplicationInfoSuccess(TAG_TEST_0100_001, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0100_002
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0100_002, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_ALL_APPLICATION_INFO, USER_ID_100)
        .catch((error) => {
          console.info(TAG_TEST_0100_002 + 'UserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0100_002, startTime, endTime);
      console.info(TAG_TEST_0100_002 + ' UserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0100_002, data);
      getApplicationInfoSuccess_plus(TAG_TEST_0100_002, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0100_003
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0100_003, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_DISABLE,
        USER_ID_100)
        .catch((error) => {
          console.info(TAG_TEST_0100_003 + 'UserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0100_003, startTime, endTime);
      console.info(TAG_TEST_0100_003 + ' UserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0100_003, data);
      getApplicationInfoSuccess_plus(TAG_TEST_0100_003, data);
      done();
    });


    /**
     * @tc.number: bundle_getApplicationInfo_test_0200_004
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0200_001, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION)
        .catch((error) => {
          console.info(TAG_TEST_0200_001 + 'onUserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0200_001, startTime, endTime);
      console.info(TAG_TEST_0200_001 + ' onUserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0200_001, data);
      getApplicationInfoSuccess(TAG_TEST_0200_001, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0200_005
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0200_002, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_ALL_APPLICATION_INFO)
        .catch((error) => {
          console.info(TAG_TEST_0200_002 + 'onUserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0200_002, startTime, endTime);
      console.info(TAG_TEST_0200_002 + ' onUserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0200_002, data);
      getApplicationInfoSuccess_plus(TAG_TEST_0200_002, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0200_006
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0200_003, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_DISABLE)
        .catch((error) => {
          console.info(TAG_TEST_0200_003 + 'onUserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0200_003, startTime, endTime);
      console.info(TAG_TEST_0200_003 + ' onUserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0200_003, data);
      getApplicationInfoSuccess_plus(TAG_TEST_0200_003, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0300_007
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by callBack)
     * @tc.level 0
     */
    it(TAG_TEST_0300_001, 0, async function (done) {
      let errors;
      let datas;
      var startTime = await Utils.getNowTime();
      Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
        USER_ID_100, (error, data) => {
          var endTime = Utils.getNowTime();
          Utils.getDurationTime(TAG_TEST_0300_001, startTime, endTime);
          errors = error;
          if(errors){
            expect(errors).assertFail();
            console.info(TAG_TEST_0300_001 + 'UserId callBack error: ' + error);
          }
          console.info(TAG_TEST_0300_001 + 'UserId callBack data is:' + JSON.stringify(data));
          datas = data;
        });
      await Utils.sleep(2000);
      console.info(TAG_TEST_0300_001 + 'UserId callBack data is: ' + JSON.stringify(datas));
      expectData(TAG_TEST_0300_001, datas);
      getApplicationInfoSuccess(TAG_TEST_0300_001, datas);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0400_008
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by callBack)
     * @tc.level 0
     */
    it(TAG_TEST_0400_001, 0, async function (done) {
      let errors;
      let datas;
      var startTime = await Utils.getNowTime();
      Bundle.getApplicationInfo(BUNDLE_NAME, Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION, (error, data) => {
        var endTime = Utils.getNowTime();
        errors = error;
        if(errors){
          expect(errors).assertFail();
          console.info(TAG_TEST_0400_001 + 'UserId callBack error: ' + error);
        }
        Utils.getDurationTime(TAG_TEST_0400_001, startTime, endTime);
        console.info(TAG_TEST_0400_001 + 'noUserId callBack data is:' + JSON.stringify(data));
        datas = data;
      });
      await Utils.sleep(2000);
      console.info(TAG_TEST_0400_001 + 'noUserId callBack data is: ' + JSON.stringify(datas));
      expectData(TAG_TEST_0400_001, datas);
      getApplicationInfoSuccess(TAG_TEST_0400_001, datas);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0500_009
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0500_001, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      await Bundle.getApplicationInfo('', 0)
        .then((data) => {
          console.info(TAG_TEST_0500_001 + 'noUserId promise data is: ' + data);
          expect(data).assertFail();
        })
        .catch((error) => {
          console.info(TAG_TEST_0500_001 + 'noUserId promise error is: ' + error);
          expect(error).assertEqual(1);
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0500_001, startTime, endTime);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0500_0010
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0500_002, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      await Bundle.getApplicationInfo('', '0', USER_ID_100)
        .then((data) => {
          console.info(TAG_TEST_0500_002 + 'UserId promise data is: ' + data);
          expect(data).assertFail();
        })
        .catch((error) => {
          console.info(TAG_TEST_0500_002 + 'UserId promise error is: ' + error);
          expect(error).assertEqual(1);
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0500_002, startTime, endTime);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0600_0011
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0600_001, 0, async function (done) {
      let errors
      var startTime = await Utils.getNowTime();
      await Bundle.getApplicationInfo(BUNDLE_NAME_OTHER,
        Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION)
        .then((data) => {
          console.info(TAG_TEST_0600_001 + 'other bundleName noUserId promise data is: ' + data);
          expect(data).assertFail();
        })
        .catch((error) => {
          console.info(TAG_TEST_0600_001 + 'other bundleName noUserId promise error is: ' + error);
          errors = error
          expect(errors).assertEqual(1);
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0600_001, startTime, endTime);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0600_0012
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0600_002, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let errors
      await Bundle.getApplicationInfo(BUNDLE_NAME_OTHER, Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION,
          USER_ID_100)
          .then((data) => {
            console.info(TAG_TEST_0600_002 + 'other bundleName UserId promise data is: ' + data);
            expect(data).assertFail();
          })
          .catch((error) => {
            console.info(TAG_TEST_0600_002 + 'other bundleName UserId promise error is: ' + error);
            errors = error
            expect(errors).assertEqual(1);
          });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0600_002, startTime, endTime);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0500_009
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level 0
     */
    it(TAG_TEST_0600_003, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      await Bundle.getApplicationInfo(BUNDLE_NAME, 0)
        .then((data) => {
          var endTime = Utils.getNowTime();
          Utils.getDurationTime(TAG_TEST_0600_003, startTime, endTime);
          console.info(TAG_TEST_0600_003 + 'noUserId promise data is: ' + data);
          expect(typeof (data)).assertEqual("object");
          expectData(TAG_TEST_0600_003, data);
          getApplicationInfoSuccess_plus(TAG_TEST_0600_003, data);
        })
        .catch((error) => {
          console.info(TAG_TEST_0600_003 + 'noUserId promise error is: ' + error);
          expect(error).assertFail();
        });
      done();
    });


    /**
     * expect属性
     * @param msg log信息
     * @param data 数据源
     */
    function expectData(msg, data) {
      expect(typeof (data)).assertEqual('object');
      expect(typeof (data.name)).assertEqual('string');
      expect(typeof (data.codePath)).assertEqual('string');
      expect(typeof (data.accessTokenId)).assertEqual('number');
      expect(typeof (data.description)).assertEqual('string');
      expect(typeof (data.descriptionId)).assertEqual('number');
      expect(typeof (data.icon)).assertEqual('string');
      expect(typeof (data.iconId)).assertEqual('number');
      expect(typeof (data.label)).assertEqual('string');
      expect(typeof (data.labelId)).assertEqual('number');
      expect(typeof (data.systemApp)).assertEqual('boolean')
      expect(typeof (data.supportedModes)).assertEqual('number');
      expect(typeof (data.process)).assertEqual('string');
      expect(typeof (data.entryDir)).assertEqual('string');
      expect(typeof (data.metaData)).assertEqual('object');
      expect(typeof (data.metadata)).assertEqual('object');
      expect(typeof (data.enabled)).assertEqual('boolean');
      expect(typeof (data.flags)).assertEqual('number');
      expect(typeof (data.uid)).assertEqual('number');
      expect(typeof (data.entityType)).assertEqual('string');
      expect(typeof (data.removable)).assertEqual('boolean');
      expect(Array.isArray(data.permissions)).assertEqual(true);
      expect(Array.isArray(data.moduleSourceDirs)).assertEqual(true);
      expect(Array.isArray(data.moduleInfos)).assertEqual(true);
    }

    /**
     * expect属性值
     * @param msg log信息
     * @param data 数据源
     */
    function getApplicationInfoSuccess(msg, data) {
      expect(data.name).assertEqual('com.open.harmony.packagemag');
      expect(data.codePath).assertEqual('/data/app/el1/bundle/public/com.open.harmony.packagemag');
      expect(data.accessTokenId > 0).assertEqual(true);
      expect(data.description).assertEqual('$string:mainability_description');
      expect(data.descriptionId > 0).assertEqual(true);
      expect(data.icon).assertEqual('$media:icon');
      expect(data.iconId > 0).assertEqual(true);
      expect(data.uid > 0).assertEqual(true);
      expect(data.label).assertEqual('$string:entry_MainAbility');
      expect(data.labelId > 0).assertEqual(true);
      expect(data.systemApp).assertEqual(true);
      expect(data.supportedModes).assertEqual(0);
      expect(data.flags).assertEqual(0);
      expect(data.process).assertEqual("");
      expect(data.entryDir).assertEqual('/data/app/el1/bundle/public/com.open.harmony.packagemag' +
      '/com.open.harmony.packagemag');
      expect(data.enabled).assertEqual(true);
      expect(data.entityType).assertEqual('unspecified');
      expect(data.removable).assertEqual(true);
      expect(data.moduleInfos[0].moduleName).assertEqual('entry');
      expect(data.moduleInfos[0].moduleSourceDir).assertEqual('/data/app/el1/bundle/public/' +
      'com.open.harmony.packagemag/com.open.harmony.packagemag');
      expect(data.moduleSourceDirs[0]).assertEqual('/data/app/el1/bundle/public/' +
      'com.open.harmony.packagemag/com.open.harmony.packagemag');
      expect(data.permissions[0]).assertEqual("ohos.permission.CHANGE_ABILITY_ENABLED_STATE");
      expect(data.permissions[1]).assertEqual("ohos.permission.GET_BUNDLE_INFO");
      expect(data.permissions[2]).assertEqual("ohos.permission.GET_BUNDLE_INFO_PRIVILEGED");
      console.log(msg + ' end  ' + JSON.stringify(data));
    }

    /**
     * expect属性值
     * @param msg log信息
     * @param data 数据源
     */
    function getApplicationInfoSuccess_plus(msg, data) {
      expect(data.name).assertEqual('com.open.harmony.packagemag');
      expect(data.codePath).assertEqual('/data/app/el1/bundle/public/com.open.harmony.packagemag');
      expect(data.accessTokenId > 0).assertEqual(true);
      expect(data.description).assertEqual('$string:mainability_description');
      expect(data.descriptionId > 0).assertEqual(true);
      expect(data.icon).assertEqual('$media:icon');
      expect(data.iconId > 0).assertEqual(true);
      expect(data.uid > 0).assertEqual(true);
      expect(data.label).assertEqual('$string:entry_MainAbility');
      expect(data.labelId > 0).assertEqual(true);
      expect(data.systemApp).assertEqual(true);
      expect(data.supportedModes).assertEqual(0);
      expect(data.flags).assertEqual(0);
      expect(data.process).assertEqual("");
      expect(data.entryDir).assertEqual('/data/app/el1/bundle/public/com.open.harmony.packagemag' +
      '/com.open.harmony.packagemag');
      expect(data.enabled).assertEqual(true);
      expect(data.entityType).assertEqual('unspecified');
      expect(data.removable).assertEqual(true);
      expect(data.moduleInfos[0].moduleName).assertEqual('entry');
      expect(data.moduleInfos[0].moduleSourceDir).assertEqual('/data/app/el1/bundle/public/' +
      'com.open.harmony.packagemag/com.open.harmony.packagemag');
      expect(data.moduleSourceDirs[0]).assertEqual('/data/app/el1/bundle/public/' +
      'com.open.harmony.packagemag/com.open.harmony.packagemag');
      console.log(msg + ' end  ' + JSON.stringify(data));
    }
  });
}