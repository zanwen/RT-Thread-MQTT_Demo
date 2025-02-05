#ifndef __HELLO_H__
#define __HELLO_H__
#include <rtthread.h>

typedef struct {
    char* name;
    void (*say_hello)(char* word);
} hello_dev;

void hello_dev_register(hello_dev* dev);

#endif /* __HELLO_H__ */
