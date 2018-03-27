/*
 * Copyright (C) 2017-2018 zengfanfan.com & https://github.com/zengfanfan/.
 * All rights reserved.
 *
 * `holymsg` is a implementation of publish-subscribe pattern.
 */

#ifndef HOLYMSG_H
#define HOLYMSG_H

typedef void (*holymsg_handler_t)(char *type, void *data, unsigned len);

int holymsg_publish(char *type, void *data, unsigned len);
int holymsg_subscribe(char *type, holymsg_handler_t handler);
int holymsg_run();

#endif // HOLYMSG_H
