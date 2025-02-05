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

static rt_size_t hello_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
    rt_device_hello *hello_dev = (rt_device_hello *)dev;
    hello_dev->say_hello((char *)buffer);
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

int hello_dev_register(rt_device_hello* dev) {
    rt_err_t result = RT_EOK;

    dev->parent.type = RT_Device_Class_Miscellaneous;
    dev->parent.rx_indicate = RT_NULL;
    dev->parent.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    hello_dev.ops = &hello_ops;
#else
    dev->parent.init = RT_NULL;
    dev->parent.open = RT_NULL;
    dev->parent.close = RT_NULL;
    dev->parent.read = RT_NULL;
    dev->parent.write = hello_write;
    dev->parent.control = RT_NULL;
#endif
    result = rt_device_register(&dev->parent, dev->name, RT_DEVICE_FLAG_RDWR);

    return result;
}

static int hello(int argc, char **argv) {
    if (argc == 3) {
        rt_device_t dev = rt_device_find(argv[1]);
        if (dev == RT_NULL) {
            LOG_E("rt_device_find fail");
            return -RT_ERROR;
        }
        if (rt_device_open(dev, RT_DEVICE_OFLAG_RDWR) != RT_EOK) {
            LOG_E("open hello dev fail");
            return -RT_ERROR;
        }
        rt_size_t  size = rt_device_write(dev, 0, argv[2], strlen(argv[2]));
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
        rt_kprintf("hello <dev> <greeting> greet to dev\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(hello, hello function);
