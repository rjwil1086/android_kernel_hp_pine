/* drivers/input/sensors/access/kxtik.c
 *
 * Copyright (C) 2012-2015 ROCKCHIP.
 * Author: luowei <lw@rock-chips.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <mach/gpio.h>
#include <mach/board.h> 
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <linux/kxtik.h>
#include <linux/sensor-dev.h>

#if 0
#define SENSOR_DEBUG_TYPE SENSOR_TYPE_ACCEL
//#define DBG(x...) if(sensor->pdata->type == SENSOR_DEBUG_TYPE) printk(x)
#define DBG printk
#else
#define DBG(x...)
#endif

/****************operate according to sensor chip:start************/

static int sensor_active(struct i2c_client *client, int enable, int rate)
{
	struct sensor_private_data *sensor =
	    (struct sensor_private_data *) i2c_get_clientdata(client);	
	int result = 0;
	int status = 0;
		
	sensor->ops->ctrl_data = sensor_read_reg(client, sensor->ops->ctrl_reg);
	
	//register setting according to chip datasheet		
	if(enable)
	{	
		status = KXTIK_ENABLE;	//kxtik	
		sensor->ops->ctrl_data |= status;	
	}
	else
	{
		status = ~KXTIK_ENABLE;	//kxtik
		sensor->ops->ctrl_data &= status;
	}

	DBG("%s:reg=0x%x,reg_ctrl=0x%x,enable=%d\n",__func__,sensor->ops->ctrl_reg, sensor->ops->ctrl_data, enable);
	result = sensor_write_reg(client, sensor->ops->ctrl_reg, sensor->ops->ctrl_data);
	if(result)
		printk("%s:fail to active sensor\n",__func__);
	
	return result;

} 

static int sensor_init(struct i2c_client *client)
{	
	struct sensor_private_data *sensor =
	    (struct sensor_private_data *) i2c_get_clientdata(client);	
	int result = 0;
	
	result = sensor->ops->active(client,0,0);
	if(result)
	{
		printk("%s:line=%d,error\n",__func__,__LINE__);
		return result;
	}
	
	sensor->status_cur = SENSOR_OFF;

////////////////write PC1=0 to stop sensor working
	result = sensor_write_reg(client, sensor->ops->ctrl_reg, 0);
	if(result)
	{
		printk("%s:line=%d,error\n",__func__,__LINE__);
		return result;
	}
//////////////////////////	
	result = sensor_write_reg(client, KXTIK_DATA_CTRL_REG, KXTIK_ODR50F);//KXTIK_ODR25F);
	if(result)
	{
		printk("%s:line=%d,error\n",__func__,__LINE__);
		return result;
	}

	//if(sensor->pdata->irq_enable)	//open interrupt
	{
		result = sensor_write_reg(client, KXTIK_INT_CTRL_REG1, 0x00);//0x34);//enable int,active high,need read INT_REL
		if(result)
		{
			printk("%s:line=%d,error\n",__func__,__LINE__);
			return result;
		}
	}
	
	sensor->ops->ctrl_data = (KXTIK_RES_12BIT | KXTIK_G_2G);
	result = sensor_write_reg(client, sensor->ops->ctrl_reg, sensor->ops->ctrl_data);
	if(result)
	{
		printk("%s:line=%d,error\n",__func__,__LINE__);
		return result;
	}	
	return result;
}

static int sensor_convert_data(struct i2c_client *client, char high_byte, char low_byte)
{
    s64 result;
	int rusult_tmp;
	struct sensor_private_data *sensor =
	    (struct sensor_private_data *) i2c_get_clientdata(client);	
	//int precision = sensor->ops->precision;
	switch (sensor->devid) {	
		case KXTIK_DEVID:		
//			result = (((int)high_byte << 8) | ((int)low_byte ))>>4;
			rusult_tmp = ((int)high_byte << 8) | ((int)low_byte );
			rusult_tmp = (rusult_tmp & 0x8000) ? (rusult_tmp | 0xFFFF0000) : (rusult_tmp);
			rusult_tmp >>= 4;
			result = (s64)rusult_tmp;
		if (result < KXTIK_BOUNDARY)
       			result = result* KXTIK_GRAVITY_STEP;
    		else
       			result = ~( ((~result & (0x7fff>>(16-KXTIK_PRECISION)) ) + 1) 
			   			* KXTIK_GRAVITY_STEP) + 1;
			break;

		default:
			printk(KERN_ERR "%s: devid wasn't set correctly\n",__func__);
			return -EFAULT;
    }

    return (int)result;
}

static int gsensor_report_value(struct i2c_client *client, struct sensor_axis *axis)
{
	struct sensor_private_data *sensor =
		(struct sensor_private_data *) i2c_get_clientdata(client);	

	/* Report acceleration sensor information */
	input_report_abs(sensor->input_dev, ABS_X, axis->x);
	input_report_abs(sensor->input_dev, ABS_Y, axis->y);
	input_report_abs(sensor->input_dev, ABS_Z, axis->z);
	input_sync(sensor->input_dev);
	DBG("Gsensor x==%d  y==%d z==%d\n",axis->x,axis->y,axis->z);

	return 0;
}

#define GSENSOR_MIN  10


#define SENSOR_I2C_RATE 200*1000
static int senosr_i2c_read_reg(struct i2c_adapter *i2c_adap,
			   unsigned char address, unsigned char reg,
			   unsigned int len, unsigned char *data)
{
	struct i2c_msg msgs[2];
	int res;


	msgs[0].addr = address;
	msgs[0].flags = 0;	/* write */
	msgs[0].buf = &reg;
	msgs[0].len = 1;
	msgs[0].scl_rate = SENSOR_I2C_RATE;
	
	msgs[1].addr = address;
	msgs[1].flags = 1;
	msgs[1].buf = data;
	msgs[1].len = len;
	msgs[1].scl_rate = SENSOR_I2C_RATE;	

	res = i2c_transfer(i2c_adap, msgs, 2);
	if (res == 2)
		return 0;
	else if(res == 0)
		return -EBUSY;
	else
		return res;

}
static int sensor_report_value(struct i2c_client *client)
{
	struct sensor_private_data *sensor =
			(struct sensor_private_data *) i2c_get_clientdata(client);	
    	struct sensor_platform_data *pdata = sensor->pdata;
	int ret = 0;
	int x,y,z;
	struct sensor_axis axis;	
	char buffer[6] = {0};	
	char value = 0;
	char test_reg_value[6];
	if(sensor->ops->read_len < 6)	//sensor->ops->read_len = 6
	{
		printk("%s:lenth is error,len=%d\n",__func__,sensor->ops->read_len);
		return -1;
	}
	
	memset(buffer, 0, 6);
	
	/* Data bytes from hardware xL, xH, yL, yH, zL, zH */	
	do {
		*buffer = sensor->ops->read_reg;
		ret = sensor_rx_data(client, buffer, sensor->ops->read_len);
	DBG( "%s: ACCEL_DATA = %x %x %x %x %x %x \n", __func__, buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);		
	ret = senosr_i2c_read_reg(client->adapter, client->addr, 0x21, 1, test_reg_value);   //////TEST LXH 1
	DBG( "%s: REG 0X21 VALUE = %x \n", __func__, test_reg_value[0]); ////TEST LXH 1
/*	ret = senosr_i2c_read_reg(client->adapter, client->addr, 0x1b, 1, test_reg_value);   //////TEST LXH 1
	DBG( "%s: REG 0X1b VALUE = %x \n", __func__, test_reg_value[0]); ////TEST LXH 1
		if (ret < 0)
*/		if (ret < 0)
		return ret;
	} while (0);

	//this gsensor need 6 bytes buffer
	x = sensor_convert_data(sensor->client, buffer[1], buffer[0]);	//buffer[1]:high bit 
	y = sensor_convert_data(sensor->client, buffer[3], buffer[2]);
	z = sensor_convert_data(sensor->client, buffer[5], buffer[4]);		
	DBG( "%s: x = %d y = %d z = %d \n", __func__, x, y, z);	   ////TEST LXH 1
	
	axis.x = (pdata->orientation[0])*x + (pdata->orientation[1])*y + (pdata->orientation[2])*z;
	axis.y = (pdata->orientation[3])*x + (pdata->orientation[4])*y + (pdata->orientation[5])*z;	
	axis.z = (pdata->orientation[6])*x + (pdata->orientation[7])*y + (pdata->orientation[8])*z;

	DBG( "%s: axis = %d  %d  %d \n", __func__, axis.x, axis.y, axis.z);

	//Report event  only while value is changed to save some power
	if((abs(sensor->axis.x - axis.x) > GSENSOR_MIN) || (abs(sensor->axis.y - axis.y) > GSENSOR_MIN) || (abs(sensor->axis.z - axis.z) > GSENSOR_MIN))
	{
		gsensor_report_value(client, &axis);

		/* »¥³âµØ»º´æÊý¾Ý. */
		mutex_lock(&(sensor->data_mutex) );
		sensor->axis = axis;
		mutex_unlock(&(sensor->data_mutex) );
	}

	if((sensor->pdata->irq_enable)&& (sensor->ops->int_status_reg >= 0))	//read sensor intterupt status register
	{
		
		value = sensor_read_reg(client, sensor->ops->int_status_reg);
		DBG("%s:sensor int status :0x%x\n",__func__,value);
	}
	
	return ret;
}

struct sensor_operate gsensor_kxtik_ops = {
	.name				= "kxtik",
	.type				= SENSOR_TYPE_ACCEL,		//sensor type and it should be correct
	.id_i2c				= ACCEL_ID_KXTIK,		//i2c id number
	.read_reg			= KXTIK_XOUT_L,			//read data
	.read_len			= 6,				//data length
	.id_reg				= KXTIK_WHO_AM_I,		//read device id from this register
	.id_data 			= KXTIK_DEVID,			//device id
	.precision			= KXTIK_PRECISION,		//12 bits
	.ctrl_reg 			= KXTIK_CTRL_REG1,		//enable or disable 
	.int_status_reg 		= KXTIK_INT_REL,		//intterupt status register
	.range				= {-KXTIK_RANGE,KXTIK_RANGE},	//range
	.trig				= IRQF_TRIGGER_LOW|IRQF_ONESHOT,		
	.active				= sensor_active,	
	.init				= sensor_init,
	.report				= sensor_report_value,
};

/****************operate according to sensor chip:end************/

//function name should not be changed
static struct sensor_operate *gsensor_get_ops(void)
{
	return &gsensor_kxtik_ops;
}


static int __init gsensor_kxtik_init(void)
{
	struct sensor_operate *ops = gsensor_get_ops();
	int result = 0;
	int type = ops->type;
	//ops->id_data=0x11;//add by king.xu for debug
	result = sensor_register_slave(type, NULL, NULL, gsensor_get_ops);
	DBG("%s,id_reg=%d,id_data=%d\n",__func__,ops->id_reg,ops->id_data);
	return result;
}

static void __exit gsensor_kxtik_exit(void)
{
	struct sensor_operate *ops = gsensor_get_ops();
	int type = ops->type;
	sensor_unregister_slave(type, NULL, NULL, gsensor_get_ops);
}


module_init(gsensor_kxtik_init);
module_exit(gsensor_kxtik_exit);

