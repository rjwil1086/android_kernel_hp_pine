/*
 * acs422.h - ACS422 audio codec interface
 *
 * Copyright (C) 2010 Integrated Device Technology, Inc. All Rights Reserved.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#ifndef _ACS422_H
#define _ACS422_H

#define ROCKCHIP
//#undef ROCKCHIP

//#define DEBUG_REG
#undef DEBUG_REG

//#define DEBUG_DSP
#undef DEBUG_DSP


#if defined(ROCKCHIP)
#include <linux/gpio.h>
#include <mach/iomux.h>

#define ACS_HPMIC 1
#define ACS_D_MIC 0
#endif
/*
 * Registers
 */
#define ACS422_HPVOLL		0
#define ACS422_HPVOLR		1
#define ACS422_SPKVOLL		2
#define ACS422_SPKVOLR		3
#define ACS422_DACVOLL		4
#define ACS422_DACVOLR		5
#define ACS422_ADCVOLL		6
#define ACS422_ADCVOLR		7
#define ACS422_INPVOLL		8
#define ACS422_INPVOLR		9
#define ACS422_VOLUPDCTRL	10
#define ACS422_ADCINPMODE	11
#define ACS422_ADCSIGPATHL	12
#define ACS422_ADCSIGPATHR	13
#define ACS422_ALC0		14
#define ACS422_ALC1		15

#define ACS422_AICTL		19
#define ACS422_AICTRL2		20

#define ACS422_ADCSR		23

#define ACS422_ADCCTL		22
#define ACS422_ADCSR		23
#define ACS422_DACCTL		24
#define ACS422_DACSR		25
#define ACS422_PWRM1		26
#define ACS422_PWRM2		27
#define ACS422_HPDETECT		28
#define ACS422_THERMTS		29

#define ACS422_EQCTRL		32

#define ACS422_DMICCTRL		36
#define ACS422_CLECTRL		37

#define ACS422_FXCTRL		57
#define ACS422_WDSPRAM		58

#define ACS422_RDSPRAM		61

#define ACS422_DSPRAM_ADR	64

#define ACS422_PWM3CTL	69

#define ACS422_PLLCTL1D		98

#define ACS422_DEVADDR		124
#define ACS422_DEVIDL		125
#define ACS422_DEVIDH		126
#define ACS422_REVID		127
#define ACS422_RESET		128

#define ACS422_DACRAMSTAT	138
#define ACS422_HPDETSTAT	139

/*
 * Bits and masks
 */

/*
 * ACS422_PWRM
 */
#define ACS422_PWRM1_DIGEN			0x01
#define ACS422_PWRM1_MICBIASEN		0x02
#define ACS422_PWRM1_RADCEN			0x04
#define ACS422_PWRM1_LADCEN			0x08
#define ACS422_PWRM1_RPGAEN			0x10
#define ACS422_PWRM1_LPGAEN			0x20
#define ACS422_PWRM1_RBSTEN			0x40
#define ACS422_PWRM1_LBSTEN			0x80

#define ACS422_PWRM2_VREF				0x01
#define ACS422_PWRM2_RINMUXEN			0x02
#define ACS422_PWRM2_LINMUXEN			0x04
#define ACS422_PWRM2_RSPKEN			0x08
#define ACS422_PWRM2_LSPKEN			0x10
#define ACS422_PWRM2_RHPEN			0x20
#define ACS422_PWRM2_LHPEN			0x40
#define ACS422_PWRM2_D2SEN			0x80

/*
 * ACS422_AICTRL
 */
#define ACS422_AICTRL_LJ				0x0
#define ACS422_AICTRL_RJ				0x1
#define ACS422_AICTRL_I2S				0x2
#define ACS422_AICTRL_16BIT			0x0
#define ACS422_AICTRL_20BIT			0x4
#define ACS422_AICTRL_24BIT			0x8
#define ACS422_AICTRL_32BIT			0xc
#define ACS422_AICTRL_LRCLKINV		0x10
#define ACS422_AICTRL_MASTER			0x20
#define ACS422_AICTRL_BCLKINV			0x40
#define ACS422_AICTRL_WL_MASK			0xc

/*
 * ACS422_AICTRL2
 */
#define ACS422_AICTRL2_DAC_CHN_CFG		0xc0
#define ACS422_AICTRL2_BLRCM_MASK		0x7

/*
 * ACS422_ADCSR & ACS422_DACSR
 */
#define ACS422_SR_MUL_025				0x0
#define ACS422_SR_MUL_050				0x1
#define ACS422_SR_MUL_1				0x2
#define ACS422_SR_MUL_2				0x3

#define ACS422_SR_32000				0x0
#define ACS422_SR_44100				0x8
#define ACS422_SR_48000				0x10
#define ACS422_SR_MASK				0x1f

#define ACS422_SR_64x				0xc0

/*
 * ACS422_EQCTRL
 */
#define ACS422_EQCTRL_EQ0_EN			0x8
#define ACS422_EQCTRL_EQ1_EN			0x80

/*
 * ACS422_CLECTRL
 */

#define ACS422_CLECTRL_COMP		0
#define ACS422_CLECTRL_LIMITER	1
#define ACS422_CLECTRL_EXPANDER 2

/*
 * ACS422_FXCTRL
 */
#define ACS422_FXCTRL_BASS_EN		0x2
#define ACS422_FXCTRL_BASS_SH		1
#define ACS422_FXCTRL_TREBLE_EN	0x8
#define ACS422_FXCTRL_TREBLE_SH	3
#define ACS422_FXCTRL_3D_EN		0x10
#define ACS422_FXCTRL_3D_SH		4

/*
 * ACS422_ADCSIGPATH
 */
#define ACS422_ADCSIGPATH_MBST_SH 4
#define ACS422_ADCSIGPATH_INSEL_SH 6

#define ACS422_HPDETECT_HPSWEN	0x80
#define ACS422_HPDETECT_HPSWPOL 0x40

#define ACS422_HPDETSTAT_HPDET 0x80

#define ACS322_PARTID_H 0x32
#define ACS422_PARTID_H 0x22

#define ACS422_MCLK		0

#define ACS422_DSPREG_SIZE	3

/*
 * ACS422_PLLCTL1D
 */
#define ACS422_PLLCTL1D_22_24_SEL 1

/*
 * ACS422_PWM3CTL
 */
#define ACS422_PWM3CTL_CHN_SWAP		0x40

#define ACS422_MAX_REG 178
#define ACS422_MAX_CACHED_REG 0x77
#define ACS422_CACHE_RANGES_NUM 2

struct acs422_priv {
	struct i2c_client *i2c;
	struct snd_soc_codec *codec;
	int master;
	int off;
	u8 idl;
	u8 idh;
	u8 rev;
	int clk_dir;
	int clk_freq;
	u8 vol_ctls[8];
	u8 vsw_ctls[8];
	struct mutex mutex;
	unsigned int pbk_sr;
	int hp_on;
	bool mdet_initalized;
	struct acs422_platform_data *preset_data;
	u8 reg_wp_mask[ACS422_MAX_REG];
	u8 reg_app_cache[ACS422_MAX_REG];
	u8 reg_cache[ACS422_MAX_CACHED_REG + 2];
};

int acs422_hp_event(struct snd_soc_codec *codec, int jck_stat);
int acs422_select_input(struct snd_soc_codec *codec, int inp, int boost);

#endif
