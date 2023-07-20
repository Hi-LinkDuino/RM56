/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
 * @syscap SystemCapability.Communication.Bluetooth.Lite
 * @since 6
 */
 export default class Bluetooth {
  
  static bindResponse(data: string, callback: Callback): void;

  static bindNicknameResponse(data: string, callback: Callback): void;

  static readBindCode(): void;

  static readBindStatus(): void;

  static readPayCode(): void;

  static readUserInfo(): void;

  static readNickname():void;

}
