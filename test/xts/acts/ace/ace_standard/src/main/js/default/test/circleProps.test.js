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
 import {describe, beforeAll,afterAll, it, expect} from 'deccjsunit/index';
 
 
 describe('circlePropsJsTest', function () {
 
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
         console.info('[circlePropsJsTest] before each called')
 
         let result;
         let options = {
             uri: 'pages/circle/prop/index'
         }
         try {
             result = router.push(options)
             console.info("push circleProps page success " + JSON.stringify(result));
         } catch (err) {
             console.error("push circleProps page error " + JSON.stringify(result));
         }
         await sleep(4000)
         done()
     });
 
     /**
     * run after testcase
     */
     afterAll(async function () {
         console.info('[circlePropsJsTest] after each called')
         await backToIndex()
         await sleep(1000)
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircletFillProp
      * @tc.desc      ACE
      */
     it('testCircleFillProp', 0, async function (done) { 
         console.info('testCircleFillProp START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillProp);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillProp')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('50')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleFillOpacityPropOne
      * @tc.desc      ACE
      */
     it('testCircleFillOpacityPropOne', 0, async function (done) {
         console.info('testCircleFillOpacityPropOne START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillOpacityPropOne);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillOpacityPropOne')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillOpacity).assertEqual('0.5')
         console.info("[circleProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleFillOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testCircleFillOpacityPropTwo', 0, async function (done) {
         console.info('testCircleFillOpacityPropTwo START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillOpacityPropTwo);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillOpacityPropTwo')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillOpacity).assertEqual('1')
         console.info("[circleProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleFillOpacityPropThree
      * @tc.desc      ACE
      */
     it('testCircleFillOpacityPropThree', 0, async function (done) {
         console.info('testCircleFillOpacityPropThree START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillOpacityPropThree);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillOpacityPropThree')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillOpacity).assertEqual('1.5')
         console.info("[circleProps] get inspector fill-opacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleFillRulePropOne
      * @tc.desc      ACE
      */
     it('testCircleFillRulePropOne', 0, async function (done) {
         console.info('testCircleFillRulePropOne START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillRulePropOne);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillRulePropOne')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillRule).assertEqual('evenodd')
         console.info("[circleProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleFillRulePropTwo
      * @tc.desc      ACE
      */
     it('testCircleFillRulePropTwo', 0, async function (done) {
         console.info('testCircleFillRulePropTwo START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.fillRulePropTwo);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('fillRulePropTwo')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.fillRule).assertEqual('nonzero')
         console.info("[circleProps] get inspector fill-rule value is: " + JSON.stringify(obj.$attrs.fillRule));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleOpacityPropOne
      * @tc.desc      ACE
      */
     it('testCircleOpacityPropOne', 0, async function (done) {
         console.info('testCircleOpacityPropOne START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropOne);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropOne')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.opacity).assertEqual('0.5')
         console.info("[lineProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));;
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testCircleOpacityPropTwo', 0, async function (done) {
         console.info('testCircleOpacityPropTwo START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropTwo);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropTwo')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.opacity).assertEqual('1')
         console.info("[circleProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleOpacityPropThree
      * @tc.desc      ACE
      */
     it('testCircleOpacityPropThree', 0, async function (done) {
         console.info('testCircleOpacityPropThree START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.opacityPropThree);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('opacityPropThree')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.fill).assertEqual('red')
         console.info("[circleProps] get inspector fill value is: " + JSON.stringify(obj.$attrs.fill));
         expect(obj.$attrs.opacity).assertEqual('1.5')
         console.info("[circleProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.opacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeProp
      * @tc.desc      ACE
      */
     it('testCircleStrokeProp', 0, async function (done) {
         console.info('testCircleStrokeProp START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeProp);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeProp')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeDashArrayProp
      * @tc.desc      ACE
      */
     it('testCircleStrokeDashArrayProp', 0, async function (done) {
         console.info('testCircleStrokeDashArrayProp START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeDashArrayProp);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeDashArrayProp')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeDasharray).assertEqual('2 3')
         console.info("[circleProps] get inspector dasharray value is: " + JSON.stringify(obj.$attrs.strokeDasharray));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeDashOffsetProp
      * @tc.desc      ACE
      */
     it('testCircleStrokeDashOffsetProp', 0, async function (done) {
         console.info('testCircleStrokeDashOffsetProp START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeDashOffsetProp);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeDashOffsetProp')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeDashoffset).assertEqual('23')
         console.info("[circleProps] get dashoffset value is:" + JSON.stringify(obj.$attrs.strokeDashoffset));
         done();
     });
 
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeLineJoinPropOne
      * @tc.desc      ACE
      */
     it('testCircleStrokeLineJoinPropOne', 0, async function (done) {
         console.info('testCircleStrokeLineJoinPropOne START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropOne);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropOne')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('bevel')
         console.info("[circleProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeLineJoinPropTwo
      * @tc.desc      ACE
      */
     it('testCircleStrokeLineJoinPropTwo', 0, async function (done) {
         console.info('testCircleStrokeLineJoinPropTwo START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropTwo);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropTwo')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('miter')
         console.info("[circleProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeLineJoinPropThree
      * @tc.desc      ACE
      */
     it('testCircleStrokeLineJoinPropThree', 0, async function (done) {
         console.info('testCircleStrokeLineJoinPropThree START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineJoinPropThree);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineJoinPropThree')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLinejoin).assertEqual('round')
         console.info("[circleProps] get inspector linejoin value is: " + JSON.stringify(obj.$attrs.strokeLinejoin));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeLineJoinPropOne
      * @tc.desc      ACE
      */
     it('testCircleStrokeLineCapPropOne', 0, async function (done) {
         console.info('testCircleStrokeLineCapPropOne START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropOne);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropOne')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('butt')
         console.info("[circleProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeLineCapPropTwo
      * @tc.desc      ACE
      */
     it('testCircleStrokeLineCapPropTwo', 0, async function (done) {
         console.info('testCircleStrokeLineCapPropTwo START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropTwo);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropTwo')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('round')
         console.info("[circleProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeLineCapPropThree
      * @tc.desc      ACE
      */
     it('testCircleStrokeLineCapPropThree', 0, async function (done) {
         console.info('testCircleStrokeLineCapPropThree START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeLineCapPropThree);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeLineCapPropThree')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeLineCap).assertEqual('square')
         console.info("[circleProps] get inspector lineCap value is: " + JSON.stringify(obj.$attrs.strokeLineCap));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeMiterLimitProp
      * @tc.desc      ACE
      */
     it('testCircleStrokeMiterLimitProp', 0, async function (done) {
         console.info('testCircleStrokeMiterLimitProp START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeMiterLimitProp);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeMiterLimitProp')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeMiterlimit).assertEqual('10')
         console.info("[circleProps] get miterlimit value is: " + JSON.stringify(obj.$attrs.strokeMiterlimit));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeOpacityPropOne
      * @tc.desc      ACE
      */
     it('testCircleStrokeOpacityPropOne', 0, async function (done) {
         console.info('testCircleStrokeOpacityPropOne START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropOne);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropOne')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('0.5')
         console.info("[circleProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeOpacityPropTwo
      * @tc.desc      ACE
      */
     it('testCircleStrokeOpacityPropTwo', 0, async function (done) {
         console.info('testCircleStrokeOpacityPropTwo START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropTwo);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropTwo')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('1')
         console.info("[circleProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeOpacityPropThree
      * @tc.desc      ACE
      */
     it('testCircleStrokeOpacityPropThree', 0, async function (done) {
         console.info('testCircleStrokeOpacityPropThree START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeOpacityPropThree);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeOpacityPropThree')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeOpacity).assertEqual('1.5')
         console.info("[circleProps] get inspector opacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleStrokeWidthProp
      * @tc.desc      ACE
      */
     it('testCircleStrokeWidthProp', 0, async function (done) {
         console.info('testCircleStrokeWidthProp START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.strokeWidthProp);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('strokeWidthProp')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.stroke).assertEqual('red')
         console.info("[circleProps] get inspector stroke value is: " + JSON.stringify(obj.$attrs.stroke));
         expect(obj.$attrs.strokeWidth).assertEqual('6')
         console.info("[circleProps] get inspector stroke-width value is: " + JSON.stringify(obj.$attrs.strokeWidth));
         done();
     });
 
     /**
      * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
      * @tc.name      testCircleTransformProp
      * @tc.desc      ACE
      */
     it('testCircleTransformProp', 0, async function (done) {
         console.info('testCircleTransformProp START');
         console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
 
         let obj = JSON.parse(globalThis.value.transformProp);
         console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
         console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
 
         expect(obj.$type).assertEqual('circle')
         console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
         expect(obj.$attrs.id).assertEqual('transformProp')
         console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
         expect(obj.$attrs.cx).assertEqual('30')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('12')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('10')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
         expect(obj.$attrs.transform).assertEqual('translate(10,5)scale(0.5 1.3)rotate(20 40 90)skewX(10)skewY(-15)')
         console.info("[circleProps] get inspector transform value is: " + JSON.stringify(obj.$attrs.transform));
         done();
     });

     /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleIdProp
     * @tc.desc      ACE
     */
      it('testCircleIdProp', 0, async function (done) {
        console.info('testCircleIdProp START');
        console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('circle')
        console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('idProp')
        console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.cx).assertEqual('50')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('25')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('20')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleCxPropOne
     * @tc.desc      ACE
     */
     it('testCircleCxPropOne', 0, async function (done) {
        console.info('testCircleCxPropOne START');
        console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.cxPropOne);
        console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('circle')
        console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('cxPropOne')
        console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.cx).assertEqual('50')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('25')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('20')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleCxPropTwo
     * @tc.desc      ACE
     */
     it('testCircleCxPropTwo', 0, async function (done) {
        console.info('testCircleCxPropTwo START');
        console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.cxPropTwo);
        console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('circle')
        console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('cxPropTwo')
        console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.cx).assertEqual('50%')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('25')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('20')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
        expect(obj.$attrs.fill).assertEqual('pink')
        console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleCyPropOne
     * @tc.desc      ACE
     */
     it('testCircleCyPropOne', 0, async function (done) {
        console.info('testCircleCyPropOne START');
        console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.cyPropOne);
        console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('circle')
        console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('cyPropOne')
        console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.cx).assertEqual('50')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('25')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('20')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleCyPropTwo
     * @tc.desc      ACE
     */
     it('testCircleCyPropTwo', 0, async function (done) {
        console.info('testCircleCyPropTwo START');
        console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.cxPropTwo);
        console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('circle')
        console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('cxPropTwo')
        console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.cx).assertEqual('100')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('25%')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('20')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
        expect(obj.$attrs.fill).assertEqual('pink')
        console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleRPropOne
     * @tc.desc      ACE
     */
     it('testCircleRPropOne', 0, async function (done) {
        console.info('testCircleRPropOne START');
        console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.rPropOne);
        console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('circle')
        console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('rPropOne')
        console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.cx).assertEqual('50')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('25')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('20')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleRPropTwo
     * @tc.desc      ACE
     */
     it('testCircleRPropTwo', 0, async function (done) {
        console.info('testCircleRPropTwo START');
        console.info("[circleProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.rPropTwo);
        console.info("[circleProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[circleProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('circle')
        console.info("[circleProps] get inspector $type value is: " + JSON.stringify(obj.$attrs.$type));
        expect(obj.$attrs.id).assertEqual('rPropTwo')
        console.info("[circleProps] get inspector id value is: " + JSON.stringify(obj.$attrs.id));
        expect(obj.$attrs.cx).assertEqual('100')
         console.info("[circleProps] get inspector cx value is: " + JSON.stringify(obj.$attrs.cx));
         expect(obj.$attrs.cy).assertEqual('25')
         console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.cy));
         expect(obj.$attrs.r).assertEqual('20%')
         console.info("[circleProps] get inspector r value is: " + JSON.stringify(obj.$attrs.r));
        expect(obj.$attrs.fill).assertEqual('pink')
        console.info("[circleProps] get inspector cy value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });
  
 });
 