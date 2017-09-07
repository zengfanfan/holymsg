#include <unistd.h>
#include <sys/resource.h>
#include <utils/print.h>
#include <utils/dict.h>
#include <utils/msg.h>
#include <holymsg.h>

#define MAX_BUF_LEN (1024*1024) // 1M
#define SUBSCRIBE_MSG_TYPE ".holymsg.subscribe"

typedef struct {
    int key;
    char type[0];
} subscribe_msg_t;

static holymq_t mq;
static dict_t maps;
int __holymsg_i_am_master = 0;

static int holymsg_init()
{
    static int inited = 0;
    int key = __holymsg_i_am_master ? 0 : getpid();

    if (inited) {
        return 1;
    }
    
    if (!holymq_init(&mq, key)) {
        return 0;
    }

    if (!dict_init(&maps)) {
        return 0;
    }

    inited = 1;
    return 1;
}

int holymsg_publish(char *type, void *data, unsigned len)
{
    char *buf;
    int type_len;

    if (!type || !holymsg_init()) {
        return 0;
    }
    
    type_len = strlen(type) + 1;

    if (!data) {
        len = 0;
    }

    buf = malloc(type_len + len);
    if (!buf) {
        return 0;
    }

    strcpy(buf, type);
    memcpy(buf + type_len, data, len);
    
    // send to holymsg
    if (!mq.put(&mq, 0, buf, type_len + len)) {
        free(buf);
        return 0;
    }

    free(buf);
    return 1;
}

int holymsg_subscribe(char *type, holymsg_handler_t handler)
{
    char *data;
    subscribe_msg_t *msg;
    unsigned dlen;

    if (!type) {
        return 0;
    }
    
    if (!holymsg_init()) {
        return 0;
    }

    if (!maps.set_sp(&maps, type, handler)) {
        return 0;
    }
    
    // tell holymsg my pid and type
    dlen = sizeof(SUBSCRIBE_MSG_TYPE) + sizeof(*msg) + strlen(type) + 1;
    data = malloc(dlen);
    if (!data) {
        return 0;
    }

    strcpy(data, SUBSCRIBE_MSG_TYPE);
    msg = (subscribe_msg_t *)(data + sizeof(SUBSCRIBE_MSG_TYPE));
    msg->key = mq.key;
    strcpy(msg->type, type);

    if (!mq.put(&mq, 0, data, dlen)) {
        free(data);
        return 0;
    }
    
    free(data);
    return 1;
}

static void msg_handler(void *buf, int len)
{
    char *type;
    int key, dlen;
    subscribe_msg_t *msg;
    void *data;
    holymsg_handler_t handler;
    
    if (!strcmp(buf, SUBSCRIBE_MSG_TYPE)) {
        msg = buf + strlen(buf) + 1;
        DEBUG("%d subcribe %s", msg->key, msg->type);
        maps.set_si(&maps, msg->type, msg->key);
        return;
    }

    type = buf;
    data = buf + strlen(type) + 1;
    dlen = len - (buf - data);

    if (!mq.key) { // master
        //DEBUG("forward %s", type);
        key = maps.get_si(&maps, type);
        if (key) {
            mq.put(&mq, key, buf, len);
        }
    } else { // others
        //DEBUG("handle %s", type);
        handler = maps.get_sp(&maps, type);
        if (handler) {
            handler(type, data, dlen);
        }
    }
}

int holymsg_run()
{
    char buf[MAX_BUF_LEN];
    int len;
    
    if (!holymsg_init()) {
        return 0;
    }
    
    for (;;) {
        len = mq.get(&mq, buf, sizeof(buf));
        if (!len) {
            sleep(1);
        } else {
            msg_handler(buf, len);
        }
    }
}

