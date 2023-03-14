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

export default function backgroundJsunit() {
  describe('backgroundTest', function () {
    beforeEach(async function (done) {
      console.info("background beforeEach start");
      let options = {
        uri: 'pages/general-properties/background',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get background state success " + JSON.stringify(pages));
        if (!("background" == pages.name)) {
          console.info("get background state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push background page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push background page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("background after each called");
    });

    it('testBackground_0100', 0, async function (done) {
      console.info('testBackground_0100 START');
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontColor');
      console.info("testBackground_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBackground_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontColor).assertEqual("#FFFF0000");
      console.info('testBackground_0100 END');
      done();
    });

    it('testBackground_0200', 0, async function (done) {
      console.info('testBackground_0200 START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "color": '0xFF0000'
          }
        }
        var innerEvent = {
          eventId: 2,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testBackground_0200 start pages");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testBackground_0200 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('backgroundColor');
      console.info("testBackground_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBackground_0200 rowDirection: " + obj.$attrs.direction);
      expect(obj.$attrs.backgroundColor).assertEqual("#FFEEEE00");
      done();
    });

    it('testBackground_0300', 0, async function (done) {
      console.info('testBackground_0300 START');
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('backgroundColor');
      console.info("testBackground_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBackground_0300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderWidth).assertEqual("1.00vp");
      console.info('testBackground_0300 END');
      done();
    });
  })
}
