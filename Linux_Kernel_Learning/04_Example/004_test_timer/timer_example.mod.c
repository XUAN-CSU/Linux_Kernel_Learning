#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x058c185a, "jiffies" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x02f9bbf0, "init_timer_key" },
	{ 0x32feeafc, "mod_timer" },
	{ 0x2352b148, "timer_delete" },
	{ 0xd272d446, "__fentry__" },
	{ 0xab006604, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x058c185a,
	0xe8213e80,
	0xd272d446,
	0x02f9bbf0,
	0x32feeafc,
	0x2352b148,
	0xd272d446,
	0xab006604,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"jiffies\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"init_timer_key\0"
	"mod_timer\0"
	"timer_delete\0"
	"__fentry__\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "72A662BF41C41DC0A744EF5");
