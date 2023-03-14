/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup UI_Utils
 * @{
 *
 * @brief Defines basic UI utils.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file graphic_assert.h
 *
 * @brief Declares the assertion utility for the graphics module.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_GRAPHIC_ASSERT_H
#define GRAPHIC_LITE_GRAPHIC_ASSERT_H

#include "graphic_config.h"

#if ENABLE_DEBUG
#include <assert.h>
#endif

namespace OHOS {
#if ENABLE_DEBUG
#define ASSERT(cond) assert(cond)
#else
#define ASSERT(cond)
#endif
} // namespace OHOS
#endif // GRAPHIC_LITE_GRAPHIC_ASSERT_H
