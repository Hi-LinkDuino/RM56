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
export default function visibilityJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("visibility beforeEach start");
      let options = {
        uri: 'pages/visibility',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get visibility state pages:" + JSON.stringify(pages));
        if (!("visibility" == pages.name)) {
          console.info("get visibility state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push visibility page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push visibility page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("visibility after each called");
    });

    it('testVisibility01', 0, async function (done) {
      console.info('[testVisibility01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('box');
      let obj = JSON.parse(strJson);
      console.info("[testVisibility01] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.visibility).assertEqual('Visibility.Visible');
      console.info('[testVisibility01] END');
      done();
    });

    it('testVisibility02', 0, async function (done) {
      console.info('[testVisibility02] START');
      try {
        var eventData = {
          data: {
            "visibility": Visibility.Hidden,
          }
        }
        var innerEvent = {
          eventId: 47,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVisibility02] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVisibility02] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('box');
      let obj = JSON.parse(strJson);
      console.info("[testVisibility02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.visibility).assertEqual('Visibility.Hidden');
      console.info('[testVisibility02] END');
      done();
    });

    it('testVisibility03', 0, async function (done) {
      console.info('[testVisibility03] START');
      try {
        var eventData = {
          data: {
            "visibility": Visibility.None,
          }
        }
        var innerEvent = {
          eventId: 48,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testVisibility03] start to publish emit:" + JSON.stringify(eventData.data));
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testVisibility03] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('box');
      let obj = JSON.parse(strJson);
      console.info("[testVisibility03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.visibility).assertEqual('Visibility.None');
      console.info('[testVisibility03] END');
      done();
    });
  })
}
