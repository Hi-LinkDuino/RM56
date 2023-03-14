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

export default function blankJsunit() {
  describe('blankTest', function () {
    beforeEach(async function (done) {
      console.info("blank beforeEach start");
      let options = {
        uri: 'pages/blank',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get blank state pages: " + JSON.stringify(pages));
        if (!("blank" == pages.name)) {
          console.info("get blank state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push blank page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push blank page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("blank after each called");
    })

    it('testBlank01', 0, async function (done) {
      console.info('[testBlank01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('blank');
      let obj = JSON.parse(strJson);
      console.info("[testBlank01] component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Blank');
      expect(obj.$attrs.color).assertEqual('#FF0000FF');
      expect(obj.$attrs.min).assertEqual('0.00px');
      done();
    });

    it('testBlank02', 0, async function (done) {
      console.info('[testBlank02] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "colorValue": Color.Red,
          }
        }
        var innerEvent = {
          eventId: 69,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBlank02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBlank02] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('blank');
      let objNew = JSON.parse(strJsonNew);
      console.info("[testBlank02] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.color).assertEqual('#FFFF0000');
      expect(objNew.$type).assertEqual('Blank');
      done();
    });
  })
}
