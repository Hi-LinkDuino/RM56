/**
 ****************************************************************************************
 *
 * @file co_djob.h
 *
 * @brief Common delayed job definitions
 *
 * Copyright (C) RivieraWaves 2009-2019
 *
 ****************************************************************************************
 */
#ifndef _CO_DJOB_H_
#define _CO_DJOB_H_

/**
 ****************************************************************************************
 * @defgroup CO_DJOB Utilities
 * @ingroup COMMON
 * @brief  Delayed job utilities
 *
 * This module contains the delayed job utilities functions and macros.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>       // standard definitions
#include <stddef.h>       // standard definitions
#include "co_list.h"      // common bt definitions


/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * ENUMERATIONS DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Job function to called into a background context
 *
 * @param[in] p_env   Pointer to environment that will be used as callback parameter.
 ****************************************************************************************
 */
typedef void (*co_djob_cb)(void* p_env);

/// Job element structure
typedef struct co_djob
{
    /// List element header
    co_list_hdr_t hdr;
    /// Pointer to environment that will be used as callback parameter.
    void*         p_env;
    /// Callback to execute in background context
    co_djob_cb    cb;
} co_djob_t;


/*
 * CONSTANT DECLARATIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/*
 ****************************************************************************************
 * Delayed Job functions
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief Prepare Delayed job structure
 *
 * @param[in] p_djob   Pointer to the delayed job structure
 * @param[in] cb       Callback to execute in background context
 * @param[in] p_env    Pointer to environment that will be used as callback parameter.
 ****************************************************************************************
 */
void co_djob_prepare(co_djob_t* p_djob, co_djob_cb cb, void* p_env);

/**
 ****************************************************************************************
 * @brief Register to execute a job delayed in background
 *
 * @param[in] p_djob   Pointer to the delayed job structure
 ****************************************************************************************
 */
void co_djob_reg(co_djob_t* p_djob);

/**
 ****************************************************************************************
 * @brief Un-register a job that waits to be executed
 *
 * @param[in] p_djob   Pointer to the delayed job structure
 ****************************************************************************************
 */
void co_djob_unreg(co_djob_t* p_djob);

/**
 ****************************************************************************************
 * @brief Initialize Common delayed job module.
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void co_djob_init(uint8_t init_type);
/// @} CO_DJOB

#endif // _CO_DJOB_H_
