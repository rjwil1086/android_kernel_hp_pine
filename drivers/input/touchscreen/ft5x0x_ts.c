/* 
 * drivers/input/touchscreen/ft5x0x_ts.c
 *
 * FocalTech ft5x0x TouchScreen driver. 
 *
 * Copyright (c) 2010  Focal tech Ltd.
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
 *
 *	note: only support mulititouch	Wenfs 2010-10-01
 */
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/async.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <mach/gpio.h>
#include <linux/irq.h>
#include <mach/board.h>
#include <linux/input/mt.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#include "ft_elan_tp_hook.h"

#define CONFIG_FT5X0X_MULTITOUCH 0

/* cwz: 1: for fire auto update. */
#define FT5X0X_FIRMWARE_AUTO_UPDATE	1

#if (FT5X0X_FIRMWARE_AUTO_UPDATE != 0)
#define FT5X0X_REG_THRES          0x80         /* Thresshold, the threshold be low, the sensitivy will be high */
#define FT5X0X_REG_REPORT_RATE    0x88         /* **************report rate, in unit of 10Hz **************/
#define FT5X0X_REG_PMODE          0xA5         /* Power Consume Mode 0 -- active, 1 -- monitor, 3 -- sleep */    
#define FT5X0X_REG_FIRMID         0xA6         /* ***************firmware version **********************/
#define FT5X0X_REG_NOISE_MODE     0xb2         /* to enable or disable power noise, 1 -- enable, 0 -- disable */
#endif

/*
 * Added by yick @RockChip
 * Compatible with both types of firmware
 * default: point - only pressdown finger num
 * 			event - both down and up event
 */

#define SCREEN_MAX_X    1024
#define SCREEN_MAX_Y    600
#define PRESS_MAX       255

#define FT5X0X_NAME	"ft5x0x_ts" 
#define MAX_CONTACTS 	5

#define FT_TP_USE_POINT 1

#define FT5X0X_REG_PMODE	0xA5
#define PMODE_ACTIVE        0x00
#define PMODE_MONITOR       0x01
#define PMODE_STANDBY       0x02
#define PMODE_HIBERNATE     0x03

#ifndef ABS_MT_TOUCH_MAJOR
#define ABS_MT_TOUCH_MAJOR	0x30	/* touching ellipse */
#define ABS_MT_TOUCH_MINOR	0x31	/* (omit if circular) */
#define ABS_MT_WIDTH_MAJOR	0x32	/* approaching ellipse */
#define ABS_MT_WIDTH_MINOR	0x33	/* (omit if circular) */
#define ABS_MT_ORIENTATION	0x34	/* Ellipse orientation */
#define ABS_MT_POSITION_X	  0x35	/* Center X ellipse position */
#define ABS_MT_POSITION_Y	  0x36	/* Center Y ellipse position */
#define ABS_MT_TOOL_TYPE	  0x37	/* Type of touching device */
#define ABS_MT_BLOB_ID		  0x38	/* Group set of pkts as blob */
#endif /* ABS_MT_TOUCH_MAJOR */

#define FT_TP_RESET_PIN 			RK30_PIN4_PD0
#define FT_TP_RESET_DELAY_IIME 		(20) /* ft_acgzx: reseting IO delay time is 20 millisecond defaultly. */

/* ft_acgzx */
#define FT_TP_I2C_RATE_100K		(100*1000)
#define FT_TP_I2C_RATE_150K		(150*1000)
#define FT_TP_I2C_RATE_200K		(200*1000)
#define FT_TP_I2C_RATE_250K		(250*1000)
#define FT_TP_I2C_RATE_300K		(300*1000)
#define FT_TP_I2C_RATE_350K		(350*1000)
#define FT_TP_I2C_RATE_400K		(400*1000)

#define FT_TP_I2C_RATE			FT_TP_I2C_RATE_300K

#define FT_TP_AXIS_TRANS	(0)
#define FT_TP_AXIS_X_TRANS	(0)

/*register address*/
#define FT5x0x_REG_FW_VER       0xA6
#define FT5x0x_REG_POINT_RATE   0x88
#define FT5X0X_REG_THGROUP  0x80

#define FT_MAX_ID               0x0F
#define FT_TOUCH_STEP           6
#define FT_TOUCH_X_H_POS        3
#define FT_TOUCH_X_L_POS        4
#define FT_TOUCH_Y_H_POS        5
#define FT_TOUCH_Y_L_POS        6
#define FT_TOUCH_EVENT_POS      3
#define FT_TOUCH_ID_POS         5

/* add for tp calibrate */
#define FTS_FACTORYMODE_VALUE   0x40
#define FTS_WORKMODE_VALUE      0x00

/* add for tp upgrade mode select */
#define FT_UPGRADE_DIRECTLY     (0)
#define FT_UPGRADE_WORKQUEUE    (1)
#define FT_UPGRADE_MODE         FT_UPGRADE_DIRECTLY

//#define FT_TP_SCAN_RATE_TEST	/* ft_acgzx: test ft tp scan rate */

#ifdef FT_TP_SCAN_RATE_TEST
static struct timeval tv_pre, tv_cur;
#endif

#ifdef FT_TP_SCAN_RATE_TEST
#define ft5x0x_debug_rate(...) \
	do { \
		printk(KERN_DEBUG __VA_ARGS__); \
	} while (0) 
#else
#define ft5x0x_debug_rate(...)
#endif

#if 0
#define ft5x0x_debug(level, ...) \
	do { \
		printk(KERN_DEBUG __VA_ARGS__); \
	} while (0) 
#else
#define ft5x0x_debug(level, ...)
#endif

#if 0
#define ft5x0x_debug_2(...) \
    do { \
        printk(KERN_DEBUG __VA_ARGS__); \
    } while (0) 
#else
#define ft5x0x_debug_2(...)
#endif

#if 0
#define ft5x0x_debug_report( ...) \
	do { \
		printk(KERN_DEBUG __VA_ARGS__); \
	} while (0) 
#else
#define ft5x0x_debug_report( ...)
#endif

typedef int ft_finger_status_t;

#define FT_FINGER_RELEASE  0
#define FT_FINGER_PRESS    1

#define FT_INVALID_ID (255)

static ft_finger_status_t g_cur_finger_status[MAX_CONTACTS][2] = {
    {FT_FINGER_RELEASE, FT_INVALID_ID},
    {FT_FINGER_RELEASE, FT_INVALID_ID},
    {FT_FINGER_RELEASE, FT_INVALID_ID},
    {FT_FINGER_RELEASE, FT_INVALID_ID},
    {FT_FINGER_RELEASE, FT_INVALID_ID}
};

typedef enum {
    FT_TP_SUSPEND = 0,
    FT_TP_RESUME
} ft_tp_work_t;

extern tp_status_t g_tp_status;

struct point_data {
    u8 status;
    u8 event_flag;
    u8 id;
	u16 x;
	u16 y;
};

struct ts_event {
  u16  touch_point;
  struct point_data point[5];
};

struct ft5x0x_ts_dev {
  struct i2c_client *client;	
	struct input_dev	*input_dev;
	int    irq;
	struct ts_event		event;
	struct work_struct 	pen_event_work;
	struct workqueue_struct *ts_workqueue;

#if (FT_UPGRADE_MODE == FT_UPGRADE_WORKQUEUE)
    struct work_struct  upgrade_work;
    struct workqueue_struct *upgrade_workqueue;
#endif

	struct early_suspend	early_suspend;
    int ft_thgroup;
};

static struct ft5x0x_ts_dev *g_dev;

static int ft5x0x_i2c_rxdata(char *rxdata, int length)
{
	int ret;

	struct i2c_msg msgs[] = {
		{
			.addr	= g_dev->client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= rxdata,
			.scl_rate = FT_TP_I2C_RATE,
		},
		{
			.addr	= g_dev->client->addr,
			.flags	= I2C_M_RD,
			.len	= length,
			.buf	= rxdata,
			.scl_rate = FT_TP_I2C_RATE,
		},
	};

	ret = i2c_transfer(g_dev->client->adapter, msgs, 2);
	if (ret < 0)
		pr_err("msg %s i2c read error: %d\n", __func__, ret);
	
	return ret;
}

static int ft5x0x_i2c_txdata(char *txdata, int length)
{
	int ret;

	struct i2c_msg msg[] = {
		{
			.addr	= g_dev->client->addr,
			.flags	= 0,
			.len	= length,
			.buf	= txdata,
			.scl_rate = FT_TP_I2C_RATE,
		},
	};

	ret = i2c_transfer(g_dev->client->adapter, msg, 1);
	if (ret < 0)
		pr_err("%s i2c write error: %d\n", __func__, ret);

	return ret;
}

static int ft5x0x_i2c_Read(struct i2c_client *client, char *writebuf,
            int writelen, char *readbuf, int readlen)
{
    int ret;

    if (writelen > 0) {
        struct i2c_msg msgs[] = {
            {
             .addr = client->addr,
             .flags = 0,
             .len = writelen,
             .buf = writebuf,
             .scl_rate = FT_TP_I2C_RATE,
             },
            {
             .addr = client->addr,
             .flags = I2C_M_RD,
             .len = readlen,
             .buf = readbuf,
             .scl_rate = FT_TP_I2C_RATE,
             },
        };
        ret = i2c_transfer(client->adapter, msgs, 2);
        if (ret < 0)
            dev_err(&client->dev, "f%s: i2c read error.\n",
                __func__);
    } else {
        struct i2c_msg msgs[] = {
            {
             .addr = client->addr,
             .flags = I2C_M_RD,
             .len = readlen,
             .buf = readbuf,
             .scl_rate = FT_TP_I2C_RATE,
             },
        };
        ret = i2c_transfer(client->adapter, msgs, 1);
        if (ret < 0)
            dev_err(&client->dev, "%s:i2c read error.\n", __func__);
    }
    return ret;
}
/*write data by i2c*/
static int ft5x0x_i2c_Write(struct i2c_client *client, char *writebuf, int writelen)
{
    int ret;

    struct i2c_msg msg[] = {
        {
         .addr = client->addr,
         .flags = 0,
         .len = writelen,
         .buf = writebuf,
         .scl_rate = FT_TP_I2C_RATE,
         },
    };

    ret = i2c_transfer(client->adapter, msg, 1);
    if (ret < 0)
        dev_err(&client->dev, "%s i2c write error.\n", __func__);

    return ret;
}

static int ft5x0x_set_reg(u8 addr, u8 para)
{
    u8 buf[3];
    int ret = -1;

    buf[0] = addr;
    buf[1] = para;
    ret = ft5x0x_i2c_txdata(buf, 2);
    if (ret < 0) {
        pr_err("write reg failed! %#x ret: %d", buf[0], ret);
        return -1;
    }    
    return ret;
}

/* add for tp calibrate */
static int ft5x0x_write_reg(struct i2c_client *client, u8 regaddr, u8 regvalue)
{
    unsigned char buf[2] = {0};
    buf[0] = regaddr;
    buf[1] = regvalue;

    return ft5x0x_i2c_Write(client, buf, sizeof(buf));
}

static int ft5x0x_read_reg(struct i2c_client *client, u8 regaddr, u8 *regvalue)
{
    return ft5x0x_i2c_Read(client, &regaddr, 1, regvalue, 1);
}
/* add for tp calibrate */

#if (FT5X0X_FIRMWARE_AUTO_UPDATE != 0)
#define    FTS_PACKET_LENGTH        128
static u8 CTPM_FW[]= {
    // #include "t7h_ft_app.i"
    // #include "BYD_T7H_0xa0_0x21_20130223_app.i"
    // #include "BYD_T7H_0xa0_0x22_20130301_app.i"
    // #include "BYD_T7H_0xa0_0x23_20130306_app.i"
    #include "BYD_T7H_0xa0_0x24_20130307_app.i"
};

typedef enum
{
    ERR_OK,
    ERR_MODE,
    ERR_READID,
    ERR_ERASE,
    ERR_STATUS,
    ERR_ECC,
    ERR_DL_ERASE_FAIL,
    ERR_DL_PROGRAM_FAIL,
    ERR_DL_VERIFY_FAIL
}E_UPGRADE_ERR_TYPE;

/***********************************************************************
    [function]: 
		           callback:                send data to ctpm by i2c interface;
    [parameters]:
			    txdata[in]:              data buffer which is used to send data;
			    length[in]:              the length of the data buffer;
    [return]:
			    FTS_TRUE:              success;
			    FTS_FALSE:             fail;
************************************************************************/
static int fts_i2c_txdata(u8 *txdata, int length)
{
	int ret;

	struct i2c_msg msg;

	msg.addr = g_dev->client->addr;
	msg.flags = 0;
	msg.len = length;
	msg.buf = txdata;
    msg.scl_rate = 100 * 1000;

	ret = i2c_transfer(g_dev->client->adapter, &msg, 1);
	if (ret < 0)
		pr_err("%s i2c write error: %d\n", __func__, ret);

	return ret;
}

/***********************************************************************
    [function]: 
		           callback:               write data to ctpm by i2c interface;
    [parameters]:
			    buffer[in]:             data buffer;
			    length[in]:            the length of the data buffer;
    [return]:
			    FTS_TRUE:            success;
			    FTS_FALSE:           fail;
************************************************************************/
static bool i2c_write_interface(u8* pbt_buf, int dw_lenth)
{
    int ret;
    ret=i2c_master_send(g_dev->client, pbt_buf, dw_lenth);
    if(ret<=0)
    {
        printk("[TSP]i2c_write_interface error line = %d, ret = %d\n", __LINE__, ret);
        return false;
    }

    return true;
}

/***********************************************************************
    [function]: 
		           callback:                read register value ftom ctpm by i2c interface;
    [parameters]:
                        reg_name[in]:         the register which you want to write;
			    tx_buf[in]:              buffer which is contained of the writing value;
    [return]:
			    FTS_TRUE:              success;
			    FTS_FALSE:             fail;
************************************************************************/
static bool fts_register_write(u8 reg_name, u8* tx_buf)
{
	u8 write_cmd[2] = {0};

	write_cmd[0] = reg_name;
	write_cmd[1] = *tx_buf;

	/*call the write callback function*/
	return i2c_write_interface(write_cmd, 2);
}

/***********************************************************************
[function]: 
                      callback:         send a command to ctpm.
[parameters]:
			  btcmd[in]:       command code;
			  btPara1[in]:     parameter 1;    
			  btPara2[in]:     parameter 2;    
			  btPara3[in]:     parameter 3;    
                      num[in]:         the valid input parameter numbers, 
                                           if only command code needed and no 
                                           parameters followed,then the num is 1;    
[return]:
			  FTS_TRUE:      success;
			  FTS_FALSE:     io fail;
************************************************************************/
static bool cmd_write(u8 btcmd,u8 btPara1,u8 btPara2,u8 btPara3,u8 num)
{
    u8 write_cmd[4] = {0};

    write_cmd[0] = btcmd;
    write_cmd[1] = btPara1;
    write_cmd[2] = btPara2;
    write_cmd[3] = btPara3;
    return i2c_write_interface(write_cmd, num);
}

/***********************************************************************
    [function]: 
		           callback:              read data from ctpm by i2c interface;
    [parameters]:
			    buffer[in]:            data buffer;
			    length[in]:           the length of the data buffer;
    [return]:
			    FTS_TRUE:            success;
			    FTS_FALSE:           fail;
************************************************************************/
static bool i2c_read_interface(u8* pbt_buf, int dw_lenth)
{
    int ret;
    
    ret=i2c_master_recv(g_dev->client, pbt_buf, dw_lenth);

    if(ret<=0)
    {
        printk("[TSP]i2c_read_interface error\n");
        return false;
    }
  
    return true;
}


/***********************************************************************
[function]: 
                      callback:         read a byte data  from ctpm;
[parameters]:
			  buffer[in]:       read buffer;
			  length[in]:      the size of read data;    
[return]:
			  FTS_TRUE:      success;
			  FTS_FALSE:     io fail;
************************************************************************/
static bool byte_read(u8* buffer, int length)
{
    return i2c_read_interface(buffer, length);
}

/***********************************************************************
[function]: 
                      callback:         write a byte data  to ctpm;
[parameters]:
			  buffer[in]:       write buffer;
			  length[in]:      the size of write data;    
[return]:
			  FTS_TRUE:      success;
			  FTS_FALSE:     io fail;
************************************************************************/
static bool byte_write(u8* buffer, int length)
{
    
    return i2c_write_interface(buffer, length);
}

/***********************************************************************
    [function]: 
		           callback:                 read register value ftom ctpm by i2c interface;
    [parameters]:
                        reg_name[in]:         the register which you want to read;
			    rx_buf[in]:              data buffer which is used to store register value;
			    rx_length[in]:          the length of the data buffer;
    [return]:
			    FTS_TRUE:              success;
			    FTS_FALSE:             fail;
************************************************************************/
static bool fts_register_read(u8 reg_name, u8* rx_buf, int rx_length)
{
	u8 read_cmd[2]= {0};
	u8 cmd_len 	= 0;

	read_cmd[0] = reg_name;
	cmd_len = 1;	

	/*send register addr*/
	if(!i2c_write_interface(&read_cmd[0], cmd_len))
	{
		return false;
	}

	/*call the read callback function to get the register value*/		
	if(!i2c_read_interface(rx_buf, rx_length))
	{
		return false;
	}
	return true;
}

int fts_ctpm_auto_clb(struct i2c_client *client)
{
    unsigned char uc_temp = 0x00;
    unsigned char i = 0;

    /*start auto CLB */
    msleep(200);

    ft5x0x_write_reg(client, 0, FTS_FACTORYMODE_VALUE);
    /*make sure already enter factory mode */
    msleep(100);
    /*write command to start calibration */
    ft5x0x_write_reg(client, 2, 0x4);
    msleep(300);
    for (i = 0; i < 100; i++) {
        ft5x0x_read_reg(client, 0, &uc_temp);
        /*return to normal mode, calibration finish */
        if (0x0 == ((uc_temp & 0x70) >> 4))
            break;
    }

    //msleep(200);
    /*calibration OK */
    msleep(300);
    ft5x0x_write_reg(client, 0, FTS_FACTORYMODE_VALUE); /*goto factory mode for store */
    msleep(100);    /*make sure already enter factory mode */
    ft5x0x_write_reg(client, 2, 0x5);   /*store CLB result */
    msleep(300);
    ft5x0x_write_reg(client, 0, FTS_WORKMODE_VALUE);    /*return to normal mode */
    msleep(300);

    /*store CLB result OK */
    return 0;
}

/***********************************************************************
[function]: 
                        callback:          burn the FW to ctpm.
[parameters]:
			    pbt_buf[in]:     point to Head+FW ;
			    dw_lenth[in]:   the length of the FW + 6(the Head length);    
[return]:
			    ERR_OK:          no error;
			    ERR_MODE:      fail to switch to UPDATE mode;
			    ERR_READID:   read id fail;
			    ERR_ERASE:     erase chip fail;
			    ERR_STATUS:   status error;
			    ERR_ECC:        ecc error.
************************************************************************/
E_UPGRADE_ERR_TYPE  fts_ctpm_fw_upgrade(u8* pbt_buf, int dw_lenth)
{
    u8  cmd,reg_val[2] = {0};
	u8  buffer[2] = {0};
    u8  packet_buf[FTS_PACKET_LENGTH + 6];
    u8  auc_i2c_write_buf[10];
    u8  bt_ecc;
	
    int  j,temp,lenght,i_ret,packet_number, i = 0;
    int  i_is_new_protocol = 0;
	

    /******write 0xaa to register 0xfc******/
    cmd=0xaa;
    fts_register_write(0xfc,&cmd);
    msleep(50);
	
     /******write 0x55 to register 0xfc******/
    cmd=0x55;
    fts_register_write(0xfc,&cmd);
    printk("[TSP] Step 1: Reset CTPM test\n");
   
    msleep(30);   

    /*******Step 2:Enter upgrade mode ****/
    printk("\n[TSP] Step 2:enter new update mode\n");
    auc_i2c_write_buf[0] = 0x55;
    auc_i2c_write_buf[1] = 0xaa;
    do
    {
        i ++;
        i_ret = fts_i2c_txdata(auc_i2c_write_buf, 2);
        msleep(5);
    }while(i_ret <= 0 && i < 10 );

    if (i > 1)
    {
        i_is_new_protocol = 1;
    }

    /********Step 3:check READ-ID********/        
    cmd_write(0x90,0x00,0x00,0x00,4);
    byte_read(reg_val,2);
    if (reg_val[0] == 0x79 && reg_val[1] == 0x3)
    {
        printk("[TSP] Step 3: CTPM ID,ID1 = 0x%x,ID2 = 0x%x\n",reg_val[0],reg_val[1]);
    }
    else
    {
        return ERR_READID;
        //i_is_new_protocol = 1;
    }
    

     /*********Step 4:erase app**********/
    if (i_is_new_protocol)
    {
        cmd_write(0x61,0x00,0x00,0x00,1);
    }
    else
    {
        cmd_write(0x60,0x00,0x00,0x00,1);
    }
    msleep(1500);
    printk("[TSP] Step 4: erase. \n");

    /*Step 5:write firmware(FW) to ctpm flash*/
    bt_ecc = 0;
    printk("[TSP] Step 5: start upgrade. \n");
    dw_lenth = dw_lenth - 8;
    packet_number = (dw_lenth) / FTS_PACKET_LENGTH;
    packet_buf[0] = 0xbf;
    packet_buf[1] = 0x00;
    for (j=0;j<packet_number;j++)
    {
        temp = j * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8)(temp>>8);
        packet_buf[3] = (u8)temp;
        lenght = FTS_PACKET_LENGTH;
        packet_buf[4] = (u8)(lenght>>8);
        packet_buf[5] = (u8)lenght;

        for (i=0;i<FTS_PACKET_LENGTH;i++)
        {
            packet_buf[6+i] = pbt_buf[j*FTS_PACKET_LENGTH + i]; 
            bt_ecc ^= packet_buf[6+i];
        }
        
        byte_write(&packet_buf[0],FTS_PACKET_LENGTH + 6);
        msleep(FTS_PACKET_LENGTH/6 + 1);
        if ((j * FTS_PACKET_LENGTH % 1024) == 0)
        {
              printk("[TSP] upgrade the 0x%x th byte.\n", ((unsigned int)j) * FTS_PACKET_LENGTH);
        }
    }

    if ((dw_lenth) % FTS_PACKET_LENGTH > 0)
    {
        temp = packet_number * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8)(temp>>8);
        packet_buf[3] = (u8)temp;

        temp = (dw_lenth) % FTS_PACKET_LENGTH;
        packet_buf[4] = (u8)(temp>>8);
        packet_buf[5] = (u8)temp;

        for (i=0;i<temp;i++)
        {
            packet_buf[6+i] = pbt_buf[ packet_number*FTS_PACKET_LENGTH + i]; 
            bt_ecc ^= packet_buf[6+i];
        }

        byte_write(&packet_buf[0],temp+6);    
        msleep(20);
    }

    /***********send the last six byte**********/
    for (i = 0; i<6; i++)
    {
        temp = 0x6ffa + i;
        packet_buf[2] = (u8)(temp>>8);
        packet_buf[3] = (u8)temp;
        temp =1;
        packet_buf[4] = (u8)(temp>>8);
        packet_buf[5] = (u8)temp;
        packet_buf[6] = pbt_buf[ dw_lenth + i]; 
        bt_ecc ^= packet_buf[6];

        byte_write(&packet_buf[0],7);  
        msleep(20);
    }

    /********send the opration head************/
    cmd_write(0xcc,0x00,0x00,0x00,1);
    byte_read(reg_val,1);
    printk("[TSP] Step 6:  ecc read 0x%x, new firmware 0x%x. \n", reg_val[0], bt_ecc);
    if(reg_val[0] != bt_ecc)
    {
        return ERR_ECC;
    }

    /*******Step 7: reset the new FW**********/
    cmd_write(0x07,0x00,0x00,0x00,1);
    msleep(100);//100ms	
    fts_register_read(0xfc, buffer, 1);	
    if (buffer[0] == 1)
    {
        cmd=4;
        fts_register_write(0xfc, &cmd);
        msleep(2500);//2500ms	
        do	
        {	
            fts_register_read(0xfc, buffer, 1);	
            msleep(100);//100ms	
        }while (buffer[0] != 1); 		   	
    }
    return ERR_OK;
}


/***********************************************************************/
int fts_ctpm_fw_upgrade_with_i_file(void)
{
	u8*     pbt_buf = 0;
	int i_ret;

	pbt_buf = CTPM_FW;
	i_ret =  fts_ctpm_fw_upgrade(pbt_buf,sizeof(CTPM_FW));

	return i_ret;
}


/***********************************************************************/
unsigned char fts_ctpm_get_upg_ver(void)
{
    unsigned int ui_sz;
	
    ui_sz = sizeof(CTPM_FW);
    if (ui_sz > 2)
    {
        return CTPM_FW[ui_sz - 2];
    }
    else
        return 0xff; 
 
}
#endif /* FT5X0X_FIRMWARE_AUTO_UPDATE */

static int ft5x0x_ts_fingers_status_id_map(int finger_id)
{
    int i;
    int invalid_idx = -1;

    for (i = 0; i < MAX_CONTACTS; ++i) {
        if (g_cur_finger_status[i][1] == finger_id) {
            /* find the previous matched id and pressed finger. */
            g_cur_finger_status[i][0] = FT_FINGER_PRESS;
            ft5x0x_debug_report("index[%d]: finger_id = %d, FT_FINGER_PRESS\n", i, finger_id);
            break;
        } else if (g_cur_finger_status[i][1] == FT_INVALID_ID) {
            /* find current finger invalid id and update. */
            invalid_idx = i;
            ft5x0x_debug_report("index[%d]: finger_id = %d, FT_INVALID_ID\n", i, finger_id);
        }
    }

    if (MAX_CONTACTS == i) {
        /* insert current pressed finger to invalid id slot. */
        g_cur_finger_status[invalid_idx][0] = FT_FINGER_PRESS;
        g_cur_finger_status[invalid_idx][1] = finger_id;
        i = invalid_idx;
        ft5x0x_debug_report("MAX_CONTACTS == i: %d, invalid_idx: %d\n", i, invalid_idx);
    }

    return i;
}

static int ft5x0x_read_data(void)
{
	struct ft5x0x_ts_dev *data = i2c_get_clientdata(g_dev->client);
	struct ts_event *event = &data->event;

	u8 buf[32]= {0};//set send addr to 0x00 *important*
	int ret = -1;    
    int i;
    int point_idx, temp_id;

	ret = ft5x0x_i2c_rxdata(buf, 32);

	if (ret < 0) {
		printk("%s read_data i2c_rxdata failed: %d\n", __func__, ret);
		return ret;
	}
	
	memset(event, ~0x00, sizeof(struct ts_event));

#if FT_TP_USE_POINT
	event->touch_point = buf[2] & 0x0f;// 0000 1111
#else
	event->touch_point = buf[2] >>4;// 0000 1111
#endif

#ifdef CONFIG_FT5X0X_MULTITOUCH

    /* clean all fingers status firstly */
    for (i = 0; i < MAX_CONTACTS; ++i) {
        g_cur_finger_status[i][0] = FT_FINGER_RELEASE;
    }

    ft5x0x_debug_2("=======================================================\n");

    for (i = 0; i < event->touch_point; ++i) {
        temp_id = (buf[FT_TOUCH_ID_POS + i * FT_TOUCH_STEP] >> 4) & 0x0f;
        point_idx = ft5x0x_ts_fingers_status_id_map(temp_id);
        event->point[point_idx].id = temp_id;         
        event->point[point_idx].event_flag = (buf[FT_TOUCH_EVENT_POS + i * FT_TOUCH_STEP] >> 6) & 0x03;        
        event->point[point_idx].x = (((buf[FT_TOUCH_X_H_POS + i * FT_TOUCH_STEP]) & 0x0f) << 8) |
                                    (buf[FT_TOUCH_X_L_POS + i * FT_TOUCH_STEP]);
        event->point[point_idx].y = (((buf[FT_TOUCH_Y_H_POS + i * FT_TOUCH_STEP]) & 0x0f) << 8) |
                                    (buf[FT_TOUCH_Y_L_POS + i * FT_TOUCH_STEP]);

        ft5x0x_debug_2("<%d>: point[%d] press: event_flag=%d, id=%d, x=%d, y=%d\n", 
                        i, point_idx, event->point[point_idx].event_flag, event->point[point_idx].id, 
                        event->point[point_idx].x, event->point[point_idx].y);  
    }

#if FT_TP_AXIS_TRANS	
	{
		int cnt = 0;
		short xy_swap;

		ft5x0x_debug(DEBUG_TRACE, "point axis xy swap.....\n");	

		for (cnt = 0; cnt < event->touch_point; cnt++) {
			xy_swap = event->point[cnt].x;
			event->point[cnt].x = event->point[cnt].y;
			event->point[cnt].y = xy_swap;
			event->point[cnt].x = SCREEN_MAX_X - event->point[cnt].x;
		}
	}
#endif

#if FT_TP_AXIS_X_TRANS	
		{
			int cnt = 0;
	
			ft5x0x_debug(DEBUG_TRACE, "point axis x swap.....\n");	
	
			for (cnt = 0; cnt < event->touch_point; cnt++) {
					event->point[cnt].x = SCREEN_MAX_X - event->point[cnt].x;
			}
		}
#endif

	return 0;
#endif
}

static void ft5x0x_report_value(void)
{
	struct ft5x0x_ts_dev *data = i2c_get_clientdata(g_dev->client);
	struct ts_event *event = &data->event;
	u8 i = 0;

    for (i = 0; i < MAX_CONTACTS; ++i) {
        ft5x0x_debug_report("point[%d] x: %d, y: %d, id: %d\n", 
            i, event->point[i].x, event->point[i].y, event->point[i].id);
    }

    for (i = 0; i < MAX_CONTACTS; ++i) {
        if (FT_FINGER_RELEASE == g_cur_finger_status[i][0] && FT_INVALID_ID != g_cur_finger_status[i][1]) {
            ft5x0x_debug_report("point[%d] released\n", i);
            input_mt_slot(data->input_dev, i);
            input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER, false);
            /* current pressed finger is release */
            g_cur_finger_status[i][1] = FT_INVALID_ID;
        } else if (FT_INVALID_ID == g_cur_finger_status[i][1]) {
            ft5x0x_debug_report("point[%d] not be reported...\n", i);
        } else {
            ft5x0x_debug_report("point[%d] pressed\n", i);
            input_mt_slot(data->input_dev, i);
            input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER, true);
       
            input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR, 127);
            input_report_abs(data->input_dev, ABS_MT_TOUCH_MINOR, 0);
            input_report_abs(data->input_dev, ABS_MT_POSITION_X, event->point[i].x);
            input_report_abs(data->input_dev, ABS_MT_POSITION_Y, event->point[i].y);
        }
        //ft5x0x_debug_report("point[%d] status update\n", i);
    }

    for (i = 0; i < MAX_CONTACTS; ++i) {
        ft5x0x_debug_report("<%d> g_cur_finger_status[0]: %d, [1]: %d\n", 
            i, g_cur_finger_status[i][0], g_cur_finger_status[i][1]);
    }

    input_sync(data->input_dev);
}	/*end ft5x0x_report_value*/

#ifdef FT_TP_SCAN_RATE_TEST
static void ft5x0x_ts_scan_rate_test(void)
{
	unsigned int sec_diff, usec_diff;
	unsigned int msec_diff, frq;
	
	tv_pre = tv_cur;

	do_gettimeofday(&tv_cur);

	ft5x0x_debug_rate("tv_cur: sec = %d, usec = %d\n", (unsigned int)tv_cur.tv_sec, (unsigned int)tv_cur.tv_usec);
	ft5x0x_debug_rate("tv_pre: sec = %d, usec = %d\n", (unsigned int)tv_pre.tv_sec, (unsigned int)tv_pre.tv_usec);
	
	sec_diff = tv_cur.tv_sec - tv_pre.tv_sec;
	usec_diff = tv_cur.tv_usec - tv_pre.tv_usec;

	msec_diff = (sec_diff*1000000 + usec_diff) / 1000;
	frq = 1000 / msec_diff;

	ft5x0x_debug_rate("%s: tp scan rate = %dms, frq = %d\n", __func__, msec_diff, frq);
}
#endif

static void ft5x0x_ts_do_upgrade(struct ft5x0x_ts_dev *ft5x0x_ts)
{
#if (FT5X0X_FIRMWARE_AUTO_UPDATE != 0)
{
    unsigned char reg_value;
    unsigned char reg_version;
    unsigned char upgrade_version;
    int err = 0;

    printk("===> FT5X0X_FIRMWARE_AUTO_UPDATE start in working\n");  

    fts_register_read(FT5X0X_REG_FIRMID, &reg_version,1);
    printk("[TSP] firmware version = 0x%2x\n", reg_version);
    fts_register_read(FT5X0X_REG_REPORT_RATE, &reg_value,1);
    printk("[TSP]firmware report rate = %dHz\n", reg_value*10);
    fts_register_read(FT5X0X_REG_THRES, &reg_value,1);
    printk("[TSP]firmware threshold = %d\n", reg_value * 4);
    fts_register_read(FT5X0X_REG_NOISE_MODE, &reg_value,1);
    printk("[TSP]nosie mode = 0x%2x\n", reg_value);

    upgrade_version = fts_ctpm_get_upg_ver();

    if (upgrade_version > reg_version)
    {
        printk("[TSP] start upgrade new verison 0x%2x\n", upgrade_version);
        msleep(200);
        err =  fts_ctpm_fw_upgrade_with_i_file();
        if (err == 0)
        {
            printk("[TSP] ugrade successfuly.\n");
            msleep(300);
            printk("[TSP] start auto CLB.\n");
            fts_ctpm_auto_clb(ft5x0x_ts->client);       /* start auto CLB importantly */
            fts_register_read(FT5X0X_REG_FIRMID, &reg_value,1);
            printk("FTS_DBG from old version 0x%2x to new version = 0x%2x\n", reg_version, reg_value);
        }
        else
        {
            printk("[TSP] ugrade fail err=%d, line = %d.\n",err, __LINE__);
        }
        msleep(4000);
    }
    else
    {
        printk("[TSP] upgrade verison[0x%2x] <= current version[0x%2x], don't need upgrade.\n", upgrade_version, reg_version);
    }
}
#endif
}

#if (FT_UPGRADE_MODE == FT_UPGRADE_WORKQUEUE)
static void ft5x0x_ts_upgrade_work(struct work_struct *work)
{
    struct ft5x0x_ts_dev *ft5x0x_ts = container_of(work, struct ft5x0x_ts_dev, upgrade_work);

    ft5x0x_ts_do_upgrade(ft5x0x_ts);

    cancel_work_sync(&ft5x0x_ts->pen_event_work);
}
#endif

extern int dwc_vbus_status(void);

static void ft5x0x_ts_pen_irq_work(struct work_struct *work)
{
    struct ft5x0x_ts_dev *ft5x0x_ts = container_of(work, struct ft5x0x_ts_dev, pen_event_work);
	int ret = -1;
    unsigned char buf;
    static int thgroup_flag = 0;
    
	ft5x0x_debug(DEBUG_TRACE, "%s: enter!\n");

    if (0 == thgroup_flag && dwc_vbus_status()) {    // changer TP sensitive when USB inserted and not inserted.
        ft5x0x_debug_2("USB Inserted!\n");
        ft5x0x_set_reg(FT5X0X_REG_THGROUP, 30);
        // fts_register_read(FT5X0X_REG_THGROUP, &buf, 1);
        // ft5x0x_debug_2("buf = %d\n", buf);
        thgroup_flag = 1;
    } else if (1 == thgroup_flag && !dwc_vbus_status()) {
        ft5x0x_debug_2("USB not Inserted!\n");
        ft5x0x_set_reg(FT5X0X_REG_THGROUP, ft5x0x_ts->ft_thgroup);
        // fts_register_read(FT5X0X_REG_THGROUP, &buf, 1);
        // ft5x0x_debug_2("buf = %d\n", buf);
        thgroup_flag = 0;
    }
	
	ret = ft5x0x_read_data();	
	if (ret == 0) {	
		ft5x0x_report_value();
	}
  	enable_irq(g_dev->irq);
}

static irqreturn_t ft5x0x_ts_interrupt(int irq, void *dev_id)
{
	struct ft5x0x_ts_dev *ft5x0x_ts = dev_id;

	//ft5x0x_debug(DEBUG_TRACE, "%s: enter!\n");	

#ifdef FT_TP_SCAN_RATE_TEST	
	ft5x0x_ts_scan_rate_test();
#endif

	disable_irq_nosync(g_dev->irq);		

	queue_work(ft5x0x_ts->ts_workqueue, &ft5x0x_ts->pen_event_work);

	return IRQ_HANDLED;
}

static void ft5x0x_ts_reset_hw(ft_tp_work_t tp_work)
{
    if (tp_work == FT_TP_RESUME) {
        gpio_direction_output(FT_TP_RESET_PIN, GPIO_HIGH);
        gpio_set_value(FT_TP_RESET_PIN, GPIO_HIGH);
        mdelay(FT_TP_RESET_DELAY_IIME);
        
        gpio_direction_output(FT_TP_RESET_PIN, GPIO_LOW);
        gpio_set_value(FT_TP_RESET_PIN, GPIO_LOW);
        mdelay(FT_TP_RESET_DELAY_IIME);

        gpio_direction_output(FT_TP_RESET_PIN, GPIO_HIGH);
        gpio_set_value(FT_TP_RESET_PIN, GPIO_HIGH);
    } else {   // FT_TP_SUSPEND
        gpio_direction_output(FT_TP_RESET_PIN, GPIO_LOW);
        gpio_set_value(FT_TP_RESET_PIN, GPIO_LOW);
    }	
}

static void ft5x0x_ts_all_fingers_clean(struct ft5x0x_ts_dev *ts)
{
    int i;

    ft5x0x_debug_2("all fingers released...\n");

    for (i = 0; i < MAX_CONTACTS; ++i) {
        input_mt_slot(ts->input_dev, i);
        input_mt_report_slot_state(ts->input_dev, MT_TOOL_FINGER, false);
        g_cur_finger_status[i][1] = FT_INVALID_ID;
    }
    input_sync(ts->input_dev);
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void ft5x0x_ts_suspend(struct early_suspend *handler)
{
	struct ft5x0x_ts_dev *ts = container_of(handler, struct ft5x0x_ts_dev, early_suspend);
	struct ft5x0x_platform_data *pdata = pdata = ts->client->dev.platform_data;
	int ret;

	ft5x0x_debug(DEBUG_TRACE, "%s: ----> enter!\n", __func__);	
	
	if(ts->irq)
		disable_irq_nosync(ts->irq);

	ret = cancel_work_sync(&ts->pen_event_work);
	if (ret && ts->irq) /* if work was pending disable-count is now 2 */
		enable_irq(ts->irq);

    ft5x0x_ts_all_fingers_clean(ts);
	
	/* ft_acgzx: step_1: set PMODE_HIBERNATE */
    ft5x0x_debug_2("%s: suspend by hw_reset\n", __func__);
	ft5x0x_set_reg(FT5X0X_REG_PMODE, PMODE_HIBERNATE);
}

static void ft5x0x_ts_resume(struct early_suspend *handler)
{
	struct ft5x0x_ts_dev *ts = container_of(handler, struct ft5x0x_ts_dev, early_suspend);
	struct ft5x0x_platform_data *pdata = pdata = ts->client->dev.platform_data;

	ft5x0x_debug(DEBUG_TRACE, "%s: ----> enter!\n", __func__);	

	/* ft_acgzx: step_2: don't set PMODE_ACTIVE, reset hardware directly!*/
    ft5x0x_debug_2("%s: resume by hw_reset\n", __func__);
	ft5x0x_ts_reset_hw(FT_TP_RESUME);

	if(ts->irq)
		enable_irq(ts->irq);
	return;
}
#endif  //CONFIG_HAS_EARLYSUSPEND

static int ft5x0x_rw_data_test(void)
{
	struct ft5x0x_ts_dev *data = i2c_get_clientdata(g_dev->client);
	struct ts_event *event = &data->event;

	u8 buf[32]= {0};//set send addr to 0x00 *important*
	int ret = -1, i;

	ft5x0x_debug(DEBUG_TRACE, "%s: enter!\n", __func__);	

	ret = ft5x0x_i2c_rxdata(buf, 32);

	if (ret < 0) {
		ft5x0x_debug(DEBUG_TRACE, "%s read i2c_rxdata failed: %d\n", __func__, ret);
		return ret;
	}

	for (i = 0; i < 32; i++) {
		ft5x0x_debug(DEBUG_TRACE, "buf[%d] = 0x%x, ", i, buf[i]);	
		if (3 == i+1) {
			ft5x0x_debug(DEBUG_TRACE, "\n");
		}
	}
	ft5x0x_debug(DEBUG_TRACE, "\n");	

	return 0;
}

static int ft5x0x_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ft5x0x_ts_dev *ft5x0x_ts = NULL;
	struct ft5x0x_platform_data *pdata = pdata = client->dev.platform_data;
	struct input_dev *input_dev;
	int err = 0;
	uint8_t version[4] = {0xA1, 0x00, 0x00, 0x00};
    unsigned char uc_reg_value;
    unsigned char uc_reg_addr;

	dev_info(&client->dev, "ft5x0x_ts_probe!\n");
	/*
		0xA1: High byte of lib version
		0xA2: Low byte of lib version
		0xA3: Vendor ID
		0xA6: Firmware ID
	 */

    printk("%s: enter, g_tp_status = %d ............... g_tp_status\n", __func__, g_tp_status);

	if (pdata == NULL) {
		dev_err(&client->dev, "%s: platform data is null\n", __func__);
		goto err_pdata_null;
	}
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		goto err_i2c_check_functionality;
	}

    if(pdata->init_hw)
        pdata->init_hw();

    ft5x0x_ts = (struct ft5x0x_ts_dev *)kzalloc(sizeof(*ft5x0x_ts), GFP_KERNEL);
    if (!ft5x0x_ts) {
        err = -ENOMEM;
        goto err_kzalloc_failed;
    }

    ft5x0x_ts->client = client;     /* note: g_dev use client. */

    g_dev = ft5x0x_ts;

    err = ft5x0x_i2c_rxdata(version, 4);
    if (err < 0) {
        dev_err(&client->dev, "ft5x0x_probe : fail to read TP lib version and vendor id\n");
        goto err_tp_i2c_failed;
    }
    dev_info(&client->dev, "ft5x0x_probe : TP lib version 0x%x vendor ID 0x%x\n", 
          ((unsigned int)version[1]<<8 | (unsigned int)version[2]), (unsigned int)version[3]);

    version[0] = 0xA6;
    err = ft5x0x_i2c_rxdata(version, 2);
    if (err < 0) {
        dev_err(&client->dev, "ft5x0x_probe : fail to read TP fw version\n");
        goto err_tp_i2c_failed;
    }
    dev_info(&client->dev, "ft5x0x_probe : TP firmware version 0x%x\n", (unsigned int)version[1]);

    /*get some register information */
    uc_reg_addr = FT5x0x_REG_FW_VER;
    ft5x0x_i2c_Read(client, &uc_reg_addr, 1, &uc_reg_value, 1);
    dev_info(&client->dev, "[FTS] Firmware version = 0x%x\n", uc_reg_value);

    uc_reg_addr = FT5x0x_REG_POINT_RATE;
    ft5x0x_i2c_Read(client, &uc_reg_addr, 1, &uc_reg_value, 1);
    dev_info(&client->dev, "[FTS] report rate is %dHz.\n",
        uc_reg_value * 10);

    uc_reg_addr = FT5X0X_REG_THGROUP;
    ft5x0x_i2c_Read(client, &uc_reg_addr, 1, &uc_reg_value, 1);
    dev_info(&client->dev, "[FTS] touch threshold is %d.\n",
        uc_reg_value * 4);

    /***wait CTP to bootup normally***/
    msleep(200);

    input_dev = input_allocate_device();
    if (!input_dev) {
        err = -ENOMEM;
        dev_err(&client->dev, "failed to allocate input device\n");
        goto err_input_allocate_device;
    }
	
	ft5x0x_ts->input_dev = input_dev;
	ft5x0x_ts->irq = client->irq;

    ft5x0x_ts->ft_thgroup = uc_reg_value;

    if(!ft5x0x_ts->irq) {
        dev_info(&ft5x0x_ts->client->dev, "no IRQ?\n");
        goto err_irq;
    } else {
#if 0
        err = gpio_request(ft5x0x_ts->irq, FT5X0X_NAME);
        if(err != 0) {
            gpio_free(ft5x0x_ts->irq);
            dev_err(&client->dev, "ft5x0x_probe : request irq_gpio failed\n");
            goto exit_platform_data_null;
        }
#endif
    }

	__set_bit(EV_ABS, input_dev->evbit);
	__set_bit(INPUT_PROP_DIRECT, input_dev->propbit);
	
	input_mt_init_slots(input_dev, MAX_CONTACTS);

	input_set_abs_params(input_dev,ABS_MT_POSITION_X, 0, SCREEN_MAX_X, 0, 0);
	input_set_abs_params(input_dev,ABS_MT_POSITION_Y, 0, SCREEN_MAX_Y, 0, 0);
	input_set_abs_params(input_dev,ABS_MT_TOUCH_MAJOR, 0, PRESS_MAX, 0, 0);
	input_set_abs_params(input_dev,ABS_MT_WIDTH_MAJOR, 0, 200, 0, 0);

    //input_set_abs_params(input_dev,ABS_MT_TRACKING_ID, 1, 5, 0, 0);

	input_dev->name	= FT5X0X_NAME;		//dev_name(&client->dev)

	err = input_register_device(input_dev);
	if (err) {
		dev_err(&client->dev,
		"ft5x0x_ts_probe: failed to register input device: %s\n",
		dev_name(&client->dev));
		goto err_input_register_device;
	}

	//ft5x0x_rw_data_test();
	
	i2c_set_clientdata(client, ft5x0x_ts);
	INIT_WORK(&ft5x0x_ts->pen_event_work, ft5x0x_ts_pen_irq_work);
	ft5x0x_ts->ts_workqueue = create_singlethread_workqueue(FT5X0X_NAME);
	if (!ft5x0x_ts->ts_workqueue) {
		err = -ESRCH;
		goto err_create_singlethread_workqueue;
	}

#if (FT_UPGRADE_MODE == FT_UPGRADE_WORKQUEUE)
    INIT_WORK(&ft5x0x_ts->upgrade_work, ft5x0x_ts_upgrade_work);
    ft5x0x_ts->upgrade_workqueue = create_singlethread_workqueue("upgrade_wq");
    if (!ft5x0x_ts->upgrade_workqueue) {
        err = -ESRCH;
        goto err_create_singlethread_workqueue;
    }
#endif

	err = request_irq(gpio_to_irq(ft5x0x_ts->irq), ft5x0x_ts_interrupt, 
		  			IRQF_TRIGGER_FALLING, "ft5x0x_ts", ft5x0x_ts);
	if (err < 0) {
		dev_err(&client->dev, "ft5x0x_probe: request irq failed\n");
		goto err_request_irq;
	}

	disable_irq_nosync(g_dev->irq);

#ifdef CONFIG_HAS_EARLYSUSPEND
	ft5x0x_ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ft5x0x_ts->early_suspend.suspend = ft5x0x_ts_suspend;
	ft5x0x_ts->early_suspend.resume	= ft5x0x_ts_resume;
	register_early_suspend(&ft5x0x_ts->early_suspend);
#endif
//wake the CTPM
//	__gpio_as_output(GPIO_FT5X0X_WAKE);		
//	__gpio_clear_pin(GPIO_FT5X0X_WAKE);		//set wake = 0,base on system
//	 msleep(100);
//	__gpio_set_pin(GPIO_FT5X0X_WAKE);			//set wake = 1,base on system
//	msleep(100);
//	ft5x0x_set_reg(0x88, 0x05); //5, 6,7,8
//	ft5x0x_set_reg(0x80, 30);
//	msleep(50);

#if (FT_UPGRADE_MODE == FT_UPGRADE_WORKQUEUE)
    /* submit upgrade_workqueue */
    queue_work(ft5x0x_ts->upgrade_workqueue, &ft5x0x_ts->upgrade_work);
#else
    ft5x0x_ts_do_upgrade(ft5x0x_ts);
#endif
    enable_irq(g_dev->irq);

    g_tp_status = TP_FT_OK;

	ft5x0x_debug(DEBUG_TRACE, "%s: success!\n", __func__);

    return 0;

err_entry:
	free_irq(client->irq, ft5x0x_ts);
err_request_irq:
	cancel_work_sync(&ft5x0x_ts->pen_event_work);
	destroy_workqueue(ft5x0x_ts->ts_workqueue);

	//printk("==singlethread error =\n");
err_create_singlethread_workqueue:
	i2c_set_clientdata(client, NULL);
	
err_input_register_device:
err_irq:
    input_free_device(input_dev);
err_input_allocate_device:
err_tp_i2c_failed:
    kfree(ft5x0x_ts);
    ft5x0x_ts = NULL;

err_kzalloc_failed:
err_i2c_check_functionality:
err_pdata_null:
    g_tp_status = TP_FT_FAIL;

	return err;
}

static int __devexit ft5x0x_ts_remove(struct i2c_client *client)
{
	struct ft5x0x_ts_dev *ft5x0x_ts = i2c_get_clientdata(client);
	unregister_early_suspend(&ft5x0x_ts->early_suspend);
	free_irq(client->irq, ft5x0x_ts);
	input_unregister_device(ft5x0x_ts->input_dev);
	kfree(ft5x0x_ts);
	cancel_work_sync(&ft5x0x_ts->pen_event_work);
	destroy_workqueue(ft5x0x_ts->ts_workqueue);
	i2c_set_clientdata(client, NULL);
	return 0;
}

static const struct i2c_device_id ft5x0x_ts_id[] = {
	{ FT5X0X_NAME, 0 },{ }
};
MODULE_DEVICE_TABLE(i2c, ft5x0x_ts_id);

static struct i2c_driver ft5x0x_ts_driver = {
	.probe		= ft5x0x_ts_probe,
	.remove		= __devexit_p(ft5x0x_ts_remove),
	.id_table	= ft5x0x_ts_id,
	.driver	= {
		.name	= FT5X0X_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init ft5x0x_ts_init(void)
{
	return i2c_add_driver(&ft5x0x_ts_driver);
}

static void __exit ft5x0x_ts_exit(void)
{
	i2c_del_driver(&ft5x0x_ts_driver);
}

#if defined(CONFIG_TOUCHSCREEN_FT_ELAN_TS_SEL)
int ft5x0x_ts_entry(void)
{
    return i2c_add_driver(&ft5x0x_ts_driver);
}
EXPORT_SYMBOL_GPL(ft5x0x_ts_entry);

void ft5x0x_ts_quit(void)
{
    i2c_del_driver(&ft5x0x_ts_driver);
}
EXPORT_SYMBOL_GPL(ft5x0x_ts_quit);
#else
module_init(ft5x0x_ts_init);
module_exit(ft5x0x_ts_exit);
#endif

MODULE_AUTHOR("<wenfs@Focaltech-systems.com>");
MODULE_DESCRIPTION("FocalTech ft5x0x TouchScreen driver");
MODULE_LICENSE("GPL");

