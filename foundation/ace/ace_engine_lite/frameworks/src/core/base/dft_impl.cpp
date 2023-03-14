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

#include "dft_impl.h"

#if ((OHOS_ACELITE_PRODUCT_WATCH == 1) || (FEATURE_CUSTOM_ENTRY_PAGE == 1))
#include "ace_log.h"
#include "js_app_context.h"

namespace OHOS {
namespace ACELite {
DftImpl *DftImpl::GetInstance()
{
    static DftImpl instance;
    return &instance;
}

char *DftImpl::GetPagePath()
{
    const char * const pagePath = JsAppContext::GetInstance()->GetCurrentJsPath();
    if (pagePath == nullptr) {
        return nullptr;
    }

    // remove suffix from js
    // pages/index/index.js -> pages/index/index
    size_t suffixLen = 3;
    size_t len = strlen(pagePath) - suffixLen;
    char *path = static_cast<char *>(ace_malloc(len + 1));
    if (path == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc buffer for path failed.");
        return nullptr;
    }
    if (memcpy_s(path, len, pagePath, len) != EOK) {
        ace_free(path);
        path = nullptr;
        return nullptr;
    }
    path[len] = '\0';
    return path;
}

void DftImpl::RegisterPageReplaced(PageReplacedCallback handler)
{
    pageReplacedCallback_ = handler;
}

void DftImpl::RegisterPageChangedCallback(PageInfoFunc pageInfoFunc)
{
    pageInfoFunc_ = pageInfoFunc;
}

void DftImpl::CallbackPageReplaced(int state, const char *param)
{
    if (pageReplacedCallback_ != nullptr) {
        char *currentPath = DftImpl::GetPagePath();
        pageReplacedCallback_(currentPath, state);
        ACE_FREE(currentPath);
    }
    if (pageInfoFunc_ == nullptr) {
        return;
    }
    Param value;
    char *paramValue = nullptr;
    jerry_value_t global = jerry_get_global_object();
    if (jerryx_has_property_str(global, ROUTER_PAGE)) {
        jerry_value_t param = jerryx_get_property_str(global, ROUTER_PAGE);
        jerry_value_t paramJson = jerry_json_stringify(param);
        paramValue = MallocStringOf(paramJson);
        ReleaseJerryValue(paramJson, param, VA_ARG_END_FLAG);
    }
    jerry_release_value(global);
    value.routerParam = paramValue;
    value.path = JsAppContext::GetInstance()->GetCurrentJsPath();
    value.routerPath = DftImpl::GetPagePath();
    pageInfoFunc_(value);
    ACE_FREE(paramValue);
}
} // namespace ACELite
} // namespace OHOS
#endif
