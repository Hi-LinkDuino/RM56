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


describe('menuPropsJsTest', function () {

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
                })
                resolve()
            }, 500)
        })
        let clearPromise = new Promise((resolve, reject) => {
            setTimeout(() => {
                router.clear()
                resolve()
            }, 500)
        })
        await backToIndexPromise.then(() => {
            return clearPromise
        })
    }

    /**
    * run before testcase
    */
    beforeAll(async function (done) {
        console.info('[menuPropsJsTest] before each called')

        let result;
        let options = {
            uri: 'pages/menu/prop/index'
        }
        try {
            result = router.push(options)
            console.info("push menuProps page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push menuProps page error " + JSON.stringify(result));
        }
        await sleep(4000)
        done()
    })

    /**
    * run after testcase
    */
    afterAll(async function () {
        console.info('[menuPropsJsTest] after each called')
        await backToIndex()
        await sleep(1000)
    })

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuIdProp
     * @tc.desc      ACE
     */
    it('testMenuIdProp', 0, async function (done) {
        console.info('testMenuIdProp START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.idProp);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('idProp')
        done();
    });

    it('testMenuClassProp', 0, async function (done) {
        console.info('testMenuClassProp START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classProp);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('classProp')
        expect(obj.$attrs.className).assertEqual('classProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuClassPropNone
     * @tc.desc      ACE
     */
    it('testMenuClassPropNone', 0, async function (done) {
        console.info('testMenuClassPropNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.classPropNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('classPropNone')
        expect(obj.$attrs.className).assertEqual(undefined)
        console.info("[menuProps] get className value is: " + JSON.stringify(obj.$attrs.className));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuRefProp
     * @tc.desc      ACE
     */
    it('testMenuRefProp', 0, async function (done) {
        console.info('testMenuRefProp START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refProp);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('refProp')
        expect(obj.$attrs.ref).assertEqual('refProp')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuRefPropNone
     * @tc.desc      ACE
     */
    it('testMenuRefPropNone', 0, async function (done) {
        console.info('testMenuRefPropNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.refPropNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('refPropNone')
        expect(obj.$attrs.ref).assertEqual(undefined)
        console.info("[menuProps] get ref value is: " + JSON.stringify(obj.$attrs.ref));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuDataProp
     * @tc.desc      ACE
     */
    it('testMenuDataProp', 0, async function (done) {
        console.info('testMenuDataProp START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataProp);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('dataProp')
        expect(obj.$attrs.dataMenu).assertEqual(undefined);
        console.info("[menuProps] get dataMenu value is: " + JSON.stringify(obj.$attrs.dataMenu));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuDataPropNone
     * @tc.desc      ACE
     */
    it('testMenuDataPropNone', 0, async function (done) {
        console.info('testMenuDataPropNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dataPropNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('dataPropNone')
        expect(obj.$attrs.dataMenu).assertEqual(undefined)
        console.info("[menuProps] get dataMenu value is: " + JSON.stringify(obj.$attrs.dataMenu));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuClickEffectPropSmall
     * @tc.desc      ACE
     */
    it('testMenuClickEffectPropSmall', 0, async function (done) {
        console.info('testMenuClickEffectPropSmall START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropSmall);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('clickEffectPropSmall')
        expect(obj.$attrs.clickEffect).assertEqual('spring-small')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuClickEffectPropMedium
     * @tc.desc      ACE
     */
    it('testMenuClickEffectPropMedium', 0, async function (done) {
        console.info('testMenuClickEffectPropMedium START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropMedium);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('clickEffectPropMedium')
        expect(obj.$attrs.clickEffect).assertEqual('spring-medium')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuClickEffectPropLarge
     * @tc.desc      ACE
     */
    it('testMenuClickEffectPropLarge', 0, async function (done) {
        console.info('testMenuClickEffectPropLarge START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropLarge);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('clickEffectPropLarge')
        expect(obj.$attrs.clickEffect).assertEqual('spring-large')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuClickEffectPropNone
     * @tc.desc      ACE
     */
    it('testMenuClickEffectPropNone', 0, async function (done) {
        console.info('testMenuClickEffectPropNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.clickEffectPropNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('clickEffectPropNone')
        expect(obj.$attrs.clickEffect).assertEqual(undefined)
        console.info("[menuProps] get clickEffect value is: " + JSON.stringify(obj.$attrs.clickEffect));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuDirPropRtl
     * @tc.desc      ACE
     */
    it('testMenuDirPropRtl', 0, async function (done) {
        console.info('testMenuDirPropRtl START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropRtl);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('dirPropRtl')
        expect(obj.$attrs.dir).assertEqual('rtl')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuDirPropLtr
     * @tc.desc      ACE
     */
    it('testMenuDirPropLtr', 0, async function (done) {
        console.info('testMenuDirPropLtr START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropLtr);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('dirPropLtr')
        expect(obj.$attrs.dir).assertEqual('ltr')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuDirPropAuto
     * @tc.desc      ACE
     */
    it('testMenuDirPropAuto', 0, async function (done) {
        console.info('testMenuDirPropAuto START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropAuto);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('dirPropAuto')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuDirPropNone
     * @tc.desc      ACE
     */
    it('testMenuDirPropNone', 0, async function (done) {
        console.info('testMenuDirPropNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.dirPropNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('dirPropNone')
        expect(obj.$attrs.dir).assertEqual('auto')
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuForPropNull
     * @tc.desc      ACE
     */
    it('testMenuForPropNull', 0, async function (done) {
        console.info('testMenuForPropNull START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropNull);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('forPropNull')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[menuProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuForPropOne
     * @tc.desc      ACE
     */
    it('testMenuForPropOne', 0, async function (done) {
        console.info('testMenuForPropOne START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropOne);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('forPropOne')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[menuProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuForPropThree
     * @tc.desc      ACE
     */
    it('testMenuForPropThree', 0, async function (done) {
        console.info('testMenuForPropThree START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.forPropThree);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('forPropThree')
        expect(obj.$attrs.for).assertEqual(undefined)
        console.info("[menuProps] get for value is: " + JSON.stringify(obj.$attrs.for));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuIfPropTrue
     * @tc.desc      ACE
     */
    it('testMenuIfPropTrue', 0, async function (done) {
        console.info('testMenuIfPropTrue START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.ifPropTrue);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('ifPropTrue')
        expect(obj.$attrs.if).assertEqual(undefined)
        console.info("[menuProps] get for value is: " + JSON.stringify(obj.$attrs.if));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuShowPropTrue
     * @tc.desc      ACE
     */
    it('testMenuShowPropTrue', 0, async function (done) {
        console.info('testMenuShowPropTrue START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropTrue);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('showPropTrue')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[menuProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuShowPropFalse
     * @tc.desc      ACE
     */
    it('testMenuShowPropFalse', 0, async function (done) {
        console.info('testMenuShowPropFalse START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropFalse);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('showPropFalse')
        expect(obj.$attrs.show).assertEqual('false')
        console.info("[menuProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuShowPropNone
     * @tc.desc      ACE
     */
    it('testMenuShowPropNone', 0, async function (done) {
        console.info('testMenuShowPropNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.showPropNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('showPropNone')
        expect(obj.$attrs.show).assertEqual('true')
        console.info("[menuProps] get show value is: " + JSON.stringify(obj.$attrs.show));
        done();
    });

    //   特有属性

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuMenuTarget
     * @tc.desc      ACE
     */
    it('testMenuMenuTarget', 0, async function (done) {
        console.info('testMenuMenuTarget START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuTarget);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('menuTarget')
        expect(obj.$attrs.target).assertEqual('testTarget')
        console.info("[menuProps] get target value is: " + JSON.stringify(obj.$attrs.target));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuMenuTargetNone
     * @tc.desc      ACE
     */
    it('testMenuMenuTargetNone', 0, async function (done) {
        console.info('testMenuMenuTargetNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuTargetNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('menuTargetNone')
        expect(obj.$attrs.target).assertEqual(undefined)
        console.info("[menuProps] get target value is: " + JSON.stringify(obj.$attrs.target));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuTypeClick
     * @tc.desc      ACE
     */
    it('testMenuTypeClick', 0, async function (done) {
        console.info('testMenuTypeClick START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeClick);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('typeClick')
        expect(obj.$attrs.type).assertEqual('click')
        console.info("[menuProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuTypeLongPress
     * @tc.desc      ACE
     */
    it('testMenuTypeLongPress', 0, async function (done) {
        console.info('testMenuTypeLongPress START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeLongPress);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('typeLongPress')
        expect(obj.$attrs.type).assertEqual('longpress')
        console.info("[menuProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuTypeNone
     * @tc.desc      ACE
     */
    it('testMenuTypeNone', 0, async function (done) {
        console.info('testMenuTypeNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.typeNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('typeNone')
        expect(obj.$attrs.type).assertEqual('click')
        console.info("[menuProps] get type value is: " + JSON.stringify(obj.$attrs.type));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuMenuTitle
     * @tc.desc      ACE
     */
    it('testMenuMenuTitle', 0, async function (done) {
        console.info('testMenuMenuTitle START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuTitle);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('menuTitle')
        expect(obj.$attrs.title).assertEqual('menu')
        console.info("[menuProps] get title value is: " + JSON.stringify(obj.$attrs.title));
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMenuMenuTitleNone
     * @tc.desc      ACE
     */
    it('testMenuMenuTitleNone', 0, async function (done) {
        console.info('testMenuMenuTitleNone START');
        console.info("[menuProps] get globalThis.value is: " + JSON.stringify(globalThis.value));

        let obj = JSON.parse(globalThis.value.menuTitleNone);
        console.info("[menuProps] get inspector value is: " + JSON.stringify(obj));
        console.info("[menuProps] get inspector attrs value is: " + JSON.stringify(obj.$attrs));

        expect(obj.$type).assertEqual('menu')
        expect(obj.$attrs.id).assertEqual('menuTitleNone')
        expect(obj.$attrs.title).assertEqual(undefined)
        console.info("[menuProps] get title value is: " + JSON.stringify(obj.$attrs.title));
        done();
    });
});
