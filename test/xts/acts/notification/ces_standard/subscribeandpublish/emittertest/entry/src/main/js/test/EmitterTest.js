/*
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
import emitter from '@ohos.events.emitter'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('EmitterTest', function () {
    console.info("===========EmitterTest start====================>");

    var eventData = {
        data: {
            "id": 1,
            "content": "message",
        }
    }

    var InnerEventImmediate = {
        eventId: 1,
        priority: emitter.EventPriority.IMMEDIATE
    }

    var InnerEventLow = {
        eventId: 2,
        priority: emitter.EventPriority.LOW
    }

    function callback(data){
        console.info('callback success');
        expect(data.eventId).assertEqual("message");
    }

    function callback01(data){
        console.info('callback success');
        expect(data.data.id).assertEqual(1);
        expect(data.data.content).assertEqual("message");
    }

    /*
     * @tc.number    : EmitterOnTest
     * @tc.name      : verify on : Check subscribe same on common ordered event
     * @tc.desc      : Check the subscriber can receive event "EmitterOnTest" type of the interface (by Promise)
     */
    it('EmitterOnTest', 0, async function (done) {
        console.info("===========EmitterOnTest====================>");
        emitter.on(InnerEventImmediate, callback);
        done();
    })

    /*
     * @tc.number    : EmitterOnceTest
     * @tc.name      : verify once : Check subscribe same once common ordered event
     * @tc.desc      : Check the subscriber can receive event "EmitterOnceTest" type of the interface (by Promise)
     */
    it('EmitterOnceTest', 0, async function (done){
        console.info("===========EmitterOnceTest====================>");
        emitter.once(InnerEventImmediate, callback);
        done();
    })

    /*
     * @tc.number    : EmitterEmitTest
     * @tc.name      : verify emit : Check subscribe same emit common ordered event
     * @tc.desc      : Check the subscriber can receive event "EmitterEmitTest" type of the interface (by Promise)
     */
    it('EmitterEmitTest', 0, async function (done){
        console.info("===========EmitterEmitTest====================>");
        emitter.once(InnerEventImmediate, callback01);
        emitter.emit(InnerEventImmediate, eventData);
        done();
    })

    /*
     * @tc.number    : EmitterLowTest
     * @tc.name      : verify low : Check subscribe same low common ordered event
     * @tc.desc      : Check the subscriber can receive event "EmitterLowTest" type of the interface (by Promise)
     */
    it('EmitterLowTest', 0, async function (done){
        console.info("===========EmitterLowTest====================>");
        emitter.once(InnerEventLow, callback01);
        emitter.emit(InnerEventLow,eventData);
        done();
    })



})
