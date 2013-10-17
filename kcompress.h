/* 
 * kcompress.h - Loadable Kernel Module for adding file compressing systemcall
 * Authors:
 * 	Jignesh Kumar Patel <Jignesh.Patel@umkc.edu>
 * 	Mitulkumar Patel    <mvpt33@umkc.edu>
 * 	Harshil Parikh      <happ67@umkc.edu>
 */

#ifndef __KCOMPRESS__H__
	#define __KCOMPRESS__H__

/* Header files */
#include <linux/kernel.h>	/* printk and related Warning level macros */
#include <linux/module.h>	/* dynamic loading of modules into the kernel */
#include <sys/syscall.h>	/* for __NR_* macros */
#include <linux/sys.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/errno.h>
#include <linux/zlib.h>
#include "lzo/minilzo.h"
#include "fiok.h"		/* functions for file io inside kernel */
#define __NR_sys_kcompress 252

/* flags for compression / decompression */
#define ZLIB_C 1
#define ZLIB_D 2
#define LZO_C 3
#define LZO_D 4

/* options for level of compression */
#define L_LOW 1
#define L_MED 2
#define L_HI  3

#define CHUNK 4096
#endif

