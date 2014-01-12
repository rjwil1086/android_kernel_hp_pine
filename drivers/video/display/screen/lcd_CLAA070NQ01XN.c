#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/rk_fb.h>
#include <mach/gpio.h>
#include <mach/iomux.h>
#include <mach/board.h>
#include "../../rockchip/hdmi/rk_hdmi.h"
#include "screen.h"

#ifdef CONFIG_RK610_LVDS
#include "../transmitter/rk610_lcd.h"
#endif


/* Base */

#define OUT_TYPE	    SCREEN_RGB
#define OUT_FACE	    OUT_P888


#define OUT_CLK	          48000000
#define LCDC_ACLK         300000000           //29 lcdc axi DMA ÆµÂÊ

/* Timing */
#define H_PW			10
#define H_BP			300//  Horizontal Blank sequence Time mini

#define H_VD			1024// Horizontal effective Time type
#define H_FP			20// Horizontal total Time type(1344)-H_VD-H_BP

#define V_PW			2
#define V_BP			25// Vertical Blank sequence Time mini
#define V_VD			600// Vertical effective Time type
#define V_FP			10// Vertical total Time type(635)-V_VD-V_BP

#define LCD_WIDTH          154//	1024
#define LCD_HEIGHT         86//	600
/* Other */

#define DCLK_POL	0
#define SWAP_RB		0




void set_lcd_info(struct rk29fb_screen *screen, struct rk29lcd_info *lcd_info )
{
	/* screen type & face */
	screen->face = OUT_FACE;
	screen->type = OUT_TYPE;
#ifdef CONFIG_RK610_LVDS
	screen->hw_format = OUT_FORMAT;
#endif
	
	/* Screen size */
	screen->x_res = H_VD;
	screen->y_res = V_VD;

	screen->width = LCD_WIDTH;
	screen->height = LCD_HEIGHT;

    /* Timing */
	screen->lcdc_aclk = LCDC_ACLK;
	screen->pixclock = OUT_CLK;
	screen->left_margin = H_BP;
	screen->right_margin = H_FP;
	screen->hsync_len = H_PW;
	screen->upper_margin = V_BP;
	screen->lower_margin = V_FP;
	screen->vsync_len = V_PW;

	/* Pin polarity */
	screen->pin_hsync = 0;
	screen->pin_vsync = 0;
	screen->pin_den = 0;
	screen->pin_dclk = DCLK_POL;

	/* Swap rule */
	screen->swap_rb = SWAP_RB;
	screen->swap_rg = 0;
	screen->swap_gb = 0;
	screen->swap_delta = 0;
	screen->swap_dumy = 0;

	/* Operation function*/
	screen->init = NULL;
	screen->standby = NULL;
	//screen->dsp_lut = dsp_lut;
	//screen->sscreen_get = set_scaler_info;
}
