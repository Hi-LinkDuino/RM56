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

export default function fontWeightJsunit() {
  describe('panDirectionTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/panDirection',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get panDirection state success " + JSON.stringify(pages));
        if (!("panDirection" == pages.name)) {
          console.info("get panDirection state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(3000);
          console.info("push panDirection page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push panDirection page error " + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("panDirection after each called");
    });

    it('panDirectionText_0100', 0, async function (done) {
      console.info('panDirectionText_0100 START');
      let strJson = getInspectorByKey('panDirection');
      console.info("panDirectionText_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("panDirectionText_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Flex');
      done();
    });

  })
}