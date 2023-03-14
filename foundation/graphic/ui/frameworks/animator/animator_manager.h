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

/**
 * @addtogroup UI_Animator
 * @{
 *
 * @brief Defines UI animation effects and provides matched curves.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file animator_manager.h
 *
 * @brief Represents the animator manager.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_ANIMATOR_MANAGER_H
#define GRAPHIC_LITE_ANIMATOR_MANAGER_H

#include "animator/animator.h"
#include "common/task.h"
#include "gfx_utils/list.h"

namespace OHOS {
/**
 * @brief Represents the animator manager.
 *
 * This is a singleton class used to manage <b>Animator</b> instances.
 *
 * @see Task
 * @since 1.0
 * @version 1.0
 */
class AnimatorManager : public Task {
public:
    /**
     * @brief Obtains the <b>AnimatorManager</b> instance.
     *
     * @return Returns the <b>AnimatorManager</b> instance.
     * @since 1.0
     * @version 1.0
     */
    static AnimatorManager* GetInstance();

    void Init() override;

    /**
     * @brief Adds the <b>Animator</b> instance to the <b>AnimatorManager</b> linked list for management,
     *        so that the {@link Run} function of the <b>Animator</b> class is called once for each frame.
     *
     * @param animator Indicates the pointer to the <b>Animator</b> instance to add.
     * @see Remove
     * @since 1.0
     * @version 1.0
     */
    void Add(Animator* animator);

    /**
     * @brief Removes the <b>Animator</b> instance from the <b>AnimatorManager</b> linked list.
     *
     * @param animator Indicates the pointer to the <b>Animator</b> instance to remove.
     * @see Add
     * @since 1.0
     * @version 1.0
     */
    void Remove(const Animator* animator);

    void AnimatorTask();

    void Callback() override
    {
        AnimatorTask();
    }

protected:
    List<Animator*> list_;
    AnimatorManager() {}
    virtual ~AnimatorManager() {}
    AnimatorManager(const AnimatorManager&) = delete;
    AnimatorManager& operator=(const AnimatorManager&) = delete;
    AnimatorManager(AnimatorManager&&) = delete;
    AnimatorManager& operator=(AnimatorManager&&) = delete;
};
} // namespace OHOS
#endif
