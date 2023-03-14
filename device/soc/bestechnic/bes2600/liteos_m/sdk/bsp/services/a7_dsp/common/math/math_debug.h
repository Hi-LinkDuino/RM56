#ifndef _MATH_DEBUG_H_
#define _MATH_DEBUG_H_


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <stdio.h>
#include <string.h>

#ifndef COLOR_LOG_ENABLE
#define COLOR_LOG_ENABLE

#define COLOR_LOG

#ifdef COLOR_LOG
#define ESC_START       "\033[1;"
#define ESC_END         "\033[0m"
#define COLOR_FATAL     "31;40;5m"
#define COLOR_ALERT     "31;40;1m"
#define COLOR_CRIT      "31;40;1m"
#define COLOR_ERROR     "35;40;1m"
#define COLOR_WARN      "33;40;1m"
#define COLOR_NOTICE    "34;40;1m"
#define COLOR_INFO      "32;40;1m"
#define COLOR_DEBUG     "36;40;1m"
#define COLOR_TRACE     "37;40;1m"
#define CD ESC_START COLOR_DEBUG
#define CI ESC_START COLOR_INFO
#define CW ESC_START COLOR_WARN
#define CE ESC_START COLOR_ERROR
#define CF ESC_START COLOR_FATAL
#define CEND ESC_END
#else
#define CD
#define CI
#define CW
#define CE
#define CF
#define CEND
#endif


#include "hal_trace.h"
#define LOG(fmt, args...) TRACE(0, fmt, ##args);
#define PRE_FMT "A|%5d-%s|"
#define POST_FMT ""

#define PRE_CONTENT __LINE__, __func__
#define FILENAME(x) (strrchr(x,'/') ? strrchr(x,'/') + 1 : x )

#define prt(level, fmt, args...) \
        LOG( level PRE_FMT fmt CEND POST_FMT, PRE_CONTENT, ##args)

#define fprt(level, fmt, args...) \
        LOG( level "%5d-%s-%s|" fmt CEND POST_FMT, __LINE__, FILENAME(__FILE__), __func__, ##args)

#define dfprt(fmt,args...) fprt(CD, fmt, ##args);
// #define dprt(fmt,args...) prt(CD, fmt, ##args);
#define dprt(fmt,args...) do{}while(0)
#define iprt(fmt,args...) prt(CI, fmt, ##args);
#define wprt(fmt,args...) prt(CW, fmt, ##args);
#define eprt(fmt,args...) prt(CE, "Err:" fmt, ##args);


#define HEX_DUMP(t, s, l) \
    do { \
        printf("%s", t); \
        for (unsigned int n = 0; n < l; ++n) { \
            if ((n % 16) == 0) { \
                printf("\n%04x", n); \
            } \
            printf( " %02x", *(s + n)); \
        } \
        printf("\n"); \
    } while(0);

#define HEX_CHAR_DUMP(tag, ptr, len) \
    do { \
        printf("%s\n", tag); \
        uint8_t *c = ptr; \
        unsigned int remain = len; \
        unsigned i; \
        while (remain > 0) { \
            for (i = 0; i < 16; i++) { if (i < remain) { printf("%02x ", c[i]); } else { printf("   "); } } \
            for (i = 0; i < 16; i++) { if (i < remain) { printf("%c", ((unsigned int)(c[i] - ' ') < 127u - ' ') ? c[i] : '.'); } else { printf(" "); } } \
            printf("\n"); \
            if (remain <= 16) { break; } \
            remain -= 16; \
            c += 16; \
        } \
    } while(0);

#define prt_long_str(s, l) \
    do { \
        int n = 0; \
        for (; n < l; ++n) { \
            if ((n % 70) == 0) { \
                printf("\n"); \
            } \
            printf( "%c", s[n]); \
        } \
        printf("\n"); \
    } while(0);

#define prt_ls(t, s, l) \
    do { \
        int n = 0; \
        int last_n = 0; \
        printf("%s", t); \
        for (; n < l; ++n) { \
            if (s[n] == '\n') { \
                last_n = n; \
            } \
            if ((n % 70) == 0 && (n - last_n >= 70)) { \
                printf("\n"); \
            } \
            printf( "%c", s[n]); \
        } \
        printf("\n"); \
    } while(0);

#define CHECK_RET_BREAK(cond, fmt, args...) if(cond){ eprt(fmt, ##args); break;}
#define CHECK_RET_BREAK_I(cond, fmt, args...) if(cond){ iprt(fmt, ##args); break;}
#define CHECK_RET_RETURN(cond, ret, fmt, args...) if(cond){ eprt(fmt, ##args); return ret;}
#define CHECK_RET_RETURN_NULL(cond, fmt, args...) if(cond){ eprt(fmt, ##args); return;}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //_MATH_DEBUG_H_

