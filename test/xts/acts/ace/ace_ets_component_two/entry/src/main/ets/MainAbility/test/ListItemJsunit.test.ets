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

export default function listItemJsunit() {
  describe('listItemTest', function () {
    beforeEach(async function (done) {
      console.info("listItem beforeEach start");
      let options = {
        uri: 'pages/listItem',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info("get listItem state pages:" + JSON.stringify(pages));
        if (!("listItem" == pages.name)) {
          console.info("get listItem state pages.name:" + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push listItem page result:" + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push listItem page error:" + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("listItem after each called");
    });

    it('testListItem01', 0, async function (done) {
      console.info('[testListItem01] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ListItem');
      let obj = JSON.parse(strJson);
      console.info("[testListItem01] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.sticky).assertEqual('Sticky.Normal');
      console.info('testListItem01 END');
      done();
    });

    it('testListItem02', 0, async function (done) {
      console.info('[testListItem02] START');
      try {
        var eventData = {
          data: {
            "sticky": Sticky.None
          }
        }
        var innerEvent = {
          eventId: 122,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testListItem02] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testListItem02] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('ListItem');
      let obj = JSON.parse(strJson);
      console.info("[testListItem02] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.sticky).assertEqual('Sticky.None');
      console.info('testListItem02 END');
      done();
    });

    it('testListItem03', 0, async function (done) {
      console.info('[testListItem03] START');
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('List');
      let obj = JSON.parse(strJson);
      console.info("[testListItem03] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.editMode).assertEqual('true');
      console.info('testListItem03 END');
      done();
    });

    it('testListItem04', 0, async function (done) {
      console.info('[testEllipse03] START');
      try {
        var eventData = {
          data: {
            "editMode": false
          }
        }
        var innerEvent = {
          eventId: 123,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[testListItem04] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[testListItem04] change component data error: " + err.message);
      }
      await Utils.sleep(1000);
      let strJson = getInspectorByKey('List');
      let obj = JSON.parse(strJson);
      console.info("[testListItem04] obj is: " + JSON.stringify(obj));
      expect(obj.$attrs.editMode).assertEqual('false');
      console.info('testListItem04 END');
      done();
    });
  })
}
