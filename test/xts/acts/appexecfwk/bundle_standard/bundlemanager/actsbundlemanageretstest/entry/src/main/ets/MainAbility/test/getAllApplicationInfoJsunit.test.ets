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

const TAG_TEST_0100_001 = ' bundle_getAllApplicationInfo_test_0100_001 ';
const TAG_TEST_0100_002 = ' bundle_getAllApplicationInfo_test_0100_002 ';
const TAG_TEST_0100_003 = ' bundle_getAllApplicationInfo_test_0100_003 ';
const TAG_TEST_0200_001 = ' bundle_getAllApplicationInfo_test_0200_004 ';
const TAG_TEST_0200_002 = ' bundle_getAllApplicationInfo_test_0200_005 ';
const TAG_TEST_0200_003 = ' bundle_getAllApplicationInfo_test_0200_006 ';
const TAG_TEST_0300_001 = ' bundle_getAllApplicationInfo_test_0300_007 ';
const TAG_TEST_0400_001 = ' bundle_getAllApplicationInfo_test_0400_008 ';
const TAG_TEST_0500_001 = ' bundle_getAllApplicationInfo_test_0500_009 ';
const TAG_TEST_0500_002 = ' bundle_getAllApplicationInfo_test_0500_0010 ';
const USER_ID_100 = 100;

export default function applicationBundleJsunit() {
  describe('appInfoTest', function () {
    /**
     * @tc.number: bundle_getApplicationInfo_test_0100_001
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0100_001, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION, USER_ID_100)
        .catch((error) => {
          console.info(TAG_TEST_0100_001 + 'UserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0100_001, startTime, endTime);
      console.info(TAG_TEST_0100_001 + ' UserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0100_001, data)
      getApplicationInfoSuccess(TAG_TEST_0100_001, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0100_002
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0100_002, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_ALL_APPLICATION_INFO, USER_ID_100)
        .catch((error) => {
          console.info(TAG_TEST_0100_002 + 'UserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0100_002, startTime, endTime);
      console.info(TAG_TEST_0100_002 + ' UserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0100_002, data)
      getApplicationInfoSuccess(TAG_TEST_0100_002, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0100_003
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0100_003, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_DISABLE, USER_ID_100)
        .catch((error) => {
          console.info(TAG_TEST_0100_003 + 'UserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0100_003, startTime, endTime);
      console.info(TAG_TEST_0100_003 + ' UserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0100_003, data)
      getApplicationInfoSuccess(TAG_TEST_0100_003, data);
      done();
    });


    /**
     * @tc.number: bundle_getApplicationInfo_test_0200_004
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0200_001, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION)
        .catch((error) => {
          console.info(TAG_TEST_0200_001 + 'onUserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0200_001, startTime, endTime);
      console.info(TAG_TEST_0200_001 + ' onUserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0200_001, data)
      getApplicationInfoSuccess(TAG_TEST_0200_001, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0200_005
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0200_002, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_ALL_APPLICATION_INFO)
        .catch((error) => {
          console.info(TAG_TEST_0200_002 + 'onUserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0200_002, startTime, endTime);
      console.info(TAG_TEST_0200_002 + ' onUserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0200_002, data)
      getApplicationInfoSuccess(TAG_TEST_0200_002, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0200_006
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0200_003, 0, async function (done) {
      var startTime = await Utils.getNowTime();
      let data = await Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_DISABLE)
        .catch((error) => {
          console.info(TAG_TEST_0200_003 + 'onUserId promise error is: ' + error);
          expect(error).assertFail();
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0200_003, startTime, endTime);
      console.info(TAG_TEST_0200_003 + ' onUserId promise data is: ' + JSON.stringify(data));
      expectData(TAG_TEST_0200_003, data)
      getApplicationInfoSuccess(TAG_TEST_0200_003, data);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0300_007
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by callBack)
     * @tc.level   0
     */
    it(TAG_TEST_0300_001, 0, async function (done) {
      let datas;
      var startTime = await Utils.getNowTime();
      Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION, USER_ID_100,
        (error, data) => {
          var endTime = Utils.getNowTime();
          Utils.getDurationTime(TAG_TEST_0300_001, startTime, endTime);
          if(error){
            expect(error).assertFail();
            console.info(TAG_TEST_0300_001 + 'UserId callBack error: ' + error);
          }
          console.info(TAG_TEST_0300_001 + 'UserId callBack error: ' + error);
          console.info(TAG_TEST_0300_001 + 'UserId callBack data is:' + JSON.stringify(data));
          datas = data;
        });
      await Utils.sleep(2000);
      console.info(TAG_TEST_0300_001 + 'UserId callBack datas is:' + JSON.stringify(datas));
      expectData(TAG_TEST_0300_001, datas)
      getApplicationInfoSuccess(TAG_TEST_0300_001, datas);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0400_008
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by callBack)
     * @tc.level   0
     */
    it(TAG_TEST_0400_001, 0, async function (done) {
      let datas;
      var startTime = await Utils.getNowTime();
      Bundle.getAllApplicationInfo(Bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION, (error, data) => {
        var endTime = Utils.getNowTime();
        Utils.getDurationTime(TAG_TEST_0400_001, startTime, endTime);
        if(error){
          expect(error).assertFail();
          console.info(TAG_TEST_0400_001 + 'UserId callBack error: ' + error);
        }
        console.info(TAG_TEST_0400_001 + 'noUserId callBack error: ' + error);
        console.info(TAG_TEST_0400_001 + 'noUserId callBack data is:' + JSON.stringify(data));
        datas = data;
      });
      await Utils.sleep(2000);
      console.info(TAG_TEST_0400_001 + 'noUserId callBack datas is:' + JSON.stringify(datas));
      expectData(TAG_TEST_0400_001, datas)
      getApplicationInfoSuccess(TAG_TEST_0400_001, datas);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0500_009
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0500_001, 0, async function (done) {
      let errors;
      var startTime = await Utils.getNowTime();
      await Bundle.getAllApplicationInfo('0')
        .then((data) => {
          console.info(TAG_TEST_0500_001 + 'noUserId promise data is: ' + data);
          expect(data).assertFail();
        })
        .catch((error) => {
          console.info(TAG_TEST_0500_001 + 'noUserId promise error is: ' + error);
          errors = error;
          expect(errors).assertEqual(1);
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0500_001, startTime, endTime);
      done();
    });

    /**
     * @tc.number: bundle_getApplicationInfo_test_0500_0010
     * @tc.name: getApplicationInfo : Obtains based on a given bundle name.
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it(TAG_TEST_0500_002, 0, async function (done) {
      let errors;
      var startTime = await Utils.getNowTime();
      await Bundle.getAllApplicationInfo('0', USER_ID_100)
        .then((data) => {
          console.info(TAG_TEST_0500_002 + 'noUserId promise data is: ' + data);
          expect(data).assertFail();
        })
        .catch((error) => {
          console.info(TAG_TEST_0500_002 + 'UserId promise error is: ' + error);
          errors = error;
          expect(errors).assertEqual(1);
        });
      var endTime = Utils.getNowTime();
      Utils.getDurationTime(TAG_TEST_0500_002, startTime, endTime);
      done();
    });


    /**
     * expect属性
     * @param msg log信息
     * @param data 数据源
     */
    function expectData(msg, data) {
      console.info(msg + 'commonTest data length [' + data.length + ']');
      for (var i = 0; i < data.length; i++) {
        expect(typeof (data)).assertEqual('object');
        expect(typeof (data[i].name)).assertEqual('string');
        expect(typeof (data[i].codePath)).assertEqual('string');
        expect(typeof (data[i].accessTokenId)).assertEqual('number');
        expect(typeof (data[i].description)).assertEqual('string');
        expect(typeof (data[i].descriptionId)).assertEqual('number');
        expect(typeof (data[i].icon)).assertEqual('string');
        expect(typeof (data[i].iconId)).assertEqual('number');
        expect(typeof (data[i].label)).assertEqual('string');
        expect(typeof (data[i].labelId)).assertEqual('number');
        expect(typeof (data[i].systemApp)).assertEqual('boolean')
        expect(typeof (data[i].supportedModes)).assertEqual('number');
        expect(typeof (data[i].process)).assertEqual('string');
        expect(typeof (data[i].entryDir)).assertEqual('string');
        expect(typeof (data[i].metaData)).assertEqual('object');
        expect(typeof (data[i].metadata)).assertEqual('object');
        expect(typeof (data[i].enabled)).assertEqual('boolean');
        expect(typeof (data[i].flags)).assertEqual('number');
        expect(typeof (data[i].uid)).assertEqual('number');
        expect(typeof (data[i].entityType)).assertEqual('string');
        expect(typeof (data[i].removable)).assertEqual('boolean');
        expect(Array.isArray(data[i].permissions)).assertEqual(true);
        expect(Array.isArray(data[i].moduleSourceDirs)).assertEqual(true);
        expect(Array.isArray(data[i].moduleInfos)).assertEqual(true);
      }
    }

    /**
     * expect指定数据属性值
     * @param msg log信息
     * @param data 数据源
     */
    function getApplicationInfoSuccess(msg, data) {
      console.info(msg + 'getApplicationInfoSuccess data length [' + data.length + ']');
      for (var i = 0; i < data.length; i++) {
        if (data[i].name === 'com.open.harmony.packagemag' || i === 0 && data[i].name === '') {
          console.info(msg + JSON.stringify(data[i]));
          expect(data[i].name).assertEqual('com.open.harmony.packagemag');
          expect(data[i].codePath).assertEqual('/data/app/el1/bundle/public/com.open.harmony.packagemag');
          expect(data[i].description).assertEqual('$string:mainability_description');
          if (data[i].descriptionId !== 0) {
            expect(data[i].descriptionId > 0).assertEqual(true);
          }
          expect(data[i].icon).assertEqual('$media:icon');
          expect(data[i].iconId > 0).assertEqual(true);
          expect(data[i].label).assertEqual('$string:entry_MainAbility');
          expect(data[i].accessTokenId > 0).assertEqual(true);
          if (data[i].label !== 0) {
            expect(data[i].labelId > 0).assertEqual(true);
          };
          if (data[i].uid !== 0) {
            expect(data[i].uid > 0).assertEqual(true);
          };
          expect(data[i].systemApp).assertEqual(true);
          expect(data[i].supportedModes).assertEqual(0);
          expect(data[i].process).assertEqual("");
          expect(data[i].entryDir).assertEqual('/data/app/el1/bundle/public/com.open.harmony.packagemag' +
          '/com.open.harmony.packagemag');
          expect(data[i].enabled).assertEqual(true);
          expect(data[i].flags).assertEqual(0);
          expect(data[i].entityType).assertEqual('unspecified');
          expect(data[i].removable).assertEqual(true);
          expect(data[i].moduleInfos[0].moduleName).assertEqual('entry');
          expect(data[i].moduleInfos[0].moduleSourceDir).assertEqual('/data/app/el1/bundle/public/' +
          'com.open.harmony.packagemag/com.open.harmony.packagemag');
          expect(data[i].moduleSourceDirs[0]).assertEqual('/data/app/el1/bundle/public/' +
          'com.open.harmony.packagemag/com.open.harmony.packagemag');
        }
      }
    }
  });
}