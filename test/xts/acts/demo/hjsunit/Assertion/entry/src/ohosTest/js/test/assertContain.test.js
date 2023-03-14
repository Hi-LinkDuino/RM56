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

describe('assertContain', function () {
    it('assertContainSuccessOnString', 0, function () {
        let a = 'abc';
        let b = 'b'
        expect(a).assertContain(b)
    })
    it('assertContainSuccessOnArray', 0, function () {
        let a = ['a', 'b', 'c'];
        let b = 'b'
        expect(a).assertContain(b)
    })
    it('assertContainFailOnString', 0, function () {
        let a = 'abc';
        let b = 'd'
        expect(a).assertContain(b)
    })
    it('assertContainFailOnArray', 0, function () {
        let a = ['a', 'b', 'c'];
        let b = 'd'
        expect(a).assertContain(b)
    })
})