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

export default function richTextJsunit() {
  describe('richTextTest', function () {
    beforeEach(async function (done) {
      console.info("richText beforeEach start");
      let options = {
        uri: 'pages/richText',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get richText state pages:" + JSON.stringify(pages));
        if (!("richText" == pages.name)) {
          console.info("get richText state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push richText page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push richText page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("richText after each called");
    });

    it('testRichText01', 0, async function (done) {
      console.info('[testRichText01] START');
      let strJson = getInspectorByKey('richText');
      console.info('[testRichText01]' + strJson);
      console.info('[testRichText01] END');
      done();
    });
  });
}