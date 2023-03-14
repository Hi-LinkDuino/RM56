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

#ifndef CLASS_CREATOR_H
#define CLASS_CREATOR_H

#include <map>
#include <string>

namespace OHOS {
namespace bluetooth {
using ClassCreateFun = void *(*)(void);

class ClassFactory {
public:
    static void RegisterClass(const std::string &name, ClassCreateFun func);
    static void *NewInstance(const std::string &name);

private:
    static std::map<std::string, ClassCreateFun> g_register;
};

class ClassRegister {
public:
    ClassRegister(const std::string &name, ClassCreateFun func)
    {
        ClassFactory::RegisterClass(name, func);
    };

private:
    ClassRegister() = delete;
};

template<class T>
class ClassCreator {
public:
    static T *NewInstance(const std::string &name)
    {
        return (T *)ClassFactory::NewInstance(name);
    };
};

#define REGISTER_CLASS_CREATOR(class_name) \
    static void *class_name##Creator(void) \
    {                                      \
        return (void *)(new class_name()); \
    }                                      \
    const ClassRegister class_name##Register(#class_name, class_name##Creator)
}  // namespace bluetooth
}  // namespace OHOS

#endif  // CLASS_CREATOR_H