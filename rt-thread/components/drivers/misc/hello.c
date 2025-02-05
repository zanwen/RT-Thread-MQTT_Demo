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

#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>
#include <stdlib.h>

#define DBG_TAG "hello"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define HELLO_BUF_LEN 128
static struct rt_device hello_dev;
static char hello_buffer[HELLO_BUF_LEN] = "this is hello buffer";


static rt_size_t hello_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    int len = strlen(hello_buffer);
	if(size > len) size = len;
   	strncpy(buffer, hello_buffer, size);
    return size;
}

static rt_size_t hello_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
	if(size >= HELLO_BUF_LEN) size = HELLO_BUF_LEN - 1;
	strncpy(hello_buffer, buffer, size);
	hello_buffer[size] = '\0';
	return size;
}



#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops hello_ops =
{
    RT_NULL,
    RT_NULL,
    RT_NULL,
    hello_read,
    hello_write,
    RT_NULL,
};
#endif


static int hello_dev_init(void)
{
    rt_err_t result = RT_EOK;

    hello_dev.type = RT_Device_Class_Miscellaneous;
    hello_dev.rx_indicate = RT_NULL;
    hello_dev.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    hello_dev.ops         = &hello_ops;
#else
    hello_dev.init        = RT_NULL;
    hello_dev.open        = RT_NULL;
    hello_dev.close       = RT_NULL;
    hello_dev.read        = hello_read;
    hello_dev.write       = hello_write;
    hello_dev.control     = RT_NULL;
#endif
    result = rt_device_register(&hello_dev, "hello_dev", RT_DEVICE_FLAG_RDWR);

    return result;
}

INIT_DEVICE_EXPORT(hello_dev_init);


#ifdef FINSH_USING_MSH

static int hello(int argc, char **argv)
{
    if (argc > 1)
    {
        if (argc == 3 && !strcmp(argv[1], "w"))
        {
			rt_device_t dev = rt_device_find("hello_dev");
			if(dev == RT_NULL) {
			    LOG_E("rt_device_find fail");
			    return -RT_ERROR;
			}
			if(rt_device_open(dev, RT_DEVICE_OFLAG_RDWR) != RT_EOK) {
			    LOG_E("open hello dev fail");
			    return -RT_ERROR;
			}
			rt_size_t size = rt_device_write(dev, 0, argv[2], strlen(argv[2]));
			LOG_I("write success, size => %d, data => %s", size, argv[2]);
			if (rt_device_close(dev) != RT_EOK) {
                LOG_E("close hello dev fail");
                return -RT_ERROR;
            }
        }
        else if(argc == 2 && !strcmp(argv[1], "r")) {
            rt_device_t dev = rt_device_find("hello_dev");
            if(dev == RT_NULL) {
                LOG_E("rt_device_find fail");
                return -RT_ERROR;
            }
            if(rt_device_open(dev, RT_DEVICE_OFLAG_RDONLY) != RT_EOK) {
                LOG_E("open hello dev fail");
                return -RT_ERROR;
            }
            char buffer[100] = {0};
            rt_size_t size = rt_device_read(dev, 0, buffer, 100);
            LOG_I("read success, size => %d, data => %s", size, buffer);
            if (rt_device_close(dev) != RT_EOK) {
                LOG_E("close hello dev fail");
                return -RT_ERROR;
            }
       	}
    }
    else
    {
        rt_kprintf("Usage: \n");
        rt_kprintf("hello w <string>   - write string into hello driver\n");
        rt_kprintf("hello r    		   - read string from hello driver \n");
        return -RT_ERROR;
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(hello, hello function);

#endif /* FINSH_USING_MSH */
