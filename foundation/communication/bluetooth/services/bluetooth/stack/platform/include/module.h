/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef MODULE_H
#define MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MODULE_DEPENDENCIES (5)
#define MAX_MODULES (20)

typedef struct {
    const char *name;
    void (*init)(int trace_level);
    void (*startup)();
    void (*shutdown)();
    void (*cleanup)();
    const char *dependencies[MAX_MODULE_DEPENDENCIES];
} Module;

void ModuleRegister(Module *module);
Module *ModuleGet(const char *name);
void ModuleInit(const char *name, int trace_level);
void ModuleStartup(const char *name);
void ModuleShutdown(const char *name);
void ModuleCleanup(const char *name);

#define MODULE_DECL(module)                                         \
    void __attribute__((constructor)) do_module_init_##module(void) \
    {                                                               \
        ModuleRegister(&module);                                    \
    }

#ifndef NO_SANITIZE
#ifdef __has_attribute
#if __has_attribute(no_sanitize)
#define NO_SANITIZE(type) __attribute__((no_sanitize(type)))
#endif
#endif
#endif

#ifndef NO_SANITIZE
#define NO_SANITIZE(type)
#endif

#ifdef __cplusplus
}
#endif

#endif  // MODULE_H
