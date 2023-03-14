/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

export enum Category  {
  All,
  API,
  Scenario,
  AutoTest,
  GattAutoTest,
  SppAutoTest,
  ProfileAutoTest,
  Profile,
  BenchMark
}

let NextId = 0;
export class TestData {
  id: string;
  name: string;
  detail: string;
  url: string;
  image: Resource
  category: Category;

  constructor(name: string, detail: string, url: string, image: Resource, category: Category) {
    this.id = `${ NextId++ }`;
    this.name = name;
    this.url = url;
    this.detail = detail;
    this.image = image;
    this.category = category;
  }
}

export enum CaseCategory  {
  BR,
  BLE,
  Gatt,
  SPP,
  Profile,
  GattClient,
  GattServer,
  SPPClient,
  SPPServer,
  ProfileA2dpSource,
  ProfileHandsFreeAudioGateway,
  ProfileHitHost,
  ProfilePanNetwork,
  GattAutoTest,
  SppAutoTest,
}

export class TestApi {
  id: string;
  name: string;
  detail: string;
  method: string;
  api: any;
  result: string;
  category: CaseCategory;

  constructor(name: string, detail: string, method: string, api: any, result: string, category: CaseCategory) {
    this.id = `${ NextId++ }`;
    this.name = name;
    this.detail = detail;
    this.method = method;
    this.api = api;
    this.result = result;
    this.category = category;
  }
}

export enum AutoCategory {
  BrAuto,
  BleAuto,
  GattClientAuto,
  GattServerAuto,
  SppClientAuto,
  SppServerAuto,
  A2dpSourceAuto,
  HandsFreeAudioGatewayAuto,
  HitHostAuto,
  PanNetworkAuto
}

export class TestAuto{
  id: string;
  name: string;
  detail: string;
  method: string;
  api: any;
  image: Resource;
  result: string;
  category: AutoCategory;

  constructor(name: string, detail: string, method: string , api:any ,result: string, category: AutoCategory) {
    this.id = `${ NextId++ }`;
    this.name = name;
    this.detail = detail;
    this.method = method;
    this.api = api;
    this.result = result;
    this.category = category;
  }
}


export enum ScenarioCategory {
  BrFoundScenario,
  BleFoundScenario,
  GattClientScenario,
  GattServerScenario,
  SppClientScenario,
  SppServerScenario,
//  A2dpSourceScenario,
//  HandsFreeAudioGatewayAuto,
//  HitHostAuto,
//  PanNetworkAuto,
}

export class TestScenario{
  id: string;
  name: string;
  detail: string;
  method: string;
  api: any;
  image: Resource;
  result: string;
  category: ScenarioCategory;

  constructor(name: string, detail: string, method: string , api:any ,result: string, category: ScenarioCategory) {
    this.id = `${ NextId++ }`;
    this.name = name;
    this.detail = detail;
    this.method = method;
    this.api = api;
    this.result = result;
    this.category = category;
  }
}

export enum BenchmarkCategory {
  BrBenchmark,
  BleBenchmark,
  GattClientBenchmark,
  GattServerBenchmark,
  SppClientBenchmark,
  SppServerBenchmark,
  //  A2dpSourceBenchmark,
  //  HFPBenchmark,
  //  HitHostBenchmark,
  //  PanNetworkBenchmark,
}

export class TestBenchmark{
  id: string;
  name: string;
  detail: string;
  method: string;
  api: any;
  image: Resource;
  result: string;
  category: BenchmarkCategory;

  constructor(name: string, detail: string, method: string , api:any ,result: string, category: BenchmarkCategory) {
    this.id = `${ NextId++ }`;
    this.name = name;
    this.detail = detail;
    this.method = method;
    this.api = api;
    this.result = result;
    this.category = category;
  }
}