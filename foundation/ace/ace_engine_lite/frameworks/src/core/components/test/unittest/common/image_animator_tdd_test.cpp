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

#include "image_animator_tdd_test.h"

#include "js_app_context.h"
#include "ui_image_animator.h"
#include "ui_view_group.h"

namespace OHOS {
namespace ACELite {
const char * const BUNDLE01 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('image-animator', {\n"
    "        staticClass: ['animator'],\n"
    "        attrs: {\n"
    "          ref: 'animator',\n"
    "          images: function images() {\n"
    "            return _vm.images;\n"
    "          },\n"
    "          duration: '1s'\n"
    "        }\n"
    "      })]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        animator: {\n"
    "          height: '80px',\n"
    "          width: '80px'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      images: [{\n"
    "        src: 'common/asserts/numbers/0.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/1.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/2.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/3.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/4.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/5.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/6.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/7.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/8.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/9.png'\n"
    "      }]\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char * const BUNDLE02 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('image-animator', {\n"
    "        staticClass: ['animator'],\n"
    "        attrs: {\n"
    "          ref: 'animator',\n"
    "          images: function images() {\n"
    "            return _vm.images;\n"
    "          },\n"
    "          duration: '1s',\n"
    "          iteration: 3,\n"
    "          reverse: true,\n"
    "          fixedsize: false,\n"
    "          fillmode: 'none'\n"
    "        }\n"
    "      })]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        animator: {\n"
    "          height: '100px',\n"
    "          width: '100px'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      images: [{\n"
    "        src: 'common/asserts/numbers/0.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/1.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/2.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/3.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/4.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/5.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/6.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/7.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/8.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/9.png',\n"
    "        width: 80,\n"
    "        height: 80,\n"
    "        left: 10,\n"
    "        right: 10\n"
    "      }]\n"
    "    }\n"
    "  });\n"
    "})();\n";

const char * const BUNDLE03 =
    "(function () {\n"
    "  return new ViewModel({\n"
    "    render: function render(vm) {\n"
    "      var _vm = vm || this;\n"
    "      return _c('div', {\n"
    "        staticClass: ['container']\n"
    "      }, [_c('image-animator', {\n"
    "        staticClass: ['animator'],\n"
    "        attrs: {\n"
    "          ref: 'animator',\n"
    "          images: function images() {\n"
    "            return _vm.images;\n"
    "          },\n"
    "          duration: '1s'\n"
    "        },\n"
    "        on: {\n"
    "          stop: _vm.handleStop\n"
    "        }\n"
    "      })]);\n"
    "    },\n"
    "    styleSheet: {\n"
    "      classSelectors: {\n"
    "        container: {\n"
    "          flexDirection: 'column',\n"
    "          justifyContent: 'center',\n"
    "          alignItems: 'center',\n"
    "          height: '100%',\n"
    "          width: '100%'\n"
    "        },\n"
    "        animator: {\n"
    "          height: '80px',\n"
    "          width: '80px'\n"
    "        }\n"
    "      }\n"
    "    },\n"
    "    data: {\n"
    "      stopped: false,\n"
    "      images: [{\n"
    "        src: 'common/asserts/numbers/0.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/1.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/2.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/3.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/4.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/5.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/6.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/7.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/8.png'\n"
    "      }, {\n"
    "        src: 'common/asserts/numbers/9.png'\n"
    "      }]\n"
    "    },\n"
    "    start: function start() {\n"
    "      this.$refs.animator.start();\n"
    "    },\n"
    "    stop: function stop() {\n"
    "      this.$refs.animator.stop();\n"
    "    },\n"
    "    pause: function pause() {\n"
    "      this.$refs.animator.pause();\n"
    "    },\n"
    "    resume: function resume() {\n"
    "      this.$refs.animator.resume();\n"
    "    },\n"
    "    getState: function getState() {\n"
    "      return this.$refs.animator.getState();\n"
    "    },\n"
    "    handleStop: function handleStop() {\n"
    "      this.stopped = true;\n"
    "    }\n"
    "  });\n"
    "})();\n";

void ImageAnimatorTddTest::RunTests()
{
    ImageAnimatorTest001();
    ImageAnimatorTest002();
    ImageAnimatorTest003();
}

void ImageAnimatorTddTest::ImageAnimatorTest001()
{
    TDD_CASE_BEGIN();
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("app/run/", "com.example.test", 0);
    JSValue page = CreatePage(BUNDLE01, strlen(BUNDLE01));
    UIViewGroup *wrapper = reinterpret_cast<UIViewGroup *>(GetViewByRef(page, "animator"));
    EXPECT_TRUE(wrapper != nullptr);
    if (wrapper != nullptr) {
        UIImageAnimatorView *animator = reinterpret_cast<UIImageAnimatorView *>(wrapper->GetChildrenHead());
        EXPECT_TRUE(animator != nullptr);
        if (animator != nullptr) {
            const int16_t wrapperSize = 80;
            const uint8_t imagseSize = 10;
            EXPECT_EQ(wrapperSize, wrapper->GetWidth());
            EXPECT_EQ(wrapperSize, wrapper->GetHeight());
            EXPECT_EQ(imagseSize, animator->GetImageAnimatorImageNum());
            EXPECT_TRUE(animator->IsRepeat());
            EXPECT_FALSE(animator->IsReverse());
            EXPECT_TRUE(animator->IsSizeFixed());
            EXPECT_TRUE(animator->GetFillMode());
        }
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void ImageAnimatorTddTest::ImageAnimatorTest002()
{
    TDD_CASE_BEGIN();
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("app/run/", "com.example.test", 0);
    JSValue page = CreatePage(BUNDLE02, strlen(BUNDLE02));
    UIViewGroup *wrapper = reinterpret_cast<UIViewGroup *>(GetViewByRef(page, "animator"));
    EXPECT_TRUE(wrapper != nullptr);
    if (wrapper != nullptr) {
        const int16_t wrapperSize = 100;
        EXPECT_EQ(wrapperSize, wrapper->GetWidth());
        EXPECT_EQ(wrapperSize, wrapper->GetHeight());
        UIImageAnimatorView *animator = reinterpret_cast<UIImageAnimatorView *>(wrapper->GetChildrenHead());
        EXPECT_TRUE(animator != nullptr);
        if (animator != nullptr) {
            const int16_t animatorSize = 80;
            const uint8_t imagesSize = 10;
            const uint8_t repeatTimes = 3;
            EXPECT_EQ(animatorSize, animator->GetWidth());
            EXPECT_EQ(animatorSize, animator->GetHeight());
            EXPECT_EQ(imagesSize, animator->GetImageAnimatorImageNum());
            EXPECT_FALSE(animator->IsRepeat());
            EXPECT_EQ(repeatTimes, animator->GetRepeatTimes());
            EXPECT_TRUE(animator->IsReverse());
            EXPECT_FALSE(animator->IsSizeFixed());
            EXPECT_FALSE(animator->GetFillMode());
        }
    }
    DestroyPage(page);
    TDD_CASE_END();
}

void ImageAnimatorTddTest::ImageAnimatorTest003()
{
    TDD_CASE_BEGIN();
    JsAppContext::GetInstance()->SetCurrentAbilityInfo("app/run/", "com.example.test", 0);
    JSValue page = CreatePage(BUNDLE03, strlen(BUNDLE03));
    UIViewGroup *wrapper = reinterpret_cast<UIViewGroup *>(GetViewByRef(page, "animator"));
    EXPECT_TRUE(wrapper != nullptr);
    if (wrapper != nullptr) {
        UIImageAnimatorView *animator = reinterpret_cast<UIImageAnimatorView *>(wrapper->GetChildrenHead());
        EXPECT_TRUE(animator != nullptr);
        if (animator != nullptr) {
            const char *propStopped = "stopped";
            const char *methodStart = "start";
            const char *methodPause = "pause";
            const char *methodResume = "resume";
            const char *methodStop = "stop";
            const char *methodGetState = "getState";
            const char *statePlaying = "playing";
            const char *statePaused = "paused";
            const char *stateStopped = "stopped";
            EXPECT_FALSE(JSObject::GetBoolean(page, propStopped));
            char *state = CallFuncAsString(page, methodGetState);
            EXPECT_STREQ(state, statePlaying);
            ACE_FREE(state);

            JSRelease(JSObject::Call(page, methodPause));
            state = CallFuncAsString(page, methodGetState);
            EXPECT_STREQ(state, statePaused);
            ACE_FREE(state);

            JSRelease(JSObject::Call(page, methodResume));
            state = CallFuncAsString(page, methodGetState);
            EXPECT_STREQ(state, statePlaying);
            ACE_FREE(state);

            JSRelease(JSObject::Call(page, methodStop));
            state = CallFuncAsString(page, methodGetState);
            EXPECT_STREQ(state, stateStopped);
            ACE_FREE(state);
            EXPECT_TRUE(JSObject::GetBoolean(page, propStopped));

            JSRelease(JSObject::Call(page, methodStart));
            state = CallFuncAsString(page, methodGetState);
            EXPECT_STREQ(state, statePlaying);
            ACE_FREE(state);
        }
    }
    DestroyPage(page);
    TDD_CASE_END();
}

char *ImageAnimatorTddTest::CallFuncAsString(JSValue target, const char * const funcName) const
{
    JSValue state = JSObject::Call(target, funcName);
    char *value = JSString::Value(state);
    JSRelease(state);
    return value;
}

#ifdef TDD_ASSERTIONS
/**
 * @tc.name: ImageAnimatorTest001
 * @tc.desc:
 */
HWTEST_F(ImageAnimatorTddTest, ImageAnimatorTest001, TestSize.Level1)
{
    ImageAnimatorTddTest::ImageAnimatorTest001();
}

/**
 * @tc.name: ImageAnimatorTest002
 * @tc.desc:
 */
HWTEST_F(ImageAnimatorTddTest, ImageAnimatorTest002, TestSize.Level0)
{
    ImageAnimatorTddTest::ImageAnimatorTest002();
}

/**
 * @tc.name: ImageAnimatorTest003
 * @tc.desc:
 */
HWTEST_F(ImageAnimatorTddTest, ImageAnimatorTest003, TestSize.Level1)
{
    ImageAnimatorTddTest::ImageAnimatorTest003();
}
#endif

} // namespace ACELite
} // namespace OHOS
