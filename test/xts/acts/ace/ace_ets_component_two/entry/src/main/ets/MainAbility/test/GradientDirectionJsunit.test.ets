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

export default function gridJsunit() {
  describe('GradientDirectionTest', function () {
    beforeEach(async function (done) {
      console.info('beforeEach ===================GradientDirection test beforeEach called===================');
      let options = {
        uri: 'pages/gradientDirection',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info('beforeEach get GradientDirection state pages:' + JSON.stringify(pages));
        if (!('gradientDirection' == pages.name)) {
          console.info('beforeEach get GradientDirection state pages.name:' + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info('beforeEach push GradientDirection page result:' + JSON.stringify(result));
        }
      } catch (err) {
        console.error('beforeEach push GradientDirection page error:' + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info('[ afterEach ] ===================GradientDirection test after each called===================');
    });

    it('GradientDirectionTest_0100', 0, async function (done) {
      console.info('GradientDirectionTest_0100 START');
      let strJson = getInspectorByKey('GradientDirection');
      console.info("GradientDirectionTest_0100 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("GradientDirectionTest_0100 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.linearGradient.direction).assertEqual('GradientDirection.LeftTop');
      console.info('GradientDirectionTest_0100 END');
      done();
    });

    it('GradientDirectionTest_0200', 0, async function (done) {
      console.info('GradientDirectionTest_0200 START');
      try {
        let eventData = {
          data: {
            "ACTION": GradientDirection.LeftBottom
          }
        }
        let indexEvent = {
          eventId: 106,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("GradientDirectionTest_0200 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("GradientDirectionTest_0200 change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('GradientDirection');
      console.info("GradientDirectionTest_0200 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("GradientDirectionTest_0200 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.linearGradient.direction).assertEqual('GradientDirection.LeftBottom');
      console.info('GradientDirectionTest_0200 END');
      done();
    });

    it('GradientDirectionTest_0300', 0, async function (done) {
      console.info('GradientDirectionTest_0300 START');
      try {
        let eventData = {
          data: {
            "ACTION": GradientDirection.RightBottom
          }
        }
        let indexEvent = {
          eventId: 107,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("GradientDirectionTest_0300 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("GradientDirectionTest_0300 change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('GradientDirection');
      console.info("GradientDirectionTest_0300 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("GradientDirectionTest_0300 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.linearGradient.direction).assertEqual('GradientDirection.RightBottom');
      console.info('GradientDirectionTest_0300 END');
      done();
    });

    it('GradientDirectionTest_0400', 0, async function (done) {
      console.info('GradientDirectionTest_0400 START');
      try {
        let eventData = {
          data: {
            "ACTION": GradientDirection.None
          }
        }
        let indexEvent = {
          eventId: 108,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("GradientDirectionTest_0400 start to publish emit");
        events_emitter.emit(indexEvent, eventData);
      } catch (err) {
        console.log("GradientDirectionTest_0400 change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('GradientDirection');
      console.info("GradientDirectionTest_0400 component strJson:" + strJson);
      let obj = JSON.parse(strJson);
      console.info("GradientDirectionTest_0400 component obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.linearGradient.direction).assertEqual('GradientDirection.None');
      console.info('GradientDirectionTest_0400 END');
      done();
    });
  })
}
