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
import router from '@system.router';

const frames = [
  { src: '/common/A094_051.png' },
  { src: '/common/A094_052.png' },
  { src: '/common/A094_053.png' },
  { src: '/common/A094_054.png' },
  { src: '/common/A094_055.png' },
  { src: '/common/A094_056.png' },
  { src: '/common/A094_057.png' },
  { src: '/common/A094_058.png' },
  { src: '/common/A094_059.png' },
  { src: '/common/A094_060.png' },
  { src: '/common/A094_061.png' },
  { src: '/common/A094_062.png' },
  { src: '/common/A094_063.png' },
  { src: '/common/A094_064.png' },
  { src: '/common/A094_065.png' },
  { src: '/common/A094_066.png' },
  { src: '/common/A094_067.png' },
];

const framesSize = frames.length;
const MILLIONS_PER_TICK = 30;
const duration = framesSize * MILLIONS_PER_TICK;

export default {
  data: {
    frames,
    duration
  },
  quick() {
    this.duration -= duration;
  },
  slow() {
    this.duration += duration;
  },
  set0() {
    this.duration = 0;
  },
  set1() {
    this.duration = 1;
  },
  setA() {
    this.duration = 'A';
  },
  goHome() {
    router.replace({ uri: "/" });
  }
}
