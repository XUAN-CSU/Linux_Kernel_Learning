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
	{ 0xd272d446, "__fentry__" },
	{ 0x67628f51, "msleep" },
	{ 0x16ab4215, "__wake_up" },
	{ 0xaef1f20d, "system_wq" },
	{ 0x49733ad6, "queue_work_on" },
	{ 0x7851be11, "__SCT__might_resched" },
	{ 0x7a5ffe84, "init_wait_entry" },
	{ 0xd272d446, "schedule" },
	{ 0x0db8d68d, "prepare_to_wait_event" },
	{ 0xc87f4bab, "finish_wait" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xab006604, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xd272d446,
	0x67628f51,
	0x16ab4215,
	0xaef1f20d,
	0x49733ad6,
	0x7851be11,
	0x7a5ffe84,
	0xd272d446,
	0x0db8d68d,
	0xc87f4bab,
	0xd272d446,
	0xe8213e80,
	0xd272d446,
	0xab006604,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__fentry__\0"
	"msleep\0"
	"__wake_up\0"
	"system_wq\0"
	"queue_work_on\0"
	"__SCT__might_resched\0"
	"init_wait_entry\0"
	"schedule\0"
	"prepare_to_wait_event\0"
	"finish_wait\0"
	"__stack_chk_fail\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B20469EC822CB1DC15993D1");
