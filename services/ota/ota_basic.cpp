#include "hal_trace.h"
#include "string.h"
#include "cmsis_os.h"
#include "ota_basic.h"
#ifdef IBRT
#include "app_ibrt_if.h"
#if defined(IBRT_UI_V1)
#include "app_tws_ibrt.h"
#else
#include "app_tws_ibrt_conn_api.h"
#endif
#endif

#define OTA_BASIC_MAC_ADDRESS_LEN   6
typedef struct
{
    OTA_BASIC_ENABLED_DATAPATH_E enabledDatapath;
    uint8_t macAddress[OTA_BASIC_MAC_ADDRESS_LEN];
} OTA_BASIC_ENV_T;

static OTA_BASIC_ENV_T ota_basic_env;

static const char* ota_basic_datapath2str(OTA_BASIC_ENABLED_DATAPATH_E datapath)
{
#define CASE_DATAPATH(datapath) \
    case datapath:       \
        return "[" #datapath "]";
#define CASE_INVALID_DATAPATH() \
    default:     \
        return "[INVALID]";

    switch (datapath)
    {
        CASE_DATAPATH(OTA_BASIC_NO_DATAPATH_ENALBED)
        CASE_DATAPATH(OTA_BASIC_SPP_DATAPATH_ENABLED)
        CASE_DATAPATH(OTA_BASIC_BLE_DATAPATH_ENABLED)
        CASE_INVALID_DATAPATH()
    }
}

void ota_basic_env_init(void)
{
    memset((uint8_t *)&ota_basic_env, 0, sizeof(ota_basic_env));
    ota_basic_env.enabledDatapath = OTA_BASIC_NO_DATAPATH_ENALBED;
}

#ifdef IBRT
#if !defined(IBRT_UI_V1)
static OTA_BASIC_TWS_ROLE_E ota_basic_convert_uirole_to_ota_twsrole(uint8_t ui_role)
{
    TWS_UI_ROLE_E uiRole = (TWS_UI_ROLE_E)ui_role;
    
    if (TWS_UI_MASTER == uiRole)
    {
        return OTA_BASIC_TWS_MASTER;
    }
    else if (TWS_UI_SLAVE == uiRole)
    {
        return OTA_BASIC_TWS_SLAVE;
    }
    else if (TWS_UI_UNKNOWN == uiRole)
    {
        return OTA_BASIC_TWS_UNKNOWN;
    }
    else
    {
        return OTA_BASIC_TWS_INVALID_ROLE;
    }
}
#endif
#endif

bool ota_basic_enable_datapath_status(OTA_BASIC_ENABLED_DATAPATH_E datapath, uint8_t* macAddr)
{
    if (OTA_BASIC_NO_DATAPATH_ENALBED == ota_basic_env.enabledDatapath)
    {
        TRACE(1, "OTA DATAPATH TO: %s", ota_basic_datapath2str(datapath));
        TRACE(1, "OTA PEER ADDR:");
        DUMP8("%02x ", macAddr, BT_ADDR_OUTPUT_PRINT_NUM);
        
        ota_basic_env.enabledDatapath = datapath;
        memcpy(ota_basic_env.macAddress, macAddr, OTA_BASIC_MAC_ADDRESS_LEN);
        return true;
    }
    else
    {
        return false;
    }
}

bool ota_basic_disable_datapath_status(OTA_BASIC_ENABLED_DATAPATH_E datapath, uint8_t* macAddr)
{
    if (OTA_BASIC_NO_DATAPATH_ENALBED == ota_basic_env.enabledDatapath)
    {
        return false;
    }
    else
    {
        if (datapath == ota_basic_env.enabledDatapath)
        {
            if (!memcmp(macAddr, ota_basic_env.macAddress, OTA_BASIC_MAC_ADDRESS_LEN))
            {
                TRACE(1, "OTA DATAPATH TO: OTA_BASIC_NO_DATAPATH_ENALBED");
                ota_basic_env.enabledDatapath = OTA_BASIC_NO_DATAPATH_ENALBED;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}

uint8_t* ota_basic_get_mac_addr(void)
{
    return ota_basic_env.macAddress;
}

#if defined(IBRT)&&(!defined(IBRT_UI_V1))
OTA_BASIC_TWS_ROLE_E ota_basic_get_tws_role(void)
{
    TWS_UI_ROLE_E uiRole = app_ibrt_if_get_ui_role();
    return ota_basic_convert_uirole_to_ota_twsrole(uiRole);
}
#endif

OTA_BASIC_ENABLED_DATAPATH_E ota_basic_get_datapath(void)
{
    return ota_basic_env.enabledDatapath;
}

bool ota_basic_is_remap_enabled(void)
{
#ifdef FLASH_REMAP
    return true;
#else
    return false;
#endif

}

uint32_t ota_basic_get_remap_offset(void)
{
#ifdef FLASH_REMAP
    return ((uint32_t)OTA_REMAP_OFFSET);
#else
    return ((uint32_t)0x0);
#endif

}

uint32_t ota_basic_get_max_image_size(void)
{
#ifdef FLASH_REMAP
    return ((uint32_t)OTA_REMAP_OFFSET);
#else
    return ((uint32_t)(NEW_IMAGE_FLASH_OFFSET - __APP_IMAGE_FLASH_OFFSET__));
#endif

}


