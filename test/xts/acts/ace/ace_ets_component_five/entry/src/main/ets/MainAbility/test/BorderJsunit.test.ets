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
import Utils from './Utils.ets';

export default function borderJsunit() {
  describe('borderTest', function () {
    beforeEach(async function (done) {
      console.info("border beforeEach start");
      let options = {
        uri: 'pages/border',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get border state success " + JSON.stringify(pages));
        if (!("border" == pages.name)) {
          console.info("get border state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push border page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push border page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("border after each called");
    });

    it('testBorder_0100', 0, async function (done) {
      console.info('testBorder_0100 START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('borderWidth');
      console.info("testBorder_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBorder_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderWidth).assertEqual("5.00vp");
      console.info('testBorder_0100 END');
      done();
    });

    it('testBorder_0200', 0, async function (done) {
      console.info('testBorder_0200 START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('borderColor');
      console.info("testBorder_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBorder_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderColor).assertEqual("#FF317AF7");
      console.info('testBorder_0200 END');
      done();
    });

    it('testBorder_0300', 0, async function (done) {
      console.info('testBorder_0300 START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "borderRadius": 70
          }
        }
        var innerEvent = {
          eventId: 3,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBorder_0300] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testBorder_0300] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('borderRadius');
      console.info("testBorder_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBorder_0300 component obj is: " + obj.$attrs.borderRadius);
      expect(obj.$attrs.borderRadius).assertEqual("70.00vp");
      console.info('testBorder_0300 END');
      done();
    });
  })
}

