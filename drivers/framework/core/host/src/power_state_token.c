/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "power_state_token.h"
#include "devmgr_service_clnt.h"
#include "hdf_device_desc.h"
#include "hdf_slist.h"
#include "osal_mem.h"
#include "hdf_power_state.h"

static void PowerStateTokenOnFirstAcquire(struct HdfSRef *sref)
{
    struct PowerStateToken *stateToken = NULL;
    if (sref == NULL) {
        return;
    }
    stateToken = (struct PowerStateToken *)HDF_SLIST_CONTAINER_OF(
        struct HdfSRef, sref, struct PowerStateToken, wakeRef);

    if (stateToken->psmState == PSM_STATE_ACTIVE) {
        return;
    }

    if (stateToken->psmState == PSM_STATE_INACTIVE || stateToken->psmState == PSM_STATE_IDLE) {
        const struct IPowerEventListener *listener = stateToken->listener;
        if ((listener != NULL) && (listener->Resume != NULL)) {
            listener->Resume(stateToken->deviceObject);
        }
    }
    stateToken->psmState = PSM_STATE_ACTIVE;
}

static void PowerStateTokenOnLastRelease(struct HdfSRef *sref)
{
    struct PowerStateToken *stateToken = NULL;
    const struct IPowerEventListener *listener = NULL;
    if (sref == NULL) {
        return;
    }
    stateToken = (struct PowerStateToken *)HDF_SLIST_CONTAINER_OF(
        struct HdfSRef, sref, struct PowerStateToken, wakeRef);

    if (stateToken->psmState != PSM_STATE_ACTIVE && stateToken->psmState != PSM_STATE_IDLE) {
        return;
    }

    listener = stateToken->listener;
    if ((listener != NULL) && (listener->Suspend != NULL)) {
        listener->Suspend(stateToken->deviceObject);
    }
    stateToken->psmState = PSM_STATE_INACTIVE;
}

int PowerStateChange(struct PowerStateToken *stateToken, uint32_t pEvent)
{
    if (stateToken == NULL || stateToken->listener == NULL || stateToken->mode != HDF_POWER_SYS_CTRL) {
        return HDF_SUCCESS;
    }

    switch (pEvent) {
        case POWER_STATE_SUSPEND:
            if (stateToken->listener->Suspend != NULL) {
                return stateToken->listener->Suspend(stateToken->deviceObject);
            }
            break;
        case POWER_STATE_RESUME:
            if (stateToken->listener->Resume != NULL) {
                return stateToken->listener->Resume(stateToken->deviceObject);
            }
            break;
        case POWER_STATE_DOZE_SUSPEND:
            if (stateToken->listener->DozeSuspend != NULL) {
                return stateToken->listener->DozeSuspend(stateToken->deviceObject);
            }
            break;
        case POWER_STATE_DOZE_RESUME:
            if (stateToken->listener->DozeResume != NULL) {
                return stateToken->listener->DozeResume(stateToken->deviceObject);
            }
            break;
        default:
            break;
    }

    return HDF_SUCCESS;
}

static void PowerStateTokenAcquireWakeLock(struct IPowerStateToken *token)
{
    struct HdfSRef *sref = NULL;
    struct PowerStateToken *stateToken = (struct PowerStateToken *)token;
    if (stateToken == NULL || stateToken->mode != HDF_POWER_DYNAMIC_CTRL) {
        return;
    }
    sref = (struct HdfSRef *)&stateToken->wakeRef;
    if ((sref != NULL) && (sref->Acquire != NULL)) {
        sref->Acquire(sref);
    }
}

static void PowerStateTokenReleaseWakeLock(struct IPowerStateToken *token)
{
    struct HdfSRef *sref = NULL;
    struct PowerStateToken *stateToken = (struct PowerStateToken *)token;
    if (stateToken == NULL || stateToken->mode != HDF_POWER_DYNAMIC_CTRL) {
        return;
    }
    sref = (struct HdfSRef *)&stateToken->wakeRef;
    if ((sref == NULL) || (sref->Release == NULL)) {
        return;
    }

    /* Not allowed to decrease the ref count to negative */
    if (HdfSRefCount(sref) == 0) {
        PowerStateTokenOnLastRelease(sref);
    } else {
        sref->Release(sref);
    }
}

static int32_t PowerStateTokenConstruct(struct PowerStateToken *powerStateToken,
    struct HdfDeviceObject *deviceObject, const struct IPowerEventListener *listener)
{
    struct IPowerStateToken *tokenIf = &powerStateToken->super;
    struct IHdfSRefListener *srefListener = (struct IHdfSRefListener *)OsalMemCalloc(sizeof(struct IHdfSRefListener));
    if (srefListener == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    tokenIf->AcquireWakeLock = PowerStateTokenAcquireWakeLock;
    tokenIf->ReleaseWakeLock = PowerStateTokenReleaseWakeLock;

    srefListener->OnFirstAcquire = PowerStateTokenOnFirstAcquire;
    srefListener->OnLastRelease = PowerStateTokenOnLastRelease;

    powerStateToken->psmState = PSM_STATE_IDLE;
    powerStateToken->listener = listener;
    powerStateToken->deviceObject = deviceObject;
    HdfSRefConstruct(&powerStateToken->wakeRef, srefListener);

    return HDF_SUCCESS;
}

struct PowerStateToken *PowerStateTokenNewInstance(
    struct HdfDeviceObject *deviceObject, const struct IPowerEventListener *listener)
{
    struct PowerStateToken *stateToken =
        (struct PowerStateToken *)OsalMemCalloc(sizeof(struct PowerStateToken));
    if (stateToken == NULL) {
        return NULL;
    }

    if (PowerStateTokenConstruct(stateToken, deviceObject, listener) != HDF_SUCCESS) {
        OsalMemFree(stateToken);
        return NULL;
    }

    return stateToken;
}

void PowerStateTokenFreeInstance(struct PowerStateToken *stateToken)
{
    if (stateToken != NULL) {
        if (stateToken->wakeRef.listener != NULL) {
            OsalMemFree(stateToken->wakeRef.listener);
            stateToken->wakeRef.listener = NULL;
        }
        OsalMemFree(stateToken);
    }
}
