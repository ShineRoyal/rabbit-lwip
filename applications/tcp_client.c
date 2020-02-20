/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-02-20     ShineRoyal   the first version
 */

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

static const char *send_data = "hello RT-Thread";

void tcpclient(int argc, char **argv)
{
    int sock, port, count;
    struct hostent *host;
    struct sockaddr_in send_addr;
//    struct sockaddr_in recv_addr;
    const char *url;

    /* 接收到的参数小于 3 个 */
    if (argc < 3)
    {
        rt_kprintf("Usage: tcpclient URL PORT [COUNT = 10]\n");
        rt_kprintf("Like: tcpclient 192.168.12.44 5000\n");
        return;
    }

    url = argv[1];
    port = strtoul(argv[2], 0, 10);

    if (argc > 3)
        count = strtoul(argv[3], 0, 10);
    else
        count = 10;

    /* 通过函数入口参数 url 获得 host 地址（如果是域名，会做域名解析） */
    host = (struct hostent *) gethostbyname(url);

    /* 创建一个 socket，类型是 SOCK_DGRAM，UDP 类型 */
    /*SOCK_STREAM => TCP*/
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        return;
    }

    /* 初始化预连接的服务端地址 */
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(port);
    send_addr.sin_addr = *((struct in_addr *) host->h_addr);
    rt_memset(&(send_addr.sin_zero), 0, sizeof(send_addr.sin_zero));

//    recv_addr.sin_family = AF_INET;
//    recv_addr.sin_port = IPADDR_ANY;
//    recv_addr.sin_addr = *((struct in_addr *) host->h_addr);
//    rt_memset(&(recv_addr.sin_zero), 0, sizeof(recv_addr.sin_zero));

    /* 总计发送 count 次数据 */
    if (connect(sock, (struct sockaddr * )&send_addr, sizeof(struct sockaddr)) < 0)
    {
        rt_kprintf("Connect fail!\n");
        goto __exit;
    }
    while (count)
    {
        /* 发送数据到服务远端 */
        send(sock, send_data, strlen(send_data), 0);

        /* 线程休眠一段时间 */
        rt_thread_delay(50);

        char str[100];
        memset(str, 0, sizeof(str));

        recv(sock, str, sizeof(str), 0);
        rt_kprintf("%s:%d=>%s\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port), str);

        /* 计数值减一 */
        count--;
    }
__exit:
    /* 关闭这个 socket */
    closesocket(sock);
}
MSH_CMD_EXPORT(tcpclient, tcpclient);

