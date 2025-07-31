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
	{ 0x535f4f5f, "hrtimer_init" },
	{ 0x5fa07cc0, "hrtimer_start_range_ns" },
	{ 0x36a36ab1, "hrtimer_cancel" },
	{ 0xd272d446, "__fentry__" },
	{ 0xab006604, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x058c185a,
	0xe8213e80,
	0xd272d446,
	0x535f4f5f,
	0x5fa07cc0,
	0x36a36ab1,
	0xd272d446,
	0xab006604,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"jiffies\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"hrtimer_init\0"
	"hrtimer_start_range_ns\0"
	"hrtimer_cancel\0"
	"__fentry__\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "870CCA817F6599E69BDEF78");
