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

#ifndef UTILS_BASE_PUBDEF_H
#define UTILS_BASE_PUBDEF_H

namespace OHOS {

#define B_TO_M(b) ((b) / 1024 / 1024)
#define M_TO_B(m) ((m) * 1024 * 1024)
#define B_TO_K(b) ((b) / 1024)
#define K_TO_B(m) ((m) * 1024)

#define HOUR_TO_SECOND(h) ((h) * 3600)
#define SECOND_TO_HOUR(s) ((s) / 3600)

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a[0])))

#define FREE_AND_NIL(p) do { if (p) { delete (p); (p) = nullptr; } } while(0)
#define FREE_AND_NIL_ARRAY(p) do { if (p) { delete[] (p); (p) = nullptr; } } while(0)

#define MAX(x, y)  (((x) > (y)) ? (x) : (y))
#define MIN(x, y)  (((x) < (y)) ? (x) : (y))

#define EPS (1e-8)
#define EQUAL_TO_ZERO(x) (fabs(x) <= (EPS))

// Used to retry syscalls that can return EINTR.
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp)            \
    ({                                     \
    decltype(exp) _rc;                     \
    do {                                   \
        _rc = (exp);                       \
    } while ((_rc == -1) && (errno == EINTR)); \
    _rc;                                   \
    })
#endif
}

#endif

