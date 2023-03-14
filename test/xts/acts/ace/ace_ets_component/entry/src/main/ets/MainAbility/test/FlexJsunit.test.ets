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
import router from '@ohos.events.emitter';
import Utils from './Utils.ets';
import router from '@system.router';

export default function flexJsunit() {
  describe('flexTest', function () {
    beforeEach(async function (done) {
      console.info("flex beforeEach start");
      let options = {
        uri: 'pages/flex',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get flex state success " + JSON.stringify(pages));
        if (!("flex" == pages.name)) {
          console.info("get flex state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push flex page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push flex page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("flex after each called");
    });

    it('testFlex_0100', 0, async function (done) {
      console.info("testFlex_0100 START");
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('flexBasis');
      console.info("testFlex_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      await Utils.sleep(2000);
      console.info("testFlex_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.flexBasis).assertEqual("auto");
      console.info('testFlex_0100 END');
      done();
    });

    it('testFlex_0200', 0, async function (done) {
      console.info('testFlex_0200 START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('flexGrow');
      console.info("testFlex_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testFlex_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.flexGrow).assertEqual(0);
      console.info('testFlex_0200 END');
      done();
    });

    it('testFlex_0300', 0, async function (done) {
      console.info('testFlex_0300 START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('flexShrink');
      console.info("testFlex_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testFlex_0300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.flexShrink).assertEqual(0);
      console.info('testFlex_0300 END');
      done();
    });

    it('testFlex_0400', 0, async function (done) {
      console.info('testFlex_0400 START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('alignSelf');
      console.info("testFlex_0400 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testFlex_0400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.alignSelf).assertEqual("ItemAlign.Auto");
      console.info('testFlex_0400 END');
      done();
    });

    it('testFlex_0500', 0, async function (done) {
      console.info('testFlex_0500 START');
      let strJson = getInspectorByKey('flex');
      console.info("testFlex_0500 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testFlex_0500 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Flex');
      done();
    });

    it('testFlex_0600', 0, async function (done) {
      console.info('testFlex_0600 START');
      let strJson = getInspectorByKey('flex');
      console.info("testFlex_0600 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testFlex_0600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.constructor.direction).assertEqual('FlexDirection.Row');
      console.info('testFlex_0600 END');
      done();
    });
  })
}