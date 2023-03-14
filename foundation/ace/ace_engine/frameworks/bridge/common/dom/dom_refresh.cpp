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

#include "frameworks/bridge/common/dom/dom_refresh.h"

#include "core/components/progress/progress_theme.h"
#include "core/components/refresh/refresh_theme.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMRefresh::DOMRefresh(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    refreshChild_ = AceType::MakeRefPtr<RefreshComponent>();
    columnChild_ = AceType::MakeRefPtr<ColumnComponent>(
        FlexAlign::FLEX_START, FlexAlign::FLEX_START, std::list<RefPtr<Component>>());
}

void DOMRefresh::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMRefresh::ResetInitializedStyle()
{
    RefPtr<RefreshTheme> theme = GetTheme<RefreshTheme>();
    if (theme) {
        refreshChild_->SetLoadingDistance(theme->GetLoadingDistance());
        refreshChild_->SetRefreshDistance(theme->GetRefreshDistance());
        refreshChild_->SetProgressDistance(theme->GetProgressDistance());
        refreshChild_->SetProgressDiameter(theme->GetProgressDiameter());
        refreshChild_->SetMaxDistance(theme->GetMaxDistance());
        refreshChild_->SetShowTimeDistance(theme->GetShowTimeDistance());
        refreshChild_->SetTextStyle(theme->GetTextStyle());
        refreshChild_->SetProgressColor(theme->GetProgressColor());
        refreshChild_->SetBackgroundColor(theme->GetBackgroundColor());
    }
}

bool DOMRefresh::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_REFRESH_OFFSET) {
        Dimension offset = ParseDimension(attr.second);
        refreshChild_->SetUseOffset(true);
        refreshChild_->SetIndicatorOffset(offset);
    } else if (attr.first == DOM_REFRESH_REFRESHING) {
        refreshing_ = StringToBool(attr.second);
        refreshChild_->SetRefreshing(refreshing_);
    } else if (attr.first == DOM_REFRESH_TYPE) {
        if (attr.second == "pulldown") {
            refreshType_ = RefreshType::PULL_DOWN;
        } else {
            refreshType_ = RefreshType::AUTO;
        }
        refreshChild_->SetRefreshType(refreshType_);
    } else if (attr.first == DOM_REFRESH_LASTTIME) {
        showLastTime_ = StringToBool(attr.second);
        refreshChild_->SetShowLastTime(showLastTime_);
    } else if (attr.first == DOM_REFRESH_FRICTION) {
        friction_ = StringToInt(attr.second);
        refreshChild_->SetFriction(friction_);
    } else if (attr.first == DOM_REFRESH_TIME_OFFSET) {
        refreshChild_->SetTimeOffset(ParseDimension(attr.second));
    } else {
        return false;
    }
    return true;
}

bool DOMRefresh::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    if (style.first == DOM_REFRESH_PROGRESS_COLOR) {
        progressColor_.first = ParseColor(style.second);
        progressColor_.second = true;
        return true;
    }
    if (style.first == DOM_REFRESH_BACKGROUND_COLOR) {
        backgroundColor_ = ParseColor(style.second);
        refreshChild_->SetBackgroundColor(backgroundColor_);
        return true;
    }
    return false;
}

void DOMRefresh::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (columnChild_) {
        columnChild_->AppendChild(child->GetRootComponent());
    }
}

void DOMRefresh::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (columnChild_) {
        columnChild_->RemoveChild(child->GetRootComponent());
    }
}

bool DOMRefresh::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    LOGD("DOMRefresh start add event");
    if (event == DOM_REFRESH) {
        LOGD("DOMRefresh Add refresh Event");
        refreshEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        refreshChild_->SetRefreshEventId(refreshEventId_);
        return true;
    }
    if (event == DOM_REFRESH_EVENT_PULL_DOWN) {
        LOGD("DOMRefresh Add pulldown Event");
        pullDownEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        refreshChild_->SetPulldownEventId(pullDownEventId_);
        return true;
    }
    return false;
}

RefPtr<Component> DOMRefresh::GetSpecializedComponent()
{
    if (refreshChild_) {
        refreshChild_->SetChild(columnChild_);
        if (!progressColor_.second) {
            auto theme = GetTheme<ProgressTheme>();
            progressColor_.first = theme->GetProgressColor();
        }
        refreshChild_->SetProgressColor(progressColor_.first);
    }
    return refreshChild_;
}

void DOMRefresh::PrepareSpecializedComponent()
{
    refreshChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    columnChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
}

} // namespace OHOS::Ace::Framework
