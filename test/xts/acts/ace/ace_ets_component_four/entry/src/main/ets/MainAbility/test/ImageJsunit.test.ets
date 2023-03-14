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

export default function imageJsunit() {
  describe('imageText', function () {
    beforeEach(async function (done) {
      console.info("image beforeEach start");
      let options = {
        uri: 'pages/image',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get image state pages:" + JSON.stringify(pages));
        if (!("image" == pages.name)) {
          console.info("get image state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push image page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push image page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("image after each called");
    });

    it('imageTest_0100', 0, async function (done) {
      console.info('imageTest_0100 START');
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('text');
      console.info("imageTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("imageTest_0100 obj.$attrs.content is: " + obj.$attrs.content);
      expect(obj.$attrs.content).assertEqual('string_hello');
      expect(obj.$attrs.fontSize).assertEqual('9.00fp');
      console.info('imageTest_0100 END');
      done();
    });

    it('imageTest_0200', 0, async function (done) {
      console.info('imageTest_0200 START');
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('image');
      console.info("imageTest_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("imageTest_0200 obj.$attrs.src is: " + obj.$attrs.src);
      console.info("imageTest_0200 obj.obj.$type is: " + obj.$type);
      var res = obj.$attrs.src.indexOf('rawfile/test.png');
      console.info("imageTest_0200 result is: " + res);
      var sres = obj.$attrs.src.slice(res, res + 16);
      console.info("imageTest_0200 slice result is: " + sres);
      expect(obj.$type).assertEqual('Image');
      expect(obj.$attrs.src.slice(res, res + 16)).assertEqual('rawfile/test.png');
      console.info('imageTest_0200 END');
      done();
    });
  })
}