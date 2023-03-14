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

class MockStorage implements IStorage {
  private dbFileName_: string;
  private fs = require('fs');
  private data_ = {};

  /**
   * Connect to existing db or create new one
   * Note: Filename could also be managed by underlying
   * system in C++.
   * @param fileName
   */
  constructor(fileName: string) {
    this.dbFileName_ = fileName;
    const file = this.fs.readFileSync(fileName, 'utf-8')

    try {
      this.data_ = JSON.parse(file);
    } catch (e) {
      console.debug("Error reading from database! Initializing empty object");
      this.data_ = {};
    }
    console.debug(`MockStorage: database opened: ${JSON.stringify(this.data_)}`);
  }

  /**
   * retrieve property
   * @param key property name
   * @returns property value of undefined
   */
  public get<T>(key: string): T | undefined {
    console.debug(`MockStorage: get(${key}) returns ${this.data_[key]}`);
    return this.data_[key];
  }

  /**
   * Create new or update existing entry in the DB
   * @param key  property name
   * @param val  value
   */
  public set<T>(key: string, val: T): void {
    console.debug(`MockStorage: set(${key}, ${val})`);
    this.data_[key] = val;
    this.fs.writeFileSync(this.dbFileName_, JSON.stringify(this.data_));
  }

  /**
   * Delete all contents from the DB
   */
  public clear(): void {
    console.debug("MockStorage: clear()");
    this.data_ = {};
  }

  /**
   * Delete a prop from the DB
   * @param key property name
   */

  public delete(key: string): void {
    console.debug(`MockStorage: delete(${key})`);
    delete this.data_[key];
    this.fs.writeFileSync(this.dbFileName_, JSON.stringify(this.data_));
  }

  /**
   * Enumate the DB contents
   * @returns array of all property names
   */
  public keys(): string[] {
    return Object.getOwnPropertyNames(this.data_);
  }
};
