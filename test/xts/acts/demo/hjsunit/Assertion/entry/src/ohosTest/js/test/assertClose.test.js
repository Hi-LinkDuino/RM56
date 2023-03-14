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
import {describe, it, expect} from 'deccjsunit/index'

describe('assertClose', function () {
    it('assertCloseSuccess001', 0, function () {
        let a = 100;
        let b = 0.1
        expect(a).assertClose(99, b)
    })
    it('assertCloseFail002', 0, function () {
        let a = 100;
        let b = 0.1
        expect(a).assertClose(1, b)
    })
    it('assertCloseFail003', 0, function () {
        let a = 100;
        let b = 0.1
        expect(a).assertClose(null, b)
    })
    it('assertCloseFail004', 0, function () {
        expect(null).assertClose(null, 0)
    })
})