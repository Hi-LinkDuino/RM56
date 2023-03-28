#ifndef  _NORFLASH_ASYNC_API_H_
#define _NORFLASH_ASYNC_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_norflash.h"

#define FLASH_SECTOR_SIZE               4096

#if defined(FLASH_API_SIMPLE)
#define NORFLASH_API_WRITE_BUFF_LEN     (0)
#endif
#if defined(FLASH_API_HIGHPERFORMANCE)
#define NORFLASH_API_WRITE_BUFF_LEN     (8)
#endif
#if defined(FLASH_API_NORMAL)
#define NORFLASH_API_WRITE_BUFF_LEN     (4)
#endif
#define NORFLASH_API_OPRA_LIST_LEN      ((NORFLASH_API_WRITE_BUFF_LEN + 1) * 3)
#define NORFLASH_API_SECTOR_SIZE        FLASH_SECTOR_SIZE

#define TO_FLASH_NC_ADDR(addr)          (((addr) & HAL_NORFLASH_ADDR_MASK) | FLASH_NC_BASE)
#define TO_FLASH_C_ADDR(addr)           (((addr) & HAL_NORFLASH_ADDR_MASK) | FLASH_BASE)
enum NORFLASH_API_MODULE_ID_T
{
    NORFLASH_API_MODULE_ID_LOG_DUMP,
    NORFLASH_API_MODULE_ID_USERDATA,
    NORFLASH_API_MODULE_ID_BOOTUP_INFO,
    NORFLASH_API_MODULE_ID_OTA,
    NORFLASH_API_MODULE_ID_UPGRADE_LOG,
#if defined(_AUTO_TEST_)
    NORFLASH_API_MODULE_ID_CUSTOM_PARAMETER,
    NORFLASH_API_MODULE_ID_FREE,
    NORFLASH_API_MODULE_ID_RESERVED,
    NORFLASH_API_MODULE_ID_AUD,
#endif
    NORFLASH_API_MODULE_ID_CRASH_DUMP,
    NORFLASH_API_MODULE_ID_COREDUMP,
    NORFLASH_API_MODULE_ID_FACTORY,
    NORFLASH_API_MODULE_ID_HOTWORD_MODEL,
    NORFLASH_API_MODULE_ID_INTERACTION_OTA,
    NORFLASH_API_MODULE_ID_GMA_OTA,
    NORFLASH_API_MODULE_ID_TEST1,
    NORFLASH_API_MODULE_ID_TEST2,
    NORFLASH_API_MODULE_ID_CUSTOM,
    NORFLASH_API_MODULE_ID_CUSTOM_BACKUP,
    NORFLASH_API_MODULE_ID_CUSTOM1,
    NORFLASH_API_MODULE_ID_CUSTOM2,
    NORFLASH_API_MODULE_ID_CUSTOM3,
    NORFLASH_API_MODULE_ID_CUSTOM4,
    NORFLASH_API_MODULE_ID_CUSTOM5,
    NORFLASH_API_MODULE_ID_CUSTOM6,
    NORFLASH_API_MODULE_ID_CUSTOM7,
    NORFLASH_API_MODULE_ID_CUSTOM8,
    NORFLASH_API_MODULE_ID_CUSTOM9,
    NORFLASH_API_MODULE_ID_COUNT,
};

enum NORFLASH_API_RET_T
{
    NORFLASH_API_OK,
    NORFLASH_API_BUFFER_FULL,
    NORFLASH_API_BAD_DEV_ID,
    NORFLASH_API_BAD_MOD_ID,
    NORFLASH_API_BAD_BUFF_LEN,
    NORFLASH_API_BAD_ADDR,
    NORFLASH_API_BAD_LEN,
    NORFLASH_API_ERR_UNINIT,
    NORFLASH_API_ERR_HASINIT,
    NORFLASH_API_ERR,
};

enum NORFLASH_API_OPRATION_TYPE
{
    NORFLASH_API_WRITTING = 0x01,
    NORFLASH_API_ERASING = 0x02,
    NORFLASH_API_ALL    = 0x03
};


typedef struct
{
    enum NORFLASH_API_OPRATION_TYPE type;
    uint32_t addr;
    uint32_t len;
    uint32_t remain_num;
    enum NORFLASH_API_RET_T result;
    uint32_t suspend_num;
}NORFLASH_API_OPERA_RESULT;

enum NORFLASH_API_STATE
{
    NORFLASH_API_STATE_UNINITED,
    NORFLASH_API_STATE_IDLE,
    NORFLASH_API_STATE_WRITTING,
    NORFLASH_API_STATE_WRITTING_SUSPEND,
    NORFLASH_API_STATE_WRITTING_RESUME,
    NORFLASH_API_STATE_ERASE,
    NORFLASH_API_STATE_ERASE_SUSPEND,
    NORFLASH_API_STATE_ERASE_RESUME,
};

enum NORFLASH_API_USER
{
    NORFLASH_API_USER_CP,
    NORFLASH_API_USER_COUNTS,
};

typedef void (* NORFLASH_API_OPERA_CB)(void* opera_result);
typedef bool (*NOFLASH_API_FLUSH_ALLOWED_CB)(void);

typedef struct _opera_info
{
    enum NORFLASH_API_OPRATION_TYPE type;
    uint32_t addr;
    uint32_t len;
    uint32_t w_offs;
    uint32_t w_len;
    uint8_t *buff;
    bool lock;
    struct _opera_info *next;
}OPRA_INFO;


typedef struct
{
    bool is_inited;
    enum HAL_FLASH_ID_T dev_id;
    enum NORFLASH_API_MODULE_ID_T mod_id;
    uint32_t mod_base_addr;
    uint32_t mod_len;
    uint32_t mod_block_len;
    uint32_t mod_sector_len;
    uint32_t mod_page_len;
    uint32_t buff_len;
    NORFLASH_API_OPERA_CB cb_func;
    OPRA_INFO *opera_info;
    OPRA_INFO *cur_opera_info;
    enum NORFLASH_API_STATE state;
}MODULE_INFO;

typedef struct
{
    bool is_inited;
    MODULE_INFO mod_info[NORFLASH_API_MODULE_ID_COUNT];
    enum NORFLASH_API_MODULE_ID_T cur_mod_id;
    MODULE_INFO* cur_mod;
    NOFLASH_API_FLUSH_ALLOWED_CB allowed_cb[NORFLASH_API_USER_COUNTS];
}NORFLASH_API_INFO;

typedef struct
{
    bool is_used;
    OPRA_INFO opera_info;
}OPERA_INFO_LIST;

typedef struct
{
    bool is_used;
    uint8_t buffer[NORFLASH_API_SECTOR_SIZE];
}DATA_LIST;

enum NORFLASH_API_RET_T norflash_api_init(void);

enum NORFLASH_API_RET_T norflash_api_register(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                enum HAL_FLASH_ID_T dev_id,
                uint32_t mod_base_addr,
                uint32_t mod_len,
                uint32_t mod_block_len,
                uint32_t mod_sector_len,
                uint32_t mod_page_len,
                uint32_t buffer_len,
                NORFLASH_API_OPERA_CB cb_func
                );

// read flash buffer or flash,priority read flash buffer.
enum NORFLASH_API_RET_T norflash_api_read(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                uint32_t start_addr,
                uint8_t *buffer,
                uint32_t len
                );

// read flash.
enum NORFLASH_API_RET_T norflash_sync_read(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                uint32_t start_addr,
                uint8_t *buffer,
                uint32_t len
                );

enum NORFLASH_API_RET_T norflash_api_erase(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                uint32_t start_addr,
                uint32_t len,
                bool async
                );

enum NORFLASH_API_RET_T norflash_api_write(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                uint32_t start_addr,
                const uint8_t *buffer,
                uint32_t len,
                bool async
                );

int norflash_api_flush(void);

bool norflash_api_buffer_is_free(
                enum NORFLASH_API_MODULE_ID_T mod_id);

uint32_t norflash_api_get_used_buffer_count(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                enum NORFLASH_API_OPRATION_TYPE type
                );

uint32_t norflash_api_get_free_buffer_count(
                enum NORFLASH_API_OPRATION_TYPE type
                );
void norflash_api_flush_all(void);
void norflash_api_flush_disable(enum NORFLASH_API_USER user_id,uint32_t cb);
void norflash_api_flush_enable(enum NORFLASH_API_USER user_id);
void norflash_api_flush_enable_all(void);
enum NORFLASH_API_STATE norflash_api_get_state(enum NORFLASH_API_MODULE_ID_T mod_id);

void norflash_flush_all_pending_op(void);

enum NORFLASH_API_RET_T norflash_api_get_base_addr(
                enum NORFLASH_API_MODULE_ID_T mod_id,
                uint32_t *addr);

enum NORFLASH_API_RET_T norflash_api_get_sector_size(
                enum NORFLASH_API_MODULE_ID_T mod_id, uint32_t *s_size);

enum NORFLASH_API_RET_T norflash_api_remap_start(
                         enum NORFLASH_API_MODULE_ID_T mod_id,
                         uint32_t start_addr,
                         uint32_t len);
enum NORFLASH_API_RET_T norflash_api_remap_done(
                         enum NORFLASH_API_MODULE_ID_T mod_id,
                         uint32_t start_addr,
                         uint32_t len);

#ifdef FLASH_API_GUARD_THREAD
void norflash_suspend_guard_wake(void);
#endif
#ifdef __cplusplus
}
#endif

#endif //_NORFLASH_ASYNC_API_H_
