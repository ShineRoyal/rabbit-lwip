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

    /* ���յ��Ĳ���С�� 3 �� */
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

    /* ͨ��������ڲ��� url ��� host ��ַ��������������������������� */
    host = (struct hostent *) gethostbyname(url);

    /* ����һ�� socket�������� SOCK_DGRAM��UDP ���� */
    /*SOCK_STREAM => TCP*/
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        return;
    }

    /* ��ʼ��Ԥ���ӵķ���˵�ַ */
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(port);
    send_addr.sin_addr = *((struct in_addr *) host->h_addr);
    rt_memset(&(send_addr.sin_zero), 0, sizeof(send_addr.sin_zero));

//    recv_addr.sin_family = AF_INET;
//    recv_addr.sin_port = IPADDR_ANY;
//    recv_addr.sin_addr = *((struct in_addr *) host->h_addr);
//    rt_memset(&(recv_addr.sin_zero), 0, sizeof(recv_addr.sin_zero));

    /* �ܼƷ��� count ������ */
    if (connect(sock, (struct sockaddr * )&send_addr, sizeof(struct sockaddr)) < 0)
    {
        rt_kprintf("Connect fail!\n");
        goto __exit;
    }
    while (count)
    {
        /* �������ݵ�����Զ�� */
        send(sock, send_data, strlen(send_data), 0);

        /* �߳�����һ��ʱ�� */
        rt_thread_delay(50);

        char str[100];
        memset(str, 0, sizeof(str));

        recv(sock, str, sizeof(str), 0);
        rt_kprintf("%s:%d=>%s\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port), str);

        /* ����ֵ��һ */
        count--;
    }
__exit:
    /* �ر���� socket */
    closesocket(sock);
}
MSH_CMD_EXPORT(tcpclient, tcpclient);

