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
#ifndef HIVIEW_INCLUDE_FEATURE_H
#define HIVIEW_INCLUDE_FEATURE_H

// hiview export service
#ifdef HAS_HIVIEW_EXPORT_SERVICE
#define ENABLE_HIVIEW_EXPORT_SERVICE
#endif

// fault log management service
// this service is loaded by hiview export service
#ifdef HAS_FAULTLOGGER_PLUGIN
#define ENABLE_FAULTLOGGER_SERVICE
#endif

#endif // HIVIEW_INCLUDE_FEATURE_H