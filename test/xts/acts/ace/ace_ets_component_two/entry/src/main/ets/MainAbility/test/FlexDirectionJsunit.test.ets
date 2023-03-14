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

export default function flexDirectionJsunit(){
  describe('flexDirectionTest', function () {
    beforeEach(async function (done) {
      console.info("text beforeEach start");
      let options = {
        uri: 'pages/flexDirection',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get flexDirection state pages:" + JSON.stringify(pages));
        if (!("flexDirection" == pages.name)) {
          console.info("get flexDirection state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push flexDirection page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push flexDirection page error:" + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("flexDirection after each called");
    });

    it('flexDirection_0100', 0, async function (done) {
      console.info('flexDirection_0100 START');
      let strJson = getInspectorByKey('Row');
      console.info("flexDirection_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexDirection_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual("Text");
      expect(obj.$attrs.width).assertEqual("20.00%");
      console.info('flexDirection_0100 END');
      done();
    });

    it('flexDirection_0200', 0, async function (done) {
      console.info('flexDirection_0200 START');
      let strJson = getInspectorByKey('Row');
      console.info("flexDirection_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexDirection_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual("40.00vp");
      expect(obj.$attrs.backgroundColor).assertEqual("#FFD2B48C");
      console.info('flexDirection_0200 END');
      done();
    });

    it('flexDirection_0300', 0, async function (done) {
      console.info('flexDirection_0300 START');
      let strJson = getInspectorByKey('ColumnReverse');
      console.info("flexDirection_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexDirection_0300 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual("Text");
      expect(obj.$attrs.width).assertEqual("20.00%");
      console.info('flexDirection_0300 END');
      done();
    });

    it('flexDirection_0400', 0, async function (done) {
      console.info('flexDirection_0400 START');
      let strJson = getInspectorByKey('ColumnReverse');
      console.info("flexDirection_0400 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexDirection_0400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual("40.00vp");
      expect(obj.$attrs.backgroundColor).assertEqual("#FFD2B48C");
      console.info('flexDirection_0400 END');
      done();
    });

    it('flexDirection_0500', 0, async function (done) {
      console.info('flexDirection_0500 START');
      let strJson = getInspectorByKey('Column');
      console.info("flexDirection_0500 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexDirection_0500 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual("Text");
      expect(obj.$attrs.width).assertEqual("20.00%");
      console.info('flexDirection_0500 END');
      done();
    });

    it('flexDirection_0600', 0, async function (done) {
      console.info('flexDirection_0600 START');
      let strJson = getInspectorByKey('Column');
      console.info("flexDirection_0600 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("flexDirection_0600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.height).assertEqual("40.00vp");
      expect(obj.$attrs.backgroundColor).assertEqual("#FFD2B48C");
      console.info('flexDirection_0600 END');
      done();
    });

    it('flexDirection_0700', 0, async function (done) {
      console.info('flexDirection_0700 START');
      try {
        let eventData = {
          data: {
            "width": "40%",
            "height": "70",
            "backgroundColor": 0xF5DEB3,
          }
        }
        let indexEvent = {
          eventId: 97,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("flexDirection_0700 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("flexDirection_0700 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('Row');
      let objNew = JSON.parse(strJsonNew);
      console.info("flexDirection_0700 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.width).assertEqual('40.00%');
      expect(objNew.$attrs.height).assertEqual('70.00vp');
      expect(objNew.$attrs.backgroundColor).assertEqual("#FFF5DEB3");
      console.info('flexDirection_0700 END');
      done();
      });

    it('flexDirection_0800', 0, async function (done) {
      console.info('flexDirection_0800 START');
      try {
        let eventData = {
          data: {
            "width": "45%",
            "height": "60",
            "backgroundColor": 0xF5DEB3,
          }
        }
        let indexEvent = {
          eventId: 98,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("flexDirection_0800 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("flexDirection_0800 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('ColumnReverse');
      let objNew = JSON.parse(strJsonNew);
      console.info("flexDirection_0800 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.width).assertEqual('45.00%');
      expect(objNew.$attrs.height).assertEqual('60.00vp');
      expect(objNew.$attrs.backgroundColor).assertEqual("#FFF5DEB3");
      console.info('flexDirection_0800 END');
      done();
    });

    it('flexDirection_0900', 0, async function (done) {
      console.info('flexDirection_0900 START');
      try {
        let eventData = {
          data: {
            "width": "60%",
            "height": "55",
            "backgroundColor": "#FF000990",
          }
        }
        let indexEvent = {
          eventId: 99,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("flexDirection_0900 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("flexDirection_0900 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('Column');
      let objNew = JSON.parse(strJsonNew);
      console.info("flexDirection_0900 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.width).assertEqual('60.00%');
      expect(objNew.$attrs.height).assertEqual('55.00vp');
      expect(objNew.$attrs.backgroundColor).assertEqual("#FF000990");
      console.info('flexDirection_0900 END');
      done();
    });
  })
}