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

 # How to contribute to the runtime-core of JS framework
 ## 1. Modify code
 ## 2. run the build-core.sh file
 > cd ace_engine_lite/frameworks/packages/runtime-core/
 > ./build_core.sh

 ## 3. add following two C header files to git and upload them to your PR
 > git add src/core/base/framework_min_js.h
 > git add src/core/base/framework_min_bc.h
