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

#include "picker_animation_controller.h"

namespace OHOS::Ace {

void PickerAnimationController::CreateTriangleAnimation()
{
    WeakPtr<PickerAnimationController> weak = AceType::WeakClaim(this);
    // it is fixed. time start from 0.0ms to 300ms. angle start from 0.0 to -PI which is 3.14
    inTriangle_ =
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, -3.14, 0, 300, Curves::SHARP, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto triangle = ref->triangle_.Upgrade();
        if (!triangle) {
            return;
        }
        triangle->UpdateAngle(value);
    });
    // it is fixed. time start from 0.0ms to 300ms. angle start from -PI (3.14) to 0.0
    outTriangle_ = AceType::MakeRefPtr<PickerAnimation>(pipe_, -3.14, 0.0, 0, 300, Curves::SHARP, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto triangle = ref->triangle_.Upgrade();
        if (!triangle) {
            return;
        }
        triangle->UpdateAngle(value);
    });
}

void PickerAnimationController::CreateMovingAnimation()
{
    WeakPtr<PickerAnimationController> weak = AceType::WeakClaim(this);
    inYearMoving_ = // position moving from 0% to 100% (1.0), time from 0ms to 500ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 1.0, 0, 500, Curves::FRICTION, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto pickerColumn = ref->year_.Upgrade();
        if (!pickerColumn) {
            LOGW("year column is null in picker animation.");
            return;
        }
        value = ref->yearStart_ + (ref->yearEnd_ - ref->yearStart_) * value;
        pickerColumn->UpdateOffset(value);
    });
    inMonthMoving_ = // position from 0% to 100% (1.0), time from 0ms to 500ms
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 1.0, 0, 500, Curves::FRICTION, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        value = ref->monthStart_ + (ref->monthEnd_ - ref->monthStart_) * value;
        auto pickerColumn = ref->month_.Upgrade();
        if (pickerColumn) {
            pickerColumn->UpdateOffset(value);
        } else {
            LOGW("month column is null in picker animation.");
        }
        pickerColumn = ref->monthDay_.Upgrade();
        if (pickerColumn) {
            pickerColumn->UpdateOffset(value + ref->monthEnd_ - ref->monthStart_);
        }
    });
    inDayMoving_ = // positin from 0% to 100% (1.0), time from 0ms to 500ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 1.0, 0, 500, Curves::FRICTION, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto pickerColumn = ref->day_.Upgrade();
        if (!pickerColumn) {
            LOGW("day column is null in picker animation.");
            return;
        }
        value = ref->dayStart_ + (ref->dayEnd_ - ref->dayStart_) * value;
        pickerColumn->UpdateOffset(value);
    });

    outYearMoving_ = // position from 0% to 100% (1.0), time from 0ms to 500ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 1.0, 0, 500, Curves::FRICTION, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto pickerColumn = ref->year_.Upgrade();
        if (!pickerColumn) {
            return;
        }
        value = ref->yearEnd_ + (ref->yearStart_ - ref->yearEnd_) * value;
        pickerColumn->UpdateOffset(value);
    });
    outMonthMoving_ = // posotion from 0% to 100% (1.0), time from 0ms to 500ms
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 1.0, 0, 500, Curves::FRICTION, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        value = ref->monthEnd_ + (ref->monthStart_ - ref->monthEnd_) * value;
        auto pickerColumn = ref->month_.Upgrade();
        if (pickerColumn) {
            pickerColumn->UpdateOffset(value);
        }
        pickerColumn = ref->monthDay_.Upgrade();
        if (pickerColumn) {
            pickerColumn->UpdateOffset(value + ref->monthEnd_ - ref->monthStart_);
        }
    });
    outDayMoving_ = // position from 0% to 100% (1.0), time from 0ms to 500ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 1.0, 0, 500, Curves::FRICTION, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto pickerColumn = ref->day_.Upgrade();
        if (!pickerColumn) {
            return;
        }
        value = ref->dayEnd_ + (ref->dayStart_ - ref->dayEnd_) * value;
        pickerColumn->UpdateOffset(value);
    });
}

void PickerAnimationController::CreateInOpacityAnimation()
{
    WeakPtr<PickerAnimationController> weak = AceType::WeakClaim(this);
    inOldColumnOpacity_ = // opacity from 255 to 0.0, time from 0ms to 150ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 255.0, 0.0, 0, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto column = ref->monthDay_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->amPm_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->hour_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->minute_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->second_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
    });
    inMonthDayOpacity_ = // opacity from 0.0 to 255, time from 0ms to 150ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 255.0, 0, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto column = ref->month_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->day_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
    });
    inYearOpacity_ = // opacity from 0.0 to 255, time from 50ms to 150ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 255.0, 100, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto column = ref->year_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
    });
    inLunarButtonsOpacity_ = // opacity from 255 to 0, time from 0ms to 150ms.
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 255.0, 0.0, 0, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto lunar = ref->lunar_.Upgrade();
        if (lunar) {
            lunar->UpdateOpacity(static_cast<uint8_t>(value));
        } else {
            LOGW("lunar display is null in picker animation.");
        }
        auto buttons = ref->buttons_.Upgrade();
        if (buttons) {
            buttons->UpdateOpacity(static_cast<uint8_t>(value));
        } else {
            LOGW("buttons display is null in picker animation.");
        }
    });
}

void PickerAnimationController::InitColumnOpacity(const RefPtr<RenderPickerColumn>& oldColumn,
                                                  const RefPtr<RenderPickerColumn>& newColumn)
{
    if (newColumn != oldColumn && newColumn) {
        newColumn->UpdateColumnOpacity(0.0);
    }
}

void PickerAnimationController::CreateOutOpacityAnimation()
{
    WeakPtr<PickerAnimationController> weak = AceType::WeakClaim(this);
    outOldColumnOpacity_ = // opacity from 0 to 255, time from 150ms to 150ms
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 255.0, 150, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto column = ref->monthDay_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->amPm_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->hour_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->minute_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->second_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
    });
    outMonthDayOpacity_ = // opacity from 255 to 0, time from 100ms to 250ms
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 255.0, 0.0, 100, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto column = ref->month_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
        column = ref->day_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
    });
    outYearOpacity_ = // opacity from 255 to 0, time from 0ms to 150ms
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 255.0, 0.0, 0, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto column = ref->year_.Upgrade();
        if (column) {
            column->UpdateColumnOpacity(value);
        }
    });
    outLunarButtonsOpacity_ = // opacity from 0 to 255, time from 100ms to 250ms
        AceType::MakeRefPtr<PickerAnimation>(pipe_, 0.0, 255.0, 100, 150, Curves::LINEAR, [weak](double value) {
        auto ref = weak.Upgrade();
        if (!ref) {
            return;
        }
        auto lunar = ref->lunar_.Upgrade();
        if (lunar) {
            lunar->UpdateOpacity(static_cast<uint8_t>(value));
        }
        auto buttons = ref->buttons_.Upgrade();
        if (buttons) {
            buttons->UpdateOpacity(static_cast<uint8_t>(value));
        }
    });
}

void PickerAnimationController::CreateAnimation()
{
    CreateTriangleAnimation();
    CreateMovingAnimation();
    CreateInOpacityAnimation();
    CreateOutOpacityAnimation();
}

void PickerAnimationController::PlayInAnimation()
{
    if (inTriangle_) {
        inTriangle_->Play();
    }
    if (inYearMoving_) {
        inYearMoving_->Play();
    }
    if (inMonthMoving_) {
        inMonthMoving_->Play();
    }
    if (inDayMoving_) {
        inDayMoving_->Play();
    }
    if (inOldColumnOpacity_) {
        inOldColumnOpacity_->Play();
    }
    if (inMonthDayOpacity_) {
        auto month = month_.Upgrade();
        auto day = day_.Upgrade();
        if (month) {
            month->UpdateColumnOpacity(0.0);
        }
        if (day) {
            day->UpdateColumnOpacity(0.0);
        }
        inMonthDayOpacity_->Play();
    }
    if (inYearOpacity_) {
        auto year = year_.Upgrade();
        if (year) {
            year->UpdateColumnOpacity(0.0);
        }
        inYearOpacity_->Play();
    }
    if (inLunarButtonsOpacity_) {
        inLunarButtonsOpacity_->Play();
    }
}

void PickerAnimationController::PlayOutAnimation()
{
    if (outTriangle_) {
        outTriangle_->Play();
    }
    if (outYearMoving_) {
        outYearMoving_->Play();
    }
    if (outMonthMoving_) {
        outMonthMoving_->Play();
    }
    if (outDayMoving_) {
        outDayMoving_->Play();
    }
    if (outOldColumnOpacity_) {
        outOldColumnOpacity_->Play();
    }
    if (outMonthDayOpacity_) {
        outMonthDayOpacity_->Play();
    }
    if (outYearOpacity_) {
        outYearOpacity_->Play();
    }
    if (outLunarButtonsOpacity_) {
        outLunarButtonsOpacity_->Play();
    }
}

void PickerAnimationController::StopInAnimation()
{
    if (inTriangle_) {
        inTriangle_->Stop();
    }
    if (inYearMoving_) {
        inYearMoving_->Stop();
    }
    if (inMonthMoving_) {
        inMonthMoving_->Stop();
    }
    if (inDayMoving_) {
        inDayMoving_->Stop();
    }
    if (inOldColumnOpacity_) {
        inOldColumnOpacity_->Stop();
    }
    if (inMonthDayOpacity_) {
        inMonthDayOpacity_->Stop();
    }
    if (inYearOpacity_) {
        inYearOpacity_->Stop();
    }
    if (inLunarButtonsOpacity_) {
        inLunarButtonsOpacity_->Stop();
    }
}

void PickerAnimationController::StopOutAnimation()
{
    if (outTriangle_) {
        outTriangle_->Stop();
    }
    if (outYearMoving_) {
        outYearMoving_->Stop();
    }
    if (outMonthMoving_) {
        outMonthMoving_->Stop();
    }
    if (outDayMoving_) {
        outDayMoving_->Stop();
    }
    if (outOldColumnOpacity_) {
        outOldColumnOpacity_->Stop();
    }
    if (outMonthDayOpacity_) {
        outMonthDayOpacity_->Stop();
    }
    if (outYearOpacity_) {
        outYearOpacity_->Stop();
    }
    if (outLunarButtonsOpacity_) {
        outLunarButtonsOpacity_->Stop();
    }
}

void PickerAnimationController::InitMoveRange()
{
    yearEnd_ = 0.0;
    monthEnd_ = 0.0;
    dayEnd_ = 0.0;

    auto year = year_.Upgrade();
    auto month = month_.Upgrade();
    auto day = day_.Upgrade();
    auto monthDay = monthDay_.Upgrade();
    if (!monthDay) {
        LOGE("month day column is null.");
        return;
    }

    double position = monthDay->GetInnerColumnGlobalOffset().GetX();
    if (year) {
        yearStart_ = position - year->GetInnerColumnGlobalOffset().GetX();
    }
    if (month) {
        monthStart_ = position - month->GetInnerColumnGlobalOffset().GetX();
    }
    if (day) {
        dayStart_ = position - day->GetInnerColumnGlobalOffset().GetX();
    }
}

void PickerAnimationController::Play(bool isIn)
{
    if (!created_) {
        InitMoveRange();
        CreateAnimation();
        created_ = true;
    }

    if (isIn) {
        if (outDayMoving_) {
            outDayMoving_->ClearStopCallback();
        }
        StopOutAnimation();
        PlayInAnimation();
    } else {
        StopInAnimation();
        if (outDayMoving_ && outStopCallback_) {
            outDayMoving_->AddStopCallback(outStopCallback_);
        }
        PlayOutAnimation();
    }
}

} // namespace OHOS::Ace

