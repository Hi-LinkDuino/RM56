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

#include "gtest/gtest.h"

#include "core/components/box/box_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/root/root_component.h"
#include "core/components/root/root_element.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/composed_element.h"

#include "core/components/test/json/json_frontend.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS::Ace {

const ComposeId ROOT_COMPOSE_ID = "root";
const string ROOT_COMPOSE_NAME = "rootCompose";

class NodeInfo {
public:
    NodeInfo(const string& tag, int32_t depth, int32_t childrenNum)
        : tag_(tag), depth_(depth), childrenNum_(childrenNum), data_()
    {}

    NodeInfo(const string& tag, int32_t depth, int32_t childrenNum, string data)
        : tag_(tag), depth_(depth), childrenNum_(childrenNum), data_(data)
    {}

    ~NodeInfo() = default;

public:
    string tag_;
    int32_t depth_ = 0;
    int32_t childrenNum_ = 0;
    string data_;
};

class ViewUpdateTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    void BuildEmptyTrees();
    void BuildSingleChildTrees(const string& child, const string& textData = "");
    void BuildMultipleChildrenTrees(const vector<string>& children, const vector<string>& textData = vector<string>());
    void UpdateAndValidate();

    static void DumpElementTree(const RefPtr<Element>& element, int32_t depth, vector<NodeInfo>& treeInfo);
    static void DumpRenderTree(const RefPtr<RenderNode>& renderNode, int32_t depth, vector<NodeInfo>& treeInfo);
    static void PrintTreeInfo(const vector<NodeInfo>& treeInfo);
    static bool IsTreeEqual(const vector<NodeInfo>& treeInfoA, const vector<NodeInfo>& treeInfoB);

private:
    RefPtr<ComposedElement> composedElement_;
    RefPtr<RootElement> rootElement_;
    RefPtr<RenderNode> rootRender_;

    RefPtr<ComposedComponent> newComposedComponent_;
    vector<NodeInfo> newElementTree_;
    vector<NodeInfo> newRenderTree_;

    RefPtr<PipelineContext> pipelineContext_;
};

void ViewUpdateTest::SetUp()
{
    // setup component tree
    RefPtr<ComposedComponent> rootCompose =
        AceType::MakeRefPtr<ComposedComponent>(ROOT_COMPOSE_ID, ROOT_COMPOSE_NAME, nullptr);
    RefPtr<RootComponent> rootComponent = AceType::MakeRefPtr<RootComponent>(rootCompose);

    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto frontend = Frontend::CreateDefault();
    pipelineContext_ = AceType::MakeRefPtr<PipelineContext>(
        std::move(window), nullptr, nullptr, nullptr, frontend, 0);

    // setup element tree and render tree
    rootElement_ = AceType::MakeRefPtr<RootElement>();
    ASSERT_TRUE(rootElement_);
    rootElement_->SetPipelineContext(pipelineContext_);
    rootElement_->SetNewComponent(rootComponent);
    rootElement_->Mount(nullptr);
    ASSERT_FALSE(rootElement_->GetChildren().empty());
    composedElement_ = AceType::DynamicCast<ComposedElement>(rootElement_->GetChildren().front());
    ASSERT_TRUE(composedElement_);
    rootRender_ = rootElement_->GetRenderNode();
    ASSERT_TRUE(rootRender_);
}

void ViewUpdateTest::TearDown()
{
    rootElement_ = nullptr;
    composedElement_ = nullptr;
    rootRender_ = nullptr;

    newComposedComponent_ = nullptr;
    newElementTree_.clear();
    newRenderTree_.clear();
}

void ViewUpdateTest::DumpElementTree(const RefPtr<Element>& element, int32_t depth, vector<NodeInfo>& treeInfo)
{
    const list<RefPtr<Element>>& children = element->GetChildren();
    treeInfo.emplace_back(AceType::TypeName(element), depth, children.size());
    for (const auto& item : children) {
        DumpElementTree(item, depth + 1, treeInfo);
    }
}

void ViewUpdateTest::DumpRenderTree(const RefPtr<RenderNode>& renderNode, int32_t depth, vector<NodeInfo>& treeInfo)
{
    const list<RefPtr<RenderNode>>& children = renderNode->GetChildren();
    RefPtr<RenderText> renderText = AceType::DynamicCast<RenderText>(renderNode);
    if (renderText) {
        treeInfo.emplace_back(AceType::TypeName(renderNode), depth, children.size(), renderText->GetTextData());
    } else {
        treeInfo.emplace_back(AceType::TypeName(renderNode), depth, children.size());
    }
    for (const auto& item : children) {
        DumpRenderTree(item, depth + 1, treeInfo);
    }
}

void ViewUpdateTest::PrintTreeInfo(const vector<NodeInfo>& treeInfo)
{
    for (const auto& node : treeInfo) {
        ostringstream nodeInfo;
        nodeInfo << "[" << node.depth_ << "]\t[" << node.childrenNum_ << "]\t[" << node.tag_;
        if (node.data_.empty()) {
            nodeInfo << "]";
        } else {
            nodeInfo << "]\t[" << node.data_ << "]";
        }
        GTEST_LOG_(INFO) << nodeInfo.str();
    }
}

bool ViewUpdateTest::IsTreeEqual(const vector<NodeInfo>& treeInfoA, const vector<NodeInfo>& treeInfoB)
{
    if (treeInfoA.size() != treeInfoB.size()) {
        return false;
    }
    for (size_t i = 0; i < treeInfoA.size(); ++i) {
        if (treeInfoA[i].tag_ != treeInfoB[i].tag_ ||
            treeInfoA[i].depth_ != treeInfoB[i].depth_ ||
            treeInfoA[i].childrenNum_ != treeInfoB[i].childrenNum_ ||
            treeInfoA[i].data_ != treeInfoB[i].data_) {
            return false;
        }
    }
    return true;
}

void ViewUpdateTest::BuildEmptyTrees()
{
    newElementTree_.clear();
    newElementTree_.emplace_back("RootElement", 0, 1);
    newElementTree_.emplace_back("ComposedElement", 1, 0);

    newRenderTree_.clear();
    newRenderTree_.emplace_back("FlutterRenderRoot", 0, 0);

    newComposedComponent_ = AceType::MakeRefPtr<ComposedComponent>(ROOT_COMPOSE_ID, ROOT_COMPOSE_NAME, nullptr);
};

void ViewUpdateTest::BuildSingleChildTrees(const string& child, const string& textData)
{
    newElementTree_.clear();
    newElementTree_.emplace_back("RootElement", 0, 1);
    newElementTree_.emplace_back("ComposedElement", 1, 1);

    newRenderTree_.clear();
    newRenderTree_.emplace_back("FlutterRenderRoot", 0, 1);

    RefPtr<Component> childComponent;
    if (child == "text") {
        childComponent = AceType::MakeRefPtr<TextComponent>(textData);
        newElementTree_.emplace_back("TextElement", 2, 0);
        newRenderTree_.emplace_back("FlutterRenderText", 1, 0, textData);
    } else if (child == "box") {
        childComponent = AceType::MakeRefPtr<BoxComponent>();
        newElementTree_.emplace_back("BoxElement", 2, 0);
        newRenderTree_.emplace_back("FlutterRenderBox", 1, 0);
    } else if (child == "padding") {
        childComponent = AceType::MakeRefPtr<PaddingComponent>();
        newElementTree_.emplace_back("PaddingElement", 2, 0);
        newRenderTree_.emplace_back("FlutterRenderPadding", 1, 0);
    }
    newComposedComponent_ = AceType::MakeRefPtr<ComposedComponent>(ROOT_COMPOSE_ID, ROOT_COMPOSE_NAME, childComponent);
}

void ViewUpdateTest::BuildMultipleChildrenTrees(const vector<string>& children, const vector<string>& textData)
{
    size_t textChildrenNum = count(children.begin(), children.end(), "text");
    ASSERT_EQ(textChildrenNum, textData.size());

    newElementTree_.clear();
    newElementTree_.emplace_back("RootElement", 0, 1);
    newElementTree_.emplace_back("ComposedElement", 1, 1);
    newElementTree_.emplace_back("RowElement", 2, children.size());

    newRenderTree_.clear();
    newRenderTree_.emplace_back("FlutterRenderRoot", 0, 1);
    newRenderTree_.emplace_back("RenderFlex", 1, children.size());

    std::list<RefPtr<Component>> rowChildren;
    size_t textId = 0;
    for (const auto& node : children) {
        if (node == "text") {
            rowChildren.emplace_back(AceType::MakeRefPtr<TextComponent>(textData.at(textId)));
            newElementTree_.emplace_back("TextElement", 3, 0);
            newRenderTree_.emplace_back("FlutterRenderText", 2, 0, textData.at(textId));
            ++textId;
        } else if (node == "box") {
            rowChildren.emplace_back(AceType::MakeRefPtr<BoxComponent>());
            newElementTree_.emplace_back("BoxElement", 3, 0);
            newRenderTree_.emplace_back("FlutterRenderBox", 2, 0);
        } else if (node == "padding") {
            rowChildren.emplace_back(AceType::MakeRefPtr<PaddingComponent>());
            newElementTree_.emplace_back("PaddingElement", 3, 0);
            newRenderTree_.emplace_back("FlutterRenderPadding", 2, 0);
        }
    }
    RefPtr<RowComponent> rowComponent =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, rowChildren);
    newComposedComponent_ = AceType::MakeRefPtr<ComposedComponent>(ROOT_COMPOSE_ID, ROOT_COMPOSE_NAME, rowComponent);
}

void ViewUpdateTest::UpdateAndValidate()
{
    vector<NodeInfo> elementTreeInfo;
    vector<NodeInfo> renderTreeInfo;
    composedElement_->SetNewComponent(newComposedComponent_);
    composedElement_->Rebuild();
    DumpElementTree(rootElement_, 0, elementTreeInfo);
    DumpRenderTree(rootRender_, 0, renderTreeInfo);

    ASSERT_TRUE(IsTreeEqual(elementTreeInfo, newElementTree_));
    ASSERT_TRUE(IsTreeEqual(renderTreeInfo, newRenderTree_));
}

/**
 * @tc.name: ViewUpdateTest001
 * @tc.desc: View Updates caused by adding components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGB
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest001";

    /**
     * @tc.steps: step1. Build a composed component tree containing only one TextComponent, update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    BuildSingleChildTrees("text", "text data");
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest002
 * @tc.desc: View Updates caused by adding components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGB
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest002";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component, update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest003
 * @tc.desc: View Updates caused by adding components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGB
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest003";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with two children,
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing a Row Component with three children,
     * update and check results.
     * @tc.expected: step2. New elements are correctly added into element tree,
     * and new render objects are correctly added into render tree.
     */
    children.clear();
    textData.clear();
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest004
 * @tc.desc: View Updates caused by deleting components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGD
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest004";

    /**
     * @tc.steps: step1. Build a composed component tree containing only one TextComponent, update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    BuildSingleChildTrees("text", "text data");
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build an empty composed component tree, update and check results.
     * @tc.expected: step2. TextElement is removed form element tree and RenderText is removed from render tree.
     */
    BuildEmptyTrees();
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest005
 * @tc.desc: View Updates caused by deleting components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGD
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest005";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with three children,
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build an empty composed component tree, update and check results.
     * @tc.expected: step2. All TextElement is removed form element tree and All RenderText is removed from render tree.
     */
    BuildEmptyTrees();
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest006
 * @tc.desc: View Updates caused by deleting components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGD
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest006";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with three children,
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing a Row Component with two children,
     * update and check results.
     * @tc.expected: step2. One TextElement is removed form element tree and One RenderText is removed from render tree.
     */
    children.clear();
    textData.clear();
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest007
 * @tc.desc: View Updates caused by updating components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGC
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest007";

    /**
     * @tc.steps: step1. Build a composed component tree containing only one TextComponent which content is "text data",
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    BuildSingleChildTrees("text", "text data");
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing only one TextComponent which content is "updated
     * text data", update and check results.
     * @tc.expected: step2. Content of RenderText is changed as expected.
     */
    BuildSingleChildTrees("text", "updated text data");
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest008
 * @tc.desc: View Updates caused by updating components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGC
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest008";

    /**
     * @tc.steps: step1. Build a composed component tree containing only one TextComponent which content is "text data",
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    BuildSingleChildTrees("text", "text data");
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing one BoxComponent, update and check results.
     * @tc.expected: step2. Elements in element tree and render objects in render tree are changed as expected.
     */
    BuildSingleChildTrees("box");
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest009
 * @tc.desc: View Updates caused by updating components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGC
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest009";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with five children, all of them are
     * Text Component. The content of these child nodes as follows:
     * "first text" | "second text" | "third text" | "fourth text" | "fifth text"
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    textData.emplace_back("fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing a Row Component with five children, all of them are
     * Text Component. The content of these child nodes as follows:
     * "first text" | "updated second text" | "third text" | "fourth text" | "fifth text"
     * update and check results.
     * @tc.expected: step2. Element tree and render tree are the same as expected.
     */
    children.clear();
    textData.clear();
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("updated second text");
    textData.emplace_back("third text");
    textData.emplace_back("fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest010
 * @tc.desc: View Updates caused by updating components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGC
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest010";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with five children, all of them are
     * Text Component. The content of these child nodes as follows:
     * "first text" | "second text" | "third text" | "fourth text" | "fifth text"
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    textData.emplace_back("fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing a Row Component with five children, all of them are
     * Text Component. The content of these child nodes as follows:
     * "first text" | "updated second text" | "third text" | "updated fourth text" | "fifth text"
     * update and check results.
     * @tc.expected: step2. Element tree and render tree are the same as expected.
     */
    children.clear();
    textData.clear();
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("updated second text");
    textData.emplace_back("third text");
    textData.emplace_back("updated fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest011
 * @tc.desc: View Updates caused by updating components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGC
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest011";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with five children, all of them are
     * Text Component.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    textData.emplace_back("fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing a Row Component with five children, four of them are
     * Text Component and one is Box Component.
     * @tc.expected: step2. Element tree and render tree are the same as expected.
     */
    children.clear();
    textData.clear();
    children.emplace_back("text");
    children.emplace_back("box");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("third text");
    textData.emplace_back("fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest012
 * @tc.desc: View Updates caused by updating components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGC
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest012";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with five children, all of them are
     * Text Component.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    textData.emplace_back("fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing a Row Component with five children, three of them
     * are Text Component and two are Box Component.
     * @tc.expected: step2. Element tree and render tree are the same as expected.
     */
    children.clear();
    textData.clear();
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("box");
    children.emplace_back("box");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

/**
 * @tc.name: ViewUpdateTest013
 * @tc.desc: View Updates caused by updating components.
 * @tc.type: FUNC
 * @tc.require: AR000DAIGC
 * @tc.author: fanzhaonan
 */
HWTEST_F(ViewUpdateTest, ViewUpdateTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ViewUpdateTest0013";
    vector<string> children;
    vector<string> textData;

    /**
     * @tc.steps: step1. Build a composed component tree containing a Row Component with five children, all of them are
     * Text Component. The content of these child nodes as follows:
     * "first text" | "second text" | "third text" | "fourth text" | "fifth text"
     * update and check results.
     * @tc.expected: step1. Element tree and render tree are the same as expected.
     */
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("first text");
    textData.emplace_back("second text");
    textData.emplace_back("third text");
    textData.emplace_back("fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();

    /**
     * @tc.steps: step2. Build a composed component tree containing a Row Component with five children, three of them
     * are Text Component and two are Box Component. The content of those Text Component as follows:
     * "updated second text" | "updated fourth text" | "fifth text"
     * update and check results.
     * @tc.expected: step2. Element tree and render tree are the same as expected.
     */
    children.clear();
    textData.clear();
    children.emplace_back("box");
    children.emplace_back("text");
    children.emplace_back("box");
    children.emplace_back("text");
    children.emplace_back("text");
    textData.emplace_back("updated second text");
    textData.emplace_back("updated fourth text");
    textData.emplace_back("fifth text");
    BuildMultipleChildrenTrees(children, textData);
    UpdateAndValidate();
}

} // namespace OHOS::Ace
