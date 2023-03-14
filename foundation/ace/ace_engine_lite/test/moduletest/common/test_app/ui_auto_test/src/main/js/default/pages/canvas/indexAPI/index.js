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

import { goPage, routeSimplePage } from "../../../common/js/general";
export default {
  ...goPage("pages/index/index"),
  changePage1: routeSimplePage("pages/canvas/indexAPIAttr1/index"),
  changePage2: routeSimplePage("pages/canvas/indexAPIAttr2/index"),
  changePage3: routeSimplePage("pages/canvas/indexAPIDraw1/index"),
  changePage4: routeSimplePage("pages/canvas/indexAPIDraw2/index"),
  changePage5: routeSimplePage("pages/canvas/indexAPIPerformance/index"),
  changePage6: routeSimplePage("pages/canvas/indexAPIScenario/index")
};
