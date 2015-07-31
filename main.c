#include <linux/delay.h>
#include <linux/module.h>
#include <linux/init.h>

uint32_t ReadUserControl(void)
{
	uint32_t ret;
	asm ("mrs %[val], PMUSERENR_EL0"
			: [val] "=r" (ret));
	return ret;
}

static void EnableUserControl(void* unused)
{
	uint32_t new_val = 0xF;

	// Enable user access
	asm ("msr PMUSERENR_EL0, %[val]"
			:: [val] "r" (new_val));

	new_val = 0xDEADBEEF;
	asm ("mrs %[val], PMUSERENR_EL0"
			: [val] "=r" (new_val));
}

uint32_t ResetCycleCounter(void)
{
	uint32_t old_val;
	asm ("mrs %[old], PMCR_EL0"
			: [old] "=r" (old_val));

	old_val |= 0x57; // Enable Cycle & Event Counters, and reset them
	asm ("msr PMCR_EL0, %[val]"
			:: [val] "r" (old_val));

	return old_val;
}

uint64_t GetCycleCounter(void)
{
	uint64_t ret;
	asm ("mrs %[ret], PMCCNTR_EL0"
			: [ret] "=r" (ret));
	return ret;
}

static int __init cycle_module_init(void)
{
	printk("Previous control is: 0x%08x\n", ReadUserControl());
	on_each_cpu(EnableUserControl, NULL, 1);
	printk("The user control is: 0x%08x\n", ReadUserControl());

	return 0;
}

static void __exit cycle_module_exit(void)
{
	printk ("Unloading my module.\n");
	return;
}

module_init(cycle_module_init);
module_exit(cycle_module_exit);

MODULE_LICENSE("GPL");

