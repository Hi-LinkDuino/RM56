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


describe('svgPropsJsTest', function () {

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
        console.info('[svgPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/svg/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push svgProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push svgProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[svgPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgFillProp
     * @tc.desc      ACE
     */
    it('testSvgFillProp', 0, async function (done) {
        console.info('testSvgFillProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgFillOpacityPropOne
     * @tc.desc      ACE
     */
    it('testSvgFillOpacityPropOne', 0, async function (done) {
        console.info('testSvgFillOpacityPropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillOpacityPropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillOpacity).assertEqual('0.5')
        console.info("[svgProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgFillOpacityPropTwo
     * @tc.desc      ACE
     */
    it('testSvgFillOpacityPropTwo', 0, async function (done) {
        console.info('testSvgFillOpacityPropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillOpacityPropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillOpacity).assertEqual('1')
        console.info("[svgProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgFillOpacityPropThree
     * @tc.desc      ACE
     */
    it('testSvgFillOpacityPropThree', 0, async function (done) {
        console.info('testSvgFillOpacityPropThree START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropThree);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillOpacityPropThree')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillOpacity).assertEqual('1.5')
        console.info("[svgProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgFillRulePropOne
     * @tc.desc      ACE
     */
    it('testSvgFillRulePropOne', 0, async function (done) {
        console.info('testSvgFillRulePropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillRulePropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillRulePropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillRule).assertEqual('evenodd')
        console.info("[svgProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgFillRulePropTwo
     * @tc.desc      ACE
     */
    it('testSvgFillRulePropTwo', 0, async function (done) {
        console.info('testSvgFillRulePropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillRulePropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('fillRulePropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.fillRule).assertEqual('nonzero')
        console.info("[svgProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgOpacityPropOne
     * @tc.desc      ACE
     */
    it('testSvgOpacityPropOne', 0, async function (done) {
        console.info('testSvgOpacityPropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('opacityPropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.opacity).assertEqual('0.5')
         console.info("[lineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgOpacityPropTwo
     * @tc.desc      ACE
     */
    it('testSvgOpacityPropTwo', 0, async function (done) {
        console.info('testSvgOpacityPropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('opacityPropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.opacity).assertEqual('1')
        console.info("[svgProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgOpacityPropThree
     * @tc.desc      ACE
     */
    it('testSvgOpacityPropThree', 0, async function (done) {
        console.info('testSvgOpacityPropThree START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropThree);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('opacityPropThree')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        expect(obj.$attrs.opacity).assertEqual('1.5')
        console.info("[svgProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeProp
     * @tc.desc      ACE
     */
    it('testSvgStrokeProp', 0, async function (done) {
        console.info('testSvgStrokeProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeDashArrayProp
     * @tc.desc      ACE
     */
    it('testSvgStrokeDashArrayProp', 0, async function (done) {
        console.info('testSvgStrokeDashArrayProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeDashArrayProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeDashArrayProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeDasharray).assertEqual('2 3')
        console.info("[svgProps] get inspector dasharray value is: " + JSON.stringify(obj.$attrs.strokeDasharray));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeDashOffsetProp
     * @tc.desc      ACE
     */
    it('testSvgStrokeDashOffsetProp', 0, async function (done) {
        console.info('testSvgStrokeDashOffsetProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeDashOffsetProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeDashOffsetProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeDashoffset).assertEqual('23')
        console.info("[svgProps] get inspector dashoffset value is: " + JSON.stringify(obj.$attrs.strokeDashoffset));
        done();
    });


    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeLineJoinPropOne
     * @tc.desc      ACE
     */
    it('testSvgStrokeLineJoinPropOne', 0, async function (done) {
        console.info('testSvgStrokeLineJoinPropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineJoinPropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineJoinPropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLinejoin).assertEqual('bevel')
        console.info("[svgProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeLineJoinPropTwo
     * @tc.desc      ACE
     */
    it('testSvgStrokeLineJoinPropTwo', 0, async function (done) {
        console.info('testSvgStrokeLineJoinPropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineJoinPropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineJoinPropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLinejoin).assertEqual('miter')
        console.info("[svgProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeLineJoinPropThree
     * @tc.desc      ACE
     */
    it('testSvgStrokeLineJoinPropThree', 0, async function (done) {
        console.info('testSvgStrokeLineJoinPropThree START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineJoinPropThree);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineJoinPropThree')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLinejoin).assertEqual('round')
        console.info("[svgProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeLineJoinPropOne
     * @tc.desc      ACE
     */
    it('testSvgStrokeLineCapPropOne', 0, async function (done) {
        console.info('testSvgStrokeLineCapPropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineCapPropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineCapPropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLineCap).assertEqual('butt')
        console.info("[svgProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeLineCapPropTwo
     * @tc.desc      ACE
     */
    it('testSvgStrokeLineCapPropTwo', 0, async function (done) {
        console.info('testSvgStrokeLineCapPropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineCapPropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineCapPropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLineCap).assertEqual('round')
        console.info("[svgProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeLineCapPropThree
     * @tc.desc      ACE
     */
    it('testSvgStrokeLineCapPropThree', 0, async function (done) {
        console.info('testSvgStrokeLineCapPropThree START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeLineCapPropThree);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeLineCapPropThree')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeLineCap).assertEqual('square')
        console.info("[svgProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeMiterLimitProp
     * @tc.desc      ACE
     */
    it('testSvgStrokeMiterLimitProp', 0, async function (done) {
        console.info('testSvgStrokeMiterLimitProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeMiterLimitProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeMiterLimitProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeMiterlimit).assertEqual('10')
        console.info("[svgProps] get inspector miterlimit value is: " + JSON.stringify(obj.$attrs.strokeMiterlimit));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeOpacityPropOne
     * @tc.desc      ACE
     */
    it('testSvgStrokeOpacityPropOne', 0, async function (done) {
        console.info('testSvgStrokeOpacityPropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeOpacity).assertEqual('0.5')
        console.info("[svgProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeOpacityPropTwo
     * @tc.desc      ACE
     */
    it('testSvgStrokeOpacityPropTwo', 0, async function (done) {
        console.info('testSvgStrokeOpacityPropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeOpacity).assertEqual('1')
        console.info("[svgProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeOpacityPropThree
     * @tc.desc      ACE
     */
    it('testSvgStrokeOpacityPropThree', 0, async function (done) {
        console.info('testSvgStrokeOpacityPropThree START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropThree);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropThree')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeOpacity).assertEqual('1.5')
        console.info("[svgProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgStrokeWidthProp
     * @tc.desc      ACE
     */
    it('testSvgStrokeWidthProp', 0, async function (done) {
        console.info('testSvgStrokeWidthProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeWidthProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('strokeWidthProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.stroke).assertEqual('red')
        console.info("[svgProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        expect(obj.$attrs.strokeWidth).assertEqual('6')
        console.info("[svgProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgTransformProp
     * @tc.desc      ACE
     */
    it('testSvgTransformProp', 0, async function (done) {
        console.info('testSvgTransformProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.transformProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('transformProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.transform).assertEqual('translate(10,5)scale(0.5 1.3)rotate(20 40 90)skewX(10)skewY(-15)')
        console.info("[svgProps] get inspector transform value is: " + JSON.stringify(obj.$attrs.transform));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgIdProp
     * @tc.desc      ACE
     */
    it('testSvgIdProp', 0, async function (done) {
        console.info('testSvgIdProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('idProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('40px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testSvgWidthPropOne
      * @tc.desc      ACE
      */
    it('testSvgWidthPropOne', 0, async function (done) {
        console.info('testSvgWidthPropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.widthPropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('widthPropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testSvgWidthPropTwo
      * @tc.desc      ACE
      */
    it('testSvgWidthPropTwo', 0, async function (done) {
        console.info('testSvgWidthPropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.widthPropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('widthPropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50%')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testSvgHeightPropOne
      * @tc.desc      ACE
      */
    it('testSvgHeightPropOne', 0, async function (done) {
        console.info('testSvgHeightPropOne START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.heightPropOne);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('heightPropOne')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testSvgHeightPropTwo
      * @tc.desc      ACE
      */
    it('testSvgHeightPropTwo', 0, async function (done) {
        console.info('testSvgHeightPropTwo START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.heightPropTwo);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('heightPropTwo')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('50px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('20%')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[svgProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testSvgViewBoxProp
      * @tc.desc      ACE
      */
    it('testSvgViewBoxProp', 0, async function (done) {
        console.info('testSvgViewBoxProp START');
        console.info("[svgProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.viewBoxProp);
        console.info("[svgProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[svgProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('svg')
        console.info("[svgProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('viewBoxProp')
        console.info("[svgProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.width).assertEqual('100px')
        console.info("[svgProps] get inspector width value is: " + JSON.stringify(obj.$attrs.width));
        expect(obj.$attrs.height).assertEqual('40px')
        console.info("[svgProps] get inspector height value is: " + JSON.stringify(obj.$attrs.height));
        expect(obj.$attrs.viewBox).assertEqual('0 0 20 20')
        console.info("[svgProps] get inspector viewBox value is: " + JSON.stringify(obj.$attrs.viewBox));
        done();
    });
});
