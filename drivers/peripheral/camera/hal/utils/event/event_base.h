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

#ifndef EVENT_BASE_UTILS_H
#define EVENT_BASE_UTILS_H

#include <cstdio>
#include <stdarg.h>
#include <iostream>
#include <unistd.h>
#include <memory>

class Event {
};

template<typename T, typename... Args>
class EventBase {
typedef T (Event::*pMemFunc)(T arg, Args... args);
public:
    EventBase()
    {
        m_obj = NULL;
        m_func = NULL;
    }

    template<class _func_type>
    void Associate(Event* obj, _func_type func)
    {
        m_obj = obj;
        m_func = static_cast<pMemFunc>(func);
    }
    template<class _func_type>
    void DisAssociate(Event* obj, _func_type func)
    {
        if (obj != m_obj) {
            return;
        }
        m_func = NULL;
    }

    void SendEvent(T arg,Args... args)
    {
        (m_obj->*pMemFunc(m_func))(arg, args...);
    }

private:
    Event*      m_obj;
    pMemFunc    m_func;
};

/*************************event array**************************************/
template<typename T, typename... Args>
class EventBaseArray {
#define EVENT_LIST_MAX_NUM (10)
typedef T (Event::*pMemFunc)(T arg, Args... args);
public:
    EventBaseArray()
    {
        m_totalFunc = 0;
        m_obj = NULL;
        for (int i = 0; i < EVENT_LIST_MAX_NUM; i++) {
            m_func[i] = NULL;
        }
    }

    template<class _func_type>
    void Associate(Event* obj, _func_type func)
    {
        m_obj = obj;
        m_func[m_totalFunc] = static_cast<pMemFunc>(func);
        m_totalFunc++;
    }

    template<class _func_type>
    void DisAssociate(Event* obj, _func_type func)
    {
        if (obj != m_obj) {
            return;
        }
        int i;
        for (i = 0; i < m_totalFunc; i++) {
            if (m_func[i] == func) {
                break;
            }
        }
        for (i = 0; i < m_totalFunc - 1; i++) {
            m_func[i] = m_func[i + 1];
        }

        m_func[i] = NULL;
        m_totalFunc--;
    }

    void SendEvent(T arg, Args... args)
    {
        for (int i = 0; i < m_totalFunc; i++) {
            if (m_func[i] != NULL) {
                ((m_obj->*pMemFunc(m_func[i])))(arg, args...);
            }
        }
    }

private:
    Event*      m_obj;
    pMemFunc    m_func[EVENT_LIST_MAX_NUM];
    int         m_totalFunc;
};
#endif
