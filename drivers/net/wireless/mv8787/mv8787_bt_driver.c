#if defined(CONFIG_WIFI_CONTROL_FUNC)
#include <linux/platform_device.h>
#include <mach/board.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
struct bt_irq_data{
    struct resource *bt_irqres;
    unsigned int wake_irq;
    int removed;
};
static struct bt_irq_data * pbt_irq_data;
static irqreturn_t mv8787_bt_irq_handler (int irq, void *dev_id) 
{ 
    //struct bt_irq_data  *bt_irq = dev_id;
    disable_irq_nosync(pbt_irq_data->wake_irq);
    printk(KERN_ERR "%s successfully for bluetooth!\n",__FUNCTION__);
    return IRQ_HANDLED; 
} 
static int bt_probe(struct platform_device *pdev)
{
    /* Power On */
    printk("%s:%d\n", __FUNCTION__,__LINE__);
    struct bt_platform_data *bt_ctrl =
        (struct bt_platform_data *)(pdev->dev.platform_data);

    printk("=== %s\n", __FUNCTION__);
	
    if (bt_ctrl && bt_ctrl->set_power) {
        printk("=== set_power(1)\n");
        bt_ctrl->set_power(1);
        msleep(100);
    }
    
    pbt_irq_data = kzalloc(sizeof(struct bt_irq_data), GFP_KERNEL);
    if(!pbt_irq_data){
        printk("kzalloc irq data for mv8787 bluetooth failed!\n");
        return -1;
    }

    pbt_irq_data->bt_irqres =
        platform_get_resource_byname(pdev, IORESOURCE_IRQ,
            "mv8787_bt_irq");
    
    if(pbt_irq_data->bt_irqres != NULL){
	 pbt_irq_data->wake_irq = gpio_to_irq(pbt_irq_data->bt_irqres->start);
	 pbt_irq_data->removed = 1;
    }	

    printk("=== end bt_probe\n");
    
    return 0;
}

static int bt_remove(struct platform_device *pdev)
{
    /* Power Off */
    printk("%s:%d\n", __FUNCTION__,__LINE__);
    struct bt_platform_data *bt_ctrl =
        (struct bt_platform_data *)(pdev->dev.platform_data);
       
    printk("=== %s\n", __FUNCTION__);

    if (bt_ctrl && bt_ctrl->set_power) {
        printk("=== set_power(0)\n");
        bt_ctrl->set_power(0);
    }
    if(pbt_irq_data->bt_irqres != NULL && !pbt_irq_data->removed){
        disable_irq_wake(pbt_irq_data->wake_irq);
        disable_irq(pbt_irq_data->wake_irq);
        free_irq(gpio_to_irq(pbt_irq_data->bt_irqres->start),NULL);
	 pbt_irq_data->removed = 1;
        gpio_free(pbt_irq_data->bt_irqres->start);
    }
    kfree(pbt_irq_data);
    return 0;
}

static int bt_suspend(struct platform_device *pdev, pm_message_t state)
{
    int ret = 0;
    printk("%s:%d\n", __FUNCTION__,__LINE__);
    if(pbt_irq_data->bt_irqres != NULL && pbt_irq_data->wake_irq != 0){
        if (gpio_request(pbt_irq_data->bt_irqres->start, "mv8787_bt_irq")) {
            printk("!!! mv8787 gpio_request for bluetooth failed !!!--%s,line:%d\n",__FUNCTION__,__LINE__);
        }else{
            gpio_direction_input(pbt_irq_data->bt_irqres->start);
            ret = request_irq(pbt_irq_data->wake_irq, mv8787_bt_irq_handler, 
			IRQF_TRIGGER_HIGH, "mv8787_bt_irq", NULL);
            if(ret){
                gpio_free(pbt_irq_data->bt_irqres->start);
                printk("!!! mv8787 request_irq for bluetooth failed !!!--%s,line:%d\n",__FUNCTION__,__LINE__);
            }else{
                pbt_irq_data->removed = 0;
		  enable_irq(pbt_irq_data->wake_irq);
                enable_irq_wake(pbt_irq_data->wake_irq);
            }
        }
    }	
    return 0;
}

static int bt_resume(struct platform_device *pdev)
{
    printk("%s:%d\n", __FUNCTION__,__LINE__);
    if(pbt_irq_data->bt_irqres != NULL && !pbt_irq_data->removed){
        disable_irq_wake(pbt_irq_data->wake_irq);
        disable_irq(pbt_irq_data->wake_irq);
        free_irq(gpio_to_irq(pbt_irq_data->bt_irqres->start),NULL);
	 pbt_irq_data->removed = 1;
        gpio_free(pbt_irq_data->bt_irqres->start);
    }
    return 0;
}

struct platform_driver bt_driver = {
	.probe          = bt_probe,
	.remove         = bt_remove,
	.suspend        = bt_suspend,
	.resume         = bt_resume,
	.driver         = {
	.name   = "mv8787_rfkill",
	}
};
EXPORT_SYMBOL(bt_driver);
#endif
