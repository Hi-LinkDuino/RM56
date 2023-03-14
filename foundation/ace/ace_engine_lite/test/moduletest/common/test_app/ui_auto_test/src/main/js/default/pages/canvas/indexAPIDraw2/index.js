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

import { routePage, routeSimplePage } from "../../../common/js/general";
export default {
  ...routePage("pages/index/index"),
  changePage1: routeSimplePage("pages/canvas/indexAPIBeginPath/index"),
  changePage2: routeSimplePage("pages/canvas/indexAPIRect/index"),
  changePage3: routeSimplePage("pages/canvas/indexAPIStroke/index"),
  changePage4: routeSimplePage("pages/canvas/indexAPIMoveTo/index"),
  changePage5: routeSimplePage("pages/canvas/indexAPILineTo/index"),
  changePage6: routeSimplePage("pages/canvas/indexAPIArc/index"),
  changePage7: routeSimplePage("pages/canvas/indexAPIClosePath/index")
};
