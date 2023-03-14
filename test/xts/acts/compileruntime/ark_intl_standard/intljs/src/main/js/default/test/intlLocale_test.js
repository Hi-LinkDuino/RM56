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
import Intl from '@ohos.intl';
import {describe,beforeAll,beforeEach,afterEach,afterAll,it,expect}from 'deccjsunit/index'
describe('intlDateTimeFormatTest', function () {
    console.log('*******  start IntlLocale_Test ******')

    /* *
    * @tc.number ARK_INTL_JS_DATETIME_001
    * @tc.name  the locale in  en-Latn-GB
    * @tc.desc check the locale in en-Latn-GB
    */
    it('locale_test_001', 0, function () {
        let locale = new Intl.Locale('en-Latn-GB');
        console.log('locale_test_001 ' + locale.language);
        expect(locale.language).assertEqual('en');
    })

})