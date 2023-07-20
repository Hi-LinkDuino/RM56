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

#ifndef OHOS_ACELITE_JS_APP_CONTEXT_H
#define OHOS_ACELITE_JS_APP_CONTEXT_H
#include "acelite_config.h"
#include "lazy_load_manager.h"
#include "stylemgr/app_style_manager.h"
#include <list>

namespace OHOS {
namespace ACELite {
class JSAbilityImpl;

struct StyleManageInfo: public MemoryHeap{
    AppStyleManager* manager;
    char * jsPath;
};


/**
 * @brief Global App context.
 */
class JsAppContext final : public MemoryHeap {
public:
    static JsAppContext *GetInstance()
    {
        static JsAppContext instance;
        return &instance;
    }

    /**
     * @brief trigger terminate request
     */
    void TerminateAbility() const;
    /**
     * @brief eval user's JS Code and return FeatureAbility object
     *
     * @param: fullPath js file full path
     * @param: fullPathLength the given file name length
     * @param: the flag for app eval or page eval.True is eval flag.
     */
    jerry_value_t Eval(char *fullPath, size_t fullPathLength, bool isAppEval) const;
    /**
     * @brief call FeatureAbility's render function
     *
     * @param: page instance, viewModel to call render on
     */
    jerry_value_t Render(jerry_value_t viewModel) const;
    /**
     * @brief set the ability path, bundle name and token info for current ability
     *
     * @param: ability's path, bundle name and token info
     */
    void SetCurrentAbilityInfo(const char * const abilityPath, const char * const bundleName, uint16_t token);
    /**
     * @brief return ability path
     */
    const char *GetCurrentAbilityPath() const
    {
        return currentAbilityPath_;
    }
    /**
     * @brief set the js path and uuid info for current ability
     *
     * @param: js's path
     */
    void SetCurrentJsPath(const char * const jsPath);
    /**
     * @brief return js path
     */
    const char *GetCurrentJsPath() const
    {
        return currentJsPath_;
    }
    
    void SetTempJsPath(const char * jsPath)
    {
        styleManage_ = nullptr;
        tempJsPath_ = jsPath;
    }

    /**
     * @brief return current bundle name
     */
    const char *GetCurrentBundleName() const
    {
        return currentBundleName_;
    }

    const JSAbilityImpl *GetTopJSAbilityImpl() const
    {
        return topJSAbilityImpl_;
    }
    /**
     * @brief return current ability implementation
     */
    void SetTopJSAbilityImpl(JSAbilityImpl *object)
    {
        topJSAbilityImpl_ = object;
    }
    void LoadApiVersion();
    int32_t GetCompatibleApi() const;
    void SetCompatibleApi(int32_t compatibleApi);
    int32_t GetTargetApi() const;
    void SetTargetApi(int32_t targetApi);

    const AppStyleManager *GetStyleManager();

    void ReleaseStyles(const char * jsPath);

    /*
     * @brief: clear app env.
     */
    void ClearContext();

    const LazyLoadManager *GetLazyLoadManager()
    {
        if (lazyLoadManager_ == nullptr) {
            lazyLoadManager_ = new LazyLoadManager();
        }
        return lazyLoadManager_;
    }

    void ReleaseLazyLoadManager()
    {
        if (lazyLoadManager_) {
            delete lazyLoadManager_;
            lazyLoadManager_ = nullptr;
        }
    }

    /**
     * @brief 移除指定的数据监听器
     * @param pageRootView 指定页面的根 view
     */
    void RemovePageWatchers(const UIView* const pageRootView = nullptr)
    {
        if (lazyLoadManager_ == nullptr) return;

        lazyLoadManager_->RemoveLazyWatcher(pageRootView);

        if (pageRootView == nullptr || lazyLoadManager_->IsWatcherEmpty()) {
            ReleaseLazyLoadManager();
            return;
        }
    }

    char *GetResourcePath(const char *uri) const;

    uint16_t GetCurrentAbilityToken() const
    {
        return currentToken_;
    }

private:
    /**
     * @brief: release the ability info saved
     */
    void ReleaseAbilityInfo();

    void SetGlobalNamedProperty(bool isAppEval, jerry_value_t viewModel) const;
    /**
     * @brief try read the target mode file content, if failed, change to read another mode
     *
     * @param: isSnapshotMode target mode, can be adjusted to the proper mode
     * @param: outLength the reading content length
     * @param: fullPath js file full path
     * @param: fullPathLength the given file name length
     *
     * @return the target mode file content or nullptr for reading failure
     */
    char *EvaluateFile(bool &isSnapshotMode, uint32_t &outLength, char *fullPathPath, size_t fullPathLength) const;
    void CheckSnapshotVersion(const char *bcFileContent, uint32_t contentLength) const;
    char *ProcessResourcePathByConfiguration(size_t origUriLength, const char *filePath) const;
    char *currentBundleName_ = nullptr;
    char *currentAbilityPath_ = nullptr;
    char *currentJsPath_ = nullptr;
    const char *tempJsPath_ = nullptr; // for fragment
    JSAbilityImpl *topJSAbilityImpl_ = nullptr;
    std::list<StyleManageInfo*> *styleList_ = nullptr;
    AppStyleManager *styleManage_ = nullptr;
    LazyLoadManager *lazyLoadManager_ = nullptr;
    // record current running ability's uuid && ability path, will be release during app-cleanup
    uint16_t currentToken_ = 0;
    int32_t compatibleApi_ = 0;
    int32_t targetApi_ = 0;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_JS_APP_CONTEXT_H
