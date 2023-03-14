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

export default function qrCodeJsunit() {
  describe('qrCodeTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/QrCode',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get QrCode state success " + JSON.stringify(pages));
        if (!("QrCode" == pages.name)) {
          console.info("get QrCode pages success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push QrCode page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push QrCode page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("QrCode after each called");
    });

    it('test_qrCode_001', 0, async function (done) {
      console.info('[test_qrCode_001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('QrCodeColor');
      let obj = JSON.parse(strJson);
      console.info("[test_qrCode_001] component obj is: " + JSON.stringify(obj));
      await Utils.sleep(1000);
      expect(obj.$attrs.backgroundColor).assertEqual('#FF008000');
      expect(obj.$attrs.color).assertEqual('#FF0000FF');
      done();
    });

    it('test_qrCode_002', 0, async function (done) {
      console.info('[test_qrCode_002] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "color": "#FFB6C1"
          }
        }
        var innerEvent = {
          eventId: 170,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_qrCode_002] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_qrCode_002] change component color error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('QrCodeColor');
      var obj = JSON.parse(strJson);
      console.info("[test_qrCode_002] component obj is: " + JSON.stringify(obj.$attrs.color));
      expect(obj.$attrs.color).assertEqual('#FFFFB6C1');
      done();
    });

    it('test_qrCode_003', 0, async function (done) {
      console.info('[test_qrCode_003] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "backgroundColor": "#FFB6C1"
          }
        }
        var innerEvent = {
          eventId: 171,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_qrCode_003] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_qrCode_003] change component color error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('QrCodeBackgroundColor');
      var obj = JSON.parse(strJson);
      console.info("[test_qrCode_003] component obj is: " + JSON.stringify(obj.$attrs.backgroundColor));
      expect(obj.$attrs.backgroundColor).assertEqual('#FFFFB6C1');
      done();
    });

    it('test_qrCode_004', 0, async function (done) {
      console.info('[test_qrCode_004] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "color": 'a'
          }
        }
        var innerEvent = {
          eventId: 1023,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_qrCode_004] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_qrCode_004] change component color error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('QrCodeColor');
      var obj = JSON.parse(strJson);
      console.info("[test_qrCode_004] component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.color).assertEqual('#FF000000');
      done();
    });

    it('test_qrCode_005', 0, async function (done) {
      console.info('[test_qrCode_005] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "color": 332
          }
        }
        var innerEvent = {
          eventId: 1024,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_qrCode_005] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_qrCode_005] change component color error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('QrCodeColor');
      var obj = JSON.parse(strJson);
      console.info("[test_qrCode_005] component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.color).assertEqual('#0000014C');
      done();
    });

    it('test_qrCode_006', 0, async function (done) {
      console.info('[test_qrCode_006] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "backgroundColor": 'b'
          }
        }
        var innerEvent = {
          eventId: 1025,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_qrCode_006] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_qrCode_006] change component color error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('QrCodeColor');
      var obj = JSON.parse(strJson);
      console.info("[test_qrCode_006] component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.backgroundColor).assertEqual('#FF000000');
      done();
    });

    it('test_qrCode_007', 0, async function (done) {
      console.info('[test_qrCode_007] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "backgroundColor": 333
          }
        }
        var innerEvent = {
          eventId: 1026,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[test_qrCode_007] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[test_qrCode_007] change component color error: " + err.message);
      }
      await Utils.sleep(2000);
      var strJson = getInspectorByKey('QrCodeColor');
      var obj = JSON.parse(strJson);
      console.info("[test_qrCode_007] component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.backgroundColor).assertEqual('#0000014D');
      done();
    });
  })
}
