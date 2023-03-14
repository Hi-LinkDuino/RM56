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
     * @tc.number: context_getApplicationContext_test_0100
     * @tc.name: getApplicationContext:Gets the application context information.
     * @tc.desc: Check the return value of the interface
     * @tc.level  0
     */
    it('context_getApplicationContext_test_0100', 0, async function (done) {
      try {
        console.info('[context_getApplicationContext_test_0100] START');
        await Utils.sleep(1000);
        var timeOldStamp = await Utils.getNowTime();
        var context = ability_featureAbility.getContext().getApplicationContext();
        var timeNewStamp = Utils.getNowTime();
        Utils.getDurationTime('context_getApplicationContext_test_0100', timeOldStamp, timeNewStamp);
        console.info('[context_getApplicationContext_test_0100] data is:' + JSON.stringify(context));
      }
      catch (error) {
        console.error('[context_getApplicationContext_test_0100] failed:' + JSON.stringify(error));
      }
      expect(typeof (context)).assertEqual("object");
      console.info('[context_getApplicationContext_test_0100] FINISH');
      done();
    });

  })
}