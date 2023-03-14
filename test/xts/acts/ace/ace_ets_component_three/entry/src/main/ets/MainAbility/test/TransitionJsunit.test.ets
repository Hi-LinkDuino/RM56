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

export default function transitionJsunit() {
  describe('transitionTest', function () {
    beforeEach(async function (done) {
      console.info("text beforeEach start");
      let options = {
        uri: 'pages/transition',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get transition state pages:" + JSON.stringify(pages));
        if (!("transition" == pages.name)) {
          console.info("get transition state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push transition page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push transition page error:" + JSON.stringify(result));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("transition after each called");
    });

    it('transitionTest_0100', 0, async function (done) {
      console.info('transitionTest_0100 START');
      let strJson = getInspectorByKey('button');
      console.info("transitionTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("transitionTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Button');
      expect(obj.$attrs.opacity).assertEqual(1);
      console.info('transitionTest_0100 END');
      done();
    });

    it('transitionTest_0200', 0, async function (done) {
      console.info('transitionTest_0200 START');
      let indexEvent = {
        eventId: 233,
        priority: events_emitter.EventPriority.LOW
      }
      await Utils.sleep(1000);
      let callback = (indexEvent) => {
        console.info("transitionTest_0200 get state result is: " + JSON.stringify(indexEvent));
        expect(indexEvent.data.btn1).assertEqual(false);
      }
      try {
        events_emitter.on(indexEvent, callback);
      } catch (err) {
        console.info("transitionTest_0200 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("transitionTest_0200 click result is: " + JSON.stringify(sendEventByKey('button', 10, "")));
      await Utils.sleep(1000);
      console.info('transitionTest_0200 END');
      done();
    });

    it('transitionTest_0300', 0, async function (done) {
      console.info('transitionTest_0300 START');
      try {
        let eventData = {
          data: {
            "opacity": 2,
          }
        }
        let indexEvent = {
          eventId: 234,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("transitionTest_0300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("transitionTest_0300 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('button');
      let objNew = JSON.parse(strJsonNew);
      console.info("transitionTest_0300 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.opacity).assertEqual(2);
      console.info('transitionTest_0300 END');
      done();
    });

    it('transitionTest_0400', 0, async function (done) {
      console.info('transitionTest_0400 START');
      try {
        let eventData = {
          data: {
            "opacity": 'a',
          }
        }
        let indexEvent = {
          eventId: 234,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("transitionTest_0400 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("transitionTest_0400 change component data error: " + err.message);
      }
      await Utils.sleep(4000);
      let strJsonNew = getInspectorByKey('button');
      let objNew = JSON.parse(strJsonNew);
      console.info("transitionTest_040 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.opacity).assertEqual(null);
      console.info('transitionTest_0400 END');
      done();
    });
  })
}