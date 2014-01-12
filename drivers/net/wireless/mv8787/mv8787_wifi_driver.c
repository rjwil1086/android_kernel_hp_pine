#if defined(CONFIG_WIFI_CONTROL_FUNC)
#include <linux/platform_device.h>
#include <mach/board.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
struct wifi_irq_data{
    struct resource *wifi_irqres;
    unsigned int wake_irq;
    int removed;
};
static struct wifi_irq_data * pwifi_irq_data;
static irqreturn_t mv8787_wifi_irq_handler (int irq, void *dev_id) 
{ 
    //struct wifi_irq_data  *wifi_irq = dev_id;
    disable_irq_nosync(pwifi_irq_data->wake_irq);
    printk(KERN_ERR "%s successfully for WiFi!\n",__FUNCTION__);
    return (IRQ_HANDLED); 
} 
static int wifi_probe(struct platform_device *pdev)
{
    /* Power On */
    struct wifi_platform_data *wifi_ctrl =
    (struct wifi_platform_data *)(pdev->dev.platform_data);

    printk("=== %s\n", __FUNCTION__);
    if (wifi_ctrl && wifi_ctrl->set_power) {
        printk("=== set_power(1)\n");
        wifi_ctrl->set_power(1);
        msleep(100);
    }

    pwifi_irq_data = kzalloc(sizeof(struct wifi_irq_data), GFP_KERNEL);
    if(!pwifi_irq_data){
        printk("kzalloc wifi irq data for mv8787 failed!\n");
        return -1;
    }
    pwifi_irq_data->wifi_irqres =
        platform_get_resource_byname(pdev, IORESOURCE_IRQ,
            "mv8787_wlan_irq");
    
    if(pwifi_irq_data->wifi_irqres != NULL){
	 pwifi_irq_data->wake_irq = gpio_to_irq(pwifi_irq_data->wifi_irqres->start);
	 pwifi_irq_data->removed = 1;
    }	
    printk("=== end wifi_probe\n");
    
    return 0;
}

static int wifi_remove(struct platform_device *pdev)
{
    /* Power Off */
    struct wifi_platform_data *wifi_ctrl =
       (struct wifi_platform_data *)(pdev->dev.platform_data);
       
    printk("=== %s\n", __FUNCTION__);
    if (wifi_ctrl && wifi_ctrl->set_power) {
        printk("=== set_power(0)\n");
        wifi_ctrl->set_power(0);
    }
    if(pwifi_irq_data->wifi_irqres != NULL && !pwifi_irq_data->removed){
        disable_irq_wake(pwifi_irq_data->wake_irq);
        disable_irq(pwifi_irq_data->wake_irq);
        free_irq(gpio_to_irq(pwifi_irq_data->wifi_irqres->start),NULL);
	 pwifi_irq_data->removed = 1;
        gpio_free(pwifi_irq_data->wifi_irqres->start);
    }
    kfree(pwifi_irq_data);
    return 0;
}

static int wifi_suspend(struct platform_device *pdev, pm_message_t state)
{
    int ret = 0;
    if(pwifi_irq_data->wifi_irqres != NULL && pwifi_irq_data->wake_irq != 0){
        if (gpio_request(pwifi_irq_data->wifi_irqres->start, "mv8787_wlan_irq")) {
            printk("!!! mv8787 gpio_request failed !!!--%s,line:%d\n",__FUNCTION__,__LINE__);
        }else{
            gpio_direction_input(pwifi_irq_data->wifi_irqres->start);
            ret = request_irq(pwifi_irq_data->wake_irq, mv8787_wifi_irq_handler, 
			IRQF_TRIGGER_LOW, "mv8787_wlan_irq", (void*)pwifi_irq_data);
            if(ret){
                gpio_free(pwifi_irq_data->wifi_irqres->start);
                printk("!!! mv8787 request_irq failed !!!--%s,line:%d\n",__FUNCTION__,__LINE__);
            }else{
                pwifi_irq_data->removed = 0;
		  enable_irq(pwifi_irq_data->wake_irq);
                enable_irq_wake(pwifi_irq_data->wake_irq);
            }
        }
    }	
    printk("=== %s\n", __FUNCTION__);
    return 0;
}

static int wifi_resume(struct platform_device *pdev)
{
    if(pwifi_irq_data->wifi_irqres != NULL && !pwifi_irq_data->removed){
        disable_irq_wake(pwifi_irq_data->wake_irq);
        disable_irq(pwifi_irq_data->wake_irq);
        free_irq(gpio_to_irq(pwifi_irq_data->wifi_irqres->start),NULL);
	 pwifi_irq_data->removed = 1;
        gpio_free(pwifi_irq_data->wifi_irqres->start);
    }
    printk("=== %s\n", __FUNCTION__);
    return 0;
}

struct platform_driver wifi_driver = {
	.probe          = wifi_probe,
	.remove         = wifi_remove,
	.suspend        = wifi_suspend,
	.resume         = wifi_resume,
	.driver         = {
	.name   = "mv8787_wlan",
	}
};
EXPORT_SYMBOL(wifi_driver);
#endif
