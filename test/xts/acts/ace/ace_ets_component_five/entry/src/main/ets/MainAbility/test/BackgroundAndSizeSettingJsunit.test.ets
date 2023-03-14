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

export default function backgroundAndSizeSettingJsunit() {
  describe('backgroundAndSizeSettingTest', function () {
    beforeEach(async function (done) {
      console.info("backgroundAndSizeSetting beforeEach start");
      let options = {
        uri: 'pages/backgroundAndSizeSetting',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get backgroundAndSizeSetting state success " + JSON.stringify(pages));
        if (!("backgroundAndSizeSetting" == pages.name)) {
          console.info("get backgroundAndSizeSetting state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push backgroundAndSizeSetting page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push backgroundAndSizeSetting page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("backgroundAndSizeSetting after each called");
    });

    it('testBackgroundAndSizeSetting_0100', 0, async function (done) {
      console.info('testBackgroundAndSizeSetting_0100 START');
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontColor');
      console.info("testBackgroundAndSizeSetting_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBackgroundAndSizeSetting_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.fontColor).assertEqual("#FFFF0000");
      console.info('testBackgroundAndSizeSetting_0100 END');
      done();
    });

    it('testBackgroundAndSizeSetting_0200', 0, async function (done) {
      console.info('testBackgroundAndSizeSetting_0200 START');
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
        console.info("testBackgroundAndSizeSetting_0200 start pages");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("testBackgroundAndSizeSetting_0200 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('backgroundColor');
      console.info("testBackgroundAndSizeSetting_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBackgroundAndSizeSetting_0200 rowDirection: " + obj.$attrs.direction);
      expect(obj.$attrs.backgroundColor).assertEqual("#FFEEEE00");
      done();
    });

    it('testBackgroundAndSizeSetting_0300', 0, async function (done) {
      console.info('testBackgroundAndSizeSetting_0300 START');
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('backgroundColor');
      console.info("testBackgroundAndSizeSetting_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testBackgroundAndSizeSetting_0300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.borderWidth).assertEqual("1.00vp");
      console.info('testBackgroundAndSizeSetting_0300 END');
      done();
    });

    it('testBackgroundAndSizeSetting_0400', 0, async function (done) {
      console.info('[testBackgroundAndSizeSetting_0400] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('centerRow');
      console.info("[testBackgroundAndSizeSetting_0400] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      expect(obj.$attrs.width).assertEqual('80.00vp');
      expect(obj.$attrs.height).assertEqual('80.00vp');
      expect(obj.$attrs.padding).assertEqual('20.00vp');
      expect(obj.$attrs.margin).assertEqual('20.00vp');
      console.info('[testBackgroundAndSizeSetting_0400] END');
      done();
    });

    it('testBackgroundAndSizeSetting_0500', 0, async function (done) {
      console.info('[testBackgroundAndSizeSetting_0500] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('innerRow');
      console.info("[testBackgroundAndSizeSetting_0500] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      expect(obj.$attrs.width).assertEqual('100.00%');
      expect(obj.$attrs.height).assertEqual('100.00%');
      console.info('[testBackgroundAndSizeSetting_0500] END');
      done();
    });

    it('testBackgroundAndSizeSetting_0600', 0, async function (done) {
      console.info('[testBackgroundAndSizeSetting_0600] START');
      try {
        let eventData = {
          data: {
            "width": '200',
            "height": '200',
            "padding": '10',
            "margin": '40'
          }
        }
        let indexEvent = {
          eventId: 34,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBackgroundAndSizeSetting_0600] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testBackgroundAndSizeSetting_0600] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('centerRow');
      console.info("[testBackgroundAndSizeSetting_0600] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      expect(obj.$attrs.width).assertEqual('200.00vp');
      expect(obj.$attrs.height).assertEqual('200.00vp');
      expect(obj.$attrs.padding).assertEqual('10.00vp');
      expect(obj.$attrs.margin).assertEqual('40.00vp');
      console.info('[testBackgroundAndSizeSetting_0600] END');
      done();
    });

    it('testBackgroundAndSizeSetting_0700', 0, async function (done) {
      console.info('[testBackgroundAndSizeSetting_0700] START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "width": '80%',
            "height": '80%'
          }
        }
        let indexEvent = {
          eventId: 35,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBackgroundAndSizeSetting_0700] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testBackgroundAndSizeSetting_0700] change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJson = getInspectorByKey('innerRow');
      console.info("[testBackgroundAndSizeSetting_0700] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      expect(obj.$attrs.width).assertEqual('80.00%');
      expect(obj.$attrs.height).assertEqual('80.00%');
      console.info('[testBackgroundAndSizeSetting_0700] END');
      done();
    });

    it('testBackgroundAndSizeSetting_0800', 0, async function (done) {
      console.info('[testBackgroundAndSizeSetting_0800] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('layoutWeight1');
      console.info("[testBackgroundAndSizeSetting_0800] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      expect(obj.$attrs.layoutWeight).assertEqual(1);
      console.info('[testBackgroundAndSizeSetting_0800] END');
      done();
    });

    it('testBackgroundAndSizeSetting_0900', 0, async function (done) {
      console.info('[testBackgroundAndSizeSetting_0900] START');
      await Utils.sleep(1000);
      try {
        let eventData = {
          data: {
            "weight": '3'
          }
        }
        let indexEvent = {
          eventId: 36,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testBackgroundAndSizeSetting_0900] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testBackgroundAndSizeSetting_0900] change component data error: " + err.message);
      }
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('layoutWeight1');
      console.info("[testBackgroundAndSizeSetting_0900] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      expect(obj.$attrs.layoutWeight).assertEqual(3);
      console.info('[testBackgroundAndSizeSetting_0900] END');
      done();
    });
  })
}
