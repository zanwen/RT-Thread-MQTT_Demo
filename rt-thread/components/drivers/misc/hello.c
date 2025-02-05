/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-07     aozima       the first version
 * 2018-11-16     Ernest Chen  add finsh command and update adc function
 */

#include <rtdevice.h>
#include <rtthread.h>

#include <stdlib.h>
#include <string.h>

#define DBG_TAG "hello"
#define DBG_LVL DBG_LOG
#include "drivers/hello.h"
#include <rtdbg.h>

static struct rt_device rt_device_hello;
hello_dev *hello_devices[100];
int hello_devices_cnt = 0;

void hello_dev_register(hello_dev *dev) {
    hello_devices[hello_devices_cnt++] = dev;
}

hello_dev *hello_dev_get(char *name) {
    for (int i = 0; i < hello_devices_cnt; i++) {
        if (strcmp(hello_devices[i]->name, name) == 0) return hello_devices[i];
    }
    return RT_NULL;
}

static rt_size_t hello_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
    char* token = strstr((char *)buffer, ",");
    if(token == RT_NULL) {
        LOG_E("invalid usage for hello cmd");
        return 0;
    }
    *token = '\0';
    hello_dev* hello_device =  hello_dev_get((char *)buffer);
    if(hello_device == RT_NULL) {
        LOG_E("hello device %s not found", buffer);
        return 0;
    }
    hello_device->say_hello(token + 1);
    return size;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops hello_ops = {
        RT_NULL,
        RT_NULL,
        RT_NULL,
        RT_NULL,
        hello_write,
        RT_NULL,
};
#endif

static int hello_dev_init(void) {
    rt_err_t result = RT_EOK;

    rt_device_hello.type = RT_Device_Class_Miscellaneous;
    rt_device_hello.rx_indicate = RT_NULL;
    rt_device_hello.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    hello_dev.ops = &hello_ops;
#else
    rt_device_hello.init = RT_NULL;
    rt_device_hello.open = RT_NULL;
    rt_device_hello.close = RT_NULL;
    rt_device_hello.read = RT_NULL;
    rt_device_hello.write = hello_write;
    rt_device_hello.control = RT_NULL;
#endif
    result = rt_device_register(&rt_device_hello, "hello_dev", RT_DEVICE_FLAG_RDWR);

    return result;
}

INIT_DEVICE_EXPORT(hello_dev_init);

static int hello(int argc, char **argv) {
    if (argc == 2) {
        rt_device_t dev = rt_device_find("hello_dev");
        if (dev == RT_NULL) {
            LOG_E("rt_device_find fail");
            return -RT_ERROR;
        }
        if (rt_device_open(dev, RT_DEVICE_OFLAG_RDWR) != RT_EOK) {
            LOG_E("open hello dev fail");
            return -RT_ERROR;
        }
        rt_size_t  size = rt_device_write(dev, 0, argv[1], strlen(argv[1]));
        if(size > 0) {
            LOG_D("write %d bytes to hello dev", size);
        } else {
            LOG_E("write fail");
        }
        if (rt_device_close(dev) != RT_EOK) {
            LOG_E("close hello dev fail");
            return -RT_ERROR;
        }
    } else {
        rt_kprintf("Usage: \n");
        rt_kprintf("hello <dev>,<greeting> greet to dev\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(hello, hello function);
