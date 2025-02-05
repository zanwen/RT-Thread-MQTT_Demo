#include <rtdevice.h>
#include <rtthread.h>

#include <stdlib.h>
#include <string.h>

#define DBG_TAG "hello_andy"
#define DBG_LVL DBG_LOG
#include "drivers/hello.h"
#include <rtdbg.h>

static void say_hello(char *word) {
    if (strcmp(word, "hi") == 0) {
        LOG_D("turn on the light");
    } else {
        LOG_D("turn of the light");
    }
}

static hello_dev hello_andy_dev = {
        "andy",
        .say_hello = say_hello};

static int hello_andy_dev_init(void) {
    hello_dev_register(&hello_andy_dev);
    return 0;
}

INIT_DEVICE_EXPORT(hello_andy_dev_init);
