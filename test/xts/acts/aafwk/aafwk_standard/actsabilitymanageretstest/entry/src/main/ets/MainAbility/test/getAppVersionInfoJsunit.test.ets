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
import {describe, it, expect} from "deccjsunit/index.ets";
import Utils from './Utils';
import ability_featureAbility from '@ohos.ability.featureAbility';

export default function getHapModuleInfoJsunit() {
  describe('appInfoTest', function () {
    /**
     * @tc.number: context_getAppVersionInfo_test_0100
     * @tc.name: getAppVersionInfo:Gets the version information for the application.
     * @tc.desc: Check the return type of the interface (by Promise)
     * @tc.level  0
     */
    it('context_getAppVersionInfo_test_0100', 0, async function (done) {
      console.info('[context_getAppVersionInfo_test_0100] START');
      await Utils.sleep(1000);
      var timeOldStamp = await Utils.getNowTime();
      var context = ability_featureAbility.getContext();
      context.getAppVersionInfo()
        .then((data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getAppVersionInfo_test_0100', timeOldStamp, timeNewStamp);
          console.info('[context_getAppVersionInfo_test_0100] data is:' + JSON.stringify(data));
          expect(typeof (data)).assertEqual("object");
        }).catch((error) => {
        console.error('[context_getAppVersionInfo_test_0100] failed:' + JSON.stringify(error));
      })
      var promise = await context.getAppVersionInfo();
      console.info('---[context_getAppVersionInfo_test_0100] check---');
      checkAppVersionInfo('context_getAppVersionInfo_test_0100', promise);
      console.info('[context_getAppVersionInfo_test_0100] FINISH');
      done();
    });

    /**
     * @tc.number: context_getAppVersionInfo_test_0200
     * @tc.name: getAppVersionInfo:Gets the version information for the application.
     * @tc.desc: Check the return type of the interface (by Callback)
     * @tc.level  0
     */
    it('context_getAppVersionInfo_test_0200', 0, async function (done) {
      try {
        console.info('[context_getAppVersionInfo_test_0200] START');
        await Utils.sleep(1000);
        var timeOldStamp = await Utils.getNowTime();
        var context = ability_featureAbility.getContext();
        context.getAppVersionInfo((error, data) => {
          var timeNewStamp = Utils.getNowTime();
          Utils.getDurationTime('context_getAppVersionInfo_test_0200', timeOldStamp, timeNewStamp);
          console.info('[context_getAppVersionInfo_test_0200] data is:' + JSON.stringify(data));
          expect(typeof (data)).assertEqual("object");
        });
      }
      catch (error) {
        console.error('[context_getAppVersionInfo_test_0200] failed: ' + JSON.stringify(error));
      }
      var callback = await context.getAppVersionInfo();
      console.info('---[context_getAppVersionInfo_test_0200] check---');
      checkAppVersionInfo('context_getAppVersionInfo_test_0200', callback);
      console.info('[context_getAppVersionInfo_test_0200] FINISH');
      done();
    });

    function checkAppVersionInfo(msg, data) {
      console.log(msg + "checkAppVersionInfo start : " + JSON.stringify(data));
      console.log(msg + "checkAppVersionInfo appName : " + data.appName);
      console.log(msg + "checkAppVersionInfo versionCode : " + data.versionCode);
      console.log(msg + "checkAppVersionInfo versionName : " + data.versionName);
      expect(typeof (data)).assertEqual("object");
      expect(typeof (data.appName)).assertEqual("string");
      expect(typeof (data.versionCode)).assertEqual("number");
      expect(typeof (data.versionName)).assertEqual("string");
      expect(data.appName).assertEqual("");
      expect(data.versionCode).assertEqual(1000000);
      expect(data.versionName).assertEqual("1.0.0");
      console.log(msg + "---checkAppVersionInfo End--- ");
    }
  })
}