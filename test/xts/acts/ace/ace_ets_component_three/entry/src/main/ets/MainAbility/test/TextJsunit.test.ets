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

export default function textJsunit() {
  describe('testText', function () {
    beforeEach(async function (done) {
      console.info("text beforeEach start");
      let options = {
        uri: 'pages/text',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get text state pages:" + JSON.stringify(pages));
        if (!("text" == pages.name)) {
          console.info("get text state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push text page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push text page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("text after each called");
    });

    it('testText_0100', 0, async function (done) {
      console.info('testText_0100 START');
      let strJson = getInspectorByKey('text');
      console.info("testText_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testText_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Text');
      expect(obj.$attrs.fontSize).assertEqual('9.00fp');
      console.info('testText_0100 END');
      done();
    });

    it('testText_0200', 0, async function (done) {
      console.info('testText_0200 START');
      let strJson = getInspectorByKey('image');
      console.info("testText_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testText_0200 component obj is: " + JSON.stringify(obj));
      var res = obj.$attrs.src.indexOf('rawfile/test.png');
      console.info("testText_0200 result is: " + res);
      var sres = obj.$attrs.src.slice(res, res + 16);
      console.info("testText_0200 slice result is: " + sres);
      expect(obj.$type).assertEqual('Image');
      expect(obj.$attrs.src.slice(res, res + 16)).assertEqual('rawfile/test.png');
      console.info('testText_0200 END');
      done();
    });

    it('testText_0300', 0, async function (done) {
      console.info('testText_0300 START');
      try {
        let eventData = {
          data: {
            "fontSize": "10",
          }
        }
        let indexEvent = {
          eventId: 226,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_0300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_0300 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_0300 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.fontSize).assertEqual('10.00fp');
      console.info('testText_0300 END');
      done();
    });

    it('testText_0400', 0, async function (done) {
      console.info('testText_0400 START');
      let strJson = getInspectorByKey('text');
      console.info("testText_0400 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testText_0400 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Text');
      expect(obj.$attrs.fontColor).assertEqual('#FF000000');
      console.info('testText_0400 END');
      done();
    });

    it('testText_0500', 0, async function (done) {
      console.info('testText_0500 START');
      try {
        let eventData = {
          data: {
            "fontColor": "#330A59F7",
          }
        }
        let indexEvent = {
          eventId: 227,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_0500 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_0500 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_0500 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.fontColor).assertEqual('#330A59F7');
      console.info('testText_0500 END');
      done();
    });

    it('testText_0600', 0, async function (done) {
      console.info('testText_0600 START');
      let strJson = getInspectorByKey('text');
      console.info("testText_0600 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testText_0600 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.textAlign).assertEqual('TextAlign.Start');
      console.info('testText_0600 END');
      done();
    });

    it('testText_0700', 0, async function (done) {
      console.info('testText_0700 START');
      try {
        let eventData = {
          data: {
            "textAlign": TextAlign.Center,
          }
        }
        let indexEvent = {
          eventId: 228,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_0700 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_0700 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_0700 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.textAlign).assertEqual('TextAlign.Center');
      console.info('testText_0700 END');
      done();
    });

    it('testText_0800', 0, async function (done) {
      console.info('testText_0800 START');
      let strJson = getInspectorByKey('text');
      console.info("testText_0800 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testText_0800 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.textOverflow).assertEqual('TextOverflow.Clip');
      console.info('testText_0800 END');
      done();
    });

    it('testText_0900', 0, async function (done) {
      console.info('testText_0900 START');
      try {
        let eventData = {
          data: {
            "textOverflow": TextOverflow.None,
          }
        }
        let indexEvent = {
          eventId: 229,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_0900 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_0900 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_0900 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.textOverflow).assertEqual('TextOverflow.None');
      console.info('testText_0900 END');
      done();
    });

    it('testText_1000', 0, async function (done) {
      console.info('testText_1000 START');
      let strJson = getInspectorByKey('text');
      console.info("testText_1000 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("testText_1000 component obj is: " + JSON.stringify(obj.$attrs.decoration));
      expect(obj.$attrs.decoration).assertEqual("{\"type\":\"TextDecorationType.None\",\"color\":\"#FFFF0000\"}");
      console.info('testText_1000 END');
      done();
    });

    it('testText_1100', 0, async function (done) {
      console.info('testText_1100 START');
      try {
        let eventData = {
          data: {
            "fontColor": "-a",
          }
        }
        let indexEvent = {
          eventId: 230,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_1100 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_1100 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_1100 component objNew is: " + JSON.stringify(objNew.$attrs.fontColor));
      expect(objNew.$attrs.fontColor).assertEqual('#FF000000');
      console.info('testText_1100 END');
      done();
    });

    it('testText_1200', 0, async function (done) {
      console.info('testText_1200 START');
      try {
        let eventData = {
          data: {
            "fontColor": "12321321",
          }
        }
        let indexEvent = {
          eventId: 231,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_1200 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_1200 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_1200 component objNew is: " + JSON.stringify(objNew.$attrs.fontColor));
      expect(objNew.$attrs.fontColor).assertEqual('#00BC0229');
      console.info('testText_1200 END');
      done();
    });

    it('testText_1300', 0, async function (done) {
      console.info('testText_1300 START');
      try {
        let eventData = {
          data: {
            "fontSize": "-a",
          }
        }
        let indexEvent = {
          eventId: 232,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_1300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_1300 change component data error: " + err.message);
      }
      await Utils.sleep(3000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_1300 component objNew is: " + JSON.stringify(objNew.$attrs.fontSize));
      expect(objNew.$attrs.fontSize).assertEqual('nanfp');
      console.info('testText_1300 END');
      done();
    });

    it('testText_1400', 0, async function (done) {
      console.info('testText_1400 START');
      try {
        let eventData = {
          data: {
            "fontSize": "-20",
          }
        }
        let indexEvent = {
          eventId: 233,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("testText_1400 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("testText_1400 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('text');
      let objNew = JSON.parse(strJsonNew);
      console.info("testText_1400 component objNew is: " + JSON.stringify(objNew.$attrs.fontSize));
      expect(objNew.$attrs.fontSize).assertEqual('-20.00fp');
      console.info('testText_1400 END');
      done();
    });
  })
}