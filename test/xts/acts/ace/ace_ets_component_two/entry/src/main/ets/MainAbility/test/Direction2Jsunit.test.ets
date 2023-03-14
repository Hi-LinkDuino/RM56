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

export default function direction2Jsunit(){
  describe('direction2Test', function (){
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/direction2',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get direction state success " + JSON.stringify(pages));
        if (!("direction2" == pages.name)) {
          console.info("get direction state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(1000);
          console.info("push direction page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push direction page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("direction after each called");
    });

    it('direction2Test_0100', 0, async function (done) {
      console.info('direction2Test_0100 START');
      let strJson = getInspectorByKey('direction');
      console.info("direction2Test_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("direction2Test_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.direction).assertEqual("Direction.Ltr");
      done();
    });

    it('direction2Test_0200', 0, async function (done) {
      console.info('direction2Test_0200 START');
      try {
        let eventData = {
          data: {
            "direction": Direction.Rtl,
          }
        }
        let indexEvent = {
          eventId: 82,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("direction2Test_0200 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("direction2Test_0200 change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJsonNew = getInspectorByKey('direction');
      let objNew = JSON.parse(strJsonNew);
      console.info("direction2Test_0200 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.direction).assertEqual('Direction.Rtl');
      console.info('direction2Test_0200 END');
      done();
    });

    it('direction2Test_0300', 0, async function (done) {
      console.info('direction2Test_0300 START');
      try {
        let eventData = {
          data: {
            "direction":  Direction.Auto,
          }
        }
        let indexEvent = {
          eventId: 83,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("direction2Test_0300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("direction2Test_0300 change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJsonNew = getInspectorByKey('direction');
      let objNew = JSON.parse(strJsonNew);
      console.info("direction2Test_0300 component objNew is: " + JSON.stringify(objNew));
      expect(objNew.$attrs.direction).assertEqual('Direction.Auto');
      console.info('direction2Test_0300 END');
      done();
    });
  })
}