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

export default function ohosMatrix4Jsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("ohosMatrix4 beforeEach start");
      let options = {
        uri: 'pages/ohosMatrix4',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get ohosMatrix4 state pages:" + JSON.stringify(pages));
        if (!("ohosMatrix4 " == pages.name)) {
          console.info("get ohosMatrix4  state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push ohosMatrix4  page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push ohosMatrix4  page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(3000);
      console.info("OhosMatrix4 each called");
    });

    it('test_OhosMatrix4_001', 0, async function (done) {
      console.info('[test_OhosMatrix4_001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('rotate7');
      let obj = JSON.parse(strJson);
      console.info("[test_OhosMatrix4_001] component strJson:" + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Image');
      expect(obj.$attrs.transform.type)
        .assertEqual('matrix')
      expect(obj.$attrs.transform.matrix)
      .assertNotNull;
      done();
    });

    it('test_OhosMatrix4_002', 0, async function (done) {
      console.info('[test_OhosMatrix4_002] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('rotate8');
      let obj = JSON.parse(strJson);
      console.info("[test_OhosMatrix4_001] component strJson:" + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Image');
      expect(obj.$attrs.transform.type)
        .assertEqual('matrix')
      expect(obj.$attrs.transform.matrix)
        .assertEqual('5.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,-200.000000,0.000000,0.000000,1.000000,')
      done();
    });

    it('test_OhosMatrix4_003', 0, async function (done) {
      console.info('[test_OhosMatrix4_003] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('rotate9');
      let obj = JSON.parse(strJson);
      console.info("[test_OhosMatrix4_003] component strJson:" + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Image');
      expect(obj.$attrs.transform.type)
        .assertEqual('matrix')
      expect(obj.$attrs.transform.matrix)
        .assertEqual('1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,100.000000,200.000000,30.000000,1.000000,')
      done();
    });

    it('test_OhosMatrix4_004', 0, async function (done) {
      console.info('[test_OhosMatrix4_004] START');
      await Utils.sleep(1000);
      let scaleValue = {
        x: 1, y: 1
      }
      try {
        var eventData = {
          data: {
            "scaleValue": JSON.stringify(scaleValue)
          }
        }
        var innerEvent = {
          eventId: 140,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_OhosMatrix4_004] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_OhosMatrix4_004] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('rotate3');
      let objNew = JSON.parse(strJsonNew);
      console.info("[test_OhosMatrix4_004] component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.scale.x).assertEqual('1.000000');
      expect(objNew.$attrs.scale.y).assertEqual('1.000000');
      done();
    });

    it('test_OhosMatrix4_005', 0, async function (done) {
      await Utils.sleep(1000)
      var callback = (eventData) => {
        console.info("[test_OhosMatrix4_005] get event state result is: " + JSON.stringify(eventData));
        expect(eventData.data.matrix1).assertEqual("[100, 10]")
      }
      var innerEvent = {
        eventId: 141,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(innerEvent, callback)
      } catch (err) {
        console.info("[test_OhosMatrix4_005] on events_emitter err : " + JSON.stringify(err));
      }
      console.info('[test_OhosMatrix4_005] sendEventByKey ' + JSON.stringify(sendEventByKey('rotate5', 10, "")));
      await Utils.sleep(1000)
      console.info('[test_OhosMatrix4_005] testSendTouchEvent END');
      done();
    });

    it('test_OhosMatrix4_006', 0, async function (done) {
      console.info('[test_OhosMatrix4_006] START');
      await Utils.sleep(1000);
      let matrix2 = {
        x: 400
      }
      let matrix3 = {
        x: 2
      }
      try {
        var eventData = {
          data: {
            "matrix2": JSON.stringify(matrix2),
            "matrix3": JSON.stringify(matrix3)
          }
        }
        var innerEvent = {
          eventId: 140,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_OhosMatrix4_006] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_OhosMatrix4_006] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJsonNew = getInspectorByKey('rotate6');
      let objNew = JSON.parse(strJsonNew);
      console.info("[test_OhosMatrix4_006] component objNew is: " + JSON.stringify(objNew));
      expect(JSON.stringify(objNew.$attrs.transform)).assertEqual('{}');
      done();
    });
  })
}




