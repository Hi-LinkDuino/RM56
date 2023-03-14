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

export default function spanJsunit() {
  describe('spanTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/span',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get span state success " + JSON.stringify(pages));
        if (!("span" == pages.name)) {
          console.info("get span success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push span page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push span page error " + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(2000);
      console.info("span after each called");
    });

    it('testSpan001', 0, async function (done) {
      console.info('[testSpan001] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('decoration');
      console.info("[testSpan001] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      let decoration = JSON.parse(obj.$attrs.decoration);
      expect(decoration.type).assertEqual('TextDecorationType.None');
      expect(decoration.color).assertEqual('#FFFF0000');
      done();
    });

    it('testSpan002', 0, async function (done) {
      console.info('[testSpan002] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('decoration');
      console.info("[testSpan002] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan002] textCase:" + obj.$attrs.textCase);
      expect(obj.$attrs.textCase).assertEqual('TextCase.Normal');
      done();
    });

    it('testSpan003', 0, async function (done) {
      console.info('[testSpan003] START');
      try {
        let eventData = {
          data: {
            "textCaseValue": TextCase.UpperCase
          }
        }
        let indexEvent = {
          eventId: 200,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testSpan003] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testSpan003] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('decoration');
      console.info("[testSpan003] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan003] textCase:" + obj.$attrs.textCase);
      expect(obj.$attrs.textCase).assertEqual('TextCase.UpperCase');
      done();
    });

    it('testSpan004', 0, async function (done) {
      console.info('[testSpan004] START');
      await Utils.sleep(2000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testSpan004] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan004] fontColor:" + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#FFCCCCCC');
      done();
    });

    it('testSpan005', 0, async function (done) {
      console.info('[testSpan005] START');
      try {
        let eventData = {
          data: {
            "fontColorValue": '#FF000990'
          }
        }
        let indexEvent = {
          eventId: 201,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testSpan005] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testSpan005] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testSpan005] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan005] fontColor:" + obj.$attrs.fontColor);
      expect(obj.$attrs.fontColor).assertEqual('#FF000990');
      done();
    });

    it('testSpan006', 0, async function (done) {
      console.info('[testSpan006] START');
      try {
        let eventData = {
          data: {
            "textCaseValue": a
          }
        }
        let indexEvent = {
          eventId: 1029,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testSpan006] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testSpan006] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('decoration');
      console.info("[testSpan006] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan006] textCase:" + obj);
      expect(obj.$attrs.textCase).assertEqual('TextCase.UpperCase');
      done();
    });

    it('testSpan007', 0, async function (done) {
      console.info('[testSpan007] START');
      try {
        let eventData = {
          data: {
            "textCaseValue": -1
          }
        }
        let indexEvent = {
          eventId: 1030,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testSpan007] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testSpan007] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('decoration');
      console.info("[testSpan007] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan007] textCase:" + obj);
      expect(obj.$attrs.textCase).assertEqual('TextCase.Normal');
      done();
    });

    it('testSpan008', 0, async function (done) {
      console.info('[testSpan008] START');
      try {
        let eventData = {
          data: {
            "fontColorValue": 'abc'
          }
        }
        let indexEvent = {
          eventId: 1031,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testSpan008] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testSpan008] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testSpan008] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan008] fontColor:" + obj);
      expect(obj.$attrs.fontColor).assertEqual('#FF000000');
      done();
    });

    it('testSpan009', 0, async function (done) {
      console.info('[testSpan009] START');
      try {
        let eventData = {
          data: {
            "fontColorValue": '-1'
          }
        }
        let indexEvent = {
          eventId: 1032,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testSpan009] start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("[testSpan009] change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJson = getInspectorByKey('fontColor');
      console.info("[testSpan009] component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("[testSpan009] fontColor:" + obj);
      expect(obj.$attrs.fontColor).assertEqual('#FFFFFFFF');
      done();
    });
  })
}