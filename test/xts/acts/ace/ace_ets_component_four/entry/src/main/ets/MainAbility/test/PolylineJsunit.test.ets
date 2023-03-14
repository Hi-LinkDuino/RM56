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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets";
import router from '@system.router';
import Utils from './Utils';

export default function polylineJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("polyline beforeEach start");
      let options = {
        uri: 'pages/polyline',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get polyline state pages:" + JSON.stringify(pages));
        if (!("polyline" == pages.name)) {
          console.info("get polyline state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push polyline page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push polyline page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("polyline after each called");
    });

    it('testPolyline001', 0, async function (done) {
      console.info('[testPolyline001] START');
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('polyline');
      let obj = JSON.parse(strJson);
      console.info("[testPolyline001] obj is: " + JSON.stringify(obj));
      console.info('[testPolyline001] END');
      done();
    });
  })
}
