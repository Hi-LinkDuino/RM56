/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef REGULATOR_CORE_H
#define REGULATOR_CORE_H

#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "osal_mutex.h"
#include "regulator_if.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifdef REGULATOR_PRINTK_OPEN
#define REGULATOR_PRINT_LOG_DBG(fmt, arg...) \
dprintf("[HDF]-[REGULATOR]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##arg)
#define REGULATOR_PRINT_LOG_INF(fmt, arg...) \
dprintf("[HDF]-[REGULATOR]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##arg)
#define REGULATOR_PRINT_LOG_ERR(fmt, arg...) \
dprintf("[HDF]-[REGULATOR]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##arg)
#else
#define REGULATOR_PRINT_LOG_DBG(fmt, arg...) HDF_LOGD_WRAPPER("[HDF]-[REGULATOR]:" fmt "\r\n",  ##arg)
#define REGULATOR_PRINT_LOG_INF(fmt, arg...) HDF_LOGI_WRAPPER("[HDF]-[REGULATOR]:" fmt "\r\n",  ##arg)
#define REGULATOR_PRINT_LOG_ERR(fmt, arg...) HDF_LOGE_WRAPPER("[HDF]-[REGULATOR]:" fmt "\r\n",  ##arg)
#endif

#define CHECK_NULL_PTR_RETURN_VALUE(ptr, ret) do { \
    if ((ptr) == NULL) { \
        REGULATOR_PRINT_LOG_ERR("%s:line %d pointer is null and return ret", __func__, __LINE__); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_PTR_RETURN(ptr) do { \
    if ((ptr) == NULL) { \
        REGULATOR_PRINT_LOG_ERR("%s:line %d pointer is null and return", __func__, __LINE__); \
        return; \
    } \
} while (0)

#define CHECK_PARSER_RESULT_RETURN_VALUE(ret, str) do { \
    if ((ret) != HDF_SUCCESS) { \
        REGULATOR_PRINT_LOG_ERR("%s:line %d %s fail, ret = %d!", __func__, __LINE__, (str), (ret)); \
        return HDF_FAILURE; \
    } \
} while (0)

struct RegulatorStatusChangeInfo {
    const char *name;
    uint32_t status;
};
/**
 * @brief Defines a callback that will be invoked when a regulator's status change.
 *
 */
typedef int32_t (*RegulatorStatusChangecb)(struct RegulatorStatusChangeInfo *);

struct RegulatorConstraints {
    uint8_t alwaysOn;     /* the regulator is alwaysOn */
    uint8_t mode;         /* mode: voltage or current */
    uint32_t minUv;       /* min voltage that can be set by the regulator */
    uint32_t maxUv;       /* max voltage that can be set by the regulator */
    uint32_t minUa;       /* min current that can be set by the regulator */
    uint32_t maxUa;       /* max current that can be set by the regulator */
};

struct RegulatorDesc {
    const char *name;              /* regulator name */
    const char *parentName;        /* regulator parent name */
    struct RegulatorConstraints constraints;    /* the regulator constraint info */
    uint32_t minUv;                  /* min voltage */
    uint32_t maxUv;                  /* min voltage */
    uint32_t minUa;                  /* min current */
    uint32_t maxUa;                  /* max current */
    uint32_t status;                 /* the regulator's status, on or off */
    int useCount;
    int consumerRegNums;             /* the regulator's consumer nums */
    RegulatorStatusChangecb cb;      /* when regulator status change, can notify by call cb */
};

struct RegulatorNode {
    struct RegulatorDesc regulatorInfo;
    struct DListHead node;
    struct RegulatorMethod *ops;
    void *priv;
    struct OsalMutex lock;
};

struct RegulatorMethod {
    int32_t (*open)(struct RegulatorNode *node);
    int32_t (*close)(struct RegulatorNode *node);
    int32_t (*release)(struct RegulatorNode *node);
    int32_t (*enable)(struct RegulatorNode *node);
    int32_t (*disable)(struct RegulatorNode *node);
    int32_t (*forceDisable)(struct RegulatorNode *node);
    int32_t (*setVoltage)(struct RegulatorNode *node, uint32_t minUv, uint32_t maxUv);
    int32_t (*getVoltage)(struct RegulatorNode *node, uint32_t *voltage);
    int32_t (*setCurrent)(struct RegulatorNode *node, uint32_t minUa, uint32_t maxUa);
    int32_t (*getCurrent)(struct RegulatorNode *node, uint32_t *regCurrent);
    int32_t (*getStatus)(struct RegulatorNode *node, uint32_t *status);
};

/**
 * @brief Find and return a regulator controller by regulator info
 * @param info Indicates regulator info.
 * @return a regulator controller
 */
struct RegulatorNode *RegulatorNodeOpen(const char *name);
int32_t RegulatorNodeClose(struct RegulatorNode *node);
/**
 * @brief add a regulator controller to manager list
 * @param node Indicates a regulator controller.
 * @constraints: first add parent, then add child
 * @return success or fail
 */
int32_t RegulatorNodeAdd(struct RegulatorNode *node);
int32_t RegulatorNodeRemove(const char *name);
/**
 * @brief remove all regulator controllers
 * @param
 * @return success or fail
 */
int32_t RegulatorNodeRemoveAll(void);
/**
 * @brief enable a regulator
 * @param node Indicates a regulator controller.
 * @return success or fail
 */
int32_t RegulatorNodeEnable(struct RegulatorNode *node);
/**
 * @brief disable a regulator if it's downstream node all close, if alwayson is true,forbid disable
 * @param node Indicates a regulator controller.
 * @return success or fail
 */
int32_t RegulatorNodeDisable(struct RegulatorNode *node);
/**
 * @brief disable a regulator regardless of whether there is an downstream node
 * @param node Indicates a regulator controller.
 * @return success or fail
 */
int32_t RegulatorNodeForceDisable(struct RegulatorNode *node);
/**
 * @brief set regulator voltage
 * @param node Indicates a regulator controller.
 * @param minUv min voltage
 * @param maxUv max voltage
 * @return success or fail
 */
int32_t RegulatorNodeSetVoltage(struct RegulatorNode *node, uint32_t minUv, uint32_t maxUv);
/**
 * @brief get regulator voltage
 * @param node Indicates a regulator controller.
 * @param voltage regulator voltage
 * @return success or fail
 */
int32_t RegulatorNodeGetVoltage(struct RegulatorNode *node, uint32_t *voltage);
/**
 * @brief set regulator current
 * @param node Indicates a regulator controller.
 * @param minUa min current
 * @param maxUa max current
 * @return success or fail
 */
int32_t RegulatorNodeSetCurrent(struct RegulatorNode *node, uint32_t minUA, uint32_t maxUA);
/**
 * @brief get regulator current
 * @param node Indicates a regulator controller.
 * @param current regulator current
 * @return success or fail
 */
int32_t RegulatorNodeGetCurrent(struct RegulatorNode *node, uint32_t *regCurrent);
/**
 * @brief get regulator status
 * @param node Indicates a regulator controller.
 * @param status regulator status,enable or disable
 * @return success or fail
 */
int32_t RegulatorNodeGetStatus(struct RegulatorNode *node, uint32_t *status);
/**
 * @brief register regulator's callback when it's status change
 * @param node Indicates a regulator controller.
 * @param cb callback function
 * @return success or fail
 */
int32_t RegulatorNodeRegisterStatusChangeCb(struct RegulatorNode *node, RegulatorStatusChangecb cb);
int32_t RegulatorNodeStatusCb(struct RegulatorNode *node);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* REGULATOR_CORE_H */
