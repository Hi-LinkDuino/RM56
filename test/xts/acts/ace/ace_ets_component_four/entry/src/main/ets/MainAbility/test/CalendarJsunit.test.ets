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

export default function calendarJsunit() {
  describe('appInfoTest', function () {
    beforeEach(async function (done) {
      console.info("calendar beforeEach start");
      let options = {
        uri: 'pages/calendar',
      }
      let result;
      try {
        router.clear();
        let pages = router.getState();
        console.info("get calendar state pages: " + JSON.stringify(pages));
        if (!("calendar" == pages.name)) {
          console.info("get calendar state pages.name: " + JSON.stringify(pages.name));
          result = await router.push(options);
          await Utils.sleep(2000);
          console.info("push calendar page result: " + JSON.stringify(result));
        }
      } catch (err) {
        console.error("push calendar page error: " + err);
      }
      done()
    });

    afterEach(async function () {
      await Utils.sleep(1000);
      console.info("calendar after each called");
    })

    it('ACE_calendar_CurrentDayStyle_colSpace', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_colSpace] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "colSpace": 10,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_colSpace] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_colSpace] change component data error: " + err.message);
      }
      done();
    });

    it('ACE_calendar_CurrentDayStyle_dailyFiveRowSpace', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_dailyFiveRowSpace] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "dailyFiveRowSpace": 11,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_dailyFiveRowSpace] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_dailyFiveRowSpace] change component data error: " + err.message);
      }
      done();
    });


    it('ACE_calendar_CurrentDayStyle_dailySixRowSpace', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_dailySixRowSpace] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "dailySixRowSpace": 12,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_dailySixRowSpace] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_dailySixRowSpace] change component data error: " + err.message);
      }
      done();
    });


    it('ACE_calendar_CurrentDayStyle_lunarHeight', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_lunarHeight] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "lunarHeight": 13,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_lunarHeight] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_lunarHeight] change component data error: " + err.message);
      }
      done();
    });


    it('ACE_calendar_CurrentDayStyle_underscoreWidth', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_underscoreWidth] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "underscoreWidth": 14,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_underscoreWidth] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_underscoreWidth] change component data error: " + err.message);
      }
      done();
    });


    it('ACE_calendar_CurrentDayStyle_underscoreLength', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_underscoreLength] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "underscoreLength": 15,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_underscoreLength] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_underscoreLength] change component data error: " + err.message);
      }
      done();
    });


    it('ACE_calendar_CurrentDayStyle_scheduleMarkerRadius', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_scheduleMarkerRadius] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "scheduleMarkerRadius": 16,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_scheduleMarkerRadius] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_scheduleMarkerRadius] change component data error: " + err.message);
      }
      done();
    });


    it('ACE_calendar_CurrentDayStyle_boundaryRowOffset', 0, async function (done) {
      console.info('[ACE_calendar_CurrentDayStyle_boundaryRowOffset] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "boundaryRowOffset": 17,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CurrentDayStyle_boundaryRowOffset] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CurrentDayStyle_boundaryRowOffset] change component data error: " + err.message);
      }
      done();
    });

    it('ACE_calendar_WeekStyle_weekAndDayRowSpace', 0, async function (done) {
      console.info('[ACE_calendar_WeekStyle_weekAndDayRowSpace] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "weekAndDayRowSpace": 18,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_WeekStyle_weekAndDayRowSpace] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_WeekStyle_weekAndDayRowSpace] change component data error: " + err.message);
      }
      done();
    });


    it('ACE_calendar_CalendarAttribute_direction', 0, async function (done) {
      console.info('[ACE_calendar_CalendarAttribute_direction] START');
      await Utils.sleep(1000);
      try {
        var eventData = {
          data: {
            "direction": Axis.Vertical,
          }
        }
        var innerEvent = {
          eventId: 74,
          priority: events_emitter.EventPriority.LOW
        }
        console.info("[ACE_calendar_CalendarAttribute_direction] start to publish emit");
        events_emitter.emit(innerEvent, eventData);
      } catch (err) {
        console.log("[ACE_calendar_CalendarAttribute_direction] change component data error: " + err.message);
      }
      done();
    });
  })
}
