/**
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// sim slot id
export const DEFAULT_SLOTID = 0;
export const SLOTID3 = 3;
export const SLOTID2 = 2;
export const SLOTID_MINUS1 = -1;

// sim correct password
export const CORRECT_PIN = '1234';
export const CORRECT_PIN2 = '18099014';

// sim incorrect password
export const INCORRECT_PIN = '134679';
export const INCORRECT_PIN_LEN3 = '123';
export const INCORRECT_PUK = '134679';
export const INCORRECT_PIN2 = '134679';
export const INCORRECT_PIN2_LEN3 = '123';
export const INCORRECT_PIN2_LEN9 = '123456789';
export const INCORRECT_PUK2 = '134679';

// lock status response
export const LOCK_RESULT = { SUCCESS: 0, FAILURE: -1, EXCEPTION: -2 };