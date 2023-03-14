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
  describe('gridTest', function () {
    beforeEach(async function (done) {
      console.info('beforeEach ===================grid test beforeEach called===================');
      let options = {
        uri: 'pages/gridPage',
      }
      try {
        router.clear();
        let pages = router.getState();
        console.info('beforeEach get prompt state pages:' + JSON.stringify(pages));
        if (!('gridPage' == pages.name)) {
          console.info('beforeEach get prompt state pages.name:' + JSON.stringify(pages.name));
          let result = await router.push(options);
          await Utils.sleep(2000);
          console.info('beforeEach push prompt page result:' + JSON.stringify(result));
        }
      } catch (err) {
        console.error('beforeEach push prompt page error:' + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info('[ afterEach ] ===================grid test after each called===================');
    });

    it('gridTest01', 0, async function (done) {
      console.info('gridTest01==================== start');
      await Utils.sleep(1000)
      let strJson = getInspectorByKey('grid');
      let obj = JSON.parse(strJson);
      console.info('gridTest01 all attribute before change is: ' + JSON.stringify(obj.$attrs.columnsGap));
      expect(obj.$attrs.columnsGap).assertEqual('2.00px')
      try {
        var eventData = {
          data: {
            "columnsGap": '3px'
          }
        }
        var innerEvent = {
          eventId: 110,
          priority: events_emitter.EventPriority.LOW
        }
        console.info('gridTest01 start to publish emit')
        events_emitter.emit(innerEvent, eventData)
      } catch (err) {
        console.log('gridTest01 change component data error: ' + err.message)
      }
      await Utils.sleep(1000)
      let strJsonAfterChange = getInspectorByKey('grid');
      let objAfterChange = JSON.parse(strJsonAfterChange);
      console.info('gridTest01 columnsGap attribute after change is: ' + JSON.stringify(objAfterChange.$attrs.columnsGap));
      expect(objAfterChange.$attrs.columnsGap).assertEqual('3.00px')
      console.info('[gridTest01] ==================== end');
      done();
    });

    it('gridTest02', 0, async function (done) {
      console.info('[ gridTest02 ] ==================== start');
      await Utils.sleep(1000)
      let strJson = getInspectorByKey('grid');
      let obj = JSON.parse(strJson);
      console.info('[ gridTest02 ] attribute rowsGap before change is: ' + JSON.stringify(obj.$attrs.rowsGap));
      expect(obj.$attrs.rowsGap).assertEqual('2.00px')
      try {
        var eventData = {
          data: {
            "rowsGap": '3px'
          }
        }
        var innerEvent = {
          eventId: 111,
          priority: events_emitter.EventPriority.LOW
        }
        console.info('[ gridTest02 ] start to publish emit')
        events_emitter.emit(innerEvent, eventData)
      } catch (err) {
        console.log('[ gridTest02 ] change component data error: ' + err.message)
      }
      await Utils.sleep(1000)
      let strJsonAfterChange = getInspectorByKey('grid');
      let objAfterChange = JSON.parse(strJsonAfterChange);
      console.info('[ gridTest02 ] attribute rowsGap after change is: ' + JSON.stringify(objAfterChange.$attrs.rowsGap));
      expect(objAfterChange.$attrs.rowsGap).assertEqual('3.00px')
      console.info('[ gridTest02 ] ==================== end');
      done();
    });

    it('gridTest03', 0, async function (done) {
      console.info('[ gridTest03 ] ==================== start');
      await Utils.sleep(1000)
      let strJson = getInspectorByKey('grid');
      let obj = JSON.parse(strJson);
      console.info('[ gridTest03 ] attribute columnsTemplate before change is: ' + JSON.stringify(obj.$attrs.columnsTemplate));
      expect(obj.$attrs.columnsTemplate).assertEqual('1fr 1fr 1fr 1fr 1fr')
      try {
        var eventData = {
          data: {
            "columnsTemplate": '1fr 1fr 1fr 1fr'
          }
        }
        var innerEvent = {
          eventId: 112,
          priority: events_emitter.EventPriority.LOW
        }
        console.info('[ gridTest03 ] start to publish emit')
        events_emitter.emit(innerEvent, eventData)
      } catch (err) {
        console.log('[ gridTest03 ] change component data error: ' + err.message)
      }
      await Utils.sleep(1000)
      let strJsonAfterChange = getInspectorByKey('grid');
      let objAfterChange = JSON.parse(strJsonAfterChange);
      console.info('[ gridTest03 ] attribute columnsTemplate after change is: ' + JSON.stringify(objAfterChange.$attrs.columnsTemplate));
      expect(objAfterChange.$attrs.columnsTemplate).assertEqual('1fr 1fr 1fr 1fr')
      console.info('[ gridTest03 ] ==================== end');
      done();
    });

    it('gridTest04', 0, async function (done) {
      console.info('[ gridTest04 ] ==================== start');
      await Utils.sleep(1000)
      let strJson = getInspectorByKey('grid');
      let obj = JSON.parse(strJson);
      console.info('[ gridTest04 ] attribute rowsTemplate before change is: ' + JSON.stringify(obj.$attrs.rowsTemplate));
      expect(obj.$attrs.rowsTemplate).assertEqual('1fr 1fr 1fr 1fr 1fr')
      try {
        var eventData = {
          data: {
            "rowsTemplate": '1fr 1fr 1fr 1fr'
          }
        }
        var innerEvent = {
          eventId: 113,
          priority: events_emitter.EventPriority.LOW
        }
        console.info('[ gridTest04 ] start to publish emit')
        events_emitter.emit(innerEvent, eventData)
      } catch (err) {
        console.log('[ gridTest04 ] change component data error: ' + err.message)
      }
      await Utils.sleep(1000)
      let strJsonAfterChange = getInspectorByKey('grid');
      let objAfterChange = JSON.parse(strJsonAfterChange);
      console.info('[ gridTest04 ] attribute rowsTemplate after change is: ' + JSON.stringify(objAfterChange.$attrs.rowsTemplate));
      expect(objAfterChange.$attrs.rowsTemplate).assertEqual('1fr 1fr 1fr 1fr')
      console.info('[ gridTest04 ] ==================== end');
      done();
    });
  })
}
