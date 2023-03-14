/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INPUT_HDI_TEST_H
#define INPUT_HDI_TEST_H

const int INIT_DEFAULT_VALUE = 255;
const int KEEP_ALIVE_TIME_MS = 5000;
const int TOUCH_INDEX = 1;
const int MAX_DEVICES = 32;
const int INVALID_INDEX = 5;

#define INPUT_CHECK_NULL_POINTER(pointer, ret) do { \
    if ((pointer) == nullptr) { \
        printf("%s: null pointer", __func__); \
        ASSERT_EQ ((ret), INPUT_SUCCESS); \
    } \
} while (0)

#endif