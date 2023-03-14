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
import Utils from './Utils';

export default function pageTransitionJsunit() {
  describe('pageTransitionTest', function () {
    beforeEach(async function (done) {
      console.info("pageTransition beforeEach start");
      let options = {
        uri: 'pages/pageTransition',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get pageTransition state pages:" + JSON.stringify(pages));
        if (!("pageTransition" == pages.name)) {
          console.info("get pageTransition state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push pageTransition page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push pageTransition page error:" + JSON.stringify(err));
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("pageTransition after each called");
    });

    it('pageTransitionTest_0100', 0, async function (done) {
      console.info('pageTransitionTest_0100 START');
      let strJson = getInspectorByKey('Navigator');
      let obj = JSON.parse(strJson);
      console.info("pageTransitionTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$type).assertEqual('Navigator');
      expect(obj.$attrs.target).assertEqual('pages/pageTransition2');
      console.info('pageTransitionTest_0100 END');
      done();
    });

    it('pageTransitionTest_0200', 0, async function (done) {
      console.info('pageTransitionTest_0200 START');
      let strJson = getInspectorByKey('Navigator');
      console.info("pageTransitionTest_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("pageTransitionTest_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.active).assertEqual('false');
      console.info('pageTransitionTest_0200 END');
      done();
    });

    it('pageTransitionTest_0300', 0, async function (done) {
      console.info('pageTransitionTest_0300 START');
      let strJson = getInspectorByKey('Navigator');
      console.info("pageTransitionTest_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("pageTransitionTest_0300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.type).assertEqual('NavigationType.Push');
      console.info('pageTransitionTest_0300 END');
      done();
    });

    it('pageTransitionTest_0400', 0, async function (done) {
      console.info('pageTransitionTest_0400 START');
      try {
        let eventData = {
          data: {
            "scale": 6,
            "opacity": 5
          }
        }
        let indexEvent = {
          eventId: 145,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("pageTransitionTest_0400 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("pageTransitionTest_0400 change component data error: " + err.message);
      }
      let callback = (indexEventOne) => {
        console.info("pageTransitionTest_0400 get state result is: " + JSON.stringify(indexEventOne));
        expect(indexEventOne.data.scale).assertEqual(6);
        expect(indexEventOne.data.opacity).assertEqual(5);
        expect(indexEventOne.data.type).assertEqual(RouteType.Pop);
      }
      let indexEventOne = {
        eventId: 144,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(indexEventOne, callback);
      } catch (err) {
        console.info("pageTransitionTest_0400 on events_emitter err : " + JSON.stringify(err));
      }
      console.info("pageTransitionTest_0400 click result is: " + JSON.stringify(sendEventByKey('Navigator',10,"")));
      await Utils.sleep(2000);
      console.info('pageTransitionTest_0400 END');
      done();
    });
  })
}

