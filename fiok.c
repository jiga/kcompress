/* 
 * fiok.c - File i/o in kernel
 * Authors:
 * 	Jignesh Kumar Patel <Jignesh.Patel@umkc.edu>
 * 	Mitulkumar Patel    <mvpt33@umkc.edu>
 * 	Harshil Parikh      <happ67@umkc.edu>
 */

#include "kcompress.h"

/* 
 * Read "len" bytes from "filename" into "buf".
 * "buf" is in kernel space.
 */

int
read_file(struct file* filep, void *buf, int len)
{
 
    mm_segment_t oldfs; /* explained below in comment 2 */
    int bytes;    

    /* now read len bytes from offset 0 */
 //   filep->f_pos = 0;/* start offset */
   /* note that f_pos is automatically updated as you read the file */
    
	oldfs = get_fs();
	set_fs(KERNEL_DS);
        bytes = filep->f_op->read(filep, buf, len, &filep->f_pos); 
	
        set_fs(oldfs);

    return bytes;
}



/* Writing "len" bytes from "buf" to "filename" "buf" is in kernel space. */

int
write_file(struct file* filep, void *buf, int len)
{
    mm_segment_t oldfs;  /* see explanation in reading.txt */
    int bytes;

    /* now write len bytes from offset 0 */
   // filep->f_pos = 0;		
    /* start offset, note that this offset is 
			           automatically updated when you read or
write from the file */
    oldfs = get_fs();  /* read explanation in reading.txt */
    set_fs(KERNEL_DS); /* read explanation in writing.txt */
    bytes = filep->f_op->write(filep, buf, len, &filep->f_pos); 
    set_fs(oldfs);

    return bytes;
}




