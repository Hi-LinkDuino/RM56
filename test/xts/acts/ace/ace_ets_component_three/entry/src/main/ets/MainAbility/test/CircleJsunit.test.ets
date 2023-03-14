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
import events_emitter from '@ohos.events.emitter';
import Utils from './Utils';

export default function circleJsunit() {
  describe('circleTest', function () {
    beforeEach(async function (done) {
      console.info("circle beforeEach start");
      let options = {
        uri: 'pages/circle',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get circle state pages:" + JSON.stringify(pages));
        if (!("circle" == pages.name)) {
          console.info("get circle state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push circle page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push circle page error:" + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("circle after each called");
    });

    it('circleTest_0100', 0, async function (done) {
      console.info('circleTest_0100 START');
      let strJson = getInspectorByKey('circle');
      console.info("circleTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("circleTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual('450.00px');
      expect(obj.$attrs.width).assertEqual('450.00px');
      done();
    });

    it('circleTest_0200', 0, async function (done) {
      console.info('[circleTest_0200] START');
      try {
        var eventData = {
          data: {
            "width": '500px',
            "height": '500px',
          }
        }
        console.info('[circleTest_0200] START');
        var innerEvent = {
          eventId: 77,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[circleTest_0200] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[circleTest_0200] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('circle');
      let obj = JSON.parse(strJson);
      console.info("[circleTest_0200] component objNew is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual('500.00px');
      expect(obj.$attrs.width).assertEqual('500.00px');
      console.info('circleTest_0200 END');
      done();
    });

    it('circleTest_0300', 0, async function (done) {
      console.info('[circleTest_0300] START');
      try {
        var eventData = {
          data: {
            "width": '-a',
            "height": '-b',
          }
        }
        console.info('[circleTest_0300] START');
        var innerEvent = {
          eventId: 77,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[circleTest_030] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[circleTest_030] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('circle');
      let obj = JSON.parse(strJson);
      console.info("[circleTest_0300] component objNew is: " + JSON.stringify(obj));
      expect(obj.$attrs.width).assertEqual('0.00px');
      expect(obj.$attrs.height).assertEqual('0.00px');
      console.info('circleTest_0300 END');
      done();
    });

  })
}
