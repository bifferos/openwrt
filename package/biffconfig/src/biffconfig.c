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


// get the md5, write it to the given buffer.
void config_get_md5(md5_byte_t* digest)
{
  md5_state_t pms;
  
  md5_init(&pms);
  md5_append(&pms, (char*)&g_vals, 0x1ff0);
  md5_finish(&pms, digest);  
}


void print_md5(md5_byte_t* d)
{
  int i;
  for (i=0;i<16;i++)
  {
    printf("%02x", d[i]);
  }
}


// read the config
void config_init()
{
  md5_byte_t digest[16];
  int found_term, i;
  int fd = open("/dev/mtd0", O_RDONLY);
  lseek(fd, 0x4000, SEEK_SET);
  read(fd, &g_vals, sizeof(struct _cfg));

  config_get_md5(digest);

  if (memcmp(g_vals.digest, digest, sizeof(digest)) !=0)
  {
    printf("WARNING: Configuration md5 mismatch, configuration may have been corrupted\n");
    printf("Expected: ");  print_md5(digest);  printf("\n");
    printf("Actual:   ");    print_md5(g_vals.digest);  printf("\n");
  }
  
  found_term = 0;
  for (i=0;i<sizeof(g_vals.val.cmndline);i++)
  {
    if (!g_vals.val.cmndline[i]) 
    {
      found_term = 1;
      break;
    }
  }
  if (!found_term)
  {
    printf("Kernel command-line lacks termination, configuration may have been corrupted\n");
    // add some termination
    g_vals.val.cmndline[sizeof(g_vals.val.cmndline)-1] = 0;
  }
}


typedef struct _cfg_param
{
  char* name;
  int index;
  char* help;
} cfg_param_t;

#define CFG_BOOTSOURCE 0
#define CFG_CONSOLE 1
#define CFG_NIC 2
#define CFG_BOOTTYPE 3
#define CFG_LOADADDRESS 4
#define CFG_CMNDLINE 5
#define CFG_KERNELMAX 6


cfg_param_t g_params[] = {
 { "bootsource", CFG_BOOTSOURCE, "flash|MMC|Net" },
 { "console", CFG_CONSOLE, "disabled|enabled" },
 { "nic", CFG_NIC, "disabled|enabled|promiscuous" },
 { "boottype", CFG_BOOTTYPE, "flat binary|Linux|Multiboot|Coreboot" },
 { "loadaddress", CFG_LOADADDRESS, "<RAM address>" },
 { "cmndline", CFG_CMNDLINE, "<string>" },
 { "kernelmax", CFG_KERNELMAX, "<count of 64k sectors>" },
 { NULL },
};


// Print out one of the config values in human-readable form
char* render(int index)
{
  static char buffer[sizeof(g_vals.val.cmndline)];
  switch (index)
  {
  	case CFG_BOOTSOURCE:
  	  switch (g_vals.val.bootsource)
  	  {
	     case 0: return "flash";
	     case 1: return "MMC";
	     case 2: return "Net";
	     default:
	       return "Invalid";
          }
        case CFG_CONSOLE:
  	  switch (g_vals.val.console)
  	  {
	     case 0: return "disabled";
	     case 1: return "enabled";
	     default:
	       return "Invalid";
          }
          break;
        case CFG_NIC:
  	  switch (g_vals.val.nic)
  	  {
	     case 0: return "disabled";
	     case 1: return "enabled";
	     case 2: return "promiscuous";
	     default:
	       return "Invalid";
          }
          break;
        case CFG_BOOTTYPE:
  	  switch (g_vals.val.boottype)
  	  {
	     case 0: return "flat binary";
	     case 1: return "Linux";
	     case 2: return "Multiboot";
	     case 3: return "Coreboot";
	     default:
	       return "Invalid";
          }
        case CFG_LOADADDRESS:
          sprintf(buffer, "0x%x", g_vals.val.loadaddress);
          break;
        case CFG_CMNDLINE:
          sprintf(buffer, "%s", g_vals.val.cmndline);
          break;
        case CFG_KERNELMAX:
          sprintf(buffer, "0x%x", g_vals.val.kernelmax);
          break;
        default:
          strcpy(buffer, "<INVALID>");          
  }
  return buffer;  
}


// Print all the config values
void config_print()
{
  cfg_param_t* param;
  printf("Current configuration:\n");
  
  param = g_params;
  while (param->name)
  {
    printf("  %s: %s\n", param->name, render(param->index));
    param++;
  }
}


// Print one config values
int config_print_one(char* param)
{
  cfg_param_t* p;
  int found;
  
  p = g_params;
  found = 0;
  while (p->name)
  {
    if (strcmp(p->name, param)==0)
    {
      printf("%s\n", render(p->index));
      found = 1;
      break;
    }
    p++;
  }
  return found;
}


// Print one config values
int config_set_one(char* param, char* value)
{
  cfg_param_t* p;
  int found;
  
  p = g_params;
  found = 0;
  while (p->name)
  {
    if (strcmp(p->name, param)==0)
    {
      // set the value p->index.
      printf("TODO: Setting values is unsupported\n");
      found = 1;
      break;
    }
    p++;
  }
  return found;
}


#define DEVICE "/dev/mtd0"


void help()
{
  printf("Usage: biffconfig [set|get] <parameter> [value]\n");
  _exit(-1);
}


int main(int argc, char *argv[])
{
  int ok;
	config_init();
	switch (argc)
	{
	  case 1:
		config_print();
		break;
	  case 2:
	  	help();
	  case 3:
	  	if (strcmp(argv[1],"get")==0)
	  	{
		  ok = config_print_one(argv[2]);
		  if (!ok)
		  {
		    printf("Unrecognised option '%s'\n", argv[2]);
		  }
	  	}
	  	else
	  	{
	  	  help(); // need three parameters for a set operation
	  	}
	  	break;
	  case 4:
	  	if (strcmp(argv[1],"set")==0)
	  	{
		  ok = config_set_one(argv[2], argv[3]);
		  if (!ok)
		  {
		    printf("Unrecognised option '%s'\n", argv[2]);
		  }
	  	}
	  	else
	  	{
	  	  help(); // get needs one less parameter.
	  	}	  
	        break;
	  default:
	    help();
	}
	return 0;
}

