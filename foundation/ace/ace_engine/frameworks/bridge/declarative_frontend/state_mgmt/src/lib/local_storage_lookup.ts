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

type LocalStoragePath = string;

class LocalStorageLookup {

    /** 
     * Factory method for creating a new LocalStorageInstance
     * creates and initializes a new instance if 'path' is unused.
     * otherwise returns existing instance (no re-initialization)
    *
    *  Initialization of a new instance: 
    *  Initialzies the new instance with all properties and their values that Object.keys(params) returns
     * Property values must not be undefined.
   */
    public static GetOrCreate(path: LocalStoragePath, initializingProperties: Object = { }): LocalStorage {
        return LocalStorageLookup.GetOrCreateSingleton().getOrCreate(path, initializingProperties);
    }

    /**
     * Add an existing instance of LocalStorage 
     * Assign it and map it eother under stated path, or the path property of the LocalStorage instance
     * @param existingStorage 
     * @path path
     * @returns true if successfully added, return s false and does nothing if 
     * LocalStorage instance with chosen path already in LocalStorageLookup
     */
    public static Add(path: LocalStoragePath, existingStorage : LocalStorage) : boolean {
        return LocalStorageLookup.GetOrCreateSingleton().add(path, existingStorage);
    }

    /** 
     * Orderly delete a LocalStorge instance before its owning Ability, View, or service UI goes out of scope.
     * Precondition for deleting a LocalStorqge instance is that it ni longer contains any proeprties with 
     * any subscribers left.  If even a single property with one or more subscribers exists the storage object
     * will remain untouched and Delete() returns false
     */
    public static Delete(path: LocalStoragePath): boolean {
        return LocalStorageLookup.GetOrCreateSingleton().delete(path);
    }

    /**
     * chk LocalStorage object with given path is known
     * same as Map.has
    */
    public static Has(path: LocalStoragePath): boolean {
        return LocalStorageLookup.GetOrCreateSingleton().has(path);
    }

    /**
     * retrieve LocalStorge by path
     * zame as Map.get
    */
    public static Get(path: LocalStoragePath): LocalStorage | undefined {
        return LocalStorageLookup.GetOrCreateSingleton().get(path);
    }


    // internals =======================================================================

    protected static GetOrCreateSingleton() {
        if (!LocalStorageLookup.INSTANCE_) {
            console.log("Creating LocalStorageLookup singleton");
            LocalStorageLookup.INSTANCE_ = new LocalStorageLookup();
        }
        return LocalStorageLookup.INSTANCE_;
    }

    protected constructor() {
        this.mapByPath_ = new Map<LocalStoragePath, LocalStorage>();
    }

    protected has(path: LocalStoragePath): boolean {
        return this.mapByPath_.has(path);
    }

    protected get(path: LocalStoragePath): LocalStorage | undefined {
        return this.mapByPath_.get(path);
    }

    protected delete(path: LocalStoragePath): boolean {
        let stor = this.get(path);
        if (!stor) {
            console.warn(`LocalStorageLookup: Attempt to delete non-existing LocalStorage with path ${path}`);
            return false;
        }
        const canDelete = stor.aboutToBeDeleted();
        if (canDelete) {
            this.mapByPath_.delete(path);
            return true;
        } else {
            console.warn(`LocalStorageLookup: Attempt to delete LocalStorage with path 
               ${path}, still has subscribers. Can not delete!`);
            return false;
        }
    }

    public add(path: LocalStoragePath, existingStorage : LocalStorage) {
        if (this.has(path)) {
            console.error(`LocalStorageLookup: Add LocalStorge instance with path '${path}' exists already. Failing to add.`)
            return false;
        }
         this.mapByPath_.set(path, existingStorage);
         return true;
    }

    protected getOrCreate(path: LocalStoragePath, initializingProperties: Object): LocalStorage {
        let stor = this.get(path);
        if (stor) {
            console.log(`LocalStorageLookup: GetOrCreateNew returning exisitng LocalStorage with path '${path}' .`);
            return stor;
        }
        console.log(`LocalStorageLookup: GetOrCreateNew creating new LocalStorage with path '${path}' .`);
        stor = new LocalStorage(initializingProperties);
        this.mapByPath_.set(path, stor);
        return stor;
    }


    private static INSTANCE_: LocalStorageLookup = undefined;
    private mapByPath_: Map<LocalStoragePath, LocalStorage>;
}
