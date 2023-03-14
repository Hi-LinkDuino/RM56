#ifndef _NET_MEMORY_H__
#define _NET_MEMORY_H__

#include <stdlib.h>
#include "net_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mem_element {
    unsigned int size;
    unsigned char data[0];
};

void nts_system_heap_init(void *begin_addr, void *end_addr);
void *nts_malloc(size_t size);
void *nts_zalloc(size_t size);
void *nts_realloc(void *rmem, size_t newsize);
void *nts_calloc(size_t count, size_t size);
void nts_free(void *rmem);
uint32_t nts_size(const void *rmem);
void *nts_kmalloc(size_t size);
void *nts_kzalloc(size_t size);
int nts_kfree(void *mem);
void *nts_krealloc(const void *p, size_t new_size);
void *nts_kmemdup(const void *src, size_t len);
uint32_t nts_get_free_heap_size(void);
void print_mem_dump(void *mem);

#if 1
#define  kmalloc(size, flags) (\
   {\
       void* ret = NULL;\
       (void)flags; \
       ret = nts_kmalloc(size);\
       ret;\
    }\
)
#else
#define  kmalloc(size, flags) (\
       {\
           uint8_t* ret = NULL;\
           (void)flags; \
           ret = nts_kmalloc(size);\
           printk(KERN_DEBUG, "kmalloc %s %d mem = 0x%x size:%d\n", __func__, __LINE__, ret, size);\
           ret;\
        }\
    )
#endif
/**
 * kzalloc - allocate memory. The memory is set to zero.
 * @size: how many bytes of memory are required.
 * @flags: the type of memory to allocate (see kmalloc).
 */
#if 1
#define kzalloc(size, flags) (\
   {   \
       void* ret = NULL;\
       (void)flags; \
       ret = nts_kzalloc(size);\
       ret;\
   }\
)
#else
#define kzalloc(size, flags) (\
   {   \
       uint8_t* ret = NULL;\
       (void)flags; \
       ret = nts_kzalloc(size);\
       printk(KERN_DEBUG, "kzalloc %s %d mem = 0x%x\n", __func__, __LINE__, ret);\
       ret;\
   }\
)
#endif
/**
 * kcalloc - allocate memory for an array. The memory is set to zero.
 * @n: number of elements.
 * @size: element size.
 * @flags: the type of memory to allocate (see kmalloc).
 */
#define kcalloc( n,  size, flags) kzalloc(n*size, flags)
#ifndef mem_calloc_skb
#define mem_calloc_skb(n, size) kzalloc(n*size, 1)
#endif

static __inline size_t ksize(const void *objp)
{
    return nts_size(objp);
}

#if 1
#define kzfree(mem) ({ \
    uint32_t ret;\
    if(mem) \
        ret = nts_kfree(mem);\
    else\
        ret = 0;\
        ret;\
})
#else
#define kzfree(mem) ({ \
    uint32_t ret;\
    printk(KERN_DEBUG, "kzfree %s %d %x \n", __func__, __LINE__, mem);\
    if(mem) \
        ret = nts_kfree(mem);\
    else\
        ret = 0;\
        ret;\
})

#endif
#if 1
#define kfree(ptr) ({ \
    int ret; \
    ret = nts_kfree((void *)ptr);\
    ret;\
})

#define krealloc(p, new_size, flags) ({\
    void* ret = NULL;\
    (void)flags;\
    ret = nts_krealloc(p, new_size);\
    ret;\
})

#define kmemdup(src, len, flags) ({ \
    void* ret = NULL;\
    (void)flags;\
    ret = nts_kmemdup((void *)src, len);\
    ret;\
})
#else
#define kfree(ptr) ({ \
    uint32_t ret;\
    printk(KERN_DEBUG, "kfree %s %d %x \n", __func__, __LINE__, ptr);\
    if(ptr) \
        ret = nts_kfree(ptr);\
    else\
        ret = 0;\
        ret;\
})

#define krealloc(p, new_size, flags) ({\
    uint8_t* ret = NULL;\
    (void)flags;\
    printk(KERN_DEBUG, "krealloc %s %d src_mem = 0x%x new_size:%d\n", __func__, __LINE__, p, new_size);\
    ret = nts_krealloc(p, new_size);\
    printk(KERN_DEBUG, "krealloc mem = 0x%x\n", ret);\
    ret;\
})

#endif

#ifdef __cplusplus
}
#endif

#endif

