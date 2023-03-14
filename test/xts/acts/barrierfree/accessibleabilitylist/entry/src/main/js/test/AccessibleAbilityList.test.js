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
const abilityType = 'all';
const abilityState = 'install';

describe('AccessibleAbilityList', function () {

    beforeAll(async function (done) {
        console.info(`AccessibleAbilityList: beforeAll starts`);
        setTimeout(done, 30000);
    })

    beforeEach(async function (done) {
        console.info(`AccessibleAbilityList: beforeEach starts`);
        done();
    })

    afterEach(async function (done) {
        console.info(`AccessibleAbilityList: afterEach starts`);
        setTimeout(done, 1000);
    })

    /******************************************************************************** */
    /* Cases AbilityList_0210-0460 are for accessibility.getAbilityLists() API test   */
    /******************************************************************************** */

    /*
    * @tc.number  AbilityList_0210
    * @tc.name    AbilityList_0210
    * @tc.desc    The parameters input are 'audible' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0210', 0, async function (done) {
        console.info('AbilityList_0210');
        let abilityType = 'audible';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0210 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0210 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0220
    * @tc.name    AbilityList_0220
    * @tc.desc    The parameters input are 'generic' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0220', 0, async function (done) {
        console.info('AbilityList_0220');
        let abilityType = 'generic';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0220 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0220 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0230
    * @tc.name    AbilityList_0230
    * @tc.desc    The parameters input are 'haptic' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0230', 0, async function (done) {
        console.info('AbilityList_0230');
        let abilityType = 'haptic';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0230 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0230 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0240
    * @tc.name    AbilityList_0240
    * @tc.desc    The parameters input are 'spoken' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0240', 0, async function (done) {
        console.info('AbilityList_0240');
        let abilityType = 'spoken';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0240 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0240 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0250
    * @tc.name    AbilityList_0250
    * @tc.desc    The parameters input are 'visual' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0250', 0, async function (done) {
        console.info('AbilityList_0250');
        let abilityType = 'visual';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0250 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0250 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0260
    * @tc.name    AbilityList_0260
    * @tc.desc    The parameters input are 'all' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0260', 0, async function (done) {
        console.info('AbilityList_0260');
        let abilityType = 'all';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0260 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0260 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0270
    * @tc.name    AbilityList_0270
    * @tc.desc    The parameters input are '' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0270', 0, async function (done) {
        console.info('AbilityList_0270');
        let abilityType = '';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0270 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0270 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0280
    * @tc.name    AbilityList_0280
    * @tc.desc    The parameters input are null and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0280', 0, async function (done) {
        console.info('AbilityList_0280');
        let abilityType = null;
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0280 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0280 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0290
    * @tc.name    AbilityList_0290
    * @tc.desc    The parameters input are 'all' and 'enable', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
     it('AbilityList_0290', 0, async function (done) {
        console.info('AbilityList_0290');
        let abilityState = 'enable';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0290 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0290 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0300
    * @tc.name    AbilityList_0300
    * @tc.desc    The parameters input are 'all' and 'disable', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0300', 0, async function (done) {
        console.info('AbilityList_0300');
        let abilityState = 'disable';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0300 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0300 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0310
    * @tc.name    AbilityList_0310
    * @tc.desc    The parameters input are 'all' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0310', 0, async function (done) {
        console.info('AbilityList_0310');
        let abilityState = 'install';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0310 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0310 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0320
    * @tc.name    AbilityList_0320
    * @tc.desc    The parameters input are 'all' and '', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0320', 0, async function (done) {
        console.info('AbilityList_0320');
        let abilityState = '';
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0320 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0320 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0330
    * @tc.name    AbilityList_0330
    * @tc.desc    The parameters input are 'all' and null, test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0330', 0, async function (done) {
        console.info('AbilityList_0330');
        let abilityState = null;
        accessibility.getAbilityLists(abilityType, abilityState, (err, data) => {
            if (err.code != 0) {
                console.error(`AccessibleAbilityList: AbilityList_0330 has error: ${err.code}`);
                expect(null).assertFail();
                done();
            }
            console.info(`AccessibleAbilityList: AbilityList_0330 result ${data.length}`);
            expect(Array.isArray(data)).assertEqual(true);
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0340
    * @tc.name    AbilityList_0340
    * @tc.desc    The parameters input are 'audible' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0340', 0, async function (done) {
        console.info('AbilityList_0340');
        let abilityType = 'audible';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0340 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0340 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

     /*
    * @tc.number  AbilityList_0350
    * @tc.name    AbilityList_0350
    * @tc.desc    The parameters input are 'generic' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
     it('AbilityList_0350', 0, async function (done) {
        console.info('AbilityList_0350');
        let abilityType = 'generic';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0350 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0350 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0360
    * @tc.name    AbilityList_0360
    * @tc.desc    The parameters input are 'haptic' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0360', 0, async function (done) {
        console.info('AbilityList_0360');
        let abilityType = 'haptic';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0360 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0360 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

     /*
    * @tc.number  AbilityList_0370
    * @tc.name    AbilityList_0370
    * @tc.desc    The parameters input are 'spoken' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
     it('AbilityList_0370', 0, async function (done) {
        console.info('AbilityList_0370');
        let abilityType = 'spoken';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0370 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0370 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0380
    * @tc.name    AbilityList_0380
    * @tc.desc    The parameters input are 'visual' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0380', 0, async function (done) {
        console.info('AbilityList_0380');
        let abilityType = 'visual';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0380 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0380 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

     /*
    * @tc.number  AbilityList_0390
    * @tc.name    AbilityList_0390
    * @tc.desc    The parameters input are 'all' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
     it('AbilityList_0390', 0, async function (done) {
        console.info('AbilityList_0390');
        let abilityType = 'all';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0390 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0390 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0400
    * @tc.name    AbilityList_0400
    * @tc.desc    The parameters input are '' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0400', 0, async function (done) {
        console.info('AbilityList_0400');
        let abilityType = '';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0400 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0400 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0410
    * @tc.name    AbilityList_0410
    * @tc.desc    The parameters input are null and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
     it('AbilityList_0410', 0, async function (done) {
        console.info('AbilityList_0410');
        let abilityType = null;
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0410 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0410 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0420
    * @tc.name    AbilityList_0420
    * @tc.desc    The parameters input are 'all' and 'enable', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0420', 0, async function (done) {
        console.info('AbilityList_0420');
        let abilityState = 'enable';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0420 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0420 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0430
    * @tc.name    AbilityList_0430
    * @tc.desc    The parameters input are 'all' and 'disable', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0430', 0, async function (done) {
        console.info('AbilityList_0430');
        let abilityState = 'disable';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0430 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0430 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0440
    * @tc.name    AbilityList_0440
    * @tc.desc    The parameters input are 'all' and 'install', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0440', 0, async function (done) {
        console.info('AbilityList_0440');
        let abilityState = 'install';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0440 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0440 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0450
    * @tc.name    AbilityList_0450
    * @tc.desc    The parameters input are 'all' and '', test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0450', 0, async function (done) {
        console.info('AbilityList_0450');
        let abilityState = '';
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0450 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0450 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  AbilityList_0460
    * @tc.name    AbilityList_0460
    * @tc.desc    The parameters input are 'all' and null, test the getAbilityLists() function,
    *             and the output is the list of AccessibilityAbilityInfo
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('AbilityList_0460', 0, async function (done) {
        console.info('AbilityList_0460');
        let abilityState = null;
        accessibility.getAbilityLists(abilityType, abilityState).then((result) => {
            console.info(`AccessibleAbilityList: AbilityList_0460 result ${result.length}`);
            expect(Array.isArray(result)).assertEqual(true);
            done();
        }).catch((err) => {
            console.error(`AccessibleAbilityList: AbilityList_0460 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })
})

