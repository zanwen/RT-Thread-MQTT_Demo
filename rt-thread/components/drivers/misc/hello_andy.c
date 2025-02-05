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
        LOG_D("turn off the light");
    }
}

static rt_device_hello hello_andy_dev;

static int hello_andy_dev_init(void) {
    hello_andy_dev.name = "andy";
    hello_andy_dev.say_hello = say_hello;
    hello_dev_register(&hello_andy_dev);
    return 0;
}

INIT_DEVICE_EXPORT(hello_andy_dev_init);
