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

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops adc_ops =
{
    RT_NULL,
    RT_NULL,
    RT_NULL,
    _adc_read,
    RT_NULL,
    _adc_control,
};
#endif

rt_err_t rt_hw_adc_register(rt_adc_device_t device, const char *name, const struct rt_adc_ops *ops, const void *user_data)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(ops != RT_NULL && ops->convert != RT_NULL);

    device->parent.type = RT_Device_Class_Miscellaneous;
    device->parent.rx_indicate = RT_NULL;
    device->parent.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    device->parent.ops         = &adc_ops;
#else
    device->parent.init        = RT_NULL;
    device->parent.open        = RT_NULL;
    device->parent.close       = RT_NULL;
    device->parent.read        = _adc_read;
    device->parent.write       = RT_NULL;
    device->parent.control     = _adc_control;
#endif
    device->ops = ops;
    device->parent.user_data = (void *)user_data;

    result = rt_device_register(&device->parent, name, RT_DEVICE_FLAG_RDWR);

    return result;
}

#ifdef FINSH_USING_MSH

static int adc(int argc, char **argv)
{
    int value = 0;
    int result = RT_EOK;
    static rt_adc_device_t adc_device = RT_NULL;
    char *result_str;

    if (argc > 1)
    {
        if (!strcmp(argv[1], "probe"))
        {
            if (argc == 3)
            {
                adc_device = (rt_adc_device_t)rt_device_find(argv[2]);
                result_str = (adc_device == RT_NULL) ? "failure" : "success";
                rt_kprintf("probe %s %s \n", argv[2], result_str);
            }
            else
            {
                rt_kprintf("adc probe <adc_name>   - probe adc by name\n");
            }
        }
        else
        {
            if (adc_device == RT_NULL)
            {
                rt_kprintf("Please using 'adc probe <adc_name>' first\n");
                return -RT_ERROR;
            }
            if (!strcmp(argv[1], "enable"))
            {
                if (argc == 3)
                {
                    result = rt_adc_enable(adc_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "success" : "failure";
                    rt_kprintf("%s channel %d enables %s \n", adc_device->parent.parent.name, atoi(argv[2]), result_str);
                }
                else
                {
                    rt_kprintf("adc enable <channel>   - enable adc channel\n");
                }
            }
            else if (!strcmp(argv[1], "read"))
            {
                if (argc == 3)
                {
                    value = rt_adc_read(adc_device, atoi(argv[2]));
                    rt_kprintf("%s channel %d  read value is 0x%08X \n", adc_device->parent.parent.name, atoi(argv[2]), value);
                }
                else
                {
                    rt_kprintf("adc read <channel>     - read adc value on the channel\n");
                }
            }
            else if (!strcmp(argv[1], "disable"))
            {
                if (argc == 3)
                {
                    result = rt_adc_disable(adc_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "success" : "failure";
                    rt_kprintf("%s channel %d disable %s \n", adc_device->parent.parent.name, atoi(argv[2]), result_str);
                }
                else
                {
                    rt_kprintf("adc disable <channel>  - disable adc channel\n");
                }
            }
            else
            {
                rt_kprintf("Unknown command. Please enter 'adc' for help\n");
            }
        }
    }
    else
    {
        rt_kprintf("Usage: \n");
        rt_kprintf("adc probe <adc_name>   - probe adc by name\n");
        rt_kprintf("adc read <channel>     - read adc value on the channel\n");
        rt_kprintf("adc disable <channel>  - disable adc channel\n");
        rt_kprintf("adc enable <channel>   - enable adc channel\n");
        result = -RT_ERROR;
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(adc, adc function);

#endif /* FINSH_USING_MSH */
