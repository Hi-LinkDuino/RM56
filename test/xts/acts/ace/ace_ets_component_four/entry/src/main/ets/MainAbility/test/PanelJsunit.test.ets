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
import events_emitter from '@ohos.events.emitter';

export default function panelJsunit() {
  describe('panelTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/panel',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get panel state success " + JSON.stringify(pages));
        if (!("panel" == pages.name)) {
          console.info("get panel state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push panel page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push panel page error " + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("panel after each called");
    });

    it('testPanel_100', 0, async function (done) {
      console.info('testPanel_100 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "type": "PanelType.Minibar"
          }
        }
        var innerEvent = {
          eventId: 147,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testPanel_100 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testPanel_100 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Panel');
      let obj = JSON.parse(strJson);
      console.info("testPanel_100 component obj is: " + JSON.stringify(obj));
      await Utils.sleep(2000);
      expect(obj.$attrs.type).assertEqual("PanelType.Minibar");
      console.info('testPanel_100 END');
      done();
    });

    it('testPanel_200', 0, async function (done) {
      console.info('testPanel_200 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "mode": "PanelMode.Mini"
          }
        }
        var innerEvent = {
          eventId: 148,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testPanel_200 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testPanel_200 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Panel');
      let obj = JSON.parse(strJson);
      console.info("testPanel_200 component obj is: " + JSON.stringify(obj));
      await Utils.sleep(2000);
      expect(obj.$attrs.mode).assertEqual("PanelMode.Mini");
      console.info('testPanel_200 END');
      done();
    });

    it('testPanel_300', 0, async function (done) {
      console.info('testPanel_300 START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "show": "true"
          }
        }
        var innerEvent = {
          eventId: 149,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testPanel_300 start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testPanel_300 change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Panel');
      let obj = JSON.parse(strJson);
      console.info("testPanel_300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.show).assertEqual("true");
      console.info('testPanel_300 END');
      done();
    });

    it('testPanel_400', 0, async function (done) {
      console.info('testPanel_400 START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Panel');
      let obj = JSON.parse(strJson);
      console.info("testPanel_400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.dragBar).assertEqual("true");
      console.info('testPanel_400 END');
      done();
    });

    it('testPanel_500', 0, async function (done) {
      console.info('testPanel_500 START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Panel');
      let obj = JSON.parse(strJson);
      console.info("testPanel_500 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fullHeight).assertEqual("0.000000vp");
      console.info('testPanel_500 END');
      done();
    });

    it('testPanel_600', 0, async function (done) {
      console.info('testPanel_600 START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Panel');
      let obj = JSON.parse(strJson);
      console.info("testPanel_600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.halfHeight).assertEqual("500.000000vp");
      console.info('testPanel_600 END');
      done();
    });

    it('testPanel_700', 0, async function (done) {
      console.info('testPanel_700 START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('Panel');
      let obj = JSON.parse(strJson);
      console.info("testPanel_700 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.miniHeight).assertEqual("0.000000vp");
      console.info('testPanel_700 END');
      done();
    });
  })
}