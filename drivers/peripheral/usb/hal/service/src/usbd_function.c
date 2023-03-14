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
#include "usbd_function.h"

#include <hdf_sbuf.h>
#include <servmgr_hdi.h>
#include "devmgr_hdi.h"
#include "hdf_log.h"
#include "hdf_remote_service.h"
#include "osal_time.h"
#include "parameter.h"
#include "securec.h"

#define DEV_SERVICE_NAME "usbfn_master"
#define ACM_SERVICE_NAME "usbfn_cdcacm"
#define ECM_SERVICE_NAME "usbfn_cdcecm"

#define SYS_USB_CONFIGFS "sys.usb.configfs"
#define SYS_USB_CONFIG "sys.usb.config"
#define HDC_CONFIG_OFF "none"
#define HDC_CONFIG_ON "hdc"
#define HDC_CONFIGFS_OFF "0"
#define HDC_CONFIGFS_ON "1"

#define FUNCTION_ADD 1
#define FUNCTION_DEL 2

#define ACM_INIT 100
#define ACM_RELEASE 101
#define ECM_INIT 100
#define ECM_RELEASE 101

#define USB_FUNCTION_ACM_ECM 3
#define FUNCTIONS_MAX 7

static uint8_t currentFuncs = USB_FUNCTION_HDC;
static uint8_t WAIT_SLEEP_TIME = 10;

static int32_t SendCmdToService(const char *name, int32_t cmd, unsigned char funcMask)
{
    struct HdfRemoteService *service = NULL;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct HDIServiceManager *servmgr = NULL;
    servmgr = HDIServiceManagerGet();
    if (servmgr == NULL) {
        HDF_LOGE("%{public}s:%{public}d HDIServiceManagerGet err\n", __func__, __LINE__);
        return HDF_FAILURE;
    }
    service = servmgr->GetService(servmgr, name);
    HDIServiceManagerRelease(servmgr);
    if (service == NULL) {
        HDF_LOGE("%{public}s:%{public}d GetService err\n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (HdfRemoteServiceSetInterfaceDesc(service, HDF_USB_USBFN_DESC) == false) {
        HDF_LOGE("%{public}s:%{public}d set desc fail\n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    data = HdfSbufTypedObtain(SBUF_IPC);
    reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s:%{public}d data or reply err\n", __func__, __LINE__);
        HdfRemoteServiceRecycle(service);
        return HDF_FAILURE;
    }

    if (HdfRemoteServiceWriteInterfaceToken(service, data) == false) {
        HDF_LOGE("%{public}s:%{public}d write interface token fail\n", __func__, __LINE__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        HdfRemoteServiceRecycle(service);
        return HDF_FAILURE;
    }
    
    if (!HdfSbufWriteInt8(data, funcMask)) {
        HDF_LOGE("%{public}s:%{public}d HdfSbufWriteInt8 error\n", __func__, __LINE__);
        HdfSbufRecycle(data);
        HdfSbufRecycle(reply);
        HdfRemoteServiceRecycle(service);
        return HDF_FAILURE;
    }
    int32_t status = service->dispatcher->Dispatch(service, cmd, data, reply);
    if (status) {
        HDF_LOGW("%{public}s:%{public}d serice %{public}s dispatch cmd : %{public}d error\n", __func__, __LINE__, name,
                 cmd);
    }
    HdfSbufRecycle(data);
    HdfSbufRecycle(reply);
    HdfRemoteServiceRecycle(service);
    return status;
}

static int32_t RemoveHdc()
{
    uint8_t status = SetParameter(SYS_USB_CONFIG, HDC_CONFIG_OFF);
    if (status) {
        HDF_LOGE("%{public}s:%{public}d remove hdc config error = %{public}hhu\n", __func__, __LINE__, status);
        return HDF_FAILURE;
    }
    status = SetParameter(SYS_USB_CONFIGFS, HDC_CONFIGFS_OFF);
    if (status) {
        HDF_LOGE("%{public}s:%{public}d remove hdc configs error = %{public}hhu\n", __func__, __LINE__, status);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t AddHdc()
{
    uint8_t status = SetParameter(SYS_USB_CONFIGFS, HDC_CONFIGFS_ON);
    if (status) {
        HDF_LOGE("%{public}s:%{public}d add hdc configfs error = %{public}hhu\n", __func__, __LINE__, status);
        return HDF_FAILURE;
    }
    status = SetParameter(SYS_USB_CONFIG, HDC_CONFIG_ON);
    if (status) {
        HDF_LOGE("%{public}s:%{public}d add hdc config error = %{public}hhu\n", __func__, __LINE__, status);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SetFunctionToNone()
{
    if (RemoveHdc()) {
        HDF_LOGE("%{public}s:%{public}d RemoveHdc error ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    SendCmdToService(ACM_SERVICE_NAME, ACM_RELEASE, USB_FUNCTION_ACM);
    SendCmdToService(ECM_SERVICE_NAME, ECM_RELEASE, USB_FUNCTION_ECM);
    SendCmdToService(DEV_SERVICE_NAME, FUNCTION_DEL, USB_FUNCTION_ACM_ECM);
    currentFuncs = USB_FUNCTION_NONE;
    return HDF_SUCCESS;
}

static int32_t SetFunctionToACM()
{
    if (SendCmdToService(DEV_SERVICE_NAME, FUNCTION_ADD, USB_FUNCTION_ACM)) {
        HDF_LOGE("%{public}s:%{public}d create acm dev error ", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (SendCmdToService(ACM_SERVICE_NAME, ACM_INIT, USB_FUNCTION_ACM)) {
        HDF_LOGE("%{public}s:%{public}d acm init error ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SetFunctionToECM()
{
    if (SendCmdToService(DEV_SERVICE_NAME, FUNCTION_ADD, USB_FUNCTION_ECM)) {
        HDF_LOGE("%{public}s:%{public}d create ecm dev error ", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (SendCmdToService(ECM_SERVICE_NAME, ECM_INIT, USB_FUNCTION_ECM)) {
        HDF_LOGE("%{public}s:%{public}d ecm init error ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SetFunctionToACMECM()
{
    if (SendCmdToService(DEV_SERVICE_NAME, FUNCTION_ADD, USB_FUNCTION_ACM_ECM)) {
        HDF_LOGE("%{public}s:%{public}d create acm&ecm dev error ", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (SendCmdToService(ACM_SERVICE_NAME, ACM_INIT, USB_FUNCTION_ACM)) {
        HDF_LOGE("%{public}s:%{public}d acm init error ", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (SendCmdToService(ECM_SERVICE_NAME, ECM_INIT, USB_FUNCTION_ECM)) {
        HDF_LOGE("%{public}s:%{public}d ecm init dev error ", __func__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t UsbdSetFunction(int funcs)
{
    uint8_t _acm_ecm = funcs & USB_FUNCTION_ACM_ECM;
    if (funcs < USB_FUNCTION_NONE || funcs >= FUNCTIONS_MAX) {
        HDF_LOGI("%{public}s:%{public}d funcs invalid \n", __func__, __LINE__);
        return HDF_FAILURE;
    }

    if (SetFunctionToNone()) {
        HDF_LOGI("%{public}s:%{public}d setFunctionToNone error \n", __func__, __LINE__);
    }

    if (funcs & USB_FUNCTION_HDC) {
        if (AddHdc()) {
            HDF_LOGE("%{public}s:%{public}d AddHdc error ", __func__, __LINE__);
            return HDF_FAILURE;
        }
    }

    if (_acm_ecm == USB_FUNCTION_ACM) {
        if (SetFunctionToACM()) {
            HDF_LOGE("%{public}s:%{public}d set function to acm error ", __func__, __LINE__);
            return HDF_FAILURE;
        }
    } else if (_acm_ecm == USB_FUNCTION_ECM) {
        if (SetFunctionToECM()) {
            HDF_LOGE("%{public}s:%{public}d set function to ecm error ", __func__, __LINE__);
            return HDF_FAILURE;
        }
    } else if (_acm_ecm == USB_FUNCTION_ACM_ECM) {
        if (SetFunctionToACMECM()) {
            HDF_LOGE("%{public}s:%{public}d set function to acm&ecm error ", __func__, __LINE__);
            return HDF_FAILURE;
        }
    }
    OsalMSleep(WAIT_SLEEP_TIME);
    currentFuncs = funcs;
    return HDF_SUCCESS;
}

int32_t UsbdGetFunction(void)
{
    return currentFuncs;
}
