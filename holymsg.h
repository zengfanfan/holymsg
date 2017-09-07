//
// Created by zengfanfan on 8-31 031.
//

#ifndef HOLYMSG_H
#define HOLYMSG_H

typedef void (*holymsg_handler_t)(char *type, void *data, unsigned len);

int holymsg_publish(char *type, void *data, unsigned len);
int holymsg_subscribe(char *type, holymsg_handler_t handler);
int holymsg_run();

#endif // HOLYMSG_H
