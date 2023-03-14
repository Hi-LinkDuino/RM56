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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_LAYER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_LAYER_H

#include <list>

#include "base/memory/ace_type.h"

namespace OHOS::Ace::Flutter {

class SceneBuilder;

class Layer : public AceType {
    DECLARE_ACE_TYPE(Layer, AceType)
public:
    Layer() = default;
    ~Layer() override = default;

    WeakPtr<Layer> GetParent()
    {
        return parentLayer_;
    }

    void SetParent(const RefPtr<Layer>& layer)
    {
        parentLayer_ = layer;
    }

    virtual void AddToScene(SceneBuilder& builder, double x, double y) = 0;

    void AddChildren(const RefPtr<Layer>& layer);
    void RemoveChildren()
    {
        children_.clear();
    }

    const std::list<RefPtr<Layer>>& GetChildren() const
    {
        return children_;
    }

    int32_t Size()
    {
        return children_.size();
    }

    virtual void DumpTree(int32_t depth);
    virtual void Dump() = 0;

protected:
    WeakPtr<Layer> parentLayer_;
    std::list<RefPtr<Layer>> children_;
};

} // namespace OHOS::Ace::Flutter

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_LAYERS_LAYER_H
