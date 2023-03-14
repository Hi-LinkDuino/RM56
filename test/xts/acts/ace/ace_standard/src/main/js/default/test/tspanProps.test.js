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


describe('toolbarPropsJsTest', function () {

    var name = 'tspan', nameA = 'Tspan', labelName = 'tspan';

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
        console.info('['+name+'PropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/'+name+'/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push "+name+"Props page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push "+name+"Props page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    });

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('['+name+'PropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanIdProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'IdProp', 0, async function (done) {
        console.info('test'+nameA+'IdProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanXPropLength
     * @tc.desc      ACE
     */
    it('test'+nameA+'XPropLength', 0, async function (done) {
        console.info('test'+nameA+'XPropLength START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.xPropLength);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('xPropLength')
        expect(obj.$attrs.x).assertEqual('30')
        console.info("["+name+"Props] get x value is: " + JSON.stringify(obj.$attrs.x));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgTspanXPropPercentage
     * @tc.desc      ACE
     */
    it('test'+nameA+'XPropPercentage', 0, async function (done) {
        console.info('test'+nameA+'XPropPercentage START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.xPropPercentage);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('xPropPercentage')
        expect(obj.$attrs.x).assertEqual('60%')
        console.info("["+name+"Props] get x value is: " + JSON.stringify(obj.$attrs.x));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanXPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'XPropNone', 0, async function (done) {
        console.info('test'+nameA+'XPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.xPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('xPropNone')
        expect(obj.$attrs.x).assertEqual(undefined)
        console.info("["+name+"Props] get x value is: " + JSON.stringify(obj.$attrs.x));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanYPropLength
     * @tc.desc      ACE
     */
    it('test'+nameA+'YPropLength', 0, async function (done) {
        console.info('test'+nameA+'YPropLength START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.yPropLength);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('yPropLength')
        expect(obj.$attrs.y).assertEqual('10')
        console.info("["+name+"Props] get y value is: " + JSON.stringify(obj.$attrs.y));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanYPropPercentage
     * @tc.desc      ACE
     */
    it('test'+nameA+'YPropPercentage', 0, async function (done) {
        console.info('test'+nameA+'YPropPercentage START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.yPropPercentage);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('yPropPercentage')
        expect(obj.$attrs.y).assertEqual('100%')
        console.info("["+name+"Props] get y value is: " + JSON.stringify(obj.$attrs.y));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanYPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'YPropNone', 0, async function (done) {
        console.info('test'+nameA+'YPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.yPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('yPropNone')
        expect(obj.$attrs.y).assertEqual(undefined)
        console.info("["+name+"Props] get y value is: " + JSON.stringify(obj.$attrs.y));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanDxPropLength
     * @tc.desc      ACE
     */
    it('test'+nameA+'DxPropLength', 0, async function (done) {
        console.info('test'+nameA+'DxPropLength START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dxPropLength);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('dxPropLength')
        expect(obj.$attrs.dx).assertEqual('30')
        console.info("["+name+"Props] get dx value is: " + JSON.stringify(obj.$attrs.dx));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanDxPropPercentage
     * @tc.desc      ACE
     */
    it('test'+nameA+'DxPropPercentage', 0, async function (done) {
        console.info('test'+nameA+'DxPropPercentage START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dxPropPercentage);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('dxPropPercentage')
        expect(obj.$attrs.dx).assertEqual('60%')
        console.info("["+name+"Props] get dx value is: " + JSON.stringify(obj.$attrs.dx));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanDxPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'DxPropNone', 0, async function (done) {
        console.info('test'+nameA+'DxPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dxPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('dxPropNone')
        expect(obj.$attrs.dx).assertEqual(undefined)
        console.info("["+name+"Props] get dx value is: " + JSON.stringify(obj.$attrs.dx));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanDyPropLength
     * @tc.desc      ACE
     */
    it('test'+nameA+'DyPropLength', 0, async function (done) {
        console.info('test'+nameA+'DyPropLength START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dyPropLength);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('dyPropLength')
        expect(obj.$attrs.dy).assertEqual('10')
        console.info("["+name+"Props] get dy value is: " + JSON.stringify(obj.$attrs.dy));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanDyPropPercentage
     * @tc.desc      ACE
     */
    it('test'+nameA+'DyPropPercentage', 0, async function (done) {
        console.info('test'+nameA+'DyPropPercentage START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dyPropPercentage);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('dyPropPercentage')
        expect(obj.$attrs.dy).assertEqual('100%')
        console.info("["+name+"Props] get dy value is: " + JSON.stringify(obj.$attrs.dy));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanDyPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'DyPropNone', 0, async function (done) {
        console.info('test'+nameA+'DyPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dyPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('dyPropNone')
        expect(obj.$attrs.dy).assertEqual(undefined)
        console.info("["+name+"Props] get dy value is: " + JSON.stringify(obj.$attrs.dy));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanRotateProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'RotateProp', 0, async function (done) {
        console.info('test'+nameA+'RotateProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.rotateProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('rotateProp')
        expect(obj.$attrs.rotate).assertEqual('30')
        console.info("["+name+"Props] get rotate value is: " + JSON.stringify(obj.$attrs.rotate));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanRotatePropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'RotatePropNone', 0, async function (done) {
        console.info('test'+nameA+'RotatePropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.rotatePropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('rotatePropNone')
        expect(obj.$attrs.rotate).assertEqual(undefined)
        console.info("["+name+"Props] get rotate value is: " + JSON.stringify(obj.$attrs.rotate));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanFontSizeProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'FontSizeProp', 0, async function (done) {
        console.info('test'+nameA+'FontSizeProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fontSizeProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fontSizeProp')
        expect(obj.$attrs.fontSize).assertEqual('20')
        console.info("["+name+"Props] get fontSize value is: " + JSON.stringify(obj.$attrs.fontSize));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanFontSizePropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FontSizePropNone', 0, async function (done) {
        console.info('test'+nameA+'FontSizePropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fontSizePropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fontSizePropNone')
        expect(obj.$attrs.fontSize).assertEqual(undefined)
        console.info("["+name+"Props] get fontSize value is: " + JSON.stringify(obj.$attrs.fontSize));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanFillProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillProp', 0, async function (done) {
        console.info('test'+nameA+'FillProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillProp')
        expect(obj.$attrs.fill).assertEqual('red')
        console.info("["+name+"Props] get fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanFillPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillPropNone', 0, async function (done) {
        console.info('test'+nameA+'FillPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillPropNone')
        expect(obj.$attrs.fill).assertEqual(undefined)
        console.info("["+name+"Props] get fill value is: " + JSON.stringify(obj.$attrs.fill));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanFillOpacityProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillOpacityProp', 0, async function (done) {
        console.info('test'+nameA+'FillOpacityProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillOpacityProp')
        expect(obj.$attrs.fillOpacity).assertEqual('0.5')
        console.info("["+name+"Props] get fillOpacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanFillOpacityPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FillOpacityPropNone', 0, async function (done) {
        console.info('test'+nameA+'FillOpacityPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.fillOpacityPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('fillOpacityPropNone')
        expect(obj.$attrs.fillOpacity).assertEqual(undefined)
        console.info("["+name+"Props] get fillOpacity value is: " + JSON.stringify(obj.$attrs.fillOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanOpacityProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'OpacityProp', 0, async function (done) {
        console.info('test'+nameA+'OpacityProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('opacityProp')
        expect(obj.$attrs.opacity).assertEqual('0.8')
        console.info("["+name+"Props] get opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanOpacityPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'OpacityPropNone', 0, async function (done) {
        console.info('test'+nameA+'OpacityPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.opacityPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('opacityPropNone')
        expect(obj.$attrs.opacity).assertEqual(undefined)
        console.info("["+name+"Props] get opacity value is: " + JSON.stringify(obj.$attrs.opacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanStrokeProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeProp', 0, async function (done) {
        console.info('test'+nameA+'StrokeProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeProp')
        expect(obj.$attrs.stroke).assertEqual('#ff00ff')
        console.info("["+name+"Props] get stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanStrokePropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokePropNone', 0, async function (done) {
        console.info('test'+nameA+'StrokePropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokePropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokePropNone')
        expect(obj.$attrs.stroke).assertEqual(undefined)
        console.info("["+name+"Props] get stroke value is: " + JSON.stringify(obj.$attrs.stroke));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanStrokeWidthProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeWidthProp', 0, async function (done) {
        console.info('test'+nameA+'StrokeWidthProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeWidthProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeWidthProp')
        expect(obj.$attrs.strokeWidth).assertEqual('2')
        console.info("["+name+"Props] get strokeWidth value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanStrokeWidthPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeWidthPropNone', 0, async function (done) {
        console.info('test'+nameA+'StrokeWidthPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeWidthPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeWidthPropNone')
        expect(obj.$attrs.strokeWidth).assertEqual(undefined)
        console.info("["+name+"Props] get strokeWidth value is: " + JSON.stringify(obj.$attrs.strokeWidth));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanStrokeOpacityProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeOpacityProp', 0, async function (done) {
        console.info('test'+nameA+'StrokeOpacityProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeOpacityProp')
        expect(obj.$attrs.strokeOpacity).assertEqual('0.4')
        console.info("["+name+"Props] get strokeOpacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanStrokeOpacityPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'StrokeOpacityPropNone', 0, async function (done) {
        console.info('test'+nameA+'StrokeOpacityPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.strokeOpacityPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(labelName)
        expect(obj.$attrs.id).assertEqual('strokeOpacityPropNone')
        expect(obj.$attrs.strokeOpacity).assertEqual(undefined)
        console.info("["+name+"Props] get strokeOpacity value is: " + JSON.stringify(obj.$attrs.strokeOpacity));
        done();
    });
});
