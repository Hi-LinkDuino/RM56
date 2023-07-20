/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_JS_PAGE_RECORD_H
#define OHOS_ACELITE_JS_PAGE_RECORD_H

#include <list>
#include "js_page_state_machine.h"
#include "non_copyable.h"

namespace OHOS{
namespace ACELite{

class PageRecord final: public MemoryHeap{

public:
    ACE_DISALLOW_COPY_AND_MOVE(PageRecord);
    PageRecord();
    ~PageRecord();

    void PushPage(StateMachine* stateMachine);

    StateMachine* BackPage(const char* const uri);

private: 
    std::list<StateMachine*>* pageList_;

};
} // ACELite
} // OHOS

#endif // OHOS_ACELITE_JS_PAGE_RECORD_H