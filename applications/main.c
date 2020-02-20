/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-09-09     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* PLEASE DEFINE the LED0 pin for your board, such as: PA5 */
#define LED0_PIN    GET_PIN(E, 5)

int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    LOG_D("Hello RT-Thread!");
    while (count++)
    {
        /* set LED0 pin level to high or low */
        rt_pin_write(LED0_PIN, count % 2);

        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

static const char *send_data = "GET /download/rt-thread.txt HTTP/1.1\r\n"
        "Host: www.rt-thread.org\r\n"
        "User-Agent: rtthread/4.0.1 rtt\r\n\r\n";

void udpclient(int argc, char **argv)
{
    int sock, port, count;
    struct hostent *host;
    struct sockaddr_in send_addr, recv_addr;
    const char *url;

    /* ���յ��Ĳ���С�� 3 �� */
    if (argc < 3)
    {
        rt_kprintf("Usage: udpclient URL PORT [COUNT = 10]\n");
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
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        return;
    }

    /* ��ʼ��Ԥ���ӵķ���˵�ַ */
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(port);
    send_addr.sin_addr = *((struct in_addr *) host->h_addr);
    rt_memset(&(send_addr.sin_zero), 0, sizeof(send_addr.sin_zero));

    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = IPADDR_ANY;
    recv_addr.sin_addr = *((struct in_addr *) host->h_addr);
    rt_memset(&(recv_addr.sin_zero), 0, sizeof(recv_addr.sin_zero));

    /* �ܼƷ��� count ������ */
    while (count)
    {
        /* �������ݵ�����Զ�� */
        sendto(sock, send_data, strlen(send_data), 0, (struct sockaddr * )&send_addr, sizeof(struct sockaddr));
        /* �߳�����һ��ʱ�� */
        rt_thread_delay(50);
        char str[100];
        memset(str, 0, sizeof(str));
        recvfrom(sock, str, sizeof(str), 0, &recv_addr, sizeof(recv_addr));
        rt_kprintf("%s:%d=>%s\n", inet_ntoa(recv_addr.sin_addr), ntohs(recv_addr.sin_port), str);
        /* ����ֵ��һ */
        count--;
    }

    /* �ر���� socket */
    closesocket(sock);
}
MSH_CMD_EXPORT(udpclient, udpclient);
