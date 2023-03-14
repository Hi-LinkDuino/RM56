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

export default function rowJsunit() {
  describe('rowTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/row',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get row state success " + JSON.stringify(pages));
        if (!("row" == pages.name)) {
          console.info("get row state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push row page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push row page error " + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("row after each called");
    });

    it('testRow_100', 0, async function (done) {
      console.info('[testRow_100] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Row');
      let obj = JSON.parse(strJson);
      console.info("testRow_100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.align).assertEqual("Alignment.Center");
      console.info('testRow_100 END');
      done();
    });

    it('testRow_200', 0, async function (done) {
      console.info('[testRow_200] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Row');
      let obj = JSON.parse(strJson);
      console.info("testRow_200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.direction).assertEqual("Direction.Ltr");
      console.info('testRow_200 END');
      done();
    });

    it('testRow_300', 0, async function (done) {
      console.info('[testRow_300] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Row');
      let obj = JSON.parse(strJson);
      console.info("testRow_300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual("90.00%");
      console.info('testRow_300 END');
      done();
    });

    it('testRow_400', 0, async function (done) {
      console.info('[testRow_400] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Row');
      let obj = JSON.parse(strJson);
      console.info("testRow_400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Auto");
      console.info('testRow_400 END');
      done();
    });

    it('testRow_500', 0, async function (done) {
      console.info('[testRow_500] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Row');
      let obj = JSON.parse(strJson);
      console.info("testRow_500 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual("107.00vp");
      console.info('testRow_500 END');
      done();
    });

    it('testRow_600', 0, async function (done) {
      console.info('[testRow_600] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('Row');
      let obj = JSON.parse(strJson);
      console.info("testRow_600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderStyle).assertEqual("BorderStyle.Solid");
      console.info('testRow_600 END');
      done();
    });
  })
}