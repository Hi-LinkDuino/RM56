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

class MockEnvironmentBackend implements IEnvironmentBackend {
  private callback_: (key: string, value: any) => void = undefined;
  getAccessibilityEnabled(): string {
    return "false";
  }
  getColorMode(): number {
    return 1;
  }
  getFontScale(): number {
    return 1.0;
  }
  getFontWeightScale(): number {
    return 1.0;
  }
  getLayoutDirection(): string {
    return "LTR";
  }
  getLanguageCode(): string {
    return "cn";
  }

  simulateChange<S>(key: string, value: S): void {
    if (!this.callback_)
      return;
    this.callback_(key, value);
  }

  onValueChanged(callback: (key: string, value: any) => void): void {
    this.callback_ = callback;
  }
}
