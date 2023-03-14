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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_JS_COMMAND_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_JS_COMMAND_H

#include <string>
#include <unordered_map>

#include "base/memory/referenced.h"
#include "base/utils/macros.h"
#include "core/accessibility/accessibility_manager.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/bridge/common/dom/dom_badge.h"
#include "frameworks/bridge/common/dom/dom_canvas.h"
#include "frameworks/bridge/common/dom/dom_chart.h"
#include "frameworks/bridge/common/dom/dom_clock.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_image_animator.h"
#include "frameworks/bridge/common/dom/dom_input.h"
#include "frameworks/bridge/common/dom/dom_proxy.h"
#include "frameworks/bridge/common/dom/dom_stepper.h"
#include "frameworks/bridge/common/dom/dom_stepper_item.h"
#include "frameworks/bridge/common/dom/dom_xcomponent.h"

namespace OHOS::Ace::Framework {

class JsAcePage;

// Basic class of command from JS framework
class ACE_EXPORT JsCommand : public Referenced {
public:
    JsCommand() : Referenced(false) {}
    ~JsCommand() override = default;

    virtual void Execute(const RefPtr<JsAcePage>& page) const = 0;
};

class ACE_EXPORT JsCommandDomElementOperator : public JsCommand {
public:
    ~JsCommandDomElementOperator() override = default;

    void SetAttributes(std::vector<std::pair<std::string, std::string>>&& attrs)
    {
        attrs_ = std::move(attrs);
    }

    void SetStyles(std::vector<std::pair<std::string, std::string>>&& styles)
    {
        styles_ = std::move(styles);
    }

    void AddEvents(std::vector<std::string>&& events)
    {
        events_ = std::move(events);
    }

    void SetAnimationStyles(std::vector<std::unordered_map<std::string, std::string>>&& animationStyles)
    {
        if (!animationStyles.empty()) {
            animationStyles_ =
                std::make_unique<std::vector<std::unordered_map<std::string, std::string>>>(std::move(animationStyles));
        }
    }

    void SetTransitionEnter(std::vector<std::unordered_map<std::string, std::string>>&& transitionEnter)
    {
        if (!transitionEnter.empty()) {
            transitionEnter_ =
                std::make_unique<std::vector<std::unordered_map<std::string, std::string>>>(std::move(transitionEnter));
        }
    }

    void SetTransitionExit(std::vector<std::unordered_map<std::string, std::string>>&& transitionExit)
    {
        if (!transitionExit.empty()) {
            transitionExit_ =
                std::make_unique<std::vector<std::unordered_map<std::string, std::string>>>(std::move(transitionExit));
        }
    }

    void SetSharedTransitionName(std::vector<std::unordered_map<std::string, std::string>>&& sharedTransitionName)
    {
        if (!sharedTransitionName.empty()) {
            sharedTransitionName_ = std::make_unique<std::vector<std::unordered_map<std::string, std::string>>>(
                std::move(sharedTransitionName));
        }
    }

    void SetId(const std::string& id)
    {
        id_ = id;
    }

    void SetShareId(const std::string& shareId)
    {
        shareId_ = shareId;
    }

    void SetTarget(const std::string& target)
    {
        target_ = target;
    }

    void SetSegments(const std::vector<Segment>& segments)
    {
        segments_ = std::make_unique<std::vector<Segment>>(segments);
    }

    void SetOptions(const ChartOptions& chartOptions)
    {
        chartOptions_ = std::make_unique<ChartOptions>(chartOptions);
    }

    void SetDatasets(const std::vector<MainChart>& datasets)
    {
        if (!datasets.empty()) {
            chartDatasets_ = std::make_unique<std::vector<MainChart>>(datasets);
        }
    }

    void SetImagesAttr(std::vector<ImageProperties>&& images)
    {
        if (!images.empty()) {
            images_ = std::make_unique<std::vector<ImageProperties>>(std::move(images));
        }
    }

    void SetClockConfig(const ClockConfig& clockConfig)
    {
        clockConfig_ = std::make_unique<ClockConfig>(clockConfig);
    }

    void SetBadgeConfig(const BadgeConfig& badgeConfig)
    {
        badgeConfig_ = std::make_unique<BadgeConfig>(badgeConfig);
    }

    void SetStepperLabel(const StepperLabels& stepperLabel)
    {
        stepperLabel_ = std::make_unique<StepperLabels>(stepperLabel);
    }

    void SetInputOptions(const std::vector<InputOption>& inputOptions)
    {
        inputOptions_ = std::make_unique<std::vector<InputOption>>(inputOptions);
    }

    void SetForIndex(const int32_t& itemIndex)
    {
        itemIndex_ = itemIndex;
    }

    void SetPipelineContext(const WeakPtr<PipelineContext>& pipelineContext)
    {
        pipelineContext_ = pipelineContext;
    }

    void SetIsCustomComponent(bool isCustom)
    {
        isCustomComponent_ = isCustom;
    }

protected:
    explicit JsCommandDomElementOperator(NodeId nodeId) : nodeId_(nodeId) {}

    void UpdateForChart(const RefPtr<DOMNode>& node) const;
    void UpdateForImageAnimator(const RefPtr<DOMNode>& node) const;
    void UpdateForClock(const RefPtr<DOMNode>& node) const;
    void UpdateForBadge(const RefPtr<DOMNode>& node) const;
    void UpdateForStepperLabel(const RefPtr<DOMNode>& node) const;
    void UpdateForInput(const RefPtr<DOMNode>& node) const;

    NodeId nodeId_ = -1;
    int32_t itemIndex_ = -1;
    std::vector<std::pair<std::string, std::string>> attrs_;
    std::vector<std::pair<std::string, std::string>> styles_;
    std::vector<std::string> events_;
    bool isCustomComponent_ = false;
    std::unique_ptr<std::vector<std::unordered_map<std::string, std::string>>> animationStyles_;
    std::unique_ptr<std::vector<std::unordered_map<std::string, std::string>>> transitionEnter_;
    std::unique_ptr<std::vector<std::unordered_map<std::string, std::string>>> transitionExit_;
    std::unique_ptr<std::vector<std::unordered_map<std::string, std::string>>> sharedTransitionName_;
    std::string id_;
    std::string shareId_;
    std::string target_;
    std::unique_ptr<std::vector<Segment>> segments_;
    std::unique_ptr<ChartOptions> chartOptions_;
    std::unique_ptr<std::vector<MainChart>> chartDatasets_;
    std::unique_ptr<std::vector<ImageProperties>> images_;
    std::unique_ptr<ClockConfig> clockConfig_;
    std::unique_ptr<BadgeConfig> badgeConfig_;
    std::unique_ptr<StepperLabels> stepperLabel_;
    std::unique_ptr<std::vector<InputOption>> inputOptions_;
    WeakPtr<PipelineContext> pipelineContext_;
    bool useLiteStyle_ = false;
};

class ACE_EXPORT JsCommandDomElementCreator : public JsCommandDomElementOperator {
public:
    ~JsCommandDomElementCreator() override = default;

protected:
    JsCommandDomElementCreator(const std::string& tagName, NodeId nodeId)
        : JsCommandDomElementOperator(nodeId), tagName_(tagName)
    {}

    RefPtr<DOMNode> CreateDomNode(const RefPtr<JsAcePage>& page, NodeId parentNodeId = -1) const;

    void MountDomNode(const RefPtr<DOMNode>& node, const RefPtr<DOMDocument>& domDocument, NodeId parentNodeId) const;

    RefPtr<DOMProxy> CreateDomProxy(const RefPtr<DOMDocument>& domDocument, NodeId parentNodeId = -1) const;

    void ScheduleUpdateForFixedNode(const RefPtr<DOMDocument>& domDocument) const;

    RefPtr<DOMNode> CreateDomElement(const RefPtr<JsAcePage>& page) const;
    std::string tagName_;
};

// JS command, which used to create body of DOM tree.
class ACE_EXPORT JsCommandCreateDomBody final : public JsCommandDomElementCreator {
public:
    JsCommandCreateDomBody(const std::string& tagName, NodeId nodeId) : JsCommandDomElementCreator(tagName, nodeId) {}
    ~JsCommandCreateDomBody() override = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;
};

// JS command, which used to create a new element
class ACE_EXPORT JsCommandCreateDomElement final : public JsCommandDomElementCreator {
public:
    JsCommandCreateDomElement(const std::string& tagName, NodeId nodeId)
        : JsCommandDomElementCreator(tagName, nodeId) {}
    ~JsCommandCreateDomElement() override = default;
    void Execute(const RefPtr<JsAcePage>& page) const final;
};

// JS command, which used to create and add new element into DOM tree.
class ACE_EXPORT JsCommandAddDomElement final : public JsCommandDomElementCreator {
public:
    JsCommandAddDomElement(const std::string& tagName, NodeId nodeId, NodeId parentNodeId)
        : JsCommandDomElementCreator(tagName, nodeId), parentNodeId_(parentNodeId)
    {}
    ~JsCommandAddDomElement() override = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    NodeId parentNodeId_ = -1;
};

class ACE_EXPORT  JsCommandAppendElement final : public JsCommandDomElementCreator {
public:
JsCommandAppendElement(const std::string& tagName, NodeId nodeId, NodeId parentNodeId)
    : JsCommandDomElementCreator(tagName, nodeId), parentNodeId_(parentNodeId)
    {}
    ~JsCommandAppendElement() override = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    NodeId parentNodeId_ = -1;
};

// JS command, which used to remove element from DOM tree.
class ACE_EXPORT JsCommandRemoveDomElement final : public JsCommand {
public:
    explicit JsCommandRemoveDomElement(NodeId nodeId) : nodeId_(nodeId) {}
    ~JsCommandRemoveDomElement() final = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    NodeId nodeId_ = -1;
};

// JS command, which used to update attributes of element in DOM tree.
class ACE_EXPORT JsCommandUpdateDomElementAttrs final : public JsCommandDomElementOperator {
public:
    explicit JsCommandUpdateDomElementAttrs(NodeId nodeId) : JsCommandDomElementOperator(nodeId) {}
    ~JsCommandUpdateDomElementAttrs() final = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;
};

// JS command, which used to update styles of element in DOM tree.
class ACE_EXPORT JsCommandUpdateDomElementStyles final : public JsCommandDomElementOperator {
public:
    explicit JsCommandUpdateDomElementStyles(NodeId nodeId) : JsCommandDomElementOperator(nodeId) {}
    ~JsCommandUpdateDomElementStyles() final = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;
};

// JS command, which used to call native method of element in DOM tree.
class ACE_EXPORT JsCommandCallDomElementMethod final : public JsCommand {
public:
    JsCommandCallDomElementMethod(NodeId nodeId, const std::string& method, const std::string& param)
        : nodeId_(nodeId), method_(method), param_(param)
    {}
    ~JsCommandCallDomElementMethod() final = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    NodeId nodeId_ = -1;
    std::string method_;
    std::string param_;
};

class ACE_EXPORT JsCommandContextOperation final : public JsCommand {
public:
    JsCommandContextOperation(NodeId nodeId, std::function<void(const RefPtr<CanvasTaskPool>&)> task)
        : nodeId_(nodeId), task_(std::move(task))
    {}
    ~JsCommandContextOperation() final = default;
    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    NodeId nodeId_ = -1;
    std::function<void(const RefPtr<CanvasTaskPool>&)> task_;
};

class ACE_EXPORT JsCommandXComponentOperation final : public JsCommand {
public:
    JsCommandXComponentOperation(NodeId nodeId, std::function<void(const RefPtr<XComponentTaskPool>&)> task)
        : nodeId_(nodeId), task_(std::move(task))
    {}
    ~JsCommandXComponentOperation() final = default;
    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    NodeId nodeId_ = -1;
    std::function<void(const RefPtr<XComponentTaskPool>&)> task_;
};

class ACE_EXPORT AnimationBridgeTask : public AceType {
    DECLARE_ACE_TYPE(AnimationBridgeTask, AceType)

public:
    AnimationBridgeTask() = default;
    ~AnimationBridgeTask() override = default;
    virtual void AnimationBridgeTaskFunc(const RefPtr<JsAcePage>&, NodeId) = 0;
};

class ACE_EXPORT JsCommandAnimation final : public JsCommand {
public:
    JsCommandAnimation(NodeId nodeId, RefPtr<AnimationBridgeTask> task) : nodeId_(nodeId), task_(std::move(task)) {}
    ~JsCommandAnimation() final = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    NodeId nodeId_ = -1;
    RefPtr<AnimationBridgeTask> task_;
};

class ACE_EXPORT AnimatorBridgeTask : public AceType {
    DECLARE_ACE_TYPE(AnimatorBridgeTask, AceType)

public:
    AnimatorBridgeTask() = default;
    ~AnimatorBridgeTask() override = default;
    virtual void AnimatorBridgeTaskFunc(const RefPtr<JsAcePage>& page, int32_t bridgeId) = 0;
};

class ACE_EXPORT JsCommandAnimator final : public JsCommand {
public:
    JsCommandAnimator(int32_t bridgeId, RefPtr<AnimatorBridgeTask> task) : bridgeId_(bridgeId),
    task_(std::move(task)) {}
    ~JsCommandAnimator() final = default;

    void Execute(const RefPtr<JsAcePage>& page) const final;

private:
    int32_t bridgeId_ = -1;
    RefPtr<AnimatorBridgeTask> task_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_JS_COMMAND_H
