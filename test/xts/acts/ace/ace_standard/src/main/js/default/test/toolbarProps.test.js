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

    var name = 'toolbar', nameA = 'Toolbar';

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
     * @tc.name      testToolbarIdProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'IdProp', 0, async function (done) {
        console.info('test'+nameA+'IdProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarClassProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'ClassProp', 0, async function (done) {
        console.info('test'+nameA+'ClassProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarClassPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'ClassPropNone', 0, async function (done) {
        console.info('test'+nameA+'ClassPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("["+name+"Props] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarRefProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'RefProp', 0, async function (done) {
        console.info('test'+nameA+'RefProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarRefPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'RefPropNone', 0, async function (done) {
        console.info('test'+nameA+'RefPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("["+name+"Props] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDisabledPropTrue
     * @tc.desc      ACE
     */
    it('test'+nameA+'DisabledPropTrue', 0, async function (done) {
        console.info('test'+nameA+'DisabledPropTrue START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropTrue);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('disabledPropTrue')
        expect(obj.$attrs.disabled).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDisabledPropFalse
     * @tc.desc      ACE
     */
    it('test'+nameA+'DisabledPropFalse', 0, async function (done) {
        console.info('test'+nameA+'DisabledPropFalse START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropFalse);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('disabledPropFalse')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDisabledPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'DisabledPropNone', 0, async function (done) {
        console.info('test'+nameA+'DisabledPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.disabledPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('disabledPropNone')
        expect(obj.$attrs.disabled).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarFocusablePropTrue
     * @tc.desc      ACE
     */
    it('test'+nameA+'FocusablePropTrue', 0, async function (done) {
        console.info('test'+nameA+'FocusablePropTrue START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropTrue);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('focusablePropTrue')
        expect(obj.$attrs.focusable).assertEqual('true')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarFocusablePropFalse
     * @tc.desc      ACE
     */
    it('test'+nameA+'FocusablePropFalse', 0, async function (done) {
        console.info('test'+nameA+'FocusablePropFalse START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropFalse);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('focusablePropFalse')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarFocusablePropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'FocusablePropNone', 0, async function (done) {
        console.info('test'+nameA+'FocusablePropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.focusablePropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('focusablePropNone')
        expect(obj.$attrs.focusable).assertEqual('false')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDataProp
     * @tc.desc      ACE
     */
    it('test'+nameA+'DataProp', 0, async function (done) {
        console.info('test'+nameA+'DataProp START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataDiv).assertEqual(undefined);
        console.info("["+name+"Props] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDataPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'DataPropNone', 0, async function (done) {
        console.info('test'+nameA+'DataPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataDiv).assertEqual(undefined)
        console.info("["+name+"Props] get dataDiv value is: " + JSON.stringify(obj.$attrs.dataDiv));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('test'+nameA+'ClickEffectPropSmall', 0, async function (done) {
        console.info('test'+nameA+'ClickEffectPropSmall START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('test'+nameA+'ClickEffectPropMedium', 0, async function (done) {
        console.info('test'+nameA+'ClickEffectPropMedium START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('test'+nameA+'ClickEffectPropLarge', 0, async function (done) {
        console.info('test'+nameA+'ClickEffectPropLarge START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarClickEffectPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'ClickEffectPropNone', 0, async function (done) {
        console.info('test'+nameA+'ClickEffectPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("["+name+"Props] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDirPropRtl
     * @tc.desc      ACE
     */
    it('test'+nameA+'DirPropRtl', 0, async function (done) {
        console.info('test'+nameA+'DirPropRtl START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDirPropLtr
     * @tc.desc      ACE
     */
    it('test'+nameA+'DirPropLtr', 0, async function (done) {
        console.info('test'+nameA+'DirPropLtr START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDirPropAuto
     * @tc.desc      ACE
     */
    it('test'+nameA+'DirPropAuto', 0, async function (done) {
        console.info('test'+nameA+'DirPropAuto START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarDirPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'DirPropNone', 0, async function (done) {
        console.info('test'+nameA+'DirPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarForPropNull
     * @tc.desc      ACE
     */
    it('test'+nameA+'ForPropNull', 0, async function (done) {
        console.info('test'+nameA+'ForPropNull START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("["+name+"Props] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarForPropOne
     * @tc.desc      ACE
     */
    it('test'+nameA+'ForPropOne', 0, async function (done) {
        console.info('test'+nameA+'ForPropOne START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("["+name+"Props] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarForPropThree
     * @tc.desc      ACE
     */
    it('test'+nameA+'ForPropThree', 0, async function (done) {
        console.info('test'+nameA+'ForPropThree START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("["+name+"Props] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarIfPropTrue
     * @tc.desc      ACE
     */
    it('test'+nameA+'IfPropTrue', 0, async function (done) {
        console.info('test'+nameA+'IfPropTrue START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("["+name+"Props] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivIfPropFalse
     * @tc.desc      ACE
     */
    //    it('testDivIfPropFalse', 0, async function (done) {
    //        console.info('testDivIfPropFalse START');
    //        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropFalse);
    //        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('div')
    //        expect(obj.$attrs.id).assertEqual('ifPropFalse')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivIfPropNone
     * @tc.desc      ACE
     */
    //    it('testDivIfPropNone', 0, async function (done) {
    //        console.info('testDivIfPropNone START');
    //        console.info("[divProps] get globalThis.value is: " + JSON.stringify(globalThis.value));
    //
    //        let obj = JSON.parse(globalThis.value.ifPropNone);
    //        console.info("[divProps] get inspector value is: " + JSON.stringify(obj));
    //        console.info("[divProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));
    //
    //        expect(obj.$type).assertEqual('div')
    //        expect(obj.$attrs.id).assertEqual('ifPropNone')
    //        expect(obj.$attrs.if).assertEqual(false)
    //        done();
    //    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarShowPropTrue
     * @tc.desc      ACE
     */
    it('test'+nameA+'ShowPropTrue', 0, async function (done) {
        console.info('test'+nameA+'ShowPropTrue START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("["+name+"Props] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarShowPropFalse
     * @tc.desc      ACE
     */
    it('test'+nameA+'ShowPropFalse', 0, async function (done) {
        console.info('test'+nameA+'ShowPropFalse START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("["+name+"Props] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testToolbarShowPropNone
     * @tc.desc      ACE
     */
    it('test'+nameA+'ShowPropNone', 0, async function (done) {
        console.info('test'+nameA+'ShowPropNone START');
        console.info("["+name+"Props] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("["+name+"Props] get inspector value is: " + JSON.stringify(obj));
        console.info("["+name+"Props] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual(name)
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("["+name+"Props] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });
});
