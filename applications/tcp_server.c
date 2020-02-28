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
#include <netdev.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#define DBG_TAG "tcpserver"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static const char *send_data = "hello RT-Thread\n";

struct client_info
{
    int socketnum;              //socket 号
    struct sockaddr_in addr;    //socket客户端的ip和port信息
    int sockaddrlen;            //socketaddr的长度信息
};

void client_thread_entry(void *param)
{
    struct client_info* client = param;
    LOG_D("[%d]%s:%d is connect...", client->socketnum, inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
    send(client->socketnum, (const void* )send_data, strlen(send_data), 0);
    while (1)
    {
        char str[100];
        rt_memset(str, 0, sizeof(str));
        int bytes = recv(client->socketnum, str, sizeof(str), 0);
        LOG_D("bytes:%d", bytes);
        if (bytes == 0)
            goto __exit;
        else if (bytes == -1)
            goto __error;
        LOG_D("[%d]%s:%d=>%s...", client->socketnum, inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port),
                str);
        send((int )client->socketnum, (const void * )str, (size_t )strlen(str), 0);
    }
    __exit: LOG_D("[%d]%s:%d is disconnect...", client->socketnum, inet_ntoa(client->addr.sin_addr),
            ntohs(client->addr.sin_port));
    rt_free(client);
    closesocket(client->socketnum);
    return;

    __error: LOG_D("[%d]%s:%d is error...", client->socketnum, inet_ntoa(client->addr.sin_addr),
            ntohs(client->addr.sin_port));
    rt_free(client);
    closesocket(client->socketnum);
    return;

}

void tcpserver_thread_entry(int argc, char **argv)
{
    rt_thread_t tid = RT_NULL;
    int sock_listen, sock_connect, port;
    struct hostent *host;
    struct sockaddr_in listen_addr;
    struct sockaddr_in connect_addr;
    const char *url;

    url = "192.168.1.42";   //localhost ip
    port = 5000;
    /* 通过函数入口参数 url 获得 host 地址（如果是域名，会做域名解析） */
    host = (struct hostent *) gethostbyname(url);

    if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        LOG_D("Socket error");
        return;
    }

    struct netdev *netdev = RT_NULL;

    netdev = netdev_get_by_family(AF_INET);
    if (netdev == RT_NULL)
    {
        LOG_D("get network interface device by AF_INET failed.");
    }

    LOG_D("localip:%s", inet_ntoa(netdev->ip_addr));
    /* 初始化预连接的服务端地址 */
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(port);
    listen_addr.sin_addr = *((struct in_addr *) host->h_addr);
    listen_addr.sin_addr.s_addr = netdev->ip_addr.addr;

    rt_memset(&(listen_addr.sin_zero), 0, sizeof(listen_addr.sin_zero));

    if (bind(sock_listen, (struct sockaddr * )&listen_addr, sizeof(struct sockaddr)) < 0)
    {
        LOG_D("Bind fail!");
        goto __exit;
    }

    listen(sock_listen, 2);
    LOG_D("begin listing...");

    while (1)
    {
        int sin_size = sizeof(struct sockaddr_in);

        sock_connect = accept(sock_listen, (struct sockaddr* )&connect_addr, (socklen_t* )&sin_size);
        if (sock_connect == -1)
        {
            LOG_D("no socket,waitting others socket disconnect.");
            continue;
        }

        char tid_name[10] = "cli";
        char tid_num[10];
        itoa(sock_connect, tid_num, 10);
        strcat(tid_name, tid_num);

        struct client_info *client;
        client = rt_malloc(sizeof(struct client_info));
        client->socketnum = sock_connect;
        rt_memcpy(&client->addr, &connect_addr, sizeof(struct sockaddr_in));
        client->sockaddrlen = sin_size;
        tid = rt_thread_create(tid_name, client_thread_entry, (void*) client, 4096, 25, 10);
        if (tid == RT_NULL)
        {
            LOG_D("no memery for thread %s startup failed!", tid_name);
            rt_free(client);
            continue;
        }
        rt_thread_startup(tid);
    }
    __exit: LOG_D("close listener...");
    /* 关闭这个 socket */
    closesocket(sock_listen);
    return;
}

int tcpserver_app_init(void *param)
{
    rt_thread_t tid;
    tid = rt_thread_create("tcpsrv", tcpserver_thread_entry, (void*) RT_NULL, 4096, 25, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    return 0;
}

//INIT_APP_EXPORT(tcpserver_app_init);
