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

const BUNDLE_NAME_ERROR = 'com.ohos.acepackage.error'

export default function IsApplicationEnabledETSUnit() {
  describe('context_isApplicationEnabled_test', function () {
    /*
     * @tc.number: context_isApplicationEnabled_test_0100
     * @tc.name: isApplicationEnabled : Get whether to enable a specified application
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_isApplicationEnabled_test_0100', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      await Bundle.isApplicationEnabled(BUNDLE_NAME)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isApplicationEnabled_test_0100]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_isApplicationEnabled_test_0100]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          console.info('[context_isApplicationEnabled_test_0100]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getApplicationEnabledSuccess('[context_isApplicationEnabled_test_0100]', mData);
      done();
    });

    /*
     * @tc.number: context_isApplicationEnabled_test_0200
     * @tc.name: isApplicationEnabled : Get whether to enable a specified application
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_isApplicationEnabled_test_0200', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.isApplicationEnabled(BUNDLE_NAME, (error, data) => {
        if(error){
          console.error('[context_isApplicationEnabled_test_0200]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_isApplicationEnabled_test_0200]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_isApplicationEnabled_test_0200]  callBack error: ' + error);
        console.info('[context_isApplicationEnabled_test_0200]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getApplicationEnabledSuccess('[context_isApplicationEnabled_test_0200]', mData);
      done();
    });

    /*
     * @tc.number: context_isApplicationEnabled_test_0300
     * @tc.name: isApplicationEnabled : Get whether to enable a specified application
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_isApplicationEnabled_test_0300', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      await Bundle.isApplicationEnabled(BUNDLE_NAME_ERROR)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isApplicationEnabled_test_0300]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_isApplicationEnabled_test_0300]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isApplicationEnabled_test_0300]', timeOldStamp, timeNewStamp);
          console.info('[context_isApplicationEnabled_test_0300]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getApplicationEnabledFalse('[context_isApplicationEnabled_test_0300]', mData);
      done();
    });

    /*
     * @tc.number: context_isApplicationEnabled_test_0400
     * @tc.name: isApplicationEnabled : Get whether to enable a specified application
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_isApplicationEnabled_test_0400', 0, async function (done) {
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.isApplicationEnabled(BUNDLE_NAME_ERROR, (error, data) => {
        if(error){
          console.error('[context_isApplicationEnabled_test_0400]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_isApplicationEnabled_test_0400]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_isApplicationEnabled_test_0400]  callBack error: ' + error);
        console.info('[context_isApplicationEnabled_test_0400]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getApplicationEnabledFalse('[context_isApplicationEnabled_test_0300]', mData);
      console.info('[context_isApplicationEnabled_test_0400]  Failure ');
      done();
    });

    /*
     * @tc.number: context_isApplicationEnabled_test_0500
     * @tc.name: isApplicationEnabled : Get whether to enable a specified application
     * @tc.desc: Check the return value of the interface (by promise)
     * @tc.level   0
     */
    it('context_isApplicationEnabled_test_0500', 0, async function (done) {
      await Bundle.setApplicationEnabled(BUNDLE_NAME, true)
        .then((data) => {
          console.info('[context_isApplicationEnabled_test_0500]  set enable true data is: ' + JSON.stringify(data));
        }).catch((error) => {
          console.info('[context_isApplicationEnabled_test_0500]  set enable true error is: ' + error);
          expect(error).assertFail();
        });
      await Utils.sleep(1000);
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      await Bundle.isApplicationEnabled(BUNDLE_NAME)
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('[context_isApplicationEnabled_test_0500]', timeOldStamp, timeNewStamp);
          mData = data;
          console.info('[context_isApplicationEnabled_test_0500]  promise data is: ' + JSON.stringify(data));
        })
        .catch((error) => {
          console.info('[context_isApplicationEnabled_test_0500]  promise error is: ' + error);
          expect(error).assertFail();
        });
      getApplicationEnabledTrue('[context_isApplicationEnabled_test_0500]', mData);
      done();
    });

    /*
     * @tc.number: context_isApplicationEnabled_test_0600
     * @tc.name: isApplicationEnabled : Get whether to enable a specified application
     * @tc.desc: Check the return value of the interface (by callback)
     * @tc.level   0
     */
    it('context_isApplicationEnabled_test_0600', 0, async function (done) {
      await Bundle.setApplicationEnabled(BUNDLE_NAME, false)
        .then((data) => {
          console.info('[context_isApplicationEnabled_test_0600]  set enable false data is: ' + JSON.stringify(data));
        }).catch((error) => {
          console.info('[context_isApplicationEnabled_test_0600]  set enable false error is: ' + error);
          expect(error).assertFail();
        });

      await Utils.sleep(1000);
      var timeOldStamp = await Utils.getNowTime();
      let mData;
      Bundle.isApplicationEnabled(BUNDLE_NAME, (error, data) => {
        if(error){
          console.error('[context_isApplicationEnabled_test_0600]Operation failed. Cause: ' + JSON.stringify(error));
          expect(error).assertFail();
        }
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('[context_isApplicationEnabled_test_0600]', timeOldStamp, timeNewStamp);
        mData = data;
        console.info('[context_isApplicationEnabled_test_0600]  callBack error: ' + error);
        console.info('[context_isApplicationEnabled_test_0600]  callBack data is:' + JSON.stringify(data));
      });
      await Utils.sleep(2000);
      getApplicationEnabledFalse('[context_isApplicationEnabled_test_0600]', mData);
      done();
    });

    function getApplicationEnabledSuccess(msg, data) {
      console.log(msg + ' start  ' + JSON.stringify(data));
      console.log(msg + ' data : ' + data);
      expect(typeof (data)).assertEqual('boolean')
    }

    function getApplicationEnabledTrue(msg, data) {
      getApplicationEnabledSuccess(msg, data);
      expect(data).assertEqual(true);
    }

    function getApplicationEnabledFalse(msg, data) {
      getApplicationEnabledSuccess(msg, data);
      expect(data).assertEqual(false);
    }
  })
}