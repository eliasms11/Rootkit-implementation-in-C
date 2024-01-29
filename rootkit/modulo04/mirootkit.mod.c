#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x9de7765d, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xf6ba0674, "__register_chrdev" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xc5850110, "printk" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0x9b858114, "module_put" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0xa8fdfe8, "try_module_get" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "1D1BF469077E7FE4C8E0CE7");
