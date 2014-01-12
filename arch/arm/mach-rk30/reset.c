#include <linux/io.h>
#include <linux/kernel.h>
#include <mach/system.h>
#include <linux/string.h>
#include <mach/cru.h>
#include <mach/iomux.h>
#include <mach/loader.h>
#include <mach/board.h>
#include <mach/pmu.h>
#include <linux/delay.h>

static void rk30_arch_reset(char mode, const char *cmd)
{
	u32 boot_flag = 0;
	u32 boot_mode = BOOT_MODE_REBOOT;

	if (cmd) {
		if (!strcmp(cmd, "loader") || !strcmp(cmd, "bootloader")) 
			boot_flag = SYS_LOADER_REBOOT_FLAG + BOOT_LOADER;
		else if(!strcmp(cmd, "recovery"))
			boot_flag = SYS_LOADER_REBOOT_FLAG + BOOT_RECOVER;
		else if (!strcmp(cmd, "charge"))
			boot_mode = BOOT_MODE_CHARGE;
	} else {
		if (system_state != SYSTEM_RESTART)
			boot_mode = BOOT_MODE_PANIC;
	}
	writel_relaxed(boot_flag, RK30_PMU_BASE + PMU_SYS_REG0);	// for loader
	writel_relaxed(boot_mode, RK30_PMU_BASE + PMU_SYS_REG1);	// for linux
	dsb();

{	//zyf emmc reset to boot mode
	writel_relaxed(0x40000000, RK30_CRU_BASE + CRU_CLKGATES_CON(2)); // bit14
	writel_relaxed(0x10000000, RK30_CRU_BASE + CRU_CLKGATES_CON(5)); // bit12

#define RK30_EMMC_BASE RK30_IO_TO_VIRT0(RK30_EMMC_PHYS)
	writel_relaxed(0, RK30_EMMC_BASE + 0x04);// power disable
	writel_relaxed(0, RK30_EMMC_BASE + 0x78); //reset on
	dsb();
	mdelay(200);
	writel_relaxed(1, RK30_EMMC_BASE + 0x04); // power enable
	writel_relaxed(1, RK30_EMMC_BASE + 0x78); // reset off
	dsb();

	sram_printascii("reset\n");
}
	
	/* disable remap */
	writel_relaxed(1 << (12 + 16), RK30_GRF_BASE + GRF_SOC_CON0);
	/* pll enter slow mode */
	writel_relaxed(PLL_MODE_SLOW(APLL_ID) | PLL_MODE_SLOW(CPLL_ID) | PLL_MODE_SLOW(GPLL_ID), RK30_CRU_BASE + CRU_MODE_CON);
	dsb();
	writel_relaxed(0xeca8, RK30_CRU_BASE + CRU_GLB_SRST_SND);
	dsb();
}

void (*arch_reset)(char, const char *) = rk30_arch_reset;
