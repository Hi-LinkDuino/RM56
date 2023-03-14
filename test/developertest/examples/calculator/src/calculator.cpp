/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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


int Add(int e1, int e2)
{
    int result = e1 + e2;
    if ((e1 >= 0 && e2 >= 0 && result < 0) || (e1 < 0 && e2 < 0 && result >= 0)) {
        result = -1;
    }
    return result;
}

int Sub(int e1, int e2)
{
    int result = e1 - e2;
    return result;
}

int Mul(int e1, int e2)
{
    int result = e1 * e2;
    if (!e1) {
        return 0;
    }
    if ((result / e1) != e2) {
        result = -1;
    }
    return result;
}

int Div(int e1, int e2)
{
    if (!e2) {
        return -1;
    }

    int result = e1 / e2;
    return result;
}
