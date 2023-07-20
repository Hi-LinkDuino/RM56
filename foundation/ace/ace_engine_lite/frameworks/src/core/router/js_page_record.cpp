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

#include "js_page_record.h"
#include "ace_log.h"
#include "string_util.h"

namespace OHOS{
namespace ACELite{

using namespace std;

PageRecord::PageRecord()
{
    pageList_ = new std::list<StateMachine*>();
}

PageRecord::~PageRecord()
{
    while(!pageList_->empty()){
        StateMachine* item = pageList_->back();
        pageList_->pop_back();
        delete item;
        item = nullptr;
    }

    delete pageList_;
    pageList_ = nullptr;

}

void PageRecord::PushPage(StateMachine* stateMachine)
{
    if(stateMachine == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "push page failed stateMachine is nullptr");
        return;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "PageRecord Push Page uri: %s stack size: %d ", stateMachine->GetJsPagePath(), pageList_->size());

    pageList_->push_back(stateMachine);
    
    
}

StateMachine* PageRecord::BackPage(const char* const uri)
{
    HILOG_INFO(HILOG_MODULE_ACE, "PageRecord Back Page uri: %s stack size: %d ", uri, pageList_->size());

    if(pageList_->empty()){
        HILOG_ERROR(HILOG_MODULE_ACE, "back page stack is empty");
        return nullptr;
    }

    StateMachine* res = nullptr;

    if(uri == nullptr || strlen(uri) == 0){
        res = pageList_->back();
        pageList_->pop_back();
        return res;
    }

    std::list<StateMachine*>::reverse_iterator it = pageList_->rbegin();

    int count = 0;
    while(it != pageList_->rend()){

        if(StringUtil::ComparePath(uri,(*it)->GetJsPagePath()) ){
            while (count > 0){
                StateMachine* item = pageList_->back();
                pageList_->pop_back();
                delete item;
                item = nullptr;
                count--;
            }

            res = pageList_->back();
            pageList_->pop_back();
            return res;
        }
        count++;
        it++;
    }


    res = pageList_->back();
    pageList_->pop_back();
 
    return res;
}

} // ACELite
} // OHOS
