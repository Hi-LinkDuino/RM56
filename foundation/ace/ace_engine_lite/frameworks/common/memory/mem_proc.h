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

#ifndef OHOS_ACELITE_MEM_PROC_H
#define OHOS_ACELITE_MEM_PROC_H

#include "acelite_config.h"
#ifdef SIMULATOR_MEMORY_ANALYSIS
#define CLEAR_UP() AceMemProc::GetInstance()->ClearUp()
#define SYS_MEMORY_TRACING() AceMemProc::GetInstance()->SysMemTracing()
#define JERRY_MEMORY_TRACING() AceMemProc::GetInstance()->JerryMemTracing()
#else
#define CLEAR_UP()
#define SYS_MEMORY_TRACING()
#define JERRY_MEMORY_TRACING()
#endif // SIMULATOR_MEMORY_ANALYSIS

#ifdef SIMULATOR_MEMORY_ANALYSIS
namespace OHOS {
namespace ACELite {
class AceMemProc {
public:
    /**
     * @brief Use static global variable for easy access in different source code file.
     *
     * @return global singleton AceMemProc object
     */
    static AceMemProc *GetInstance();

    /**
     * @brief Check if the memory proc is enabled or not.
     *
     * @return true for enabled, false for not
     */
    static bool IsEnabled();

    /**
     * @brief Clear up the previous analysis file.
     */
    void ClearUp();

    /**
     * @brief Output the system memory.
     */
    void SysMemTracing();

    /**
     * @brief Output the jerry memory.
     *
     */
    void JerryMemTracing();

private:
    /**
     * @brief Default constructor.
     */
    AceMemProc() = default;

    /**
     * @brief Default destructor.
     */
    ~AceMemProc() = default;
};
} // namespace ACELite
} // namespace OHOS
#endif // SIMULATOR_MEMORY_ANALYSIS
#endif // OHOS_ACELITE_MEM_PROC_H
