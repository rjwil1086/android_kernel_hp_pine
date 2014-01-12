/*
 * asc42201.c  --  ACS422 ALSA SoC Audio driver
 *
 * Author Vitaliy Kulikov <Vitaliy.Kulikov@idt.com>
 * Copyright (C) 2010 Integrated Device Technology, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#define DEBUG 1
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <sound/acs422.h>
#include "acs422x.h"
#if defined(ROCKCHIP)
#include "tpa6130a2.h"
#endif


static struct acs422_cache_range {
	int r_start;
	int r_end;
} acs422_cache_ranges[ACS422_CACHE_RANGES_NUM] = {
	{0, 0x39},
	{0x41, ACS422_MAX_CACHED_REG},
};

/*
 * device at index 1 is a "shadow"
 * and not registered with asoc
 */
struct acs422_priv *acs422_dev[2] = {0, 0};

static int dsp_def_params[] = {
		 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000,
		 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000,
		 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000,
		 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000,
		 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000,
		 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000,
		 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000,
		 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000,
		 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000,
		 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000,
		 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000,
		 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x400000,
		 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000,
		 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x000000, 0x000000, 0x000000,
		 0x400000, 0x000000, 0x000000, 0x000000, 0x000000, 0x400000, 0x000000, 0x400000,
};

static int acs422_hw_read(struct i2c_client *i2c,
				     u8 reg, u8 *val, u8 len)
{
	int ret;
	u16 addr = i2c->addr;
	u16 flags = i2c->flags;
	struct i2c_msg msg[2] = {
		{.addr=addr, .flags=flags, .len=1, .buf=&reg, .scl_rate = 100 * 1000},
		{.addr=addr, .flags=flags | I2C_M_RD, .len=len, .buf=val, .scl_rate = 100 * 1000},
	};
	
	ret = i2c_transfer(i2c->adapter, msg, 2);
	if (ret < 0) {
		dev_err(&i2c->dev, "%s: read reg error : reg 0x%02x cnt %d\n", __func__, reg, len);
		return ret;
	}
	return 0;
}

static unsigned int acs422_int_read(struct acs422_priv *acs422, unsigned int reg)
{
	u8 val, i;

	for (i = 0; i < ACS422_CACHE_RANGES_NUM; i++)
		if (reg >= acs422_cache_ranges[i].r_start
				&& reg <= acs422_cache_ranges[i].r_end) {
			return acs422->reg_app_cache[reg];
		}
	acs422_hw_read(acs422->i2c, reg, &val, 1);
	return val;
}

static unsigned int acs422_read(struct snd_soc_codec *codec, unsigned int reg)
{
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	return acs422_int_read(acs422, reg);
}

/*
 * first byte of the buf must contain register address
 */
static int acs422_hw_write(struct i2c_client *i2c,
			  u8 *buf, u8 len)
{
	int ret;
	u16 addr = i2c->addr;
	u16 flags = i2c->flags;
	struct i2c_msg msg = { .addr=addr, .flags=flags,
			.len=len, .buf=buf, .scl_rate = 100 * 1000};
		
	ret = i2c_transfer(i2c->adapter, &msg, 1);
	if (ret < 0) {
		dev_err(&i2c->dev, "%s: write reg error : reg 0x%02x cnt 0x%02x\n",
		       __func__, buf[0], len);
		return ret;
	}
	dev_dbg(&i2c->dev, "W r %02x v %02x%s\n", buf[0], buf[1], len > 2?"...":"");

	return 0;
}

/*static int acs422_wp_write(struct acs422_priv *acs422,
					    unsigned int reg, unsigned int val, unsigned int wp_mask)
{
	u8 buf[2];

	buf[0] = reg;
	acs422->reg_cache[reg + 1] = buf[1] = val;
	acs422->reg_wp_mask[reg] = wp_mask;
	return acs422_hw_write(acs422->i2c, buf, 2);
}*/

static int acs422_int_write(struct acs422_priv *acs422,
					    unsigned int reg, unsigned int val)
{
	u8 wp_msk, oldval, buf[2];

	acs422->reg_app_cache[reg] = val;

	buf[0] = reg;
	if (acs422->reg_wp_mask[reg]) {
		wp_msk = acs422->reg_wp_mask[reg];
		oldval = acs422->reg_cache[reg + 1];

		//dev_dbg(&acs422->i2c->dev, "W(m) r %02x v %02x wpm %02x\n", reg, val, wp_msk);

		val = (val & ~wp_msk) | (oldval & wp_msk);
	}
	acs422->reg_cache[reg + 1] = buf[1] = val;
	return acs422_hw_write(acs422->i2c, buf, 2);
}

static int acs422_write(struct snd_soc_codec *codec,
					    unsigned int reg, unsigned int val)
{
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	int ret;

	/* write to "shadow" device if any*/
	if (acs422_dev[1] && acs422 != acs422_dev[1]) {
		acs422_int_write(acs422_dev[1], reg, val);
	}
	ret = acs422_int_write(acs422, reg, val);
	return ret;
}

static int acs422_init_regs(struct acs422_priv *acs422)
{
	int i, rin, ret;
	struct acs422_reg_init *ri;
	u8 buf[2];

	if (acs422_dev[1] && acs422 != acs422_dev[1]) {
		acs422_init_regs(acs422_dev[1]);
	}

	if (acs422->preset_data && acs422->preset_data->init_regs) {
		ri = acs422->preset_data->init_regs;
		rin = acs422->preset_data->init_regs_num;
		for (i = 0; i < rin && ri[i].reg != -1; i++) {
			buf[0] = ri[i].reg;
			buf[1] = ri[i].val;
			ret = acs422_hw_write(acs422->i2c, buf, 2);
			if (ret < 0) {
				return ret;
			}
		}
	}
	ret = acs422_hw_read(acs422->i2c, 0, &acs422->reg_cache[1], ARRAY_SIZE(acs422->reg_cache)-1);
	if (ret >= 0) {
		for (i = 0; i < ARRAY_SIZE(acs422->reg_cache)-1; i++)
			acs422->reg_app_cache[i] = acs422->reg_cache[i+1];
	}
	return ret;
}

static int acs422_restore_regs(struct acs422_priv *acs422)
{
	int i;
	u8 *rcache = acs422->reg_cache;

	if (acs422_dev[1] && acs422 != acs422_dev[1]) {
		acs422_restore_regs(acs422_dev[1]);
	}

	for (i = 0; i < ACS422_CACHE_RANGES_NUM; i++) {
		rcache[acs422_cache_ranges[i].r_start] =
					acs422_cache_ranges[i].r_start;
			acs422_hw_write(acs422->i2c, &rcache[acs422_cache_ranges[i].r_start],
					acs422_cache_ranges[i].r_end - acs422_cache_ranges[i].r_start + 2);
	}
	return 0;
}


static int acs422_dsp_write(struct i2c_client *i2c,
		unsigned int dsp_addr, unsigned int *val, unsigned int num)
{
	int ret, i, j;
	u8 buf[ACS422_DSPREG_SIZE + 1];

	if (num == 0)
		return 0;

	buf[0] = ACS422_DSPRAM_ADR;
	buf[1] = dsp_addr;
	ret = acs422_hw_write(i2c, buf, 2);
	if (ret < 0) {
		return ret;
	}

	buf[0] = ACS422_WDSPRAM;
	for (i = 0; i < num; i++) {
		for (j = 0; j < ACS422_DSPREG_SIZE; j++)
			buf[j + 1] = (val[i] >> j * 8) & 0xff;

		ret = acs422_hw_write(i2c, buf, ACS422_DSPREG_SIZE + 1);
		if (ret < 0) {
			dev_err(&i2c->dev, "write error at dsp ram %d v 0x%02x\n",
			       i, val[i]);
			return ret;
		}
	}
	//dev_dbg(&i2c->dev, "%d DSP parameters loaded\n", i);
	return 0;
}

static int acs422_verify_dsp(struct acs422_priv *acs422, int *dsp_ram, int sz)
{
#if defined(DEBUG_DSP)
	int i, j;
	u8 buf[ACS422_DSPREG_SIZE];
	u32 dsp_ram_hw;

	dev_dbg(&acs422->i2c->dev, "validating dsp ram...\n");
	buf[0] = ACS422_DSPRAM_ADR;
	buf[1] = offs;
	acs422_hw_write(acs422->i2c, buf, 2);
	for (i = offs; i < ACS422_DSP_SZ && i < sz; i++) {
		acs422_hw_read(acs422->i2c, ACS422_RDSPRAM, buf, ACS422_DSPREG_SIZE);
		dsp_ram_hw = 0;
		for (j = 0; j < ACS422_DSPREG_SIZE; j++)
			dsp_ram_hw |= (buf[j] << j * 8);
		if ((dsp_ram[i] & 0x00ffffff) != (dsp_ram_hw & 0x00ffffff)) {
			dev_err(&acs422->i2c->dev, "invalid dsp ram %d - loaded %08x read %08x (%02x %02x %02x)\n", i, dsp_ram[i], dsp_ram_hw, buf[2], buf[1], buf[0]);
			return -EINVAL;
		}
	}
	dev_dbg(&acs422->i2c->dev, "dsp ram loaded OK\n");
#endif
	return 0;
}

static int acs422_valid_preset_name(struct acs422_priv *acs422, const char *name)
{
	int i;
	
	dev_dbg(&acs422->i2c->dev, "%s name - %s\n", __func__, name);

	if (acs422->preset_data && acs422->preset_data->presets) {
		for (i = 0; i < acs422->preset_data->presets_num; i++) {
			dev_dbg(&acs422->i2c->dev, "%s preset name - %s\n", __func__, acs422->preset_data->presets[i].name);

			if (!strcmp(name, acs422->preset_data->presets[i].name)) {
				return 1;
			}
		}
	}
	dev_dbg(&acs422->i2c->dev, "%s not a preset name - %s\n", __func__, name);
	return 0;
}

static int acs422_load_preset(struct acs422_priv *acs422)
{
	struct acs422_preset *prst;
	const char *prst_name;
	int i, j;
	int ret = 0;
	int dsp_offs;
	int dsp_sz = 0;
	u8 buf[2], eq_ctl = 0;

	if (!acs422->preset_data || !acs422->preset_data->presets)
		return 0;

	if (acs422_dev[1] && acs422 != acs422_dev[1]) {
		acs422_load_preset(acs422_dev[1]);
	}

	if (acs422->hp_on)
		prst_name = acs422->preset_data->hp_preset_name;
	else
		prst_name = acs422->preset_data->spk_preset_name;

	dev_dbg(&acs422->i2c->dev, "%s preset %s to load\n", __func__, prst_name);

	for (i = 0; i < acs422->preset_data->presets_num; i++) {
		prst = &acs422->preset_data->presets[i];
		if (!strcmp(prst_name, prst->name)
				&& prst->sample_rate == acs422->pbk_sr) {
			for (j = 0; j < ARRAY_SIZE(prst->preset_regs); j++)
				if (prst->preset_regs[j].reg == ACS422_EQCTRL)
					eq_ctl = prst->preset_regs[j].val;

			if (eq_ctl & ACS422_EQCTRL_EQ1_EN) {
				dsp_offs = ACS422_EQ1_OFFS;
				dsp_sz += ACS422_EQ_SZ;
			}
			if (eq_ctl & ACS422_EQCTRL_EQ0_EN) {
				dsp_offs = ACS422_EQ0_OFFS;
				dsp_sz += ACS422_EQ_SZ;
			}
			if (dsp_sz) {
				dev_dbg(&acs422->i2c->dev, "%s loading preset dsp - sz %d offs %d\n", __func__, dsp_sz, dsp_offs);
				buf[0] = ACS422_DACCTL;
				buf[1] = 0x8;
				acs422_hw_write(acs422->i2c, buf, 2);
				acs422_write(acs422->codec, ACS422_EQCTRL, 0);
				ret = acs422_dsp_write(acs422->i2c, dsp_offs, &prst->dsp_params[dsp_offs], dsp_sz);
#if defined(DEBUG_DSP)
				acs422_verify_dsp(acs422, dsp_offs, &prst->dsp_params[dsp_offs], dsp_sz);
#endif
			}
			dev_dbg(&acs422->i2c->dev, "%s loading preset regs\n", __func__);
			for (j = 0; ret == 0 && j < ARRAY_SIZE(prst->preset_regs) && prst->preset_regs[j].reg >= 0; j++) {
				ret = acs422_write(acs422->codec, prst->preset_regs[j].reg, prst->preset_regs[j].val);
			}
			buf[1] = acs422_read(acs422->codec, ACS422_DACCTL);
			acs422_hw_write(acs422->i2c, buf, 2);
			dev_dbg(&acs422->i2c->dev, "%s preset %s for %d SR loaded\n", __func__, prst_name, acs422->pbk_sr);
			return ret;
		}
	}
	dev_dbg(&acs422->i2c->dev, "%s preset %s for %d SR not found\n", __func__, prst_name, acs422->pbk_sr);
	return -EINVAL;
}

static void dump_codec(struct acs422_priv *acs422)
{
#if defined(DEBUG_REG)
	#define MAX_REGS 192
	int i, j;
	u8 val[MAX_REGS] = {0,};
#if defined(DEBUG_DSP)
	u8 buf[ACS422_DSPREG_SIZE];
	u32 dsp_ram_hw;
#endif

	if (!acs422)
		return;
	
	acs422_hw_read(acs422->i2c, 0, val, MAX_REGS);
	dev_dbg(&acs422->i2c->dev, "ACS422X %02x%02xr%02x registers dump\n", acs422->idh, acs422->idl, acs422->rev);
	printk(KERN_DEBUG "%s   ", dev_driver_string(&acs422->i2c->dev));
	for (i = 0; i < 16; i++)
		printk("   %x", i);
	printk("\n");
	for (i = 0; i < MAX_REGS/16; i++) {
		printk(KERN_DEBUG "%s %02x:", dev_driver_string(&acs422->i2c->dev), i * 16);
		for (j = 0; j < 16; j++) {
			printk("  %02x", val[i * 16 + j]);
		}
	}
	printk(KERN_DEBUG "\n");
	
	#if defined(DEBUG_DSP)
	dev_dbg(&acs422->i2c->dev, "DSP registers\n");
	buf[0] = ACS422_DSPRAM_ADR;
	buf[1] = 0;
	acs422_hw_write(acs422->i2c, buf, 2);
	printk(KERN_DEBUG "");
	for (i = 0; i < ACS422_DSP_SZ; i++) {
		acs422_hw_read(acs422->i2c, ACS422_RDSPRAM, buf, ACS422_DSPREG_SIZE);
		dsp_ram_hw = 0;
		for (j = 0; j < ACS422_DSPREG_SIZE; j++)
			dsp_ram_hw |= (buf[j] << j * 8);
		if (dsp_ram_hw & 0x800000)
			dsp_ram_hw |= 0xff000000;
		printk("0x%08x, ", dsp_ram_hw);
		if (i % 8 == 7)
			printk("\n");
	}
	printk(KERN_DEBUG "\n");
	#endif
	
#endif
}

static int acs422_snd_soc_get_vol_2r(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;

	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);

	if (reg < ARRAY_SIZE(acs422->vol_ctls) && reg2 < ARRAY_SIZE(acs422->vol_ctls)) {
		ucontrol->value.integer.value[0] =
			(acs422->vol_ctls[reg] >> shift) & mask;
		ucontrol->value.integer.value[1] =
			(acs422->vol_ctls[reg2] >> shift) & mask;
	} else {
		return -EINVAL;
	}

	if (invert) {
		ucontrol->value.integer.value[0] =
			max - ucontrol->value.integer.value[0];
		ucontrol->value.integer.value[1] =
			max - ucontrol->value.integer.value[1];
	}

	return 0;
}

static int acs422_snd_soc_put_vol_2r(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;
	int err = 0;
	unsigned int val, val2, val_mask;

	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);

	val_mask = mask << shift;
	val = (ucontrol->value.integer.value[0] & mask);
	val2 = (ucontrol->value.integer.value[1] & mask);

	if (invert) {
		val = max - val;
		val2 = max - val2;
	}

	val = val << shift;
	val2 = val2 << shift;

	if (reg < ARRAY_SIZE(acs422->vol_ctls) && reg2 < ARRAY_SIZE(acs422->vol_ctls)) {
		acs422->vol_ctls[reg] = val;
		acs422->vol_ctls[reg2] = val2;
	} else {
		return -EINVAL;
	}

	if (acs422->vsw_ctls[reg]) {
		err = snd_soc_update_bits_locked(codec, reg, val_mask, val);
		if (err < 0)
			return err;
	}

	if (acs422->vsw_ctls[reg2]) {
		err = snd_soc_update_bits_locked(codec, reg2, val_mask, val2);
	}
	return err;
}

static int acs422_snd_soc_get_sw_2r(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;

	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);

	if (reg < ARRAY_SIZE(acs422->vol_ctls) && reg2 < ARRAY_SIZE(acs422->vol_ctls)) {
		ucontrol->value.integer.value[0] =	acs422->vsw_ctls[reg];
		ucontrol->value.integer.value[1] =	acs422->vsw_ctls[reg2];
	} else {
		return -EINVAL;
	}

	return 0;
}

static int acs422_snd_soc_put_sw_2r(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned val, val2;
	int err;

	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);

	val = ucontrol->value.integer.value[0];
	val2 = ucontrol->value.integer.value[1];

	if (reg < ARRAY_SIZE(acs422->vol_ctls) && reg2 < ARRAY_SIZE(acs422->vol_ctls)) {
		if ((acs422->vsw_ctls[reg] = val)) {
			val = acs422->vol_ctls[reg];
		} else {
			val = 0;
		}
		if ((acs422->vsw_ctls[reg2] = val2)) {
			val2 = acs422->vol_ctls[reg2];
		} else {
			val2 = 0;
		}
	} else {
		return -EINVAL;
	}

	err = snd_soc_update_bits_locked(codec, reg, 0xff, val);
	if (err < 0)
		return err;
	err = snd_soc_update_bits_locked(codec, reg2, 0xff, val2);
	return err;
}

static const char *eq_ctl_text[] = {
		"OFF", "EQ1", "EQ2", "EQ1&2",
};
static const int eq_ctl_val[] = {
	0, 0xe, 0xe0, 0xee,
};
static const struct soc_enum eq_ctl =
SOC_VALUE_ENUM_SINGLE(ACS422_EQCTRL, 0, 0xff, 4, eq_ctl_text, eq_ctl_val);

/*
 * MIC boost level control
 * 0 dB to 30 dB in 10 dB steps
 */
static DECLARE_TLV_DB_MINMAX(mic_boost_tlv, 0, 3000);
/*
 * ADC level control
 * -71.25 dB to 24 dB in 0.375 dB steps
 */
static DECLARE_TLV_DB_MINMAX(adc_lvl_tlv, -7125, 2400);
/*
 * DAC volume control:
 * from -95.25 to 0 dB in 0.375 dB steps (mute instead of -95.625 dB)
 */
static DECLARE_TLV_DB_LINEAR(dac_vol_tlv, -9563, 0);
/*
 * HP volume control:
 * from -88.5 to 0 dB in 0.75 dB steps (mute instead of -89.25 dB)
 */
static DECLARE_TLV_DB_LINEAR(hp_vol_tlv, -8925, 0);
/*
 * SPK volume control:
 * from -82.5 to 3 dB in 0.75 dB steps (mute instead of -83.25 dB)
 */
static DECLARE_TLV_DB_LINEAR(spk_vol_tlv, -8325, 300);

#define SOC_DOUBLE_R_EXT(xname, reg_left, reg_right, xshift, xmax, xinvert, \
		xhandler_get, xhandler_put) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_soc_info_volsw_2r, \
	.get = xhandler_get, .put = xhandler_put, \
	.private_value = (unsigned long)&(struct soc_mixer_control) \
		{.reg = reg_left, .rreg = reg_right, .shift = xshift, \
		.max = xmax, .invert = xinvert} }

static const struct snd_kcontrol_new acs422_snd_controls[] = {
	SOC_DOUBLE_R_EXT_TLV("Master Playback Volume",
		ACS422_DACVOLL, ACS422_DACVOLR,
		0, 0xff, 0, acs422_snd_soc_get_vol_2r, acs422_snd_soc_put_vol_2r, dac_vol_tlv),
	SOC_DOUBLE_R_EXT("Master Playback Switch",
		ACS422_DACVOLL, ACS422_DACVOLR, 0, 1, 0, acs422_snd_soc_get_sw_2r, acs422_snd_soc_put_sw_2r),
	SOC_DOUBLE_R_EXT_TLV("Headphone Playback Volume",
		ACS422_HPVOLL, ACS422_HPVOLR,
		0, 0x77, 0, acs422_snd_soc_get_vol_2r, acs422_snd_soc_put_vol_2r, hp_vol_tlv),
	SOC_DOUBLE_R_EXT("Headphone Playback Switch",
		ACS422_HPVOLL, ACS422_HPVOLR, 0, 1, 0, acs422_snd_soc_get_sw_2r, acs422_snd_soc_put_sw_2r),
	SOC_DOUBLE_R_EXT_TLV("Speaker Playback Volume",
		ACS422_SPKVOLL, ACS422_SPKVOLR,
		0, 0x73, 0, acs422_snd_soc_get_vol_2r, acs422_snd_soc_put_vol_2r, spk_vol_tlv),
	SOC_DOUBLE_R_EXT("Speaker Playback Switch",
		ACS422_SPKVOLL, ACS422_SPKVOLR, 0, 1, 0, acs422_snd_soc_get_sw_2r, acs422_snd_soc_put_sw_2r),
	SOC_VALUE_ENUM("EQ", eq_ctl),
	SOC_SINGLE("Bass Playback Switch",
			ACS422_FXCTRL, 1, 1, 0),
	SOC_SINGLE("Treble Playback Switch",
			ACS422_FXCTRL, 3, 1, 0),
	SOC_SINGLE("3D Playback Switch",
			ACS422_FXCTRL, 4, 1, 0),
	SOC_SINGLE("Compressor Playback Switch",
			ACS422_CLECTRL, 0, 1, 0),
	SOC_SINGLE("Limiter Playback Switch",
			ACS422_CLECTRL, 1, 1, 0),
	SOC_SINGLE("Expander Playback Switch",
			ACS422_CLECTRL, 2, 1, 0),

	SOC_DOUBLE_R_TLV("Capture Volume",
				ACS422_ADCVOLL, ACS422_ADCVOLR,
			0, 0xff, 0, adc_lvl_tlv),
	SOC_SINGLE("Capture Switch",
			ACS422_ADCCTL, 3, 1, 1),
	SOC_DOUBLE_R_TLV("MIC Boost Capture Volume",
			ACS422_ADCSIGPATHL, ACS422_ADCSIGPATHR,
			4, 3, 0, mic_boost_tlv),
};

static const char *adc_muxl_text[] = {
	"IN1L", "IN2L", "IN3L", "D2S",
};
static const struct soc_enum adc_lin_enum =
SOC_ENUM_SINGLE(ACS422_ADCSIGPATHL, ACS422_ADCSIGPATH_INSEL_SH, 4, adc_muxl_text);
static const struct snd_kcontrol_new adc_muxl =
SOC_DAPM_ENUM("Route", adc_lin_enum);

static const char *adc_muxr_text[] = {
	"IN1R", "IN2R", "IN3R", "D2S",
};
static const struct soc_enum adc_rin_enum =
SOC_ENUM_SINGLE(ACS422_ADCSIGPATHR, ACS422_ADCSIGPATH_INSEL_SH, 4, adc_muxr_text);
static const struct snd_kcontrol_new adc_muxr =
SOC_DAPM_ENUM("Route", adc_rin_enum);

static const struct snd_soc_dapm_widget acs422_dapm_widgets[] = {
	SND_SOC_DAPM_DAC("DACL", "Playback", SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_DAC("DACR", "Playback", SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_PGA("HPL", ACS422_PWRM2, 6, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HPR", ACS422_PWRM2, 5, 0, NULL, 0),
	SND_SOC_DAPM_PGA("SPKL", ACS422_PWRM2, 4, 0, NULL, 0),
	SND_SOC_DAPM_PGA("SPKR", ACS422_PWRM2, 3, 0, NULL, 0),
	SND_SOC_DAPM_OUTPUT("HPL_OUT"),
	SND_SOC_DAPM_OUTPUT("HPR_OUT"),
	SND_SOC_DAPM_OUTPUT("SPKL_OUT"),
	SND_SOC_DAPM_OUTPUT("SPKR_OUT"),

	SND_SOC_DAPM_MICBIAS("Mic Bias", ACS422_PWRM1, 1, 0),

	SND_SOC_DAPM_INPUT("INP1L"),
	SND_SOC_DAPM_INPUT("INP1R"),
	SND_SOC_DAPM_INPUT("INP2L"),
	SND_SOC_DAPM_INPUT("INP2R"),
	SND_SOC_DAPM_INPUT("INP3L"),
	SND_SOC_DAPM_INPUT("INP3R"),
	SND_SOC_DAPM_INPUT("D2S_IN"),
	SND_SOC_DAPM_MUX("Left Capture Route", SND_SOC_NOPM, 0, 0, &adc_muxl),
	SND_SOC_DAPM_MUX("Right Capture Route", SND_SOC_NOPM, 0, 0, &adc_muxr),
	SND_SOC_DAPM_PGA("D2S PGA", ACS422_PWRM2, 7, 0, NULL, 0),
	SND_SOC_DAPM_PGA("BSTL", ACS422_PWRM1, 7, 0, NULL, 0),
	SND_SOC_DAPM_PGA("BSTR", ACS422_PWRM1, 6, 0, NULL, 0),
	SND_SOC_DAPM_PGA("ALCL", ACS422_PWRM1, 5, 0, NULL, 0),
	SND_SOC_DAPM_PGA("ALCR", ACS422_PWRM1, 4, 0, NULL, 0),
	SND_SOC_DAPM_ADC("ADCL", "Capture", ACS422_PWRM1, 3, 0),
	SND_SOC_DAPM_ADC("ADCR", "Capture", ACS422_PWRM1, 2, 0),
};

static const struct snd_soc_dapm_route audio_map[] = {
	{"SPKL", NULL, "DACL"},
	{"SPKR", NULL, "DACR"},
	{"HPL", NULL, "DACL"},
	{"HPR", NULL, "DACR"},
	{"SPKL_OUT", NULL, "SPKL"},
	{"SPKR_OUT", NULL, "SPKR"},
	{"HPL_OUT", NULL, "HPL"},
	{"HPR_OUT", NULL, "HPR"},

	{"D2S PGA", NULL, "D2S_IN"},
	{"Left Capture Route", "IN1L", "INP1L"},
	{"Left Capture Route", "IN2L", "INP2L"},
	{"Left Capture Route", "IN3L", "INP3L"},
	{"Left Capture Route", "D2S", "D2S PGA"},
	{"Right Capture Route", "IN1R", "INP1R"},
	{"Right Capture Route", "IN2R", "INP2R"},
	{"Right Capture Route", "IN3R", "INP3R"},
	{"Right Capture Route", "D2S", "D2S PGA"},
	{"BSTL", NULL, "Left Capture Route"},
	{"BSTR", NULL, "Right Capture Route"},
	{"ALCL", NULL, "BSTL"},
	{"ALCR", NULL, "BSTR"},
	{"ADCL", NULL, "ALCL"},
	{"ADCR", NULL, "ALCR"},
};

static int acs422_add_widgets(struct snd_soc_codec *codec)
{
	snd_soc_dapm_new_controls(&codec->dapm, acs422_dapm_widgets,
				  ARRAY_SIZE(acs422_dapm_widgets));
	snd_soc_dapm_add_routes(&codec->dapm, audio_map, ARRAY_SIZE(audio_map));

	return 0;
}

static int acs422_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	u8 aictrl;

	dev_dbg(codec->dev, "%s fmt 0x%x\n", __func__, fmt);

	aictrl = acs422_read(codec, ACS422_AICTL);
	aictrl &= ACS422_AICTRL_WL_MASK;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		acs422->master = 0;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		aictrl |= ACS422_AICTRL_MASTER;
		acs422->master = 1;
		break;
	default:
		dev_err(codec->dev, "invalid clock mode\n");
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		aictrl |= ACS422_AICTRL_I2S;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		aictrl |= ACS422_AICTRL_RJ;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		aictrl |= ACS422_AICTRL_LJ;
		break;
	default:
		dev_err(codec->dev, "invalid DAI mode\n");
		return -EINVAL;
	}

	/* Clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_NB_IF:
		aictrl |= ACS422_AICTRL_LRCLKINV;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		aictrl |= ACS422_AICTRL_BCLKINV;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		aictrl |= ACS422_AICTRL_BCLKINV | ACS422_AICTRL_LRCLKINV;
		break;
	default:
		dev_err(codec->dev, "invalid clock inversion\n");
		return -EINVAL;
	}

	acs422_write(codec, ACS422_AICTL, aictrl);
	return 0;
}

static int acs422_set_dai_clk(struct snd_soc_dai *dai,
				   int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	u8 pllctrl;

	dev_dbg(codec->dev, "%s id %d frq %u dir %d\n", __func__, clk_id, freq, dir);

	if (clk_id == ACS422_MCLK) {
		acs422->clk_freq = freq;
		acs422->clk_dir = dir;
		if (dir == SND_SOC_CLOCK_OUT) {
			pllctrl = acs422_read(codec, ACS422_PLLCTL1D);
			switch (freq) {
			case 0:
				break;
			case 24576000:
				pllctrl |= ACS422_PLLCTL1D_22_24_SEL;
				break;
			case 22579200:
				pllctrl &= ~ACS422_PLLCTL1D_22_24_SEL;
				break;
			default:
				dev_err(codec->dev, "unsupported clock frequency %d\n", freq);
				return -EINVAL;
			}
			acs422_write(codec, ACS422_PLLCTL1D, pllctrl);
		}
	} else {
		dev_err(codec->dev, "invalid clock id %d\n", clk_id);
		return -EINVAL;
	}
	return 0;
}

static int acs422_hw_params(struct snd_pcm_substream *substream,
				  struct snd_pcm_hw_params *params,
				  struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	u8 aictrl, aictrl2, srctrl, srctrl_reg;
	int srate = params_rate(params);

	dev_dbg(codec->dev, "%s strm %d sr %d chn %d fmt %x\n", __func__, 
		substream->stream, srate, params_channels(params), params_format(params));

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK
			&& acs422->pbk_sr != srate) {
		mutex_lock(&acs422->mutex);
		acs422->pbk_sr = srate;
		if (acs422_dev[1])
			acs422_dev[1]->pbk_sr = srate;
		acs422_load_preset(acs422);
		mutex_unlock(&acs422->mutex);
	}
	
	/*
	 * if codec is clock slave and mclk frequency is provided as 0,
	 * assume that external mclk frequency adjusted dynamically
	 * based on sample rate, so no internal adjustment is required
	 */
	if (acs422->clk_dir == SND_SOC_CLOCK_OUT ||
			(acs422->clk_freq != 0 && acs422->clk_freq % srate != 0)) {
		srctrl = 0;
		switch (srate) {
		case 8000:
			srctrl |= ACS422_SR_32000 | ACS422_SR_MUL_025;
			break;
		case 16000:
			srctrl |= ACS422_SR_32000 | ACS422_SR_MUL_050;
			break;
		case 32000:
			srctrl |= ACS422_SR_32000 | ACS422_SR_MUL_1;
			break;
		case 64000:
			srctrl |= ACS422_SR_32000 | ACS422_SR_MUL_2;
			break;
		case 11025:
			srctrl |= ACS422_SR_44100 | ACS422_SR_MUL_025;
			break;
		case 22050:
			srctrl |= ACS422_SR_44100 | ACS422_SR_MUL_050;
			break;
		case 44100:
			srctrl |= ACS422_SR_44100 | ACS422_SR_MUL_1;
			break;
		case 88200:
			srctrl |= ACS422_SR_44100 | ACS422_SR_MUL_2;
			break;
		case 24000:
			srctrl |= ACS422_SR_48000 | ACS422_SR_MUL_050;
			break;
		case 48000:
			srctrl |= ACS422_SR_48000 | ACS422_SR_MUL_1;
			break;
		case 96000:
			srctrl |= ACS422_SR_48000 | ACS422_SR_MUL_2;
			break;
		default:
			dev_err(codec->dev, "invalid rate %u\n", params_rate(params));
			return -EINVAL;
		}
		aictrl2 = acs422_read(codec, ACS422_AICTRL2);
		if ((aictrl2 & ACS422_AICTRL2_BLRCM_MASK) > 1) {
			snd_soc_update_bits(codec, ACS422_DACSR, ACS422_SR_MASK, srctrl);
			snd_soc_update_bits(codec, ACS422_ADCSR, ACS422_SR_MASK, srctrl);
		} else {
			srctrl_reg = substream->stream == SNDRV_PCM_STREAM_CAPTURE?
					ACS422_ADCSR : ACS422_DACSR;
			snd_soc_update_bits(codec, srctrl_reg, ACS422_SR_MASK, srctrl);
		}
	}

	aictrl = acs422_read(codec, ACS422_AICTL);
	aictrl &= ~ACS422_AICTRL_WL_MASK;

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		aictrl |= ACS422_AICTRL_16BIT;
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		aictrl |= ACS422_AICTRL_20BIT;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		aictrl |= ACS422_AICTRL_24BIT;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		aictrl |= ACS422_AICTRL_32BIT;
		break;
	default:
		dev_err(codec->dev, "invalid format %x\n", params_format(params));
		return -EINVAL;
	}
	acs422_write(codec, ACS422_AICTL, aictrl);

	return 0;
}

static int acs422_startup(struct snd_pcm_substream *substream,
	struct snd_soc_dai *dai)
{	
	struct snd_soc_codec *codec = dai->codec;
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	
	dev_dbg(&acs422->i2c->dev, "%s: strm is %s \n",__FUNCTION__,
		   substream->stream == SNDRV_PCM_STREAM_PLAYBACK ? "playback":"capture");
	
	dev_dbg(&acs422->i2c->dev, "%s: hp state is 0x%x \n",__FUNCTION__,
		   acs422->hp_on);

	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
		tpa6130a2_stereo_enable(NULL, 1);
	}
	return 0;
}

static void acs422_shutdown(struct snd_pcm_substream *substream,
	struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);

	dev_dbg(&acs422->i2c->dev, "%s: strm is %s \n",__FUNCTION__,
		   substream->stream == SNDRV_PCM_STREAM_PLAYBACK ? "playback":"capture");
	
	dev_dbg(&acs422->i2c->dev, "%s: hp state is 0x%x \n",__FUNCTION__,
		   acs422->hp_on);
	
	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
		tpa6130a2_stereo_enable(NULL, 0);
	}
	return;
}

static void acs422_off(struct snd_soc_codec *codec)
{
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	u8 buf[2];

	snd_soc_update_bits(codec, ACS422_PWRM2, 0xFE, 0);
	snd_soc_update_bits(codec, ACS422_PWRM1, 0xFE, 0);
	msleep(50);
	snd_soc_update_bits(codec, ACS422_PWRM2, 0x01, 0);

	buf[0] = 0x15;
	buf[1] = 0x3f;
	acs422_hw_write(codec->control_data, buf, 2);
	if (acs422_dev[1])
		acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

	buf[0] = 0x1f;
	buf[1] = 0x00;
	acs422_hw_write(codec->control_data, buf, 2);
	if (acs422_dev[1])
		acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

	buf[0] = 0x61;
	buf[1] = 0x00;
	acs422_hw_write(codec->control_data, buf, 2);
	if (acs422_dev[1])
		acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

	buf[0] = 0x63;
	buf[1] = 0x00;
	acs422_hw_write(codec->control_data, buf, 2);
	if (acs422_dev[1])
		acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

	buf[0] = 0x22;
	buf[1] = 0x00;
	acs422_hw_write(codec->control_data, buf, 2);
	if (acs422_dev[1])
		acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

	buf[0] = 0x24;
	buf[1] = 0x00;
	acs422_hw_write(codec->control_data, buf, 2);
	if (acs422_dev[1])
		acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

	buf[0] = 0x42;
	buf[1] = 0x00;
	acs422_hw_write(codec->control_data, buf, 2);
	if (acs422_dev[1])
		acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

	snd_soc_update_bits(codec, ACS422_PWRM1, 0x01, 0x01);

	buf[0] = ACS422_PWRM1;
	buf[1] = 0x1;
	acs422_hw_write(codec->control_data, buf, 2);
	acs422->off = 1;
	
	dump_codec(acs422_dev[0]);
	dump_codec(acs422_dev[1]);

	return;
}

static void acs422_on(struct snd_soc_codec *codec)
{
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	u8 buf[2];

	if (acs422->off) {
		snd_soc_update_bits(codec, ACS422_PWRM1, 0x01, 0);

		buf[0] = 0x15;
		buf[1] = acs422_read(codec, buf[0]);
		acs422_hw_write(codec->control_data, buf, 2);
		if (acs422_dev[1])
			acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

		buf[0] = 0x1f;
		buf[1] = acs422_read(codec, buf[0]);
		acs422_hw_write(codec->control_data, buf, 2);
		if (acs422_dev[1])
			acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

		buf[0] = 0x63;
		buf[1] = acs422_read(codec, buf[0]);
		acs422_hw_write(codec->control_data, buf, 2);
		if (acs422_dev[1])
			acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

		buf[0] = 0x22;
		buf[1] = acs422_read(codec, buf[0]);
		acs422_hw_write(codec->control_data, buf, 2);
		if (acs422_dev[1])
			acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

		buf[0] = 0x24;
		buf[1] = acs422_read(codec, buf[0]);
		acs422_hw_write(codec->control_data, buf, 2);
		if (acs422_dev[1])
			acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

		buf[0] = 0x42;
		buf[1] = acs422_read(codec, buf[0]);
		acs422_hw_write(codec->control_data, buf, 2);
		if (acs422_dev[1])
			acs422_hw_write(acs422_dev[1]->i2c, buf, 2);

		snd_soc_update_bits(codec, ACS422_PWRM2, 0x01, 0x01);
		msleep(50);
		snd_soc_update_bits(codec, ACS422_PWRM2, 0x78, 0x78);
		snd_soc_update_bits(codec, ACS422_PWRM1, 0xFE, 0xFE);

		acs422->off = 0;
	}
	return;
}

static int acs422_set_bias_level(struct snd_soc_codec *codec,
				   enum snd_soc_bias_level level)
{
	if (codec->dapm.bias_level != level) {
		switch (level) {
		case SND_SOC_BIAS_OFF:
			dev_dbg(codec->dev, "%s off\n", __func__);
			acs422_off(codec);
			break;

		case SND_SOC_BIAS_STANDBY:
			dev_dbg(codec->dev, "%s standby\n", __func__);
			//if (codec->dapm.bias_level == SND_SOC_BIAS_PREPARE)
			//	acs422_off(codec);
			break;

		case SND_SOC_BIAS_PREPARE:
			dev_dbg(codec->dev, "%s prepare\n", __func__);
			if (codec->dapm.bias_level == SND_SOC_BIAS_STANDBY) {
				acs422_on(codec);
				snd_soc_update_bits(codec, 0x68, 0x80, 0x80);
			} else
				snd_soc_update_bits(codec, 0x68, 0x80, 0);
			break;

		case SND_SOC_BIAS_ON:
			dev_dbg(codec->dev, "%s on\n", __func__);
			break;
		}
		codec->dapm.bias_level = level;
	}
	return 0;
}

static int acs422_digital_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);

	dev_dbg(codec->dev, "%s mute %d\n", __func__, mute);

	if (mute) {
		tpa6130a2_stereo_enable(NULL, 0);
		mdelay(1);
		snd_soc_update_bits(codec, 0x68, 0x80, 0);
		if (!acs422->master) {
			snd_soc_update_bits(codec, ACS422_PWRM2,
					0x78, 0);
		}
		snd_soc_update_bits(codec, ACS422_DACCTL, 8, 8);
	} else {
		if (dai->playback_active) {
			if (!acs422->master) {
				snd_soc_update_bits(codec, 0x68, 0x80, 0x80);
				snd_soc_update_bits(codec, ACS422_PWRM2,
					0x78, 0x78);
			}
			snd_soc_update_bits(codec, ACS422_DACCTL, 8, 0);
			mdelay(8);
			tpa6130a2_stereo_enable(NULL, 1);
		} else {
			dev_dbg(codec->dev, "no playback to unmute\n");
		}
		
		dump_codec(acs422_dev[0]);
		dump_codec(acs422_dev[1]);
	}
	return 0;
}

#define ACS422_RATES SNDRV_PCM_RATE_8000_96000

#define ACS422_FORMATS (SNDRV_PCM_FMTBIT_S16_LE |\
			SNDRV_PCM_FMTBIT_S20_3LE |\
			SNDRV_PCM_FMTBIT_S24_LE | \
			SNDRV_PCM_FMTBIT_S32_LE)

struct snd_soc_dai_ops acs422_dai_ops = {
	.digital_mute = acs422_digital_mute,
	.hw_params = acs422_hw_params,
	.set_fmt = acs422_set_dai_fmt,
	.set_sysclk = acs422_set_dai_clk,
	.startup = acs422_startup,
	.shutdown = acs422_shutdown,
};

struct snd_soc_dai_driver acs422_dai [] = {
{
	.name = "ACS422 HiFi",
	.playback = {
		     .stream_name = "Playback",
		     .channels_min = 2,
		     .channels_max = 2,
		     .rates = ACS422_RATES,
		     .formats = ACS422_FORMATS,
		     },
	.capture = {
		    .stream_name = "Capture",
		    .channels_min = 2,
		    .channels_max = 2,
		    .rates = ACS422_RATES,
		    .formats = ACS422_FORMATS,
		    },
	.ops = &acs422_dai_ops,
},
};
EXPORT_SYMBOL_GPL(acs422_dai);


static void det_function(struct work_struct *work);
static irqreturn_t det_handler(int irq, void *dev_id);
static DECLARE_DELAYED_WORK(det_work, det_function);
extern void rk_headset_report(int state);
#define BIT_HEADSET_NO             (0 << 0)
#define BIT_HEADSET_NO_MIC      (1 << 1)

static int acs422_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	dev_dbg(codec->dev, "%s\n", __func__);
	acs422_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int acs422_resume(struct snd_soc_codec *codec)
{
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);

	dev_dbg(codec->dev, "%s\n", __func__);
	acs422_restore_regs(acs422);
	//mutex_lock(&acs422->mutex);
	//acs422_load_preset(acs422);
	//mutex_unlock(&acs422->mutex);
	schedule_delayed_work(&det_work, msecs_to_jiffies(10));
	if (codec->dapm.idle_bias_off == 0)
		acs422_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	return 0;
}

static u8 spkr_rotation_state = 0x00;

int acs422_rotate_spkr_channels(u8 chn_cfg)
{
	int ret;
	u8 val;

	spkr_rotation_state = chn_cfg;

	if (!acs422_dev[0]
	    || !acs422_dev[0]->codec)
		return 0;

	dev_dbg(&acs422_dev[0]->i2c->dev, "%s cfg %02x\n", __func__, chn_cfg);

	mutex_lock(&acs422_dev[0]->codec->mutex);

	val = acs422_int_read(acs422_dev[0], ACS422_AICTRL2);
	val &= ~(C0_CHN_CFG_MASK << C0_CHN_CFG_SHIFT);
	val |= (chn_cfg & C0_CHN_CFG_MASK) << C0_CHN_CFG_SHIFT;
	ret = acs422_int_write(acs422_dev[0], ACS422_AICTRL2, val);

	if (ret < 0 || !acs422_dev[1])
		goto ext;

	val = acs422_int_read(acs422_dev[1], ACS422_AICTRL2);
	val &= ~(C1_CHN_CFG_MASK << C1_CHN_CFG_SHIFT);
	val |= (chn_cfg & C1_CHN_CFG_MASK) << C1_CHN_CFG_SHIFT;
	ret = acs422_int_write(acs422_dev[1], ACS422_AICTRL2, val);

ext:
	mutex_unlock(&acs422_dev[0]->codec->mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(acs422_rotate_spkr_channels);

int acs422_hp_event(struct snd_soc_codec *codec, int jck_stat)
{
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	u8 b3d_mask = ACS422_FXCTRL_BASS_EN | ACS422_FXCTRL_3D_EN;
	u8 hp_det_stat = 0;
	u8 hp_det_ctrl = 0;
	int hp_det_enabled;

	dev_dbg(codec->dev, "%s %d\n", __func__, jck_stat);
	hp_det_ctrl = acs422_read(codec, ACS422_HPDETECT);
	hp_det_enabled = hp_det_ctrl & ACS422_HPDETECT_HPSWEN;

	if (jck_stat < 0) {
		if (hp_det_enabled) {
			acs422_hw_read(codec->control_data, ACS422_HPDETSTAT, &hp_det_stat, 1);
			if (hp_det_ctrl & ACS422_HPDETECT_HPSWPOL)
				hp_det_stat ^= ACS422_HPDETSTAT_HPDET;
			jck_stat = (hp_det_stat & ACS422_HPDETSTAT_HPDET) != 0;
		}
	}
	if (jck_stat >= 0) {
		if (acs422->hp_on != jck_stat) {
			mutex_lock(&acs422->mutex);
			acs422->hp_on = jck_stat;
			if (!hp_det_enabled) 
			{
				if (jck_stat) {
					tpa6130a2_stereo_enable(NULL, 1);
					acs422_write(codec, ACS422_SPKVOLL, 0);
					acs422_write(codec, ACS422_SPKVOLR, 0);
					acs422_load_preset(acs422);
					acs422_write(codec, ACS422_HPVOLL, acs422->vol_ctls[ACS422_HPVOLL]);
					acs422_write(codec, ACS422_HPVOLR, acs422->vol_ctls[ACS422_HPVOLR]);
				} else {
					tpa6130a2_stereo_enable(NULL, 0);
					acs422_write(codec, ACS422_HPVOLL, 0);
					acs422_write(codec, ACS422_HPVOLR, 0);
					acs422_load_preset(acs422);
					acs422_write(codec, ACS422_SPKVOLL, acs422->vol_ctls[ACS422_SPKVOLL]);
					acs422_write(codec, ACS422_SPKVOLR, acs422->vol_ctls[ACS422_SPKVOLR]);
				}
			} 
			else {
				acs422_load_preset(acs422);
			}
			mutex_unlock(&acs422->mutex);	
		}
	}
	return 0;
}
EXPORT_SYMBOL_GPL(acs422_hp_event);

int acs422_select_input(struct snd_soc_codec *codec, int inp, int boost)
{
	int ret;
	u8 inpl, inpr;

	dev_dbg(codec->dev, "%s: input=%s, boost=%d dB\n", 
		__func__, (inp==ACS_HPMIC)? "hpmic" : "d-mic", boost);

	inpr = inpl = inp << ACS422_ADCSIGPATH_INSEL_SH
			| (boost/10) << ACS422_ADCSIGPATH_MBST_SH;
	ret = acs422_write(codec, ACS422_ADCSIGPATHL, inpl);
	ret = acs422_write(codec, ACS422_ADCSIGPATHR, inpr);
	return ret;
}
EXPORT_SYMBOL_GPL(acs422_select_input);

static void det_function(struct work_struct *work)
{
	struct snd_soc_codec * codec;
	struct acs422_priv *acs422;
	unsigned long flags;
	int hp_gpio,irq;
	int hp_in_level;
	int state = BIT_HEADSET_NO;
	int ret;

#if defined(CONFIG_MACH_RK30_T7H_DVT)
	hp_in_level = GPIO_LOW;
#else
	hp_in_level = GPIO_HIGH;
#endif

	if (!acs422_dev[0]) {
		return;
	}
	acs422 = acs422_dev[0];
	codec = acs422->codec;
	
	hp_gpio = gpio_get_value(RK30_PIN0_PC7);
	if (hp_gpio == hp_in_level) {
		state |= BIT_HEADSET_NO_MIC;
	}

	rk_headset_report(state);
	msleep(220);

	if (hp_gpio == hp_in_level) {
		acs422_hp_event(codec, 1);
	} else {
		acs422_hp_event(codec, 0);
	}
	acs422_select_input(codec, ACS_D_MIC, 30);
	irq = gpio_to_irq(RK30_PIN0_PC7);
	if (acs422->mdet_initalized)
		free_irq(irq, NULL);
	
	if (hp_gpio != gpio_get_value(RK30_PIN0_PC7)) {
		schedule_delayed_work(&det_work, msecs_to_jiffies(180));
		acs422->mdet_initalized = false;
		dev_dbg(codec->dev, "Plug operation happened\n");
		return;
	}
	
	flags = (hp_gpio == GPIO_HIGH)?(IRQF_TRIGGER_FALLING) : (IRQF_TRIGGER_RISING);
	ret = request_irq(irq, det_handler, flags, "pdet", NULL);
	if (ret < 0) {
		dev_err(codec->dev, "request_irq(%d) failed\n", irq);
	}
	acs422->mdet_initalized = true;
	
	return;
}

static irqreturn_t det_handler(int irq, void *dev_id)
{
	pr_debug("%s\n", __func__);
	schedule_delayed_work(&det_work, msecs_to_jiffies(1000));
	return IRQ_HANDLED;
}

static void det_initalize(void)
{
	struct acs422_priv *acs422;
	
	if (!acs422_dev[0]) {
		return;
	}
	acs422 = acs422_dev[0];
	acs422->mdet_initalized = false;
	schedule_delayed_work(&det_work, msecs_to_jiffies(10));
	return;
}

static ssize_t acs422_spk_prst_show(struct device *dev,
			  struct device_attribute *attr,
			  char *buf)
{
	struct acs422_priv *acs422 = dev_get_drvdata(dev);
	size_t len = 0;
	if (acs422->preset_data) {
		len = snprintf(buf, sizeof(acs422->preset_data->spk_preset_name),
					"%s\n", acs422->preset_data->spk_preset_name);
	}
	return len;
}

static ssize_t acs422_spk_prst_set(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct acs422_priv *acs422 = dev_get_drvdata(dev);
	size_t sz;
	char *cr;

	dev_dbg(dev, "%s hp_on=%d, buf=%s", __func__, acs422->hp_on, buf);

	if (acs422->preset_data) {
		cr = strnchr(buf, count, '\n');
	if (cr)
		*cr = 0;

		if (acs422_valid_preset_name(acs422, buf)) {
			mutex_lock(&acs422->mutex);
			if (strcmp(acs422->preset_data->spk_preset_name, buf)) {
				sz = min(count, sizeof(acs422->preset_data->spk_preset_name));
				strncpy(acs422->preset_data->spk_preset_name, buf, sz);
				if (!acs422->hp_on)
					acs422_load_preset(acs422);
			}
			mutex_unlock(&acs422->mutex);
		}
	} else
		dev_dbg(dev, "%s no presets found\n", __func__);

	return count;
}

static ssize_t acs422_hp_prst_show(struct device *dev,
			  struct device_attribute *attr,
			  char *buf)
{
	struct acs422_priv *acs422 = dev_get_drvdata(dev);
	size_t len = 0;
	if (acs422->preset_data) {
		len = snprintf(buf, sizeof(acs422->preset_data->hp_preset_name),
					"%s\n", acs422->preset_data->hp_preset_name);
	}
	return len;
}

static ssize_t acs422_hp_prst_set(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct acs422_priv *acs422 = dev_get_drvdata(dev);
	size_t sz;
	char *cr;

	dev_dbg(dev, "%s hp_on=%d, buf=%s", __func__, acs422->hp_on, buf);

	if (acs422->preset_data) {
		cr = strnchr(buf, count, '\n');
	if (cr)
		*cr = 0;
		if (acs422_valid_preset_name(acs422, buf)) {
			mutex_lock(&acs422->mutex);
			if (strcmp(acs422->preset_data->hp_preset_name, buf)) {
				sz = min(count, sizeof(acs422->preset_data->hp_preset_name));
				strncpy(acs422->preset_data->hp_preset_name, buf, sz);
				if (acs422->hp_on)
					acs422_load_preset(acs422);
			}
			mutex_unlock(&acs422->mutex);
		}
	} else
		dev_dbg(dev, "%s no presets found\n", __func__);

	return count;
}

static DEVICE_ATTR(acs422_spk_preset, 0664, acs422_spk_prst_show, acs422_spk_prst_set);
static DEVICE_ATTR(acs422_hp_preset, 0664, acs422_hp_prst_show, acs422_hp_prst_set);

static int acs422_probe(struct snd_soc_codec *codec)
{
	struct acs422_priv *acs422 = snd_soc_codec_get_drvdata(codec);
	unsigned int i;
	int ret;
	u8 buf[2];

	dev_dbg(&acs422->i2c->dev, "%s codec %s\n", __func__, codec->name);
	
	mutex_init(&acs422->mutex);
	
	acs422_hw_read(acs422->i2c, ACS422_DEVIDL, &acs422->idl, 1);
	acs422_hw_read(acs422->i2c, ACS422_DEVIDH, &acs422->idh, 1);
	if (acs422->idh != ACS322_PARTID_H &&
			acs422->idh != ACS422_PARTID_H) {
		dev_err(&acs422->i2c->dev, "device ID %02x%02x is not of ACS422/ACS322\n", acs422->idh, acs422->idl);
		//return -ENODEV;
	}
	
	acs422_hw_read(acs422->i2c, ACS422_REVID, &acs422->rev, 1);
	dev_info(&acs422->i2c->dev, "ACS422(%02x%02x) revision %02x\n", acs422->idh, acs422->idl, acs422->rev);
	buf[0] = ACS422_RESET;
	buf[1] = 0x85;
	acs422_hw_write(acs422->i2c, buf, 2);
	if (acs422_dev[1]) {
		acs422_hw_read(acs422_dev[1]->i2c, ACS422_DEVIDL, &acs422_dev[1]->idl, 1);
		acs422_hw_read(acs422_dev[1]->i2c, ACS422_DEVIDH, &acs422_dev[1]->idh, 1);
		if (acs422_dev[1]->idh != ACS322_PARTID_H &&
				acs422_dev[1]->idh != ACS422_PARTID_H) {
			dev_err(&acs422_dev[1]->i2c->dev, "device ID %02x%02x is not of ACS422/ACS322\n", acs422_dev[1]->idh, acs422_dev[1]->idl);
			acs422_dev[1] = 0;
		} else {
			acs422_hw_read(acs422_dev[1]->i2c, ACS422_REVID, &acs422_dev[1]->rev, 1);
			dev_info(&acs422_dev[1]->i2c->dev, "ACS422(%02x%02x) revision %02x\n", acs422_dev[1]->idh, acs422_dev[1]->idl, acs422_dev[1]->rev);
			acs422_hw_write(acs422_dev[1]->i2c, buf, 2);
		}
	}
	ret = acs422_init_regs(acs422);
	if (ret < 0) {
		dev_err(&acs422->i2c->dev, "fail to initalize device\n");
		return ret;
	}

	codec->control_data = acs422->i2c;
	acs422->codec = codec;

	for (i = 0; i < ARRAY_SIZE(acs422->vol_ctls); i++) {
		acs422->vol_ctls[i] = acs422_read(codec, i);
		if (acs422->vol_ctls[i])
			acs422->vsw_ctls[i] = 1;
	}

	acs422_set_bias_level(codec, SND_SOC_BIAS_OFF);
	//acs422_select_input(codec, ACS_D_MIC, 30);
	codec->dapm.idle_bias_off = 1;

	ret = snd_soc_add_controls(codec, acs422_snd_controls,
			     ARRAY_SIZE(acs422_snd_controls));
#if !defined(ROCKCHIP)
	if (ret == 0)
		acs422_add_widgets(codec);
#endif	
	ret = device_create_file(codec->dev, &dev_attr_acs422_spk_preset);
	if (ret == 0)
		ret = device_create_file(codec->dev, &dev_attr_acs422_hp_preset);
	if (ret == 0)
		ret = sysfs_create_link(&codec->card->dev->kobj, &codec->dev->kobj, "acs422_device");

#if defined(ROCKCHIP)
	det_initalize();
#endif
	//acs422_rotate_spkr_channels(spkr_rotation_state);
	return ret;
}

static int acs422_remove(struct snd_soc_codec *codec)
{
	acs422_set_bias_level(codec, SND_SOC_BIAS_OFF);
	sysfs_remove_link(&codec->card->dev->kobj, "acs422_device");
	device_remove_file(codec->dev, &dev_attr_acs422_spk_preset);
	device_remove_file(codec->dev, &dev_attr_acs422_hp_preset);
	return 0;
}

struct snd_soc_codec_driver soc_codec_dev_acs422 = {
	.probe = acs422_probe,
	.remove = acs422_remove,
	.suspend = acs422_suspend,
	.resume = acs422_resume,
	.read = acs422_read,
	.write = acs422_write,
    .set_bias_level = acs422_set_bias_level,
	.reg_cache_size = ACS422_MAX_CACHED_REG,
	.reg_word_size = sizeof(u8),
	.reg_cache_step = 1,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_acs422);

static const struct i2c_device_id acs422_id[] = {
	{"ACS422X", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, acs422_id);

static __devinit int acs422_i2c_probe(struct i2c_client *i2c,
					const struct i2c_device_id *id)
{
	struct acs422_priv *acs422;
	int ret;

	acs422 = kzalloc(sizeof(struct acs422_priv), GFP_KERNEL);
	if (acs422 == NULL) {
		ret = -ENOMEM;
		goto ext;
	}
	
	acs422->preset_data = (struct acs422_platform_data*)i2c->dev.platform_data;
	acs422->i2c = i2c;
	i2c_set_clientdata(i2c, acs422);

	if (acs422->preset_data == 0 || acs422->preset_data->codec_idx == 0) {
		if (acs422_dev[0]) {
			dev_err(&i2c->dev, "primary ACS422 device already exists\n");
			ret = -EINVAL;
			goto err;
		}
		acs422_dev[0] = acs422;
		ret = snd_soc_register_codec(&i2c->dev, &soc_codec_dev_acs422,
			acs422_dai, ARRAY_SIZE(acs422_dai));
		if (ret != 0) {
			dev_err(&i2c->dev, "failed to register codec: %d\n", ret);
			goto err;
		}
	} else {
		/*"shadow" codec cannot be I2S master*/
		acs422->reg_wp_mask[ACS422_AICTL] = ACS422_AICTRL_MASTER;
		/*"shadow" codec cannot record*/
		acs422->reg_wp_mask[ACS422_DMICCTRL] = 0xFF;
		acs422->reg_wp_mask[ACS422_ADCSIGPATHL] = 0xFF;
		acs422->reg_wp_mask[ACS422_ADCSIGPATHR] = 0xFF;
		acs422->reg_wp_mask[ACS422_ADCCTL] = 0xFF;
		acs422->reg_wp_mask[ACS422_ADCSR] = 0xFF;
		acs422->reg_wp_mask[ACS422_PWRM1] = 0xFE;
		acs422->reg_wp_mask[ACS422_PWRM2] = 0xE0;
		acs422_dev[1] = acs422;
	}
	
	return 0;
err:
	kfree(acs422);
ext:
	return ret;
}

static __devexit int acs422_i2c_remove(struct i2c_client *client)
{
	struct acs422_priv *acs422;
	snd_soc_unregister_codec(&client->dev);
	acs422 = i2c_get_clientdata(client);
	if (acs422_dev[1] == acs422)
		acs422_dev[1] = 0;
	kfree(acs422);
	return 0;
}

static void acs422_i2c_shutdown(struct i2c_client *client)
{
	struct acs422_priv *acs422;
	struct snd_soc_codec *codec;

	printk("%s !!!\n",__func__);
	acs422 = i2c_get_clientdata(client);
	codec = acs422->codec;
	snd_soc_update_bits(codec, 0x68, 0x80, 0);
	acs422_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return;
}

static struct i2c_driver acs422_i2c_driver = {
	.driver = {
		   .name = "ACS422",
		   .owner = THIS_MODULE,
		   },
	.probe = acs422_i2c_probe,
	.remove = __devexit_p(acs422_i2c_remove),
	.shutdown = acs422_i2c_shutdown,
	.id_table = acs422_id,
};

static int __init acs422_init(void)
{
	return i2c_add_driver(&acs422_i2c_driver);
}

static void __exit acs422_exit(void)
{
	i2c_del_driver(&acs422_i2c_driver);
}

module_init(acs422_init);
module_exit(acs422_exit);

MODULE_AUTHOR("Vitaliy Kulikov <Vitaliy.Kulikov@idt.com>");
MODULE_DESCRIPTION("ASoC ACS422 driver");
MODULE_LICENSE("GPL");
