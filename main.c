/*
 * Copyright (C) 2017-2018 zengfanfan.com & https://github.com/zengfanfan/.
 * All rights reserved.
 *
 * `holymsg` is a implementation of publish-subscribe pattern.
 */

#include <utils/print.h>
#include <holymsg.h>

extern int g_holymsg_i_am_master;

int main()
{
    g_holymsg_i_am_master = 1;
    holymsg_run();
    return 0;
}

