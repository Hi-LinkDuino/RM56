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
export const setResource = function() {
  for (var i = 0; i < 50; i++) {
    this.listData.push("common/left.png");
    this.listData.push("common/right.png");
    this.listData.push("common/music_prev.png");
    this.listData.push("common/music_next.png");
    this.listData.push("common/music_play.png");
    this.listData.push("common/goAhead.png");
  }
};
export const listData = [
  "common/left.png",
  "common/right.png",
  "common/music_prev.png",
  "common/music_next.png",
  "common/music_play.png",
  "common/goAhead.png"
];
export const textData = ["Item0", "Item1", "Item2", "Item3"];