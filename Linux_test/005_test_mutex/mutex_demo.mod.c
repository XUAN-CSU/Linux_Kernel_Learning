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
	{ 0xe8213e80, "_printk" },
	{ 0x67628f51, "msleep" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xfc961df9, "kmalloc_caches" },
	{ 0xe18fddbc, "__kmalloc_cache_noprof" },
	{ 0xc1e6c71e, "__mutex_init" },
	{ 0x61be6fd1, "mutex_is_locked" },
	{ 0x357aaab3, "mutex_trylock" },
	{ 0xef959adb, "kthread_create_on_node" },
	{ 0x9a9aee9e, "wake_up_process" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0x8697f569, "kthread_stop" },
	{ 0xd272d446, "__fentry__" },
	{ 0xf46d5bf3, "mutex_lock" },
	{ 0x40a621c5, "snprintf" },
	{ 0xd268ca91, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xe8213e80,
	0x67628f51,
	0xf46d5bf3,
	0xd272d446,
	0xbd03ed67,
	0xfc961df9,
	0xe18fddbc,
	0xc1e6c71e,
	0x61be6fd1,
	0x357aaab3,
	0xef959adb,
	0x9a9aee9e,
	0xcb8b6ec6,
	0x8697f569,
	0xd272d446,
	0xf46d5bf3,
	0x40a621c5,
	0xd268ca91,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"_printk\0"
	"msleep\0"
	"mutex_unlock\0"
	"__x86_return_thunk\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"__mutex_init\0"
	"mutex_is_locked\0"
	"mutex_trylock\0"
	"kthread_create_on_node\0"
	"wake_up_process\0"
	"kfree\0"
	"kthread_stop\0"
	"__fentry__\0"
	"mutex_lock\0"
	"snprintf\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "CC5876AFB6F667A029BBF85");
