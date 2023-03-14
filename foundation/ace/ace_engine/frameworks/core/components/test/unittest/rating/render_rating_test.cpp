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

#include "gtest/gtest.h"

#include "base/utils/system_properties.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "frameworks/core/components/rating/rating_component.h"
#include "frameworks/core/components/test/json/json_frontend.h"
#include "frameworks/core/event/key_event.h"
#include "frameworks/core/image/image_object.h"
#include "frameworks/core/mock/mock_resource_register.h"
#define protected public
#include "frameworks/core/components/rating/render_rating.h"
#include "frameworks/core/components/test/unittest/mock/mock_render_depend.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

using RatingEventCallback = std::function<void(const std::string&, const std::string&)>;
using UpdateRatingCallback = std::function<void(const RefPtr<RatingComponent>&)>;
const UpdateRatingCallback EMPTY_CALLBACK;

constexpr int32_t SURFACE_WIDTH = 1080;
constexpr int32_t SURFACE_HEIGHT = 2244;

constexpr uint32_t TEST_RATING_STAR_NUM = 5;
constexpr double TEST_RATING_SCORE = 3.0;
constexpr double TEST_RATING_STEP_SIZE = 1.5;
constexpr double TEST_RATING_STEP_SIZE_FOR_TV = 1.0;
constexpr double TEST_RATING_BIG_STEP_SIZE = 6;
constexpr double TEST_RATING_BIG_SCORE = 6;
constexpr double TEST_SINGLE_STAR_WIDTH = 216.0;
constexpr uint32_t TEST_INVALID_RATING_STAR_NUM = -1;
constexpr double TEST_INVALID_RATING_SCORE = -1.0;
constexpr double TEST_INVALID_RATING_STEP_SIZE = -1.0;
constexpr uint32_t TEST_DEFAULT_RATING_STAR_NUM = 5;
constexpr double TEST_DEFAULT_RATING_SCORE = 0.0;
constexpr double TEST_DEFAULT_RATING_STEP_SIZE = 0.5;

const KeyEvent KEY_EVENT_RIGHT(KeyCode::KEY_DPAD_RIGHT, KeyAction::CLICK);
const KeyEvent KEY_EVENT_LEFT(KeyCode::KEY_DPAD_LEFT, KeyAction::CLICK);
const KeyEvent KEY_EVENT_ENTER(KeyCode::KEY_ENTER, KeyAction::CLICK);

const TouchEvent MOCK_DOWN_TOUCH_EVENT { 10, 648, 20, TouchType::DOWN };
const TouchEvent MOCK_MOVE_TOUCH_EVENT { 10, 648, 20, TouchType::MOVE };
const TouchEvent MOCK_UP_TOUCH_EVENT { 10, 648, 20, TouchType::UP };
enum class DragDirection {
    LEFT,
    RIGHT,
};

int64_t g_timeStamp = 0;

int64_t GetTickCount()
{
    return g_timeStamp;
}

class TestRatingEventHandler : public MockEventHandler {
public:
    explicit TestRatingEventHandler(const RatingEventCallback& eventCallback) : eventCallback_(eventCallback) {}
    ~TestRatingEventHandler() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, param);
        }
    };

private:
    RatingEventCallback eventCallback_;
};

} // namespace

class MockImageCache : public ImageCache {
    void Clear() override {};
};

RefPtr<ImageCache> ImageCache::Create()
{
    return AceType::MakeRefPtr<MockImageCache>();
}

void ImageCache::Purge() {}

bool IsTv()
{
    return SystemProperties::GetDeviceType() == DeviceType::TV;
}

class RatingComponentTest : public testing::Test {
public:
    static void SetUpTestCase() {}

    static void TearDownTestCase() {}

    void SetUp()
    {
        auto platformWindow = PlatformWindow::Create(nullptr);
        auto window = std::make_unique<Window>(std::move(platformWindow));
        auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
        auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
        auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
        frontend_ = Frontend::CreateDefault();
        context_ = AceType::MakeRefPtr<PipelineContext>(
            std::move(window), taskExecutor, assetManager, resRegister, frontend_, 0);
    }
    void TearDown() {}

protected:
    RefPtr<PipelineContext> context_;
    RefPtr<Frontend> frontend_;
    void CreateAndRenderRating(UpdateRatingCallback callback) const;
    void DragRating(const DragDirection& dragDirection, double dragOffset) const;
};

void RatingComponentTest::CreateAndRenderRating(UpdateRatingCallback updateRating) const
{
    RefPtr<RatingComponent> rating = AceType::MakeRefPtr<RatingComponent>();

    if (updateRating) {
        updateRating(rating);
    }

    auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", rating);

    context_->SetupRootElement();
    context_->PushPage(pageComponent);
    context_->OnVsyncEvent(GetTickCount(), 0);
    context_->OnSurfaceChanged(SURFACE_WIDTH, SURFACE_HEIGHT);
}

void RatingComponentTest::DragRating(const DragDirection& dragDirection, double dragOffset) const
{
    context_->OnTouchEvent(MOCK_DOWN_TOUCH_EVENT);
    TouchEvent touchPoint = MOCK_MOVE_TOUCH_EVENT;
    if (dragDirection == DragDirection::LEFT) {
        touchPoint.x -= dragOffset;
    } else if (dragDirection == DragDirection::RIGHT) {
        touchPoint.x += dragOffset;
    }
    context_->OnTouchEvent(touchPoint);
    TouchEvent touchEndPoint = MOCK_UP_TOUCH_EVENT;
    touchEndPoint.x = touchPoint.x;
    context_->OnTouchEvent(touchEndPoint);
    context_->OnVsyncEvent(GetTickCount(), 0);
}

/**
 * @tc.name: RatingOperation001
 * @tc.desc: Test when rating component receives drag event, score is increased by stepsize and won't exceed starNum
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = TEST_RATING_SCORE;
    score += IsTv() ? TEST_RATING_STEP_SIZE_FOR_TV : TEST_RATING_STEP_SIZE;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);   // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);  // ratingScore = 3
        rating->SetStepSize(TEST_RATING_STEP_SIZE); // stepsize = 1.5
    });

    /**
     * @tc.steps: step2. Drag from third star to fourth star.
     * @tc.expected: step2. Event of change of score triggers, score is 4.5 on phone (score is 4 on tv).
     */
    DragRating(DragDirection::RIGHT, TEST_SINGLE_STAR_WIDTH);

    /**
     * @tc.steps: step3. Drag forward from third star to area beyond rating zone.
     * @tc.expected: step3. Event of change of score triggers, score is 5.
     */
    score = TEST_RATING_STAR_NUM;
    DragRating(DragDirection::RIGHT, TEST_SINGLE_STAR_WIDTH * TEST_RATING_STAR_NUM);
}

/**
 * @tc.name: RatingOperation002
 * @tc.desc: Test when stepsize is greater than starNum, score won't exceed starNum
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = IsTv() ? TEST_RATING_SCORE + TEST_RATING_STEP_SIZE_FOR_TV : TEST_RATING_STAR_NUM;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);       // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);      // ratingScore = 3
        rating->SetStepSize(TEST_RATING_BIG_STEP_SIZE); // stepsize = 6
    });

    /**
     * @tc.steps: step2. Drag from third star to fourth star.
     * @tc.expected: step2. Event of change of score triggers, score is 5 on phone (score is 4 on tv).
     */
    DragRating(DragDirection::RIGHT, TEST_SINGLE_STAR_WIDTH);
}

/**
 * @tc.name: RatingOperation003
 * @tc.desc: Test while operating rating, if stepsize is greater than starNum, score won't be less than starNum
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = IsTv() ? TEST_RATING_STEP_SIZE_FOR_TV : TEST_RATING_STAR_NUM;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);       // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);      // ratingScore = 3
        rating->SetStepSize(TEST_RATING_BIG_STEP_SIZE); // stepsize = 6
    });

    /**
     * @tc.steps: step2. Drag from third star to zero star.
     * @tc.expected: step2. Event of change of score triggers, score is 5 on phone (score is 4 on tv).
     */
    DragRating(DragDirection::LEFT, TEST_SINGLE_STAR_WIDTH * TEST_RATING_SCORE);
}

/**
 * @tc.name: RatingOperation004
 * @tc.desc: Test while operating rating, if stepsize is less than starNum, score won't be less than stepsize and
 *           change event will trigger even if score doesn't change when click event occurs.
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = IsTv() ? TEST_RATING_STEP_SIZE_FOR_TV : TEST_RATING_STEP_SIZE;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);   // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);  // ratingScore = 3
        rating->SetStepSize(TEST_RATING_STEP_SIZE); // stepsize = 1.5
    });

    /**
     * @tc.steps: step2. Drag from third star to zero star.
     * @tc.expected: step2. Event of change of score triggers, score is 1.5 (score is 1 on tv).
     */
    DragRating(DragDirection::LEFT, TEST_SINGLE_STAR_WIDTH * TEST_RATING_SCORE);

    /**
     * @tc.steps: step3. Click star 1.
     * @tc.expected: step3. Event of change of score still triggers.
     */
    double scoreDelta = 2.0;
    TouchEvent touchPoint = MOCK_DOWN_TOUCH_EVENT;
    touchPoint.x -= TEST_SINGLE_STAR_WIDTH * scoreDelta;
    context_->OnTouchEvent(touchPoint);
    touchPoint = MOCK_UP_TOUCH_EVENT;
    touchPoint.x -= TEST_SINGLE_STAR_WIDTH * scoreDelta;
    context_->OnTouchEvent(touchPoint);
}

/**
 * @tc.name: RatingOperation005
 * @tc.desc: Test when rating component receives keyevent:right, score is increased by stepsize and won't exceed starNum
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = TEST_RATING_SCORE + TEST_RATING_STEP_SIZE;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);   // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);  // ratingScore = 3
        rating->SetStepSize(TEST_RATING_STEP_SIZE); // stepsize = 1.5
    });

    /**
     * @tc.steps: step2. Receive a keyevent:right and keyevent:enter.
     * @tc.expected: step2. Event of change of score triggers, score is 4.5.
     */
    context_->OnKeyEvent(KEY_EVENT_RIGHT);
    context_->OnKeyEvent(KEY_EVENT_ENTER);
}

/**
 * @tc.name: RatingOperation006
 * @tc.desc: Test when stepsize is greater than starNum, keyevent won't make score greater than starNum
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = TEST_RATING_STAR_NUM;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);       // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);      // ratingScore = 3
        rating->SetStepSize(TEST_RATING_BIG_STEP_SIZE); // stepsize = 6
    });

    /**
     * @tc.steps: step2. Receive a keyevent:right and keyevent:enter.
     * @tc.expected: step2. Event of change of score triggers, score is 5.
     */
    context_->OnKeyEvent(KEY_EVENT_RIGHT);
    context_->OnKeyEvent(KEY_EVENT_ENTER);
}

/**
 * @tc.name: RatingOperation007
 * @tc.desc: Test while operating rating via keyboard, if stepsize is greater than starNum,
 *           score won't be less than starNum.
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = TEST_RATING_STAR_NUM;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);       // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);      // ratingScore = 3
        rating->SetStepSize(TEST_RATING_BIG_STEP_SIZE); // stepsize = 6
    });

    /**
     * @tc.steps: step2. Receive a keyevent:left and keyevent:enter.
     * @tc.expected: step2. Event of change of score triggers, score is 5.
     */
    context_->OnKeyEvent(KEY_EVENT_LEFT);
    context_->OnKeyEvent(KEY_EVENT_ENTER);
}

/**
 * @tc.name: RatingOperation008
 * @tc.desc: Test while operating rating via keyboard, if stepsize is less than starNum, score won't be less than
 *           stepsize. And change event still triggers via keyevent:enter even if score doesn't change.
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingOperation008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Rating component tree, and add event listeners in Rating.
     */
    std::string changeEventId = "rating_change_id";
    double score = TEST_RATING_STEP_SIZE;
    RefPtr<TestRatingEventHandler> eventHandler = AceType::MakeRefPtr<TestRatingEventHandler>(
        [this, changeEventId, &score](const std::string& eventId, const std::string& param) {
            GTEST_LOG_(INFO) << param;
            std::string expectChangeResult =
                std::string("\"change\",{\"rating\":").append(std::to_string(score).append("}"));
            EXPECT_EQ(param, expectChangeResult);
            EXPECT_EQ(eventId, changeEventId);
        });

    context_->RegisterEventHandler(eventHandler);
    CreateAndRenderRating([this, changeEventId](const RefPtr<RatingComponent>& rating) {
        rating->SetChangeEventId(EventMarker(changeEventId));
        rating->SetStarNum(TEST_RATING_STAR_NUM);   // starNum = 5
        rating->SetRatingScore(TEST_RATING_SCORE);  // ratingScore = 3
        rating->SetStepSize(TEST_RATING_STEP_SIZE); // stepsize = 1.5
    });

    /**
     * @tc.steps: step2. Receive a keyevent:left and keyevent:enter.
     * @tc.expected: step2. Event of change of score triggers, score is 1.5.
     */
    context_->OnKeyEvent(KEY_EVENT_LEFT);
    context_->OnKeyEvent(KEY_EVENT_ENTER);

    /**
     * @tc.steps: step3. Receive a keyevent:left and keyevent:enter.
     * @tc.expected: step3. Event of change triggers.
     */
    context_->OnKeyEvent(KEY_EVENT_LEFT);
    context_->OnKeyEvent(KEY_EVENT_ENTER);
}

/**
 * @tc.name: RatingUpdate001
 * @tc.desc: Test rating component can handle valid params
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create a rating component and set valid params.
     */
    RefPtr<RatingComponent> rating = AceType::MakeRefPtr<RatingComponent>();
    rating->SetStepSize(TEST_RATING_STEP_SIZE);
    rating->SetStarNum(TEST_RATING_STAR_NUM);
    rating->SetRatingScore(TEST_RATING_SCORE);
    RefPtr<RenderRating> renderRating = AceType::MakeRefPtr<MockRenderRating>();
    renderRating->Attach(context_);
    renderRating->Update(rating);

    /**
     * @tc.steps: step2. Verify params after update.
     * @tc.expected: step2. Params are as expected.
     */
    ASSERT_TRUE(renderRating->stepSize_ == (IsTv() ? TEST_RATING_STEP_SIZE_FOR_TV : TEST_RATING_STEP_SIZE));
    ASSERT_TRUE(renderRating->starNum_ == TEST_RATING_STAR_NUM);
    ASSERT_TRUE(renderRating->ratingScore_ == TEST_RATING_SCORE);
}

/**
 * @tc.name: RatingUpdate002
 * @tc.desc: Test rating component can handle invalid params
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingUpdate002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create a rating component and set invalid params.
     */
    RefPtr<RatingComponent> rating = AceType::MakeRefPtr<RatingComponent>();
    rating->SetStepSize(TEST_INVALID_RATING_STEP_SIZE);
    rating->SetStarNum(TEST_INVALID_RATING_STAR_NUM);
    rating->SetRatingScore(TEST_INVALID_RATING_SCORE);
    RefPtr<RenderRating> renderRating = AceType::MakeRefPtr<MockRenderRating>();
    renderRating->Attach(context_);
    renderRating->Update(rating);

    /**
     * @tc.steps: step2. Verify params after update.
     * @tc.expected: step2. Params are as expected.
     */
    ASSERT_TRUE(renderRating->stepSize_ == (IsTv() ? TEST_RATING_STEP_SIZE_FOR_TV : TEST_DEFAULT_RATING_STEP_SIZE));
    ASSERT_TRUE(renderRating->starNum_ == TEST_DEFAULT_RATING_STAR_NUM);
    ASSERT_TRUE(renderRating->ratingScore_ == TEST_DEFAULT_RATING_SCORE);
}

/**
 * @tc.name: RatingUpdate003
 * @tc.desc: Test rating component can handle valid params that does not match
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB
 * @tc.author: chenxuankai
 */
HWTEST_F(RatingComponentTest, RatingUpdate003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create a rating component and set params that does not match.
     */
    RefPtr<RatingComponent> rating = AceType::MakeRefPtr<RatingComponent>();
    rating->SetStepSize(TEST_RATING_BIG_STEP_SIZE); // stepsize = 6, which exceeds starNum
    rating->SetStarNum(TEST_RATING_STAR_NUM);       // starNum = 5
    rating->SetRatingScore(TEST_RATING_BIG_SCORE);  // ratingScore = 6, which exceeds starNum
    RefPtr<RenderRating> renderRating = AceType::MakeRefPtr<MockRenderRating>();
    renderRating->Attach(context_);
    renderRating->Update(rating);

    /**
     * @tc.steps: step2. Verify params after update.
     * @tc.expected: step2. Params are as expected.
     */
    // stepsize is limited to starNum on phone (stepsize remains to be 1 on tv)
    ASSERT_TRUE(renderRating->stepSize_ == (IsTv() ? TEST_RATING_STEP_SIZE_FOR_TV : TEST_RATING_STAR_NUM));
    ASSERT_TRUE(renderRating->starNum_ == TEST_RATING_STAR_NUM);
    ASSERT_TRUE(renderRating->ratingScore_ == TEST_RATING_STAR_NUM); // ratingScore is limited to starNum
}

} // namespace OHOS::Ace