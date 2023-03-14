/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import accessibility from '@ohos.accessibility'

describe('AccessibleCheckAbility', function () {
    const TIMEOUT = 1000;

    beforeEach(async function (done) {
        console.info(`AccessibleAbilityList: beforeEach starts`);
        done();
    })

    afterEach(async function (done) {
        console.info(`AccessibleAbilityList: afterEach starts`);
        setTimeout(done, TIMEOUT);
    })


    /******************************************************************************************* */
    /* Cases CheckAbilityIsOpen_0170-0180 are for accessibility.isOpenAccessibility() API test   */
    /******************************************************************************************* */

    /* Cases for namespace accessibility */
    /*
    * @tc.number  CheckAbilityIsOpen_0170
    * @tc.name    CheckAbilityIsOpen_0170
    * @tc.desc    Test isOpenAccessibility() function in callback mode.
    *             The result of isOpenAccessibility() should be boolean type.
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('CheckAbilityIsOpen_0170', 0, async function (done) {
        console.info('CheckAbilityIsOpen_0170');
        accessibility.isOpenAccessibility((err, data) => {
            if (err.code != 0) {
                console.error(`AccessibilityApi: CheckAbilityIsOpen_0170 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            expect(typeof (data)).assertEqual('boolean');
            done();
        });
      })
  
    /*
      * @tc.number  CheckAbilityIsOpen_0180
      * @tc.name    CheckAbilityIsOpen_0180
      * @tc.desc    Test isOpenAccessibility() function in promise mode.
      *             The result of isOpenAccessibility() should be boolean type.
      * @tc.size    SmallTest
      * @tc.type    User
      */
    it('CheckAbilityIsOpen_0180', 0, async function (done) {
        console.info('CheckAbilityIsOpen_0180');
        accessibility.isOpenAccessibility().then((result) => {
            console.info(`AccessibilityApi: CheckAbilityIsOpen_0180 result: ${result}`);
            expect(typeof (result)).assertEqual('boolean');
            done();
        }).catch((err) => {
            console.error(`AccessibilityApi: CheckAbilityIsOpen_0180 has error: ${err.code}`);
            expect(null).assertFail();
            done();
        });
    })
  
    /******************************************************************************************* */
    /* Cases CheckAbilityIsOpen_0190-0200 are for accessibility.isOpenTouchGuide() API test   */
    /******************************************************************************************* */

    /*
      * @tc.number  CheckAbilityIsOpen_0190
      * @tc.name    CheckAbilityIsOpen_0190
      * @tc.desc    Test isOpenTouchGuide() function in callback mode.
      *             The result of isOpenTouchGuide() should be boolean type.
      * @tc.size    SmallTest
      * @tc.type    User
      */
    it('CheckAbilityIsOpen_0190', 0, async function (done) {
        console.info('CheckAbilityIsOpen_0190');
        accessibility.isOpenTouchGuide((err, data) => {
            if (err.code != 0) {
                console.error(`AccessibilityApi: CheckAbilityIsOpen_0190 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            expect(typeof (data)).assertEqual('boolean');
            done();
        });
    })
  
    /*
      * @tc.number  CheckAbilityIsOpen_0200
      * @tc.name    CheckAbilityIsOpen_0200
      * @tc.desc    Test isOpenTouchGuide() function in promise mode.
      *             The result of isOpenTouchGuide() should be boolean type.
      * @tc.size    SmallTest
      * @tc.type    User
      */
    it('CheckAbilityIsOpen_0200', 0, async function (done) {
        console.info('CheckAbilityIsOpen_0200');
        accessibility.isOpenTouchGuide().then((result) => {
            console.info(`AccessibilityApi: CheckAbilityIsOpen_0200 result: ${result}`);
            expect(typeof (result)).assertEqual('boolean');
            done();
        }).catch((err) => {
            console.error(`AccessibilityApi: CheckAbilityIsOpen_0200 has error: ${err.code}`);
            expect(null).assertFail();
            done();
        });
    })
})

