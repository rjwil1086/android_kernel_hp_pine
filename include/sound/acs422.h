/*
 * acs422.h
 *
 *  Author: Vitaliy Kulikov
 *  Copyright (C) 2010 Integrated Device Technology, Inc. All Rights Reserved.
 */

#ifndef __LINUX_SOUND_ACS422_H
#define __LINUX_SOUND_ACS422_H

#define MAX_PRESET_NAME_SZ 64
#define MAX_PRESET_REGS_NUM 32
#define ACS422_DSP_SZ 176
#define ACS422_EQ_SZ 64
#define ACS422_EQ0_OFFS 0
#define ACS422_EQ1_OFFS ACS422_EQ0_OFFS+ACS422_EQ_SZ

struct acs422_reg_init {
	int reg;
	int val;
};

struct acs422_preset {
	const char name[MAX_PRESET_NAME_SZ];
	struct acs422_reg_init preset_regs[MAX_PRESET_REGS_NUM];
	unsigned int sample_rate;
	unsigned int dsp_params[ACS422_DSP_SZ];
};

struct acs422_platform_data {
	int codec_idx;
	unsigned int init_regs_num;
	struct acs422_reg_init *init_regs;
	unsigned int presets_num;
	struct acs422_preset *presets;
	char spk_preset_name[MAX_PRESET_NAME_SZ];
	char hp_preset_name[MAX_PRESET_NAME_SZ];
};

/*
 * u8 c0_chn_cfg:2;
 * u8 c1_chn_cfg:2;
 * channel configuration:
 * 00 - Left,Right (normal)
 * 01 - Left,Left (left only)
 * 10 - Right,Right (right only)
 * 11 - Right,Left (swapped)
 */

int acs422_rotate_spkr_channels(u8 chn_cfg);
#define C0_CHN_CFG_MASK 0x03
#define C1_CHN_CFG_MASK 0x0C
#define C0_CHN_CFG_SHIFT 6
#define C1_CHN_CFG_SHIFT 4

#endif /* __LINUX_SOUND_ACS422_H */
