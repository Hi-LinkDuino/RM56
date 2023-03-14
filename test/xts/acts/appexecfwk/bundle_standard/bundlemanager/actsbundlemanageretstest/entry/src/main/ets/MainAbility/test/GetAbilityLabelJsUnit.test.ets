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
const SERVICE_ABILITY_NAME = 'com.open.harmony.packagemag.ServiceAbility'

const OTHER_BUNDLE_NAME = 'com.ohos.acepackage'
const OTHER_ABILITY_NAME = 'com.ohos.acepackage.MainAbility'

const BUNDLE_NAME_ERROR = 'com.ohos.acepackage.error'
const ABILITY_NAME_ERROR = 'com.ohos.acepackage.error.MainAbility'

export default function getAbilityLabelJsUnit() {

  describe('context_getAbilityLabel_test', function () {
    /*
     * @tc.number: context_getAbilityLabel_test_0100
     * @tc.name: getAbilityLabel : Gets the specified ability label
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_getAbilityLabel_test_0100', 0, async function (done) {
      let mData;
      var timeOldStamp = await Utils.getNowTime();
      await Bundle.getAbilityLabel(BUNDLE_NAME, ABILITY_NAME)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_getAbilityLabel_test_0100]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_getAbilityLabel_test_0100]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          console.info('[context_getAbilityLabel_test_0100]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getAbilityLabelSuccess('[context_getAbilityLabel_test_0100]', mData);
      done();
    });

    /*
     * @tc.number: context_getAbilityLabel_test_0200
     * @tc.name: getAbilityLabel : Gets the specified ability label
     * @tc.desc: Check the return value of the interface (by callbac)
     * @tc.level   0
     */
    it('context_getAbilityLabel_test_0200', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.getAbilityLabel(BUNDLE_NAME, ABILITY_NAME, (error, data) => {
        if(error){
          console.error('[context_getAbilityLabel_test_0200]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_getAbilityLabel_test_0200]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_getAbilityLabel_test_0200]  callBack error: ' + error);
        console.info('[context_getAbilityLabel_test_0200]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getAbilityLabelSuccess('[context_getAbilityLabel_test_0200]', mData);
      done();
    });

    /*
     * @tc.number: context_getAbilityLabel_test_0300
     * @tc.name: getAbilityLabel : Gets the specified ability label
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_getAbilityLabel_test_0300', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mError;
      await Bundle.getAbilityLabel(BUNDLE_NAME_ERROR, ABILITY_NAME_ERROR)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_getAbilityLabel_test_0300]', timeOldStamp, timeNewStamp);
          console.info('[context_getAbilityLabel_test_0300]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          var timeNewStamp = Utils.getNowTime();
          mError = error;
          Utils.getDurationTime('[context_getAbilityLabel_test_0300]', timeOldStamp, timeNewStamp);
          console.info('[context_getAbilityLabel_test_0300]  promise error is: ' + error);
          expect(mError).assertEqual(1);
        });
      await Utils.sleep(2000);
      getAbilityLabelFailure('[context_getAbilityLabel_test_0300]', mError);
      done();
    });

    /*
     * @tc.number: context_getAbilityLabel_test_0400
     * @tc.name: getAbilityLabel : Gets the specified ability label
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_getAbilityLabel_test_0400', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mError;
      Bundle.getAbilityLabel(BUNDLE_NAME_ERROR, ABILITY_NAME_ERROR, (error, data) => {
        if(error !== null){
          console.info('[context_getAbilityLabel_test_0400]  callBack error: ' + error);
          expect(error).assertEqual(1);
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_getAbilityLabel_test_0400]', timeOldStamp, timeNewStamp);
        mError = error;
        console.info('[context_getAbilityLabel_test_0400]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getAbilityLabelFailure('[context_getAbilityLabel_test_0400]', mError);
      done();
    });

    /*
     * @tc.number: context_getAbilityLabel_test_0500
     * @tc.name: getAbilityLabel : Gets the specified ability label
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_getAbilityLabel_test_0500', 0, async function (done) {
      let mData;
      let mError = null;
      var timeOldStamp = await Utils.getNowTime();
      await Bundle.getAbilityLabel(OTHER_BUNDLE_NAME, OTHER_ABILITY_NAME)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_getAbilityLabel_test_0500]', timeOldStamp, timeNewStamp);
          mData = data;
          expect(mData).assertFail();
          console.info('[context_getAbilityLabel_test_0500]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          mError = error;
          console.info('[context_getAbilityLabel_test_0500]  promise error is: ' + error);
          getAbilityLabelFailure('[context_getAbilityLabel_test_0500]', mError);
        });
      done();
    });

    /*
     * @tc.number: context_getAbilityLabel_test_0600
     * @tc.name: getAbilityLabel : Gets the specified ability label
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_getAbilityLabel_test_0600', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      Bundle.getAbilityLabel(OTHER_BUNDLE_NAME, OTHER_ABILITY_NAME, (error, data) => {
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_getAbilityLabel_test_0600]', timeOldStamp, timeNewStamp);
        console.info('[context_getAbilityLabel_test_0600]  callBack error: ' + error);
        getAbilityLabelFailure('[context_getAbilityLabel_test_0600]', error);
        expect(data).assertEqual(undefined);
      });
      done();
    });


    /*
     * @tc.number: context_getAbilityLabel_test_0700
     * @tc.name: getAbilityLabel : Gets the specified ability label
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_getAbilityLabel_test_0700', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mError;
      Bundle.getAbilityLabel(BUNDLE_NAME, SERVICE_ABILITY_NAME, (error, data) => {
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_getAbilityLabel_test_0700]', timeOldStamp, timeNewStamp);
        mError = error;
        if(mError !== null){
          console.info('[context_getAbilityLabel_test_0600]  callBack error: ' + error);
        }
        console.info('[context_getAbilityLabel_test_0700]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getAbilityLabelFailure('[context_getAbilityLabel_test_0700]', mError);
      done();
    });

    function getAbilityLabelSuccess(msg, data) {
      console.log(msg + ' start  ' + JSON.stringify(data));
      console.log(msg + ' result  ' + data);
      expect(typeof (data)).assertEqual('string');
      expect(data).assertEqual("entry_MainAbility");
    }

    function getAbilityLabelFailure(msg, error) {
      console.log(msg + ' Failure  ');
      console.log(msg + ' error  ' + JSON.stringify(error));
      expect(typeof (error)).assertEqual('number');
      expect(error).assertEqual(1)
    }
  })
}