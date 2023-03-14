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

#include "lazy_load_watcher.h"
namespace OHOS {
namespace ACELite {
LazyLoadWatcher::LazyLoadWatcher(jerry_value_t nativeElement,
                                 jerry_value_t attrName,
                                 jerry_value_t getter,
                                 uint16_t keyId)
    : nativeElement_(nativeElement),
      attrName_(jerry_acquire_value(attrName)),
      getter_(jerry_acquire_value(getter)),
      keyId_(keyId)
{
}

LazyLoadWatcher::~LazyLoadWatcher()
{
    jerry_release_value(attrName_);
    jerry_release_value(getter_);
}
} // namespace ACELite
} // namespace OHOS
