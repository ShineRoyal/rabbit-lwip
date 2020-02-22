/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-02-21     ShineRoyal   the first version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct people
{
    int age;
    char name[10];
};

struct boss
{
    int money;
    struct people p;
    int employee;
};

void test(void *param)
{
    printf("test\n");
    struct boss *b=param;
    printf("money:%d\n", b->money);
    printf("employee:%d\n", b->employee);
    printf("name:%s\n", b->p.name);
    free(b);
}
int main1()
{
    printf("running...\n");
    struct people *p;
    struct boss* Tom;

    p=malloc(sizeof(struct people));
    Tom=malloc(sizeof(struct boss));

    p->age = 25;
    strcpy(p->name, "Tom");
    printf("name:%s\n", p->name);
    printf("age:%d\n", p->age);

    memcpy(&Tom->p, p,sizeof(struct people));
    free(p);
    Tom->money = 1000;
    Tom->employee = 250;
    printf("name:%s\n", Tom->p.name);
    printf("age:%d\n", Tom->p.age);
    printf("money:%d\n", Tom->money);
    printf("employee:%d\n", Tom->employee);

    test((void*) Tom);
    return 0;
}
