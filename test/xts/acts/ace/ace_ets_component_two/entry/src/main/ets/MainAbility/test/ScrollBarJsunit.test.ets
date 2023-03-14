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
import events_emitter from '@ohos.events.emitter';
import Utils from './Utils';

export default function scrollBarJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/scrollBar',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get scrollBar state success " + JSON.stringify(pages));
        if (!("span" == pages.name)) {
          console.info("get scrollBar success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push scrollBar page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push scrollBar page error " + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("scrollBar after each called");
    });

    it('testScrollBar001', 0, async function (done) {
      console.info('[testScrollBar001] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('scroll');
      console.info("[testScrollBar001] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testScrollBar001] direction:" + obj.$attrs.direction);
      expect(obj.$attrs.direction).assertEqual('ScrollBarDirection.Vertical');
      done();
    });

    it('testScrollBar002', 0, async function (done) {
      console.info('[testScrollBar002] START');
      let object = {
        "directionValue": ScrollBarDirection.Horizontal
      };
      try {
        var eventData = {
          data: {
            "directionObject": JSON.stringify(object)
          }
        }
        var innerEvent = {
          eventId: 183,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testScrollBar002] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testScrollBar002] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('scroll');
      let obj = JSON.parse(strJson);
      console.info("[testScrollBar002] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.direction).assertEqual('ScrollBarDirection.Vertical');
      console.info('testScrollBar002 END');
      done();
    });

    it('testScrollBar003', 0, async function (done) {
      console.info('[testScrollBar003] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('scroll');
      console.info("[testScrollBar003] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testScrollBar003] state:" + obj.$attrs.state);
      expect(obj.$attrs.state).assertEqual('BarState.Off');
      done();
    });

    it('testScrollBar004', 0, async function (done) {
      console.info('[testScrollBar004] START');
      try {
        let eventData = {
          data: {
            "stateValue":"BarState.On"
          }
        }
        let indexEvent = {
          eventId: 184,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testScrollBar004] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testScrollBar004] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('scroll');
      console.info("[testScrollBar004] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testScrollBar004] state:" + obj.$attrs.state);
      expect(obj.$attrs.state).assertEqual('BarState.Off');
      done();
    });

    it('testScrollBar005', 0, async function (done) {
      console.info('[testScrollBar005] START');
      try {
        let eventData = {
          data: {
            "stateValue": BarState.Auto
          }
        }
        let indexEvent = {
          eventId: 184,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testScrollBar005] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testScrollBar005] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('scroll');
      console.info("[testScrollBar005] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testScrollBar005] state:" + obj.$attrs.state);
      expect(obj.$attrs.state).assertEqual('BarState.Off');
      done();
    });
  })
}