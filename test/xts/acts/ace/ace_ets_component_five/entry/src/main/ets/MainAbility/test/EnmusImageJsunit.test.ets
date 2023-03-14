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

export default function enumsImageJsunit() {
  describe('enumsImage', function () {
    beforeEach(async function (done) {
      console.info('beforeEach ===================enumsImage test beforeEach called===================');
      let options = {
        uri: 'pages/enumsImage',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info('beforeEach get enumsImage state pages:' + JSON.stringify(pages));
        if (!('enumsFlexItem' == pages.name)) {
          console.info('beforeEach get enumsItemAlign state pages.name:' + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info('beforeEach push enumsImageTest page result:' + JSON.stringify(result));
        }
      } catch (err) {
        console.error('beforeEach push enumsImageTest page error:' + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info('[ afterEach ] ===================enumsImageTest test after each called===================');
    });

    it('enumsImageTest_0100', 0, async function (done) {
      console.info('enumsImageTest_0100 START');
      let strJson = getInspectorByKey('Image');
      console.info("enumsImageTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("enumsImageTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.backgroundImageSize).assertEqual('ImageSize.Auto');
      console.info('enumsImageTest_0100 END');
      done();
    });

    it('enumsImageTest_0200', 0, async function (done) {
      console.info('enumsImageTest_0200 START');
      let strJson = getInspectorByKey('Image');
      console.info("enumsImageTest_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("enumsImageTest_0200 component obj is: " + JSON.stringify(obj));
      let obj1 = obj.$attrs.backgroundImage.split(', ')
      console.info("enumsImageTest_0200 component obj1 is: " + obj1[1]);
      expect(obj1[1]).assertEqual('ImageRepeat.Y');
      console.info('enumsImageTest_0200 END');
      done();
    });
  })
}
