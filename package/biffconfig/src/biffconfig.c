/*
 * (C) Copyright 2011 bifferos.com (sales@bifferos.com)
 *
 * For now, a placeholder to just read the values, should be easy to set them.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <mtd/mtd-user.h>
#include "md5.h"


typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;



// Some structures to help access the data.

// The entire 8k config block
// it starts at offset 0x4000 in mtd0, regardless of the board in use.

struct _cfg_vals
{
  int version;       // one for first version
  u8 bootsource;   // 0=flash, 1=MMC 2=NET 3=USB  (0)
  u8 console; // 0 = no console output, 1= console output (1)
  u8 nic; // 0 = no nic, 1= nic init, 2=promiscuous  (1)
  u8 boottype;  // 3 == Coreboot payload, 2 == Multiboot, 1 == linux, 0 == flat bin
  u32 loadaddress;  // load address of payload (0x400000)
  char cmndline[1024];  // null term, 1023 chars max
  unsigned short kernelmax;  // counted in sectors
}  __attribute__((__packed__));


struct _cfg
{
  struct _cfg_vals val;
  char padding[0x2000 - sizeof(struct _cfg_vals) - 0x10];
  md5_byte_t digest[0x10];
};


                      
struct _cfg g_vals;


// read the config
void config_init()
{
  int fd = open("/dev/mtd0", O_RDONLY);
  lseek(fd, 0x4000, SEEK_SET);
  read(fd, &g_vals, sizeof(struct _cfg));
}


void config_print()
{
  printf("Current configuration:\n");
  printf("  bootsource: %d\n", g_vals.val.bootsource);
  printf("  console: %d\n", g_vals.val.console);
  printf("  nic: %d\n", g_vals.val.nic);
  printf("  boottype: %d\n", g_vals.val.boottype);
  printf("  cmndline: %s\n", g_vals.val.cmndline);
  printf("  kernelmax: %d\n", g_vals.val.kernelmax);
}


#define DEVICE "/dev/mtd0"


int main(int argc, char *argv[])
{
	config_init();
	if (argc == 1)  // no arguments, just print the values
	{
		
		config_print();
	}
	else
	{
	   printf("Setting values unsupported in this version\n");
	}

	return 0;
}

