#include <rtdevice.h>
#include <rtthread.h>

#include <stdlib.h>
#include <string.h>

#define DBG_TAG "hello_anwen"
#define DBG_LVL DBG_LOG
#include "drivers/hello.h"
#include <rtdbg.h>

static void say_hello(char *word) {
    if (strcmp(word, "hello") == 0) {
        LOG_D("open the door");
    } else {
        LOG_D("close the door");
    }
}

static hello_dev hello_anwen_dev = {
        "anwen",
        .say_hello = say_hello};

static int hello_anwen_dev_init(void) {
    hello_dev_register(&hello_anwen_dev);
    return 0;
}

INIT_DEVICE_EXPORT(hello_anwen_dev_init);
