/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hc_file_common.h"
#include <hctest.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

char *GenerateTestingText(int size)
{
    TEST_ASSERT_LESS_OR_EQUAL(TEN_KILOBYTE, size);
    char *text = (char *)malloc(size + 1);
    if (text == NULL) {
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        text[i] = rand() % (ASCII_END - ASCII_START) + ASCII_START;
    }
    text[size] = 0;
    return text;
}

#ifdef __cplusplus
}
#endif
