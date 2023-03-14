/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATOR_H

#include <list>

#include "base/utils/macros.h"
#include "core/animation/animation_pub.h"
#include "core/animation/interpolator.h"
#include "core/animation/motion.h"
#include "core/animation/scheduler.h"
#include "core/animation/status_listener.h"
#include "core/animation/time_event.h"
#include "core/components/common/properties/animation_option.h"

namespace OHOS::Ace {

class ACE_EXPORT Animator : public AceType, public StatusListenable {
    DECLARE_ACE_TYPE(Animator, AceType);

public:
    enum class Status {
        IDLE,     // when animation not start or been cancel.
        RUNNING,  // play in reverse / forward direction.
        PAUSED,   // paused by call Pause API.
        STOPPED,  // stopped by call Finish/Stop API or has played to the end.
    };

    // Adjust global animation duration, default scale is 1.0f.
    static void SetDurationScale(float scale);
    static float GetDurationScale();

    // Animator can play animations.
    // So far, animation has two types: Interpolator and Motion(physical-based animation).
    // But only one type will be played at a time. When playing one, the other's setting will be cleared.
    // 1. Interpolator: Play/Reverse/Stop/Finish will work
    // 2. Motion: PlayMotion will work
    explicit Animator(const WeakPtr<PipelineContext>& context);

    Animator();

    ~Animator() override;

    void AttachScheduler(const WeakPtr<PipelineContext>& context);
    bool HasScheduler() const;

    // Every interpolate animation needs to add itself into animator and use the controller to drive.
    void AddInterpolator(const RefPtr<Interpolator>& animation);
    void RemoveInterpolator(const RefPtr<Interpolator>& animation);
    void ClearInterpolators();

    // Controller (A) can be added to other controller (B) as a proxy when (B) is running.
    void AddProxyController(const RefPtr<Animator>& proxy);
    void RemoveProxyController(const RefPtr<Animator>& proxy);
    void ClearProxyControllers();

    // Can use these APIs to check controller's current state.
    Status GetStatus() const;
    bool IsStopped() const;
    bool IsRunning() const;
    bool IsPending() const;

    // Sets the total running time of the animator to drive the animation.
    void SetDuration(int32_t duration);
    int32_t GetDuration() const;
    void SetStartDelay(int32_t startDelay);

    // At last will run iteration loops. repeatTimes equals 0 as default.
    bool SetIteration(int32_t iteration);

    // fillmode used to decided the attr of last frame or first frame.
    void SetFillMode(FillMode fillMode);

    // tempo is used to control speed of animation.
    void SetTempo(float tempo);

    // init animation parameters with animationOption
    void ApplyOption(const AnimationOption& option);

    // Whether the animation should be played in reverse in turn.
    void SetAnimationDirection(AnimationDirection direction);

    // Set Whether or not the animator is allowed to run asynchronously off of the UI thread.
    void SetAllowRunningAsynchronously(bool runAsync);

    // Get whether or not the animator is allowed to run asynchronously off of the UI thread.
    bool GetAllowRunningAsynchronously();

    // Update the played time, will not trigger OnFrame callback.
    void UpdatePlayedTime(int32_t playedTime, bool checkReverse = false);
    int64_t GetPlayedTime() const;

    // Trigger one frame callback by the given time.
    void TriggerFrame(int32_t playedTime, bool checkReverse = false);

    // Motion not support set duration & repeat & Reverse.
    void PlayMotion(const RefPtr<Motion>& motion);

    // Play the Animation based current direction.
    void Play();

    // Reverse the Animation based current direction.
    void Reverse();

    // Means play forward and set direction reverse false.
    void Forward();

    // Means play backward and set direction reverse true.
    void Backward();

    // Stop at the current frame(Continueable stop).
    void Pause();

    // Resume animation from the pause frame.
    void Resume();

    // Stop at the current frame(Unrecoverable stop).
    void Stop();

    // Stop at the end frame.
    void Finish();

    // Stop at the start frame.
    void Cancel();

    // Get Controller Id.
    int32_t GetId() const;

private:
    // Screen refresh callback. duration is in millisecond.
    void OnFrame(int64_t duration);

    // Callback the played time to the interpolator animation.
    void NotifyInterpolator(int32_t playedTime);

    // Callback the played time to the motion animation.
    void NotifyMotion(int32_t playedTime);

    void StartInner(bool alwaysNotify);

    AnimationOption GetAnimationOption();

    bool IsSupportedRunningAsynchronously();

    bool StartAsync();

    bool StartInnerAsync();

    void StopInnerAsync();

    // Calculate played loops and remaining in playedTime
    int32_t GetPlayedLoopsAndRemaining(int32_t& playedTime);

    bool GetInitAnimationDirection();

    // update repeatTimesLeft_ and returns true if run out of repeat times.
    bool UpdateRepeatTimesLeftAndCheckFinished(int32_t playedLoops);

    void ToggleDirection();
    float GetNormalizedTime(float playedTime, bool needStop) const;

    void UpdateScaledTime();
    void UpdateIteration(int32_t iteration);

    void Copy(const RefPtr<Animator>& controller);

    std::list<RefPtr<Interpolator>> interpolators_;
    std::list<RefPtr<Animator>> proxyControllers_;
    RefPtr<Scheduler> scheduler_;
    RefPtr<Motion> motion_;
    FillMode fillMode_ = FillMode::FORWARDS;
    AnimationDirection direction_ = AnimationDirection::NORMAL;
    int32_t duration_ = 0;        // millisecond.
    int64_t elapsedTime_ = 0;     // millisecond. in range: 0 ~ startDelay_ + INTERPOLATE_DURATION_MAX
    int32_t startDelay_ = 0;      // millisecond.
    int32_t repeatTimes_ = 0;     // user configured repeat times.
    int32_t iteration_ = 1;       // user configured iteration times.
    int32_t repeatTimesLeft_ = 0; // repeat times for controller to play
    int32_t scaledDuration_ = 0;
    int32_t scaledStartDelay_ = 0;
    int asyncRunningAnimationCount_ = 0;
    bool isReverse_ = false;
    bool isResume_ = false;
    bool isCurDirection_ = false;
    bool toggleDirectionPending_ = false;
    bool allowRunningAsynchronously_ = false;
    Status status_ = Status::IDLE;
    int32_t controllerId_ = 0;
    static float scale_;
    float tempo_ = 1.0f;
    bool isBothBackwards = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATOR_H
