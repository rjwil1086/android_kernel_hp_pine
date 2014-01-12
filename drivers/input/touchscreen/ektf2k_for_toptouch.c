/* drivers/input/touchscreen/ektf2k.c - ELAN EKTF2K verions of driver
 *
 * Copyright (C) 2011 Elan Microelectronics Corporation.
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
 * 2011/12/06: The first release, version 0x0001
 * 2012/2/15:  The second release, version 0x0002 for new bootcode
 * 2012/5/8:   Release version 0x0003 for china market
 *             Integrated 2 and 5 fingers driver code together and
 *             auto-mapping resolution.
 */

//#define ELAN_BUFFER_MODE

#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>


// for linux 2.6.36.3
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/ioctl.h>
//#include "ektf2k.h"
#include <linux/i2c/ektf2k.h>

#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/async.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <mach/gpio.h>
#include <linux/irq.h>
#include <mach/board.h>
#include <linux/input/mt.h>
#include <linux/kthread.h>

#include "ft_elan_tp_hook.h"

extern tp_status_t g_tp_status;

//#define DEBUG 
#ifdef DEBUG
#define dbg_info(fmt, arg...) printk(fmt, ##arg);
#else
#define dbg_info(fmt, arg...)
#endif

#define PACKET_SIZE		    	 40
#define PWR_STATE_DEEP_SLEEP	0
#define PWR_STATE_NORMAL		1
#define PWR_STATE_MASK			BIT(3)

#define CMD_S_PKT			0x52
#define CMD_R_PKT			0x53
#define CMD_W_PKT			0x54

#define HELLO_PKT			0x55
#define NORMAL_PKT			0x5D

#define HELLO_PKT	    0x55
#define TWO_FINGERS_PKT     0x5A
#define FIVE_FINGERS_PKT    0x6D
#define TEN_FINGERS_PKT	    0x62

#define RESET_PKT			0x77
#define CALIB_PKT			0xA8

// modify
#define SYSTEM_RESET_PIN_SR 10

//Add these Define
#define IAP_IN_DRIVER_MODE 	1
#define IAP_PORTION         1
#define PAGERETRY           30
#define IAPRESTART           5


// For Firmware Update 
#define ELAN_IOCTLID	0xD0
#define IOCTL_I2C_SLAVE	_IOW(ELAN_IOCTLID,  1, int)
#define IOCTL_MAJOR_FW_VER  _IOR(ELAN_IOCTLID, 2, int)
#define IOCTL_MINOR_FW_VER  _IOR(ELAN_IOCTLID, 3, int)
#define IOCTL_RESET  _IOR(ELAN_IOCTLID, 4, int)
#define IOCTL_IAP_MODE_LOCK  _IOR(ELAN_IOCTLID, 5, int)
#define IOCTL_CHECK_RECOVERY_MODE  _IOR(ELAN_IOCTLID, 6, int)
#define IOCTL_FW_VER  _IOR(ELAN_IOCTLID, 7, int)
#define IOCTL_X_RESOLUTION  _IOR(ELAN_IOCTLID, 8, int)
#define IOCTL_Y_RESOLUTION  _IOR(ELAN_IOCTLID, 9, int)
#define IOCTL_FW_ID  _IOR(ELAN_IOCTLID, 10, int)
#define IOCTL_ROUGH_CALIBRATE  _IOR(ELAN_IOCTLID, 11, int)
#define IOCTL_IAP_MODE_UNLOCK  _IOR(ELAN_IOCTLID, 12, int)
#define IOCTL_I2C_INT  _IOR(ELAN_IOCTLID, 13, int)
#define IOCTL_RESUME  _IOR(ELAN_IOCTLID, 14, int)
#define IOCTL_POWER_LOCK  _IOR(ELAN_IOCTLID, 15, int)
#define IOCTL_POWER_UNLOCK  _IOR(ELAN_IOCTLID, 16, int)
#define IOCTL_2WIREICE   		_IOR(ELAN_IOCTLID, 19, int) 
#if IAP_PORTION
#define IOCTL_FW_UPDATE  _IOR(ELAN_IOCTLID, 17, int)
#define IOCTL_GET_UPDATE_PROGREE	_IOR(CUSTOMER_IOCTLID,  2, int)
#endif

#define CUSTOMER_IOCTLID	0xA0
#define IOCTL_CIRCUIT_CHECK  _IOR(CUSTOMER_IOCTLID, 1, int)

static uint8_t RECOVERY=0x00;
static int FW_VERSION=0x00;
static int X_RESOLUTION=896;//961; 
static int Y_RESOLUTION=576;////641; 
static int FW_ID=0x00;
static int FW_BC=0x00;

static int checksum_err;

static int work_lock=0x00;
static int power_lock=0x00;
static int circuit_ver=0x01;

static int tpd_flag = 0;

/*++++i2c transfer start+++++++*/
static int file_fops_addr=0x15;
/*++++i2c transfer end+++++++*/
struct point_status {
	u8 id;
	u16 x;
	u16 y;
};

#if IAP_PORTION
static uint8_t ic_status=0x00;	//0:OK 1:master fail 2:slave fail
static int update_progree=0;
static uint8_t I2C_DATA[3] = {0x15, 0x20, 0x21};/*I2C devices address*/  
static int is_OldBootCode = 0; // 0:new 1:old


/*The newest firmware, if update must be changed here*/
static uint8_t file_fw_data[] = {
		#include "BYD_T7H_TOPTOUCH_FWID_16E0_VE_5502_Addr0x15.i"
};


enum
{
	PageSize		= 132,
	PageNum		        = 249,
	ACK_Fail		= 0x00,
	ACK_OK			= 0xAA,
	ACK_REWRITE		= 0x55,
};

enum
{
	E_FD			= -1,
};
#endif

static uint8_t sbutton           = 0;
#define ELAN_KEY_HOME     0x10 
#define ELAN_KEY_BACK	  0x08 
#define ELAN_KEY_SEARCH   0x80
#define ELAN_KEY_ENTER    0x04  
#define MAX_FINGER_NUM	5


struct elan_toptouch_ts_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct workqueue_struct *elan_wq;
	struct work_struct work;
	struct early_suspend early_suspend;
	int intr_gpio;
	int rst_gpio;
	
// Firmware Information
	int fw_ver;
	int fw_id;
	int fw_bc;
	
	int x_resolution;
	int y_resolution;
// For Firmare Update 
	struct miscdevice firmware;
};


static struct elan_toptouch_ts_data *private_ts;
static struct task_struct *thread = NULL;
static DECLARE_WAIT_QUEUE_HEAD(waiter);

static int __fw_packet_handler(struct i2c_client *client);
static int elan_toptouch_ts_rough_calibrate(struct i2c_client *client);
static int elan_toptouch_ts_resume(struct i2c_client *client);



#if IAP_PORTION
static int IAPReset(struct i2c_client *client);
static int Update_FW_One(/*struct file *filp,*/ struct i2c_client *client, int recovery);
static int __hello_packet_handler(struct i2c_client *client);
#endif


// For Firmware Update 
static int elan_iap_open(struct inode *inode, struct file *filp){ 
//	printk("[ELAN_TOPTOUCH]into elan_iap_open\n");
		if (private_ts == NULL)  printk("private_ts is NULL~~~");
		
	return 0;
}


static int elan_iap_release(struct inode *inode, struct file *filp){    
	return 0;
}


static ssize_t elan_iap_write(struct file *filp, const char *buff, size_t count, loff_t *offp){  
    int ret;
    char *tmp;
//    printk("[ELAN_TOPTOUCH]into elan_iap_write\n");

    /*++++i2c transfer start+++++++*/    	
    //struct i2c_adapter *adap = private_ts->client->adapter;    	
    //struct i2c_msg msg;
    /*++++i2c transfer end+++++++*/	

    if (count > 8192)
        count = 8192;

    tmp = kmalloc(count, GFP_KERNEL);
    
    if (tmp == NULL)
        return -ENOMEM;

    if (copy_from_user(tmp, buff, count)) {
        return -EFAULT;
    }

/*++++i2c transfer start+++++++*/
#if 0
	//down(&worklock);
	msg.addr = file_fops_addr;
	msg.flags = 0x00;// 0x00
	msg.len = count;
	msg.buf = (char *)tmp;
	//up(&worklock);
	ret = i2c_transfer(adap, &msg, 1);
#else
	
    ret = i2c_master_send(private_ts->client, tmp, count);
#endif	
/*++++i2c transfer end+++++++*/

    //if (ret != count) printk("ELAN i2c_master_send fail, ret=%d \n", ret);
    kfree(tmp);
    //return ret;
    return (ret == 1) ? count : ret;

}



static ssize_t elan_iap_read(struct file *filp, char *buff, size_t count, loff_t *offp){    
    char *tmp;
    int ret;  
    long rc;
//    printk("[ELAN_TOPTOUCH]into elan_iap_read\n");
   /*++++i2c transfer start+++++++*/
    //	struct i2c_adapter *adap = private_ts->client->adapter;
    //	struct i2c_msg msg;
/*++++i2c transfer end+++++++*/
    if (count > 8192)
        count = 8192;

    tmp = kmalloc(count, GFP_KERNEL);

    if (tmp == NULL)
        return -ENOMEM;
/*++++i2c transfer start+++++++*/
#if 0
	//down(&worklock);
	msg.addr = file_fops_addr;
	//msg.flags |= I2C_M_RD;
	msg.flags = 0x00;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.buf = tmp;
	//up(&worklock);
	ret = i2c_transfer(adap, &msg, 1);
#else
    ret = i2c_master_recv(private_ts->client, tmp, count);
#endif
/*++++i2c transfer end+++++++*/
    if (ret >= 0)
        rc = copy_to_user(buff, tmp, count);
    
    kfree(tmp);

    //return ret;
    return (ret == 1) ? count : ret;
	
}



static long elan_iap_ioctl(/*struct inode *inode,*/ struct file *filp,    unsigned int cmd, unsigned long arg){

	int __user *ip = (int __user *)arg;
	printk("[ELAN_TOPTOUCH]into elan_iap_ioctl\n");
	printk("cmd value %x\n",cmd);
	
	switch (cmd) {        
		case IOCTL_I2C_SLAVE: 
			//private_ts->client->addr = (int __user)arg;
			file_fops_addr = 0x15;
			break;   
		case IOCTL_MAJOR_FW_VER:            
			break;        
		case IOCTL_MINOR_FW_VER:            
			break;        
		case IOCTL_RESET:
			#if IAP_PORTION
			IAPReset(private_ts->client);
			#endif
		    
			break;
			
		case IOCTL_IAP_MODE_LOCK:
			if(work_lock==0)
			{
				work_lock=1;
				disable_irq(private_ts->client->irq);
				//cancel_work_sync(&private_ts->work);
			}
			break;
		case IOCTL_IAP_MODE_UNLOCK:
			if(work_lock==1)
			{			
				work_lock=0;
				enable_irq(private_ts->client->irq);
			}
			break;
		case IOCTL_CHECK_RECOVERY_MODE:
			return RECOVERY;
			break;
		case IOCTL_FW_VER:
			__fw_packet_handler(private_ts->client);
			return FW_VERSION;
			break;
		case IOCTL_X_RESOLUTION:
			__fw_packet_handler(private_ts->client);
			return X_RESOLUTION;
			break;
		case IOCTL_Y_RESOLUTION:
			__fw_packet_handler(private_ts->client);
			return Y_RESOLUTION;
			break;
		case IOCTL_FW_ID:
			__fw_packet_handler(private_ts->client);
			return FW_ID;
			break;
		case IOCTL_ROUGH_CALIBRATE:
			return elan_toptouch_ts_rough_calibrate(private_ts->client);
		case IOCTL_I2C_INT:
			put_user(gpio_get_value(private_ts->intr_gpio), ip);
			break;	
		case IOCTL_RESUME:
			elan_toptouch_ts_resume(private_ts->client);
			break;	
		case IOCTL_CIRCUIT_CHECK:
			return circuit_ver;
			break;
		case IOCTL_POWER_LOCK:
			power_lock=1;
			break;
		case IOCTL_POWER_UNLOCK:
			power_lock=0;
			break;
#if IAP_PORTION		
		case IOCTL_GET_UPDATE_PROGREE:
			update_progree=(int __user)arg;
			break; 

		case IOCTL_FW_UPDATE:
			Update_FW_One(private_ts->client, 0);
			break; 
#endif
		case IOCTL_2WIREICE:
			printk("[ELAN_TOPTOUCH] IOCTL_2WIREICE\n");
			break; 
		default:            
			break;   
	}       
	return 0;
}



static  struct file_operations elan_touch_fops = {    
        .open       =elan_iap_open,    
        .write      =elan_iap_write,    
        .read       =elan_iap_read,    
        .release    =elan_iap_release,    
	.unlocked_ioctl =elan_iap_ioctl, 
 };




#if IAP_PORTION
static int EnterISPMode(struct i2c_client *client, uint8_t  *isp_cmd)
{
	char buff[4] = {0};
	int len = 0;
	
	len = i2c_master_send(private_ts->client, isp_cmd,  sizeof(isp_cmd));
	if (len != sizeof(buff)) {
		printk("[ELAN_TOPTOUCH] ERROR: EnterISPMode fail! len=%d\r\n", len);
		return -1;
	}
	else
		printk("[ELAN_TOPTOUCH] IAPMode write data successfully! cmd = [%2x, %2x, %2x, %2x]\n", isp_cmd[0], isp_cmd[1], isp_cmd[2], isp_cmd[3]);
	return 0;
}


static int ExtractPage(struct file *filp, uint8_t * szPage, int byte)
{
	int len = 0;

	len = filp->f_op->read(filp, szPage,byte, &filp->f_pos);
	if (len != byte) 
	{
		printk("[ELAN_TOPTOUCH] ExtractPage ERROR: read page error, read error. len=%d\r\n", len);
		return -1;
	}

	return 0;
}


static int WritePage(uint8_t * szPage, int byte)
{
	int len = 0;

	len = i2c_master_send(private_ts->client, szPage,  byte);
	if (len != byte) 
	{
		printk("[ELAN_TOPTOUCH] ERROR: write page error, write error. len=%d\r\n", len);
		return -1;
	}

	return 0;
}


static int GetAckData(struct i2c_client *client)
{
	int len = 0;

	char buff[2] = {0};
	
	len=i2c_master_recv(private_ts->client, buff, sizeof(buff));
	if (len != sizeof(buff)) {
		printk("[ELAN_TOPTOUCH] ERROR: read data error, write 50 times error. len=%d\r\n", len);
		return -1;
	}

//	dbg_info("[ELAN_TOPTOUCH] GetAckData:%x,%x",buff[0],buff[1]);

	if (buff[0] == 0xaa/* && buff[1] == 0xaa*/) 
		return ACK_OK;
	else if (buff[0] == 0x55 && buff[1] == 0x55)
		return ACK_REWRITE;
	else
		return ACK_Fail;

	return 0;
}


static void print_progress(int page, int ic_num, int j)
{
	int i, percent,page_tatol,percent_tatol;
	char str[256];
	str[0] = '\0';
	for (i=0; i<((page)/10); i++) {
		str[i] = '#';
		str[i+1] = '\0';
	}
	
	page_tatol=page+249*(ic_num-j);
	percent = ((100*page)/(249));
	percent_tatol = ((100*page_tatol)/(249*ic_num));

	if ((page) == (249))
		percent = 100;

	if ((page_tatol) == (249*ic_num))
		percent_tatol = 100;		

	printk("\rprogress %s| %d%%", str, percent);
	
	if (page == (249))
		printk("\n");
}


/* 
* Restet and (Send normal_command ?)
* Get Hello Packet
*/
static int IAPReset(struct i2c_client *client)
{
	int res;
	int num = 10;
	char recovery_buffer[4];
	//reset
	gpio_set_value(private_ts->rst_gpio,0);
	msleep(20);
	gpio_set_value(private_ts->rst_gpio,1);
        msleep(400);				

	printk("[ELAN_TOPTOUCH] read Hello packet data!\n"); 	  
	res= __hello_packet_handler(client);
	msleep(40);
	
	if(res == 0x80){
	
		while(num--){
			res = i2c_master_recv(private_ts->client, recovery_buffer, 4);   //55 aa 33 cc 
//			printk("\r[ELAN_TOPTOUCH] recovery byte data:%x,%x,%x,%x \n",recovery_buffer[0],recovery_buffer[1],recovery_buffer[2],recovery_buffer[3]);

			if(recovery_buffer[0]==0x55 && recovery_buffer[1]==0xaa && recovery_buffer[2]==0x33 && recovery_buffer[3]==0xcc)
				break;
		}
		if(num <= 0){
			printk("ELAN iap read response error!\n");
		}
	}
	return res;
}


static int Update_FW_One(struct i2c_client *client, int recovery)
{
	int res = 0,ic_num = 1;
	//rewriteCnt for PAGE_REWRITE
	int iPage = 0, rewriteCnt = 0; 
	int i = 0;
	uint8_t data;
	
	//struct timeval tv1, tv2,For IAP_RESTART
	int restartCnt = 0;
	
	uint8_t recovery_buffer[4] = {0};
	int byte_count;
	uint8_t *szBuff = NULL;
	int curIndex = 0;
	uint8_t isp_cmd[] = {0x45, 0x49, 0x41, 0x50}; //{0x54, 0x00, 0x12, 0x34};

//	dbg_info("\r[ELAN_TOPTOUCH] %s:  ic_num=%d\n", __func__, ic_num);
	
IAP_RESTART:	
	data=I2C_DATA[0];
//	dbg_info("\r[ELAN_TOPTOUCH] %s: address data=0x%x \n", __func__, data);

	if(recovery != 0x80)
	{
        printk("\r[ELAN_TOPTOUCH] Firmware upgrade normal mode !\n");

		gpio_set_value(private_ts->rst_gpio,0);
		msleep(20);
	    	gpio_set_value(private_ts->rst_gpio,1);
		msleep(400);

		res = EnterISPMode(private_ts->client, isp_cmd);	 //enter ISP mode
	} else{
	
    		printk("\r[ELAN_TOPTOUCH] Firmware upgrade recovery mode !\n");
		res = i2c_master_recv(private_ts->client, recovery_buffer, 4);   //55 aa 33 cc 
		printk("\r[ELAN_TOPTOUCH] recovery byte data:%x,%x,%x,%x \n",recovery_buffer[0],recovery_buffer[1],recovery_buffer[2],recovery_buffer[3]);	
	}	

	// Send Dummy Byte	
	printk("\r[ELAN_TOPTOUCH] send one byte data:%x,%x",private_ts->client->addr,data);
	res = i2c_master_send(private_ts->client, &data,  sizeof(data));

	if(res!=sizeof(data))
	{
		printk("[ELAN_TOPTOUCH] dummy error code = %d\n",res);
	}	
	mdelay(10);


	// Start IAP
	for( iPage = 1; iPage <= PageNum; iPage++ ) 
	{
PAGE_REWRITE:
#if 1 
		//szBuff = fw_data + ((iPage-1) * PageSize); 
		for(byte_count=1;byte_count<=17;byte_count++)
		{
			if(byte_count!=17)
			{		
	//			printk("[ELAN_TOPTOUCH] byte %d\n",byte_count);	
	//			printk("curIndex =%d\n",curIndex);
				szBuff = file_fw_data + curIndex;
				curIndex =  curIndex + 8;

				//ioctl(fd, IOCTL_IAP_MODE_LOCK, data);
				res = WritePage(szBuff, 8);
			}
			else
			{
	//			printk("byte %d\n",byte_count);
	//			printk("curIndex =%d\n",curIndex);
	
				szBuff = file_fw_data + curIndex;
				curIndex =  curIndex + 4;
				
				//ioctl(fd, IOCTL_IAP_MODE_LOCK, data);
				res = WritePage(szBuff, 4); 
			}
		} // end of for(byte_count=1;byte_count<=17;byte_count++)
#endif 


#if 0 // 132byte mode		
		szBuff = file_fw_data + curIndex;
		curIndex =  curIndex + PageSize;
		res = WritePage(szBuff, PageSize);
#endif


		if(iPage==249 || iPage==1)
		{
			mdelay(600); 			 
		}
		else
		{
			mdelay(50); 			 
		}	
		res = GetAckData(private_ts->client);
		

		if (ACK_OK != res) 
		{
			mdelay(50); 
			printk("[ELAN_TOPTOUCH] ERROR: GetAckData fail! res=%d\r\n", res);
			if ( res == ACK_REWRITE ) 
			{
				rewriteCnt = rewriteCnt + 1;
				if (rewriteCnt == PAGERETRY)
				{
					printk("[ELAN_TOPTOUCH] ID 0x%02x %dth page ReWrite %d times fails!\n", data, iPage, PAGERETRY);
					return E_FD;
				}
				else
				{
					printk("[ELAN_TOPTOUCH] ---%d--- page ReWrite %d times!\n",  iPage, rewriteCnt);
					goto PAGE_REWRITE;
				}
			}
			else
			{
				restartCnt = restartCnt + 1;
				if (restartCnt >= 5)
				{
					printk("[ELAN_TOPTOUCH] ID 0x%02x ReStart %d times fails!\n", data, IAPRESTART);
					return E_FD;
				}
				else
				{
					printk("[ELAN_TOPTOUCH] ===%d=== page ReStart %d times!\n",  iPage, restartCnt);
					goto IAP_RESTART;
				}
			}
		}
		else
		{       printk("  data : 0x%02x ",  data);  
			rewriteCnt=0;
			print_progress(iPage,ic_num,i);
		}
		

		mdelay(10);
	} // end of for(iPage = 1; iPage <= PageNum; iPage++)
	

	if (IAPReset(client) == 0)
		printk("[ELAN_TOPTOUCH] Update ALL Firmware successfully!\n");
	return 0;
	
}

#endif
// End Firmware Update



// Start sysfs
#ifdef CONFIG_HAS_EARLYSUSPEND
static void elan_toptouch_ts_early_suspend(struct early_suspend *h);
static void elan_toptouch_ts_late_resume(struct early_suspend *h);
#endif

static ssize_t elan_toptouch_gpio_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;
	struct elan_toptouch_ts_data *ts = private_ts;

	ret = gpio_get_value(ts->intr_gpio);
//	printk(KERN_DEBUG "GPIO_TP_INT_N=%d\n", ts->intr_gpio);
	sprintf(buf, "GPIO_TP_INT_N=%d\n", ret);
	ret = strlen(buf) + 1;
	return ret;
}

static DEVICE_ATTR(gpio, S_IRUGO, elan_toptouch_gpio_show, NULL);

static ssize_t elan_toptouch_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct elan_toptouch_ts_data *ts = private_ts;

	sprintf(buf, "%s_x%4.4x\n", "ELAN_TOPTOUCH", ts->fw_ver);
	ret = strlen(buf) + 1;
	return ret;
}

static DEVICE_ATTR(vendor, S_IRUGO, elan_toptouch_vendor_show, NULL);

static struct kobject *android_touch_kobj;

static int elan_toptouch_touch_sysfs_init(void)
{
	int ret ;

	android_touch_kobj = kobject_create_and_add("android_touch", NULL) ;
	if (android_touch_kobj == NULL) {
		printk(KERN_ERR "[ELAN_TOPTOUCH]%s: subsystem_register failed\n", __func__);
		ret = -ENOMEM;
		return ret;
	}
	ret = sysfs_create_file(android_touch_kobj, &dev_attr_gpio.attr);
	if (ret) {
		printk(KERN_ERR "[ELAN_TOPTOUCH]%s: sysfs_create_file failed\n", __func__);
		return ret;
	}
	ret = sysfs_create_file(android_touch_kobj, &dev_attr_vendor.attr);
	if (ret) {
		printk(KERN_ERR "[ELAN_TOPTOUCH]%s: sysfs_create_group failed\n", __func__);
		return ret;
	}
	return 0 ;
}

static void elan_touch_sysfs_deinit(void)
{
	sysfs_remove_file(android_touch_kobj, &dev_attr_vendor.attr);
	sysfs_remove_file(android_touch_kobj, &dev_attr_gpio.attr);
	kobject_del(android_touch_kobj);
}	

// end sysfs

static int __elan_toptouch_ts_poll(struct i2c_client *client)
{
	struct elan_toptouch_ts_data *ts = i2c_get_clientdata(client);
	int status = 0, retry = 10;
	
	do {
		status = gpio_get_value(ts->intr_gpio);
		printk("%s: status = %d\n", __func__, status);
		retry--;
		mdelay(20);
	} while (status == 1 && retry > 0);

//	printk( "[ELAN_TOPTOUCH]%s: poll interrupt status %s\n",
//			__func__, status == 1 ? "high" : "low");
	return (status == 0 ? 0 : -ETIMEDOUT);
}

static int elan_toptouch_ts_poll(struct i2c_client *client)
{
	return __elan_toptouch_ts_poll(client);
}

static int elan_toptouch_ts_get_data(struct i2c_client *client, uint8_t *cmd,
			uint8_t *buf, size_t size)
{
	int rc;

//	dbg_info("[ELAN_TOPTOUCH]%s: enter\n", __func__);

	if (buf == NULL)
		return -EINVAL;

	if ((i2c_master_send(client, cmd, 4)) != 4) {
		printk("[ELAN_TOPTOUCH]%s: i2c_master_send failed\n", __func__);
		return -EINVAL;
	}

	rc = elan_toptouch_ts_poll(client);
	if (rc < 0)
		return -EINVAL;
	else {
		if (i2c_master_recv(client, buf, size) != size ||
		    buf[0] != CMD_S_PKT)
			return -EINVAL;
		printk("buf %X %X %X %X", buf[0], buf[1], buf[2], buf[4]);
	}

	return 0;
}

static int __hello_packet_handler(struct i2c_client *client)
{
	int rc;
	uint8_t buf_recv[8] = { 0 };

	mdelay(1500);

	rc = elan_toptouch_ts_poll(client);
	if (rc < 0) {
		printk( "[ELAN_TOPTOUCH] %s: Int poll failed!\n", __func__);
		RECOVERY=0xff;
		return RECOVERY;
	}

	rc = i2c_master_recv(client, buf_recv, 8);
//	printk("[ELAN_TOPTOUCH] %s: hello packet %2x:%2X:%2x:%2x:%2x:%2X:%2x:%2x\n", __func__, buf_recv[0], buf_recv[1], buf_recv[2], buf_recv[3] , buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7]);

	if(buf_recv[0]==0x55 && buf_recv[1]==0x55 && buf_recv[2]==0x80 && buf_recv[3]==0x80)
	{
		RECOVERY=0x80;
	   	return RECOVERY; 
	}
	RECOVERY = 0;
	return 0;
}

static int elan_ktf3k_ts_rough_calibrate(struct i2c_client *client){
  	uint8_t cmd[] = {CMD_W_PKT, 0x29, 0x00, 0x01};
	printk("[ELAN_TOPTOUCH] %s: enter\n", __func__);
	printk("[ELAN_TOPTOUCH] dump cmd: %02x, %02x, %02x, %02x\n",cmd[0], cmd[1], cmd[2], cmd[3]);

	if ((i2c_master_send(client, cmd, sizeof(cmd))) != sizeof(cmd)) {
		printk("[ELAN_TOPTOUCH] %s: i2c_master_send failed\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int __fw_packet_handler(struct i2c_client *client)
{
	struct elan_toptouch_ts_data *ts = i2c_get_clientdata(client);
	int rc;
	int major, minor;
	uint8_t cmd[] = {CMD_R_PKT, 0x00, 0x00, 0x01};
	uint8_t cmd_x[] = {0x53, 0x60, 0x00, 0x00}; /*Get x resolution*/
	uint8_t cmd_y[] = {0x53, 0x63, 0x00, 0x00}; /*Get y resolution*/
	uint8_t cmd_id[] = {0x53, 0xf0, 0x00, 0x01}; /*Get firmware ID*/
	uint8_t cmd_bc[] = {0x53, 0x10, 0x00, 0x01}; /*Get firmware BC*/
	uint8_t buf_recv[4] = {0};

// Firmware version
	rc = elan_toptouch_ts_get_data(client, cmd, buf_recv, 4);
	if (rc < 0)
		return rc;
	major = ((buf_recv[1] & 0x0f) << 4) | ((buf_recv[2] & 0xf0) >> 4);
	minor = ((buf_recv[2] & 0x0f) << 4) | ((buf_recv[3] & 0xf0) >> 4);
	ts->fw_ver = major << 8 | minor;
	FW_VERSION = ts->fw_ver;

// X Resolution
	rc = elan_toptouch_ts_get_data(client, cmd_x, buf_recv, 4);
	//if (rc < 0)
	//	return rc;
	minor = ((buf_recv[2])) | ((buf_recv[3] & 0xf0) << 4);
	ts->x_resolution =minor;
	Y_RESOLUTION = ts->x_resolution;
// Y Resolution	
	rc = elan_toptouch_ts_get_data(client, cmd_y, buf_recv, 4);
	//if (rc < 0)
	//	return rc;
	minor = ((buf_recv[2])) | ((buf_recv[3] & 0xf0) << 4);
	ts->y_resolution =minor;
	X_RESOLUTION = ts->y_resolution;
// Firmware ID
	rc = elan_toptouch_ts_get_data(client, cmd_id, buf_recv, 4);
	//if (rc < 0)
	//	return rc;
	major = ((buf_recv[1] & 0x0f) << 4) | ((buf_recv[2] & 0xf0) >> 4);
	minor = ((buf_recv[2] & 0x0f) << 4) | ((buf_recv[3] & 0xf0) >> 4);
	ts->fw_id = major << 8 | minor;
	FW_ID = ts->fw_id;

// Firmware BC
	rc = elan_toptouch_ts_get_data(client, cmd_bc, buf_recv, 4);
	//if (rc < 0)
	//	return rc;

	major = ((buf_recv[1] & 0x0f) << 4) | ((buf_recv[2] & 0xf0) >> 4);
	minor = ((buf_recv[2] & 0x0f) << 4) | ((buf_recv[3] & 0xf0) >> 4);

	ts->fw_bc = major << 8 | minor;
	FW_BC = ts->fw_bc;
	
	rc=elan_ktf3k_ts_rough_calibrate(client);
/*
	printk( "[ELAN_TOPTOUCH] %s: firmware version: 0x%4.4x\n",
			__func__, ts->fw_ver);
	printk( "[ELAN_TOPTOUCH] %s: firmware ID: 0x%4.4x\n",
			__func__, ts->fw_id);
	printk( "[ELAN_TOPTOUCH] %s: firmware BC: 0x%4.4x\n",
			__func__, ts->fw_bc);

	printk( "[ELAN_TOPTOUCH] %s: x resolution: %d, y resolution: %d\n",
			__func__, ts->x_resolution, ts->y_resolution);
*/	
	return 0;
	
}

static inline int elan_toptouch_ts_parse_xy(uint8_t *data,
			uint16_t *x, uint16_t *y)
{
	*x = *y = 0;

	*x = (data[0] & 0xf0);
	*x <<= 4;
	*x |= data[1];

	*y = (data[0] & 0x0f);
	*y <<= 8;
	*y |= data[2];

	return 0;
}

static int elan_toptouch_ts_setup(struct i2c_client *client)
{
	int rc;
    
	rc = __hello_packet_handler(client);
//	printk("[ELAN_TOPTOUCH] hellopacket's rc = %d\n",rc);

	mdelay(10);
	//if (rc != 0x80){
	if(1){
	    rc = __fw_packet_handler(client);
	    if (rc < 0)
		 printk("[ELAN_TOPTOUCH] %s, fw_packet_handler fail, rc = %d", __func__, rc);
//	         dbg_info("[ELAN_TOPTOUCH] %s: firmware checking done.\n", __func__);

//Check for FW_VERSION, if 0x0000 means FW update fail!
	    if ( FW_VERSION == 0x00)
	    {
			rc = -EINVAL;
			printk("[ELAN_TOPTOUCH] FW_VERSION = %d, last FW update fail\n", FW_VERSION);
	    }
      }
	return rc;
}

static int elan_toptouch_ts_rough_calibrate(struct i2c_client *client){
      uint8_t cmd[] = {CMD_W_PKT, 0x29, 0x00, 0x01};

	//dev_info(&client->dev, "[ELAN_TOPTOUCH] %s: enter\n", __func__);
	printk("[ELAN_TOPTOUCH] %s: enter\n", __func__);
/*	dbg_info("[ELAN_TOPTOUCH] dump cmd: %02x, %02x, %02x, %02x\n",
		cmd[0], cmd[1], cmd[2], cmd[3]);
*/
	if ((i2c_master_send(client, cmd, sizeof(cmd))) != sizeof(cmd)) {
		printk("[ELAN_TOPTOUCH] %s: i2c_master_send failed\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int elan_toptouch_ts_set_power_state(struct i2c_client *client, int state)
{
	uint8_t cmd[] = {CMD_W_PKT, 0x50, 0x00, 0x01};

	printk("[ELAN_TOPTOUCH] %s: enter\n", __func__);

	cmd[1] |= (state << 3);

/*	dbg_info("[ELAN_TOPTOUCH] dump cmd: %02x, %02x, %02x, %02x\n",
		cmd[0], cmd[1], cmd[2], cmd[3]);
*/
	if ((i2c_master_send(client, cmd, sizeof(cmd))) != sizeof(cmd)) {
		printk("[ELAN_TOPTOUCH] %s: i2c_master_send failed\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int elan_toptouch_ts_get_power_state(struct i2c_client *client)
{
	int rc = 0;
	uint8_t cmd[] = {CMD_R_PKT, 0x50, 0x00, 0x01};
	uint8_t buf[4], power_state;

	rc = elan_toptouch_ts_get_data(client, cmd, buf, 4);
	if (rc)
		return rc;

	power_state = buf[1];
//	dbg_info("[ELAN_TOPTOUCH] dump repsponse: %0x\n", power_state);
	power_state = (power_state & PWR_STATE_MASK) >> 3;
/*	dbg_info("[ELAN_TOPTOUCH] power state = %s\n",
		power_state == PWR_STATE_DEEP_SLEEP ?
		"Deep Sleep" : "Normal/Idle");
*/
	return power_state;
}

static int elan_toptouch_ts_recv_data(struct i2c_client *client, uint8_t *buf)
{

	int rc, bytes_to_recv=PACKET_SIZE;
	int i;
	if (buf == NULL)
		return -EINVAL;

	memset(buf, 0, bytes_to_recv);
	
	rc = i2c_master_recv(client, buf, PACKET_SIZE);
	if (rc != PACKET_SIZE)
		 printk("[ELAN_TOPTOUCH] The first package error.\n");
	//for(i=0;i<PACKET_SIZE;i++)
	//	printk("0x%x ",buf[i]);
	//printk("\n");
	
	return rc;
}
#if 0
static void elan_toptouch_ts_report_data(struct i2c_client *client, uint8_t *buf)
{
	struct elan_toptouch_ts_data *ts = i2c_get_clientdata(client);
	struct input_dev *idev = ts->input_dev;
	uint16_t x, y;
	uint16_t fbits=0;
	uint16_t fbits_tmp=0;
	uint8_t i, num, reported = 0;
	uint8_t idx;
	int finger_num;
	uint8_t  point_count = 0;
	uint8_t vbutton  = 0;
	unsigned int position = 0;	
   // struct point_status points[5];
	static u8 points_last_flag[5] = {0};
   	uint8_t  track_id[MAX_FINGER_NUM];
	u8 current_events[MAX_FINGER_NUM] = {0};
	 struct point_status points[5];
// for 5 fingers	
   	if ((buf[0] == NORMAL_PKT) || (buf[0] == FIVE_FINGERS_PKT)){
	    	finger_num = 5;
	    	num    = buf[1] & 0x07; 
            fbits = buf[1] >>3;
			vbutton = buf[17] & 0xf0;
	    	idx=2;
      }else{
// for 2 fingers      
      	    finger_num = 2;
	    	num = buf[7] & 0x03; 
            fbits = buf[7] & 0x03;
		    vbutton = buf[7] & 0x0c;
	    	idx =1;
	   }
		fbits_tmp=fbits;
		//printk("fbits is %d num is %d\n",fbits,num);
    switch (buf[0]) {
    	case NORMAL_PKT:
    	case TWO_FINGERS_PKT:
    	case FIVE_FINGERS_PKT:	
	case TEN_FINGERS_PKT:			
			
		for(position=0; (position<finger_num)&&(fbits_tmp);position++)
        {      	
         
          if(fbits_tmp&0x01)
          {
            track_id[point_count++] = position;
			points[position].id = position; 
			elan_toptouch_ts_parse_xy(&buf[idx], &y, &x); 

     		//printk("[elan_debug] %s, x=%d, y=%d\n",__func__, x , y);			
    		x = x*800/960;	 
    		y = y*480/640;	
			points[position].x=x;
			points[position].y=y;
            current_events[position] = 1;
            //printk("***********x=%d, y=%d\n",x , y);
            fbits_tmp >>= 1;
		  	idx += 3;	
          }       
        
        }  
    //printk("point_count is %d  finger_num is %d\n",point_count,finger_num);		
	if(point_count==0)
	{
		//printk("*******no touch \n");
              for(i = 0; i < finger_num; i++)
              {
                    if (points_last_flag[i] != 0)
                    {		                     
				          input_mt_slot(ts->input_dev, i);
		                  input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, -1);
				          //input_mt_report_slot_state(ts->input_dev, MT_TOOL_FINGER, false);
		                          //input_sync(ts->input_dev);
                    }
              }
              memset(points_last_flag, 0, sizeof(points_last_flag));      
				
	} 
   else {
	    for(i = 0; i < MAX_FINGER_NUM; i++)
		{
		  if((current_events[i] == 0) && (points_last_flag[i] != 0))
		  {
      		  //printk("Point UP event.id=%d\n",i);
			  input_mt_slot(ts->input_dev, i);
              input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, -1);
			  //input_mt_report_slot_state(ts->input_dev, MT_TOOL_FINGER, false);
                          
		  }
		  else if(current_events[i])
		  {
						
    		  //printk("Point DN event.id=%d\n",i);
			  input_mt_slot(ts->input_dev, i);
              input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, points[i].id);
			  //input_mt_report_slot_state(ts->input_dev, MT_TOOL_FINGER, true);
              input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 1);
		      input_report_abs(ts->input_dev, ABS_MT_POSITION_X, points[i].x);
		      input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, points[i].y);
			  //printk("***********Point DN event.id=%d,x=%d, y=%d\n",i,points[i].x , points[i].y);
			 // input_report_abs(ts->input_dev, ABS_MT_POSITION_X,  current_events[i].x);
			 // input_report_abs(ts->input_dev, ABS_MT_POSITION_Y,  current_events[i].y);
                          //input_sync(ts->input_dev);
                          
		  } // end if finger status
		   points_last_flag[i] = current_events[i];
		
	     }
   	   }
		input_sync(ts->input_dev);
		case HELLO_PKT:
				//printk("hello packet ok!\n");
				break;
	   	default:
				dbg_info("[ELAN_TOPTOUCH] %s: unknown packet type: %0x\n", __func__, buf[0]);
				break;
		} // end switch

	return;
}
#else
static void elan_toptouch_ts_report_data(struct i2c_client *client, uint8_t *buf)
{
	struct input_dev *idev = private_ts->input_dev;
	uint16_t x, y;
	uint16_t fbits=0, checksum=0;
	uint8_t i, num, reported = 0;
	uint8_t idx;
	
	for (i=0; i<34;i++)
		checksum +=buf[i];

	if ( (checksum & 0x00ff) == buf[34]) {   
		switch (buf[0]) {
		
		case TEN_FINGERS_PKT:
			fbits = buf[2] & 0x30;	
			fbits = (fbits << 4) | buf[1];  
			num = buf[2] &0x0f;
			
			if (num == 0) {
				
//				printk( "no press\n");
				input_report_key(idev, BTN_TOUCH, 0);
				input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 0);
				
			} else {
			
				input_report_key(idev, BTN_TOUCH, 1);
				
//				printk( "[ELAN_TOPTOUCH] %d fingers\n", num);
				idx=3;
				
				for (i = 0; i < 10; i++) {
					if ((fbits & 0x1)) {
						elan_toptouch_ts_parse_xy(&buf[idx], &x, &y);  
						//printk("brain x=%d,y=%d\n",x,y);
						
						if (!((x<=0) || (y<=0) || (x>=X_RESOLUTION) || (y>=Y_RESOLUTION))) {   
							input_report_abs(idev, ABS_MT_TRACKING_ID, i);
							input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 8);
							input_report_abs(idev, ABS_MT_POSITION_X, x);
							input_report_abs(idev, ABS_MT_POSITION_Y, y);
							
							input_mt_sync(idev);
							reported++;
						} // end if border
					} // end if finger status

					fbits = fbits >> 1;
					idx += 3;
				} // end for
			}

			if (reported)
				input_sync(idev);
			else {
				input_mt_sync(idev);
				input_sync(idev);
			}

			break;
		default:
			printk("[ELAN_TOPTOUCH] %s: unknown packet type: %0x\n", __func__, buf[0]);
			break;
		}

	} 
	else {
		checksum_err +=1;
		printk("[ELAN_TOPTOUCH] Checksum Error %d\n", checksum_err);
	} 

	return;
}


#endif

#if 0
static void elan_toptouch_ts_work_func(struct work_struct *work)
{
	int rc;
	struct elan_toptouch_ts_data *ts =
	container_of(work, struct elan_toptouch_ts_data, work);
	uint8_t buf[PACKET_SIZE] = { 0 };

		/*if (gpio_get_value(ts->intr_gpio))
		{
			enable_irq(ts->client->irq);
			return;
		}*/
	
		rc = elan_toptouch_ts_recv_data(ts->client, buf);
 
		if (rc < 0)
		{
			enable_irq(ts->client->irq);
			return;
		}

		//printk("[elan_debug] %2x,%2x,%2x,%2x,%2x,%2x\n",buf[0],buf[1],buf[2],buf[3],buf[5],buf[6]);
		elan_toptouch_ts_report_data(ts->client, buf);

		enable_irq(ts->client->irq);

	return;
}
#endif

static irqreturn_t elan_toptouch_ts_irq_handler(int irq, void *dev_id)
{
	//struct elan_toptouch_ts_data *ts = dev_id;
	//struct i2c_client *client = ts->client;
//	printk("*******enter ktf2k irq\n");
//	dbg_info(&client->dev, "[ELAN_TOPTOUCH] %s\n", __func__);
	//disable_irq_nosync(ts->client->irq);
	tpd_flag = 1;
	wake_up_interruptible(&waiter);
	//queue_work(ts->elan_wq, &ts->work);

	return IRQ_HANDLED;
}

static int elan_toptouch_ts_register_interrupt(struct i2c_client *client)
{
	struct elan_toptouch_ts_data *ts = i2c_get_clientdata(client);
	int err = 0;

	err = request_irq(client->irq, elan_toptouch_ts_irq_handler,
											IRQF_TRIGGER_FALLING, client->name, ts);
	if (err)
		dev_err(&client->dev, "[ELAN_TOPTOUCH] %s: request_irq %d failed\n",
				__func__, client->irq);
	//disable_irq_nosync(client->irq);
	return err;
}

// james: ts_work_fun
static int touch_event_handler(void *unused)
{
    
	struct sched_param param = { .sched_priority = 4 };
	uint8_t buf[PACKET_SIZE];
	int rc = 0;
	sched_setscheduler(current, SCHED_RR, &param);
	
	do
	{
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(waiter, tpd_flag != 0);
		tpd_flag = 0;
		set_current_state(TASK_RUNNING);
		
		rc = elan_toptouch_ts_recv_data(private_ts->client, buf);
		if (rc < 0)
		{
			continue;
		}
		
		elan_toptouch_ts_report_data(private_ts->client, buf);

	}while(!kthread_should_stop());

    return 0;
}


static int elan_toptouch_ts_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int err = 0;
	int fw_err = 0;
	struct elan_ktf2k_i2c_platform_data *pdata;
	struct elan_toptouch_ts_data *ts;
#if IAP_PORTION    
	int New_FW_ID;	
	int New_FW_VER;
#endif
	long retval;

	printk("%s: enter, g_tp_status = %d\n", __func__, g_tp_status);

	dbg_info("*************enter toptouch probe\n");
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk(KERN_ERR "[ELAN_TOPTOUCH] %s: i2c check functionality error\n", __func__);
		err = -ENODEV;
		goto err_check_functionality_failed;
	}

	ts = kzalloc(sizeof(struct elan_toptouch_ts_data), GFP_KERNEL);
	if (ts == NULL) {
		printk(KERN_ERR "[ELAN_TOPTOUCH] %s: allocate elan_toptouch_ts_data failed\n", __func__);
		err = -ENOMEM;
		goto err_alloc_data_failed;
	}

	/*ts->elan_wq = create_singlethread_workqueue("elan_wq");
	if (!ts->elan_wq) {
		printk(KERN_ERR "[ELAN_TOPTOUCH] %s: create workqueue failed\n", __func__);
		err = -ENOMEM;
		goto err_create_wq_failed;
	}*/

	//INIT_WORK(&ts->work, elan_toptouch_ts_work_func);
	

	ts->client = client; 
	ts->intr_gpio = client->irq;
	
	i2c_set_clientdata(client, ts);

	pdata = client->dev.platform_data;
	ts->rst_gpio = pdata->rst_gpio;
	#if 0
	if (likely(pdata != NULL)) {
		ts->intr_gpio = pdata->intr_gpio;
	}
	
	ts->intr_gpio = S3C64XX_GPF(0); 
	client->irq   = IRQ_EINT_GROUP(4,0);
	s3c_gpio_cfgpin(ts->intr_gpio, S3C_GPIO_SFN(3));
	s3c_gpio_setpull(ts->intr_gpio, S3C_GPIO_PULL_UP);
	
	ts->rst_gpio  = S3C64XX_GPF(10);
	s3c_gpio_cfgpin(ts->rst_gpio, S3C_GPIO_SFN(1));
	s3c_gpio_setpull(ts->rst_gpio, S3C_GPIO_PULL_UP);
        
        gpio_set_value(ts->rst_gpio,0);
	int value = gpio_get_value(ts->rst_gpio);
	printk("\rreset pin value %d\n", value);
        mdelay(20);
	gpio_set_value(ts->rst_gpio,1);
	value = gpio_get_value(ts->rst_gpio);
	printk("\rreset pin value %d\n", value);
        mdelay(300);

	printk("ts->intr_gpio:%d\n", ts->intr_gpio);
	printk("S3C64XX_GPF(0):%d\n", S3C64XX_GPF(0));
	#else
	if(pdata->init_platform_hw)
         pdata->init_platform_hw();
	#endif
	
	fw_err = elan_toptouch_ts_setup(client);
	if (fw_err < 0) {
		printk(KERN_INFO "No Elan chip inside\n");
        //fw_err = -ENODEV;  
		return fw_err;
	}
	
	ts->input_dev = input_allocate_device();
	if (ts->input_dev == NULL) {
		err = -ENOMEM;
		dev_err(&client->dev, "[ELAN_TOPTOUCH] Failed to allocate input device\n");
		goto err_input_dev_alloc_failed;
	}
	ts->input_dev->name = "elan-touchscreen";   // for andorid2.2 Froyo  
	__set_bit(INPUT_PROP_DIRECT, ts->input_dev->propbit);
	//set_bit(EV_KEY, ts->input_dev->evbit);
	//set_bit(BTN_TOUCH, ts->input_dev->keybit);
	set_bit(EV_ABS, ts->input_dev->evbit);
	set_bit(EV_KEY, ts->input_dev->evbit);
	set_bit(EV_SYN, ts->input_dev->evbit);
	#if 0
	__set_bit(INPUT_PROP_DIRECT, ts->input_dev->propbit);
	//__set_bit(EV_ABS, ts->input_dev->evbit);
	set_bit(EV_ABS, ts->input_dev->evbit);
	set_bit(EV_SYN, ts->input_dev->evbit);
	set_bit(ABS_MT_TOUCH_MAJOR, ts->input_dev->absbit);
	set_bit(ABS_MT_POSITION_X, ts->input_dev->absbit);
	set_bit(ABS_MT_POSITION_Y, ts->input_dev->absbit);
	set_bit(ABS_MT_WIDTH_MAJOR, ts->input_dev->absbit);	
	
    input_mt_init_slots(ts->input_dev, MAX_FINGER_NUM);
	input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, 800, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, 480, 0, 0);
	#else
	__set_bit(ABS_X, ts->input_dev->absbit);
	__set_bit(ABS_Y, ts->input_dev->absbit);
	__set_bit(ABS_MT_TOUCH_MAJOR, ts->input_dev->absbit);
	__set_bit(ABS_MT_WIDTH_MAJOR, ts->input_dev->absbit);
	__set_bit(ABS_MT_POSITION_X, ts->input_dev->absbit);
	__set_bit(ABS_MT_POSITION_Y, ts->input_dev->absbit);
	__set_bit(ABS_MT_TOOL_TYPE, ts->input_dev->absbit);
	__set_bit(ABS_MT_BLOB_ID, ts->input_dev->absbit);
	__set_bit(ABS_MT_TRACKING_ID, ts->input_dev->absbit);
	__set_bit(BTN_TOUCH, ts->input_dev->keybit);

	input_set_abs_params(ts->input_dev, ABS_X, 0, X_RESOLUTION, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_Y, 0, Y_RESOLUTION, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0,  255, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0,  255, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, X_RESOLUTION, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, Y_RESOLUTION, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_MT_TRACKING_ID, 0, 5, 0, 0);
	input_set_abs_params(ts->input_dev, ABS_PRESSURE, 0, 255, 0, 0);
	#endif

   // check input_register_devices & input_set_abs_params sequence
	err = input_register_device(ts->input_dev);
	if (err) {
		    printk("[ELAN_TOPTOUCH]%s: unable to register %s input device\n",
			__func__, ts->input_dev->name);
		    goto err_input_register_device_failed;
	}

	
	if(!ts->intr_gpio)
	{
		dev_dbg(&ts->client->dev, "no IRQ?\n");
		return -ENODEV;
	}
	else
	{
		ts->intr_gpio = gpio_to_irq(ts->intr_gpio);
	}
	/*err = request_irq(ts->irq, ft5x0x_ts_interrupt, IRQF_TRIGGER_FALLING, "ft5x0x_ts", ft5x0x_ts);
	if (err < 0) {
	dev_err(&client->dev, "ft5x0x_probe: request irq failed\n");
	goto exit_irq_request_failed;
	}*/
	elan_toptouch_ts_register_interrupt(ts->client);

	//disable_irq_nosync(ts->irq);
/*
	if (gpio_get_value(ts->intr_gpio) == 0) {
		printk(KERN_INFO "[ELAN_TOPTOUCH]%s: handle missed interrupt\n", __func__);
		elan_toptouch_ts_irq_handler(client->irq, ts);
	}*/
	
// check earlysuspend
#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_STOP_DRAWING - 1;
	ts->early_suspend.suspend = elan_toptouch_ts_early_suspend;
	ts->early_suspend.resume = elan_toptouch_ts_late_resume;
	register_early_suspend(&ts->early_suspend);
#endif

	private_ts = ts;

	elan_toptouch_touch_sysfs_init();
//	dbg_info("[ELAN_TOPTOUCH] Start touchscreen %s in interrupt mode\n",ts->input_dev->name);

// Firmware Update
	ts->firmware.minor = MISC_DYNAMIC_MINOR;
	ts->firmware.name = "elan-iap";
	ts->firmware.fops = &elan_touch_fops;
	//ts->firmware.mode = S_IFREG|S_IRWXUGO; 

	if (misc_register(&ts->firmware) < 0)
  		printk("[ELAN_TOPTOUCH]misc_register failed!!");
  	else
		printk("[ELAN_TOPTOUCH]misc_register finished!!\n");
// End Firmware Update	


#if IAP_PORTION

	printk("[ELAN_TOPTOUCH] **************IAP_PORTION***************!!\n");
	work_lock=1;
	disable_irq(ts->client->irq);
	//cancel_work_sync(&ts->work);
	power_lock = 1;
	
	/* FW VER & ID*/
	printk("[7D64]=0x%02x,[7D65]=0x%02x,[7D66]=0x%02x,[7D67]=0x%02x\n",file_fw_data[0x7D64],file_fw_data[0x7D65],file_fw_data[0x7D66],file_fw_data[0x7D67]);
	New_FW_ID  = file_fw_data[0x7D67]<<8  | file_fw_data[0x7D66] ;	       
	New_FW_VER = file_fw_data[0x7D65]<<8  | file_fw_data[0x7D64] ;

	printk("FW_ID=0x%x, New_FW_ID=0x%x \n",FW_ID, New_FW_ID);   	       
	printk("FW_VERSION=0x%x,New_FW_VER=0x%x \n",FW_VERSION,New_FW_VER);  

/* for firmware auto-upgrade */  
         
	if (New_FW_ID == FW_ID) {	  
		if ((New_FW_VER &0x00ff) >(FW_VERSION &0x00ff)) 
		    Update_FW_One(client, RECOVERY);
	} else {                        
		    printk("FW_ID is different!");		
	}
		
        	
	if (FW_ID == 0) {
		    RECOVERY=0x80;	
		    Update_FW_One(client, RECOVERY);
	}

	power_lock = 0;
	work_lock=0;
        enable_irq(ts->client->irq);

#endif	
	printk("[ELAN_TOPTOUCH]++++++end porbe++++++++++++++=!");
	thread = kthread_run(touch_event_handler, 0, "elan_wq");
	if(IS_ERR(thread))
	{
	    retval = PTR_ERR(thread);
	    printk(" failed to create kernel thread: %ld\n", retval);
	}

	g_tp_status = TP_ELAN_TOPTOUCH_OK;

	return 0;

err_input_register_device_failed:
	if (ts->input_dev)
		input_free_device(ts->input_dev);

err_input_dev_alloc_failed: 
	if (ts->elan_wq)
		destroy_workqueue(ts->elan_wq);

//err_create_wq_failed:
	kfree(ts);

err_alloc_data_failed:
err_check_functionality_failed:

	g_tp_status = TP_ELAN_TOPTOUCH_FAIL;

	return err;
}

static int elan_toptouch_ts_remove(struct i2c_client *client)
{
	struct elan_toptouch_ts_data *ts = i2c_get_clientdata(client);

	elan_touch_sysfs_deinit();

	unregister_early_suspend(&ts->early_suspend);
	free_irq(client->irq, ts);

	if (ts->elan_wq)
		destroy_workqueue(ts->elan_wq);
	input_unregister_device(ts->input_dev);
	kfree(ts);

	return 0;
}

static int elan_toptouch_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{	
	int rc = 0;
	if(power_lock==0) /* The power_lock can be removed when firmware upgrade procedure will not be enter into suspend mode.  */
	{
		printk(KERN_INFO "[ELAN_TOPTOUCH] %s: enter\n", __func__);

		disable_irq(client->irq);

		rc = elan_toptouch_ts_set_power_state(client, PWR_STATE_DEEP_SLEEP);
	}
	return 0;
}

static int elan_toptouch_ts_resume(struct i2c_client *client)
{

	int rc = 0, retry = 5;
	struct elan_ktf2k_i2c_platform_data *pdata;
	pdata = client->dev.platform_data;
	if(power_lock==0)   /* The power_lock can be removed when firmware upgrade procedure will not be enter into suspend mode.  */
	{
		printk(KERN_INFO "[ELAN_TOPTOUCH] %s: enter\n", __func__);

		do {
			rc = elan_toptouch_ts_set_power_state(client, PWR_STATE_NORMAL);
			rc = elan_toptouch_ts_get_power_state(client);
			if (rc != PWR_STATE_NORMAL)
				printk(KERN_ERR "[ELAN_TOPTOUCH] %s: wake up tp failed! err = %d\n",
					__func__, rc);
			else
				break;
		} while (--retry);

		if(retry <= 0){
			printk("can not resum %s , reset now\n", __func__);
			if(pdata->init_platform_hw)
         		pdata->init_platform_hw();
		}
		
		enable_irq(client->irq);
	}
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void elan_toptouch_ts_early_suspend(struct early_suspend *h)
{
	struct elan_toptouch_ts_data *ts;
	ts = container_of(h, struct elan_toptouch_ts_data, early_suspend);
	elan_toptouch_ts_suspend(ts->client, PMSG_SUSPEND);
}

static void elan_toptouch_ts_late_resume(struct early_suspend *h)
{
	struct elan_toptouch_ts_data *ts;
	ts = container_of(h, struct elan_toptouch_ts_data, early_suspend);
	elan_toptouch_ts_resume(ts->client);
}
#endif

static const struct i2c_device_id elan_toptouch_ts_id[] = {
	{ ELAN_TOPTOUCH_NAME, 0 },
	{ }
};

static struct i2c_driver elan_toptouch_ts_driver = {
	.probe		= elan_toptouch_ts_probe,
	.remove		= elan_toptouch_ts_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend	= elan_toptouch_ts_suspend,
	.resume		= elan_toptouch_ts_resume,
#endif
	.id_table	= elan_toptouch_ts_id,
	.driver		= {
		.name = ELAN_TOPTOUCH_NAME,
	},
};

static int __devinit elan_toptouch_ts_init(void)
{
	printk(KERN_INFO "[ELAN_TOPTOUCH] %s driver version 0x0003: Integrated 2 and 5 fingers together and auto-mapping resolution\n", __func__);
	return i2c_add_driver(&elan_toptouch_ts_driver);
}

static void __exit elan_toptouch_ts_exit(void)
{
	i2c_del_driver(&elan_toptouch_ts_driver);
	return;
}

#if defined(CONFIG_TOUCHSCREEN_FT_ELAN_TS_SEL)
int elan_toptouch_ts_entry(void)
{
    return i2c_add_driver(&elan_toptouch_ts_driver);
}
EXPORT_SYMBOL_GPL(elan_toptouch_ts_entry);

void elan_toptouch_ts_quit(void)
{
    i2c_del_driver(&elan_toptouch_ts_driver);
}
EXPORT_SYMBOL_GPL(elan_toptouch_ts_quit);
#else
module_init(elan_toptouch_ts_init);
module_exit(elan_toptouch_ts_exit);
#endif

MODULE_DESCRIPTION("ELAN toptouch Touchscreen Driver");
MODULE_LICENSE("GPL");


