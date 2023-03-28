#include "cmsis_os2.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "hal_trace.h"

#define CSI_INFO_SIZE (456 + 16)
#define CSI_INFO_MAX 10
#define TCP_PORT 8888

struct csi_info {
    uint8_t info[CSI_INFO_SIZE];
};

static osMessageQueueId_t mq;
static int clientfd = -1;

static int csi_info_dequeue(struct csi_info *csi)
{
    if (mq == NULL) {
        return -1;
    }
    return osMessageQueueGet(mq, csi, NULL, osWaitForever);
}

int csi_info_enqueue(const void *buf, uint32_t size)
{
    static struct csi_info csi = {0};
    static uint16_t csi_offset = 0;
    // printf("%s: size %u\r\n", __func__, size);
    if (size > CSI_INFO_SIZE || mq == NULL || clientfd < 0) {
        return -1;
    }
    memcpy(&csi.info[csi_offset], buf, size);
    csi_offset += size;
    if (csi_offset >= ((456 + 16))) { /* match wsm.c CSI_INFO_SIZE */
        csi_offset = 0;
        return osMessageQueuePut(mq, &csi, 0, 0);
    }
    return 0;
}

void csi_reporter_task(void)
{
    int listenfd;
    struct sockaddr_in srvaddr;

    mq = osMessageQueueNew(CSI_INFO_MAX, CSI_INFO_SIZE, NULL);
    if (mq == NULL) {
        printf("%s: osSemaphoreNew failed\r\n", __func__);
        return;
    }

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket error\r\n");
        goto err_mq;
    }
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = INADDR_ANY;
    srvaddr.sin_port = htons(TCP_PORT);
    if (bind(listenfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) == -1) {
        printf("bind error\r\n");
        goto err_skt;
    }
    if (listen(listenfd, 5) == -1) {
        printf("listen error\r\n");
        goto err_skt;
    }
    printf("tcp server listening on port %d...\n", TCP_PORT);

    while (1) {
        struct sockaddr_in cliaddr;
        socklen_t addrlen = sizeof(struct sockaddr_in);
        if ((clientfd = accept(listenfd, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen)) == -1) {
            printf("accept error\r\n");
            continue;
        }
        printf("new client %d from %s:%d\r\n", clientfd, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
        // handle connection for a client once
        struct csi_info csi = {0};
        uint32_t cnt = 0;
        while (1) {
            if (csi_info_dequeue(&csi) == 0) {
                if (send(clientfd, &csi, sizeof(csi), 0) <= 0) {
                    break;
                }
                printf("csi send cnt %u\r\n", ++cnt);
            }
        }
        printf("close clientfd %d\r\n", clientfd);
        close(clientfd);
        clientfd = -1;
    }

err_skt:
    close(listenfd);
err_mq:
    osMessageQueueDelete(mq);
}