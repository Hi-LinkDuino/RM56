/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import router from '@system.router';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index';


describe('aceJsTest', function () {

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
    * run after testcase
    */
    afterEach(async function () {
        console.info('[aceJsTest] after each called')
        await backToIndex();
        await sleep(5000)
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStepperComponent
     * @tc.desc      ACE
     */
    it('testStepperComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/stepper/router/index'
        }
        try {
            result = router.push(options)
            console.info("push stepper page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push stepper page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.stepper] getState" + JSON.stringify(pages));
        expect("pages/stepper/router/").assertEqual(pages.path);
        done();
    });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testButtonComponent
         * @tc.desc      ACE
         */
        it('testButtonComponent', 0, async function (done) {
            console.info('testButtonComponent START');
            let result;
            let options = {
                uri: 'pages/button/router/index'
            }
            try {
                result = router.push(options)
                console.info("push button page success " + JSON.stringify(result));
            } catch (err) {
                console.error("push button page error " + JSON.stringify(result));
            }
            await sleep(1000)
            let pages = router.getState();
            console.info("[router.button] getState" + JSON.stringify(pages));
            expect("pages/button/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0200
         * @tc.name      testChartComponent
         * @tc.desc      ACE
         */
        it('testChartComponent', 0, async function (done) {
            console.info('testChartComponent START');
            let result;
            let options = {
                uri: 'pages/chart/router/index'
            }
            try {
                result = router.push(options)
                console.info("push chart page success " + JSON.stringify(result));
            } catch (err) {
                console.error("push chart page error " + JSON.stringify(result));
            }
            await sleep(1000)
            let pages = router.getState();
            console.info("[router.chart] getState" + JSON.stringify(pages));
            expect("pages/chart/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0300
         * @tc.name      testInputComponent
         * @tc.desc      ACE
         */
        it('testInputComponent', 0, async function (done) {
            console.info('testButtonComponent START');
            let result;
            let options = {
                uri: 'pages/input/router/index'
            }
            try {
                result = router.push(options)
                console.info("push button page success " + JSON.stringify(result));
            } catch (err) {
                console.error("push button page error " + JSON.stringify(result));
            }
            await sleep(1000)
            let pages = router.getState();
            console.info("[router.input] getState" + JSON.stringify(pages));
            expect("pages/input/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0400
         * @tc.name      testSliderComponent
         * @tc.desc      ACE
         */
        it('testSliderComponent', 0, async function (done) {
            console.info('testSliderComponent START');
            let result;
            let options = {
                uri: 'pages/slider/router/index'
            }
            try {
                result = router.push(options)
                console.info("push button page success " + JSON.stringify(result));
            } catch (err) {
                console.error("push button page error " + JSON.stringify(result));
            }
            await sleep(1000)
            let pages = router.getState();
            console.info("[router.slider] getState" + JSON.stringify(pages));
            expect("pages/slider/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0400
         * @tc.name      testTextComponent
         * @tc.desc      ACE
         */
        it('testTextComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/text/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/text/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0400
         * @tc.name      testLabelComponent
         * @tc.desc      ACE
         */
        it('testLabelComponent', 0, async function (done) {
            console.info('testLabelComponent START');
            let result;
            let options = {
                uri: 'pages/label/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/label/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testMarqueeComponent
         * @tc.desc      ACE
         */
         it('testMarqueeComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/marquee/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/marquee/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testDividerComponent
         * @tc.desc      ACE
         */
        it('testDividerComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/divider/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/divider/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testGetImageComponent
         * @tc.desc      ACE
         */
        it('testGetImageComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/image/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/image/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testGetMenuComponent
         * @tc.desc      ACE
         */
        it('testGetMenuComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/menu/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/menu/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testRatingComponent
         * @tc.desc      ACE
         */
        it('testRatingComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/rating/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/rating/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testSearchComponent
         * @tc.desc      ACE
         */
        it('testSearchComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/search/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/search/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testToolbarComponent
         * @tc.desc      ACE
         */
        it('testToolbarComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/toolbar/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/toolbar/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testTextareaComponent
         * @tc.desc      ACE
         */
        it('testTextareaComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/textarea/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/textarea/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testSpanComponent
         * @tc.desc      ACE
         */
        it('testSpanComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/span/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/span/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testSwitchComponent
         * @tc.desc      ACE
         */
        it('testSwitchComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/switch/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/switch/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testToggleComponent
         * @tc.desc      ACE
         */
        it('testToggleComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/toggle/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/toggle/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testOptionComponent
         * @tc.desc      ACE
         */
        it('testOptionComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/option/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/option/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testPickerComponent
         * @tc.desc      ACE
         */
        it('testPickerComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/picker/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/picker/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testPickerViewComponent
         * @tc.desc      ACE
         */
        it('testPickerViewComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/pickerView/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/pickerView/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testPieceComponent
         * @tc.desc      ACE
         */
        it('testPieceComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/piece/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/piece/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testProgressComponent
         * @tc.desc      ACE
         */
        it('testProgressComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/progress/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/progress/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testQrcodeComponent
         * @tc.desc      ACE
         */
        it('testQrcodeComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/qrcode/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/qrcode/router/").assertEqual(pages.path);
            done();
        });

        /**
         * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
         * @tc.name      testSelectComponent
         * @tc.desc      ACE
         */
        it('testSelectComponent', 0, async function (done) {
            let result;
            let options = {
                uri: 'pages/select/router/index'
            }
            try {
                result = router.push(options)
            } catch (err) {
                result = err
            }
            await sleep(1000)
            let pages = router.getState();
            expect("pages/select/router/").assertEqual(pages.path);
            done();
        });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testBadgeComponent
     * @tc.desc      ACE
     */
    it('testBadgeComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/badge/router/index'
        }
        try {
            result = router.push(options)
            console.info("push badge page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push badge page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.badge] getState" + JSON.stringify(pages));
        expect("pages/badge/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDialogComponent
     * @tc.desc      ACE
     */
    it('testDialogComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/dialog/router/index'
        }
        try {
            result = router.push(options)
            console.info("push dialog page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push dialog page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.dialog] getState" + JSON.stringify(pages));
        expect("pages/dialog/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testDivComponent
     * @tc.desc      ACE
     */
    it('testDivComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/div/router/index'
        }
        try {
            result = router.push(options)
            console.info("push div page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push div page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.div] getState" + JSON.stringify(pages));
        expect("pages/div/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testFormComponent
     * @tc.desc      ACE
     */
    it('testFormComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/form/router/index'
        }
        try {
            result = router.push(options)
            console.info("push form page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push form page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.form] getState" + JSON.stringify(pages));
        expect("pages/form/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testListComponent
     * @tc.desc      ACE
     */
    it('testListComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/list/router/index'
        }
        try {
            result = router.push(options)
            console.info("push list page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push list page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.list] getState" + JSON.stringify(pages));
        expect("pages/list/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testListItemComponent
     * @tc.desc      ACE
     */
    it('testListItemComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/listItem/router/index'
        }
        try {
            result = router.push(options)
            console.info("push listItem page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push listItem page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.listItem] getState" + JSON.stringify(pages));
        expect("pages/listItem/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testListItemGroupComponent
     * @tc.desc      ACE
     */
    it('testListItemGroupComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/listItemGroup/router/index'
        }
        try {
            result = router.push(options)
            console.info("push listItemGroup page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push listItemGroup page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.listItemGroup] getState" + JSON.stringify(pages));
        expect("pages/listItemGroup/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPanelComponent
     * @tc.desc      ACE
     */
    it('testPanelComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/panel/router/index'
        }
        try {
            result = router.push(options)
            console.info("push panel page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push panel page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.panel] getState" + JSON.stringify(pages));
        expect("pages/panel/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPopupComponent
     * @tc.desc      ACE
     */
    it('testPopupComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/popup/router/index'
        }
        try {
            result = router.push(options)
            console.info("push popup page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push popup page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.popup] getState" + JSON.stringify(pages));
        expect("pages/popup/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testStackComponent
     * @tc.desc      ACE
     */
    it('testStackComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/stack/router/index'
        }
        try {
            result = router.push(options)
            console.info("push stack page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push stack page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.stack] getState" + JSON.stringify(pages));
        expect("pages/stack/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSwiperComponent
     * @tc.desc      ACE
     */
    it('testSwiperComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/swiper/router/index'
        }
        try {
            result = router.push(options)
            console.info("push swiper page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push swiper page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.swiper] getState" + JSON.stringify(pages));
        expect("pages/swiper/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testMarqueeComponent
     * @tc.desc      ACE
     */
    it('testMarqueeComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/marquee/router/index'
        }
        try {
            result = router.push(options)
            console.info("push marquee page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push marquee page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.marquee] getState" + JSON.stringify(pages));
        expect("pages/marquee/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRefreshComponent
     * @tc.desc      ACE
     */
    it('testRefreshComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/refresh/router/index'
        }
        try {
            result = router.push(options)
            console.info("push refresh page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push refresh page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.refresh] getState" + JSON.stringify(pages));
        expect("pages/refresh/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageAnimatorComponent
     * @tc.desc      ACE
     */
    it('testImageAnimatorComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/imageAnimator/router/index'
        }
        try {
            result = router.push(options)
            console.info("push imageAnimator page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push imageAnimator page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.imageAnimator] getState" + JSON.stringify(pages));
        expect("pages/imageAnimator/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabsComponent
     * @tc.desc      ACE
     */
    it('testTabsComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/tabs/router/index'
        }
        try {
            result = router.push(options)
            console.info("push tabs page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push tabs page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.tabs] getState" + JSON.stringify(pages));
        expect("pages/tabs/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabBarComponent
     * @tc.desc      ACE
     */
    it('testTabBarComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/tab-bar/router/index'
        }
        try {
            result = router.push(options)
            console.info("push tab-bar page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push tab-bar page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.tab-bar] getState" + JSON.stringify(pages));
        expect("pages/tab-bar/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTabContentComponent
     * @tc.desc      ACE
     */
    it('testTabContentComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/tab-content/router/index'
        }
        try {
            result = router.push(options)
            console.info("push tab-content page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push tab-content page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.tab-content] getState" + JSON.stringify(pages));
        expect("pages/tab-content/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testVideoComponent
     * @tc.desc      ACE
     */
    it('testVideoComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/video/router/index'
        }
        try {
            result = router.push(options)
            console.info("push video page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push video page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.video] getState" + JSON.stringify(pages));
        expect("pages/video/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgTextComponent
     * @tc.desc      ACE
     */
    it('testSvgTextComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/svg_text/router/index'
        }
        try {
            result = router.push(options)
            console.info("push svg_text page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push svg_text page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.svg_text] getState" + JSON.stringify(pages));
        expect("pages/svg_text/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testGridContainerComponent
     * @tc.desc      ACE
     */
    it('testGridContainerComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/gridContainer/router/index'
        }
        try {
            result = router.push(options)
            console.info("push gridContainer page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push gridContainer page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.gridContainer] getState" + JSON.stringify(pages));
        expect("pages/gridContainer/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testGridRowComponent
     * @tc.desc      ACE
     */
    it('testGridRowComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/gridRow/router/index'
        }
        try {
            result = router.push(options)
            console.info("push gridRow page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push gridRow page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.gridRow] getState" + JSON.stringify(pages));
        expect("pages/gridRow/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testGridColComponent
     * @tc.desc      ACE
     */
    it('testGridColComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/gridCol/router/index'
        }
        try {
            result = router.push(options)
            console.info("push gridCol page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push gridCol page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.gridCol] getState" + JSON.stringify(pages));
        expect("pages/gridCol/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCanvasComponent
     * @tc.desc      ACE
     */
    it('testCanvasComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/canvas/router/index'
        }
        try {
            result = router.push(options)
            console.info("push canvas page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push canvas page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.canvas] getState" + JSON.stringify(pages));
        expect("pages/canvas/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCanvasRenderingContext2DComponent
     * @tc.desc      ACE
     */
    it('testCanvasRenderingContext2DComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_CanvasRenderingContext2D/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_CanvasRenderingContext2D page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_CanvasRenderingContext2D page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_CanvasRenderingContext2D] getState" + JSON.stringify(pages));
        expect("pages/obj_CanvasRenderingContext2D/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testObjImageComponent
     * @tc.desc      ACE
     */
    it('testObjImageComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_Image/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_Image page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_Image page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_Image] getState" + JSON.stringify(pages));
        expect("pages/obj_Image/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCanvasGradientComponent
     * @tc.desc      ACE
     */
    it('testCanvasGradientComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_CanvasGradient/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_CanvasGradient page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_CanvasGradient page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_CanvasGradient] getState" + JSON.stringify(pages));
        expect("pages/obj_CanvasGradient/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageDataComponent
     * @tc.desc      ACE
     */
    it('testImageDataComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_ImageData/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_ImageData page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_ImageData page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_ImageData] getState" + JSON.stringify(pages));
        expect("pages/obj_ImageData/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPath2DComponent
     * @tc.desc      ACE
     */
    it('testPath2DComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_Path2D/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_Path2D page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_Path2D page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_Path2D] getState" + JSON.stringify(pages));
        expect("pages/obj_Path2D/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOffCanvasRC2DComponent
     * @tc.desc      ACE
     */
    it('testOffCanvasRC2DComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_OffscreenCanvasRenderingContext2D/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_OffscreenCanvasRenderingContext2D page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_OffscreenCanvasRenderingContext2D page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_OffscreenCanvasRenderingContext2D] getState" + JSON.stringify(pages));
        expect("pages/obj_OffscreenCanvasRenderingContext2D/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testImageBitmapComponent
     * @tc.desc      ACE
     */
    it('testImageBitmapComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_ImageBitmap/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_ImageBitmap page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_ImageBitmap page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_ImageBitmap] getState" + JSON.stringify(pages));
        expect("pages/obj_ImageBitmap/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testOffScreenCanvasComponent
     * @tc.desc      ACE
     */
    it('testOffScreenCanvasComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/obj_OffscreenCanvas/router/index'
        }
        try {
            result = router.push(options)
            console.info("push obj_OffscreenCanvas page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push obj_OffscreenCanvas page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.obj_OffscreenCanvas] getState" + JSON.stringify(pages));
        expect("pages/obj_OffscreenCanvas/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testSvgComponent
     * @tc.desc      ACE
     */
    it('testSvgComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/svg/router/index'
        }
        try {
            result = router.push(options)
            console.info("push svg page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push svg page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.svg] getState" + JSON.stringify(pages));
        expect("pages/svg/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testRectComponent
     * @tc.desc      ACE
     */
    it('testRectComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/rect/router/index'
        }
        try {
            result = router.push(options)
            console.info("push rect page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push rect page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.rect] getState" + JSON.stringify(pages));
        expect("pages/rect/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testCircleComponent
     * @tc.desc      ACE
     */
    it('testCircleComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/circle/router/index'
        }
        try {
            result = router.push(options)
            console.info("push circle page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push circle page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.circle] getState" + JSON.stringify(pages));
        expect("pages/circle/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testEllipseComponent
     * @tc.desc      ACE
     */
    it('testEllipseComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/ellipse/router/index'
        }
        try {
            result = router.push(options)
            console.info("push ellipse page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push ellipse page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.ellipse] getState" + JSON.stringify(pages));
        expect("pages/ellipse/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPathComponent
     * @tc.desc      ACE
     */
    it('testPathComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/path/router/index'
        }
        try {
            result = router.push(options)
            console.info("push path page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push path page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.path] getState" + JSON.stringify(pages));
        expect("pages/path/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testLineComponent
     * @tc.desc      ACE
     */
    it('testLineComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/line/router/index'
        }
        try {
            result = router.push(options)
            console.info("push line page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push line page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.line] getState" + JSON.stringify(pages));
        expect("pages/line/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolyLineComponent
     * @tc.desc      ACE
     */
    it('testPolyLineComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/polyline/router/index'
        }
        try {
            result = router.push(options)
            console.info("push polyline page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push polyline page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.polyline] getState" + JSON.stringify(pages));
        expect("pages/polyline/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testPolygonComponent
     * @tc.desc      ACE
     */
    it('testPolygonComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/polygon/router/index'
        }
        try {
            result = router.push(options)
            console.info("push polygon page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push polygon page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.polygon] getState" + JSON.stringify(pages));
        expect("pages/polygon/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateComponent
     * @tc.desc      ACE
     */
    it('testAnimateComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/animate/router/index'
        }
        try {
            result = router.push(options)
            console.info("push animate page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push animate page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.animate] getState" + JSON.stringify(pages));
        expect("pages/animate/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateMotionComponent
     * @tc.desc      ACE
     */
    it('testAnimateMotionComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/animateMotion/router/index'
        }
        try {
            result = router.push(options)
            console.info("push animateMotion page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push animateMotion page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.animateMotion] getState" + JSON.stringify(pages));
        expect("pages/animateMotion/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testAnimateTransformComponent
     * @tc.desc      ACE
     */
    it('testAnimateTransformComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/animateTransform/router/index'
        }
        try {
            result = router.push(options)
            console.info("push animateTransform page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push animateTransform page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.animateTransform] getState" + JSON.stringify(pages));
        expect("pages/animateTransform/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTextPathComponent
     * @tc.desc      ACE
     */
    it('testTextPathComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/textPath/router/index'
        }
        try {
            result = router.push(options)
            console.info("push textPath page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push textPath page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.textPath] getState" + JSON.stringify(pages));
        expect("pages/textPath/router/").assertEqual(pages.path);
        done();
    });

    /**
     * @tc.number    SUB_ACE_BASIC_COMPONENT_JS_API_0100
     * @tc.name      testTspanComponent
     * @tc.desc      ACE
     */
    it('testTspanComponent', 0, async function (done) {
        let result;
        let options = {
            uri: 'pages/tspan/router/index'
        }
        try {
            result = router.push(options)
            console.info("push tspan page success " + JSON.stringify(result));
        } catch (err) {
            console.error("push tspan page error " + JSON.stringify(result));
        }
        await sleep(5000)
        let pages = router.getState();
        console.info("[router.tspan] getState" + JSON.stringify(pages));
        expect("pages/tspan/router/").assertEqual(pages.path);
        done();
    });
});
