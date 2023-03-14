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

export default function flexWrapJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("text beforeEach start");
      let options = {
        uri: 'pages/flexWrap',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get flexWrap state pages:" + JSON.stringify(pages));
        if (!("flexWrap" == pages.name)) {
          console.info("get flexWrap state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push flexWrap page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push flexWrap page error:" + JSON.stringify(result));
      }
      done()
    });
    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("flexWrap after each called");
    });

    it('flexWrap_0100', 0, async function (done) {
      console.info('flexWrap_0100 START');
      let strJson = getInspectorByKey('WrapReverse');
      console.info("flexWrap_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexWrap_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual("Text");
      expect(obj.$attrs.width).assertEqual("20.00%");
      console.info('flexWrap_0100 END');
      done();
    });

    it('flexWrap_0200', 0, async function (done) {
      console.info('flexWrap_0200 START');
      let strJson = getInspectorByKey('WrapReverse');
      console.info("flexWrap_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexWrap_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual("40.00vp");
      expect(obj.$attrs.backgroundColor).assertEqual("#FFD2B48C");
      console.info('flexWrap_0200 END');
      done();
    });

    it('flexWrap_0300', 0, async function (done) {
      console.info('flexWrap_0300 START');
      try {
        let eventData = {
          data: {
            "width": "40%",
            "height": "70",
            "backgroundColor": 0xF5DEB3,
          }
        }
        let indexEvent = {
          eventId: 100,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("flexWrap_0300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("flexWrap_0300 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('WrapReverse');
      let objNew = JSON.parse(strJsonNew);
      console.info("flexWrap_0300 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.width).assertEqual('40.00%');
      expect(objNew.$attrs.height).assertEqual('70.00vp');
      expect(objNew.$attrs.backgroundColor).assertEqual("#FFF5DEB3");
      console.info('flexWrap_0300 END');
      done();
      });
  })
}