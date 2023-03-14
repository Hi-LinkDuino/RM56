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

#ifndef HC_THREAD_MUTEX_SHARE_H
#define HC_THREAD_MUTEX_SHARE_H

#define REQUIRED_THREAD_COUNT 5
#define MAX_THREAD_STACK_SIZE 4096
#define MIN_THREAD_STACK_SIZE 4096

enum {
    WAIT_IN_MUTEX_SECONDS_MIN = 3,
    WAIT_IN_MUTEX_SECONDS_MAX = 5,
};

#endif // HC_THREAD_MUTEX_SHARE_H
