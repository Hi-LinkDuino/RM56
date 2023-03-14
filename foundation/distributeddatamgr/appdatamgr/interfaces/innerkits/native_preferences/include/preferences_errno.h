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

#ifndef PREFERENCES_ERRNO_H
#define PREFERENCES_ERRNO_H

#include <errno.h>

namespace OHOS {
namespace NativePreferences {
constexpr int E_OK = 0;
constexpr int E_BASE = -1000; // different from the other errno.
constexpr int E_ERROR = (E_BASE - 1);
constexpr int E_STALE = (E_BASE - 2); // Resource has been stopped, killed or destroyed.
constexpr int E_INVALID_ARGS = (E_BASE - 3); // the input args is invalid.
constexpr int E_OUT_OF_MEMORY = (E_BASE - 4); // out of memory
constexpr int E_NOT_PERMIT = (E_BASE - 5); // operation is not permitted
constexpr int E_KEY_EMPTY = (E_BASE - 6);
constexpr int E_KEY_EXCEED_MAX_LENGTH = (E_BASE - 7);
constexpr int E_PTR_EXIST_ANOTHER_HOLDER = (E_BASE - 8);
constexpr int E_DELETE_FILE_FAIL = (E_BASE - 9);
constexpr int E_EMPTY_FILE_PATH = (E_BASE - 10);
constexpr int E_RELATIVE_PATH = (E_BASE - 11);
constexpr int E_EMPTY_FILE_NAME = (E_BASE - 12);
constexpr int E_INVALID_FILE_PATH = (E_BASE - 13);
constexpr int E_PATH_EXCEED_MAX_LENGTH = (E_BASE - 14);
constexpr int E_VALUE_EXCEED_MAX_LENGTH = (E_BASE - 15);
constexpr int E_KEY_EXCEED_LENGTH_LIMIT = (E_BASE - 16);
constexpr int E_VALUE_EXCEED_LENGTH_LIMIT = (E_BASE - 17);
} // namespace NativePreferences
} // namespace OHOS
#endif // PREFERENCES_ERRNO_H
