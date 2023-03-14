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

const BUNDLE_NAME = 'com.open.harmony.packagemag'
const ABILITY_NAME = 'com.open.harmony.packagemag.MainAbility'
const ABILITY = {
  bundleName: BUNDLE_NAME,
  name: ABILITY_NAME
}

const BUNDLE_NAME_ERROR = 'com.ohos.acepackage.error'
const ABILITY_NAME_ERROR = 'com.ohos.acepackage.error.MainAbility'
const ABILITY_ERROR = {
  bundleName: BUNDLE_NAME_ERROR,
  name: ABILITY_NAME_ERROR
}

export default function isAbilityEnabledETSUnit() {
  describe('context_isAbilityEnabled_test', function () {
    /*
     * @tc.number: context_isAbilityEnabled_test_0100
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0100', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      await Bundle.isAbilityEnabled(ABILITY)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isAbilityEnabled_test_0100]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_isAbilityEnabled_test_0100]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          console.info('[context_isAbilityEnabled_test_0100]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getAbilityEnabledSuccess('[context_isAbilityEnabled_test_0100]', mData);
      done();
    });

    /*
     * @tc.number: context_isAbilityEnabled_test_0200
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0200', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.isAbilityEnabled(ABILITY, (error, data) => {
        if(error){
          console.error('[context_isAbilityEnabled_test_0200]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_isAbilityEnabled_test_0200]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_isAbilityEnabled_test_0200]  callBack error: ' + error);
        console.info('[context_isAbilityEnabled_test_0200]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getAbilityEnabledSuccess('[context_isAbilityEnabled_test_0200]', mData);
      done();
    });

    /*
     * @tc.number: context_isAbilityEnabled_test_0300
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0300', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      await Bundle.isAbilityEnabled(ABILITY_ERROR)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isAbilityEnabled_test_0300]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_isAbilityEnabled_test_0300]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isAbilityEnabled_test_0300]', timeOldStamp, timeNewStamp);
          console.info('[context_isAbilityEnabled_test_0300]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getAbilityEnabledFalse('[context_isAbilityEnabled_test_0300]', mData);
      done();
    });

    /*
     * @tc.number: context_isAbilityEnabled_test_0400
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0400', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.isAbilityEnabled(ABILITY_ERROR, (error, data) => {
        if(error){
          console.error('[context_isAbilityEnabled_test_0400]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_isAbilityEnabled_test_0300]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_isAbilityEnabled_test_0400]  callBack error: ' + error);
        console.info('[context_isAbilityEnabled_test_0400]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      console.info('[context_isAbilityEnabled_test_0400]  Failure ');
      getAbilityEnabledFalse('[context_isAbilityEnabled_test_0300]', mData);
      done();
    });


    /*
     * @tc.number: context_isAbilityEnabled_test_0500
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0500', 0, async function (done) {
      await Bundle.setAbilityEnabled(ABILITY, true)
        .then((data) => {
          console.info('[context_isAbilityEnabled_test_0500]  set enable true data is: ' + JSON.stringify(data));
        }).catch((error) => {
          console.info('[context_isAbilityEnabled_test_0500]  set enable true error is: ' + error);
          expect(error).assertFail();
        })

      var timeOldStamp = await Utils.getNowTime();
      let mData;
      await Bundle.isAbilityEnabled(ABILITY)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isAbilityEnabled_test_0500]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_isAbilityEnabled_test_0500]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          console.info('[context_isAbilityEnabled_test_0500]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getAbilityEnabledTrue('[context_isAbilityEnabled_test_0500]', mData);
      done();
    });

    /*
     * @tc.number: context_isAbilityEnabled_test_0600
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0600', 0, async function (done) {
      await Bundle.setAbilityEnabled(ABILITY, false)
        .then((data) => {
          console.info('[context_isAbilityEnabled_test_0600]  set enable false data is: ' + JSON.stringify(data));
        }).catch((error) => {
          console.info('[context_isAbilityEnabled_test_0600]  set enable false error is: ' + error);
          expect(error).assertFail();
        })

      var timeOldStamp = await Utils.getNowTime();
      let mData;
      await Bundle.isAbilityEnabled(ABILITY)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isAbilityEnabled_test_0600]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_isAbilityEnabled_test_0600]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          console.info('[context_isAbilityEnabled_test_0600]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getAbilityEnabledFalse('[context_isAbilityEnabled_test_0600]', mData);
      done();
    });

    /*
     * @tc.number: context_isAbilityEnabled_test_0700
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0700', 0, async function (done) {
      await Bundle.setAbilityEnabled(ABILITY, true)
        .then((data) => {
          console.info('[context_isAbilityEnabled_test_0700]  set enable true data is: ' + JSON.stringify(data));
        }).catch((error) => {
          console.info('[context_isAbilityEnabled_test_0700]  set enable true error is: ' + error);
          expect(error).assertFail();
        })

      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.isAbilityEnabled(ABILITY, (error, data) => {
        if(error){
          console.error('[context_isAbilityEnabled_test_0700]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_isAbilityEnabled_test_0700]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_isAbilityEnabled_test_0700]  callBack error: ' + error);
        console.info('[context_isAbilityEnabled_test_0700]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getAbilityEnabledTrue('[context_isAbilityEnabled_test_0700]', mData);
      done();
    });

    /*
     * @tc.number: context_isAbilityEnabled_test_0800
     * @tc.name: isAbilityEnabled : Get whether to enable a specified ability
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_isAbilityEnabled_test_0800', 0, async function (done) {
      await Bundle.setAbilityEnabled(ABILITY, false)
        .then((data) => {
          console.info('[context_isAbilityEnabled_test_0800]  set enable false data is: ' + JSON.stringify(data));
        }).catch((error) => {
          console.info('[context_isAbilityEnabled_test_0800]  set enable false error is: ' + error);
          expect(error).assertFail();
        })

      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.isAbilityEnabled(ABILITY, (error, data) => {
        if(error){
          console.error('[context_isAbilityEnabled_test_0800]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_isAbilityEnabled_test_0800]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_isAbilityEnabled_test_0800]  callBack error: ' + error);
        console.info('[context_isAbilityEnabled_test_0800]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getAbilityEnabledFalse('[context_isAbilityEnabled_test_0800]', mData);
      done();
    });


    function getAbilityEnabledSuccess(msg, data) {
      console.log(msg + ' start  ' + JSON.stringify(data));
      console.log(msg + ' data : ' + data);
      expect(typeof (data)).assertEqual('boolean')
    }

    function getAbilityEnabledTrue(msg, data) {
      getAbilityEnabledSuccess(msg, data);
      expect(data).assertEqual(true);
    }

    function getAbilityEnabledFalse(msg, data) {
      getAbilityEnabledSuccess(msg, data);
      expect(data).assertEqual(false);
    }

  })
}