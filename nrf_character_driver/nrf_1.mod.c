#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x5a7ba343, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0xf7e79095, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x44fe75f2, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x97255bdf, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x7186e1a, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xb4fb14f7, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0x9dbea1af, __VMLINUX_SYMBOL_STR(spi_setup) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0xd697e69a, __VMLINUX_SYMBOL_STR(trace_hardirqs_on) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x5db5e055, __VMLINUX_SYMBOL_STR(spi_busnum_to_master) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xf3342c6e, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0xabbab73d, __VMLINUX_SYMBOL_STR(spi_new_device) },
	{ 0x35a941d7, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x4a4b67, __VMLINUX_SYMBOL_STR(spi_sync) },
	{ 0x21eba563, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x7f39a635, __VMLINUX_SYMBOL_STR(spi_write_then_read) },
	{ 0x40000904, __VMLINUX_SYMBOL_STR(param_array_ops) },
	{ 0x264586be, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xcfe90fbb, __VMLINUX_SYMBOL_STR(device_unregister) },
	{ 0xec3d2e1b, __VMLINUX_SYMBOL_STR(trace_hardirqs_off) },
	{ 0x45f4466b, __VMLINUX_SYMBOL_STR(param_ops_uint) },
	{ 0x32f82cba, __VMLINUX_SYMBOL_STR(__class_create) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "EB16368AF1F414EF61E8199");
