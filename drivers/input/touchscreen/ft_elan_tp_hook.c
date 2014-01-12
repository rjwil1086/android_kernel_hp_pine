/* 
 * drivers/input/touchscreen/ft_elan_tp_hook.c
 * 
 * Author: zhengxing
 * Add the process that FocalTech ft5x0x and ELAN EKTF2K TouchScreen driver hook,
 * Reduce unnecessary I2C communication time.
 */

#include <linux/module.h>

#include "ft_elan_tp_hook.h"

#if 1
#define tp_hook_debug( ...) \
	do { \
		printk(__VA_ARGS__); \
	} while (0) 
#else
#define tp_hook_debug( ...)
#endif

tp_status_t g_tp_status = TP_INIT;

extern int ft5x0x_ts_entry(void);
extern void ft5x0x_ts_quit(void);

extern int elan_ktf2k_ts_entry(void);
extern void elan_ktf2k_ts_quit(void);

extern int elan_toptouch_ts_entry(void);
extern void elan_toptouch_ts_quit(void);

static int __init ft_elan_tp_init(void)
{
	// tp_hook_debug("%s: enter, g_tp_status = %d\n", __func__, g_tp_status);

	/* Call ft5x0x_ts_entry firstly. */
	ft5x0x_ts_entry();
	if(TP_FT_OK == g_tp_status){
		tp_hook_debug("%s: matched ft5x0x tp\n", __func__);
		return 0;
	}	
	elan_ktf2k_ts_entry();
	if(TP_ELAN_OK == g_tp_status){
		tp_hook_debug("%s: matched ektf2k tp\n", __func__);
		return 0;
		}	
	elan_toptouch_ts_entry();
	if(TP_ELAN_TOPTOUCH_OK == g_tp_status){
		tp_hook_debug("%s: matched elan toptouch tp\n", __func__);
		return 0;
		}	
	tp_hook_debug("%s: neither ft5x0x nor ektf2k tp can't be matched! no entry\n", __func__);
	return -1;
	
	/*if (TP_FT_OK != g_tp_status) {
		elan_ktf2k_ts_entry();
		if (TP_ELAN_OK != g_tp_status) {
			tp_hook_debug("%s: neither ft5x0x nor ektf2k tp can't be matched! no entry\n", __func__);
			return -1;
		} else {
			tp_hook_debug("%s: matched ektf2k tp\n", __func__);
		}
	} else {
		tp_hook_debug("%s: matched ft5x0x tp\n", __func__);
	}

	return 0;*/
}

static void __exit ft_elan_tp_exit(void)
{
	// tp_hook_debug("%s: enter, g_tp_status = %d\n", __func__, g_tp_status);

	if (TP_FT_OK == g_tp_status) {
		ft5x0x_ts_quit();
	} else if (TP_ELAN_OK == g_tp_status) {
		elan_ktf2k_ts_quit();
	}else if (TP_ELAN_TOPTOUCH_OK == g_tp_status) {
		elan_toptouch_ts_quit();
	}
	else {
		tp_hook_debug("%s: neither ft5x0x nor ektf2k tp can't be matched! no quit\n", __func__);
	}

	g_tp_status = TP_EXIT;
}

module_init(ft_elan_tp_init);
module_exit(ft_elan_tp_exit);

MODULE_AUTHOR("<zhengxing@rock-chips.com>");
MODULE_DESCRIPTION("FocalTech ft5x0x and ELAN EKTF2K TouchScreen driver hook");
MODULE_LICENSE("GPL");