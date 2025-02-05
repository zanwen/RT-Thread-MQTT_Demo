#ifndef __HELLO_H__
#define __HELLO_H__
#include <rtthread.h>

typedef struct {
    struct rt_device parent;
    char* name;
    void (*say_hello)(char* word);
} rt_device_hello;

int hello_dev_register(rt_device_hello* dev);

#endif /* __HELLO_H__ */
