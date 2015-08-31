#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x78f39d41, "module_layout" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x3ce4ca6f, "disable_irq" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x735a0bd5, "native_io_delay" },
	{ 0xfcec0987, "enable_irq" },
	{ 0x2072ee9b, "request_threaded_irq" },
	{ 0x4627aa51, "__register_chrdev" },
	{ 0xf09c7f68, "__wake_up" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xbad7d2da, "interruptible_sleep_on" },
	{ 0x37ff4c06, "copy_from_user_overflow" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "4A59825FA8CDEE826D205FD");
