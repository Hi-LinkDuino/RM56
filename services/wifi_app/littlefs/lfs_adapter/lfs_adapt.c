#include "lfs.h"
#include "lfs_adapt.h"
#include "hal_trace.h"
#include "plat_addr_map.h"

extern int lfs_block_device_open(void);
extern int lfs_block_device_close(void);
extern int lfs_block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
extern int lfs_block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
extern int lfs_block_device_erase(const struct lfs_config *c, lfs_block_t block);
extern int lfs_block_device_sync(const struct lfs_config *c);
extern lfs_size_t lfs_block_device_get_read_size(void);
extern lfs_size_t lfs_block_device_get_prog_size(void);
extern lfs_size_t lfs_block_device_get_block_size(void);
extern lfs_size_t lfs_block_device_get_block_count(void);
extern int lfs_block_device_get_block_cycles(void);
extern lfs_size_t lfs_block_device_get_cache_size(void);
extern lfs_size_t lfs_block_device_get_lookahead_size(void);

#if LFS_ON_RAM
char buf[390 * 1024];  //buffer in ram, size just for test
#endif
// configuration of the filesystem is provided by this struct
struct lfs_config lfs_cfg = {
    // block device operations
    .read = lfs_block_device_read,
    .prog = lfs_block_device_prog,
    .erase = lfs_block_device_erase,
    .sync = lfs_block_device_sync,
};

static int lfs_block_device_config(struct lfs_config *config)
{
    // block_device_config
#if LFS_ON_RAM
    config->context = (void*)PSRAMFS_BASE;
#endif
    config->read_size = lfs_block_device_get_read_size();
    config->prog_size = lfs_block_device_get_prog_size();
    config->block_size = lfs_block_device_get_block_size();
    config->block_count = lfs_block_device_get_block_count();
    config->block_cycles = lfs_block_device_get_block_cycles();
    config->cache_size = lfs_block_device_get_cache_size();
    config->lookahead_size = lfs_block_device_get_lookahead_size();
    return 0;
}

int lfs_device_open(void)
{
    lfs_block_device_open();
    lfs_block_device_config(&lfs_cfg);
    return 0;
}

int lfs_device_close(void)
{
    // lfs_block_device_close();
    return 0;
}

struct lfs_config *lfs_get_config(void)
{
    return &lfs_cfg;
}

void log_lfs_info(struct lfs_info *info)
{
    TRACE(0, "[name] = [%s]\n", info->name);
    TRACE(0, "[size] = [%d]\n", info->size);
    if (info->type == LFS_TYPE_REG)
        TRACE(0, "[type] = [REG]\n");
    else if (info->type == LFS_TYPE_DIR)
        TRACE(0, "[type] = [DIR]\n");
    else
        TRACE(0, "[type] unknow\n");
}

#define LFS_ERR_LOG(err)                                              \
    do                                                                \
    {                                                                 \
        TRACE(0, "[lfs err]%s:%d, err = %d\n", __func__, __LINE__, err); \
    } while (0)

#define LFS_ERR_CHECK(err)    \
    do                        \
    {                         \
        if (err < 0)          \
        {                     \
            LFS_ERR_LOG(err); \
            return err;       \
        }                     \
    } while (0)

static int lfs_test_example_case(lfs_t *lfs, lfs_file_t *file, struct lfs_config *cfg);
static int lfs_test_root_dir(lfs_t *lfs, lfs_dir_t *dir, struct lfs_config *cfg);
static int lfs_test_dir_creat(lfs_t *lfs, struct lfs_config *cfg);
static int lfs_test_file_creat(lfs_t *lfs, lfs_file_t *file, struct lfs_config *cfg);
static int lfs_test_dir_iterate(lfs_t *lfs, lfs_dir_t *dir, struct lfs_config *cfg, struct lfs_info *info);

int lfs_test(void)
{
    int ret = 0;

#if 0
	lfs_block_device_test();
#endif

    // variables used by the filesystem
    lfs_t test_lfs;
    lfs_file_t test_file;
    lfs_dir_t test_dir;
    struct lfs_info test_info;

    // open lfs block device
    lfs_device_open();

#if 1
    LFS_ERR_CHECK(lfs_format(&test_lfs, &lfs_cfg));
#endif

#if 1
    lfs_test_example_case(&test_lfs, &test_file, &lfs_cfg);
#endif

    ret = lfs_mount(&test_lfs, &lfs_cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (ret)
    {
        lfs_format(&test_lfs, &lfs_cfg);
        lfs_mount(&test_lfs, &lfs_cfg);
    }
    lfs_unmount(&test_lfs);

    LFS_ERR_CHECK(lfs_test_root_dir(&test_lfs, &test_dir, &lfs_cfg));
    LFS_ERR_CHECK(lfs_test_dir_creat(&test_lfs, &lfs_cfg));
    LFS_ERR_CHECK(lfs_test_file_creat(&test_lfs, &test_file, &lfs_cfg));
    LFS_ERR_CHECK(lfs_test_dir_iterate(&test_lfs, &test_dir, &lfs_cfg, &test_info));
    //LFS_ERR_CHECK();
    //LFS_ERR_CHECK();
    return 0;
}

static int lfs_test_example_case(lfs_t *lfs, lfs_file_t *file, struct lfs_config *cfg)
{
    // mount the filesystem
    int err = lfs_mount(lfs, cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err)
    {
        lfs_format(lfs, cfg);
        lfs_mount(lfs, cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(lfs, file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(lfs, file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(lfs, file);
    lfs_file_write(lfs, file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(lfs, file);

    // release any resources we were using
    lfs_unmount(lfs);

    // print the boot count
    TRACE(0, "boot_count: %d\n", boot_count);

    return 0;
}

static int lfs_test_root_dir(lfs_t *lfs, lfs_dir_t *dir, struct lfs_config *cfg)
{
    TRACE(0, "\n%s\n", __func__);
    LFS_ERR_CHECK(lfs_mount(lfs, cfg));
    LFS_ERR_CHECK(lfs_dir_open(lfs, dir, "/"));
    LFS_ERR_CHECK(lfs_dir_close(lfs, dir));
    LFS_ERR_CHECK(lfs_unmount(lfs));
    return 0;
}

static int lfs_test_dir_creat(lfs_t *lfs, struct lfs_config *cfg)
{
    TRACE(0, "\n%s\n", __func__);
    LFS_ERR_CHECK(lfs_mount(lfs, cfg));
    LFS_ERR_CHECK(lfs_mkdir(lfs, "potato"));
    LFS_ERR_CHECK(lfs_unmount(lfs));
    return 0;
}

static int lfs_test_file_creat(lfs_t *lfs, lfs_file_t *file, struct lfs_config *cfg)
{
    TRACE(0, "\n%s\n", __func__);
    LFS_ERR_CHECK(lfs_mount(lfs, cfg));
    LFS_ERR_CHECK(lfs_file_open(lfs, file, "burito", LFS_O_CREAT | LFS_O_WRONLY));
    LFS_ERR_CHECK(lfs_file_close(lfs, file));
    LFS_ERR_CHECK(lfs_unmount(lfs));
    return 0;
}

static int lfs_test_dir_iterate(lfs_t *lfs, lfs_dir_t *dir, struct lfs_config *cfg, struct lfs_info *info)
{
    TRACE(0, "\n%s\n", __func__);
    int ret = 0;

    LFS_ERR_CHECK(lfs_mount(lfs, cfg));
    LFS_ERR_CHECK(lfs_dir_open(lfs, dir, "/"));

    while (1)
    {
        ret = lfs_dir_read(lfs, dir, info);
        if (ret <= 0)
        {
            break;
        }
        log_lfs_info(info);
    }

    LFS_ERR_CHECK(lfs_dir_close(lfs, dir));
    LFS_ERR_CHECK(lfs_unmount(lfs));
    return 0;
}