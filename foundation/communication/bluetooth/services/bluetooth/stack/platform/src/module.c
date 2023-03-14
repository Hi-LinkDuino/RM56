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

#include "platform/include/module.h"
#include <string.h>

static Module *g_modules[MAX_MODULES];

void ModuleRegister(Module *module)
{
    for (unsigned long index = 0; index < sizeof(g_modules) / sizeof(g_modules[0]); index++) {
        if (!g_modules[index]) {
            g_modules[index] = module;
            break;
        }
    }
}

Module *ModuleGet(const char *name)
{
    Module *module = NULL;

    for (unsigned long index = 0; index < sizeof(g_modules) / sizeof(g_modules[0]); index++) {
        if (g_modules[index] && !strcmp(name, g_modules[index]->name)) {
            module = g_modules[index];
            break;
        }
    }
    return module;
}

NO_SANITIZE("cfi") void ModuleInit(const char *name, int traceLevel)
{
    Module *module = ModuleGet(name);
    if (module != NULL) {
        module->init(traceLevel);  
    }
}

NO_SANITIZE("cfi") void ModuleStartup(const char *name)
{
    Module *module = ModuleGet(name);
    if (module != NULL) {
        module->startup();  
    }
}

NO_SANITIZE("cfi") void ModuleShutdown(const char *name)
{
    Module *module = ModuleGet(name);
    if (module != NULL) {
        module->shutdown();
    }
}

void ModuleCleanup(const char *name)
{
    Module *module = ModuleGet(name);
    if (module != NULL) {
        module->cleanup();
    }
}
