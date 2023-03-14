/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CONTEXT_H
#define CONTEXT_H

#include "base_def.h"
#include "dispatcher.h"
#include "message.h"

namespace utility {
class IContextCallback {
public:
    /**
     * @brief A destructor used to delete the <b>IContextObserver</b> instance.
     *
     * @since 6
     */
    virtual ~IContextCallback() = default;

    /**
     * @brief Enable complete notify
     *
     * @param name adapter name or profile name.
     * @param ret enable complete successful or failed.
     * @since 6
     */
    virtual void OnEnable(const std::string &name, bool ret) = 0;

    /**
     * @brief Disable complete notify
     *
     * @param name adapter name or profile name.
     * @param ret disable complete successful or failed.
     * @since 6
     */
    virtual void OnDisable(const std::string &name, bool ret) = 0;
};

class Context {
public:
    /**
     * @brief Construct a new Context object, Usually inherited by profile services,
     *        to Construct it's own thread Context environment.
     *
     * @param name Context name.
     * @param version Context version.
     * @since 6
     */
    Context(const std::string &name, const std::string &version)
        : name_(name), version_(version), dispatcher_(std::make_unique<Dispatcher>(name))
    {}
    virtual ~Context()
    {}

    /**
     * @brief Enable.
     * @since 6
     */
    virtual void Enable() = 0;

    /**
     * @brief Disable.
     * @since 6
     */
    virtual void Disable() = 0;

    /**
     * @brief Post enable.
     * @since 6
     */
    virtual void PostEnable()
    {}

    /**
     * @brief Enable complete callback notify.
     * @since 6
     */
    virtual void OnEnable(const std::string &name, bool ret)
    {
        if (callback_)
            callback_->OnEnable(name, ret);
    }

    /**
     * @brief Disable complete callback notify.
     * @since 6
     */
    virtual void OnDisable(const std::string &name, bool ret)
    {
        if (callback_)
            callback_->OnDisable(name, ret);
    }

    /**
     * @brief register callback function.
     * @param callback callback function.
     * @since 6
     */
    void RegisterCallback(IContextCallback &callback)
    {
        callback_ = &callback;
    }

    /**
     * @brief Initialize the context
     * @since 6
     */
    void Initialize()
    {
        dispatcher_->Initialize();
        InitializeInternal();
    }

    /**
     * @brief Uninitialize the context
     * @since 6
     */
    void Uninitialize()
    {
        dispatcher_->Uninitialize();
        UninitializeInternal();
    }

    /**
     * @brief Get the Context's Dispatcher object
     * @return Dispatcher pointer.
     * @since 6
     */
    Dispatcher *GetDispatcher()
    {
        return dispatcher_.get();
    }

    /**
     * @brief Get Context's name.
     * @return Context's name.
     * @since 6
     */
    const std::string &Name() const
    {
        return name_;
    }

    /**
     * @brief Get Context's version.
     * @return Context's version.
     * @since 6
     */
    const std::string &Version() const
    {
        return version_;
    }

private:
    /**
     * @brief Internal initialize function, called by Initialize
     * @since 6
     */
    virtual void InitializeInternal()
    {}

    /**
     * @brief Internal uninitialize function, called by Uninitialize
     * @since 6
     */
    virtual void UninitializeInternal()
    {}

    std::string name_ {""};
    std::string version_ {};
    std::unique_ptr<Dispatcher> dispatcher_ {};
    IContextCallback *callback_ = nullptr;
    BT_DISALLOW_COPY_AND_ASSIGN(Context);
};
}  // namespace utility

#endif  // CONTEXT_H