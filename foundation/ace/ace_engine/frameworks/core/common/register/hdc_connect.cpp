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

#include "base/log/log.h"
#include "frameworks/core/common/register/hdc_connect.h"
#include "frameworks/core/common/register/hdc_jdwp.h"

namespace OHOS::Ace {

std::unique_ptr<ConnectManagement> g_connectManagement = nullptr;
static uv_loop_t loopMain;
static HdcJdwpSimulator *clsHdcJdwpSimulator = nullptr;

void ConnectManagement::SetPkgName(const std::string &pkgName)
{
    pkgName_ = pkgName;
}

std::string ConnectManagement::GetPkgName()
{
    return pkgName_;
}

static void PrintMessage(const char *fmt, ...)
{
    int ret = 0;
    va_list ap;
    va_start(ap, fmt);
    ret = vfprintf(stdout, fmt, ap);
    ret = fprintf(stdout, "\n");
    va_end(ap);
}

void TryCloseHandle(const uv_handle_t* handle, bool alwaysCallback, uv_close_cb closeCallBack)
{
    bool hasCallClose = false;
    if (handle->loop && !uv_is_closing(handle)) {
        uv_close((uv_handle_t *)handle, closeCallBack);
        hasCallClose = true;
    }
    if (!hasCallClose && alwaysCallback) {
        closeCallBack((uv_handle_t *)handle);
    }
}

void TryCloseHandle(const uv_handle_t *handle, uv_close_cb closeCallBack)
{
    TryCloseHandle(handle, false, closeCallBack);
}

void TryCloseHandle(const uv_handle_t *handle)
{
    TryCloseHandle(handle, nullptr);
}

bool TryCloseLoop(uv_loop_t *ptrLoop, const char *callerName)
{
    uint8_t closeRetry = 0;
    bool ret = false;
    constexpr int maxRetry = 3;
    constexpr int maxHandle = 2;
    for (closeRetry = 0; closeRetry < maxRetry; ++closeRetry) {
        if (uv_loop_close(ptrLoop) == UV_EBUSY) {
            if (closeRetry > maxRetry) {
                PrintMessage("%s close busy,try:%d", callerName, closeRetry);
            }

            if (ptrLoop->active_handles >= maxHandle) {
                PrintMessage("TryCloseLoop issue");
            }
            auto clearLoopTask = [](uv_handle_t *handle, void *arg) -> void {
                TryCloseHandle(handle);
            };
            uv_walk(ptrLoop, clearLoopTask, nullptr);
            // If all processing ends, Then return0,this call will block
            if (!ptrLoop->active_handles) {
                ret = true;
                break;
            }
            if (!uv_run(ptrLoop, UV_RUN_ONCE)) {
                ret = true;
                break;
            }
        } else {
            ret = true;
            break;
        }
    }
    return ret;
}

void FreeInstance()
{
    if (clsHdcJdwpSimulator != nullptr) {
        delete clsHdcJdwpSimulator;
        clsHdcJdwpSimulator = nullptr;
    }
    uv_stop(&loopMain);
    TryCloseLoop(&loopMain, "Hdcjdwp exit");
    LOGI("jdwp_process exit.");
    PrintMessage("jdwp_process exit.");
}

void Stop(int signo)
{
    FreeInstance();
    _exit(0);
}

void StopConnect()
{
#ifdef JS_JDWP_CONNECT
    FreeInstance();
#endif // JS_JDWP_CONNECT
}

void* HdcConnectRun(void* pkgContent)
{
    uv_loop_init(&loopMain);
    LOGI("jdwp_process start.");
    PrintMessage("jdwp_process start.");
    if (signal(SIGINT, Stop) == SIG_ERR) {
        PrintMessage("jdwp_process signal fail.");
    }
    std::string pkgName = static_cast<ConnectManagement*>(pkgContent)->GetPkgName();
    clsHdcJdwpSimulator = new (std::nothrow) HdcJdwpSimulator(&loopMain, pkgName);
    if (!clsHdcJdwpSimulator->Connect()) {
        PrintMessage("Connect fail.");
        return nullptr;
    }
    uv_run(&loopMain, UV_RUN_DEFAULT);
    return nullptr;
}

void StartConnect(const std::string& pkgName)
{
    pthread_t tid;
    g_connectManagement = std::make_unique<ConnectManagement>();
    g_connectManagement->SetPkgName(pkgName);
    if (pthread_create(&tid, nullptr, &HdcConnectRun, static_cast<void*>(g_connectManagement.get())) != 0) {
        LOGE("pthread_create fail!");
        return;
    }
}
} // namespace OHOS::Ace
