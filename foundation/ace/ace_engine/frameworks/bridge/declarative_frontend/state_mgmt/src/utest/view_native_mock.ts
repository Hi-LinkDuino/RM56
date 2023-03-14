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

abstract class NativeView {

  constructor(compilerAssignedUniqueChildId: string, parent?: View) {
    console.log(`${this.constructor.name}: new instance, child of parent ${parent && parent.id__()}:${parent && parent.constructor.name}`);
  }

  protected markNeedUpdate(): void {
    console.log(`${this.id__()}:${this.constructor.name}: markNeedUpdate`);
  }

  abstract id__(): void;

  findChildById(compilerAssignedUniqueChildId: string): View {
    console.log(`${this.id__()}:${this.constructor.name}: findChildById ${compilerAssignedUniqueChildId}. Will not work!`);
    return undefined;
  }

  static create(newView: View): void {
    console.error("View.create - unimplemented");
  }

}
