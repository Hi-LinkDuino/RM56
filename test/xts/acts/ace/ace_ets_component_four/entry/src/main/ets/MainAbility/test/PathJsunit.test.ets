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
import events_emitter from '@ohos.events.emitter'
import Utils from './Utils';

export default function enableJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("path beforeEach start");
      let options = {
        uri: 'pages/path',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get path state pages: " + JSON.stringify(pages));
        if (!("path" == pages.name)) {
          console.info("get overlay state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push path page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push path page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("path after each called");
    });

    it('testPath01', 0, async function (done) {
      console.info('[testPath01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('path1');
      let obj = JSON.parse(strJson);
      console.info("[testPath01] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual('300.00px');
      expect(obj.$attrs.height).assertEqual('10.00px');
      console.info('[testPath01] END');
      done();
    });

    it('testPath02', 0, async function (done) {
      console.info('[testPath02] START');
      try {
        var eventData = {
          data: {
            "height": 200,
            "commands": "M0 110 L900 0",
            "width": 200,
          }
        }
        var innerEvent = {
          eventId: 151,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testPath02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testPath02] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('path1');
      let obj = JSON.parse(strJson);
      console.info("[testPath02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual('200.00px');
      expect(obj.$attrs.height).assertEqual('200.00px');
      console.info('testPath02 END');
      done();
    });
  })
}
