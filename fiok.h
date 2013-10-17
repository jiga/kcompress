/* 
 * fiok.h - File i/o in kernel
 * Authors:
 * 	Jignesh Kumar Patel <Jignesh.Patel@umkc.edu>
 * 	Mitulkumar Patel    <mvpt33@umkc.edu>
 * 	Harshil Parikh      <happ67@umkc.edu>
 */


#ifndef __FIOK_H
	#define __FIOK_H
#endif

/* 
 * Read "len" bytes from "filename" into "buf".
 * "buf" is in kernel space.
 */

int
read_file(struct file* filep, void *buf, int len);


/* Writing "len" bytes from "buf" to "filename" "buf" is in kernel space. */

int
write_file(struct file* filp, void *buf, int len);


