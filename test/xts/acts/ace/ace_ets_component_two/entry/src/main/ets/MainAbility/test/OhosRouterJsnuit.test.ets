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

export default function OhosRouterJsunit() {
  describe('ohosRouterTest', function () {
    beforeEach(async function (done) {
      let options = {
        uri: 'pages/ohosRouterA',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get ohosRouter state success " + JSON.stringify(pages));
        if (!("ohosRouter" == pages.name)) {
          console.info("get ohosRouter state success " + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(4000);
          console.info("push ohosRouter page success " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push ohosRouter page error " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("ohosRouter after each called");
    });

    it('ohosRouterTest001', 0, async function (done) {
      console.info('[ohosRouterTest001] START');
      let options = {
        uri: 'pages/ohosRouterA',
      }
      try {
        let result = await router.push(options);
        await Utils.sleep(1000);
        console.info("[ohosRouterTest001] push page success " + JSON.stringify(result));
        let pages = router.getState();
        expect(pages.index).assertEqual(3);
        expect(pages.name).assertEqual('ohosRouterA');
        expect(pages.path).assertEqual('pages/');
      } catch (err) {
        console.error("[ohosRouterTest001] push page error: " + err);
      }
      await Utils.sleep(2000);
      done();
    });

    it('ohosRouterTest002', 0, async function (done) {
      console.info('[ohosRouterTest002] START');
      let options = {
        uri: 'pages/ohosRouterA'
      }
      try {
        let result = await router.push(options);
        console.info("[ohosRouterTest002] push page success " + JSON.stringify(result));
      } catch (err) {
        console.error("[ohosRouterTest002] push page error " + JSON.stringify(result));
      }
      await Utils.sleep(1500);
      let callback = (paramsEvent) => {
        console.info("[ohosRouterTest002] get state result is: " + JSON.stringify(paramsEvent));
        expect(paramsEvent.data.params).assertEqual('B Page');
      }
      let paramsEvent = {
        eventId: 142,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(paramsEvent, callback);
      } catch (err) {
        console.info("[ohosRouterTest002] on events_emitter err : " + JSON.stringify(err));
      }
      console.info("[ohosRouterTest002] click push button result is: "
      + JSON.stringify(sendEventByKey('pushButtonA', 10, "")));
      await Utils.sleep(1000);
      router.back();
      await Utils.sleep(1000);
      done();
    });

    it('ohosRouterTest003', 0, async function (done) {
      console.info('[ohosRouterTest003] START');
      await Utils.sleep(1000);
      router.clear();
      await Utils.sleep(2000);
      let size = router.getLength();
      console.info("[ohosRouterTest003] size" + size);
      expect(size).assertEqual("1");
      await Utils.sleep(1000);
      done();
    });

    it('ohosRouterTest004', 0, async function (done) {
      console.info('[ohosRouterTest004] START');
      router.clear();
      await Utils.sleep(1000);
      let options = {
        uri: 'pages/ohosRouterA'
      }
      try {
        let result = await router.push(options);
        console.info("[ohosRouterTest004] push page success " + JSON.stringify(result));
      } catch (err) {
        console.error("[ohosRouterTest004] push page error " + JSON.stringify(result));
      }
      await Utils.sleep(1000);
      let optionsB = {
        uri: 'pages/ohosRouterB'
      }
      try {
        let result = await router.replace(optionsB);
        console.info("[ohosRouterTest004] push page success " + JSON.stringify(result));
      } catch (err) {
        console.error("[ohosRouterTest004] push page error " + JSON.stringify(result));
      }
      await Utils.sleep(1500);
      let size = router.getLength();
      console.info("[ohosRouterTest004] size" + size);
      expect(size).assertEqual("2");
      done();
    });

    it('ohosRouterTest005', 0, async function(done) {
      console.info('[ohosRouterTest005] START');
      let options = {
        uri: 'pages/ohosRouterA'
      }
      try {
        let result = await router.push(options);
        console.info("[ohosRouterTest005] push page success " + JSON.stringify(result));
      } catch (err) {
        console.error("[ohosRouterTest005] push page error " + JSON.stringify(result));
      }
      await Utils.sleep(1000);
      router.clear();
      await Utils.sleep(1500);
      let callback = (paramsEvent) => {
        console.info("[ohosRouterTest005] get state result is: " + JSON.stringify(paramsEvent));
        expect(paramsEvent.data.params).assertEqual('B Page');
      }
      let paramsEvent = {
        eventId: 142,
        priority: events_emitter.EventPriority.LOW
      }
      try {
        events_emitter.on(paramsEvent, callback);
      } catch (err) {
        console.info("[ohosRouterTest005] on events_emitter err : " + JSON.stringify(err));
      }
      console.info("[ohosRouterTest005] click replace button result is: "
      + JSON.stringify(sendEventByKey('ReplaceButtonA', 10, "")));
      done();
    });
  });
}