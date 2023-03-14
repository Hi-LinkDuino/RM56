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

#ifndef CLIB_ERROR_H
#define CLIB_ERROR_H

enum {
    CLIB_SUCCESS = 0,
    CLIB_FAILED = -1,
    CLIB_ERR_INVALID_PARAM = -2,
    CLIB_ERR_INVALID_LEN = -3,
    CLIB_ERR_BAD_ALLOC = -4,
    CLIB_ERR_NULL_PTR = -5,
    CLIB_ERR_JSON_GET = -6,
    CLIB_ERR_JSON_ADD = -7,
    CLIB_ERR_JSON_REPLACE = -8,
    CLIB_ERR_JSON_DUPLICATE = -9,
};
#endif
