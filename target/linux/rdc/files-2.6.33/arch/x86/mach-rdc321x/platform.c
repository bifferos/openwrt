//  Bifferboard RDC321x platform devices
//  Copyright (C) 2010 bifferos@yahoo.co.uk

#include <linux/mtd/map.h>
#include <linux/root_dev.h>
#include <linux/init.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <asm/io.h>



static int __init parse_bifferboard_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long plat_data)
{
	int res;
	size_t len;
	struct mtd_partition *rdc_flash_parts;
	u32 kernel_len;
	u16 tmp;

	res =  master->read(master, 0x4000 + 1036, 2, &len, (char *) &tmp);
	if (res)
		return res;
	kernel_len = tmp * master->erasesize;

	rdc_flash_parts = kzalloc(sizeof(struct mtd_partition) * 3, GFP_KERNEL);

	*pparts = rdc_flash_parts;

	rdc_flash_parts[0].name = "kernel";
	rdc_flash_parts[0].offset = 0;
	rdc_flash_parts[0].size = kernel_len;
	rdc_flash_parts[1].name = "rootfs";
	rdc_flash_parts[1].offset = MTDPART_OFS_APPEND;
	rdc_flash_parts[1].size = master->size - kernel_len - 0x10000;
	rdc_flash_parts[2].name = "biffboot";
	rdc_flash_parts[2].offset = MTDPART_OFS_APPEND;
	rdc_flash_parts[2].size = 0x10000;

	return 3;
}

struct mtd_part_parser __initdata bifferboard_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_bifferboard_partitions,
	.name = "Bifferboard",
};

static int __init bifferboard_setup(void)
{
	return register_mtd_parser(&bifferboard_parser);
}
arch_initcall(bifferboard_setup);





const char *__initdata boards[] = {
	"Bifferboard",
	0
};

static struct map_info rdc_map_info = {
	.name		= "rdc_flash",
	.size		= 0x800000,	/* 8MB */
	.phys		= 0xFF800000,	/* (u32) -rdc_map_info.size */
	.bankwidth	= 2,
};

static int __init rdc_board_setup(void)
{
	struct mtd_partition *partitions;
	int count, res;
	struct mtd_info *mtdinfo;

	rdc_map_info.virt = ioremap(rdc_map_info.phys, rdc_map_info.size);
	mtdinfo = do_map_probe("cfi_probe", &rdc_map_info);
	count = parse_mtd_partitions(mtdinfo, boards, &partitions, (u32)0);
	ROOT_DEV = 0;
	res = add_mtd_partitions(mtdinfo, partitions, count);
	if (res) return res;
	return 0;
}
late_initcall(rdc_board_setup);
