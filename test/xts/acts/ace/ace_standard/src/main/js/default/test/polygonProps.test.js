/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@system.router';
import { describe, beforeAll, afterAll, it, expect } from 'deccjsunit/index';


describe('polygonPropsJsTest', function () {

    async function sleep(time) {
        return new Promise((resolve, reject) => {
            setTimeout(() => {
                resolve()
            }, time)
        }).then(() => {
            console.info(`sleep ${time} over...`)
        })
    }

    async function backToIndex() {
        let backToIndexPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.back({
                    uri: 'pages/index/index'
                });
                resolve();
            }, 500);
        });
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear();
                resolve();
            }, 500);
        });
        await backToIndexPromise.then(() => {
            return clearPromise;
        });
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[polygonPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/polygon/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push polygonProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push polygonProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[polygonPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonFillProp
     * @tc.desc      ACE
     */
    it('testPolygonFillProp', 0, async function (done) {
        console.info('testPolygonFillProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonFillOpacityPropOne
     * @tc.desc      ACE
     */
    it('testPolygonFillOpacityPropOne', 0, async function (done) {
        console.info('testPolygonFillOpacityPropOne START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropOne);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillOpacityPropOne')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillOpacity).assertEqual('0.5')
        console.info("[polygonProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonFillOpacityPropTwo
     * @tc.desc      ACE
     */
    it('testPolygonFillOpacityPropTwo', 0, async function (done) {
        console.info('testPolygonFillOpacityPropTwo START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropTwo);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillOpacityPropTwo')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillOpacity).assertEqual('1')
        console.info("[polygonProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonFillOpacityPropThree
     * @tc.desc      ACE
     */
    it('testPolygonFillOpacityPropThree', 0, async function (done) {
        console.info('testPolygonFillOpacityPropThree START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropThree);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillOpacityPropThree')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillOpacity).assertEqual('1.5')
        console.info("[polygonProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonFillRulePropOne
     * @tc.desc      ACE
     */
    it('testPolygonFillRulePropOne', 0, async function (done) {
        console.info('testPolygonFillRulePropOne START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillRulePropOne);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillRulePropOne')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillRule).assertEqual('evenodd')
        console.info("[polygonProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonFillRulePropTwo
     * @tc.desc      ACE
     */
    it('testPolygonFillRulePropTwo', 0, async function (done) {
        console.info('testPolygonFillRulePropTwo START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillRulePropTwo);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillRulePropTwo')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillRule).assertEqual('nonzero')
        console.info("[polygonProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonOpacityPropOne
     * @tc.desc      ACE
     */
    it('testPolygonOpacityPropOne', 0, async function (done) {
        console.info('testPolygonOpacityPropOne START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropOne);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('opacityPropOne')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.opacity).assertEqual('0.5')
        console.info("[polygonProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonOpacityPropTwo
     * @tc.desc      ACE
     */
    it('testPolygonOpacityPropTwo', 0, async function (done) {
        console.info('testPolygonOpacityPropTwo START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropTwo);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('opacityPropTwo')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.opacity).assertEqual('1')
        console.info("[polygonProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonOpacityPropThree
     * @tc.desc      ACE
     */
    it('testPolygonOpacityPropThree', 0, async function (done) {
        console.info('testPolygonOpacityPropThree START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropThree);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('opacityPropThree')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.opacity).assertEqual('1.5')
        console.info("[polygonProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeProp
     * @tc.desc      ACE
     */
    it('testPolygonStrokeProp', 0, async function (done) {
        console.info('testPolygonStrokeProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeDashArrayProp
     * @tc.desc      ACE
     */
    it('testPolygonStrokeDashArrayProp', 0, async function (done) {
        console.info('testPolygonStrokeDashArrayProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeDashArrayProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeDashArrayProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeDasharray).assertEqual('2 3')
        console.info("[polygonProps] get inspector dasharray value is: " + JSON.stringify(obj.$attrs.strokeDasharray));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeDashOffsetProp
     * @tc.desc      ACE
     */
    it('testPolygonStrokeDashOffsetProp', 0, async function (done) {
        console.info('testPolygonStrokeDashOffsetProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeDashOffsetProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeDashOffsetProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeDashoffset).assertEqual('23')
        console.info("[polygonProps] get dashoffset value is: " + JSON.stringify(obj.$attrs.strokeDashoffset));
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeLineJoinPropOne
     * @tc.desc      ACE
     */
    it('testPolygonStrokeLineJoinPropOne', 0, async function (done) {
        console.info('testPolygonStrokeLineJoinPropOne START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineJoinPropOne);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineJoinPropOne')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLinejoin).assertEqual('bevel')
        console.info("[polygonProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeLineJoinPropTwo
     * @tc.desc      ACE
     */
    it('testPolygonStrokeLineJoinPropTwo', 0, async function (done) {
        console.info('testPolygonStrokeLineJoinPropTwo START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineJoinPropTwo);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineJoinPropTwo')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLinejoin).assertEqual('miter')
        console.info("[polygonProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeLineJoinPropThree
     * @tc.desc      ACE
     */
    it('testPolygonStrokeLineJoinPropThree', 0, async function (done) {
        console.info('testPolygonStrokeLineJoinPropThree START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineJoinPropThree);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineJoinPropThree')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLinejoin).assertEqual('round')
        console.info("[polygonProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeLineJoinPropOne
     * @tc.desc      ACE
     */
    it('testPolygonStrokeLineCapPropOne', 0, async function (done) {
        console.info('testPolygonStrokeLineCapPropOne START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineCapPropOne);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineCapPropOne')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLineCap).assertEqual('butt')
        console.info("[polygonProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeLineCapPropTwo
     * @tc.desc      ACE
     */
    it('testPolygonStrokeLineCapPropTwo', 0, async function (done) {
        console.info('testPolygonStrokeLineCapPropTwo START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineCapPropTwo);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineCapPropTwo')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLineCap).assertEqual('round')
        console.info("[polygonProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeLineCapPropThree
     * @tc.desc      ACE
     */
    it('testPolygonStrokeLineCapPropThree', 0, async function (done) {
        console.info('testPolygonStrokeLineCapPropThree START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineCapPropThree);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineCapPropThree')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLineCap).assertEqual('square')
        console.info("[polygonProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeMiterLimitProp
     * @tc.desc      ACE
     */
    it('testPolygonStrokeMiterLimitProp', 0, async function (done) {
        console.info('testPolygonStrokeMiterLimitProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeMiterLimitProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeMiterLimitProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeMiterlimit).assertEqual('10')
        console.info("[polygonProps] get  miterlimit value is:" + JSON.stringify(obj.$attrs.strokeMiterlimit));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeOpacityPropOne
     * @tc.desc      ACE
     */
    it('testPolygonStrokeOpacityPropOne', 0, async function (done) {
        console.info('testPolygonStrokeOpacityPropOne START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropOne);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropOne')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeOpacity).assertEqual('0.5')
        console.info("[polygonProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeOpacityPropTwo
     * @tc.desc      ACE
     */
    it('testPolygonStrokeOpacityPropTwo', 0, async function (done) {
        console.info('testPolygonStrokeOpacityPropTwo START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropTwo);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropTwo')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeOpacity).assertEqual('1')
        console.info("[polygonProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeOpacityPropThree
     * @tc.desc      ACE
     */
    it('testPolygonStrokeOpacityPropThree', 0, async function (done) {
        console.info('testPolygonStrokeOpacityPropThree START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropThree);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropThree')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeOpacity).assertEqual('1.5')
        console.info("[polygonProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonStrokeWidthProp
     * @tc.desc      ACE
     */
    it('testPolygonStrokeWidthProp', 0, async function (done) {
        console.info('testPolygonStrokeWidthProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeWidthProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeWidthProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeWidth).assertEqual('6')
        console.info("[polygonProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonTransformProp
     * @tc.desc      ACE
     */
    it('testPolygonTransformProp', 0, async function (done) {
        console.info('testPolygonTransformProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.transformProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('transformProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.transform).assertEqual('translate(10,5)scale(0.5 1.3)rotate(20 40 90)skewX(10)skewY(-15)')
        console.info("[polygonProps] get inspector transform value is: " + JSON.stringify(obj.$attrs.transform));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonIdProp
     * @tc.desc      ACE
     */
     it('testPolygonIdProp', 0, async function (done) {
        console.info('testPolygonIdProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('idProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonPointProp
     * @tc.desc      ACE
     */
     it('testPolygonPointProp', 0, async function (done) {
        console.info('testPolygonPointProp START');
        console.info("[polygonProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.pointProp);
        console.info("[polygonProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[polygonProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('polygon')
        console.info("[polygonProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('pointProp')
        console.info("[polygonProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.points).assertEqual('10,30 20,0 30,30 40,0')
        console.info("[polygonProps] get inspector points value is: " + JSON.stringify(obj.$attrs.points));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[polygonProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.stroke).assertEqual('blue')
        console.info("[polygonProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });
});
