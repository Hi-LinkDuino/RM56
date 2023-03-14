// @ts-nocheck
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets"
import router from '@system.router';
import Utils from './Utils';

export default function enumsVerticalAlignJsunit() {
  describe('enumsVerticalAlign', function () {
    beforeEach(async function (done) {
      console.info('beforeEach ===================enumsFlexItem test beforeEach called===================');
      let options = {
        uri: 'pages/enumsVerticalAlign',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info('beforeEach get enumsVerticalAlign state pages:' + JSON.stringify(pages));
        if (!('enumsVerticalAlign' == pages.name)) {
          console.info('beforeEach get enumsVerticalAlign state pages.name:' + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info('beforeEach push enumsVerticalAlign page result:' + JSON.stringify(result));
        }
      } catch (err) {
        console.error('beforeEach push enumsVerticalAlign page error:' + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info('[ afterEach ] ===================enumsFlexItemTest test after each called===================');
    });

    it('enumsVerticalAlignTest_0100', 0, async function (done) {
      console.info('enumsVerticalAlignTest_0100 START');
      await Utils.sleep(1500);
      let strJson = getInspectorByKey('VerticalAlign');
      console.info("enumsVerticalAlignTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("enumsVerticalAlignTest_0100 component obj is: " + JSON.stringify(obj.$attrs.alignItems));
      expect(obj.$attrs.alignItems).assertEqual('VerticalAlign.Center');
      console.info('enumsVerticalAlignTest_0100 END');
      done();
    });
  })
}
