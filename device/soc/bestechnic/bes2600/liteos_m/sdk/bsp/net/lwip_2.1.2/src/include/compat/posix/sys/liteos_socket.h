#ifdef SOL_SOCKET
#undef SOL_SOCKET
#define SOL_SOCKET           1
#endif

#ifdef SO_ACCEPTCONN
#undef SO_ACCEPTCONN
#define SO_ACCEPTCONN        30
#endif

#ifdef SO_KEEPALIVE
#undef SO_KEEPALIVE
#define SO_KEEPALIVE         9
#endif

#ifdef SO_REUSEADDR
#undef SO_REUSEADDR
#define SO_REUSEADDR         2
#endif

#ifdef SO_TYPE
#undef SO_TYPE
#define SO_TYPE              3
#endif

#ifdef SO_ERROR
#undef SO_ERROR
#define SO_ERROR             4
#endif

#ifdef SO_SNDTIMEO
#undef SO_SNDTIMEO
#define SO_SNDTIMEO          67
#endif

#ifdef SO_RCVTIMEO
#undef SO_RCVTIMEO
#define SO_RCVTIMEO          66
#endif

#ifdef SO_RCVBUF
#undef SO_RCVBUF
#define SO_RCVBUF            8
#endif

#ifdef SO_LINGER
#undef SO_LINGER
#define SO_LINGER            13
#endif

#ifdef SO_BINDTODEVICE
#undef SO_BINDTODEVICE
#define SO_BINDTODEVICE      25
#endif

#ifdef IP_TTL
#undef IP_TTL
#define IP_TTL               2
#endif

#ifdef IP_TOS
#undef IP_TOS
#define IP_TOS               1
#endif

#ifdef IP_PKTINFO
#undef IP_PKTINFO
#define IP_PKTINFO           8
#endif

#ifdef TCP_NODELAY
#undef TCP_NODELAY
#define TCP_NODELAY          1
#endif

#ifdef SO_BROADCAST
#undef SO_BROADCAST
#define SO_BROADCAST         6
#endif

#ifdef SO_NO_CHECK
#undef SO_NO_CHECK
#define SO_NO_CHECK          11
#endif

#ifdef TCP_KEEPALIVE
#undef TCP_KEEPALIVE
#define TCP_KEEPALIVE        0xFF
#endif

#ifdef TCP_KEEPIDLE
#undef TCP_KEEPIDLE
#define TCP_KEEPIDLE         4
#endif

#ifdef TCP_KEEPINTVL
#undef TCP_KEEPINTVL
#define TCP_KEEPINTVL        5
#endif

#ifdef TCP_KEEPCNT
#undef TCP_KEEPCNT
#define TCP_KEEPCNT          6
#endif

#ifdef IP_MULTICAST_TTL
#undef IP_MULTICAST_TTL
#define IP_MULTICAST_TTL     33
#endif

#ifdef IP_MULTICAST_IF
#undef IP_MULTICAST_IF
#define IP_MULTICAST_IF      32
#endif

#ifdef IP_MULTICAST_LOOP
#undef IP_MULTICAST_LOOP
#define IP_MULTICAST_LOOP    34
#endif

#ifdef IP_ADD_MEMBERSHIP
#undef IP_ADD_MEMBERSHIP
#define IP_ADD_MEMBERSHIP    35
#endif

#ifdef IP_DROP_MEMBERSHIP
#undef IP_DROP_MEMBERSHIP
#define IP_DROP_MEMBERSHIP   36
#endif

#ifdef MSG_WAITALL
#undef MSG_WAITALL
#define MSG_WAITALL          256
#endif

#ifdef MSG_PEEK
#undef MSG_PEEK
#define MSG_PEEK             2
#endif

#ifdef MSG_OOB
#undef MSG_OOB
#define MSG_OOB              1
#endif

#ifdef MSG_MORE
#undef MSG_MORE
#define MSG_MORE             0x8000
#endif

#ifdef MSG_NOSIGNAL
#undef MSG_NOSIGNAL
#define MSG_NOSIGNAL         0x4000
#endif

#ifdef MSG_TRUNC
#undef MSG_TRUNC
#define MSG_TRUNC            32
#endif

#ifdef MSG_DONTWAIT
#undef MSG_DONTWAIT
#define MSG_DONTWAIT         64
#endif

#ifdef AI_NUMERICSERV
#undef AI_NUMERICSERV
#define AI_NUMERICSERV       1024
#endif

#ifdef AI_V4MAPPED
#undef AI_V4MAPPED
#define AI_V4MAPPED          8
#endif

#ifdef AI_ALL
#undef AI_ALL
#define AI_ALL               16
#endif

#ifdef AI_ADDRCONFIG
#undef AI_ADDRCONFIG
#define AI_ADDRCONFIG        32
#endif

#ifdef EAI_NONAME
#undef EAI_NONAME
#define EAI_NONAME           -2
#endif

#ifdef EAI_SERVICE
#undef EAI_SERVICE
#define EAI_SERVICE          -8
#endif

#ifdef EAI_FAIL
#undef EAI_FAIL
#define EAI_FAIL             -4
#endif

#ifdef EAI_MEMORY
#undef EAI_MEMORY
#define EAI_MEMORY          -10
#endif

#ifdef EAI_FAMILY
#undef EAI_FAMILY
#define EAI_FAMILY          -6
#endif

#ifdef HOST_NOT_FOUND
#undef HOST_NOT_FOUND
#define HOST_NOT_FOUND       1
#endif

#ifdef NO_DATA
#undef NO_DATA
#define NO_DATA              4
#endif

#ifdef NO_RECOVERY
#undef NO_RECOVERY
#define NO_RECOVERY          3
#endif

#ifdef TRY_AGAIN
#undef TRY_AGAIN
#define TRY_AGAIN            2
#endif

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#define FD_SETSIZE           228
#endif
